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
    ENGINE_MODE_SLIDE,
    ENGINE_MODE_DELTA_ADD
} EngineMode;

typedef enum {
    ENGINE_TRACK_ADD,
    ENGINE_TRACK_MULTI,
    ENGINE_TRACK_SLIDE
} EngineTrackMode;

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

typedef enum {
    RATIO_TRIGGER_NONE,
    RATIO_TRIGGER_GOLDEN,
    RATIO_TRIGGER_SQRT2,
    RATIO_TRIGGER_PLASTIC
} RatioTriggerMode;

typedef struct {
    PsiMode psi_mode;
    KoppaMode koppa_mode;
    EngineMode engine_mode;
    EngineTrackMode engine_upsilon;
    EngineTrackMode engine_beta;
    bool dual_track_mode;
    bool triple_psi_mode;
    bool multi_level_koppa;
    KoppaTrigger koppa_trigger;
    PrimeTarget prime_target;
    Mt10Behavior mt10_behavior;
    RatioTriggerMode ratio_trigger_mode;
    size_t ticks;
    mpq_t initial_upsilon;
    mpq_t initial_beta;
    mpq_t initial_koppa;
} Config;

void config_init(Config *config);
void config_clear(Config *config);

#endif // CONFIG_H
