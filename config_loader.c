#include "config_loader.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rational.h"

static void write_error(char *buffer, size_t capacity, const char *message) {
    if (!buffer || capacity == 0) {
        return;
    }
    snprintf(buffer, capacity, "%s", message);
}

static const char *skip_whitespace(const char *text) {
    while (text && *text && isspace((unsigned char)*text)) {
        ++text;
    }
    return text;
}

static const char *find_value_start(const char *json, const char *key) {
    if (!json || !key) {
        return NULL;
    }

    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char *location = strstr(json, pattern);
    if (!location) {
        return NULL;
    }

    location += strlen(pattern);
    location = strchr(location, ':');
    if (!location) {
        return NULL;
    }

    return skip_whitespace(location + 1);
}

static bool json_extract_int(const char *json, const char *key, int *out_value) {
    const char *start = find_value_start(json, key);
    if (!start) {
        return false;
    }

    char *end_ptr = NULL;
    long value = strtol(start, &end_ptr, 10);
    if (start == end_ptr) {
        return false;
    }
    if (out_value) {
        *out_value = (int)value;
    }
    return true;
}

static bool json_extract_unsigned(const char *json, const char *key, unsigned long *out_value) {
    const char *start = find_value_start(json, key);
    if (!start) {
        return false;
    }

    char *end_ptr = NULL;
    unsigned long value = strtoul(start, &end_ptr, 10);
    if (start == end_ptr) {
        return false;
    }
    if (out_value) {
        *out_value = value;
    }
    return true;
}

static bool json_extract_bool(const char *json, const char *key, bool *out_value) {
    const char *start = find_value_start(json, key);
    if (!start) {
        return false;
    }

    if (strncmp(start, "true", 4) == 0) {
        if (out_value) {
            *out_value = true;
        }
        return true;
    }
    if (strncmp(start, "false", 5) == 0) {
        if (out_value) {
            *out_value = false;
        }
        return true;
    }
    return false;
}

static bool json_extract_string(const char *json, const char *key, char *buffer, size_t capacity) {
    const char *start = find_value_start(json, key);
    if (!start || *start != '"') {
        return false;
    }
    ++start;

    const char *end = strchr(start, '"');
    if (!end) {
        return false;
    }

    size_t length = (size_t)(end - start);
    if (length + 1 >= capacity) {
        return false;
    }

    memcpy(buffer, start, length);
    buffer[length] = '\0';
    return true;
}

static bool parse_rational_string(const char *text, mpq_t value) {
    if (!text || !value) {
        return false;
    }

    const char *slash = strchr(text, '/');
    if (!slash) {
        return false;
    }

    char numerator_buffer[128];
    char denominator_buffer[128];

    size_t numerator_length = (size_t)(slash - text);
    size_t denominator_length = strlen(slash + 1);
    if (numerator_length == 0 || denominator_length == 0 ||
        numerator_length >= sizeof(numerator_buffer) ||
        denominator_length >= sizeof(denominator_buffer)) {
        return false;
    }

    memcpy(numerator_buffer, text, numerator_length);
    numerator_buffer[numerator_length] = '\0';
    memcpy(denominator_buffer, slash + 1, denominator_length);
    denominator_buffer[denominator_length] = '\0';

    char *end_ptr = NULL;
    long numerator = strtol(numerator_buffer, &end_ptr, 10);
    if (!end_ptr || *end_ptr != '\0') {
        return false;
    }

    end_ptr = NULL;
    unsigned long denominator = strtoul(denominator_buffer, &end_ptr, 10);
    if (!end_ptr || *end_ptr != '\0' || denominator == 0UL) {
        return false;
    }

    rational_set_si(value, numerator, denominator);
    return true;
}

static void apply_optional_bool(const char *json, const char *key, bool *target) {
    bool value = false;
    if (json_extract_bool(json, key, &value)) {
        *target = value;
    }
}

static void apply_optional_enum(const char *json, const char *key, int min_value, int max_value,
                                int *target) {
    int value = 0;
    if (!json_extract_int(json, key, &value)) {
        return;
    }
    if (value < min_value || value > max_value) {
        return;
    }
    *target = value;
}

