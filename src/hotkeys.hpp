#pragma once
#include <obs-module.h>

struct Hotkeys {
  obs_hotkey_id toggle_overlay = OBS_INVALID_HOTKEY_ID;
  obs_hotkey_id suppress_rules = OBS_INVALID_HOTKEY_ID;
};

void register_hotkeys(Hotkeys& hk);
