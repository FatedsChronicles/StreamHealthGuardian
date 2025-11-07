#include "hotkeys.hpp"
#include "logging.hpp"

static void toggle_overlay_cb(void*, bool pressed) {
  if (!pressed) return;
  PLOG(LOG_INFO, "toggle overlay (implement visibility toggle if overlay is named)");
}

static void suppress_rules_cb(void*, bool pressed) {
  if (!pressed) return;
  PLOG(LOG_INFO, "rules suppressed toggle (wire suppression flag in next iteration)");
}

void register_hotkeys(Hotkeys& hk) {
  hk.toggle_overlay = obs_hotkey_register_frontend("shg.toggle_overlay", "SHG: Toggle Overlay", toggle_overlay_cb, nullptr);
  hk.suppress_rules = obs_hotkey_register_frontend("shg.suppress_rules", "SHG: Suppress Rules", suppress_rules_cb, nullptr);
}
