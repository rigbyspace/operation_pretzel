#include "analysis_utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "rational.h"
#include "simulate.h"

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef struct {
    const char *name;
    double value;
} KnownConstant;

static const KnownConstant KNOWN_CONSTANTS[] = {
    {"phi", 1.6180339887498948482},
    {"rho", 1.3247179572447458000},
    {"delta_s", 1.4655712318767680267},
    {"tribonacci", 1.8392867552141611326},
    {"plastic", 1.3247179572447458000},
    {"sqrt2", 1.4142135623730950488},
    {"silver", 2.4142135623730950488}
};

static bool parse_values_csv(const Config *config, RunSummary *summary);
static bool parse_events_csv(RunSummary *summary);

void run_summary_init(RunSummary *summary) {
    rational_init(summary->final_ratio);
    summary->ratio_defined = false;
    summary->final_ratio_str[0] = '\0';
    summary->closest_constant[0] = '\0';
    summary->closest_delta = 0.0;
    summary->convergence_tick = 0U;
    summary->pattern[0] = '\0';
    summary->classification[0] = '\0';
    summary->stack_summary[0] = '\0';
    summary->final_ratio_snapshot = 0.0;
    summary->total_samples = 0U;
    summary->total_ticks = 0U;
    summary->psi_events = 0U;
    summary->rho_events = 0U;
    summary->mu_zero_events = 0U;
    summary->psi_spacing_mean = 0.0;
    summary->psi_spacing_stddev = 0.0;
    summary->ratio_variance = 0.0;
    summary->ratio_range = 0.0;
    summary->ratio_mean = 0.0;
    summary->ratio_stddev = 0.0;
    summary->average_stack_depth = 0.0;
    for (size_t i = 0; i < ARRAY_COUNT(summary->stack_histogram); ++i) {
        summary->stack_histogram[i] = 0U;
    }
}

void run_summary_clear(RunSummary *summary) {
    rational_clear(summary->final_ratio);
}

void run_summary_copy(RunSummary *dest, const RunSummary *src) {
    if (dest == src) {
        return;
    }
    rational_set(dest->final_ratio, src->final_ratio);
    dest->ratio_defined = src->ratio_defined;
    memcpy(dest->final_ratio_str, src->final_ratio_str, sizeof(dest->final_ratio_str));
    memcpy(dest->closest_constant, src->closest_constant, sizeof(dest->closest_constant));
    dest->closest_delta = src->closest_delta;
    dest->convergence_tick = src->convergence_tick;
    memcpy(dest->pattern, src->pattern, sizeof(dest->pattern));
    memcpy(dest->classification, src->classification, sizeof(dest->classification));
    memcpy(dest->stack_summary, src->stack_summary, sizeof(dest->stack_summary));
    dest->final_ratio_snapshot = src->final_ratio_snapshot;
    dest->total_samples = src->total_samples;
    dest->total_ticks = src->total_ticks;
    dest->psi_events = src->psi_events;
    dest->rho_events = src->rho_events;
    dest->mu_zero_events = src->mu_zero_events;
    dest->psi_spacing_mean = src->psi_spacing_mean;
    dest->psi_spacing_stddev = src->psi_spacing_stddev;
    dest->ratio_variance = src->ratio_variance;
    dest->ratio_range = src->ratio_range;
    dest->ratio_mean = src->ratio_mean;
    dest->ratio_stddev = src->ratio_stddev;
    dest->average_stack_depth = src->average_stack_depth;
    memcpy(dest->stack_histogram, src->stack_histogram, sizeof(dest->stack_histogram));
}

bool analyze_latest_run(const Config *config, RunSummary *summary) {
    if (!parse_values_csv(config, summary)) {
        return false;
    }
    if (!parse_events_csv(summary)) {
        return false;
    }
    return true;
}

bool simulate_and_analyze(const Config *config, RunSummary *summary) {
    simulate(config);
    return analyze_latest_run(config, summary);
}

const char *analysis_psi_type_label(const Config *config) {
    return config->triple_psi_mode ? "3-way" : "2-way";
}

bool analysis_constant_value(const char *name, double *value) {
    if (!name || !value) {
        return false;
    }
    for (size_t i = 0; i < ARRAY_COUNT(KNOWN_CONSTANTS); ++i) {
        if (strcmp(name, KNOWN_CONSTANTS[i].name) == 0) {
            *value = KNOWN_CONSTANTS[i].value;
            return true;
        }
    }
    return false;
}

