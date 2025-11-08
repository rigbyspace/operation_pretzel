#ifndef CONFIG_H
#define CONFIG_H

#include <gmp.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    PSI_MODE_MSTEP,
    PSI_MODE_RHO_ONLY,
    PSI_MODE_MSTEP_RHO,
    PSI_MODE_INHIBIT_RHO
} PsiMode;

typedef enum {
    KOPPA_MODE_DUMP,
    KOPPA_MODE_POP,
    KOPPA_MODE_ACCUMULATE
} KoppaMode;

typedef enum {
    ENGINE_MODE_ADD,
    ENGINE_MODE_MULTI,
    ENGINE_MODE_SLIDE
} EngineMode;

typedef enum {
    KOPPA_ON_PSI,
    KOPPA_ON_MU_AFTER_PSI,
    KOPPA_ON_ALL_MU
} KoppaTrigger;

typedef enum {
    PRIME_ON_MEMORY,
    PRIME_ON_NEW_UPSILON
} PrimeTarget;

typedef enum {
    MT10_FORCED_EMISSION_ONLY,
    MT10_FORCED_PSI
} Mt10Behavior;

typedef struct {
    PsiMode psi_mode;
    KoppaMode koppa_mode;
    EngineMode engine_mode;
    KoppaTrigger koppa_trigger;
    PrimeTarget prime_target;
    Mt10Behavior mt10_behavior;
    size_t ticks;
    mpq_t initial_upsilon;
    mpq_t initial_beta;
    mpq_t initial_koppa;
} Config;

void config_init(Config *config);
void config_clear(Config *config);

#endif // CONFIG_H
