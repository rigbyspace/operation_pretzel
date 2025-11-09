#include <gmp.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "analysis_utils.h"
#include "config.h"

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef struct {
    Config config;
    RunSummary summary;
    double score;
    bool evaluated;
} Candidate;

typedef struct {
    size_t generations;
    size_t population;
    size_t elite;
    unsigned int seed;
    char strategy[32];
    char target_constant[32];
    bool save_output;
    char output_path[256];
} EvolutionOptions;

static EngineMode ENGINE_MODES[] = {ENGINE_MODE_ADD, ENGINE_MODE_MULTI, ENGINE_MODE_SLIDE,
                                    ENGINE_MODE_DELTA_ADD};
static PsiMode PSI_MODES[] = {PSI_MODE_MSTEP, PSI_MODE_RHO_ONLY, PSI_MODE_MSTEP_RHO,
                              PSI_MODE_INHIBIT_RHO};
static KoppaMode KOPPA_MODES[] = {KOPPA_MODE_DUMP, KOPPA_MODE_POP, KOPPA_MODE_ACCUMULATE};

static EngineTrackMode track_mode_for_engine(EngineMode mode) {
    switch (mode) {
    case ENGINE_MODE_ADD:
        return ENGINE_TRACK_ADD;
    case ENGINE_MODE_MULTI:
        return ENGINE_TRACK_MULTI;
    case ENGINE_MODE_SLIDE:
        return ENGINE_TRACK_SLIDE;
    case ENGINE_MODE_DELTA_ADD:
    default:
        return ENGINE_TRACK_ADD;
    }
}

static void candidate_init(Candidate *candidate) {
    config_init(&candidate->config);
    run_summary_init(&candidate->summary);
    candidate->score = 0.0;
    candidate->evaluated = false;
    candidate->config.ticks = 30U;
    rational_set_si(candidate->config.initial_koppa, 1, 1);
    candidate->config.koppa_trigger = KOPPA_ON_ALL_MU;
    candidate->config.prime_target = PRIME_ON_MEMORY;
    candidate->config.mt10_behavior = MT10_FORCED_PSI;
}

static void candidate_clear(Candidate *candidate) {
    config_clear(&candidate->config);
    run_summary_clear(&candidate->summary);
}

static void config_clone(Config *dest, const Config *src) {
    dest->psi_mode = src->psi_mode;
    dest->koppa_mode = src->koppa_mode;
    dest->engine_mode = src->engine_mode;
    dest->engine_upsilon = src->engine_upsilon;
    dest->engine_beta = src->engine_beta;
    dest->dual_track_mode = src->dual_track_mode;
    dest->triple_psi_mode = src->triple_psi_mode;
    dest->multi_level_koppa = src->multi_level_koppa;
    dest->koppa_trigger = src->koppa_trigger;
    dest->prime_target = src->prime_target;
    dest->mt10_behavior = src->mt10_behavior;
    dest->ratio_trigger_mode = src->ratio_trigger_mode;
    dest->ticks = src->ticks;
    rational_set(dest->initial_upsilon, src->initial_upsilon);
    rational_set(dest->initial_beta, src->initial_beta);
    rational_set(dest->initial_koppa, src->initial_koppa);
    dest->enable_asymmetric_cascade = src->enable_asymmetric_cascade;
    dest->enable_conditional_triple_psi = src->enable_conditional_triple_psi;
    dest->enable_koppa_gated_engine = src->enable_koppa_gated_engine;
    dest->enable_delta_cross_propagation = src->enable_delta_cross_propagation;
    dest->enable_delta_koppa_offset = src->enable_delta_koppa_offset;
    dest->enable_ratio_threshold_psi = src->enable_ratio_threshold_psi;
    dest->enable_stack_depth_modes = src->enable_stack_depth_modes;
    dest->enable_epsilon_phi_triangle = src->enable_epsilon_phi_triangle;
    dest->enable_sign_flip = src->enable_sign_flip;
    dest->enable_modular_wrap = src->enable_modular_wrap;
    dest->enable_psi_strength_parameter = src->enable_psi_strength_parameter;
    dest->enable_ratio_snapshot_logging = src->enable_ratio_snapshot_logging;
    dest->enable_feedback_oscillator = src->enable_feedback_oscillator;
    dest->sign_flip_mode = src->sign_flip_mode;
    dest->koppa_wrap_threshold = src->koppa_wrap_threshold;
}

