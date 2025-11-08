#!/usr/bin/env python3
"""Cross-run analysis for TRTS simulation summaries."""

import argparse
import json
import math
import os
from collections import Counter, defaultdict
from dataclasses import dataclass
from itertools import combinations
from statistics import mean
from typing import Dict, Iterable, List, Optional, Tuple


@dataclass
class RunSummary:
    path: str
    run_name: str
    data: Dict
    config: Dict[str, str]

    @property
    def stability(self) -> Optional[float]:
        value = self.data.get("stability_score")
        return float(value) if value is not None else None

    @property
    def classification(self) -> str:
        return self.data.get("classification", "Unclassified")


def load_all_summaries(path: str) -> List[RunSummary]:
    """Load every ``output_summary.json`` file below ``path``."""
    runs: List[RunSummary] = []

    if os.path.isfile(path):
        try:
            with open(path, "r", encoding="utf-8") as handle:
                payload = json.load(handle)
        except (OSError, json.JSONDecodeError) as exc:  # pragma: no cover - defensive
            print(f"Warning: could not read {path}: {exc}")
            return runs

        run_name = payload.get("run") or os.path.splitext(os.path.basename(path))[0]
        config = parse_run_config(run_name)
        runs.append(RunSummary(path, run_name, payload, config))
        return runs

    for root, _dirs, files in os.walk(path):
        for filename in files:
            if filename != "output_summary.json":
                continue
            file_path = os.path.join(root, filename)
            try:
                with open(file_path, "r", encoding="utf-8") as handle:
                    payload = json.load(handle)
            except (OSError, json.JSONDecodeError) as exc:  # pragma: no cover - defensive
                print(f"Warning: could not read {file_path}: {exc}")
                continue

            run_name = payload.get("run") or os.path.splitext(filename)[0]
            config = parse_run_config(run_name)
            runs.append(RunSummary(file_path, run_name, payload, config))

    runs.sort(key=lambda item: item.run_name)
    return runs


def parse_run_config(run_name: str) -> Dict[str, str]:
    """Extract configuration parameters from a run identifier."""
    config: Dict[str, str] = {}
    for component in run_name.split("__"):
        if "_" not in component:
            continue
        key, value = component.split("_", 1)
        config[key] = value
    return config


def analyze_parameter_effects(runs: Iterable[RunSummary]):
    """Aggregate per-parameter statistics across runs."""
    stats: Dict[str, Dict[str, Dict[str, object]]] = defaultdict(
        lambda: defaultdict(
            lambda: {
                "count": 0,
                "stability_sum": 0.0,
                "stability_count": 0,
                "classifications": Counter(),
                "phi_values": [],
            }
        )
    )

    for run in runs:
        data = run.data
        phi_ratio = None
        if isinstance(data.get("ratio_band_hits"), dict):
            phi_ratio = data["ratio_band_hits"].get("phi")

        for key, value in run.config.items():
            entry = stats[key][value]
            entry["count"] += 1
            stability = run.stability
            if stability is not None:
                entry["stability_sum"] += stability
                entry["stability_count"] += 1
            entry["classifications"][run.classification] += 1
            if phi_ratio is not None:
                entry["phi_values"].append(phi_ratio)

    parameter_table = {}
    for key, values in stats.items():
        parameter_table[key] = {}
        for value, payload in values.items():
            avg_stability = (
                payload["stability_sum"] / payload["stability_count"]
                if payload["stability_count"]
                else None
            )
            avg_phi = mean(payload["phi_values"]) if payload["phi_values"] else None
            parameter_table[key][value] = {
                "runs": payload["count"],
                "avg_stability": avg_stability,
                "classifications": dict(payload["classifications"]),
                "avg_phi": avg_phi,
            }

    return parameter_table


