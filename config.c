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

#include "config.h"

void config_init(Config *config) {
    config->psi_mode = PSI_MODE_MSTEP;
    config->koppa_mode = KOPPA_MODE_DUMP;
    config->engine_mode = ENGINE_MODE_ADD;
    config->engine_upsilon = ENGINE_TRACK_ADD;
    config->engine_beta = ENGINE_TRACK_ADD;
    config->dual_track_mode = false;
    config->triple_psi_mode = false;
    config->multi_level_koppa = false;
    config->koppa_trigger = KOPPA_ON_PSI;
    config->prime_target = PRIME_ON_NEW_UPSILON;
    config->mt10_behavior = MT10_FORCED_EMISSION_ONLY;
    config->ratio_trigger_mode = RATIO_TRIGGER_NONE;
    config->ticks = 0;
    rational_init(config->initial_upsilon);
    rational_init(config->initial_beta);
    rational_init(config->initial_koppa);
    rational_set_si(config->initial_upsilon, 0, 1);
    rational_set_si(config->initial_beta, 0, 1);
    rational_set_si(config->initial_koppa, 0, 1);

    config->enable_asymmetric_cascade = true;
    config->enable_conditional_triple_psi = true;
    config->enable_koppa_gated_engine = true;
    config->enable_delta_cross_propagation = true;
    config->enable_delta_koppa_offset = true;
    config->enable_ratio_threshold_psi = true;
    config->enable_stack_depth_modes = true;
    config->enable_epsilon_phi_triangle = true;
    config->enable_sign_flip = true;
    config->enable_modular_wrap = true;
    config->enable_psi_strength_parameter = true;
    config->enable_ratio_snapshot_logging = true;
    config->enable_feedback_oscillator = true;
    config->sign_flip_mode = SIGN_FLIP_ALTERNATE;
    config->koppa_wrap_threshold = 1000UL;
}

void config_clear(Config *config) {
    rational_clear(config->initial_upsilon);
    rational_clear(config->initial_beta);
    rational_clear(config->initial_koppa);
}
