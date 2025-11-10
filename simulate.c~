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
#include <stdio.h>
#include "simulate.h"

#include <stdbool.h>
#include <stdlib.h>
#include <gmp.h>


#include "engine.h"
#include "koppa.h"
#include "rational.h"
#include "psi.h"
#include "state.h"

static char phase_for_microtick(int microtick) {
    switch (microtick) {
    case 1:
    case 4:
    case 7:
    case 10:
        return 'E';
    case 2:
    case 5:
    case 8:
    case 11:
        return 'M';
    case 3:
    case 6:
    case 9:
    default:
        return 'R';
    }
}

static bool mpz_is_prime(mpz_srcptr value) {
    if (mpz_sgn(value) == 0) {
        return false;
    }

    mpz_t abs_value;
    mpz_init(abs_value);
    mpz_abs(abs_value, value);

    bool prime = mpz_cmp_ui(abs_value, 2UL) >= 0 && mpz_probab_prime_p(abs_value, 10) > 0;
    mpz_clear(abs_value);
    return prime;
}

static bool mpq_has_prime_component(mpq_srcptr value) {
    return mpz_is_prime(mpq_numref(value)) || mpz_is_prime(mpq_denref(value));
}

static void ratio_bounds(RatioTriggerMode mode, mpq_t lower, mpq_t upper) {
    switch (mode) {
    case RATIO_TRIGGER_GOLDEN:
        rational_set_si(lower, 3, 2);
        rational_set_si(upper, 17, 10);
        break;
    case RATIO_TRIGGER_SQRT2:
        rational_set_si(lower, 13, 10);
        rational_set_si(upper, 3, 2);
        break;
    case RATIO_TRIGGER_PLASTIC:
        rational_set_si(lower, 6, 5);
        rational_set_si(upper, 7, 5);
        break;
    case RATIO_TRIGGER_NONE:
    default:
        rational_set_si(lower, 0, 1);
        rational_set_si(upper, 0, 1);
        break;
    }
}

static bool ratio_in_range(const Config *config, const TRTS_State *state) {
    if (config->ratio_trigger_mode == RATIO_TRIGGER_NONE) {
        return false;
    }

    if (rational_is_zero(state->beta)) {
        return false;
    }

    mpq_t ratio;
    mpq_t lower;
    mpq_t upper;
    rational_init(ratio);
    rational_init(lower);
    rational_init(upper);

    ratio_bounds(config->ratio_trigger_mode, lower, upper);
    rational_div(ratio, state->upsilon, state->beta);

    bool in_range = mpq_cmp(ratio, lower) > 0 && mpq_cmp(ratio, upper) < 0;

    rational_clear(ratio);
    rational_clear(lower);
    rational_clear(upper);

    return in_range;
}

static bool ratio_threshold_outside(const Config *config, const TRTS_State *state) {
    if (!config->enable_ratio_threshold_psi) {
        return false;
    }

    if (rational_is_zero(state->beta)) {
        return false;
    }

    mpq_t ratio;
    rational_init(ratio);
    rational_div(ratio, state->upsilon, state->beta);

    double ratio_snapshot = mpq_get_d(ratio); // SNAPSHOT ONLY – DO NOT FEED BACK
    rational_clear(ratio);

    double magnitude = ratio_snapshot >= 0.0 ? ratio_snapshot : -ratio_snapshot;
    return magnitude < 0.5 || magnitude > 2.0;
}

static bool should_fire_psi(const Config *config, const TRTS_State *state, bool is_memory_step,
                            bool allow_stack) {
    if (!is_memory_step) {
        return false;
    }

    if (!allow_stack) {
        return false;
    }

    switch (config->psi_mode) {
    case PSI_MODE_MSTEP:
        return true;
    case PSI_MODE_RHO_ONLY:
        return state->rho_pending;
    case PSI_MODE_MSTEP_RHO:
        return true;
    case PSI_MODE_INHIBIT_RHO:
        return !state->rho_pending;
    }

    return false;
}

