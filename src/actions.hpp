#pragma once
#include "rules_engine.hpp"
#include "logging.hpp"
#include <obs.h>
#include <obs-frontend-api.h>

namespace Actions {
  void show_overlay(const std::string& name, bool visible);
  void switch_scene(const std::string& scene_name);
  void attempt_bitrate_downshift(const DownshiftOptions& opt);
}
