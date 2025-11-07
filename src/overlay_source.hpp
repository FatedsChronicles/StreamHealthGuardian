#pragma once
#include <obs-module.h>
#include "rules_engine.hpp"

obs_source_info* shg_create_overlay_source_info();
void shg_set_overlay_state(HealthState s);