static void candidate_copy(Candidate *dest, const Candidate *src) {
    config_clone(&dest->config, &src->config);
    run_summary_copy(&dest->summary, &src->summary);
    dest->score = src->score;
    dest->evaluated = src->evaluated;
}

static long random_range(long min_value, long max_value) {
    long span = max_value - min_value + 1L;
    if (span <= 0L) {
        return min_value;
    }
    return min_value + (long)(rand() % (int)span);
}

static void mutate_seed(mpq_t value) {
    int choice = rand() % 4;
    if (choice == 0) {
        mpz_add_ui(mpq_numref(value), mpq_numref(value), 1UL);
    } else if (choice == 1) {
        mpz_sub_ui(mpq_numref(value), mpq_numref(value), 1UL);
    } else if (choice == 2) {
        unsigned long den = mpz_get_ui(mpq_denref(value));
        if (den > 1UL) {
            mpz_sub_ui(mpq_denref(value), mpq_denref(value), 1UL);
        }
    } else {
        mpz_add_ui(mpq_denref(value), mpq_denref(value), 1UL);
    }
}

static void randomize_config(Config *config) {
    config->engine_mode = ENGINE_MODES[rand() % ARRAY_COUNT(ENGINE_MODES)];
    config->engine_upsilon = track_mode_for_engine(config->engine_mode);
    config->engine_beta = track_mode_for_engine(config->engine_mode);
    config->psi_mode = PSI_MODES[rand() % ARRAY_COUNT(PSI_MODES)];
    config->koppa_mode = KOPPA_MODES[rand() % ARRAY_COUNT(KOPPA_MODES)];
    config->triple_psi_mode = (rand() % 2) == 0 ? false : true;
    config->multi_level_koppa = (rand() % 2) == 0 ? false : true;
    config->ticks = 25U + (size_t)(rand() % 10);

    long ups_num = random_range(1L, 8L);
    unsigned long ups_den = (unsigned long)random_range(1L, 8L);
    mpz_set_si(mpq_numref(config->initial_upsilon), ups_num);
    mpz_set_ui(mpq_denref(config->initial_upsilon), ups_den);

    long beta_num = random_range(1L, 8L);
    unsigned long beta_den = (unsigned long)random_range(1L, 8L);
    mpz_set_si(mpq_numref(config->initial_beta), beta_num);
    mpz_set_ui(mpq_denref(config->initial_beta), beta_den);
}

static void mutate_config(Config *config) {
    int mutations = 1 + rand() % 3;
    for (int i = 0; i < mutations; ++i) {
        int choice = rand() % 6;
        switch (choice) {
        case 0:
            config->engine_mode = ENGINE_MODES[rand() % ARRAY_COUNT(ENGINE_MODES)];
            config->engine_upsilon = track_mode_for_engine(config->engine_mode);
            config->engine_beta = track_mode_for_engine(config->engine_mode);
            break;
        case 1:
            config->psi_mode = PSI_MODES[rand() % ARRAY_COUNT(PSI_MODES)];
            break;
        case 2:
            config->koppa_mode = KOPPA_MODES[rand() % ARRAY_COUNT(KOPPA_MODES)];
            break;
        case 3:
            config->triple_psi_mode = !config->triple_psi_mode;
            break;
        case 4:
            mutate_seed(config->initial_upsilon);
            break;
        case 5:
        default:
            mutate_seed(config->initial_beta);
            break;
        }
    }
}

