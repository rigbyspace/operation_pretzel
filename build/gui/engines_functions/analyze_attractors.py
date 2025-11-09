"""TRTS ratio and event analyzer.

This script inspects the fictional Transformative Reciprocal Triadic Structure (TRTS)
engine output tables to highlight oscillatory curiosities.  It does not attempt to
confirm convergence toward known mathematical constants.  Instead, it catalogues how
often the observed ratios wander near reference irrational neighborhoods and how the
ψ rhythm behaves over time.

Only Python standard modules requested by the experiment are used.
"""

import csv
import json
import math
import os
import sys
from collections import namedtuple
from fractions import Fraction
from statistics import mean, stdev


ValueRecord = namedtuple(
    "ValueRecord",
    [
        "tick",
        "mt",
        "upsilon",
        "beta",
        "koppa",
        "memory",
        "phi",
        "ratio",
        "composite_index",
        "microtick_index",
    ],
)

EventRecord = namedtuple(
    "EventRecord",
    [
        "tick",
        "mt",
        "event_type",
        "rho_event",
        "psi_fired",
        "mu_zero",
        "forced_emission",
        "composite_index",
        "microtick_index",
    ],
)


def _fraction_from_row(row, num_key, den_key, default):
    """Read a fraction from CSV row, tolerating missing data."""

    num = row.get(num_key)
    den = row.get(den_key)
    try:
        if num is None or den is None:
            raise ValueError
        num_val = int(num)
        den_val = int(den)
        if den_val == 0:
            return default
        return Fraction(num_val, den_val)
    except (ValueError, ZeroDivisionError):
        return default


def load_configuration(argv):
    """Load optional configuration from JSON file or JSON string."""

    defaults = {
        "psi_mode": "unspecified",
        "engine_mode": "unspecified",
        "koppa_mode": "unspecified",
        "ratio_trigger": "unspecified",
    }

    if len(argv) < 2:
        fallback = os.path.join(os.path.dirname(__file__), "config.json")
        if os.path.exists(fallback):
            return _load_config_path(fallback, defaults)
        return defaults

    candidate = argv[1]
    if os.path.exists(candidate):
        return _load_config_path(candidate, defaults)

    try:
        parsed = json.loads(candidate)
    except json.JSONDecodeError:
        return defaults

    if isinstance(parsed, dict):
        merged = defaults.copy()
        for key, value in parsed.items():
            merged[key] = str(value)
        return merged
    return defaults


def _load_config_path(path, defaults):
    with open(path, "r", encoding="utf-8") as handle:
        loaded = json.load(handle)
    merged = defaults.copy()
    if isinstance(loaded, dict):
        for key, value in loaded.items():
            merged[key] = str(value)
    return merged