static void update_stack_summary(RunSummary *summary, size_t stack_sum) {
    if (summary->total_samples == 0U) {
        snprintf(summary->stack_summary, sizeof(summary->stack_summary), "avg=0.00 []");
        return;
    }

    summary->average_stack_depth = (double)stack_sum / (double)summary->total_samples;

    char buffer[128];
    int offset = snprintf(buffer, sizeof(buffer), "avg=%.2f [", summary->average_stack_depth);
    size_t histogram_limit = ARRAY_COUNT(summary->stack_histogram);
    for (size_t depth = 0; depth < histogram_limit; ++depth) {
        offset += snprintf(buffer + offset, sizeof(buffer) - (size_t)offset, "%zu:%zu", depth,
                           summary->stack_histogram[depth]);
        if (depth + 1 < histogram_limit) {
            offset += snprintf(buffer + offset, sizeof(buffer) - (size_t)offset, ",");
        }
    }
    snprintf(buffer + offset, sizeof(buffer) - (size_t)offset, "]");
    strncpy(summary->stack_summary, buffer, sizeof(summary->stack_summary));
    summary->stack_summary[sizeof(summary->stack_summary) - 1] = '\0';
}

static void determine_pattern(RunSummary *summary, bool ratio_defined, bool divergent,
                              bool fixed_point, bool oscillating, size_t best_constant_index,
                              double best_delta) {
    if (!ratio_defined) {
        snprintf(summary->pattern, sizeof(summary->pattern), "null");
        snprintf(summary->classification, sizeof(summary->classification), "Null");
        return;
    }

    if (divergent) {
        snprintf(summary->pattern, sizeof(summary->pattern), "divergent");
        snprintf(summary->classification, sizeof(summary->classification), "Chaotic");
        return;
    }

    if (fixed_point) {
        snprintf(summary->pattern, sizeof(summary->pattern), "fixed point");
        snprintf(summary->classification, sizeof(summary->classification), "FixedPoint");
        return;
    }

    if (oscillating) {
        snprintf(summary->pattern, sizeof(summary->pattern), "oscillating");
        snprintf(summary->classification, sizeof(summary->classification), "Oscillating");
        return;
    }

    snprintf(summary->pattern, sizeof(summary->pattern), "stable");

    if (best_constant_index != SIZE_MAX && best_delta < 1e-4) {
        snprintf(summary->classification, sizeof(summary->classification), "Convergent(%s)",
                 KNOWN_CONSTANTS[best_constant_index].name);
    } else {
        snprintf(summary->classification, sizeof(summary->classification), "Stable");
    }
}

