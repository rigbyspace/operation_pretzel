#!/usr/bin/env python3
"""Interpret TRTS simulation summaries into narrative commentary."""

import json
import os
import re
import sys
import textwrap
from statistics import mean


def load_summaries(paths):
    """Return a list of dictionaries with run data loaded from JSON files."""
    entries = []
    search_paths = list(paths) if paths else ['.']

    for path in search_paths:
        if os.path.isdir(path):
            for root, _dirs, files in os.walk(path):
                for filename in files:
                    if filename == 'output_summary.json':
                        file_path = os.path.join(root, filename)
                        entry = _load_single_summary(file_path)
                        if entry:
                            entries.append(entry)
        else:
            entry = _load_single_summary(path)
            if entry:
                entries.append(entry)

    entries.sort(key=lambda entry: (entry.get('run') or entry['data'].get('run') or entry['path'], entry['path']))
    return entries


def _load_single_summary(path):
    try:
        with open(path, 'r', encoding='utf-8') as handle:
            data = json.load(handle)
    except FileNotFoundError:
        sys.stderr.write(f"Warning: file not found: {path}\n")
        return None
    except json.JSONDecodeError as exc:
        sys.stderr.write(f"Warning: could not parse JSON in {path}: {exc}\n")
        return None

    run_name = data.get('run') or os.path.basename(os.path.dirname(path)) or path
    return {'path': path, 'run': run_name, 'data': data}


def interpret_single_run(entry):
    data = entry['data']
    run_name = data.get('run') or entry.get('run') or 'Unknown run'
    components = _parse_run_components(run_name)

    score = data.get('stability_score')
    classification = data.get('classification', 'Unclassified')
    psi_events = data.get('psi_events', 0)
    rho_events = data.get('rho_events', 0)
    mu_zero = data.get('mu_zero_events', 0)
    band_transitions = data.get('band_transitions')
    ratio_hits = data.get('ratio_band_hits') or {}
    psi_avg = data.get('psi_avg_spacing')
    psi_std = data.get('psi_stddev')

    sentences = []

    descriptor = _score_descriptor(score)
    if score is not None:
        sentences.append(
            f"This configuration registers as {classification} with {descriptor} (score: {score:.0f})."
        )
    else:
        sentences.append(
            f"This configuration registers as {classification}, but the stability score was not reported."
        )

    ratio_sentence = _describe_ratio_affinity(ratio_hits, band_transitions, components)
    if ratio_sentence:
        sentences.append(ratio_sentence)

    rhythm_sentence = _describe_rhythm(psi_events, rho_events, psi_avg, psi_std)
    if rhythm_sentence:
        sentences.append(rhythm_sentence)

    if mu_zero:
        sentences.append(
            _pluralize(
                mu_zero,
                "μ = 0 excursion surfaced {n} time, hinting at temporary stasis.",
                "μ = 0 excursions surfaced {n} times, hinting at temporary stasis.",
            )
        )

    anomaly = _detect_anomalies(data, ratio_hits)
    if anomaly:
        sentences.append(anomaly)

    suggestion = _suggest_next_steps(components, ratio_hits, psi_events, score)
    if suggestion:
        sentences.append(suggestion)

    paragraph = ' '.join(sentences)
    wrapped = textwrap.fill(paragraph, width=88)
    return f"▶ Run: {run_name}\n{wrapped}"


