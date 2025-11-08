#include "engine.h"

#include <gmp.h>

bool engine_step(const Config *config, TRTS_State *state) {
    mpq_t new_upsilon;
    mpq_t workspace;
    mpq_init(new_upsilon);
    mpq_init(workspace);
    bool success = true;

    switch (config->engine_mode) {
    case ENGINE_MODE_ADD:
        mpq_add(new_upsilon, state->upsilon, state->beta);
        mpq_add(new_upsilon, new_upsilon, state->koppa);
        mpq_canonicalize(new_upsilon);
        break;
    case ENGINE_MODE_MULTI:
        mpq_add(workspace, state->beta, state->koppa);
        mpq_mul(new_upsilon, state->upsilon, workspace);
        mpq_canonicalize(new_upsilon);
        break;
    case ENGINE_MODE_SLIDE:
        if (mpq_sgn(state->koppa) == 0) {
            success = false;
            break;
        }
        mpq_add(workspace, state->upsilon, state->beta);
        mpq_div(new_upsilon, workspace, state->koppa);
        mpq_canonicalize(new_upsilon);
        break;
    }

    if (success) {
        mpq_set(state->upsilon, new_upsilon);
    }

    mpq_clear(new_upsilon);
    mpq_clear(workspace);

    return success;
}
