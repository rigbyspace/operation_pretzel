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
                      bool psi_fired, bool mu_zero, bool forced_emission) {
    fprintf(events_file, "%zu,%d,%c,%d,%d,%d,%d\n", tick, microtick, phase, rho_event ? 1 : 0,
            psi_fired ? 1 : 0, mu_zero ? 1 : 0, forced_emission ? 1 : 0);
}

static void log_values(FILE *values_file, size_t tick, int microtick, const TRTS_State *state) {
    gmp_fprintf(values_file, "%zu,%d,%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n", tick, microtick,
                mpq_numref(state->upsilon), mpq_denref(state->upsilon), mpq_numref(state->beta),
                mpq_denref(state->beta), mpq_numref(state->koppa), mpq_denref(state->koppa));
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

    fprintf(events_file, "tick,mt,event_type,rho_event,psi_fired,mu_zero,forced_emission\n");
    fprintf(values_file, "tick,mt,upsilon_num,upsilon_den,beta_num,beta_den,koppa_num,koppa_den\n");

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

            switch (phase) {
            case 'E': {
                mpq_set(state.epsilon, state.upsilon);
                bool engine_ok = engine_step(config, &state);
                (void)engine_ok;

                mpq_srcptr prime_target = (config->prime_target == PRIME_ON_MEMORY) ? state.epsilon
                                                                                   : state.upsilon;
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
                    psi_fired = psi_transform(&state);
                }
                break;
            }
            case 'M': {
                mu_zero = mpq_sgn(state.beta) == 0;
                bool request_psi = should_fire_psi(config, &state, true);
                if (request_psi) {
                    psi_fired = psi_transform(&state);
                } else {
                    state.psi_recent = false;
                }

                koppa_accrue(config, &state, psi_fired, true);
                state.rho_latched = false;
                break;
            }
            case 'R': {
                koppa_accrue(config, &state, false, false);
                state.psi_recent = false;
                state.rho_latched = false;
                break;
            }
            default:
                break;
            }

            log_event(events_file, tick, microtick, phase, rho_event, psi_fired, mu_zero,
                      forced_emission);
            log_values(values_file, tick, microtick, &state);
        }
    }

    state_clear(&state);
    fclose(events_file);
    fclose(values_file);
}
