/*
 * Local copy of configuration functions for the TRTS engine.  This file
 * extends the original implementation by initialising and clearing
 * additional configuration fields introduced for experimental features.
 */

#include "config.h"

void config_init(Config *config) {
    /* Base initialisation matching the upstream defaults */
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

    /* Enable core engine features by default as in upstream */
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
    config->enable_ratio_snapshot_logging = false;
    config->enable_feedback_oscillator = false;
    config->enable_fibonacci_gate = false;
    config->sign_flip_mode = SIGN_FLIP_NONE;
    config->koppa_wrap_threshold = 1000UL;

    /* Initialise custom ratio window.  Disabled by default. */
    config->enable_ratio_custom_range = false;
    rational_init(config->ratio_custom_lower);
    rational_init(config->ratio_custom_upper);
    rational_set_si(config->ratio_custom_lower, 1, 1);
    rational_set_si(config->ratio_custom_upper, 1, 1);

    /* Additional prime pattern triggers are disabled by default. */
    config->enable_twin_prime_trigger = false;
    config->enable_fibonacci_trigger = false;
    config->enable_perfect_power_trigger = false;

    /* Modular bound initialisation.  Set to zero meaning no bound. */
    mpz_init(config->modulus_bound);
    mpz_set_ui(config->modulus_bound, 0UL);
}

void config_clear(Config *config) {
    rational_clear(config->initial_upsilon);
    rational_clear(config->initial_beta);
    rational_clear(config->initial_koppa);

    /* Clear custom ratio window rationals */
    rational_clear(config->ratio_custom_lower);
    rational_clear(config->ratio_custom_upper);

    /* Clear modulus bound big integer */
    mpz_clear(config->modulus_bound);
}