static bool parse_values_csv(const Config *config, RunSummary *summary) {
    (void)config;
    FILE *file = fopen("values.csv", "r");
    if (!file) {
        return false;
    }

    char line[4096];
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return false;
    }

    mpq_t upsilon;
    mpq_t beta;
    mpq_t ratio;
    rational_init(upsilon);
    rational_init(beta);
    rational_init(ratio);

    mpz_t abs_num;
    mpz_t abs_den;
    mpz_t max_mag_num;
    mpz_t max_mag_den;
    mpz_init(abs_num);
    mpz_init(abs_den);
    mpz_init(max_mag_num);
    mpz_init(max_mag_den);
    mpz_set_ui(max_mag_num, 0UL);
    mpz_set_ui(max_mag_den, 0UL);

    size_t stack_sum = 0U;
    double ratio_mean = 0.0;
    double ratio_m2 = 0.0;
    size_t ratio_count = 0U;
    double ratio_min = 0.0;
    double ratio_max = 0.0;
    double previous_ratio = 0.0;
    bool have_previous_ratio = false;
    double max_delta = 0.0;
    size_t sign_changes = 0U;

    double best_delta = INFINITY;
    size_t best_constant_index = SIZE_MAX;

    summary->ratio_defined = false;
    summary->total_samples = 0U;
    summary->total_ticks = 0U;

    while (fgets(line, sizeof(line), file)) {
        char *saveptr = NULL;
        char *token = strtok_r(line, ",", &saveptr);
        if (!token) {
            continue;
        }
        size_t tick = (size_t)strtoull(token, NULL, 10);
        summary->total_ticks = tick;

        token = strtok_r(NULL, ",", &saveptr);
        if (!token) {
            continue;
        }
        int microtick = atoi(token);
        (void)microtick;

        token = strtok_r(NULL, ",", &saveptr);
        if (!token) {
            continue;
        }
        mpz_set_str(mpq_numref(upsilon), token, 10);

        token = strtok_r(NULL, ",", &saveptr);
        if (!token) {
            continue;
        }
        mpz_set_str(mpq_denref(upsilon), token, 10);

        token = strtok_r(NULL, ",", &saveptr);
        if (!token) {
            continue;
        }
        mpz_set_str(mpq_numref(beta), token, 10);

        token = strtok_r(NULL, ",", &saveptr);
        if (!token) {
            continue;
        }
        mpz_set_str(mpq_denref(beta), token, 10);

        unsigned long stack_size = 0UL;
        for (int field_index = 6; field_index <= 22; ++field_index) {
            token = strtok_r(NULL, ",", &saveptr);
            if (!token) {
                break;
            }
            if (field_index == 22) {
                stack_size = strtoul(token, NULL, 10);
            }
        }

        if (stack_size >= ARRAY_COUNT(summary->stack_histogram)) {
            stack_size = ARRAY_COUNT(summary->stack_histogram) - 1U;
        }
        summary->stack_histogram[stack_size] += 1U;
        stack_sum += stack_size;

        summary->total_samples += 1U;

        mpz_abs(abs_num, mpq_numref(upsilon));
        if (mpz_cmp(abs_num, max_mag_num) > 0) {
            mpz_set(max_mag_num, abs_num);
        }

        mpz_abs(abs_den, mpq_denref(upsilon));
        if (mpz_cmp(abs_den, max_mag_den) > 0) {
            mpz_set(max_mag_den, abs_den);
        }

        mpz_abs(abs_num, mpq_numref(beta));
        if (mpz_cmp(abs_num, max_mag_num) > 0) {
            mpz_set(max_mag_num, abs_num);
        }

        mpz_abs(abs_den, mpq_denref(beta));
        if (mpz_cmp(abs_den, max_mag_den) > 0) {
            mpz_set(max_mag_den, abs_den);
        }

        if (!rational_is_zero(beta)) {
            rational_div(ratio, upsilon, beta);
            summary->ratio_defined = true;
            summary->final_ratio_snapshot = mpq_get_d(ratio);
            rational_set(summary->final_ratio, ratio);
            gmp_snprintf(summary->final_ratio_str, sizeof(summary->final_ratio_str), "%Zd/%Zd",
                         mpq_numref(ratio), mpq_denref(ratio));

            ++ratio_count;
            double snapshot = summary->final_ratio_snapshot;
            if (ratio_count == 1U) {
                ratio_min = snapshot;
                ratio_max = snapshot;
            } else {
                if (snapshot < ratio_min) {
                    ratio_min = snapshot;
                }
                if (snapshot > ratio_max) {
                    ratio_max = snapshot;
                }
            }

            double delta = snapshot - ratio_mean;
            ratio_mean += delta / (double)ratio_count;
            double delta2 = snapshot - ratio_mean;
            ratio_m2 += delta * delta2;

            if (have_previous_ratio) {
                double diff = fabs(snapshot - previous_ratio);
                if (diff > max_delta) {
                    max_delta = diff;
                }
                if ((snapshot > 0.0 && previous_ratio < 0.0) ||
                    (snapshot < 0.0 && previous_ratio > 0.0)) {
                    ++sign_changes;
                }
            }
            previous_ratio = snapshot;
            have_previous_ratio = true;

            for (size_t i = 0; i < ARRAY_COUNT(KNOWN_CONSTANTS); ++i) {
                double constant_delta = fabs(snapshot - KNOWN_CONSTANTS[i].value);
                if (constant_delta < best_delta) {
                    best_delta = constant_delta;
                    best_constant_index = i;
                }
                if (constant_delta < 1e-5 && summary->convergence_tick == 0U) {
                    summary->convergence_tick = tick;
                }
            }
        }
    }

    fclose(file);

    summary->ratio_mean = ratio_mean;
    if (ratio_count > 1U) {
        summary->ratio_variance = ratio_m2 / (double)(ratio_count - 1U);
        summary->ratio_stddev = sqrt(summary->ratio_variance);
    } else {
        summary->ratio_variance = 0.0;
        summary->ratio_stddev = 0.0;
    }
    summary->ratio_range = ratio_max - ratio_min;

    bool ratio_defined = summary->ratio_defined;

    mpz_t divergence_threshold;
    mpz_init_set_ui(divergence_threshold, 1000000000UL);

    bool divergent = ratio_defined &&
                     (summary->ratio_range > 1.0e6 ||
                      mpz_cmp(max_mag_num, divergence_threshold) > 0 ||
                      mpz_cmp(max_mag_den, divergence_threshold) > 0);

    bool fixed_point = ratio_defined && summary->ratio_range < 1.0e-9 && max_delta < 1.0e-12;
    bool oscillating = ratio_defined && !divergent && !fixed_point &&
                       summary->ratio_range < 100.0 && sign_changes > ratio_count / 3U;

    if (best_constant_index != SIZE_MAX) {
        strncpy(summary->closest_constant, KNOWN_CONSTANTS[best_constant_index].name,
                sizeof(summary->closest_constant));
        summary->closest_constant[sizeof(summary->closest_constant) - 1] = '\0';
        summary->closest_delta = best_delta;
    } else {
        strncpy(summary->closest_constant, "None", sizeof(summary->closest_constant));
        summary->closest_constant[sizeof(summary->closest_constant) - 1] = '\0';
        summary->closest_delta = INFINITY;
    }

    update_stack_summary(summary, stack_sum);
    determine_pattern(summary, ratio_defined, divergent, fixed_point, oscillating, best_constant_index,
                      best_delta);

    mpz_clear(divergence_threshold);
    mpz_clear(abs_num);
    mpz_clear(abs_den);
    mpz_clear(max_mag_num);
    mpz_clear(max_mag_den);
    rational_clear(upsilon);
    rational_clear(beta);
    rational_clear(ratio);

    return true;
}

