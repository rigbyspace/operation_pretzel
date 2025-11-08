#include "state.h"

void state_init(TRTS_State *state) {
    mpq_init(state->upsilon);
    mpq_init(state->beta);
    mpq_init(state->koppa);
    mpq_init(state->epsilon);
    mpq_init(state->phi);
    mpq_set_ui(state->upsilon, 0, 1);
    mpq_set_ui(state->beta, 0, 1);
    mpq_set_ui(state->koppa, 0, 1);
    mpq_set_ui(state->epsilon, 0, 1);
    mpq_set_ui(state->phi, 0, 1);
    state->rho_pending = false;
    state->rho_latched = false;
    state->psi_recent = false;
}

void state_clear(TRTS_State *state) {
    mpq_clear(state->upsilon);
    mpq_clear(state->beta);
    mpq_clear(state->koppa);
    mpq_clear(state->epsilon);
    mpq_clear(state->phi);
}

void state_reset(TRTS_State *state, const Config *config) {
    mpq_set(state->upsilon, config->initial_upsilon);
    mpq_set(state->beta, config->initial_beta);
    mpq_set(state->koppa, config->initial_koppa);
    mpq_set(state->epsilon, config->initial_upsilon);
    mpq_set(state->phi, config->initial_beta);
    state->rho_pending = false;
    state->rho_latched = false;
    state->psi_recent = false;
}
