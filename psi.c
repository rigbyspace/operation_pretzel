#include "psi.h"

#include <gmp.h>

bool psi_transform(const Config *config, TRTS_State *state) {
    state->psi_triple_recent = false;

    if (config->triple_psi_mode) {
        if (mpq_sgn(state->upsilon) == 0 || mpq_sgn(state->beta) == 0 || mpq_sgn(state->koppa) == 0) {
            state->psi_recent = false;
            return false;
        }

        mpq_t new_upsilon;
        mpq_t new_beta;
        mpq_t new_koppa;
        mpq_init(new_upsilon);
        mpq_init(new_beta);
        mpq_init(new_koppa);

        mpq_div(new_upsilon, state->beta, state->upsilon);
        mpq_canonicalize(new_upsilon);

        mpq_div(new_beta, state->koppa, state->beta);
        mpq_canonicalize(new_beta);

        mpq_div(new_koppa, state->upsilon, state->koppa);
        mpq_canonicalize(new_koppa);

        mpq_set(state->phi, state->upsilon);
        mpq_set(state->upsilon, new_upsilon);
        mpq_set(state->beta, new_beta);
        mpq_set(state->koppa, new_koppa);

        mpq_clear(new_upsilon);
        mpq_clear(new_beta);
        mpq_clear(new_koppa);

        state->psi_recent = true;
        state->rho_pending = false;
        state->rho_latched = false;
        state->psi_triple_recent = true;
        return true;
    }

    mpz_srcptr ups_num = mpq_numref(state->upsilon);
    mpz_srcptr beta_num = mpq_numref(state->beta);

    if (mpz_sgn(ups_num) == 0 || mpz_sgn(beta_num) == 0) {
        state->psi_recent = false;
        return false;
    }

    mpq_t new_upsilon;
    mpq_t new_beta;
    mpq_init(new_upsilon);
    mpq_init(new_beta);

    mpq_set_num(new_upsilon, mpq_denref(state->beta));
    mpq_set_den(new_upsilon, mpq_numref(state->upsilon));
    mpq_canonicalize(new_upsilon);

    mpq_set_num(new_beta, mpq_denref(state->upsilon));
    mpq_set_den(new_beta, mpq_numref(state->beta));
    mpq_canonicalize(new_beta);

    mpq_set(state->phi, state->upsilon);
    mpq_set(state->upsilon, new_upsilon);
    mpq_set(state->beta, new_beta);

    mpq_clear(new_upsilon);
    mpq_clear(new_beta);

    state->psi_recent = true;
    state->rho_pending = false;
    state->rho_latched = false;

    return true;
}