static double evaluate_candidate(Candidate *candidate, const EvolutionOptions *options) {
    if (!candidate->evaluated) {
        RunSummary summary;
        run_summary_init(&summary);
        bool ok = simulate_and_analyze(&candidate->config, &summary);
        if (!ok) {
            run_summary_clear(&summary);
            candidate->score = -INFINITY;
            candidate->evaluated = true;
            return candidate->score;
        }
        run_summary_copy(&candidate->summary, &summary);
        run_summary_clear(&summary);
        candidate->evaluated = true;
    }

    double target_value = 0.0;
    bool has_target = analysis_constant_value(options->target_constant, &target_value);

    const RunSummary *summary = &candidate->summary;
    double score = 0.0;

    if (strcmp(options->strategy, "target-convergence") == 0 && has_target) {
        double delta = fabs(summary->final_ratio_snapshot - target_value);
        if (!summary->ratio_defined) {
            score = -1e6;
        } else {
            score = 1000.0 / (delta + 1e-9);
            if (summary->convergence_tick > 0U) {
                score += 200.0 / (double)(summary->convergence_tick);
            }
            score += 25.0 / (summary->psi_spacing_stddev + 1.0);
            score -= summary->ratio_variance * 10.0;
        }
    } else if (strcmp(options->strategy, "chaos-seeker") == 0) {
        if (strcmp(summary->pattern, "divergent") == 0) {
            score = -1000.0;
        } else {
            score = summary->ratio_variance * 200.0 + (double)summary->psi_events * 5.0;
            if (strcmp(summary->pattern, "oscillating") == 0) {
                score += 250.0;
            }
        }
    } else {
        double delta = has_target ? fabs(summary->final_ratio_snapshot - target_value)
                                  : summary->closest_delta;
        if (!summary->ratio_defined) {
            score = -1e5;
        } else {
            score = 500.0 / (delta + 1e-8);
            if (summary->convergence_tick > 0U) {
                score += 150.0 / (double)summary->convergence_tick;
            }
            score += (double)summary->psi_events * 2.0;
            score -= summary->ratio_variance * 5.0;
        }
    }

    candidate->score = score;
    return score;
}

static int compare_candidates(const void *lhs, const void *rhs) {
    const Candidate *a = (const Candidate *)lhs;
    const Candidate *b = (const Candidate *)rhs;
    if (a->score < b->score) {
        return 1;
    }
    if (a->score > b->score) {
        return -1;
    }
    return 0;
}

static void print_candidate_summary(const Candidate *candidate, size_t generation_index,
                                    size_t rank) {
    const RunSummary *summary = &candidate->summary;
    printf("Gen %zu Rank %zu Score %.2f Pattern=%s Class=%s Ratio=%s Δ=%.6g Psi=%zu Stack=%s\n",
           generation_index, rank, candidate->score, summary->pattern, summary->classification,
           summary->final_ratio_str, summary->closest_delta, summary->psi_events,
           summary->stack_summary);
}

static void save_best_to_json(const Candidate *candidate, const EvolutionOptions *options) {
    if (!options->save_output) {
        return;
    }
    FILE *file = fopen(options->output_path, "w");
    if (!file) {
        return;
    }
    const RunSummary *summary = &candidate->summary;
    fprintf(file, "{\n");
    fprintf(file, "  \"score\": %.6f,\n", candidate->score);
    fprintf(file, "  \"pattern\": \"%s\",\n", summary->pattern);
    fprintf(file, "  \"classification\": \"%s\",\n", summary->classification);
    fprintf(file, "  \"final_ratio\": \"%s\",\n", summary->final_ratio_str);
    fprintf(file, "  \"closest_constant\": \"%s\",\n", summary->closest_constant);
    fprintf(file, "  \"delta\": %.12g,\n", summary->closest_delta);
    fprintf(file, "  \"convergence_tick\": %zu,\n", summary->convergence_tick);
    fprintf(file, "  \"psi_events\": %zu,\n", summary->psi_events);
    fprintf(file, "  \"rho_events\": %zu,\n", summary->rho_events);
    fprintf(file, "  \"mu_zero_events\": %zu,\n", summary->mu_zero_events);
    fprintf(file, "  \"psi_spacing_mean\": %.12g,\n", summary->psi_spacing_mean);
    fprintf(file, "  \"psi_spacing_stddev\": %.12g,\n", summary->psi_spacing_stddev);
    fprintf(file, "  \"ratio_variance\": %.12g,\n", summary->ratio_variance);
    fprintf(file, "  \"stack_summary\": \"%s\"\n", summary->stack_summary);
    fprintf(file, "}\n");
    fclose(file);
}

