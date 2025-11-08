#include "koppa.h"

#include <gmp.h>

static void koppa_dump(TRTS_State *state) {
    mpq_set_ui(state->koppa, 0, 1);
}

static void koppa_pop(TRTS_State *state) {
    mpq_set(state->koppa, state->epsilon);
}

static void koppa_accumulate(TRTS_State *state) {
    mpq_add(state->koppa, state->koppa, state->epsilon);
    mpq_canonicalize(state->koppa);
}

void koppa_accrue(const Config *config, TRTS_State *state, bool psi_fired, bool is_memory_step) {
    bool trigger = false;

    switch (config->koppa_trigger) {
    case KOPPA_ON_PSI:
        trigger = psi_fired;
        break;
    case KOPPA_ON_MU_AFTER_PSI:
        trigger = is_memory_step && !psi_fired && state->psi_recent;
        break;
    case KOPPA_ON_ALL_MU:
        trigger = is_memory_step;
        break;
    }

    if (!trigger) {
        if (!psi_fired && config->koppa_trigger != KOPPA_ON_ALL_MU) {
            state->psi_recent = state->psi_recent && (config->koppa_trigger == KOPPA_ON_MU_AFTER_PSI);
        }
        return;
    }

    switch (config->koppa_mode) {
    case KOPPA_MODE_DUMP:
        koppa_dump(state);
        break;
    case KOPPA_MODE_POP:
        koppa_pop(state);
        break;
    case KOPPA_MODE_ACCUMULATE:
        koppa_accumulate(state);
        break;
    }

    if (config->koppa_trigger == KOPPA_ON_MU_AFTER_PSI) {
        state->psi_recent = false;
    } else {
        state->psi_recent = psi_fired;
    }
}
