#include "engine.h"

#include <gmp.h>

static bool apply_track_mode(EngineTrackMode mode, mpq_t result, mpq_srcptr current,
                             mpq_srcptr counterpart, mpq_srcptr koppa, mpq_t workspace) {
    switch (mode) {
    case ENGINE_TRACK_ADD:
        mpq_add(result, current, counterpart);
        mpq_add(result, result, koppa);
        mpq_canonicalize(result);
        return true;
    case ENGINE_TRACK_MULTI:
        mpq_add(workspace, counterpart, koppa);
        mpq_mul(result, current, workspace);
        mpq_canonicalize(result);
        return true;
    case ENGINE_TRACK_SLIDE:
        if (mpq_sgn(koppa) == 0) {
            return false;
        }
        mpq_add(workspace, current, counterpart);
        mpq_div(result, workspace, koppa);
        mpq_canonicalize(result);
        return true;
    }
    return false;
}

bool engine_step(const Config *config, TRTS_State *state) {
    mpq_t ups_before;
    mpq_t beta_before;
    mpq_init(ups_before);
    mpq_init(beta_before);
    mpq_set(ups_before, state->upsilon);
    mpq_set(beta_before, state->beta);

    bool success = true;

    if (config->dual_track_mode) {
        mpq_t new_upsilon;
        mpq_t new_beta;
        mpq_t workspace;
        mpq_init(new_upsilon);
        mpq_init(new_beta);
        mpq_init(workspace);

        bool ups_success = apply_track_mode(config->engine_upsilon, new_upsilon, state->upsilon,
                                            state->beta, state->koppa, workspace);
        bool beta_success = apply_track_mode(config->engine_beta, new_beta, state->beta,
                                             state->upsilon, state->koppa, workspace);
        success = ups_success && beta_success;

        if (success) {
            mpq_set(state->upsilon, new_upsilon);
            mpq_set(state->beta, new_beta);
        }

        mpq_clear(new_upsilon);
        mpq_clear(new_beta);
        mpq_clear(workspace);
        state->dual_engine_last_step = success;
    } else {
        mpq_t new_upsilon;
        mpq_t new_beta;
        mpq_t workspace;
        mpq_init(new_upsilon);
        mpq_init(new_beta);
        mpq_init(workspace);
        bool update_beta = false;

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
            } else {
                mpq_add(workspace, state->upsilon, state->beta);
                mpq_div(new_upsilon, workspace, state->koppa);
                mpq_canonicalize(new_upsilon);
            }
            break;
        case ENGINE_MODE_DELTA_ADD:
            mpq_sub(workspace, state->upsilon, state->previous_upsilon);
            mpq_add(new_upsilon, state->upsilon, workspace);
            mpq_canonicalize(new_upsilon);

            mpq_sub(new_beta, state->beta, state->previous_beta);
            mpq_add(new_beta, state->beta, new_beta);
            mpq_canonicalize(new_beta);
            update_beta = true;
            break;
        }

        if (success) {
            mpq_set(state->upsilon, new_upsilon);
            if (update_beta) {
                mpq_set(state->beta, new_beta);
            }
        }

        mpq_clear(new_upsilon);
        mpq_clear(new_beta);
        mpq_clear(workspace);
        state->dual_engine_last_step = false;
    }

    if (success) {
        mpq_set(state->previous_upsilon, ups_before);
        mpq_set(state->previous_beta, beta_before);
    }

    mpq_clear(ups_before);
    mpq_clear(beta_before);

    return success;
}