static bool stack_allows_psi(const Config *config, const TRTS_State *state) {
    if (!config->enable_stack_depth_modes) {
        return true;
    }
    return state->koppa_stack_size == 2 || state->koppa_stack_size == 4;
}

typedef struct {
    FILE *events_file;
    FILE *values_file;
} SimulationOutputs;

static void log_event(FILE *events_file, size_t tick, int microtick, char phase, bool rho_event,
                      bool psi_fired, bool mu_zero, bool forced_emission, const TRTS_State *state) {
    fprintf(events_file, "%zu,%d,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", tick, microtick, phase,
            rho_event ? 1 : 0, psi_fired ? 1 : 0, mu_zero ? 1 : 0, forced_emission ? 1 : 0,
            state->ratio_triggered_recent ? 1 : 0, state->psi_triple_recent ? 1 : 0,
            state->dual_engine_last_step ? 1 : 0, state->koppa_sample_index,
            state->ratio_threshold_recent ? 1 : 0, state->psi_strength_applied ? 1 : 0,
            state->sign_flip_polarity ? 1 : 0);
}

static void log_values(FILE *values_file, size_t tick, int microtick, const TRTS_State *state) {
    gmp_fprintf(values_file,
                "%zu,%d,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%zu,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n",
                tick, microtick, mpq_numref(state->upsilon), mpq_denref(state->upsilon),
                mpq_numref(state->beta), mpq_denref(state->beta), mpq_numref(state->koppa),
                mpq_denref(state->koppa), mpq_numref(state->koppa_sample),
                mpq_denref(state->koppa_sample), mpq_numref(state->previous_upsilon),
                mpq_denref(state->previous_upsilon), mpq_numref(state->previous_beta),
                mpq_denref(state->previous_beta), mpq_numref(state->koppa_stack[0]),
                mpq_denref(state->koppa_stack[0]), mpq_numref(state->koppa_stack[1]),
                mpq_denref(state->koppa_stack[1]), mpq_numref(state->koppa_stack[2]),
                mpq_denref(state->koppa_stack[2]), mpq_numref(state->koppa_stack[3]),
                mpq_denref(state->koppa_stack[3]), state->koppa_stack_size,
                mpq_numref(state->delta_upsilon), mpq_denref(state->delta_upsilon),
                mpq_numref(state->delta_beta), mpq_denref(state->delta_beta),
                mpq_numref(state->triangle_phi_over_epsilon),
                mpq_denref(state->triangle_phi_over_epsilon),
                mpq_numref(state->triangle_prev_over_phi),
                mpq_denref(state->triangle_prev_over_phi),
                mpq_numref(state->triangle_epsilon_over_prev),
                mpq_denref(state->triangle_epsilon_over_prev));
}

static void emit_outputs(const SimulationOutputs *outputs, size_t tick, int microtick, char phase,
                         bool rho_event, bool psi_fired, bool mu_zero, bool forced_emission,
                         const TRTS_State *state, SimulateObserver observer, void *user_data) {
    if (outputs) {
        if (outputs->events_file) {
            log_event(outputs->events_file, tick, microtick, phase, rho_event, psi_fired, mu_zero,
                      forced_emission, state);
        }
        if (outputs->values_file) {
            log_values(outputs->values_file, tick, microtick, state);
        }
    }

    if (observer) {
        observer(user_data, tick, microtick, phase, state, rho_event, psi_fired, mu_zero,
                 forced_emission);
    }
}

