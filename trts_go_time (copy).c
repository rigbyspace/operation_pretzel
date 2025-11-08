#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <gmp.h>

// ==================== DATA STRUCTURES & ENUMS ====================

typedef struct {
    mpz_t num;
    mpz_t den;
} Rational;

typedef struct {
    Rational upsilon;   // υ
    Rational beta;      // β  
    Rational koppa;     // ϙ
    Rational memory;    // Stores υ during E mt (epsilon, ε)
    Rational koppa_stack[4]; // Enhanced stack for multi-level
    int koppa_stack_ptr;
    bool rho_inhibit_next_mu;
    bool rho_detected_next_mu;
} TRTS_State;

// Original modes
typedef enum { PSI_MODE_TERM, PSI_MODE_MSTEP, PSI_MODE_MSTEP_RHO, PSI_MODE_RHO_ONLY, PSI_MODE_INHIBIT_RHO } PsiMode;
typedef enum { KOPPA_MODE_DUMP, KOPPA_MODE_POP, KOPPA_MODE_ACCUMULATE } KoppaMode;
typedef enum { ENGINE_MODE_ADD, ENGINE_MODE_MULTI, ENGINE_MODE_SLIDE } EngineMode;

// Experimental dimensions
typedef enum { PRIME_ON_MEMORY, PRIME_ON_NEW_U } PrimeCheckTarget;
typedef enum { KOPPA_ON_PSI, KOPPA_ON_MU_AFTER_PSI, KOPPA_ON_ALL_MU } KoppaAccrualTrigger;
typedef enum { MT10_FORCED_EMISSION_ONLY, MT10_FORCED_PSI } MT10Behavior;

// Architectural variations
typedef enum { 
    ENGINE_UPSILON_ADD, ENGINE_UPSILON_MULTI, ENGINE_UPSILON_SLIDE 
} EngineUpsilonMode;

typedef enum { 
    ENGINE_BETA_ADD, ENGINE_BETA_MULTI, ENGINE_BETA_SLIDE 
} EngineBetaMode;

typedef enum { 
    RATIO_TRIGGER_NONE, RATIO_TRIGGER_GOLDEN, RATIO_TRIGGER_SQRT2, RATIO_TRIGGER_PLASTIC 
} RatioTriggerMode;

typedef struct {
    // Standard options
    PsiMode psi_mode;
    KoppaMode koppa_mode;
    EngineMode engine_mode;
    Rational seed_upsilon;
    Rational seed_beta;
    int ticks;
    
    // Experimental dimensions
    PrimeCheckTarget prime_target;
    KoppaAccrualTrigger koppa_trigger;
    MT10Behavior mt10_behavior;
    
    // Architectural variations
    bool dual_track_mode;
    EngineUpsilonMode engine_upsilon;
    EngineBetaMode engine_beta;
    
    bool triple_psi_mode;
    
    RatioTriggerMode ratio_trigger_mode;
    
    bool multi_level_koppa;
    bool reverse_causality_mode;
    bool alternating_sign_mode;
    
    // Output
    char output_prefix[100];
} Config;

// ==================== RATIONAL NUMBER UTILITIES ====================

void rational_init(Rational *r) {
    mpz_init(r->num);
    mpz_init(r->den);
    mpz_set_ui(r->num, 0);
    mpz_set_ui(r->den, 1);
}

void rational_set(Rational *r, long num, long den) {
    mpz_set_si(r->num, num);
    mpz_set_si(r->den, den);
}

void rational_clear(Rational *r) {
    mpz_clear(r->num);
    mpz_clear(r->den);
}

void rational_copy(Rational *dest, const Rational *src) {
    mpz_set(dest->num, src->num);
    mpz_set(dest->den, src->den);
}

bool rational_is_zero(const Rational *r) {
    return mpz_cmp_ui(r->num, 0) == 0;
}

void rational_add(Rational *result, const Rational *a, const Rational *b) {
    mpz_t temp1, temp2;
    mpz_init(temp1);
    mpz_init(temp2);
    
    mpz_mul(temp1, a->num, b->den);
    mpz_mul(temp2, b->num, a->den);
    mpz_add(result->num, temp1, temp2);
    mpz_mul(result->den, a->den, b->den);
    
    mpz_clear(temp1);
    mpz_clear(temp2);
}