bool config_load_from_file(Config *config, const char *path, char *error_buffer,
                           size_t error_capacity) {
    if (error_buffer && error_capacity > 0) {
        error_buffer[0] = '\0';
    }

    if (!config || !path) {
        write_error(error_buffer, error_capacity, "Invalid arguments");
        return false;
    }

    FILE *file = fopen(path, "rb");
    if (!file) {
        write_error(error_buffer, error_capacity, "Unable to open configuration file");
        return false;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        write_error(error_buffer, error_capacity, "Failed to seek configuration file");
        return false;
    }

    long size = ftell(file);
    if (size < 0) {
        fclose(file);
        write_error(error_buffer, error_capacity, "Failed to read configuration size");
        return false;
    }
    rewind(file);

    char *buffer = (char *)malloc((size_t)size + 1);
    if (!buffer) {
        fclose(file);
        write_error(error_buffer, error_capacity, "Out of memory while reading configuration");
        return false;
    }

    size_t read_count = fread(buffer, 1, (size_t)size, file);
    fclose(file);
    buffer[read_count] = '\0';

    const char *json = buffer;

    int enum_value = (int)config->psi_mode;
    apply_optional_enum(json, "psi_mode", PSI_MODE_MSTEP, PSI_MODE_INHIBIT_RHO, &enum_value);
    config->psi_mode = (PsiMode)enum_value;

    enum_value = (int)config->koppa_mode;
    apply_optional_enum(json, "koppa_mode", KOPPA_MODE_DUMP, KOPPA_MODE_ACCUMULATE, &enum_value);
    config->koppa_mode = (KoppaMode)enum_value;

    enum_value = (int)config->engine_mode;
    apply_optional_enum(json, "engine_mode", ENGINE_MODE_ADD, ENGINE_MODE_DELTA_ADD, &enum_value);
    config->engine_mode = (EngineMode)enum_value;

    enum_value = (int)config->engine_upsilon;
    apply_optional_enum(json, "upsilon_track", ENGINE_TRACK_ADD, ENGINE_TRACK_SLIDE, &enum_value);
    config->engine_upsilon = (EngineTrackMode)enum_value;

    enum_value = (int)config->engine_beta;
    apply_optional_enum(json, "beta_track", ENGINE_TRACK_ADD, ENGINE_TRACK_SLIDE, &enum_value);
    config->engine_beta = (EngineTrackMode)enum_value;

    bool bool_value = false;
    if (json_extract_bool(json, "dual_track_symmetry", &bool_value)) {
        config->dual_track_mode = bool_value;
    }

    if (json_extract_bool(json, "triple_psi", &bool_value)) {
        config->triple_psi_mode = bool_value;
    }

    apply_optional_bool(json, "multi_level_koppa", &config->multi_level_koppa);
    apply_optional_bool(json, "asymmetric_cascade", &config->enable_asymmetric_cascade);
    apply_optional_bool(json, "conditional_triple_psi", &config->enable_conditional_triple_psi);
    apply_optional_bool(json, "koppa_gated_engine", &config->enable_koppa_gated_engine);
    apply_optional_bool(json, "delta_cross_propagation", &config->enable_delta_cross_propagation);
    apply_optional_bool(json, "delta_koppa_offset", &config->enable_delta_koppa_offset);
    apply_optional_bool(json, "ratio_threshold_psi", &config->enable_ratio_threshold_psi);
    apply_optional_bool(json, "stack_depth_modes", &config->enable_stack_depth_modes);
    apply_optional_bool(json, "epsilon_phi_triangle", &config->enable_epsilon_phi_triangle);
    apply_optional_bool(json, "modular_wrap", &config->enable_modular_wrap);
    apply_optional_bool(json, "psi_strength_parameter", &config->enable_psi_strength_parameter);
    apply_optional_bool(json, "ratio_snapshot_logging", &config->enable_ratio_snapshot_logging);
    apply_optional_bool(json, "feedback_oscillator", &config->enable_feedback_oscillator);

    enum_value = (int)config->koppa_trigger;
    apply_optional_enum(json, "koppa_trigger", KOPPA_ON_PSI, KOPPA_ON_ALL_MU, &enum_value);
    config->koppa_trigger = (KoppaTrigger)enum_value;

    enum_value = (int)config->mt10_behavior;
    apply_optional_enum(json, "mt10_behavior", MT10_FORCED_EMISSION_ONLY, MT10_FORCED_PSI,
                        &enum_value);
    config->mt10_behavior = (Mt10Behavior)enum_value;

    enum_value = (int)config->ratio_trigger_mode;
    apply_optional_enum(json, "ratio_trigger_mode", RATIO_TRIGGER_NONE, RATIO_TRIGGER_PLASTIC,
                        &enum_value);
    config->ratio_trigger_mode = (RatioTriggerMode)enum_value;

    enum_value = (int)config->prime_target;
    apply_optional_enum(json, "prime_target", PRIME_ON_MEMORY, PRIME_ON_NEW_UPSILON, &enum_value);
    config->prime_target = (PrimeTarget)enum_value;

    enum_value = (int)config->sign_flip_mode;
    apply_optional_enum(json, "sign_flip_mode", SIGN_FLIP_NONE, SIGN_FLIP_ALTERNATE, &enum_value);
    config->sign_flip_mode = (SignFlipMode)enum_value;
    config->enable_sign_flip = (config->sign_flip_mode != SIGN_FLIP_NONE);

    int ticks_value = 0;
    if (json_extract_int(json, "tick_count", &ticks_value) && ticks_value > 0) {
        config->ticks = (size_t)ticks_value;
    }

    unsigned long wrap_value = 0UL;
    if (json_extract_unsigned(json, "koppa_wrap_threshold", &wrap_value)) {
        config->koppa_wrap_threshold = wrap_value;
    }

    char rational_buffer[128];
    if (json_extract_string(json, "upsilon_seed", rational_buffer, sizeof(rational_buffer))) {
        if (!parse_rational_string(rational_buffer, config->initial_upsilon)) {
            write_error(error_buffer, error_capacity, "Invalid upsilon seed");
            free(buffer);
            return false;
        }
    }

    if (json_extract_string(json, "beta_seed", rational_buffer, sizeof(rational_buffer))) {
        if (!parse_rational_string(rational_buffer, config->initial_beta)) {
            write_error(error_buffer, error_capacity, "Invalid beta seed");
            free(buffer);
            return false;
        }
    }

    if (json_extract_string(json, "koppa_seed", rational_buffer, sizeof(rational_buffer))) {
        if (!parse_rational_string(rational_buffer, config->initial_koppa)) {
            write_error(error_buffer, error_capacity, "Invalid koppa seed");
            free(buffer);
            return false;
        }
    }

    free(buffer);
    return true;
}
