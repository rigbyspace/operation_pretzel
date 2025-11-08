#include "rational.h"

void rational_init(mpq_t value) {
    mpq_init(value);
    mpq_set_ui(value, 0, 1);
}

void rational_clear(mpq_t value) {
    mpq_clear(value);
}

void rational_set(mpq_t dest, const mpq_t src) {
    mpq_set(dest, src);
}

void rational_set_si(mpq_t dest, long numerator, unsigned long denominator) {
    mpq_set_si(dest, numerator, (long)denominator);
    mpq_canonicalize(dest);
}

void rational_add(mpq_t result, const mpq_t a, const mpq_t b) {
    mpq_add(result, a, b);
    mpq_canonicalize(result);
}

void rational_add_ui(mpq_t result, const mpq_t a, unsigned long numerator, unsigned long denominator) {
    mpq_t temp;
    mpq_init(temp);
    mpq_set_ui(temp, numerator, denominator);
    mpq_canonicalize(temp);
    mpq_add(result, a, temp);
    mpq_canonicalize(result);
    mpq_clear(temp);
}

void rational_mul(mpq_t result, const mpq_t a, const mpq_t b) {
    mpq_mul(result, a, b);
    mpq_canonicalize(result);
}

void rational_div(mpq_t result, const mpq_t a, const mpq_t b) {
    mpq_div(result, a, b);
    mpq_canonicalize(result);
}

void rational_sub(mpq_t result, const mpq_t a, const mpq_t b) {
    mpq_sub(result, a, b);
    mpq_canonicalize(result);
}

void rational_print(FILE *stream, const mpq_t value) {
    gmp_fprintf(stream, "%Qd", value);
}

bool rational_is_zero(const mpq_t value) {
    return mpq_sgn(value) == 0;
}