def detect_clusters(runs: Iterable[RunSummary]):
    """Identify runs belonging to specific behavioral clusters."""
    clusters = {
        "phi_dominant": [],
        "high_psi_stddev": [],
        "high_mu_zero": [],
        "null_propagation": [],
    }

    for run in runs:
        data = run.data
        phi = None
        ratio_hits = data.get("ratio_band_hits")
        if isinstance(ratio_hits, dict):
            phi = ratio_hits.get("phi")

        if phi is not None and phi >= 0.5:
            clusters["phi_dominant"].append((phi, run))

        psi_stddev = data.get("psi_stddev")
        if psi_stddev is not None and psi_stddev > 5:
            clusters["high_psi_stddev"].append((psi_stddev, run))

        mu_zero = data.get("mu_zero_events", 0)
        if mu_zero >= 3:
            clusters["high_mu_zero"].append((mu_zero, run))

        stability = run.stability
        if stability is not None and stability < 20:
            clusters["null_propagation"].append((stability, run))

    for key in clusters:
        reverse = key not in {"null_propagation"}
        clusters[key].sort(key=lambda item: item[0], reverse=reverse)
        clusters[key] = [run for _score, run in clusters[key]]

    return clusters


def detect_redundancy(runs: Iterable[RunSummary]):
    """Find runs with nearly identical behavioral fingerprints."""
    redundant_pairs: List[Tuple[RunSummary, RunSummary]] = []
    runs_by_class = defaultdict(list)

    for run in runs:
        runs_by_class[run.classification].append(run)

    for classification, class_runs in runs_by_class.items():
        sorted_runs = sorted(class_runs, key=lambda r: (r.stability or math.nan, r.run_name))
        for i, current in enumerate(sorted_runs):
            for other in sorted_runs[i + 1 :]:
                if _is_equivalent(current, other):
                    redundant_pairs.append((current, other))
                else:
                    # sorted by stability; break when score diverges
                    if _score_difference(current, other) > 1.0:
                        break
    return redundant_pairs


def _is_equivalent(a: RunSummary, b: RunSummary) -> bool:
    if a.classification != b.classification:
        return False

    if a.stability is None or b.stability is None:
        return False

    if abs(a.stability - b.stability) > 1.0:
        return False

    data_a = a.data
    data_b = b.data
    for key in ("psi_events", "rho_events", "band_transitions"):
        if key in data_a and key in data_b and abs(data_a[key] - data_b[key]) > 3:
            return False

    phi_a = (data_a.get("ratio_band_hits") or {}).get("phi")
    phi_b = (data_b.get("ratio_band_hits") or {}).get("phi")
    if phi_a is not None and phi_b is not None and abs(phi_a - phi_b) > 0.05:
        return False

    psi_std_a = data_a.get("psi_stddev")
    psi_std_b = data_b.get("psi_stddev")
    if psi_std_a is not None and psi_std_b is not None and abs(psi_std_a - psi_std_b) > 0.5:
        return False

    return True


def _score_difference(a: RunSummary, b: RunSummary) -> float:
    if a.stability is None or b.stability is None:
        return float("inf")
    return abs(a.stability - b.stability)


def detect_co_dependencies(runs: Iterable[RunSummary]):
    """Highlight parameter pairs that always accompany a classification."""
    by_class = defaultdict(list)
    for run in runs:
        by_class[run.classification].append(run)

    dependencies = []
    for classification, class_runs in by_class.items():
        if not class_runs:
            continue
        shared_values: Dict[str, Optional[str]] = {}
        first_config = class_runs[0].config
        for key in first_config:
            shared_values[key] = first_config[key]

        for run in class_runs[1:]:
            for key in list(shared_values):
                if shared_values[key] is None:
                    continue
                if run.config.get(key) != shared_values[key]:
                    shared_values[key] = None

        invariant_keys = [k for k, v in shared_values.items() if v is not None]
        if len(invariant_keys) < 2:
            continue

        for key_a, key_b in combinations(sorted(invariant_keys), 2):
            dependencies.append(
                {
                    "classification": classification,
                    "parameters": {
                        key_a: shared_values[key_a],
                        key_b: shared_values[key_b],
                    },
                    "occurrences": len(class_runs),
                }
            )

    return dependencies


