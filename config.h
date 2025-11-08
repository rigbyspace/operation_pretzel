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

#ifndef CONFIG_H
#define CONFIG_H

#include <gmp.h>
#include <stdbool.h>
#include <stddef.h>

#include "rational.h"

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

typedef enum {
    SIGN_FLIP_NONE,
    SIGN_FLIP_ALWAYS,
    SIGN_FLIP_ALTERNATE
} SignFlipMode;

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

    bool enable_asymmetric_cascade;
    bool enable_conditional_triple_psi;
    bool enable_koppa_gated_engine;
    bool enable_delta_cross_propagation;
    bool enable_delta_koppa_offset;
    bool enable_ratio_threshold_psi;
    bool enable_stack_depth_modes;
    bool enable_epsilon_phi_triangle;
    bool enable_sign_flip;
    bool enable_modular_wrap;
    bool enable_psi_strength_parameter;
    bool enable_ratio_snapshot_logging;
    bool enable_feedback_oscillator;
    SignFlipMode sign_flip_mode;
    unsigned long koppa_wrap_threshold;
} Config;

void config_init(Config *config);
void config_clear(Config *config);

#endif // CONFIG_H