void rational_multiply(Rational *result, const Rational *a, const Rational *b) {
    mpz_mul(result->num, a->num, b->num);
    mpz_mul(result->den, a->den, b->den);
}

void rational_divide(Rational *result, const Rational *a, const Rational *b) {
    mpz_mul(result->num, a->num, b->den);
    mpz_mul(result->den, a->den, b->num);
}

void rational_print(FILE *stream, const Rational *r) {
    gmp_fprintf(stream, "%Zd/%Zd", r->num, r->den);
}

double rational_to_double(const Rational *r) {
    if (rational_is_zero(r)) return 0.0;
    return mpz_get_d(r->num) / mpz_get_d(r->den);
}

// ==================== ARCHITECTURAL VARIATIONS ====================

// Dual-track propagation
void dual_track_emission(TRTS_State *state, Config *config, int mt) {
    Rational temp1, temp2;
    rational_init(&temp1);
    rational_init(&temp2);
    
    // Upsilon track
    switch (config->engine_upsilon) {
        case ENGINE_UPSILON_MULTI:
            rational_add(&temp1, &state->beta, &state->koppa);
            rational_multiply(&state->upsilon, &state->upsilon, &temp1);
            break;
        case ENGINE_UPSILON_ADD:
            rational_add(&temp1, &state->upsilon, &state->beta);
            rational_add(&state->upsilon, &temp1, &state->koppa);
            break;
        case ENGINE_UPSILON_SLIDE:
            rational_add(&temp1, &state->upsilon, &state->beta);
            if (rational_is_zero(&state->koppa)) {
                rational_add(&state->upsilon, &temp1, &state->koppa);
            } else {
                rational_divide(&state->upsilon, &temp1, &state->koppa);
            }
            break;
    }
    
    // Beta track  
    switch (config->engine_beta) {
        case ENGINE_BETA_ADD:
            rational_add(&temp1, &state->beta, &state->upsilon);
            rational_add(&state->beta, &temp1, &state->koppa);
            break;
        case ENGINE_BETA_MULTI:
            rational_add(&temp1, &state->upsilon, &state->koppa);
            rational_multiply(&state->beta, &state->beta, &temp1);
            break;
        case ENGINE_BETA_SLIDE:
            rational_add(&temp1, &state->beta, &state->upsilon);
            if (rational_is_zero(&state->koppa)) {
                rational_add(&state->beta, &temp1, &state->koppa);
            } else {
                rational_divide(&state->beta, &temp1, &state->koppa);
            }
            break;
    }
    
    rational_clear(&temp1);
    rational_clear(&temp2);
}

// Triple psi transform
void triple_psi_transform(Rational *u, Rational *b, Rational *k) {
    mpz_t u_num, u_den, b_num, b_den, k_num, k_den;
    mpz_init_set(u_num, u->num);
    mpz_init_set(u_den, u->den);
    mpz_init_set(b_num, b->num);
    mpz_init_set(b_den, b->den);
    mpz_init_set(k_num, k->num);
    mpz_init_set(k_den, k->den);

    // (υ, β, ϙ) → (β/υ, ϙ/β, υ/ϙ)
    mpz_set(u->num, b_den);
    mpz_set(u->den, u_num);
    
    mpz_set(b->num, k_den);
    mpz_set(b->den, b_num);
    
    mpz_set(k->num, u_den);
    mpz_set(k->den, k_num);

    mpz_clears(u_num, u_den, b_num, b_den, k_num, k_den, NULL);
}

// Ratio-based triggers
bool ratio_trigger(const Rational *u, const Rational *b, RatioTriggerMode mode) {
    double ratio = rational_to_double(u) / rational_to_double(b);
    
    switch (mode) {
        case RATIO_TRIGGER_GOLDEN:
            return (ratio > 1.5 && ratio < 1.7);
        case RATIO_TRIGGER_SQRT2:
            return (ratio > 1.3 && ratio < 1.5);
        case RATIO_TRIGGER_PLASTIC:
            return (ratio > 1.2 && ratio < 1.4);
        default:
            return false;
    }
}

