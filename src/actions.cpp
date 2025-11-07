#include "actions.hpp"

static obs_source_t* find_source_by_name(const char* name) {
  return obs_get_source_by_name(name); // addref
}

void Actions::show_overlay(const std::string& name, bool visible) {
  if (name.empty()) return;
  obs_source_t* src = find_source_by_name(name.c_str());
  if (!src) return;
  obs_source_set_enabled(src, visible);
  obs_source_release(src);
}

void Actions::switch_scene(const std::string& scene_name) {
  if (scene_name.empty()) return;
  obs_source_t* scene = obs_get_source_by_name(scene_name.c_str());
  if (!scene) return;
  obs_frontend_set_current_scene(scene);
  obs_source_release(scene);
}

void Actions::attempt_bitrate_downshift(const DownshiftOptions& opt) {
  obs_output_t* out = obs_frontend_get_streaming_output();
  if (!out) { PLOG(LOG_WARNING, "no streaming output"); return; }

  obs_encoder_t* venc = obs_output_get_video_encoder(out);
  if (!venc) { PLOG(LOG_WARNING, "no video encoder"); obs_output_release(out); return; }

  obs_data_t* settings = obs_encoder_get_settings(venc);
  if (!settings) { obs_output_release(out); return; }

  // Bitrate (most likely to succeed)
  obs_data_set_int(settings, "bitrate", opt.target_bitrate_kbps);

  // Rescale (may be ignored or require restart)
  obs_data_set_bool(settings, "rescale_output", true);
  obs_data_set_int(settings, "width",  opt.rescale_width);
  obs_data_set_int(settings, "height", opt.rescale_height);

  // Some encoders ignore live updates. OBS API may not report failure.
  obs_encoder_update(venc, settings);
  PLOG(LOG_INFO, "attempted live encoder update: %d kbps, %dx%d",
      opt.target_bitrate_kbps, opt.rescale_width, opt.rescale_height);

  obs_data_release(settings);
  obs_output_release(out);
}
