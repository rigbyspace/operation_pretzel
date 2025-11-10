// simulate.h
#ifndef SIMULATE_H
#define SIMULATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "state.h"

typedef void (*SimulateObserver)(void *user_data,
                                 size_t tick,
                                 int microtick,
                                 char phase,
                                 const TRTS_State *state,
                                 bool rho_event,
                                 bool psi_fired,
                                 bool mu_zero,
                                 bool forced_emission);

void simulate(const Config *config);
void simulate_stream(const Config *config,
                     SimulateObserver observer,
                     void *user_data);

#ifdef __cplusplus
}
#endif

#endif // SIMULATE_H