// Multi-level koppa stack
void multi_level_koppa_accrue(TRTS_State *state, Config *config, int mt, bool psi_triggered) {
    if (config->multi_level_koppa && psi_triggered) {
        // Shift stack
        for (int i = 3; i > 0; i--) {
            rational_copy(&state->koppa_stack[i], &state->koppa_stack[i-1]);
        }
        rational_copy(&state->koppa_stack[0], &state->koppa);
        
        // Accrue current state
        Rational temp;
        rational_init(&temp);
        rational_add(&temp, &state->upsilon, &state->beta);
        rational_add(&state->koppa, &state->koppa, &temp);
        rational_clear(&temp);
    }
}

// ==================== CORE TRTS OPERATIONS ====================

bool is_prime(const mpz_t n) {
    if (mpz_cmp_ui(n, 2) < 0) return false;
    if (mpz_cmp_ui(n, 2) == 0) return true;
    if (mpz_even_p(n)) return false;
    
    mpz_t i, limit;
    mpz_init(i);
    mpz_init(limit);
    
    mpz_sqrt(limit, n);
    mpz_add_ui(limit, limit, 1);
    
    bool prime = true;
    for (mpz_set_ui(i, 3); mpz_cmp(i, limit) <= 0; mpz_add_ui(i, i, 2)) {
        if (mpz_divisible_p(n, i)) {
            prime = false;
            break;
        }
    }
    
    mpz_clear(i);
    mpz_clear(limit);
    return prime;
}

bool detect_rho_event(const Rational *r) {
    return is_prime(r->num) || is_prime(r->den);
}

void psi_transform(Rational *u, Rational *b) {
    mpz_t a_num, a_den, c_num, c_den;
    mpz_init_set(a_num, u->num);
    mpz_init_set(a_den, u->den);
    mpz_init_set(c_num, b->num);
    mpz_init_set(c_den, b->den);

    mpz_set(u->num, c_den);
    mpz_set(u->den, a_num);

    mpz_set(b->num, a_den);
    mpz_set(b->den, c_num);

    mpz_clears(a_num, a_den, c_num, c_den, NULL);
}

void koppa_accrue(TRTS_State *state, Config *config, int mt, bool psi_triggered_this_tick) {
    bool should_accrue = false;
    
    switch (config->koppa_trigger) {
        case KOPPA_ON_PSI:
            should_accrue = psi_triggered_this_tick;
            break;
        case KOPPA_ON_MU_AFTER_PSI:
            should_accrue = (mt == 5 || mt == 8 || mt == 11) && psi_triggered_this_tick;
            break;
        case KOPPA_ON_ALL_MU:
            should_accrue = (mt == 5 || mt == 8 || mt == 11);
            break;
    }
    
    if (should_accrue) {
        if (config->multi_level_koppa) {
            multi_level_koppa_accrue(state, config, mt, psi_triggered_this_tick);
        } else {
            Rational temp_sum;
            rational_init(&temp_sum);
            rational_add(&temp_sum, &state->upsilon, &state->beta);
            rational_add(&state->koppa, &state->koppa, &temp_sum);
            rational_clear(&temp_sum);
        }
    }
}

void koppa_dump(TRTS_State *state, Config *config) {
    if (config->koppa_mode == KOPPA_MODE_DUMP) {
        rational_set(&state->koppa, 0, 1);
    } else if (config->koppa_mode == KOPPA_MODE_POP) {
        if (state->koppa_stack_ptr > 0) {
            rational_copy(&state->koppa, &state->koppa_stack[0]);
            state->koppa_stack_ptr = 0;
        }
    }
}

// ==================== SIMULATION ENGINE ====================