def detect_test_gaps(runs: Iterable[RunSummary]):
    """Infer plausible but untested parameter combinations."""
    parameter_values: Dict[str, set] = defaultdict(set)
    seen_pairs: Dict[Tuple[str, str], set] = defaultdict(set)

    for run in runs:
        config = run.config
        for key, value in config.items():
            parameter_values[key].add(value)
        for key_a, key_b in combinations(sorted(config), 2):
            seen_pairs[(key_a, key_b)].add((config[key_a], config[key_b]))

    suggestions = []
    for (key_a, key_b), observed in seen_pairs.items():
        all_values = {
            key_a: parameter_values[key_a],
            key_b: parameter_values[key_b],
        }
        missing = [
            (value_a, value_b)
            for value_a in all_values[key_a]
            for value_b in all_values[key_b]
            if (value_a, value_b) not in observed
        ]
        for value_a, value_b in missing:
            suggestions.append((key_a, value_a, key_b, value_b))

    # Deduplicate near-duplicates by text key and cap length.
    unique_statements = []
    seen_text = set()
    for key_a, value_a, key_b, value_b in suggestions:
        statement = f"{key_a}={value_a} with {key_b}={value_b}"
        if statement not in seen_text:
            seen_text.add(statement)
            unique_statements.append(statement)

    unique_statements.sort()
    return unique_statements[:20]


def rank_runs(runs: Iterable[RunSummary]):
    runs = list(runs)
    top_by_score = [
        run
        for run in sorted(
            runs,
            key=lambda r: (r.stability if r.stability is not None else -float("inf")),
            reverse=True,
        )
        if run.stability is not None
    ][:10]

    most_chaotic = sorted(
        runs,
        key=lambda r: (
            r.data.get("psi_stddev")
            if r.data.get("psi_stddev") is not None
            else -float("inf")
        ),
        reverse=True,
    )[:10]

    phi_rich = [
        run
        for run in runs
        if (run.data.get("ratio_band_hits") or {}).get("phi", 0) >= 0.4
    ]
    phi_rich.sort(key=lambda r: (r.stability or -float("inf")), reverse=True)

    return top_by_score, most_chaotic, phi_rich[:10]


