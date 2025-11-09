#!/usr/bin/env python3
"""Autonomous symbolic hypothesis engine for TRTS outputs."""

from __future__ import annotations

import argparse
import csv
import json
import math
import os
import sys
from dataclasses import dataclass, field
from typing import Dict, Iterable, List, Optional, Sequence, Tuple


TARGET_APPROXIMATIONS = {
    "phi": (1.0 + math.sqrt(5.0)) / 2.0,
    "sqrt2": math.sqrt(2.0),
    "plastic": 1.32471795724474602596,
    "silver": 1.4142135623730951,
}


def parse_arguments(argv: Optional[Sequence[str]] = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Analyze TRTS engine outputs, synthesize symbolic hypotheses, and "
            "suggest new rational configurations for investigation."
        )
    )
    parser.add_argument(
        "--scan-all",
        action="store_true",
        help="Traverse every known analysis directory plus the repository root.",
    )
    parser.add_argument(
        "--suggest-tests",
        action="store_true",
        help="Emit suggestions.csv with proposed follow-up configurations.",
    )
    parser.add_argument(
        "--report",
        action="store_true",
        help="Generate theories.md narrative output.",
    )
    parser.add_argument(
        "--target",
        metavar="NAME",
        help="Highlight runs whose ratios linger near a named constant (phi, sqrt2, plastic, silver).",
    )
    parser.add_argument(
        "--output-dir",
        default=os.path.join(".", "hypotheses"),
        help="Directory for generated hypothesis artifacts.",
    )
    parser.add_argument(
        "--limit",
        type=int,
        default=None,
        help="Maximum number of hypotheses to report (default: all).",
    )
    parser.add_argument(
        "paths",
        nargs="*",
        help="Optional explicit paths to scan instead of default directories.",
    )
    return parser.parse_args(argv)


@dataclass
class RationalValue:
    numerator: int
    denominator: int

    def as_float(self) -> float:
        if self.denominator == 0:
            return math.inf if self.numerator >= 0 else -math.inf
        return self.numerator / self.denominator

    def to_text(self) -> str:
        return f"{self.numerator}/{self.denominator}"


@dataclass
class MicrotickRecord:
    tick: int
    microtick: int
    upsilon: RationalValue
    beta: RationalValue
    koppa: RationalValue
    memory: Optional[RationalValue] = None


@dataclass
class EventRecord:
    tick: int
    microtick: int
    event_type: str
    rho_event: bool
    psi_fired: bool
    mu_zero: bool
    triple_psi: bool = False


@dataclass
class RunContainer:
    run_id: str
    value_rows: List[Dict[str, str]] = field(default_factory=list)
    event_rows: List[Dict[str, str]] = field(default_factory=list)
    metadata: Dict[str, str] = field(default_factory=dict)
    metadata_snapshots: List[Tuple[str, Dict[str, str]]] = field(default_factory=list)
    logs: List[Tuple[str, List[str]]] = field(default_factory=list)
    phase_maps: List[Tuple[str, Dict[str, object]]] = field(default_factory=list)
    fingerprints: List[Tuple[str, str]] = field(default_factory=list)


@dataclass
class RunAnalysis:
    run_id: str
    ratios: List[float]
    psi_pattern: str
    rho_pattern: str
    triple_psi_count: int
    psi_density: float
    rho_density: float
    mu_zero_count: int
    convergence_value: Optional[float]
    ratio_variation: float
    engine_symmetry: str
    psi_mode: str
    koppa_mode: str
    engine_mode: str
    koppa_trigger: str
    triple_mode: str
    initial_upsilon: Optional[RationalValue]
    initial_beta: Optional[RationalValue]
    metadata: Dict[str, str]


@dataclass
class Hypothesis:
    identifier: str
    description: str
    cluster_key: str
    support_runs: List[str]
    contradiction_runs: List[str]
    confidence: float
    coverage: float
    entropy: float
    psi_signature: str
    koppa_mode: str
    engine_mode: str
    convergence_value: Optional[float]


class ContradictionTracker:
    def __init__(self) -> None:
        self._records: Dict[str, List[str]] = {}

    def register(self, hypothesis_id: str, run_id: str) -> None:
        self._records.setdefault(hypothesis_id, []).append(run_id)

    def contradictions_for(self, hypothesis_id: str) -> List[str]:
        return self._records.get(hypothesis_id, [])


