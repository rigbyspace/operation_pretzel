#!/usr/bin/env python3
"""Visualize TRTS rational engine dynamics."""

import argparse
import json
from pathlib import Path
from typing import Dict, Iterable, List, Tuple

import matplotlib.pyplot as plt
import numpy as np


RATIO_WINDOWS: List[Tuple[str, Tuple[float, float]]] = [
    ("plastic", (1.2, 1.4)),
    ("sqrt2", (1.3, 1.5)),
    ("golden", (1.5, 1.7)),
]


class DataRow(dict):
    """Dictionary subclass for row data with attribute access."""

    def __getattr__(self, item):
        try:
            return self[item]
        except KeyError as exc:
            raise AttributeError(item) from exc


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Visualize TRTS simulation outputs.")
    parser.add_argument("values_csv", type=Path, help="Path to values.csv from a TRTS run")
    parser.add_argument("events_csv", type=Path, help="Path to events.csv from a TRTS run")
    parser.add_argument(
        "--config",
        type=Path,
        help="Optional JSON file containing seed and configuration metadata.",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("."),
        help="Directory where plots will be written.",
    )
    return parser.parse_args()


def load_config(config_path: Path | None) -> Dict[str, str]:
    if config_path is None:
        return {}
    try:
        with config_path.open("r", encoding="utf-8") as handle:
            data = json.load(handle)
    except FileNotFoundError:
        print(f"Config file {config_path} not found; continuing without config metadata.")
        return {}
    except json.JSONDecodeError as exc:
        print(f"Failed to parse config file {config_path}: {exc}; continuing without config metadata.")
        return {}

    return {str(key): value for key, value in data.items()}


def _parse_bool(value: str) -> bool:
    return str(value).strip().lower() in {"1", "true", "t", "yes", "y"}