void simulate(TRTS_State *state, Config *config) {
    FILE *events_csv = fopen("events.csv", "w");
    FILE *values_csv = fopen("values.csv", "w");
    
    fprintf(events_csv, "tick,mt,event_type,rho_event,psi_fired,mu_zero,forced_emission\n");
    fprintf(values_csv, "tick,mt,upsilon_num,upsilon_den,beta_num,beta_den,koppa_num,koppa_den,memory_num,memory_den,phi_num,phi_den\n");
    
    for (int tick = 1; tick <= config->ticks; tick++) {
        bool psi_triggered_this_tick = false;
        
        if (config->koppa_mode == KOPPA_MODE_POP) {
            rational_copy(&state->koppa_stack[0], &state->koppa);
        }
        
        for (int mt = 1; mt <= 11; mt++) {
            char event_type = '?';
            bool rho_detected = false;
            bool psi_fired = false;
            bool mu_zero = false;
            bool forced_emission = false;
            Rational phi;
            rational_init(&phi);
            
            if (mt == 1 || mt == 4 || mt == 7 || mt == 10) {
                event_type = 'E';
                rational_copy(&state->memory, &state->upsilon);
                
                // Engine operation
                if (config->dual_track_mode) {
                    dual_track_emission(state, config, mt);
                } else {
                    Rational temp1, temp2;
                    rational_init(&temp1);
                    rational_init(&temp2);
                    
                    switch (config->engine_mode) {
                        case ENGINE_MODE_ADD:
                            rational_add(&temp1, &state->upsilon, &state->beta);
                            rational_add(&state->upsilon, &temp1, &state->koppa);
                            break;
                        case ENGINE_MODE_MULTI:
                            rational_add(&temp1, &state->beta, &state->koppa);
                            rational_multiply(&state->upsilon, &state->upsilon, &temp1);
                            break;
                        case ENGINE_MODE_SLIDE:
                            rational_add(&temp1, &state->upsilon, &state->beta);
                            if (rational_is_zero(&state->koppa)) {
                                rational_add(&state->upsilon, &temp1, &state->koppa);
                            } else {
                                rational_divide(&state->upsilon, &temp1, &state->koppa);
                            }
                            break;
                    }
                    rational_clear(&temp1);
                    rational_clear(&temp2);
                }
                
                // ρ detection
                Rational *rho_target = (config->prime_target == PRIME_ON_MEMORY) 
                    ? &state->memory : &state->upsilon;
                rho_detected = detect_rho_event(rho_target);
                
                if (config->reverse_causality_mode) {
                    if (rho_detected) {
                        state->rho_inhibit_next_mu = true;
                    }
                } else {
                    if (rho_detected) {
                        state->rho_detected_next_mu = true;
                    }
                }
                
                // MT10 forced behavior
                if (mt == 10) {
                    if (config->mt10_behavior == MT10_FORCED_PSI) {
                        forced_emission = true;
                        if (config->triple_psi_mode) {
                            triple_psi_transform(&state->upsilon, &state->beta, &state->koppa);
                        } else {
                            psi_transform(&state->upsilon, &state->beta);
                        }
                        psi_fired = true;
                        psi_triggered_this_tick = true;
                    }
                }
                
            } else if (mt == 5 || mt == 8 || mt == 11) {
                event_type = 'M';
                mu_zero = rational_is_zero(&state->upsilon);
                rational_set(&state->memory, 0, 1);
                
                // ψ application logic
                bool should_trigger_psi = false;
                
                if (config->reverse_causality_mode) {
                    // Reverse causality: ψ fires by default unless inhibited
                    should_trigger_psi = !state->rho_inhibit_next_mu;
                    state->rho_inhibit_next_mu = false;
                } else {
                    // Standard causality
                    if (state->rho_detected_next_mu) {
                        if (config->psi_mode != PSI_MODE_INHIBIT_RHO) {
                            should_trigger_psi = true;
                        }
                        state->rho_detected_next_mu = false;
                    }
                    
                    if (config->psi_mode == PSI_MODE_MSTEP || config->psi_mode == PSI_MODE_MSTEP_RHO) {
                        should_trigger_psi = true;
                    }
                    
                    if (config->psi_mode == PSI_MODE_INHIBIT_RHO) {
                        should_trigger_psi = !state->rho_inhibit_next_mu;
                        state->rho_inhibit_next_mu = false;
                    }
                }
                
                // Ratio-based triggers
                if (config->ratio_trigger_mode != RATIO_TRIGGER_NONE) {
                    if (ratio_trigger(&state->upsilon, &state->beta, config->ratio_trigger_mode)) {
                        should_trigger_psi = true;
                    }
                }
                
                if (should_trigger_psi) {
                    if (config->triple_psi_mode) {
                        triple_psi_transform(&state->upsilon, &state->beta, &state->koppa);
                    } else {
                        psi_transform(&state->upsilon, &state->beta);
                    }
                    psi_fired = true;
                    psi_triggered_this_tick = true;
                }
                
            } else {
                event_type = 'R';
                rational_copy(&phi, &state->upsilon);
                rational_add(&state->beta, &state->beta, &state->upsilon);
            }
            
            // Koppa accrual
            koppa_accrue(state, config, mt, psi_triggered_this_tick);
            
            // Logging
            fprintf(events_csv, "%d,%d,%c,%d,%d,%d,%d\n", 
                   tick, mt, event_type, rho_detected, psi_fired, mu_zero, forced_emission);
            
            fprintf(values_csv, "%d,%d,", tick, mt);
            gmp_fprintf(values_csv, "%Zd,%Zd,", state->upsilon.num, state->upsilon.den);
            gmp_fprintf(values_csv, "%Zd,%Zd,", state->beta.num, state->beta.den);
            gmp_fprintf(values_csv, "%Zd,%Zd,", state->koppa.num, state->koppa.den);
            gmp_fprintf(values_csv, "%Zd,%Zd,", state->memory.num, state->memory.den);
            gmp_fprintf(values_csv, "%Zd,%Zd\n", phi.num, phi.den);
            
            rational_clear(&phi);
        }
        
        koppa_dump(state, config);
    }
    
    fclose(events_csv);
    fclose(values_csv);
}

