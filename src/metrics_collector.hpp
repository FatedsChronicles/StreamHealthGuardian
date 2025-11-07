#pragma once
#include <obs.h>
#include <obs-frontend-api.h>

struct StreamMetrics {
  double dropped_frame_percent = 0.0;
  uint64_t dropped_frames = 0;
  uint64_t total_frames = 0;
  double avg_render_ms = 0.0;
  double avg_encode_ms = 0.0;
  double output_bitrate_kbps = 0.0;
  bool audio_active = true;
};

class MetricsCollector {
public:
  void start();
  void stop();
  StreamMetrics sample();
  void mark_audio_activity(bool active);

private:
  bool running = false;
  bool audio_recent = true;
  uint64_t last_bytes = 0;
  uint64_t last_ts_ms = 0;

  obs_output_t* get_stream_output();
};
