/*
==============================================
     TRTS SYSTEM CREED – RATIONAL ONLY
==============================================

- All propagation must remain strictly within the rational field ℚ.
- No operation may simplify, normalize, reduce, fit, scale, or apply GCD to any value.
- `mpq_canonicalize()` is strictly forbidden and must never be used.
- All propagation must use raw integer numerator/denominator tracking.
- Any evaluation to floating-point must be snapshot-only for analysis.
  These values must NEVER influence state, behavior, or propagation.
- Rational form must preserve its full historical tension; no compression.
- Zero-crossings, sign changes, and stack depth are all meaningful logic.
- Nothing shall "optimize" away the very thing we are trying to study.

Violation of these principles invalidates all results. There are no exceptions.

*/

#ifndef STATE_H
#define STATE_H

#include <gmp.h>
#include <stdbool.h>
#include <stddef.h>

#include "config.h"

typedef struct {
    mpq_t upsilon;
    mpq_t beta;
    mpq_t koppa;
    mpq_t epsilon;
    mpq_t phi;
    mpq_t previous_upsilon;
    mpq_t previous_beta;
    mpq_t delta_upsilon;
    mpq_t delta_beta;
    mpq_t triangle_phi_over_epsilon;
    mpq_t triangle_prev_over_phi;
    mpq_t triangle_epsilon_over_prev;
    mpq_t koppa_stack[4];
    size_t koppa_stack_size;
    mpq_t koppa_sample;
    int koppa_sample_index;
    bool rho_pending;
    bool rho_latched;
    bool psi_recent;
    bool ratio_triggered_recent;
    bool psi_triple_recent;
    bool dual_engine_last_step;
    bool ratio_threshold_recent;
    bool psi_strength_applied;
    bool sign_flip_polarity;
} TRTS_State;

void state_init(TRTS_State *state);
void state_clear(TRTS_State *state);
void state_reset(TRTS_State *state, const Config *config);

#endif // STATE_H
