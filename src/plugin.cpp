#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QWidget>
#include "version.hpp"
#include "logging.hpp"
#include "health_dock.hpp"
#include "overlay_source.hpp"
#include "hotkeys.hpp"
#include "events.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("StreamHealthGuardian", "en-US")

MODULE_EXPORT const char* obs_module_description(void) { return SHG_DESCRIPTION; }

static QWidget* g_dock_widget = nullptr;           // keep a pointer to delete on unload
static const char* SHG_DOCK_ID = "streamhealthguardian_dock";

static obs_source_info* g_overlay = nullptr;
static Hotkeys g_hotkeys;

static obs_properties_t* shg_properties(void* data) {
  auto* dock = reinterpret_cast<HealthDock*>(data);
  return dock->properties();
}

static void shg_update(void* data, obs_data_t* s) {
  auto* dock = reinterpret_cast<HealthDock*>(data);
  dock->update_settings(s);
}

bool obs_module_load(void)
{
  PLOG(LOG_INFO, "loaded %s v%s", SHG_NAME, PROJECT_VERSION);

  // Overlay source registration
  g_overlay = shg_create_overlay_source_info();
  obs_register_source(g_overlay);

  // Create dock
  auto* dock = new HealthDock();
  dock->start();
  g_dock_widget = dock;
  // Add dock by id + title + widget (required by current frontend API)
  obs_frontend_add_dock(SHG_DOCK_ID, obs_module_text("SHG.DockTitle"), dock);

  // Hook up properties on the module as a pseudo "settings owner"
  obs_module_set_config_path(nullptr); // using default module config dir
  obs_module_set_website(nullptr);

  register_hotkeys(g_hotkeys);
  register_frontend_events();

  // Expose a property page via module settings menu
  obs_module_set_create_properties(shg_properties, dock);
  obs_module_set_update(shg_update, dock);

  return true;
}

void obs_module_unload(void)
{
  if (g_dock_widget) {
    // Remove by id (API expects const char* id, not QWidget*)
    obs_frontend_remove_dock(SHG_DOCK_ID);
    delete g_dock_widget;
    g_dock_widget = nullptr;
  }
  PLOG(LOG_INFO, "unload");
}
