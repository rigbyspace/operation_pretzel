#include "state.h"

void state_init(TRTS_State *state) {
    mpq_init(state->upsilon);
    mpq_init(state->beta);
    mpq_init(state->koppa);
    mpq_init(state->epsilon);
    mpq_init(state->phi);
    mpq_init(state->previous_upsilon);
    mpq_init(state->previous_beta);
    for (size_t i = 0; i < 4; ++i) {
        mpq_init(state->koppa_stack[i]);
        mpq_set_ui(state->koppa_stack[i], 0, 1);
    }
    mpq_init(state->koppa_sample);

    mpq_set_ui(state->upsilon, 0, 1);
    mpq_set_ui(state->beta, 0, 1);
    mpq_set_ui(state->koppa, 0, 1);
    mpq_set_ui(state->epsilon, 0, 1);
    mpq_set_ui(state->phi, 0, 1);
    mpq_set_ui(state->previous_upsilon, 0, 1);
    mpq_set_ui(state->previous_beta, 0, 1);
    mpq_set(state->koppa_sample, state->koppa);
    state->koppa_stack_size = 0;
    state->koppa_sample_index = -1;
    state->rho_pending = false;
    state->rho_latched = false;
    state->psi_recent = false;
    state->ratio_triggered_recent = false;
    state->psi_triple_recent = false;
    state->dual_engine_last_step = false;
}

void state_clear(TRTS_State *state) {
    mpq_clear(state->upsilon);
    mpq_clear(state->beta);
    mpq_clear(state->koppa);
    mpq_clear(state->epsilon);
    mpq_clear(state->phi);
    mpq_clear(state->previous_upsilon);
    mpq_clear(state->previous_beta);
    for (size_t i = 0; i < 4; ++i) {
        mpq_clear(state->koppa_stack[i]);
    }
    mpq_clear(state->koppa_sample);
}

void state_reset(TRTS_State *state, const Config *config) {
    mpq_set(state->upsilon, config->initial_upsilon);
    mpq_set(state->beta, config->initial_beta);
    mpq_set(state->koppa, config->initial_koppa);
    mpq_set(state->epsilon, config->initial_upsilon);
    mpq_set(state->phi, config->initial_beta);
    mpq_set(state->previous_upsilon, config->initial_upsilon);
    mpq_set(state->previous_beta, config->initial_beta);
    for (size_t i = 0; i < 4; ++i) {
        mpq_set_ui(state->koppa_stack[i], 0, 1);
    }
    mpq_set(state->koppa_sample, state->koppa);
    state->koppa_stack_size = 0;
    state->koppa_sample_index = -1;
    state->rho_pending = false;
    state->rho_latched = false;
    state->psi_recent = false;
    state->ratio_triggered_recent = false;
    state->psi_triple_recent = false;
    state->dual_engine_last_step = false;
}