// ==================== CONFIGURATION & MAIN ====================

void usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("\nStandard Options:\n");
    printf("  --psi-mode MODE          Psi trigger (term, mstep, mstep_rho, rho_only, inhibit_rho)\n");
    printf("  --koppa MODE             Koppa behavior (dump, pop, accumulate)\n");
    printf("  --engine MODE            Engine mode (add, multi, slide)\n");
    printf("  --ticks N                Number of ticks (default: 10)\n");
    printf("  --seed-u NUM/DEN         Initial upsilon (default: 3/5)\n");
    printf("  --seed-b NUM/DEN         Initial beta (default: 5/7)\n");
    printf("\nExperimental Dimensions:\n");
    printf("  --prime-target T         Prime check (memory, new_u)\n");
    printf("  --koppa-trigger T        Koppa accrual (on_psi, on_mu_after_psi, on_all_mu)\n");
    printf("  --mt10-behavior T        MT10 behavior (forced_emission, forced_psi)\n");
    printf("\nArchitectural Variations:\n");
    printf("  --dual-track U_BETA      Dual-track engines (e.g., multi-add, add-multi, slide-add)\n");
    printf("  --triple-psi             Enable triple psi transform\n");
    printf("  --ratio-trigger MODE     Ratio triggers (golden, sqrt2, plastic)\n");
    printf("  --multi-level-koppa      Enable multi-level koppa stack\n");
    printf("  --reverse-causality      Reverse causality mode\n");
    printf("  --alternating-sign       Enable alternating signs\n");
    printf("  --output PREFIX          Output file prefix\n");
    printf("  --help                   Show this help\n");
}

