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
} TRTS_State;

void state_init(TRTS_State *state);
void state_clear(TRTS_State *state);
void state_reset(TRTS_State *state, const Config *config);

#endif // STATE_H
