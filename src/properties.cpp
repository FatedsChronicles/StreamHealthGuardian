#include "properties.hpp"

static bool policy_modified(obs_properties_t*, obs_property_t*, obs_data_t* s) {
  UNUSED_PARAMETER(s);
  return true;
}

obs_properties_t* shg_create_properties(UiConfig& ui) {
  obs_properties_t* p = obs_properties_create();

  // Thresholds
  obs_properties_t* grpT = obs_properties_create();
  obs_properties_add_group(p, "th", "Thresholds", OBS_GROUP_NORMAL, grpT);
  obs_properties_add_float_slider(grpT, "drop_warn", "Drop % (warn)", 0.0, 50.0, 0.1);
  obs_properties_add_float_slider(grpT, "drop_bad",  "Drop % (bad)",  0.0, 50.0, 0.1);
  obs_properties_add_float_slider(grpT, "enc_warn",  "Encode ms (warn)", 0.0, 50.0, 0.1);
  obs_properties_add_float_slider(grpT, "enc_bad",   "Encode ms (bad)",  0.0, 50.0, 0.1);
  obs_properties_add_float_slider(grpT, "ren_warn",  "Render ms (warn)", 0.0, 50.0, 0.1);
  obs_properties_add_float_slider(grpT, "ren_bad",   "Render ms (bad)",  0.0, 50.0, 0.1);
  obs_properties_add_int_slider(grpT, "silence_s",   "Audio silence (s)", 0, 30, 1);
  obs_properties_add_float_slider(grpT, "downshift_pct", "Downshift if drop % >=", 0.0, 50.0, 0.1);

  // Downshift
  obs_properties_t* grpD = obs_properties_create();
  obs_properties_add_group(p, "ds", "Downshift Policy", OBS_GROUP_NORMAL, grpD);
  obs_property_t* pol = obs_properties_add_list(grpD, "policy", "Policy",
      OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
  obs_property_list_add_int(pol, "Ask/Confirm", (int)DownshiftPolicy::Confirm);
  obs_property_list_add_int(pol, "Auto-apply",  (int)DownshiftPolicy::Auto);
  obs_property_set_modified_callback(pol, policy_modified);

  obs_properties_add_int(grpD, "bitrate", "Target Bitrate (kbps)", 200, 9000, 50);
  obs_properties_add_int(grpD, "width", "Rescale Width", 320, 3840, 1);
  obs_properties_add_int(grpD, "height","Rescale Height",180, 2160, 1);

  // Actions
  obs_properties_t* grpA = obs_properties_create();
  obs_properties_add_group(p, "act", "Actions", OBS_GROUP_NORMAL, grpA);
  obs_properties_add_text(grpA, "overlay_name", "Overlay Source Name", OBS_TEXT_DEFAULT);
  obs_properties_add_text(grpA, "scene_problem", "Scene on Problem", OBS_TEXT_DEFAULT);

  return p;
}

void shg_update_from_settings(UiConfig& ui, obs_data_t* s) {
  ui.thresholds.drop_percent_warn = obs_data_get_double(s, "drop_warn");
  ui.thresholds.drop_percent_bad  = obs_data_get_double(s, "drop_bad");
  ui.thresholds.encode_ms_warn    = obs_data_get_double(s, "enc_warn");
  ui.thresholds.encode_ms_bad     = obs_data_get_double(s, "enc_bad");
  ui.thresholds.render_ms_warn    = obs_data_get_double(s, "ren_warn");
  ui.thresholds.render_ms_bad     = obs_data_get_double(s, "ren_bad");
  ui.thresholds.audio_silence_secs= (int)obs_data_get_int(s, "silence_s");
  ui.thresholds.downshift_drop_percent = obs_data_get_double(s, "downshift_pct");

  ui.downshift.policy = (DownshiftPolicy)obs_data_get_int(s, "policy");
  ui.downshift.target_bitrate_kbps = (int)obs_data_get_int(s, "bitrate");
  ui.downshift.rescale_width  = (int)obs_data_get_int(s, "width");
  ui.downshift.rescale_height = (int)obs_data_get_int(s, "height");

  const char* ov = obs_data_get_string(s, "overlay_name");
  const char* sc = obs_data_get_string(s, "scene_problem");
  if (ov) strncpy(ui.overlay_source_name, ov, sizeof(ui.overlay_source_name)-1);
  if (sc) strncpy(ui.scene_on_problem, sc, sizeof(ui.scene_on_problem)-1);
}