void parse_arguments(int argc, char *argv[], Config *config) {
    // Set defaults
    config->psi_mode = PSI_MODE_RHO_ONLY;
    config->koppa_mode = KOPPA_MODE_DUMP;
    config->engine_mode = ENGINE_MODE_ADD;
    config->ticks = 10;
    rational_set(&config->seed_upsilon, 3, 5);
    rational_set(&config->seed_beta, 5, 7);
    
    config->prime_target = PRIME_ON_MEMORY;
    config->koppa_trigger = KOPPA_ON_PSI;
    config->mt10_behavior = MT10_FORCED_EMISSION_ONLY;
    
    config->dual_track_mode = false;
    config->engine_upsilon = ENGINE_UPSILON_MULTI;
    config->engine_beta = ENGINE_BETA_ADD;
    
    config->triple_psi_mode = false;
    config->ratio_trigger_mode = RATIO_TRIGGER_NONE;
    config->multi_level_koppa = false;
    config->reverse_causality_mode = false;
    config->alternating_sign_mode = false;
    
    strcpy(config->output_prefix, "test");
    
    // Parse command line
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--psi-mode") == 0) {
            i++;
            if (strcmp(argv[i], "term") == 0) config->psi_mode = PSI_MODE_TERM;
            else if (strcmp(argv[i], "mstep") == 0) config->psi_mode = PSI_MODE_MSTEP;
            else if (strcmp(argv[i], "mstep_rho") == 0) config->psi_mode = PSI_MODE_MSTEP_RHO;
            else if (strcmp(argv[i], "rho_only") == 0) config->psi_mode = PSI_MODE_RHO_ONLY;
            else if (strcmp(argv[i], "inhibit_rho") == 0) config->psi_mode = PSI_MODE_INHIBIT_RHO;
        }
        else if (strcmp(argv[i], "--koppa") == 0) {
            i++;
            if (strcmp(argv[i], "dump") == 0) config->koppa_mode = KOPPA_MODE_DUMP;
            else if (strcmp(argv[i], "pop") == 0) config->koppa_mode = KOPPA_MODE_POP;
            else if (strcmp(argv[i], "accumulate") == 0) config->koppa_mode = KOPPA_MODE_ACCUMULATE;
        }
        else if (strcmp(argv[i], "--engine") == 0) {
            i++;
            if (strcmp(argv[i], "add") == 0) config->engine_mode = ENGINE_MODE_ADD;
            else if (strcmp(argv[i], "multi") == 0) config->engine_mode = ENGINE_MODE_MULTI;
            else if (strcmp(argv[i], "slide") == 0) config->engine_mode = ENGINE_MODE_SLIDE;
        }
        else if (strcmp(argv[i], "--prime-target") == 0) {
            i++;
            if (strcmp(argv[i], "memory") == 0) config->prime_target = PRIME_ON_MEMORY;
            else if (strcmp(argv[i], "new_u") == 0) config->prime_target = PRIME_ON_NEW_U;
        }
        else if (strcmp(argv[i], "--koppa-trigger") == 0) {
            i++;
            if (strcmp(argv[i], "on_psi") == 0) config->koppa_trigger = KOPPA_ON_PSI;
            else if (strcmp(argv[i], "on_mu_after_psi") == 0) config->koppa_trigger = KOPPA_ON_MU_AFTER_PSI;
            else if (strcmp(argv[i], "on_all_mu") == 0) config->koppa_trigger = KOPPA_ON_ALL_MU;
        }
        else if (strcmp(argv[i], "--mt10-behavior") == 0) {
            i++;
            if (strcmp(argv[i], "forced_emission") == 0) config->mt10_behavior = MT10_FORCED_EMISSION_ONLY;
            else if (strcmp(argv[i], "forced_psi") == 0) config->mt10_behavior = MT10_FORCED_PSI;
        }
        else if (strcmp(argv[i], "--dual-track") == 0) {
            i++;
            config->dual_track_mode = true;
            if (strstr(argv[i], "multi") && strstr(argv[i], "add")) {
                config->engine_upsilon = ENGINE_UPSILON_MULTI;
                config->engine_beta = ENGINE_BETA_ADD;
            } else if (strstr(argv[i], "add") && strstr(argv[i], "multi")) {
                config->engine_upsilon = ENGINE_UPSILON_ADD;
                config->engine_beta = ENGINE_BETA_MULTI;
            } else if (strstr(argv[i], "slide") && strstr(argv[i], "add")) {
                config->engine_upsilon = ENGINE_UPSILON_SLIDE;
                config->engine_beta = ENGINE_BETA_ADD;
            }
        }
        else if (strcmp(argv[i], "--triple-psi") == 0) {
            config->triple_psi_mode = true;
        }
        else if (strcmp(argv[i], "--ratio-trigger") == 0) {
            i++;
            if (strcmp(argv[i], "golden") == 0) config->ratio_trigger_mode = RATIO_TRIGGER_GOLDEN;
            else if (strcmp(argv[i], "sqrt2") == 0) config->ratio_trigger_mode = RATIO_TRIGGER_SQRT2;
            else if (strcmp(argv[i], "plastic") == 0) config->ratio_trigger_mode = RATIO_TRIGGER_PLASTIC;
        }
        else if (strcmp(argv[i], "--multi-level-koppa") == 0) {
            config->multi_level_koppa = true;
        }
        else if (strcmp(argv[i], "--reverse-causality") == 0) {
            config->reverse_causality_mode = true;
        }
        else if (strcmp(argv[i], "--alternating-sign") == 0) {
            config->alternating_sign_mode = true;
        }
        else if (strcmp(argv[i], "--ticks") == 0) {
            i++;
            config->ticks = atoi(argv[i]);
        }
        else if (strcmp(argv[i], "--seed-u") == 0) {
            i++;
            long num, den;
            sscanf(argv[i], "%ld/%ld", &num, &den);
            rational_set(&config->seed_upsilon, num, den);
        }
        else if (strcmp(argv[i], "--seed-b") == 0) {
            i++;
            long num, den;
            sscanf(argv[i], "%ld/%ld", &num, &den);
            rational_set(&config->seed_beta, num, den);
        }
        else if (strcmp(argv[i], "--output") == 0) {
            i++;
            strcpy(config->output_prefix, argv[i]);
        }
        else if (strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            exit(0);
        }
    }
}