def load_values(values_csv: Path) -> List[DataRow]:
    import csv

    required_columns = {
        "tick",
        "mt",
        "upsilon_num",
        "upsilon_den",
        "beta_num",
        "beta_den",
        "koppa_num",
        "koppa_den",
    }

    rows: List[DataRow] = []
    with values_csv.open("r", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        missing = required_columns - set(reader.fieldnames or [])
        if missing:
            raise ValueError(f"values.csv missing required columns: {missing}")
        for raw in reader:
            row = DataRow({})
            for key in required_columns:
                row[key] = int(raw[key])
            row["tick_mt"] = row["tick"] + row["mt"] / 10.0
            row["upsilon_float"] = _safe_divide(row["upsilon_num"], row["upsilon_den"])
            row["beta_float"] = _safe_divide(row["beta_num"], row["beta_den"])
            row["koppa_float"] = _safe_divide(row["koppa_num"], row["koppa_den"])
            row["ratio_float"] = _safe_divide(row["upsilon_float"], row["beta_float"])
            rows.append(row)
    return rows


def load_events(events_csv: Path) -> List[DataRow]:
    import csv

    required_columns = {
        "tick",
        "mt",
        "event_type",
        "rho_event",
        "psi_fired",
        "mu_zero",
        "forced_emission",
    }

    rows: List[DataRow] = []
    with events_csv.open("r", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        missing = required_columns - set(reader.fieldnames or [])
        if missing:
            raise ValueError(f"events.csv missing required columns: {missing}")
        for raw in reader:
            row = DataRow({})
            row["tick"] = int(raw["tick"])
            row["mt"] = int(raw["mt"])
            row["event_type"] = raw["event_type"]
            row["rho_event"] = _parse_bool(raw["rho_event"])
            row["psi_fired"] = _parse_bool(raw["psi_fired"])
            row["mu_zero"] = _parse_bool(raw["mu_zero"])
            row["forced_emission"] = _parse_bool(raw["forced_emission"])
            row["tick_mt"] = row["tick"] + row["mt"] / 10.0
            rows.append(row)
    return rows


def _safe_divide(num: float, den: float) -> float:
    if den == 0:
        return float("nan")
    return num / den


def merge_data(values: List[DataRow], events: List[DataRow]) -> List[DataRow]:
    event_map = {(row["tick"], row["mt"]): row for row in events}
    merged: List[DataRow] = []
    for val in values:
        combined = DataRow(val.copy())
        event = event_map.get((val["tick"], val["mt"]))
        if event:
            combined["rho_event"] = event["rho_event"]
            combined["psi_fired"] = event["psi_fired"]
            combined["mu_zero"] = event["mu_zero"]
            combined["forced_emission"] = event["forced_emission"]
        else:
            combined["rho_event"] = False
            combined["psi_fired"] = False
            combined["mu_zero"] = False
            combined["forced_emission"] = False
        merged.append(combined)
    return merged


def _extract(values: Iterable[DataRow], key: str) -> np.ndarray:
    return np.array([row[key] for row in values], dtype=float)


def create_state_plot(rows: List[DataRow], config: Dict[str, str], output_path: Path) -> None:
    tick_mt = _extract(rows, "tick_mt")
    upsilon = _extract(rows, "upsilon_float")
    beta = _extract(rows, "beta_float")
    koppa = _extract(rows, "koppa_float")

    psi_mask = np.array([row["psi_fired"] for row in rows], dtype=bool)
    rho_mask = np.array([row["rho_event"] for row in rows], dtype=bool)
    mu_mask = np.array([row["mu_zero"] for row in rows], dtype=bool)

    fig, ax = plt.subplots(figsize=(12, 6))
    ax.plot(tick_mt, upsilon, label="υ (upsilon)", linewidth=2)
    ax.plot(tick_mt, beta, label="β (beta)", linewidth=2)
    ax.plot(tick_mt, koppa, label="κ (koppa)", linewidth=2)

    finite_values = np.concatenate([upsilon[np.isfinite(upsilon)], beta[np.isfinite(beta)], koppa[np.isfinite(koppa)]])
    if finite_values.size == 0:
        ymin, ymax = -1.0, 1.0
    else:
        ymin, ymax = float(np.min(finite_values)), float(np.max(finite_values))
    if ymin == ymax:
        ymin -= 1.0
        ymax += 1.0
    yspan = ymax - ymin
    psi_y = ymax + 0.1 * yspan
    rho_y = ymax + 0.2 * yspan
    mu_y = ymin - 0.1 * yspan

    ax.scatter(tick_mt[psi_mask], np.full(psi_mask.sum(), psi_y), c="red", marker="o", label="ψ fired")
    ax.scatter(tick_mt[rho_mask], np.full(rho_mask.sum(), rho_y), c="green", marker="^", label="ρ detected")
    ax.scatter(tick_mt[mu_mask], np.full(mu_mask.sum(), mu_y), c="black", marker="x", label="μ = 0")

    ax.set_xlabel("Tick.microtick")
    ax.set_ylabel("Value")
    ax.set_title("TRTS Rational Dynamics")
    ax.legend(loc="best")
    annotate_config(fig, config)
    fig.tight_layout()
    fig.savefig(output_path, dpi=300)
    plt.close(fig)


def annotate_config(fig: plt.Figure, config: Dict[str, str]) -> None:
    if not config:
        return
    lines: List[str] = []
    seeds: List[str] = []
    if "upsilon_seed" in config:
        seeds.append(f"υ₀={config['upsilon_seed']}")
    if "beta_seed" in config:
        seeds.append(f"β₀={config['beta_seed']}")
    if seeds:
        lines.append(", ".join(seeds))

    config_keys = [
        "psi_mode",
        "engine_mode",
        "koppa_mode",
        "ratio_trigger",
        "ticks",
    ]
    config_line = ", ".join(f"{key}={config[key]}" for key in config_keys if key in config)
    if config_line:
        lines.append(config_line)

    if lines:
        fig.suptitle(" | ".join(lines), fontsize=10, y=0.98)


def create_ratio_plot(rows: List[DataRow], config: Dict[str, str], output_path: Path) -> Dict[str, int]:
    tick_mt = _extract(rows, "tick_mt")
    ratio = _extract(rows, "ratio_float")

    fig, ax = plt.subplots(figsize=(12, 6))
    ax.plot(tick_mt, ratio, label="υ / β", color="purple", linewidth=2)

    counts: Dict[str, int] = {}
    for index, (name, (lower, upper)) in enumerate(RATIO_WINDOWS):
        ax.axhspan(lower, upper, alpha=0.1, label=f"{name} window" if index == 0 else None)
        mask = np.isfinite(ratio) & (ratio >= lower) & (ratio <= upper)
        counts[name] = int(np.count_nonzero(mask))

    ax.set_xlabel("Tick.microtick")
    ax.set_ylabel("υ / β ratio")
    ax.set_title("Ratio Dynamics")
    ax.legend(loc="best")
    annotate_config(fig, config)
    fig.tight_layout()
    fig.savefig(output_path, dpi=300)
    plt.close(fig)
    return counts


def create_psi_heatmap(events: List[DataRow], config: Dict[str, str], output_path: Path) -> None:
    ticks = sorted({row["tick"] for row in events})
    mts = sorted({row["mt"] for row in events})
    tick_index = {tick: idx for idx, tick in enumerate(ticks)}
    mt_index = {mt: idx for idx, mt in enumerate(mts)}

    heatmap = np.zeros((len(mts), len(ticks)))
    forced_points: List[Tuple[int, int]] = []

    for row in events:
        r = mt_index[row["mt"]]
        c = tick_index[row["tick"]]
        heatmap[r, c] = 1 if row["psi_fired"] else heatmap[r, c]
        if row["psi_fired"] and row["forced_emission"]:
            forced_points.append((r, c))

    fig, ax = plt.subplots(figsize=(12, 6))
    cax = ax.imshow(heatmap, aspect="auto", origin="lower", cmap="Reds")

    ax.set_xticks(range(len(ticks)))
    ax.set_xticklabels(ticks)
    ax.set_yticks(range(len(mts)))
    ax.set_yticklabels(mts)
    ax.set_xlabel("Tick")
    ax.set_ylabel("Microtick")
    ax.set_title("ψ Firing Heatmap")

    if forced_points:
        ys, xs = zip(*forced_points)
        ax.scatter(xs, ys, marker="*", c="blue", label="Forced ψ")
        ax.legend(loc="best")

    fig.colorbar(cax, ax=ax, label="ψ fired")
    annotate_config(fig, config)
    fig.tight_layout()
    fig.savefig(output_path, dpi=300)
    plt.close(fig)


def summarize(events: List[DataRow], ratio_counts: Dict[str, int]) -> None:
    total_psi = sum(1 for row in events if row["psi_fired"])
    total_mu = sum(1 for row in events if row["mu_zero"])
    print(f"Total ψ fired: {total_psi}")
    print(f"Total μ = 0 events: {total_mu}")
    for name in sorted(ratio_counts):
        print(f"Ratio points in {name} window: {ratio_counts[name]}")


def main() -> None:
    args = parse_args()
    config = load_config(args.config)

    values = load_values(args.values_csv)
    events = load_events(args.events_csv)
    merged = merge_data(values, events)

    args.output_dir.mkdir(parents=True, exist_ok=True)

    create_state_plot(merged, config, args.output_dir / "plot_state_dynamics.png")
    ratio_counts = create_ratio_plot(merged, config, args.output_dir / "plot_ratio.png")
    create_psi_heatmap(events, config, args.output_dir / "plot_psi_heatmap.png")

    summarize(events, ratio_counts)


if __name__ == "__main__":
    main()
