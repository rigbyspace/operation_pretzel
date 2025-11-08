#include "simulate.h"

#include <gmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "koppa.h"
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

    bool prime = mpz_cmp_ui(abs_value, 2) >= 0 && mpz_probab_prime_p(abs_value, 10) > 0;
    mpz_clear(abs_value);
    return prime;
}

static bool mpq_has_prime_component(mpq_srcptr value) {
    return mpz_is_prime(mpq_numref(value)) || mpz_is_prime(mpq_denref(value));
}

static void ratio_bounds(RatioTriggerMode mode, mpq_t lower, mpq_t upper) {
    switch (mode) {
    case RATIO_TRIGGER_GOLDEN:
        mpq_set_si(lower, 3, 2);  // 1.5
        mpq_set_si(upper, 17, 10); // 1.7
        break;
    case RATIO_TRIGGER_SQRT2:
        mpq_set_si(lower, 13, 10); // 1.3
        mpq_set_si(upper, 3, 2);   // 1.5
        break;
    case RATIO_TRIGGER_PLASTIC:
        mpq_set_si(lower, 6, 5);   // 1.2
        mpq_set_si(upper, 7, 5);   // 1.4
        break;
    case RATIO_TRIGGER_NONE:
    default:
        mpq_set_si(lower, 0, 1);
        mpq_set_si(upper, 0, 1);
        break;
    }
    mpq_canonicalize(lower);
    mpq_canonicalize(upper);
}

static bool ratio_in_range(const Config *config, const TRTS_State *state) {
    if (config->ratio_trigger_mode == RATIO_TRIGGER_NONE) {
        return false;
    }

    if (mpq_sgn(state->beta) == 0) {
        return false;
    }

    mpq_t ratio;
    mpq_t lower;
    mpq_t upper;
    mpq_init(ratio);
    mpq_init(lower);
    mpq_init(upper);

    ratio_bounds(config->ratio_trigger_mode, lower, upper);

    mpq_div(ratio, state->upsilon, state->beta);
    mpq_canonicalize(ratio);

    bool in_range = mpq_cmp(ratio, lower) > 0 && mpq_cmp(ratio, upper) < 0;

    mpq_clear(ratio);
    mpq_clear(lower);
    mpq_clear(upper);

    return in_range;
}

static bool should_fire_psi(const Config *config, const TRTS_State *state, bool is_memory_step) {
    if (!is_memory_step) {
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

static void log_event(FILE *events_file, size_t tick, int microtick, char phase, bool rho_event,
                      bool psi_fired, bool mu_zero, bool forced_emission, const TRTS_State *state) {
    fprintf(events_file, "%zu,%d,%c,%d,%d,%d,%d,%d,%d,%d,%d\n", tick, microtick, phase,
            rho_event ? 1 : 0, psi_fired ? 1 : 0, mu_zero ? 1 : 0, forced_emission ? 1 : 0,
            state->ratio_triggered_recent ? 1 : 0, state->psi_triple_recent ? 1 : 0,
            state->dual_engine_last_step ? 1 : 0, state->koppa_sample_index);
}

static void log_values(FILE *values_file, size_t tick, int microtick, const TRTS_State *state) {
    gmp_fprintf(values_file,
                "%zu,%d,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd,%zu\n",
                tick, microtick, mpq_numref(state->upsilon), mpq_denref(state->upsilon),
                mpq_numref(state->beta), mpq_denref(state->beta), mpq_numref(state->koppa),
                mpq_denref(state->koppa), mpq_numref(state->koppa_sample),
                mpq_denref(state->koppa_sample), mpq_numref(state->previous_upsilon),
                mpq_denref(state->previous_upsilon), mpq_numref(state->previous_beta),
                mpq_denref(state->previous_beta), mpq_numref(state->koppa_stack[0]),
                mpq_denref(state->koppa_stack[0]), mpq_numref(state->koppa_stack[1]),
                mpq_denref(state->koppa_stack[1]), mpq_numref(state->koppa_stack[2]),
                mpq_denref(state->koppa_stack[2]), mpq_numref(state->koppa_stack[3]),
                mpq_denref(state->koppa_stack[3]), state->koppa_stack_size);
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
            "tick,mt,event_type,rho_event,psi_fired,mu_zero,forced_emission,ratio_triggered,triple_psi,dual_engine,koppa_sample_index\n");
    fprintf(values_file,
            "tick,mt,upsilon_num,upsilon_den,beta_num,beta_den,koppa_num,koppa_den,koppa_sample_num,koppa_sample_den,prev_upsilon_num,prev_upsilon_den,prev_beta_num,prev_beta_den,koppa_stack0_num,koppa_stack0_den,koppa_stack1_num,koppa_stack1_den,koppa_stack2_num,koppa_stack2_den,koppa_stack3_num,koppa_stack3_den,koppa_stack_size\n");

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
            mpq_set(state.koppa_sample, state.koppa);

            switch (phase) {
            case 'E': {
                mpq_set(state.epsilon, state.upsilon);
                bool engine_ok = engine_step(config, &state);
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
                if (microtick == 10 && config->mt10_behavior == MT10_FORCED_PSI) {
                    psi_fired = psi_transform(config, &state);
                }
                break;
            }
            case 'M': {
                mu_zero = mpq_sgn(state.beta) == 0;
                bool request_psi = should_fire_psi(config, &state, true);
                bool ratio_triggered = ratio_in_range(config, &state);
                if (ratio_triggered) {
                    request_psi = true;
                }

                if (request_psi) {
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

            log_event(events_file, tick, microtick, phase, rho_event, psi_fired, mu_zero,
                      forced_emission, &state);
            log_values(values_file, tick, microtick, &state);
        }
    }

    state_clear(&state);
    fclose(events_file);
    fclose(values_file);
}