def parse_int(value: Optional[str], default: int = 0) -> int:
    if value is None:
        return default
    value = value.strip()
    if not value:
        return default
    try:
        return int(value)
    except ValueError:
        try:
            return int(float(value))
        except ValueError:
            return default


def parse_bool(value: Optional[str]) -> bool:
    if value is None:
        return False
    value = value.strip().lower()
    return value in {"1", "true", "yes", "on", "y"}


def determine_run_id(base: str, root: str) -> str:
    rel_path = os.path.relpath(root, base)
    if rel_path == ".":
        return os.path.basename(base)
    return os.path.join(os.path.basename(base), rel_path)


def gather_run_containers(paths: Sequence[str], include_root: bool) -> Dict[str, RunContainer]:
    base_directories = [
        os.path.abspath(path)
        for path in paths
        if os.path.exists(path)
    ]
    containers: Dict[str, RunContainer] = {}

    for base in base_directories:
        for root, _dirs, files in os.walk(base):
            run_id = determine_run_id(base, root)
            container = containers.setdefault(run_id, RunContainer(run_id))
            for filename in files:
                path = os.path.join(root, filename)
                if filename.endswith(".csv"):
                    _handle_csv(path, container)
                elif filename.endswith(".json"):
                    _handle_json(path, container)
                elif filename.endswith(".log"):
                    _handle_log(path, container)
                elif filename.endswith(".fingerprint"):
                    _handle_fingerprint(path, container)

    if include_root:
        root_path = os.getcwd()
        root_container = containers.setdefault("root", RunContainer("root"))
        for filename in os.listdir(root_path):
            path = os.path.join(root_path, filename)
            if os.path.isdir(path):
                continue
            if filename.endswith(".csv"):
                _handle_csv(path, root_container)
            elif filename.endswith(".json"):
                _handle_json(path, root_container)
            elif filename.endswith(".log"):
                _handle_log(path, root_container)
    return containers


def _handle_csv(path: str, container: RunContainer) -> None:
    try:
        with open(path, "r", encoding="utf-8") as handle:
            reader = csv.DictReader(handle)
            header = reader.fieldnames or []
            if {"upsilon_num", "upsilon_den", "beta_num", "beta_den"}.issubset(header):
                for row in reader:
                    container.value_rows.append(dict(row))
            elif {"event_type", "psi_fired", "rho_event"}.issubset(header):
                for row in reader:
                    container.event_rows.append(dict(row))
            else:
                pass
    except FileNotFoundError:
        return


def _handle_json(path: str, container: RunContainer) -> None:
    try:
        with open(path, "r", encoding="utf-8") as handle:
            data = json.load(handle)
    except (FileNotFoundError, json.JSONDecodeError):
        return
    if isinstance(data, dict):
        metadata_snapshot = {}
        for key, value in data.items():
            if isinstance(value, (str, int, float, bool)):
                metadata_snapshot[str(key)] = str(value)
        container.metadata.update(metadata_snapshot)
        container.metadata_snapshots.append((path, metadata_snapshot))
        if data.get("phase_map"):
            container.phase_maps.append((path, data.get("phase_map")))
    elif isinstance(data, list):
        container.metadata_snapshots.append((path, {"entries": str(len(data))}))


def _handle_log(path: str, container: RunContainer) -> None:
    try:
        with open(path, "r", encoding="utf-8") as handle:
            lines = handle.readlines()
    except FileNotFoundError:
        return
    container.logs.append((path, lines))


def _handle_fingerprint(path: str, container: RunContainer) -> None:
    try:
        with open(path, "r", encoding="utf-8") as handle:
            signature = handle.read().strip()
    except FileNotFoundError:
        return
    container.fingerprints.append((path, signature))


def _build_microtick_records(rows: List[Dict[str, str]]) -> List[MicrotickRecord]:
    records: List[MicrotickRecord] = []
    for row in rows:
        tick = parse_int(row.get("tick"), 0)
        mt = parse_int(row.get("mt"), 0)
        upsilon = RationalValue(parse_int(row.get("upsilon_num"), 0), parse_int(row.get("upsilon_den"), 1))
        beta = RationalValue(parse_int(row.get("beta_num"), 0), parse_int(row.get("beta_den"), 1))
        koppa = RationalValue(parse_int(row.get("koppa_num"), 0), parse_int(row.get("koppa_den"), 1))
        memory_val = None
        if "memory_num" in row and "memory_den" in row:
            memory_val = RationalValue(
                parse_int(row.get("memory_num"), 0),
                parse_int(row.get("memory_den"), 1),
            )
        records.append(MicrotickRecord(tick, mt, upsilon, beta, koppa, memory_val))
    records.sort(key=lambda rec: (rec.tick, rec.microtick))
    return records


