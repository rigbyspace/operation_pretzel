/*
==============================================
     TRTS SYSTEM CREED – RATIONAL ONLY
==============================================

- All propagation must remain strictly within the rational field ℚ.
- No operation may simplify, normalize, reduce, fit, scale, or apply GCD to any value.
- `mpq_canonicalize()` is strictly forbidden and must never be used.
- All propagation must use raw integer numerator/denominator tracking.
- Any evaluation to floating-point must be snapshot-only for analysis.
  These values must NEVER influence state, behavior, or propagation.
- Rational form must preserve its full historical tension; no compression.
- Zero-crossings, sign changes, and stack depth are all meaningful logic.
- Nothing shall "optimize" away the very thing we are trying to study.

Violation of these principles invalidates all results. There are no exceptions.

*/

#include <stdbool.h>
#include <stddef.h>
#include <gmp.h>

#include "psi.h"
#include "rational.h"

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

// Fibonacci Ticks array (used for Rho-gated Psi modes)
static const size_t FIB_TICKS[] = {
    5, 13, 89, 233, 1597, 
    4181, 10946, 28657, 75025, 196418, 
    514229, 1346269, 3524578, 9227465 // Covers runs up to 9 million ticks
};

static bool is_fibonacci_tick(size_t tick) {
    for (size_t i = 0; i < ARRAY_COUNT(FIB_TICKS); ++i) {
        if (tick == FIB_TICKS[i]) {
            return true;
        }
    }
    return false;
}

static bool numerator_is_prime(mpq_srcptr value) {
    mpz_t magnitude;
    mpz_init(magnitude);
    rational_abs_num(magnitude, value);
    // Use a reasonable primality test
    bool is_prime = mpz_cmp_ui(magnitude, 2UL) >= 0 && mpz_probab_prime_p(magnitude, 25) > 0;
    mpz_clear(magnitude);
    return is_prime;
}

// Standard psi transform: (u, b) -> (b/u, u/b)
static bool standard_psi(TRTS_State *state) {
    if (rational_is_zero(state->upsilon) || rational_is_zero(state->beta)) {
        return false;
    }

    mpz_t beta_den;
    mpz_t ups_num;
    mpz_t beta_num;
    mpz_t ups_den;
    mpz_inits(beta_den, ups_num, beta_num, ups_den, NULL);
    
    // Snapshot original components
    mpz_set(ups_num, mpq_numref(state->upsilon));
    mpz_set(ups_den, mpq_denref(state->upsilon));
    mpz_set(beta_num, mpq_numref(state->beta));
    mpz_set(beta_den, mpq_denref(state->beta));

    mpz_t new_u_num, new_u_den;
    mpz_t new_b_num, new_b_den;
    mpz_inits(new_u_num, new_u_den, new_b_num, new_b_den, NULL);

    // New Upsilon: (beta_num * ups_den) / (beta_den * ups_num)
    mpz_mul(new_u_num, beta_num, ups_den);
    mpz_mul(new_u_den, beta_den, ups_num);
    
    // New Beta: (ups_num * beta_den) / (ups_den * beta_num)
    mpz_mul(new_b_num, ups_num, beta_den);
    mpz_mul(new_b_den, ups_den, beta_num);
    
    // Set the new values
    rational_set_components(state->upsilon, new_u_num, new_u_den);
    rational_set_components(state->beta, new_b_num, new_b_den);

    mpz_clears(beta_den, ups_num, beta_num, ups_den, NULL);
    mpz_clears(new_u_num, new_u_den, new_b_num, new_b_den, NULL);

    return true;
}

