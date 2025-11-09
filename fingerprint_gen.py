#!/usr/bin/env python3
"""Generate symbolic fingerprints for TRTS run logs.

This utility inspects TRTS microtick exports (``values.csv`` and ``events.csv``
or compatible log formats) and converts them into compact visual signatures.
It produces two artifacts:

* ``<prefix>_fingerprint.csv`` – tick-level aggregates that describe the ψ
  cadence, prime detections, and ratio excursions.
* ``<prefix>_fingerprint.md`` – a human-readable report containing the
  synthesized barcode plus qualitative observations about the run.

Only Python's standard library is used so that the tool can run alongside the
other exploratory scripts in this repository.  Optional plotting requires
``matplotlib``; if the module is missing the script simply skips the figure.
"""

from __future__ import annotations

import argparse
import csv
import math
import os
import re
import statistics
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from fractions import Fraction
from typing import Dict, Iterable, List, Optional, Sequence, Tuple


GOLDEN_RATIO = (1 + math.sqrt(5)) / 2
SQRT_TWO = math.sqrt(2)
PLASTIC_CONSTANT = 1.324717957244746

MICROTICKS_PER_TICK = 11


def parse_arguments(argv: Sequence[str]) -> argparse.Namespace:
    """Parse command-line options."""

    parser = argparse.ArgumentParser(
        description=(
            "Generate a ψ barcode and markdown analysis from TRTS run exports. "
            "Pass any combination of values.csv, events.csv, or textual logs."
        )
    )
    parser.add_argument(
        "paths",
        nargs="+",
        help="Input CSV or log files that contain microtick data.",
    )
    parser.add_argument(
        "--out-prefix",
        default="fingerprint",
        help=(
            "Filename prefix for generated artifacts. "
            "Defaults to 'fingerprint'."
        ),
    )
    parser.add_argument(
        "--plot",
        action="store_true",
        help=(
            "Attempt to render an optional matplotlib plot showing ratio "
            "snapshots across microticks."
        ),
    )
    return parser.parse_args(argv)


def _safe_fraction(numerator: Optional[str], denominator: Optional[str]) -> Optional[Fraction]:
    """Create a :class:`Fraction` from CSV columns, tolerating bad data."""

    if numerator is None or denominator is None:
        return None
    try:
        num = int(numerator)
        den = int(denominator)
        if den == 0:
            return None
    except (TypeError, ValueError):
        return None
    return Fraction(num, den)


def _parse_bool(value: Optional[str]) -> bool:
    """Return ``True`` when the CSV/log value represents an affirmative flag."""

    if value is None:
        return False
    value = value.strip().lower()
    return value in {"1", "true", "yes", "y", "on"}


