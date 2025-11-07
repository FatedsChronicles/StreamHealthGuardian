#include "metrics_collector.hpp"
#include "logging.hpp"
#include <chrono>

void MetricsCollector::start() { running = true; }
void MetricsCollector::stop()  { running = false; }

obs_output_t* MetricsCollector::get_stream_output() {
  return obs_frontend_get_streaming_output();
}

void MetricsCollector::mark_audio_activity(bool active) { audio_recent = audio_recent || active; }

StreamMetrics MetricsCollector::sample() {
  StreamMetrics m{};
  if (!running) return m;

  obs_output_t* out = get_stream_output();
  if (out) {
    const uint64_t total = obs_output_get_total_frames(out);
    const uint64_t dropped = obs_output_get_frames_dropped(out);
    m.total_frames = total;
    m.dropped_frames = dropped;
    if (total > 0) m.dropped_frame_percent = 100.0 * double(dropped) / double(total);

    m.avg_render_ms = obs_get_average_frame_time_ns() / 1e6;
    m.avg_encode_ms = obs_get_average_encode_time_ns() / 1e6;

    using clock = std::chrono::steady_clock;
    const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now().time_since_epoch()).count();
    const uint64_t total_bytes = obs_output_get_total_bytes(out);
    if (last_ts_ms && total_bytes > last_bytes) {
      const double kb = double(total_bytes - last_bytes) / 1024.0;
      const double s = double(now_ms - last_ts_ms) / 1000.0;
      if (s > 0.0) m.output_bitrate_kbps = (kb * 8.0) / s;
    }
    last_bytes = total_bytes;
    last_ts_ms = now_ms;

    obs_output_release(out);
  }

  m.audio_active = audio_recent;
  audio_recent = false;
  return m;
}