// Triple psi transform: (u, b, k) -> (b/k, k/u, k/b)
static bool triple_psi(TRTS_State *state) {
    if (rational_is_zero(state->koppa) || rational_is_zero(state->upsilon) || rational_is_zero(state->beta)) {
        return false;
    }

    mpz_t ups_num, ups_den;
    mpz_t beta_num, beta_den;
    mpz_t koppa_num, koppa_den;
    mpz_inits(ups_num, ups_den, beta_num, beta_den, koppa_num, koppa_den, NULL);

    // Snapshot original components
    mpz_set(ups_num, mpq_numref(state->upsilon));
    mpz_set(ups_den, mpq_denref(state->upsilon));
    mpz_set(beta_num, mpq_numref(state->beta));
    mpz_set(beta_den, mpq_denref(state->beta));
    mpz_set(koppa_num, mpq_numref(state->koppa));
    mpz_set(koppa_den, mpq_denref(state->koppa));

    // New Upsilon: beta / koppa 
    mpz_t new_u_num, new_u_den;
    mpz_inits(new_u_num, new_u_den, NULL);
    mpz_mul(new_u_num, beta_num, koppa_den);
    mpz_mul(new_u_den, beta_den, koppa_num);

    // New Beta: koppa / upsilon
    mpz_t new_b_num, new_b_den;
    mpz_inits(new_b_num, new_b_den, NULL);
    mpz_mul(new_b_num, koppa_num, ups_den);
    mpz_mul(new_b_den, koppa_den, ups_num);
    
    // New Koppa: koppa / beta
    mpz_t new_k_num, new_k_den;
    mpz_inits(new_k_num, new_k_den, NULL);
    mpz_mul(new_k_num, koppa_num, beta_den);
    mpz_mul(new_k_den, koppa_den, beta_num);
    
    // Set the new values
    rational_set_components(state->upsilon, new_u_num, new_u_den);
    rational_set_components(state->beta, new_b_num, new_b_den);
    rational_set_components(state->koppa, new_k_num, new_k_den);

    mpz_clears(ups_num, ups_den, beta_num, beta_den, koppa_num, koppa_den, NULL);
    mpz_clears(new_u_num, new_u_den, new_b_num, new_b_den, new_k_num, new_k_den, NULL);

    return true;
}

static int psi_strength(const Config *config, const TRTS_State *state) {
    if (!config->enable_psi_strength_parameter || !state->rho_pending) {
        return 1;
    }

    int prime_count = 0;
    prime_count += numerator_is_prime(state->upsilon) ? 1 : 0;
    prime_count += numerator_is_prime(state->beta) ? 1 : 0;
    prime_count += numerator_is_prime(state->koppa) ? 1 : 0;
    
    // If no numerators are prime, we still fire the transform once (strength of 1)
    if (prime_count <= 0) {
        prime_count = 1;
    }
    return prime_count;
}

bool psi_transform(const Config *config, TRTS_State *state) {
    state->psi_triple_recent = false;
    state->psi_recent = false;
    state->psi_strength_applied = false;

    // Check firing conditions for RHO-gated modes (RHO_ONLY and MSTEP_RHO)
    if (config->psi_mode == PSI_MODE_RHO_ONLY || config->psi_mode == PSI_MODE_MSTEP_RHO) {
        if (!state->rho_pending) {
            return false;
        }

        // The RHO event must also land on a Fibonacci tick to fire the Psi transform
        if (!is_fibonacci_tick(state->tick)) {
            return false;
        }
    }

    // Determine if the Psi event can fire at all (only MSTEP fires without rho_pending)
    bool can_fire = state->rho_pending || (config->psi_mode == PSI_MODE_MSTEP);

    if (!can_fire) {
        return false;
    }

    // Determine the strength (number of transforms to execute)
    int strength = psi_strength(config, state);
    if (strength > 1) {
        state->psi_strength_applied = true;
    }

    bool fired = false;
    
    // Execute the transform 'strength' number of times
    for (int i = 0; i < strength; ++i) {
        bool request_triple = config->triple_psi_mode;
        
        // Conditional triple psi based on all three numerators being prime
        if (config->enable_conditional_triple_psi) {
            if (numerator_is_prime(state->upsilon) && numerator_is_prime(state->beta) &&
                numerator_is_prime(state->koppa)) {
                request_triple = true;
            }
        }
        
        // Conditional triple psi based on the strength count (if strength >= 3, 
        // the third-to-last fire is a triple)
        if (strength >= 3 && i == strength - 3) {
            request_triple = true;
        }

        // Apply the transform
        if (request_triple) {
            fired = triple_psi(state);
            if (fired) {
                state->psi_triple_recent = true;
            }
        } else {
            fired = standard_psi(state);
        }

        if (fired) {
            state->psi_recent = true;
            // The rho_pending flag is cleared on the *first* successful fire
            if (i == 0) {
                state->rho_pending = false;
            }
        } else {
            // If the transform fails (e.g., division by zero), stop the loop
            break;
        }
    }

    return fired;
}