int main(int argc, char *argv[]) {
    Config config;
    TRTS_State state;
    
    // Initialize state
    rational_init(&state.upsilon);
    rational_init(&state.beta);
    rational_init(&state.koppa);
    rational_init(&state.memory);
    for (int i = 0; i < 4; i++) {
        rational_init(&state.koppa_stack[i]);
    }
    state.koppa_stack_ptr = 0;
    state.rho_inhibit_next_mu = false;
    state.rho_detected_next_mu = false;
    
    // Parse configuration
    parse_arguments(argc, argv, &config);
    
    // Initialize state from config
    rational_copy(&state.upsilon, &config.seed_upsilon);
    rational_copy(&state.beta, &config.seed_beta);
    rational_set(&state.koppa, 0, 1);
    rational_set(&state.memory, 0, 1);
    
    printf("=== RIGBYSPACE ARCHITECTURAL EXPLORER ===\n");
    printf("Configuration:\n");
    printf("  Psi Mode: %d, Koppa Mode: %d, Engine: %d\n", 
           config.psi_mode, config.koppa_mode, config.engine_mode);
    printf("  Prime Target: %s, Koppa Trigger: %s, MT10: %s\n",
           config.prime_target == PRIME_ON_MEMORY ? "memory" : "new_u",
           config.koppa_trigger == KOPPA_ON_PSI ? "on_psi" : 
           config.koppa_trigger == KOPPA_ON_MU_AFTER_PSI ? "on_mu_after_psi" : "on_all_mu",
           config.mt10_behavior == MT10_FORCED_EMISSION_ONLY ? "forced_emission" : "forced_psi");
    
    if (config.dual_track_mode) {
        printf("  Dual-Track: upsilon=%d, beta=%d\n", config.engine_upsilon, config.engine_beta);
    }
    if (config.triple_psi_mode) printf("  Triple Psi: ENABLED\n");
    if (config.ratio_trigger_mode != RATIO_TRIGGER_NONE) printf("  Ratio Trigger: %d\n", config.ratio_trigger_mode);
    if (config.multi_level_koppa) printf("  Multi-Level Koppa: ENABLED\n");
    if (config.reverse_causality_mode) printf("  Reverse Causality: ENABLED\n");
    if (config.alternating_sign_mode) printf("  Alternating Sign: ENABLED\n");
    
    printf("  Ticks: %d\n", config.ticks);
    printf("  Initial υ: "); rational_print(stdout, &state.upsilon); printf("\n");
    printf("  Initial β: "); rational_print(stdout, &state.beta); printf("\n");
    
    // Run simulation
    simulate(&state, &config);
    
    printf("\nSimulation complete. Check events.csv and values.csv\n");
    
    // Cleanup
    rational_clear(&state.upsilon);
    rational_clear(&state.beta);
    rational_clear(&state.koppa);
    rational_clear(&state.memory);
    for (int i = 0; i < 4; i++) {
        rational_clear(&state.koppa_stack[i]);
    }
    
    return 0;
}