def load_values(path):
    """Load microtick values and compute auxiliary metrics."""

    records = []
    with open(path, "r", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            tick = int(row["tick"])
            mt = int(row["mt"])

            upsilon = _fraction_from_row(row, "upsilon_num", "upsilon_den", Fraction(0, 1))
            beta = _fraction_from_row(row, "beta_num", "beta_den", Fraction(1, 1))
            koppa = _fraction_from_row(row, "koppa_num", "koppa_den", Fraction(0, 1))
            memory = _fraction_from_row(row, "memory_num", "memory_den", Fraction(0, 1))
            phi = _fraction_from_row(row, "phi_num", "phi_den", Fraction(0, 1))

            ratio = float(upsilon) / float(beta) if beta != 0 else math.inf
            composite_index = tick + (mt / 100.0)
            microtick_index = tick * 11 + (mt - 1)

            records.append(
                ValueRecord(
                    tick,
                    mt,
                    upsilon,
                    beta,
                    koppa,
                    memory,
                    phi,
                    ratio,
                    composite_index,
                    microtick_index,
                )
            )
    return records


def load_events(path):
    """Load microtick events."""

    events = []
    with open(path, "r", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            tick = int(row["tick"])
            mt = int(row["mt"])
            composite_index = tick + (mt / 100.0)
            microtick_index = tick * 11 + (mt - 1)

            events.append(
                EventRecord(
                    tick,
                    mt,
                    row.get("event_type", ""),
                    int(row.get("rho_event", 0)),
                    int(row.get("psi_fired", 0)),
                    int(row.get("mu_zero", 0)),
                    int(row.get("forced_emission", 0)),
                    composite_index,
                    microtick_index,
                )
            )
    return events


def analyze_ratios(values):
    """Evaluate ratio neighborhoods for υ/β."""

    bands = {
        "phi": (1.618 - 0.02, 1.618 + 0.02),
        "sqrt2": (1.414 - 0.025, 1.414 + 0.025),
        "plastic": (1.324 - 0.02, 1.324 + 0.02),
    }

    counts = {name: 0 for name in bands}
    total = len(values)
    if total == 0:
        return {
            "counts": counts,
            "percentages": {name: 0.0 for name in bands},
            "transitions": 0,
            "longest_band": None,
            "longest_span": 0,
        }

    transitions = 0
    active_band = None
    streak = 0
    longest_band = None
    longest_span = 0

    for record in values:
        ratio = record.ratio
        hit = None
        for name, (low, high) in bands.items():
            if low <= ratio <= high:
                hit = name
                break

        if hit is not None:
            counts[hit] += 1

        if hit == active_band and hit is not None:
            streak += 1
        elif hit is not None:
            if active_band is not None and active_band != hit:
                transitions += 1
            active_band = hit
            streak = 1
        else:
            if active_band is not None:
                transitions += 1
            active_band = None
            streak = 0

        if hit is not None and streak > longest_span:
            longest_span = streak
            longest_band = active_band

    percentages = {name: (counts[name] / total) * 100.0 for name in bands}

    return {
        "counts": counts,
        "percentages": percentages,
        "transitions": transitions,
        "longest_band": longest_band,
        "longest_span": longest_span,
    }


def analyze_psi_rhythm(events):
    """Inspect ψ rhythm and aggregate event counts."""

    psi_indices = sorted(event.microtick_index for event in events if event.psi_fired == 1)
    rho_indices = sorted(event.microtick_index for event in events if event.rho_event == 1)
    mu_zero_count = sum(1 for event in events if event.mu_zero == 1)

    spacings = []
    for previous, current in zip(psi_indices, psi_indices[1:]):
        spacings.append(float(current - previous))

    average_spacing = mean(spacings) if spacings else None
    if len(spacings) >= 2:
        stddev_spacing = stdev(spacings)
    elif spacings:
        stddev_spacing = 0.0
    else:
        stddev_spacing = None

    if len(psi_indices) < 5:
        classification = "Sparse"
    else:
        effective = stddev_spacing if stddev_spacing is not None else float("inf")
        if effective < 1:
            classification = "Uniform"
        elif effective <= 5:
            classification = "Bursty"
        else:
            classification = "Chaotic"

    avg_delay = compute_average_delay(rho_indices, psi_indices)

    return (
        {
            "event_count": len(psi_indices),
            "average_spacing": average_spacing,
            "stddev_spacing": stddev_spacing,
            "classification": classification,
            "average_rho_to_psi_delay": avg_delay,
        },
        {
            "psi_events": len(psi_indices),
            "rho_events": len(rho_indices),
            "mu_zero_events": mu_zero_count,
        },
    )


def compute_average_delay(rho_indices, psi_indices):
    """Average microtick delay from ρ detection to the next ψ event."""

    if not rho_indices or not psi_indices:
        return None

    delays = []
    psi_iter = iter(psi_indices)
    try:
        current_psi = next(psi_iter)
    except StopIteration:
        return None

    for rho_index in rho_indices:
        while current_psi is not None and current_psi <= rho_index:
            try:
                current_psi = next(psi_iter)
            except StopIteration:
                current_psi = None
        if current_psi is None:
            break
        delays.append(current_psi - rho_index)

    if not delays:
        return None
    return float(mean(delays))


def score_behavior(ratio_analysis, rhythm, mu_zero_count):
    """Compute curiosity score and classification."""

    percentages = ratio_analysis.get("percentages", {})
    max_band = max(percentages.values()) if percentages else 0.0

    stddev = rhythm.get("stddev_spacing")
    if stddev is None:
        spacing_term = 0.0
    else:
        spacing_term = (1 / (1 + stddev)) * 20

    score = (max_band * 0.6) + spacing_term + (min(mu_zero_count, 5) * 4)
    score = max(0.0, min(100.0, score))

    if score >= 80:
        classification = "Stable Attractor"
    elif score >= 50:
        classification = "Structured Oscillation"
    elif score >= 20:
        classification = "Chaotic Propagation"
    else:
        classification = "Null or Sparse"

    return {"stability_score": score, "classification": classification}


def determine_run_label(config):
    return "psi_{psi}__engine_{engine}__koppa_{koppa}__ratio_{ratio}".format(
        psi=config.get("psi_mode", "unspecified"),
        engine=config.get("engine_mode", "unspecified"),
        koppa=config.get("koppa_mode", "unspecified"),
        ratio=config.get("ratio_trigger", "unspecified"),
    )


def infer_seed(config, values):
    if config.get("seed_upsilon") and config.get("seed_beta"):
        return config["seed_upsilon"], config["seed_beta"]
    if values:
        first = values[0]
        return _format_fraction(first.upsilon), _format_fraction(first.beta)
    return "?", "?"


def infer_tick_count(config, values):
    if "ticks" in config:
        try:
            return int(config["ticks"])
        except ValueError:
            pass
    if not values:
        return 0
    max_tick = max(record.tick for record in values)
    return max_tick + 1


def _format_fraction(value):
    return f"{value.numerator}/{value.denominator}"


def print_summary(config, values, ratio_analysis, rhythm, counts, scorecard):
    run_label = determine_run_label(config)
    upsilon_seed, beta_seed = infer_seed(config, values)
    ticks = infer_tick_count(config, values)

    print("=== TRTS Emergence Report ===")
    print(f"Run: {run_label}")
    print(f"Seeds: υ={upsilon_seed}, β={beta_seed}")
    print(f"Ticks: {ticks}")
    print()
    print(f"ψ events: {counts['psi_events']}")
    print(f"ρ events: {counts['rho_events']}")
    print(f"μ=0 events: {counts['mu_zero_events']}")
    if rhythm.get("average_rho_to_psi_delay") is not None:
        print(f"Avg ρ→ψ delay: {rhythm['average_rho_to_psi_delay']:.2f} MTs")
    print()
    print("Ratio occupancy:")
    print(f"  φ (1.60–1.64): {ratio_analysis['percentages']['phi']:.0f}%")
    print(f"  √2 (1.39–1.44): {ratio_analysis['percentages']['sqrt2']:.0f}%")
    print(f"  Plastic (1.30–1.34): {ratio_analysis['percentages']['plastic']:.0f}%")
    print(f"Band transitions: {ratio_analysis['transitions']}")
    if ratio_analysis.get("longest_band"):
        print(
            f"Longest stay: {ratio_analysis['longest_span']} MTs in {ratio_analysis['longest_band']} band"
        )
    print()
    if rhythm.get("average_spacing") is not None and rhythm.get("stddev_spacing") is not None:
        print(
            "ψ rhythm: avg {avg:.1f} MTs, stddev {std:.1f} → {cls}".format(
                avg=rhythm["average_spacing"],
                std=rhythm["stddev_spacing"],
                cls=rhythm["classification"],
            )
        )
    else:
        print(f"ψ rhythm: insufficient data → {rhythm['classification']}")
    print(f"Classification: {scorecard['classification']}")
    print(f"Stability Score: {scorecard['stability_score']:.0f}/100")


def write_json_summary(path, run_label, ratio_analysis, rhythm, counts, scorecard):
    summary = {
        "run": run_label,
        "psi_events": counts["psi_events"],
        "rho_events": counts["rho_events"],
        "mu_zero_events": counts["mu_zero_events"],
        "ratio_band_hits": {
            "phi": ratio_analysis["percentages"].get("phi", 0.0),
            "sqrt2": ratio_analysis["percentages"].get("sqrt2", 0.0),
            "plastic": ratio_analysis["percentages"].get("plastic", 0.0),
        },
        "band_transitions": ratio_analysis["transitions"],
        "psi_avg_spacing": rhythm.get("average_spacing"),
        "psi_stddev": rhythm.get("stddev_spacing"),
        "classification": scorecard["classification"],
        "stability_score": scorecard["stability_score"],
    }
    with open(path, "w", encoding="utf-8") as handle:
        json.dump(summary, handle, indent=2)
        handle.write("\n")


def main(argv):
    base_dir = os.path.dirname(__file__)
    values_path = os.path.join(base_dir, "values.csv")
    events_path = os.path.join(base_dir, "events.csv")

    if not os.path.exists(values_path) or not os.path.exists(events_path):
        print("Expected values.csv and events.csv in the script directory.")
        return 1

    config = load_configuration(argv)
    values = load_values(values_path)
    events = load_events(events_path)

    ratio_analysis = analyze_ratios(values)
    rhythm, counts = analyze_psi_rhythm(events)
    scorecard = score_behavior(ratio_analysis, rhythm, counts["mu_zero_events"])

    print_summary(config, values, ratio_analysis, rhythm, counts, scorecard)

    run_label = determine_run_label(config)
    output_path = os.path.join(base_dir, "output_summary.json")
    write_json_summary(output_path, run_label, ratio_analysis, rhythm, counts, scorecard)

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
