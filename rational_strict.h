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

#ifndef RATIONAL_STRICT_H
#define RATIONAL_STRICT_H

#include <gmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NDEBUG
#ifdef mpq_canonicalize
#undef mpq_canonicalize
#endif
static inline void trts_forbidden_canonicalize(const char *file, int line) {
    fprintf(stderr, "mpq_canonicalize() forbidden at %s:%d\n", file, line);
    abort();
}
#define mpq_canonicalize(...) trts_forbidden_canonicalize(__FILE__, __LINE__)
#endif

void rational_init(mpq_t value);
void rational_clear(mpq_t value);
void rational_set(mpq_t dest, const mpq_t src);
void rational_set_si(mpq_t dest, long numerator, unsigned long denominator);
void rational_set_components(mpq_t dest, mpz_srcptr numerator, mpz_srcptr denominator);
void rational_add(mpq_t result, mpq_srcptr a, mpq_srcptr b);
void rational_sub(mpq_t result, mpq_srcptr a, mpq_srcptr b);
void rational_mul(mpq_t result, mpq_srcptr a, mpq_srcptr b);
void rational_div(mpq_t result, mpq_srcptr a, mpq_srcptr b);
void rational_add_ui(mpq_t result, mpq_srcptr a, unsigned long numerator, unsigned long denominator);
void rational_negate(mpq_t value);
void rational_copy_num(mpz_t dest, mpq_srcptr value);
void rational_abs_num(mpz_t dest, mpq_srcptr value);
void rational_mod(mpq_t result, mpq_srcptr value, mpq_srcptr modulus);
void rational_delta(mpq_t result, mpq_srcptr current, mpq_srcptr previous);
bool rational_is_zero(mpq_srcptr value);
void rational_print(FILE *stream, mpq_srcptr value);
bool rational_denominator_zero(mpq_srcptr value);

#endif // RATIONAL_STRICT_H
