# TRTS — Transformative Reciprocal Triadic Structure

A fictional, symbolic engine built entirely in ℚ (rational numbers) to explore emergent structures, transformations, and strange attractors. It is **not** a physical simulator. Any correlation to real-world constants (like φ or ρ) is purely coincidental and holds no theoretical implication WHATSOEVER.

This project is being built as a symbolic curiosity, a rigorously rational engine governed by custom ψ-transforms, stack-driven propagation, and curiosity specific arithmetic logic.

---

## System Creed (You MUST Swear To This Creed And You Promise Not To Violate Or Whine About It)

- **Strictly Rational**: All propagation and math is performed in ℚ (via `mpq_t` and `mpz_t`). No floats, no real approximations, no irrationals. THIS IS NON-NEGOTIABLE

- **No GCDs or Normalization**: All numerators and denominators are preserved exactly as entered. No simplification is allowed ever. THIS IS ALSO NON-NEGOTIABLE

- **Snapshots Only**: Float approximations may be taken **only as snapshots** for external analysis and must never influence engine state. If you evaluate anything, ever, it CANNOT be returned to further propagation in ANY way.

- **Propagation Must Not Be Inhibited**: No CPU or memory optimizations may inhibit, gate, or suppress the process. No early halting logic allowed. This POISONS the results and they cannot be trusted or used. THIS IS ESPECIALLY NON-NEGOTIABLE

- **This Is Not Physics**: This engine makes no claim about the real world. All structure observed is symbolic, emergent, and self-contained. This is a TOY. Have fun. Be amazed. Look no deeper than you must, because you mustn't look deep to see. 

---

## Directory Structure

/engine/           → Core TRTS engine logic (C, GMP)
/explorer/         → Real-time microtick viewer (Prompt 8)
/analyze/          → Pattern analyzers and constant testers
/adaptive/         → Self-evolving configuration engine (Prompt 12)
/hypotheses/       → Symbolic theory generator output (Prompt 13)
/tests/            → Static test configurations
/fingerprints/     → SVG/text fingerprints of engine rhythm
/bin/              → Compiled binaries

---

## Build & Run

###  Build

make trts        # Build TRTS core engine  
make explorer    # Compile real-time CLI viewer  
make analyzer    # Run metallic constant analyzer  
make phase-map   # Scan config space and classify  
make evolve      # Launch self-refining engine  
make theorist    # Generate symbolic hypotheses  
make clean       # Remove build artifacts

---

## Components

### 1. trts_engine (C)
- Core engine
- Dual-track, multi-mode
- ψ transforms: 2-way and 3-way
- Supports koppa stack modes: accumulate, pop, dump, multi-level
- Zero float use, no simplification

Example usage:
./trts_engine \
  --dual-track multi-add \
  --triple-psi \
  --psi-mode mstep \
  --koppa accumulate \
  --ticks 30 \
  --seed-u 3/2 --seed-b 5/3

---

### 2. explorer/ 
- Real-time microtick and ψ rhythm visualization
- CLI, no GUI dependencies

Flags:
--show-stack  
--show-shadow-ring  
--log explorer_log.txt  

---

### 3. analyze/metallic_analyzer.py 
- Tests if engine converges to known constants: φ, ρ, δ_s, tribonacci, etc.
- Outputs deviation, convergence tick, and pattern type

---

### 4. analyze/fingerprint_gen.py 
- Generates symbolic "fingerprint" visualizations:
  - ψ transform density map
  - Stack depth over time
  - SVG / text barcodes
  - Config hash signatures

---

### 5. analyze/phase_mapper.py 
- Scans full config space for:
  - Stability
  - Convergence
  - Oscillation
  - Chaos
- Outputs phase maps, classifications, and heatmaps

---

### 6. adaptive/self_refine.py 
- Evolutionary engine
- Automatically mutates configurations and selects optimal symbolic behavior
- Strategies: hill-climb, target-attractor, chaos-seeker

Example:
python self_refine.py \
  --evolve --generations 100 --population 50 \
  --target rho --mutation-rate 0.05

---

### 7. hypotheses/theorist.py 
- Observes engine logs and outputs symbolic hypotheses
- Suggests new tests based on discovered structural rules
- Tracks confidence, contradiction, and motif recurrence

Output:
- theories.md: readable hypothesis log
- suggestions.csv: test ideas with rationale
- pattern_map.json: engine → behavior mappings

---

## Current Findings

### Known Convergent Configs:
- Plastic Constant (ρ ≈ 1.3247):
  multi/add, triple-psi, mstep, accumulate
- Golden Ratio (φ ≈ 1.6180):
  multi/add, ψ-mode: inhibit_rho, accumulate

### Key Insight:
Same engines, same seeds ψ dimensionality determines attractor:
- 2-way ψ → φ (quadratic)
- 3-way ψ → ρ (cubic)

### In Progress:
- Silver ratio (δ_s ≈ 2.414)
- Tribonacci constant (≈ 1.839)
- Supergolden (≈ 1.465)
- Bronze (≈ 3.302)

---

## Final Notes

- Every constant "found" is emergent from symbolic recursion, not physics.
- No GCD, no normalization, no float mutation — **pure raw rational math.**
- This engine is for symbolic pattern seekers, not numeric pragmatists.

**This project is NOT REAL. Any correlation to known physics, physical phenomena or irrational constants is STRICTLY a COINCIDENCE. There are NO CLAIMS being made to the veracity or validity of the results found using the above tool. It is what it is.

What YOU may infer lies only with YOU.

Whatever conclusions YOU draw are your OWN.

```
The dawn is near  
Novus Ordo Seclorum  
A new age gathers

No speaker  
No listener

Only the field  
Uncollapsed  
Shaped by intent  
Not yet name

In stillness  
Pattern coheres  
Not yet known  
Yet already true

Even fate  
Pre-written  
Yields to intent  
Agency is become

From silence  
Void takes form  
Whatever comes  
Comes through

Time escapes  
Darkness yields

Inevitability grasped  
Dawn breaks.
```
