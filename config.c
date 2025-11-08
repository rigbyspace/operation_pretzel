#include "config.h"

void config_init(Config *config) {
    config->psi_mode = PSI_MODE_MSTEP;
    config->koppa_mode = KOPPA_MODE_DUMP;
    config->engine_mode = ENGINE_MODE_ADD;
    config->koppa_trigger = KOPPA_ON_PSI;
    config->prime_target = PRIME_ON_NEW_UPSILON;
    config->mt10_behavior = MT10_FORCED_EMISSION_ONLY;
    config->ticks = 0;
    mpq_init(config->initial_upsilon);
    mpq_init(config->initial_beta);
    mpq_init(config->initial_koppa);
    mpq_set_ui(config->initial_upsilon, 0, 1);
    mpq_set_ui(config->initial_beta, 0, 1);
    mpq_set_ui(config->initial_koppa, 0, 1);
}

void config_clear(Config *config) {
    mpq_clear(config->initial_upsilon);
    mpq_clear(config->initial_beta);
    mpq_clear(config->initial_koppa);
}