static void parse_arguments(int argc, char **argv, EvolutionOptions *options) {
    options->generations = 10U;
    options->population = 8U;
    options->elite = 2U;
    options->seed = (unsigned int)time(NULL);
    snprintf(options->strategy, sizeof(options->strategy), "%s", "hill-climb");
    snprintf(options->target_constant, sizeof(options->target_constant), "%s", "rho");
    options->save_output = false;
    options->output_path[0] = '\0';

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--generations") == 0 && i + 1 < argc) {
            options->generations = (size_t)strtoul(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--population") == 0 && i + 1 < argc) {
            options->population = (size_t)strtoul(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--elite") == 0 && i + 1 < argc) {
            options->elite = (size_t)strtoul(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            options->seed = (unsigned int)strtoul(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--strategy") == 0 && i + 1 < argc) {
            snprintf(options->strategy, sizeof(options->strategy), "%s", argv[++i]);
        } else if (strcmp(argv[i], "--target") == 0 && i + 1 < argc) {
            snprintf(options->target_constant, sizeof(options->target_constant), "%s", argv[++i]);
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            options->save_output = true;
            snprintf(options->output_path, sizeof(options->output_path), "%s", argv[++i]);
        }
    }

    if (options->elite == 0U || options->elite > options->population) {
        options->elite = 1U;
    }
}

int main(int argc, char **argv) {
    EvolutionOptions options;
    parse_arguments(argc, argv, &options);
    srand(options.seed);

    Candidate *population = malloc(sizeof(Candidate) * options.population);
    Candidate *next_population = malloc(sizeof(Candidate) * options.population);
    if (!population || !next_population) {
        fprintf(stderr, "Failed to allocate population buffers.\n");
        free(population);
        free(next_population);
        return 1;
    }

    for (size_t i = 0; i < options.population; ++i) {
        candidate_init(&population[i]);
        randomize_config(&population[i].config);
        population[i].evaluated = false;
    }

    for (size_t generation = 0; generation < options.generations; ++generation) {
        for (size_t i = 0; i < options.population; ++i) {
            evaluate_candidate(&population[i], &options);
        }

        qsort(population, options.population, sizeof(Candidate), compare_candidates);

        if (options.population > 0U) {
            print_candidate_summary(&population[0], generation, 0U);
        }

        for (size_t i = 0; i < options.population; ++i) {
            candidate_init(&next_population[i]);
        }

        size_t elite_count = options.elite;
        if (elite_count > options.population) {
            elite_count = options.population;
        }

        for (size_t i = 0; i < elite_count; ++i) {
            candidate_copy(&next_population[i], &population[i]);
        }

        for (size_t i = elite_count; i < options.population; ++i) {
            size_t parent_index = (size_t)(rand() % (int)elite_count);
            candidate_copy(&next_population[i], &population[parent_index]);
            mutate_config(&next_population[i].config);
            next_population[i].evaluated = false;
            next_population[i].score = 0.0;
        }

        for (size_t i = 0; i < options.population; ++i) {
            candidate_clear(&population[i]);
        }

        Candidate *temp = population;
        population = next_population;
        next_population = temp;
    }

    qsort(population, options.population, sizeof(Candidate), compare_candidates);
    if (options.population > 0U) {
        save_best_to_json(&population[0], &options);
        print_candidate_summary(&population[0], options.generations, 0U);
    }

    for (size_t i = 0; i < options.population; ++i) {
        candidate_clear(&population[i]);
    }
    free(population);
    free(next_population);
    return 0;
}