def summarize_multiple_runs(entries):
    if len(entries) < 2:
        return ''

    scores = []
    classifications = {}
    ratio_stats = {}
    tight_rhythm_by_koppa = {}
    tight_rhythm_totals = {}

    for entry in entries:
        data = entry['data']
        run_name = data.get('run') or entry.get('run') or 'Unknown run'
        components = _parse_run_components(run_name)
        engine = components.get('engine')
        koppa = components.get('koppa')

        score = data.get('stability_score')
        if isinstance(score, (int, float)):
            scores.append(score)

        classification = data.get('classification', 'Unclassified')
        classifications[classification] = classifications.get(classification, 0) + 1

        ratio_hits = data.get('ratio_band_hits') or {}
        for ratio_name, value in ratio_hits.items():
            if not isinstance(value, (int, float)):
                continue
            if value >= 0.5:
                stats = ratio_stats.setdefault(ratio_name, {'count': 0, 'engines': set()})
                stats['count'] += 1
                if engine:
                    stats['engines'].add(engine)

        psi_std = data.get('psi_stddev')
        if isinstance(psi_std, (int, float)) and psi_std < 2.0:
            if koppa:
                tight_rhythm_by_koppa[koppa] = tight_rhythm_by_koppa.get(koppa, 0) + 1
            if koppa:
                tight_rhythm_totals[koppa] = tight_rhythm_totals.get(koppa, 0) + 1
        elif koppa:
            tight_rhythm_totals[koppa] = tight_rhythm_totals.get(koppa, 0) + 1

    lines = ["Comparative notes:"]

    if scores:
        lines.append(
            f"- Average stability score across {len(entries)} runs: {mean(scores):.1f} "
            f"(range {min(scores):.0f}–{max(scores):.0f})."
        )

    if classifications:
        class_fragments = [
            f"{name} ({count})" for name, count in sorted(classifications.items(), key=lambda x: (-x[1], x[0]))
        ]
        lines.append("- Classification split: " + ', '.join(class_fragments) + '.')

    for ratio_name, stats in sorted(ratio_stats.items(), key=lambda item: (-item[1]['count'], item[0])):
        if stats['count'] < 2:
            continue
        ratio_label = _format_ratio_name(ratio_name)
        engines = sorted(stats['engines'])
        if engines:
            if len(engines) == 1:
                line = (
                    f"- {ratio_label}-band dominance appeared in {stats['count']}/{len(entries)} runs, "
                    f"always with engine={engines[0]}."
                )
                lines.append(line)
            else:
                engine_list = ', '.join(engines)
                lines.append(
                    f"- {ratio_label}-band dominance surfaced {stats['count']} times across mixed engines ({engine_list})."
                )
        else:
            lines.append(
                f"- {ratio_label}-band dominance appeared in {stats['count']} runs."
            )

    tight_fragments = []
    for koppa, count in tight_rhythm_by_koppa.items():
        total = tight_rhythm_totals.get(koppa, 0)
        if total:
            tight_fragments.append(f"koppa={koppa} {count}/{total}")
    if tight_fragments:
        joined = ', '.join(tight_fragments)
        lines.append(f"- ψ rhythm stayed tight (σ < 2) only in {joined}.")

    if len(lines) <= 1:
        return ''

    return '\n'.join(lines)


def _parse_run_components(run_name):
    components = {}
    parts = run_name.split('__')
    pattern = re.compile(r'^(?P<key>[a-z]+)_(?P<value>.+)$')
    for part in parts:
        match = pattern.match(part)
        if match:
            key = match.group('key')
            value = match.group('value')
            components[key] = value
    return components


def _score_descriptor(score):
    if not isinstance(score, (int, float)):
        return 'an indeterminate stability profile'
    if score >= 80:
        return 'high structural confidence'
    if score >= 50:
        return 'measured coherence'
    if score >= 30:
        return 'fragile coordination'
    return 'low activity'


def _describe_ratio_affinity(ratio_hits, band_transitions, components):
    if not ratio_hits:
        return 'No ratio bands registered, leaving the structure largely unanchored to known constants.'

    sorted_ratios = sorted(
        ((name, value) for name, value in ratio_hits.items() if isinstance(value, (int, float))),
        key=lambda item: item[1],
        reverse=True,
    )
    if not sorted_ratios:
        return ''

    top_name, top_value = sorted_ratios[0]
    ratio_label = _format_ratio_name(top_name)

    if top_value >= 0.7:
        affinity_phrase = f"Strong {ratio_label}-band affinity ({top_value:.0%})"
    elif top_value >= 0.4:
        affinity_phrase = f"Noticeable {ratio_label}-band residence ({top_value:.0%})"
    else:
        affinity_phrase = f"Only a faint pull toward the {ratio_label} band ({top_value:.0%})"

    transitions_phrase = ''
    if isinstance(band_transitions, int):
        if band_transitions == 0:
            transitions_phrase = ' with no band transitions recorded.'
        elif band_transitions < 5:
            transitions_phrase = f" across {band_transitions} measured transitions."
        else:
            transitions_phrase = f", despite {band_transitions} restless transitions."

    engine = components.get('engine')
    if engine and top_value >= 0.4:
        affinity_phrase += f" while engine={engine} kept the ratios grounded"

    result = affinity_phrase + transitions_phrase
    if not result.endswith('.'):
        result += '.'
    return result


