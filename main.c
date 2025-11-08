#include <gmp.h>
#include <stdio.h>

#include "config.h"
#include "simulate.h"

int main(void) {
    Config config;
    config_init(&config);

    config.ticks = 5;
    config.psi_mode = PSI_MODE_INHIBIT_RHO;
    config.koppa_mode = KOPPA_MODE_POP;
    config.engine_mode = ENGINE_MODE_ADD;
    config.koppa_trigger = KOPPA_ON_ALL_MU;
    config.prime_target = PRIME_ON_NEW_UPSILON;
    config.mt10_behavior = MT10_FORCED_PSI;

    mpq_set_si(config.initial_upsilon, 3, 5);
    mpq_set_si(config.initial_beta, 5, 7);
    mpq_set_si(config.initial_koppa, 1, 1);

    simulate(&config);

    config_clear(&config);
    return 0;
}