def _build_event_records(rows: List[Dict[str, str]]) -> List[EventRecord]:
    records: List[EventRecord] = []
    for row in rows:
        tick = parse_int(row.get("tick"), 0)
        mt = parse_int(row.get("mt"), 0)
        event_type = row.get("event_type", "?")
        rho_event = parse_bool(row.get("rho_event"))
        psi_fired = parse_bool(row.get("psi_fired"))
        mu_zero = parse_bool(row.get("mu_zero"))
        triple = parse_bool(row.get("triple_psi"))
        records.append(EventRecord(tick, mt, event_type, rho_event, psi_fired, mu_zero, triple))
    records.sort(key=lambda rec: (rec.tick, rec.microtick))
    return records


def analyze_run(container: RunContainer) -> RunAnalysis:
    microticks = _build_microtick_records(container.value_rows)
    events = _build_event_records(container.event_rows)

    ratios: List[float] = []
    psi_by_tick: Dict[int, int] = {}
    rho_by_tick: Dict[int, int] = {}
    triple_count = 0
    mu_zero_count = 0

    for event in events:
        if event.psi_fired:
            psi_by_tick[event.tick] = psi_by_tick.get(event.tick, 0) + 1
        if event.rho_event:
            rho_by_tick[event.tick] = rho_by_tick.get(event.tick, 0) + 1
        if event.triple_psi:
            triple_count += 1
        if event.mu_zero:
            mu_zero_count += 1

    for record in microticks:
        beta_float = record.beta.as_float()
        if beta_float in (math.inf, -math.inf) or record.beta.denominator == 0:
            continue
        ratios.append(record.upsilon.as_float() / beta_float if beta_float else math.inf)

    convergence_value = None
    if ratios:
        tail = ratios[-5:] if len(ratios) >= 5 else ratios[:]
        convergence_value = sum(tail) / len(tail)

    ratio_variation = 0.0
    if ratios:
        ratio_variation = max(ratios) - min(ratios)

    total_events = max(1, len(events))
    psi_density = sum(psi_by_tick.values()) / total_events
    rho_density = sum(rho_by_tick.values()) / total_events

    psi_pattern = _pattern_string(psi_by_tick)
    rho_pattern = _pattern_string(rho_by_tick)

    engine_symmetry = _categorize_symmetry(ratios)

    psi_mode = container.metadata.get("psi_mode", container.metadata.get("psiMode", "unspecified"))
    koppa_mode = container.metadata.get("koppa_mode", container.metadata.get("koppaMode", "unspecified"))
    engine_mode = container.metadata.get("engine_mode", container.metadata.get("engineMode", "unspecified"))
    koppa_trigger = container.metadata.get("koppa_trigger", container.metadata.get("koppaTrigger", "unspecified"))
    triple_mode = container.metadata.get("triple_mode", container.metadata.get("tripleMode", "observed"))

    initial_upsilon = microticks[0].upsilon if microticks else None
    initial_beta = microticks[0].beta if microticks else None

    return RunAnalysis(
        run_id=container.run_id,
        ratios=ratios,
        psi_pattern=psi_pattern,
        rho_pattern=rho_pattern,
        triple_psi_count=triple_count,
        psi_density=psi_density,
        rho_density=rho_density,
        mu_zero_count=mu_zero_count,
        convergence_value=convergence_value,
        ratio_variation=ratio_variation,
        engine_symmetry=engine_symmetry,
        psi_mode=psi_mode,
        koppa_mode=koppa_mode,
        engine_mode=engine_mode,
        koppa_trigger=koppa_trigger,
        triple_mode=triple_mode,
        initial_upsilon=initial_upsilon,
        initial_beta=initial_beta,
        metadata=container.metadata,
    )


def _pattern_string(counts: Dict[int, int]) -> str:
    if not counts:
        return "none"
    parts = [f"T{tick}:{counts[tick]}" for tick in sorted(counts)]
    return "+".join(parts)


def _categorize_symmetry(ratios: Sequence[float]) -> str:
    if not ratios:
        return "unknown"
    last = ratios[-1]
    if last in (math.inf, -math.inf) or last == 0:
        return "singular"
    reciprocal = 1.0 / last
    if abs(last - reciprocal) < 1e-3:
        return "reciprocal"
    midpoint = sum(ratios) / len(ratios)
    if abs(midpoint - last) < 1e-4:
        return "steady"
    return "skewed"


