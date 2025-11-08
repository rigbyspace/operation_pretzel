#ifndef KOPPA_H
#define KOPPA_H

#include <stdbool.h>

#include "config.h"
#include "state.h"

void koppa_accrue(const Config *config, TRTS_State *state, bool psi_fired, bool is_memory_step,
                  int microtick);

#endif // KOPPA_H
