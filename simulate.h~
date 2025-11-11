// simulate.h
// Entry points for running a TRTS simulation.  This header mirrors the
// upstream project and is extended with the ability to observe simulation
// events via a callback.  No internal logic is declared here.

#ifndef SIMULATE_H
#define SIMULATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "state.h"

// A callback invoked on each microtick when using simulate_stream().  The
// callback receives the current tick, microtick number, phase ('E','M','R'),
// a pointer to the immutable state, and flags indicating whether rho or psi
// fired, whether mu was zero, and whether this was a forced emission.
typedef void (*SimulateObserver)(void *user_data,
                                  size_t tick,
                                  int microtick,
                                  char phase,
                                  const TRTS_State *state,
                                  bool rho_event,
                                  bool psi_fired,
                                  bool mu_zero,
                                  bool forced_emission);

// Run a simulation and write events/values to CSV files named events.csv and
// values.csv in the current working directory.  This function does not use
// the observer callback.
void simulate(const Config *config);

// Run a simulation and invoke the provided observer on every microtick.
// No files are written in this mode.  Both config and user_data may be
// modified after the call returns, but must remain valid for the duration
// of the simulation.
void simulate_stream(const Config *config,
                      SimulateObserver observer,
                      void *user_data);

#ifdef __cplusplus
}
#endif

#endif // SIMULATE_H
