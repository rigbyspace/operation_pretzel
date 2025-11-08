#include "psi.h"

#include <gmp.h>

bool psi_transform(TRTS_State *state) {
    mpz_srcptr ups_num = mpq_numref(state->upsilon);
    mpz_srcptr beta_num = mpq_numref(state->beta);

    if (mpz_sgn(ups_num) == 0 || mpz_sgn(beta_num) == 0) {
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