static void run_simulation(const Config *config, const SimulationOutputs *outputs,
                           SimulateObserver observer, void *user_data) {
    TRTS_State state;
    state_init(&state);
    state_reset(&state, config);

    for (size_t tick = 1; tick <= config->ticks; ++tick) {
        for (int microtick = 1; microtick <= 11; ++microtick) {
            char phase = phase_for_microtick(microtick);
            bool rho_event = false;
            bool psi_fired = false;
            bool mu_zero = false;
            bool forced_emission = false;

            state.ratio_triggered_recent = false;
            state.psi_triple_recent = false;
            state.dual_engine_last_step = false;
            state.koppa_sample_index = -1;
            rational_set(state.koppa_sample, state.koppa);
            state.ratio_threshold_recent = false;
            state.psi_strength_applied = false;

            switch (phase) {
            case 'E': {
                rational_set(state.epsilon, state.upsilon);
                bool engine_ok = engine_step(config, &state, microtick);
                (void)engine_ok;

                mpq_srcptr prime_target =
                    (config->prime_target == PRIME_ON_MEMORY) ? state.epsilon : state.upsilon;
                if (mpq_has_prime_component(prime_target)) {
                    state.rho_pending = true;
                    state.rho_latched = true;
                    rho_event = true;
                } else {
                    state.rho_pending = false;
                    state.rho_latched = false;
                }

                forced_emission = (microtick == 10);
                if (microtick == 10) {
                    mpq_srcptr prime_target =
                        (config->prime_target == PRIME_ON_MEMORY) ? state.epsilon : state.upsilon;
                    bool prime_event = mpq_has_prime_component(prime_target);
                    if (prime_event || config->mt10_behavior == MT10_FORCED_PSI) {
                         state.rho_pending = true;
                         state.rho_latched = true;
                 }
                 forced_emission = true;
}
                break;
            }
            case 'M': {
                mu_zero = rational_is_zero(state.beta);
                bool allow_stack = stack_allows_psi(config, &state);
                bool request_psi = should_fire_psi(config, &state, true, allow_stack);
                bool ratio_triggered = ratio_in_range(config, &state);
                if (ratio_triggered) {
                    request_psi = true;
                }

                bool ratio_threshold = ratio_threshold_outside(config, &state);
                if (ratio_threshold) {
                    request_psi = true;
                    state.ratio_threshold_recent = true;
                }

                if (request_psi && allow_stack) {
                    psi_fired = psi_transform(config, &state);
                } else {
                    state.psi_recent = false;
                }
                state.ratio_triggered_recent = ratio_triggered;

                koppa_accrue(config, &state, psi_fired, true, microtick);
                state.rho_latched = false;
                break;
            }
            case 'R': {
                koppa_accrue(config, &state, false, false, microtick);
                state.psi_recent = false;
                state.rho_latched = false;
                break;
            }
            default:
                break;
            }

            emit_outputs(outputs, tick, microtick, phase, rho_event, psi_fired, mu_zero,
                         forced_emission, &state, observer, user_data);
        }
    }

    state_clear(&state);
}

void simulate(const Config *config) {
    FILE *events_file = fopen("events.csv", "w");
    if (!events_file) {
        perror("events.csv");
        return;
    }
    FILE *values_file = fopen("values.csv", "w");
    if (!values_file) {
        perror("values.csv");
        fclose(events_file);
        return;
    }

    fprintf(events_file,
            "tick,mt,event_type,rho_event,psi_fired,mu_zero,forced_emission,ratio_triggered,triple_psi,dual_engine,koppa_sample_index,ratio_threshold,psi_strength,sign_flip\n");
    fprintf(values_file,
            "tick,mt,upsilon_num,upsilon_den,beta_num,beta_den,koppa_num,koppa_den,koppa_sample_num,koppa_sample_den,prev_upsilon_num,prev_upsilon_den,prev_beta_num,prev_beta_den,koppa_stack0_num,koppa_stack0_den,koppa_stack1_num,koppa_stack1_den,koppa_stack2_num,koppa_stack2_den,koppa_stack3_num,koppa_stack3_den,koppa_stack_size,delta_upsilon_num,delta_upsilon_den,delta_beta_num,delta_beta_den,triangle_phi_over_epsilon_num,triangle_phi_over_epsilon_den,triangle_prev_over_phi_num,triangle_prev_over_phi_den,triangle_epsilon_over_prev_num,triangle_epsilon_over_prev_den\n");

    SimulationOutputs outputs = {events_file, values_file};
    run_simulation(config, &outputs, NULL, NULL);

    fclose(events_file);
    fclose(values_file);
}

void simulate_stream(const Config *config, SimulateObserver observer, void *user_data) {
    run_simulation(config, NULL, observer, user_data);
}