def cluster_by_convergence(analyses: Sequence[RunAnalysis]) -> List[Tuple[float, List[RunAnalysis]]]:
    clusters: List[Tuple[float, List[RunAnalysis]]] = []
    tolerance = 1e-5
    for analysis in analyses:
        value = analysis.convergence_value
        if value is None or value in (math.inf, -math.inf):
            continue
        placed = False
        for idx, (ref, members) in enumerate(clusters):
            if abs(ref - value) < tolerance:
                members.append(analysis)
                placed = True
                break
        if not placed:
            clusters.append((value, [analysis]))
    clusters.sort(key=lambda item: item[0])
    return clusters


def compute_entropy(signatures: Sequence[str]) -> float:
    if not signatures:
        return 0.0
    counts: Dict[str, int] = {}
    for signature in signatures:
        counts[signature] = counts.get(signature, 0) + 1
    total = sum(counts.values())
    entropy = 0.0
    for count in counts.values():
        probability = count / total
        if probability > 0:
            entropy -= probability * math.log(probability, 2)
    return entropy


def generate_hypotheses(
    analyses: Sequence[RunAnalysis],
    target_constant: Optional[str],
    contradiction_tracker: ContradictionTracker,
) -> List[Hypothesis]:
    hypotheses: List[Hypothesis] = []
    convergence_clusters = cluster_by_convergence(analyses)
    target_value = TARGET_APPROXIMATIONS.get(target_constant or "", None)
    total_runs = max(1, len(analyses))
    hypothesis_index = 1

    for value, members in convergence_clusters:
        psi_signatures = [analysis.psi_pattern for analysis in members]
        entropy = compute_entropy(psi_signatures)
        support_runs = [analysis.run_id for analysis in members]
        contradiction_runs: List[str] = []
        for analysis in members:
            if analysis.ratio_variation > 0.25:
                contradiction_runs.append(analysis.run_id)
                contradiction_tracker.register(f"H{hypothesis_index}", analysis.run_id)
        coverage = len(members) / total_runs
        stability_factor = 1.0
        if members:
            local_variation = max(member.ratio_variation for member in members)
            stability_factor = max(0.0, 1.0 - min(1.0, local_variation))
        contradiction_penalty = max(0.1, 1.0 - (len(contradiction_runs) / max(1, len(members))))
        confidence = 100.0 * coverage * stability_factor * contradiction_penalty
        representative = members[0]
        descriptor = _describe_convergence(representative, value, target_value)
        hypothesis_id = f"H{hypothesis_index}"
        hypothesis_index += 1
        hypotheses.append(
            Hypothesis(
                identifier=hypothesis_id,
                description=descriptor,
                cluster_key=f"convergence::{value:.6f}",
                support_runs=support_runs,
                contradiction_runs=contradiction_runs,
                confidence=confidence,
                coverage=coverage,
                entropy=entropy,
                psi_signature=representative.psi_pattern,
                koppa_mode=representative.koppa_mode,
                engine_mode=representative.engine_mode,
                convergence_value=value,
            )
        )

    if not hypotheses:
        fallback = Hypothesis(
            identifier="H1",
            description=(
                "Insufficient convergence clusters detected. Existing runs either lack data "
                "or diverge beyond the tolerance window."
            ),
            cluster_key="convergence::none",
            support_runs=[analysis.run_id for analysis in analyses],
            contradiction_runs=[],
            confidence=0.0,
            coverage=0.0,
            entropy=0.0,
            psi_signature="none",
            koppa_mode="unspecified",
            engine_mode="unspecified",
            convergence_value=None,
        )
        hypotheses.append(fallback)
    return hypotheses


def _describe_convergence(analysis: RunAnalysis, value: float, target_value: Optional[float]) -> str:
    parts = []
    config_pieces = []
    if analysis.engine_mode != "unspecified":
        config_pieces.append(f"Engine: {analysis.engine_mode}")
    if analysis.psi_mode != "unspecified":
        config_pieces.append(f"ψ mode: {analysis.psi_mode}")
    if analysis.koppa_mode != "unspecified":
        config_pieces.append(f"κ mode: {analysis.koppa_mode}")
    if analysis.koppa_trigger != "unspecified":
        config_pieces.append(f"κ trigger: {analysis.koppa_trigger}")
    if config_pieces:
        parts.append("Configuration → " + ", ".join(config_pieces))

    ratio_line = f"Observed convergence window around {value:.6f}."
    parts.append(ratio_line)

    if target_value is not None:
        delta = abs(value - target_value)
        parts.append(f"Δ vs target ≈ {delta:.6e}.")

    parts.append(f"ψ rhythm signature: {analysis.psi_pattern} (density {analysis.psi_density:.3f}).")
    if analysis.rho_pattern != "none":
        parts.append(f"ρ flags recorded at {analysis.rho_pattern}.")
    if analysis.mu_zero_count:
        parts.append(f"μ = 0 excursions: {analysis.mu_zero_count}.")
    parts.append(f"Engine symmetry classified as {analysis.engine_symmetry}.")

    return " ".join(parts)


