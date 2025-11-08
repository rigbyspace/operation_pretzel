#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

#include "config.h"
#include "state.h"

bool engine_step(const Config *config, TRTS_State *state);

#endif // ENGINE_H
