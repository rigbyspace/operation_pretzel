#!/usr/bin/env python3
"""Run TRTS simulation across major configuration combinations."""

import itertools
import subprocess
import time
from pathlib import Path

OUTPUT_DIR = Path("output")
TRTS_BINARY = Path("./trts")

ENGINE_MODES = ["add", "multi", "slide", "delta_add"]
PSI_MODES = ["mstep", "rho_only", "inhibit_rho"]
KOPPA_MODES = ["dump", "pop", "accumulate"]
RATIO_TRIGGERS = ["none", "golden", "sqrt2", "plastic"]

MT10_BEHAVIOR = "forced_psi"
TICKS = "20"
UPSILON_SEED = "3/5"
BETA_SEED = "5/7"

# Static configuration flags
KOPPA_TRIGGER = "on_all_mu"
PRIME_TARGET = "memory"
ENGINE_UPSILON = "add"
ENGINE_BETA = "add"
DUAL_TRACK_MODE = "false"
MULTI_LEVEL_KOPPA = "false"
TRIPLE_PSI_MODE = "false"
REVERSE_CAUSALITY_MODE = "false"


def ensure_output_dir() -> None:
    """Ensure the output directory exists."""
    OUTPUT_DIR.mkdir(exist_ok=True)


def build_command(psi_mode: str, engine_mode: str, koppa_mode: str, ratio_trigger: str) -> list[str]:
    """Construct the command list for the TRTS binary."""
    prefix = (
        f"psi_{psi_mode}__engine_{engine_mode}__"
        f"koppa_{koppa_mode}__ratio_{ratio_trigger}"
    )
    output_prefix = str(OUTPUT_DIR / prefix)

    cmd = [
        str(TRTS_BINARY),
        "--psi-mode",
        psi_mode,
        "--koppa-mode",
        koppa_mode,
        "--engine-mode",
        engine_mode,
        "--koppa-trigger",
        KOPPA_TRIGGER,
        "--prime-target",
        PRIME_TARGET,
        "--mt10-behavior",
        MT10_BEHAVIOR,
        "--ratio-trigger",
        ratio_trigger,
        "--dual-track-mode",
        DUAL_TRACK_MODE,
        "--engine-upsilon",
        ENGINE_UPSILON,
        "--engine-beta",
        ENGINE_BETA,
        "--multi-level-koppa",
        MULTI_LEVEL_KOPPA,
        "--triple-psi-mode",
        TRIPLE_PSI_MODE,
        "--reverse-causality-mode",
        REVERSE_CAUSALITY_MODE,
        "--ticks",
        TICKS,
        "--output-prefix",
        output_prefix,
        "--u",
        UPSILON_SEED,
        "--b",
        BETA_SEED,
    ]

    return cmd


def run_command(cmd: list[str]) -> subprocess.CompletedProcess:
    """Execute the TRTS command and return the completed process."""
    return subprocess.run(cmd, capture_output=True, text=True)


def main() -> None:
    ensure_output_dir()

    if not TRTS_BINARY.exists():
        raise FileNotFoundError(f"TRTS binary not found at {TRTS_BINARY}")

    combinations = list(itertools.product(PSI_MODES, ENGINE_MODES, KOPPA_MODES, RATIO_TRIGGERS))
    total_runs = len(combinations)
    successes: list[str] = []
    failures: list[tuple[str, int, str]] = []

    start_time = time.perf_counter()

    for psi_mode, engine_mode, koppa_mode, ratio_trigger in combinations:
        cmd = build_command(psi_mode, engine_mode, koppa_mode, ratio_trigger)
        combo_name = (
            f"psi={psi_mode}, engine={engine_mode}, "
            f"koppa={koppa_mode}, ratio={ratio_trigger}"
        )
        print(f"Running {combo_name}...")
        result = run_command(cmd)

        if result.returncode == 0:
            successes.append(combo_name)
        else:
            failures.append((combo_name, result.returncode, result.stderr.strip()))
            print(
                f"  FAILED (code {result.returncode}). "
                "See logs for details."
            )

    elapsed = time.perf_counter() - start_time

    print("\nRun summary:")
    print(f"  Total configurations: {total_runs}")
    print(f"  Successful runs     : {len(successes)}")
    print(f"  Failed runs         : {len(failures)}")
    print(f"  Total elapsed time  : {elapsed:.2f} seconds")

    if failures:
        print("\nFailures:")
        for combo_name, returncode, stderr in failures:
            print(f"- {combo_name} (exit code {returncode})")
            if stderr:
                print(f"  stderr: {stderr}")


if __name__ == "__main__":
    main()