def _read_csv_rows(path: str) -> Tuple[List[str], List[Dict[str, str]]]:
    with open(path, "r", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        rows = list(reader)
        headers = reader.fieldnames or []
    return headers, rows


def _is_value_table(headers: Sequence[str]) -> bool:
    needed = {"upsilon_num", "upsilon_den", "beta_num", "beta_den"}
    return needed.issubset(set(h.lower() for h in headers))


def _is_event_table(headers: Sequence[str]) -> bool:
    return "event_type" in (h.lower() for h in headers)


def _extract_int(row: Dict[str, str], key: str) -> Optional[int]:
    value = row.get(key)
    if value is None:
        return None
    try:
        return int(value)
    except ValueError:
        return None


@dataclass
class MicrotickSnapshot:
    tick: int
    microtick: int
    event_type: Optional[str] = None
    upsilon: Optional[Fraction] = None
    beta: Optional[Fraction] = None
    koppa: Optional[Fraction] = None
    memory: Optional[Fraction] = None
    phi: Optional[Fraction] = None
    ratio_snapshot: Optional[float] = None
    psi_fired: bool = False
    rho_event: Optional[int] = None
    mu_zero: bool = False
    forced_emission: bool = False
    ratio_triggered: bool = False
    triple_psi: bool = False
    notes: List[str] = field(default_factory=list)

    def psi_symbol(self) -> str:
        """Return a glyph representing the ψ state for the barcode."""

        if self.psi_fired and self.triple_psi:
            return "Ψ"
        if self.psi_fired:
            return "ψ"
        if self.rho_event:
            return "ρ"
        if self.mu_zero:
            return "μ"
        return "·"


def load_snapshots(paths: Sequence[str]) -> Dict[Tuple[int, int], MicrotickSnapshot]:
    """Load microtick snapshots from CSV or textual logs."""

    snapshots: Dict[Tuple[int, int], MicrotickSnapshot] = {}

    for path in paths:
        if not os.path.exists(path):
            sys.stderr.write(f"Warning: file does not exist – {path}\n")
            continue

        extension = os.path.splitext(path)[1].lower()
        if extension == ".csv":
            headers, rows = _read_csv_rows(path)
            if not headers:
                continue
            header_lower = [h.lower() for h in headers]
            if _is_value_table(header_lower):
                _load_value_rows(rows, snapshots)
            elif _is_event_table(header_lower):
                _load_event_rows(rows, snapshots)
            else:
                _load_generic_csv(rows, snapshots)
        else:
            _load_text_log(path, snapshots)

    return snapshots


def _snapshot_for(snapshots: Dict[Tuple[int, int], MicrotickSnapshot], tick: int, mt: int) -> MicrotickSnapshot:
    key = (tick, mt)
    if key not in snapshots:
        snapshots[key] = MicrotickSnapshot(tick=tick, microtick=mt)
    return snapshots[key]


def _load_value_rows(rows: Iterable[Dict[str, str]], snapshots: Dict[Tuple[int, int], MicrotickSnapshot]) -> None:
    for row in rows:
        tick = _extract_int(row, "tick")
        mt = _extract_int(row, "mt")
        if tick is None or mt is None:
            continue

        snapshot = _snapshot_for(snapshots, tick, mt)
        snapshot.upsilon = _safe_fraction(row.get("upsilon_num"), row.get("upsilon_den"))
        snapshot.beta = _safe_fraction(row.get("beta_num"), row.get("beta_den"))
        snapshot.koppa = _safe_fraction(row.get("koppa_num"), row.get("koppa_den"))
        snapshot.memory = _safe_fraction(row.get("memory_num"), row.get("memory_den"))
        snapshot.phi = _safe_fraction(row.get("phi_num"), row.get("phi_den"))

        if snapshot.upsilon is not None and snapshot.beta not in (None, Fraction(0, 1)):
            snapshot.ratio_snapshot = float(snapshot.upsilon) / float(snapshot.beta)
        elif snapshot.beta == Fraction(0, 1):
            snapshot.ratio_snapshot = math.inf


def _load_event_rows(rows: Iterable[Dict[str, str]], snapshots: Dict[Tuple[int, int], MicrotickSnapshot]) -> None:
    for row in rows:
        tick = _extract_int(row, "tick")
        mt = _extract_int(row, "mt")
        if tick is None or mt is None:
            continue

        snapshot = _snapshot_for(snapshots, tick, mt)
        snapshot.event_type = row.get("event_type") or snapshot.event_type

        if _parse_bool(row.get("psi_fired")):
            snapshot.psi_fired = True

        rho_value = _extract_int(row, "rho_event")
        if rho_value is not None:
            snapshot.rho_event = rho_value

        if _parse_bool(row.get("mu_zero")):
            snapshot.mu_zero = True

        if _parse_bool(row.get("forced_emission")):
            snapshot.forced_emission = True

        if _parse_bool(row.get("ratio_triggered")):
            snapshot.ratio_triggered = True

        if _parse_bool(row.get("triple_psi")):
            snapshot.triple_psi = True


def _load_generic_csv(rows: Iterable[Dict[str, str]], snapshots: Dict[Tuple[int, int], MicrotickSnapshot]) -> None:
    for row in rows:
        tick = _extract_int(row, "tick")
        mt = _extract_int(row, "mt")
        if tick is None or mt is None:
            continue

        snapshot = _snapshot_for(snapshots, tick, mt)
        for key, value in row.items():
            if key in {"tick", "mt"}:
                continue
            if value is None or value == "":
                continue
            snapshot.notes.append(f"{key}={value}")


def _load_text_log(path: str, snapshots: Dict[Tuple[int, int], MicrotickSnapshot]) -> None:
    pattern = re.compile(
        r"upsilon=(?P<ups_num>-?\d+)/(?:\s*)?(?P<ups_den>-?\d+).*?beta=(?P<beta_num>-?\d+)/(?:\s*)?(?P<beta_den>-?\d+)",
        re.IGNORECASE,
    )

    psi_pattern = re.compile(r"psi[_\s]?fired[:=]\s*(1|true|yes)", re.IGNORECASE)
    rho_pattern = re.compile(r"rho[_\s]?event[:=]\s*(\d+)", re.IGNORECASE)
    mu_pattern = re.compile(r"mu[_\s]?zero[:=]\s*(1|true|yes)", re.IGNORECASE)

    with open(path, "r", encoding="utf-8") as handle:
        for line in handle:
            match = re.search(r"tick=(\d+)\s+mt=(\d+)", line)
            if not match:
                continue
            tick = int(match.group(1))
            mt = int(match.group(2))
            snapshot = _snapshot_for(snapshots, tick, mt)

            frac_match = pattern.search(line)
            if frac_match:
                snapshot.upsilon = _safe_fraction(frac_match.group("ups_num"), frac_match.group("ups_den"))
                snapshot.beta = _safe_fraction(frac_match.group("beta_num"), frac_match.group("beta_den"))
                if snapshot.upsilon is not None and snapshot.beta not in (None, Fraction(0, 1)):
                    snapshot.ratio_snapshot = float(snapshot.upsilon) / float(snapshot.beta)
                elif snapshot.beta == Fraction(0, 1):
                    snapshot.ratio_snapshot = math.inf

            if psi_pattern.search(line):
                snapshot.psi_fired = True
            rho_match = rho_pattern.search(line)
            if rho_match:
                snapshot.rho_event = int(rho_match.group(1))
            if mu_pattern.search(line):
                snapshot.mu_zero = True


def render_barcode(snapshots: Dict[Tuple[int, int], MicrotickSnapshot]) -> Tuple[str, Dict[int, str]]:
    """Return the ψ barcode string and per-tick glyph map."""

    by_tick: Dict[int, Dict[int, MicrotickSnapshot]] = defaultdict(dict)
    for (tick, mt), snapshot in snapshots.items():
        by_tick[tick][mt] = snapshot

    tick_barcodes: Dict[int, str] = {}
    global_chunks: List[str] = []

    for tick in sorted(by_tick):
        line_chars = []
        for mt in range(1, MICROTICKS_PER_TICK + 1):
            snapshot = by_tick[tick].get(mt)
            line_chars.append(snapshot.psi_symbol() if snapshot else "·")
        barcode_line = "".join(line_chars)
        tick_barcodes[tick] = barcode_line
        global_chunks.append(f"T{tick}:{barcode_line}")

    return " | ".join(global_chunks), tick_barcodes


def summarize_ticks(snapshots: Dict[Tuple[int, int], MicrotickSnapshot]) -> List[Dict[str, object]]:
    """Aggregate microtick data into tick-level rows for CSV export."""

    rows: List[Dict[str, object]] = []
    by_tick: Dict[int, List[MicrotickSnapshot]] = defaultdict(list)
    for snapshot in snapshots.values():
        by_tick[snapshot.tick].append(snapshot)

    for tick in sorted(by_tick):
        entries = sorted(by_tick[tick], key=lambda snap: snap.microtick)
        lookup = {snap.microtick: snap for snap in entries}
        psi_count = sum(1 for snap in entries if snap.psi_fired)
        rho_count = sum(1 for snap in entries if snap.rho_event)
        mu_count = sum(1 for snap in entries if snap.mu_zero)
        ratio_values = [snap.ratio_snapshot for snap in entries if snap.ratio_snapshot is not None]
        ratio_values = [value for value in ratio_values if not math.isnan(value)]
        ratio_mean: Optional[float]
        ratio_std: Optional[float]
        ratio_min: Optional[float]
        ratio_max: Optional[float]
        if ratio_values:
            ratio_mean = statistics.mean(ratio_values)
            ratio_std = statistics.pstdev(ratio_values) if len(ratio_values) > 1 else 0.0
            ratio_min = min(ratio_values)
            ratio_max = max(ratio_values)
        else:
            ratio_mean = ratio_std = ratio_min = ratio_max = None

        barcode_chars = []
        for mt in range(1, MICROTICKS_PER_TICK + 1):
            snapshot = lookup.get(mt)
            barcode_chars.append(snapshot.psi_symbol() if snapshot else "·")
        barcode_line = "".join(barcode_chars)

        rho_positions = ", ".join(str(mt) for mt in sorted(lookup) if lookup[mt].rho_event)
        mu_positions = ", ".join(str(mt) for mt in sorted(lookup) if lookup[mt].mu_zero)

        rows.append(
            {
                "tick": tick,
                "psi_events": psi_count,
                "rho_events": rho_count,
                "mu_zero": mu_count,
                "ratio_mean": ratio_mean,
                "ratio_std": ratio_std,
                "ratio_min": ratio_min,
                "ratio_max": ratio_max,
                "psi_barcode": barcode_line,
                "rho_microticks": rho_positions,
                "mu_microticks": mu_positions,
            }
        )

    return rows


def _format_float(value: Optional[float]) -> str:
    if value is None:
        return ""
    if math.isinf(value):
        return "inf"
    return f"{value:.6f}"


def write_summary_csv(rows: List[Dict[str, object]], path: str) -> None:
    """Write tick-level summary rows to CSV."""

    fieldnames = [
        "tick",
        "psi_events",
        "rho_events",
        "mu_zero",
        "ratio_mean",
        "ratio_std",
        "ratio_min",
        "ratio_max",
        "psi_barcode",
        "rho_microticks",
        "mu_microticks",
    ]

    with open(path, "w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            output_row = row.copy()
            for key in ("ratio_mean", "ratio_std", "ratio_min", "ratio_max"):
                output_row[key] = _format_float(row[key])
            writer.writerow(output_row)


def analyse_global_patterns(snapshots: Dict[Tuple[int, int], MicrotickSnapshot]) -> Dict[str, object]:
    """Compute global metrics for the markdown narrative."""

    psi_intervals: List[int] = []
    last_index: Optional[int] = None
    ratio_snapshots: List[Tuple[int, int, float]] = []
    ups_numerators: List[int] = []
    ups_denominators: List[int] = []
    beta_numerators: List[int] = []
    beta_denominators: List[int] = []
    mu_ticks: List[Tuple[int, int]] = []
    rho_ticks: List[Tuple[int, int]] = []

    for (tick, mt), snapshot in sorted(snapshots.items()):
        global_index = (tick - 1) * MICROTICKS_PER_TICK + (mt - 1)
        if snapshot.psi_fired:
            if last_index is not None:
                psi_intervals.append(global_index - last_index)
            last_index = global_index

        if snapshot.ratio_snapshot is not None and not math.isnan(snapshot.ratio_snapshot):
            ratio_snapshots.append((tick, mt, snapshot.ratio_snapshot))

        if snapshot.upsilon is not None:
            ups_numerators.append(abs(snapshot.upsilon.numerator))
            ups_denominators.append(abs(snapshot.upsilon.denominator))
        if snapshot.beta is not None:
            beta_numerators.append(abs(snapshot.beta.numerator))
            beta_denominators.append(abs(snapshot.beta.denominator))

        if snapshot.mu_zero:
            mu_ticks.append((tick, mt))
        if snapshot.rho_event:
            rho_ticks.append((tick, mt))

    ratio_affinities = detect_ratio_affinities(ratio_snapshots)
    cycle_candidates = detect_cycles(ratio_snapshots)
    prime_twins = detect_prime_twins(rho_ticks)

    return {
        "psi_intervals": psi_intervals,
        "ratio_snapshots": ratio_snapshots,
        "ups_growth": analyse_growth(ups_numerators, ups_denominators),
        "beta_growth": analyse_growth(beta_numerators, beta_denominators),
        "ratio_affinities": ratio_affinities,
        "cycle_candidates": cycle_candidates,
        "prime_twins": prime_twins,
        "mu_ticks": mu_ticks,
        "rho_ticks": rho_ticks,
    }


def analyse_growth(numerators: List[int], denominators: List[int]) -> Dict[str, object]:
    """Summarise growth behaviour for a rational sequence."""

    if not numerators or not denominators:
        return {"classification": "insufficient", "span": None, "log_ratio": None}

    num_max = max(numerators)
    num_min = min(numerators)
    den_max = max(denominators)
    den_min = min(denominators)

    span = max(num_max - num_min, den_max - den_min)

    def _log_ratio(max_val: int, min_val: int) -> Optional[float]:
        if min_val <= 0 or max_val <= 0:
            return None
        try:
            return math.log10(max_val) - math.log10(min_val)
        except ValueError:
            return None

    log_ratios = [value for value in (_log_ratio(num_max, num_min), _log_ratio(den_max, den_min)) if value is not None]
    max_log_ratio = max(log_ratios) if log_ratios else None

    if span <= 25 and (max_log_ratio is None or max_log_ratio <= math.log10(5)):
        classification = "bounded"
    elif span <= 250 or (max_log_ratio is not None and max_log_ratio <= math.log10(50)):
        classification = "moderate"
    else:
        classification = "expansive"

    span_repr: object
    if span == 0:
        span_repr = 0
    elif span < 1000:
        span_repr = span
    else:
        span_repr = f"≈10^{len(str(span)) - 1}"

    return {
        "classification": classification,
        "span": span_repr,
        "log_ratio": max_log_ratio,
    }


def detect_ratio_affinities(ratio_snapshots: List[Tuple[int, int, float]]) -> Dict[str, List[Tuple[int, int, float]]]:
    """Return ticks that flirt with notable irrational ratios."""

    affinities: Dict[str, List[Tuple[int, int, float]]] = {
        "golden": [],
        "sqrt2": [],
        "plastic": [],
    }
    tolerance = 0.02

    for tick, mt, value in ratio_snapshots:
        if not math.isfinite(value):
            continue
        if abs(value - GOLDEN_RATIO) < tolerance:
            affinities["golden"].append((tick, mt, value))
        if abs(value - SQRT_TWO) < tolerance:
            affinities["sqrt2"].append((tick, mt, value))
        if abs(value - PLASTIC_CONSTANT) < tolerance:
            affinities["plastic"].append((tick, mt, value))

    return {key: value for key, value in affinities.items() if value}


def detect_cycles(ratio_snapshots: List[Tuple[int, int, float]]) -> List[Tuple[int, int, int, int]]:
    """Identify approximate returns to previously observed ratios."""

    seen: Dict[int, Tuple[int, int, float]] = {}
    cycles: List[Tuple[int, int, int, int]] = []

    for tick, mt, value in ratio_snapshots:
        if not math.isfinite(value):
            continue
        key = int(round(value * 1000))
        if key in seen:
            prev_tick, prev_mt, _ = seen[key]
            if (tick - prev_tick) * MICROTICKS_PER_TICK + (mt - prev_mt) >= MICROTICKS_PER_TICK:
                cycles.append((prev_tick, prev_mt, tick, mt))
        seen[key] = (tick, mt, value)

    return cycles


def detect_prime_twins(rho_ticks: List[Tuple[int, int]]) -> List[Tuple[Tuple[int, int], Tuple[int, int]]]:
    """Search for pairs of ρ detections spaced like twin primes (ΔMT ≤ 4)."""

    twins = []
    rho_ticks_sorted = sorted(rho_ticks)
    for idx in range(1, len(rho_ticks_sorted)):
        prev = rho_ticks_sorted[idx - 1]
        current = rho_ticks_sorted[idx]
        if prev[0] == current[0]:
            delta = current[1] - prev[1]
        else:
            delta = (current[0] - prev[0]) * MICROTICKS_PER_TICK + (current[1] - prev[1])
        if 0 < delta <= 4:
            twins.append((prev, current))
    return twins


def build_markdown_report(
    prefix: str,
    barcode: str,
    tick_barcodes: Dict[int, str],
    summary_rows: List[Dict[str, object]],
    globals_summary: Dict[str, object],
) -> str:
    """Create the markdown report string."""

    lines = [f"# TRTS Fingerprint Report – {prefix}", ""]
    lines.append("## ψ Rhythm Barcode")
    lines.append("```")
    for tick in sorted(tick_barcodes):
        lines.append(f"T{tick:02d} {tick_barcodes[tick]}")
    lines.append("```")
    lines.append(f"Combined barcode: {barcode}")
    lines.append("")

    lines.append("## Tick Aggregates")
    lines.append("| tick | ψ events | ρ events | μ=0 | ratio μ ± σ | span |")
    lines.append("| ---: | -------: | -------: | ---: | ----------- | ----:|")

    for row in summary_rows:
        ratio_mean = _format_float(row["ratio_mean"])
        ratio_std = _format_float(row["ratio_std"])
        if ratio_mean and ratio_std:
            ratio_cell = f"{ratio_mean} ± {ratio_std}"
        elif ratio_mean:
            ratio_cell = ratio_mean
        else:
            ratio_cell = ""
        span_cell = ""
        if row["ratio_min"] is not None and row["ratio_max"] is not None:
            span_cell = f"{_format_float(row['ratio_min'])} → {_format_float(row['ratio_max'])}"
        lines.append(
            f"| {row['tick']:>4} | {row['psi_events']:>7} | {row['rho_events']:>7} "
            f"| {row['mu_zero']:>3} | {ratio_cell:>11} | {span_cell:>6} |"
        )

    lines.append("")

    lines.extend(render_global_analysis(globals_summary))

    return "\n".join(lines)


def render_global_analysis(summary: Dict[str, object]) -> List[str]:
    """Render the analytical narrative paragraphs."""

    lines = ["## Behavioral Notes", ""]

    psi_intervals: List[int] = summary.get("psi_intervals", [])  # type: ignore[assignment]
    if psi_intervals:
        mean_interval = statistics.mean(psi_intervals)
        if len(psi_intervals) > 1:
            std_interval = statistics.pstdev(psi_intervals)
        else:
            std_interval = 0.0
        lines.append(
            "- ψ cadence average spacing: "
            f"{mean_interval:.2f} microticks (σ = {std_interval:.2f})."
        )
    else:
        lines.append("- ψ cadence never fired within the observed window.")

    ups_growth = summary.get("ups_growth", {})
    if ups_growth:
        ratio_val = ups_growth.get("log_ratio")
        if isinstance(ratio_val, (int, float)):
            ratio_text = f"Δlog₁₀={ratio_val:.2f}"
        else:
            ratio_text = "n/a"
        lines.append(
            "- υ growth classified as {classification} (span={span}, ratio {ratio}).".format(
                classification=ups_growth.get("classification", "unknown"),
                span=ups_growth.get("span", "n/a"),
                ratio=ratio_text,
            )
        )

    beta_growth = summary.get("beta_growth", {})
    if beta_growth:
        ratio_val = beta_growth.get("log_ratio")
        if isinstance(ratio_val, (int, float)):
            ratio_text = f"Δlog₁₀={ratio_val:.2f}"
        else:
            ratio_text = "n/a"
        lines.append(
            "- β growth classified as {classification} (span={span}, ratio {ratio}).".format(
                classification=beta_growth.get("classification", "unknown"),
                span=beta_growth.get("span", "n/a"),
                ratio=ratio_text,
            )
        )

    ratio_affinities: Dict[str, List[Tuple[int, int, float]]] = summary.get("ratio_affinities", {})  # type: ignore[assignment]
    if ratio_affinities:
        fragments = []
        for name, hits in sorted(ratio_affinities.items()):
            glyph = {
                "golden": "φ",
                "sqrt2": "√2",
                "plastic": "ρₚ",
            }[name]
            formatted_hits = ", ".join(f"T{tick}·MT{mt} ({value:.4f})" for tick, mt, value in hits)
            fragments.append(f"{glyph} near-hits at {formatted_hits}")
        lines.append("- Ratio neighborhoods: " + "; ".join(fragments) + ".")
    else:
        lines.append("- No notable ratio neighborhoods were encountered (φ/√2/plastic).")

    cycle_candidates: List[Tuple[int, int, int, int]] = summary.get("cycle_candidates", [])  # type: ignore[assignment]
    if cycle_candidates:
        formatted = ", ".join(
            f"T{start_tick}·MT{start_mt} → T{end_tick}·MT{end_mt}" for start_tick, start_mt, end_tick, end_mt in cycle_candidates
        )
        lines.append("- Repeating ratio echoes at: " + formatted + ".")
    else:
        lines.append("- No repeating ratio echoes detected at 0.001 tolerance.")

    prime_twins: List[Tuple[Tuple[int, int], Tuple[int, int]]] = summary.get("prime_twins", [])  # type: ignore[assignment]
    if prime_twins:
        formatted = ", ".join(
            f"(T{a[0]}·MT{a[1]} ↔ T{b[0]}·MT{b[1]})" for a, b in prime_twins
        )
        lines.append("- ρ twin candidates: " + formatted + ".")
    else:
        lines.append("- No ρ twin cadence was observed within ΔMT ≤ 4.")

    mu_ticks: List[Tuple[int, int]] = summary.get("mu_ticks", [])  # type: ignore[assignment]
    if mu_ticks:
        formatted = ", ".join(f"T{tick}·MT{mt}" for tick, mt in mu_ticks)
        lines.append("- μ = 0 excursions at: " + formatted + ".")

    rho_ticks: List[Tuple[int, int]] = summary.get("rho_ticks", [])  # type: ignore[assignment]
    if rho_ticks:
        formatted = ", ".join(f"T{tick}·MT{mt}" for tick, mt in rho_ticks)
        lines.append("- ρ detections at: " + formatted + ".")

    lines.append("")
    return lines


def maybe_plot_ratio(prefix: str, ratio_snapshots: List[Tuple[int, int, float]], tick_barcodes: Dict[int, str]) -> Optional[str]:
    """Optionally create a matplotlib plot showing ratio traces."""

    if not ratio_snapshots:
        return None
    try:
        import matplotlib.pyplot as plt  # type: ignore
    except Exception:  # pragma: no cover - optional dependency
        sys.stderr.write("Matplotlib not available; skipping plot.\n")
        return None

    indices = []
    values = []
    for tick, mt, value in ratio_snapshots:
        if not math.isfinite(value):
            continue
        global_idx = (tick - 1) * MICROTICKS_PER_TICK + (mt - 1)
        indices.append(global_idx)
        values.append(value)

    if not indices:
        return None

    fig, ax = plt.subplots(figsize=(10, 4))
    ax.plot(indices, values, marker="o", linestyle="-", linewidth=1)
    ax.set_title("TRTS Ratio Trace")
    ax.set_xlabel("Microtick index")
    ax.set_ylabel("υ / β snapshot")

    for tick, barcode in sorted(tick_barcodes.items()):
        x = (tick - 1) * MICROTICKS_PER_TICK
        ax.axvline(x=x, color="lightgray", linewidth=0.5)
        ax.text(x + 0.5, ax.get_ylim()[1], barcode, fontsize=6, rotation=90, va="bottom")

    plot_path = f"{prefix}_ratio_plot.png"
    fig.tight_layout()
    fig.savefig(plot_path, dpi=150)
    plt.close(fig)
    return plot_path


def main(argv: Sequence[str]) -> int:
    args = parse_arguments(argv)
    snapshots = load_snapshots(args.paths)
    if not snapshots:
        sys.stderr.write("No usable microtick data were discovered.\n")
        return 1

    barcode, tick_barcodes = render_barcode(snapshots)
    summary_rows = summarize_ticks(snapshots)
    globals_summary = analyse_global_patterns(snapshots)

    csv_path = f"{args.out_prefix}_fingerprint.csv"
    write_summary_csv(summary_rows, csv_path)

    markdown_report = build_markdown_report(args.out_prefix, barcode, tick_barcodes, summary_rows, globals_summary)
    md_path = f"{args.out_prefix}_fingerprint.md"
    with open(md_path, "w", encoding="utf-8") as handle:
        handle.write(markdown_report)

    if args.plot:
        plot_path = maybe_plot_ratio(args.out_prefix, globals_summary.get("ratio_snapshots", []), tick_barcodes)
        if plot_path:
            print(f"Saved ratio plot to {plot_path}")

    print(f"Saved tick summary to {csv_path}")
    print(f"Saved markdown report to {md_path}")
    return 0


if __name__ == "__main__":  # pragma: no cover
    sys.exit(main(sys.argv[1:]))
