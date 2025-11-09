#include <stdio.h>
#include <gmp.h>
#include <stdbool.h> // <--- ADDED for the 'bool' return type
#include "rational_strict.h"

// =====================
// Rational Math Helpers (STRICT MODE - NO CANONICALIZATION)
// =====================

// Corrected type to 'bool' to match rational_strict.h
bool rational_is_zero(mpq_srcptr a) {
    // Denominator is assumed non-zero. Only check the numerator.
    return mpz_sgn(mpq_numref(a)) == 0;
}

// Missing from linker error list.
void rational_abs_num(mpz_ptr dest, mpq_srcptr a) {
    mpz_abs(dest, mpq_numref(a));
}

// Corrected signature to one argument to match rational_strict.h (negates in place).
void rational_negate(mpq_ptr value) {
    mpq_neg(value, value);
}

// Missing from linker error list.
void rational_set_components(mpq_ptr dest, mpz_srcptr num, mpz_srcptr den) {
    mpz_set(mpq_numref(dest), num);
    mpz_set(mpq_denref(dest), den);
    // mpq_canonicalize is strictly forbidden by the creed.
}

// Moved these functions up to resolve the 'implicit declaration' error in rational_mod

void rational_floor(mpz_ptr dest, mpq_srcptr value) {
    mpz_t temp_num;
    mpz_init(temp_num);

    // Uses mpq_get_num/den and manual division since the value might not be canonical
    mpq_get_num(temp_num, value);
    mpz_fdiv_q(dest, temp_num, mpq_denref(value));

    mpz_clear(temp_num);
}

void rational_ceil(mpz_ptr dest, mpq_srcptr value) {
    mpz_t temp_num;
    mpz_init(temp_num);

    mpq_get_num(temp_num, value);
    mpz_cdiv_q(dest, temp_num, mpq_denref(value));

    mpz_clear(temp_num);
}

// Missing from linker error list.
void rational_mod(mpq_ptr res, mpq_srcptr a, mpq_srcptr b) {
    mpq_t floor_val;
    mpz_t floor_z;

    mpq_init(floor_val);
    mpz_init(floor_z);

    // 1. Calculate a / b
    mpq_div(res, a, b);

    // 2. Get floor of (a / b) (now correctly declared above)
    rational_floor(floor_z, res);

    // 3. Convert floor(a/b) back to rational form
    mpq_set_z(floor_val, floor_z);

    // 4. Calculate fractional part: (a/b) - floor(a/b)
    mpq_sub(res, res, floor_val);

    mpq_clear(floor_val);
    mpz_clear(floor_z);
}

// Missing from linker error list.
void rational_delta(mpq_ptr res, mpq_srcptr a, mpq_srcptr b) {
    mpq_sub(res, a, b);
}


// Existing functions, with creed violations removed (mpq_canonicalize):

int rational_cmp(mpq_srcptr a, mpq_srcptr b) {
    return mpq_cmp(a, b);
}

void rational_set(mpq_ptr dest, mpq_srcptr src) {
    mpq_set(dest, src);
}

void rational_set_si(mpq_ptr dest, long num, unsigned long den) {
    mpz_set_si(mpq_numref(dest), num);
    mpz_set_ui(mpq_denref(dest), den);
    // CRITICAL FIX: mpq_canonicalize(dest); // REMOVED - VIOLATES CREED
}

void rational_add(mpq_ptr res, mpq_srcptr a, mpq_srcptr b) {
    mpq_add(res, a, b);
}

void rational_sub(mpq_ptr res, mpq_srcptr a, mpq_srcptr b) {
    mpq_sub(res, a, b);
}

void rational_mul(mpq_ptr res, mpq_srcptr a, mpq_srcptr b) {
    mpq_mul(res, a, b);
}

void rational_div(mpq_ptr res, mpq_srcptr a, mpq_srcptr b) {
    mpq_div(res, a, b);
}

void rational_neg(mpq_ptr res, mpq_srcptr a) {
    mpq_neg(res, a);
}

void rational_inv(mpq_ptr res, mpq_srcptr a) {
    mpq_inv(res, a);
}

void rational_abs(mpq_ptr res, mpq_srcptr a) {
    mpq_abs(res, a);
}

void rational_round(mpz_ptr dest, mpq_srcptr value) {
    mpz_t num, den, rem, dbl_rem;
    mpz_inits(num, den, rem, dbl_rem, NULL);

    mpq_get_num(num, value);
    mpq_get_den(den, value);
    mpz_fdiv_qr(dest, rem, num, den);
    mpz_mul_ui(dbl_rem, rem, 2);

    if (mpz_cmpabs(dbl_rem, den) >= 0) {
        if (mpq_sgn(value) >= 0) {
            mpz_add_ui(dest, dest, 1);
        } else {
            mpz_sub_ui(dest, dest, 1);
        }
    }

    mpz_clears(num, den, rem, dbl_rem, NULL);
}

int rational_sgn(mpq_srcptr value) {
    return mpq_sgn(value);
}

void rational_init(mpq_ptr value) {
    mpq_init(value);
}

void rational_clear(mpq_ptr value) {
    mpq_clear(value);
}

void rational_set_str(mpq_ptr value, const char *str, int base) {
    // This part of GMP is necessary for parsing, but we remove the subsequent canonicalization.
    mpq_set_str(value, str, base);
    // CRITICAL FIX: mpq_canonicalize(value); // REMOVED - VIOLATES CREED
}

// ====================
// Rational Output
// ====================

void rational_print(FILE *stream, mpq_srcptr value) {
    mpq_out_str(stream, 10, value);
    fprintf(stream, "\n");
}