def build_pattern_map(analyses: Sequence[RunAnalysis]) -> Dict[str, Dict[str, object]]:
    pattern_map: Dict[str, Dict[str, object]] = {}
    for analysis in analyses:
        pattern_map[analysis.run_id] = {
            "convergence_value": analysis.convergence_value,
            "psi_pattern": analysis.psi_pattern,
            "rho_pattern": analysis.rho_pattern,
            "psi_density": analysis.psi_density,
            "rho_density": analysis.rho_density,
            "mu_zero_count": analysis.mu_zero_count,
            "engine_symmetry": analysis.engine_symmetry,
            "engine_mode": analysis.engine_mode,
            "psi_mode": analysis.psi_mode,
            "koppa_mode": analysis.koppa_mode,
            "koppa_trigger": analysis.koppa_trigger,
            "triple_psi_count": analysis.triple_psi_count,
            "initial_upsilon": analysis.initial_upsilon.to_text() if analysis.initial_upsilon else None,
            "initial_beta": analysis.initial_beta.to_text() if analysis.initial_beta else None,
        }
    return pattern_map


def write_theories_markdown(path: str, hypotheses: Sequence[Hypothesis]) -> None:
    with open(path, "w", encoding="utf-8") as handle:
        handle.write("# THEORIST GPT Hypotheses\n\n")
        for hypothesis in hypotheses:
            handle.write(f"## [{hypothesis.identifier}]\n")
            handle.write(f"Cluster: {hypothesis.cluster_key}\n\n")
            handle.write(hypothesis.description + "\n\n")
            handle.write(
                f"Confidence: {hypothesis.confidence:.2f}% | Coverage: {hypothesis.coverage:.2f} | "
                f"Entropy: {hypothesis.entropy:.3f}\n"
            )
            handle.write(
                f"Support runs ({len(hypothesis.support_runs)}): {', '.join(hypothesis.support_runs) or 'None'}\n"
            )
            handle.write(
                f"Contradictions ({len(hypothesis.contradiction_runs)}): "
                f"{', '.join(hypothesis.contradiction_runs) or 'None'}\n\n"
            )


def write_confidence_scores(path: str, hypotheses: Sequence[Hypothesis]) -> None:
    with open(path, "w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(
            [
                "hypothesis_id",
                "cluster_key",
                "confidence_percent",
                "coverage",
                "entropy_bits",
                "support_count",
                "contradiction_count",
                "psi_signature",
                "engine_mode",
                "koppa_mode",
                "convergence_value",
            ]
        )
        for hypothesis in hypotheses:
            writer.writerow(
                [
                    hypothesis.identifier,
                    hypothesis.cluster_key,
                    f"{hypothesis.confidence:.2f}",
                    f"{hypothesis.coverage:.3f}",
                    f"{hypothesis.entropy:.3f}",
                    len(hypothesis.support_runs),
                    len(hypothesis.contradiction_runs),
                    hypothesis.psi_signature,
                    hypothesis.engine_mode,
                    hypothesis.koppa_mode,
                    "" if hypothesis.convergence_value is None else f"{hypothesis.convergence_value:.6f}",
                ]
            )


def write_pattern_map(path: str, pattern_map: Dict[str, Dict[str, object]]) -> None:
    with open(path, "w", encoding="utf-8") as handle:
        json.dump(pattern_map, handle, indent=2)
        handle.write("\n")


