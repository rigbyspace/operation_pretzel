#ifndef STATE_H
#define STATE_H

#include <gmp.h>
#include <stdbool.h>

#include "config.h"

typedef struct {
    mpq_t upsilon;
    mpq_t beta;
    mpq_t koppa;
    mpq_t epsilon;
    mpq_t phi;
    bool rho_pending;
    bool rho_latched;
    bool psi_recent;
} TRTS_State;

void state_init(TRTS_State *state);
void state_clear(TRTS_State *state);
void state_reset(TRTS_State *state, const Config *config);

#endif // STATE_H