def _describe_rhythm(psi_events, rho_events, psi_avg, psi_std):
    if not psi_events and not rho_events:
        return 'ψ and ρ barely activated, leaving the run effectively inert.'

    parts = []
    if psi_events or rho_events:
        if rho_events:
            ratio = psi_events / rho_events if rho_events else psi_events
            parts.append(
                f"ψ fired {psi_events} times against {rho_events} ρ triggers (ψ/ρ ≈ {ratio:.2f})."
            )
        else:
            parts.append(f"ψ fired {psi_events} times without any ρ triggers to pace it.")

    if isinstance(psi_avg, (int, float)) and isinstance(psi_std, (int, float)) and psi_events:
        if psi_std < 1:
            parts.append(
                f"Rhythm stayed tight (avg spacing {psi_avg:.1f} MTs, σ {psi_std:.1f})."
            )
        elif psi_std < 3:
            parts.append(
                f"Rhythm was balanced (avg {psi_avg:.1f} MTs, σ {psi_std:.1f})."
            )
        else:
            parts.append(
                f"Rhythm wandered (avg {psi_avg:.1f} MTs, σ {psi_std:.1f})."
            )
    elif psi_events:
        parts.append('ψ rhythm data was incomplete, limiting timing analysis.')

    return ' '.join(parts)


def _detect_anomalies(data, ratio_hits):
    classification = data.get('classification', '')
    score = data.get('stability_score')
    psi_events = data.get('psi_events', 0)

    if classification.lower().startswith('null') and score and score > 40:
        return 'Classification skews null despite a middling score—worth double-checking the analyzer thresholds.'

    if psi_events < 3 and ratio_hits and max(ratio_hits.values()) > 0.6:
        return 'Strong ratio locking with minimal ψ activity hints at an over-constrained trigger scheme.'

    if score is not None and score > 90 and data.get('psi_stddev', 0) > 4:
        return 'Exceptional score paired with noisy ψ rhythm—possible mislabel or emergent chaos.'

    if score is not None and score < 20 and data.get('psi_events', 0) > 20:
        return 'Heavy ψ traffic but low stability suggests destructive interference worth investigating.'

    return ''


def _suggest_next_steps(components, ratio_hits, psi_events, score):
    suggestions = []
    psi_mode = components.get('psi')
    engine = components.get('engine')
    koppa = components.get('koppa')

    if psi_mode == 'mstep':
        suggestions.append('Consider toggling ψ-mode toward inhibit_rho or dual_track to probe control sensitivity.')
    elif psi_mode == 'rho_only':
        suggestions.append('Try enabling a mixed ψ-mode to wake dormant channels.')

    if engine == 'add' and ratio_hits and max(ratio_hits.values()) >= 0.6:
        suggestions.append('Introduce a contrast run with engine=multi to test whether the ratio lock persists.')
    elif engine == 'slide' and (not psi_events or psi_events < 5):
        suggestions.append('Engine=slide seems starved; push β or κ or switch to add for a livelier baseline.')

    if koppa == 'pop' and (not score or score < 40):
        suggestions.append('Koppa=pop may be venting structure—try accumulate or all_mu gating.')

    if not suggestions:
        return 'Follow-up with a small parameter nudge—ratio thresholds or MT10 forcing—to map the attractor basin.'

    return ' '.join(suggestions)


def _format_ratio_name(name):
    mapping = {'phi': 'φ', 'sqrt2': '√2', 'plastic': 'plastic'}
    return mapping.get(name, name)


def _pluralize(count, singular, plural):
    template = singular if count == 1 else plural
    return template.format(n=count)


def write_report(report_text, path='narrative_report.txt'):
    try:
        with open(path, 'w', encoding='utf-8') as handle:
            handle.write(report_text)
    except OSError as exc:
        sys.stderr.write(f"Warning: could not write report file {path}: {exc}\n")


def main(argv=None):
    argv = sys.argv[1:] if argv is None else argv
    entries = load_summaries(argv)

    if not entries:
        sys.stderr.write('No output_summary.json files were found.\n')
        return 1

    narratives = [interpret_single_run(entry) for entry in entries]

    comparison = summarize_multiple_runs(entries)
    if comparison:
        narratives.append('')
        narratives.append(textwrap.fill(comparison, width=88))

    report_text = '\n\n'.join(narratives)
    print(report_text)

    write_report(report_text)
    return 0


if __name__ == '__main__':
    sys.exit(main())