def propose_test_suggestions(
    hypotheses: Sequence[Hypothesis],
    analyses: Sequence[RunAnalysis],
    limit: int,
) -> List[Dict[str, str]]:
    suggestions: List[Dict[str, str]] = []
    analysis_by_id = {analysis.run_id: analysis for analysis in analyses}
    ranked = sorted(hypotheses, key=lambda hyp: hyp.confidence, reverse=True)
    for hypothesis in ranked:
        if limit and len(suggestions) >= limit:
            break
        for run_id in hypothesis.support_runs:
            analysis = analysis_by_id.get(run_id)
            if not analysis:
                continue
            mutated_configs = _mutate_configs(analysis)
            for mutation in mutated_configs:
                suggestions.append(
                    {
                        "base_run": run_id,
                        "hypothesis_id": hypothesis.identifier,
                        "mutation": mutation,
                        "rationale": _suggestion_rationale(analysis, hypothesis),
                        "predicted_behavior": _prediction_text(analysis, hypothesis),
                        "novelty_score": f"{_novelty_score(mutation):.3f}",
                    }
                )
                if limit and len(suggestions) >= limit:
                    break
            if limit and len(suggestions) >= limit:
                break
    return suggestions


def _mutate_configs(analysis: RunAnalysis) -> List[str]:
    mutations: List[str] = []
    if analysis.initial_upsilon and analysis.initial_beta:
        base_u = analysis.initial_upsilon
        base_b = analysis.initial_beta
        deltas = [-2, -1, 1, 2]
        for delta in deltas:
            mutated_u = RationalValue(base_u.numerator + delta, base_u.denominator)
            mutated_b = RationalValue(base_b.numerator, base_b.denominator + delta)
            mutation = (
                f"upsilon={mutated_u.to_text()} | beta={mutated_b.to_text()} | "
                f"engine={analysis.engine_mode or 'unspecified'} | ψ={analysis.psi_mode or 'unspecified'}"
            )
            mutations.append(mutation)
    else:
        mutations.append("Adjust ψ toggle pattern while holding engine parameters constant.")
    return mutations


def _suggestion_rationale(analysis: RunAnalysis, hypothesis: Hypothesis) -> str:
    return (
        f"Extend {analysis.psi_pattern} rhythm; hypothesis {hypothesis.identifier} predicts "
        f"stability within convergence window {hypothesis.cluster_key}."
    )


def _prediction_text(analysis: RunAnalysis, hypothesis: Hypothesis) -> str:
    convergence = "undetermined" if hypothesis.convergence_value is None else f"{hypothesis.convergence_value:.6f}"
    return (
        f"Expect ψ density ≈ {analysis.psi_density:.3f} with convergence near {convergence}."
    )


def _novelty_score(mutation: str) -> float:
    digits = sum(ch.isdigit() for ch in mutation)
    unique_tokens = len({token for token in mutation.split() if token})
    return min(1.0, (digits + unique_tokens) / 25.0)


def write_suggestions_csv(path: str, rows: Sequence[Dict[str, str]]) -> None:
    headers = [
        "base_run",
        "hypothesis_id",
        "mutation",
        "rationale",
        "predicted_behavior",
        "novelty_score",
    ]
    with open(path, "w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=headers)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def main(argv: Optional[Sequence[str]] = None) -> int:
    args = parse_arguments(argv)

    default_paths = args.paths if args.paths else [
        os.path.join(os.getcwd(), "runs"),
        os.path.join(os.getcwd(), "tests"),
        os.path.join(os.getcwd(), "analyze"),
        os.path.join(os.getcwd(), "fingerprints"),
        os.path.join(os.getcwd(), "phase_maps"),
    ]
    include_root = bool(args.scan_all or not args.paths)
    containers = gather_run_containers(default_paths, include_root=include_root)
    analyses = [analyze_run(container) for container in containers.values() if container.value_rows]

    if not analyses:
        sys.stderr.write("No TRTS data located. Ensure runs have been generated.\n")
        return 1

    contradiction_tracker = ContradictionTracker()
    hypotheses = generate_hypotheses(analyses, args.target, contradiction_tracker)

    if args.limit is not None:
        hypotheses = hypotheses[: args.limit]

    output_dir = args.output_dir
    os.makedirs(output_dir, exist_ok=True)

    pattern_map = build_pattern_map(analyses)
    write_pattern_map(os.path.join(output_dir, "pattern_map.json"), pattern_map)

    write_confidence_scores(os.path.join(output_dir, "confidence_scores.csv"), hypotheses)

    if args.report or not args.suggest_tests:
        write_theories_markdown(os.path.join(output_dir, "theories.md"), hypotheses)

    if args.suggest_tests or not args.report:
        suggestions = propose_test_suggestions(hypotheses, analyses, limit=args.limit or 12)
        write_suggestions_csv(os.path.join(output_dir, "suggestions.csv"), suggestions)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
