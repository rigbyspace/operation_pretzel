#include <gmp.h>
#include <stdio.h>

#include "config.h"
#include "simulate.h"

int main(void) {
    Config config;
    config_init(&config);

    config.ticks = 5;
    config.psi_mode = PSI_MODE_RHO_ONLY;
    config.koppa_mode = KOPPA_MODE_DUMP;
    config.engine_mode = ENGINE_MODE_ADD;
    config.engine_upsilon = ENGINE_TRACK_ADD;
    config.engine_beta = ENGINE_TRACK_ADD;
    config.dual_track_mode = false;
    config.triple_psi_mode = false;
    config.multi_level_koppa = false;
    config.koppa_trigger = KOPPA_ON_PSI;
    config.prime_target = PRIME_ON_NEW_UPSILON;
    config.mt10_behavior = MT10_FORCED_PSI;
    config.ratio_trigger_mode = RATIO_TRIGGER_NONE;

    mpq_set_si(config.initial_upsilon, 3, 5);
    mpq_set_si(config.initial_beta, 5, 7);
    mpq_set_si(config.initial_koppa, 1, 1);

    simulate(&config);

    config_clear(&config);
    return 0;
}
