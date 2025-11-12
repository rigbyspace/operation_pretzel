/*
 * NOTE: This file is a local copy of the TRTS engine configuration header.
 * It has been adapted from the upstream project to demonstrate how
 * additional experimental features might be integrated.  These additions
 * expose new configuration options without hard‑coding any constants.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <gmp.h>
#include <stdbool.h>
#include <stddef.h>

#include "rational.h"

/*
 * Enumerations governing the general behaviour of the engine.  The
 * existing PsiMode, KoppaMode, EngineMode and related enums mirror
 * those from the upstream project.  The RatioTriggerMode has been
 * extended with a custom range option to allow arbitrary ratio
 * windows to trigger ψ events (see RATIO_TRIGGER_CUSTOM below).
 */

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

/*
 * Ratio triggers specify ranges of υ/β that will automatically
 * trigger a ψ transform on μ-steps.  Standard options include the
 * golden ratio, √2 and the plastic constant.  A new CUSTOM option
 * allows callers to define their own lower and upper bounds via
 * ratio_custom_lower and ratio_custom_upper fields in the Config
 * struct.
 */

typedef enum {
    RATIO_TRIGGER_NONE,
    RATIO_TRIGGER_GOLDEN,
    RATIO_TRIGGER_SQRT2,
    RATIO_TRIGGER_PLASTIC,
    RATIO_TRIGGER_CUSTOM
} RatioTriggerMode;

typedef enum {
    SIGN_FLIP_NONE,
    SIGN_FLIP_ALWAYS,
    SIGN_FLIP_ALTERNATE
} SignFlipMode;

/*
 * Primary configuration structure controlling engine behaviour.  The
 * boolean flags enable optional features.  Newly added fields below
 * are documented inline.  Any mpq_t or mpz_t members must be
 * initialised via config_init() and cleared via config_clear() to
 * avoid memory leaks.
 */

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

    /* Existing feature toggles */
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
    bool enable_fibonacci_gate;
    SignFlipMode sign_flip_mode;
    unsigned long koppa_wrap_threshold;

    /*
     * Custom ratio window: when ratio_trigger_mode is RATIO_TRIGGER_CUSTOM,
     * the lower and upper bounds of the window are taken from these
     * mpq_t fields.  The ψ transform fires when |υ/β| lies strictly
     * between them.  Use config_init() to set defaults and
     * config_clear() to free them.
     */
    bool enable_ratio_custom_range;
    mpq_t ratio_custom_lower;
    mpq_t ratio_custom_upper;

    /*
     * Additional prime pattern triggers: twin primes (both
     * numerator and denominator are prime and differ by ±2),
     * Fibonacci numbers and perfect powers.  When enabled these
     * cause ρ to latch in the same way as standard prime
     * detection.
     */
    bool enable_twin_prime_trigger;
    bool enable_fibonacci_trigger;
    bool enable_perfect_power_trigger;

    /*
     * Modular arithmetic bound: when enable_modular_wrap is true
     * and either the numerator or denominator of a rational exceeds
     * this modulus, the value is reduced modulo this bound.  This
     * value is an arbitrary precision integer and must be initialised
     * and cleared via config_init()/config_clear().
     */
    mpz_t modulus_bound;
} Config;

/* Initialise a Config with sane defaults.  Allocates internal GMP
 * objects.  Call config_clear() when done.
 */
#ifdef __cplusplus
extern "C" {
#endif

void config_init(Config *config);

void config_clear(Config *config);

#ifdef __cplusplus
}
#endif

/* Free any GMP allocations within a Config.  Must be called on
 * every Config initialised via config_init().
 */
void config_clear(Config *config);

#endif /* CONFIG_H */
