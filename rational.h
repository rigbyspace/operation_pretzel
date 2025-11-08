#ifndef RATIONAL_H
#define RATIONAL_H

#include <gmp.h>
#include <stdbool.h>
#include <stdio.h>

void rational_init(mpq_t value);
void rational_clear(mpq_t value);
void rational_set(mpq_t dest, const mpq_t src);
void rational_set_si(mpq_t dest, long numerator, unsigned long denominator);
void rational_add(mpq_t result, const mpq_t a, const mpq_t b);
void rational_add_ui(mpq_t result, const mpq_t a, unsigned long numerator, unsigned long denominator);
void rational_mul(mpq_t result, const mpq_t a, const mpq_t b);
void rational_div(mpq_t result, const mpq_t a, const mpq_t b);
void rational_sub(mpq_t result, const mpq_t a, const mpq_t b);
void rational_print(FILE *stream, const mpq_t value);
bool rational_is_zero(const mpq_t value);

#endif // RATIONAL_H