def build_summary_text(
    runs: List[RunSummary],
    parameter_table,
    clusters,
    redundancy,
    co_dependencies,
    test_gaps,
    rankings,
) -> str:
    lines = []
    lines.append(f"Loaded {len(runs)} run summaries")
    lines.append("")

    if not runs:
        return "\n".join(lines)

    lines.append("🔢 Parameter Impact Table")
    for parameter, value_map in sorted(parameter_table.items()):
        lines.append(f"Parameter: {parameter}")
        for value, metrics in sorted(value_map.items()):
            avg_score = metrics["avg_stability"]
            score_text = f"{avg_score:.2f}" if avg_score is not None else "n/a"
            phi_text = (
                f"{metrics['avg_phi'] * 100:.1f}%"
                if metrics["avg_phi"] is not None
                else "n/a"
            )
            class_entries = metrics["classifications"]
            class_text = (
                ", ".join(
                    f"{cls}={count}" for cls, count in sorted(class_entries.items())
                )
                if class_entries
                else "None"
            )
            lines.append(
                f"  ▶ {parameter}={value} ({metrics['runs']} runs)\n"
                f"    Avg score: {score_text}\n"
                f"    Avg φ-band: {phi_text}\n"
                f"    Classifications: {class_text}"
            )
        lines.append("")

    lines.append("🧠 Behavioral Clusters")
    for label, run_list in (
        ("φ-dominant", clusters["phi_dominant"]),
        ("High ψ stddev", clusters["high_psi_stddev"]),
        ("High μ=0 counts", clusters["high_mu_zero"]),
        ("Null propagation", clusters["null_propagation"]),
    ):
        lines.append(f"▶ {label} ({len(run_list)} runs)")
        for run in run_list[:10]:
            phi = (run.data.get("ratio_band_hits") or {}).get("phi")
            psi_std = run.data.get("psi_stddev")
            mu_zero = run.data.get("mu_zero_events")
            stability = run.stability
            details = []
            if phi is not None:
                details.append(f"φ={phi:.2f}")
            if psi_std is not None:
                details.append(f"ψσ={psi_std:.2f}")
            if mu_zero:
                details.append(f"μ₀={mu_zero}")
            if stability is not None:
                details.append(f"score={stability:.1f}")
            lines.append(f"  - {run.run_name} ({', '.join(details)})")
        lines.append("")

    lines.append("⚠️ Redundancy Detector")
    if redundancy:
        for run_a, run_b in redundancy[:20]:
            diff = abs((run_a.stability or 0) - (run_b.stability or 0))
            lines.append(
                f"▶ {run_a.run_name} and {run_b.run_name} are behaviorally equivalent "
                f"(classification={run_a.classification}, Δscore={diff:.2f})"
            )
    else:
        lines.append("No redundant configurations detected.")
    lines.append("")

    lines.append("🧬 Co-dependency Detector")
    if co_dependencies:
        for item in co_dependencies:
            params = ", ".join(f"{k}={v}" for k, v in item["parameters"].items())
            lines.append(
                f"▶ {item['classification']} only seen with {params} (n={item['occurrences']})"
            )
    else:
        lines.append("No strict parameter co-dependencies detected.")
    lines.append("")

    lines.append("🧪 Test Gap Detector")
    if test_gaps:
        for statement in test_gaps:
            lines.append(f"▶ Unexplored: {statement}")
    else:
        lines.append("No missing parameter combinations inferred.")
    lines.append("")

    top_by_score, most_chaotic, phi_rich = rankings
    lines.append("🗺️ Global Summary")
    lines.append("▶ Top configs by stability score")
    for run in top_by_score:
        lines.append(
            f"  - {run.run_name} ({run.classification}, score={run.stability:.2f})"
        )
    lines.append("")

    lines.append("▶ Most chaotic by ψ stddev")
    for run in most_chaotic:
        psi_std = run.data.get("psi_stddev")
        lines.append(
            f"  - {run.run_name} ({run.classification}, ψσ={psi_std})"
        )
    lines.append("")

    lines.append("▶ Most stable φ-rich configs")
    if phi_rich:
        for run in phi_rich:
            phi = (run.data.get("ratio_band_hits") or {}).get("phi")
            lines.append(
                f"  - {run.run_name} (score={run.stability:.2f}, φ={phi:.2f})"
            )
    else:
        lines.append("  No φ-rich runs detected.")
    lines.append("")

    lines.append("▶ Most redundant configs")
    if redundancy:
        for run_a, run_b in redundancy[:5]:
            lines.append(f"  - {run_a.run_name} ≈ {run_b.run_name}")
    else:
        lines.append("  None")

    return "\n".join(lines)


def write_markdown_report(path: str, content: str) -> None:
    with open(path, "w", encoding="utf-8") as handle:
        handle.write("# TRTS Cross-Run Correlation Summary\n\n")
        for line in content.splitlines():
            if line.startswith("▶"):
                handle.write(f"## {line[2:].strip()}\n")
            elif line.startswith("  -"):
                handle.write(f"- {line[4:]}\n")
            elif line.startswith("  "):
                handle.write(f"- {line.strip()}\n")
            else:
                handle.write(f"{line}\n")


def main():
    parser = argparse.ArgumentParser(
        description="Correlate TRTS simulation outputs across runs.",
    )
    parser.add_argument(
        "path",
        nargs="?",
        default=".",
        help="Directory containing output_summary.json files",
    )
    parser.add_argument(
        "--markdown",
        dest="markdown",
        help="Optional path to write a markdown summary.",
    )
    args = parser.parse_args()

    runs = load_all_summaries(args.path)
    parameter_table = analyze_parameter_effects(runs)
    clusters = detect_clusters(runs)
    redundancy = detect_redundancy(runs)
    co_dependencies = detect_co_dependencies(runs)
    test_gaps = detect_test_gaps(runs)
    rankings = rank_runs(runs)

    content = build_summary_text(
        runs,
        parameter_table,
        clusters,
        redundancy,
        co_dependencies,
        test_gaps,
        rankings,
    )

    print(content)

    if args.markdown:
        write_markdown_report(args.markdown, content)
        print(f"\nMarkdown report written to {args.markdown}")


if __name__ == "__main__":
    main()
