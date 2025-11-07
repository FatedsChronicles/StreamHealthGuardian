#pragma once
#include "rules_engine.hpp"
#include <obs-module.h>

struct UiConfig {
  Thresholds thresholds;
  DownshiftOptions downshift;
  char overlay_source_name[128]{};
  char scene_on_problem[128]{};
};

obs_properties_t* shg_create_properties(UiConfig& ui);
void shg_update_from_settings(UiConfig& ui, obs_data_t* s);