static bool parse_events_csv(RunSummary *summary) {
    FILE *file = fopen("events.csv", "r");
    if (!file) {
        return false;
    }

    char line[1024];
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return false;
    }

    size_t psi_event_count = 0U;
    size_t rho_event_count = 0U;
    size_t mu_zero_count = 0U;

    size_t last_psi_index = 0U;
    bool have_last_psi = false;
    double spacing_mean = 0.0;
    double spacing_m2 = 0.0;
    size_t spacing_count = 0U;

    while (fgets(line, sizeof(line), file)) {
        char *saveptr = NULL;
        char *token = strtok_r(line, ",", &saveptr);
        if (!token) {
            continue;
        }
        size_t tick = (size_t)strtoull(token, NULL, 10);

        token = strtok_r(NULL, ",", &saveptr);
        if (!token) {
            continue;
        }
        int microtick = atoi(token);

        token = strtok_r(NULL, ",", &saveptr);
        if (!token) {
            continue;
        }
        (void)token;

        token = strtok_r(NULL, ",", &saveptr);
        if (!token) {
            continue;
        }
        int rho_event = atoi(token);
        if (rho_event != 0) {
            ++rho_event_count;
        }

        token = strtok_r(NULL, ",", &saveptr);
        if (!token) {
            continue;
        }
        int psi_event = atoi(token);
        if (psi_event != 0) {
            ++psi_event_count;
            size_t current_index = (tick - 1U) * 11U + (size_t)microtick;
            if (have_last_psi) {
                double spacing = (double)(current_index - last_psi_index);
                ++spacing_count;
                double delta = spacing - spacing_mean;
                spacing_mean += delta / (double)spacing_count;
                double delta2 = spacing - spacing_mean;
                spacing_m2 += delta * delta2;
            }
            last_psi_index = current_index;
            have_last_psi = true;
        }

        token = strtok_r(NULL, ",", &saveptr);
        if (!token) {
            continue;
        }
        int mu_zero = atoi(token);
        if (mu_zero != 0) {
            ++mu_zero_count;
        }
    }

    fclose(file);

    summary->psi_events = psi_event_count;
    summary->rho_events = rho_event_count;
    summary->mu_zero_events = mu_zero_count;

    if (spacing_count > 1U) {
        summary->psi_spacing_mean = spacing_mean;
        summary->psi_spacing_stddev = sqrt(spacing_m2 / (double)(spacing_count - 1U));
    } else {
        summary->psi_spacing_mean = spacing_count == 1U ? spacing_mean : 0.0;
        summary->psi_spacing_stddev = 0.0;
    }

    return true;
}
