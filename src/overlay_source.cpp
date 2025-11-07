#include "overlay_source.hpp"
#include <atomic>

static std::atomic<HealthState> g_state{HealthState::Good};

static const char* overlay_get_name(void*) { return "StreamHealthGuardian Overlay"; }
static void overlay_update(void*, obs_data_t*) {}
static void overlay_render(void*, gs_effect_t*) {
  const HealthState s = g_state.load();
  float r=0,g=0,b=0;
  switch (s) {
    case HealthState::Good:         r=0.11f; g=0.72f; b=0.33f; break; // green
    case HealthState::VideoIssue:   r=0.88f; g=0.11f; b=0.28f; break; // red
    case HealthState::AudioIssue:   r=0.15f; g=0.39f; b=0.92f; break; // blue
    case HealthState::NetworkIssue: r=0.96f; g=0.62f; b=0.05f; break; // amber
  }

  gs_matrix_push();
  gs_matrix_identity();
  uint32_t w=40,h=40;
  gs_effect_t* solid = obs_get_base_effect(OBS_EFFECT_SOLID);
  gs_eparam_t* color = gs_effect_get_param_by_name(solid, "color");
  vec4 v; vec4_from_rgba(&v, r, g, b, 1.0f);
  gs_effect_set_vec4(color, &v);
  while (gs_effect_loop(solid, "Solid")) {
    gs_draw_sprite(nullptr, 0, w, h);
  }
  gs_matrix_pop();
}
static uint32_t overlay_width(void*) { return 40; }
static uint32_t overlay_height(void*) { return 40; }

static obs_source_info g_info = {
  .id = "streamhealthguardian_overlay",
  .type = OBS_SOURCE_TYPE_INPUT,
  .output_flags = OBS_SOURCE_VIDEO,
  .get_name = overlay_get_name,
  .create = [](obs_data_t*, obs_source_t*) -> void* { return nullptr; },
  .destroy = [](void*) {},
  .update = overlay_update,
  .video_render = overlay_render,
  .get_width = overlay_width,
  .get_height = overlay_height,
};

obs_source_info* shg_create_overlay_source_info() { return &g_info; }
void shg_set_overlay_state(HealthState s) { g_state.store(s); }
