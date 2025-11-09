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

#include <gmp.h>
#include "rational_strict.h"

static void rational_assign_components(mpq_t dest, mpz_srcptr numerator, mpz_srcptr denominator) {
    mpz_set(mpq_numref(dest), numerator);
    mpz_set(mpq_denref(dest), denominator);
}

void rational_init(mpq_t value) {
    mpq_init(value);
    mpz_set_ui(mpq_numref(value), 0);
    mpz_set_ui(mpq_denref(value), 1);
}

void rational_clear(mpq_t value) {
    mpq_clear(value);
}

void rational_set(mpq_t dest, const mpq_t src) {
    mpz_set(mpq_numref(dest), mpq_numref(src));
    mpz_set(mpq_denref(dest), mpq_denref(src));
}

void rational_set_si(mpq_t dest, long numerator, unsigned long denominator) {
    if (denominator == 0) {
        fprintf(stderr, "rational_set_si: zero denominator forbidden\n");
        abort();
    }
    mpz_set_si(mpq_numref(dest), numerator);
    mpz_set_ui(mpq_denref(dest), denominator);
}

void rational_set_components(mpq_t dest, mpz_srcptr numerator, mpz_srcptr denominator) {
    rational_assign_components(dest, numerator, denominator);
}

void rational_add(mpq_t result, mpq_srcptr a, mpq_srcptr b) {
    mpz_t lhs;
    mpz_t rhs;
    mpz_t denominator;
    mpz_init(lhs);
    mpz_init(rhs);
    mpz_init(denominator);

    mpz_mul(lhs, mpq_numref(a), mpq_denref(b));
    mpz_mul(rhs, mpq_numref(b), mpq_denref(a));
    mpz_add(lhs, lhs, rhs);
    mpz_mul(denominator, mpq_denref(a), mpq_denref(b));

    rational_assign_components(result, lhs, denominator);

    mpz_clear(lhs);
    mpz_clear(rhs);
    mpz_clear(denominator);
}

void rational_sub(mpq_t result, mpq_srcptr a, mpq_srcptr b) {
    mpz_t lhs;
    mpz_t rhs;
    mpz_t denominator;
    mpz_init(lhs);
    mpz_init(rhs);
    mpz_init(denominator);

    mpz_mul(lhs, mpq_numref(a), mpq_denref(b));
    mpz_mul(rhs, mpq_numref(b), mpq_denref(a));
    mpz_sub(lhs, lhs, rhs);
    mpz_mul(denominator, mpq_denref(a), mpq_denref(b));

    rational_assign_components(result, lhs, denominator);

    mpz_clear(lhs);
    mpz_clear(rhs);
    mpz_clear(denominator);
}

void rational_mul(mpq_t result, mpq_srcptr a, mpq_srcptr b) {
    mpz_t numerator;
    mpz_t denominator;
    mpz_init(numerator);
    mpz_init(denominator);

    mpz_mul(numerator, mpq_numref(a), mpq_numref(b));
    mpz_mul(denominator, mpq_denref(a), mpq_denref(b));

    rational_assign_components(result, numerator, denominator);

    mpz_clear(numerator);
    mpz_clear(denominator);
}

void rational_div(mpq_t result, mpq_srcptr a, mpq_srcptr b) {
    if (mpz_sgn(mpq_numref(b)) == 0) {
        fprintf(stderr, "rational_div: division by zero numerator forbidden\n");
        abort();
    }

    mpz_t numerator;
    mpz_t denominator;
    mpz_init(numerator);
    mpz_init(denominator);

    mpz_mul(numerator, mpq_numref(a), mpq_denref(b));
    mpz_mul(denominator, mpq_denref(a), mpq_numref(b));

    rational_assign_components(result, numerator, denominator);

    mpz_clear(numerator);
    mpz_clear(denominator);
}

void rational_add_ui(mpq_t result, mpq_srcptr a, unsigned long numerator, unsigned long denominator) {
    if (denominator == 0) {
        fprintf(stderr, "rational_add_ui: zero denominator forbidden\n");
        abort();
    }

    mpz_t lhs;
    mpz_t rhs;
    mpz_t denom;
    mpz_init(lhs);
    mpz_init(rhs);
    mpz_init(denom);

    mpz_mul_ui(lhs, mpq_numref(a), denominator);
    mpz_mul_ui(rhs, mpq_denref(a), numerator);
    mpz_add(lhs, lhs, rhs);
    mpz_mul_ui(denom, mpq_denref(a), denominator);

    rational_assign_components(result, lhs, denom);

    mpz_clear(lhs);
    mpz_clear(rhs);
    mpz_clear(denom);
}

void rational_negate(mpq_t value) {
    mpz_neg(mpq_numref(value), mpq_numref(value));
}

void rational_copy_num(mpz_t dest, mpq_srcptr value) {
    mpz_set(dest, mpq_numref(value));
}

void rational_abs_num(mpz_t dest, mpq_srcptr value) {
    mpz_abs(dest, mpq_numref(value));
}

void rational_mod(mpq_t result, mpq_srcptr value, mpq_srcptr modulus) {
    if (mpz_sgn(mpq_numref(modulus)) == 0) {
        rational_set(result, value);
        return;
    }

    mpz_t scaled_value_num;
    mpz_t scaled_mod_num;
    mpz_t denominator;
    mpz_t remainder;
    mpz_init(scaled_value_num);
    mpz_init(scaled_mod_num);
    mpz_init(denominator);
    mpz_init(remainder);

    mpz_mul(scaled_value_num, mpq_numref(value), mpq_denref(modulus));
    mpz_mul(scaled_mod_num, mpq_numref(modulus), mpq_denref(value));
    mpz_mul(denominator, mpq_denref(value), mpq_denref(modulus));

    if (mpz_sgn(scaled_mod_num) < 0) {
        mpz_neg(scaled_mod_num, scaled_mod_num);
    }

    mpz_fdiv_r(remainder, scaled_value_num, scaled_mod_num);

    rational_assign_components(result, remainder, denominator);

    mpz_clear(scaled_value_num);
    mpz_clear(scaled_mod_num);
    mpz_clear(denominator);
    mpz_clear(remainder);
}

void rational_delta(mpq_t result, mpq_srcptr current, mpq_srcptr previous) {
    rational_sub(result, current, previous);
}

bool rational_is_zero(mpq_srcptr value) {
    return mpz_sgn(mpq_numref(value)) == 0;
}

void rational_print(FILE *stream, mpq_srcptr value) {
    mpq_out_str(stream, 10, value);
    fprintf(stream, "\n");
}

bool rational_denominator_zero(mpq_srcptr value) {
    return mpz_sgn(mpq_denref(value)) == 0;
}
