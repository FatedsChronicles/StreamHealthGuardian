#include "rules_engine.hpp"
#include "actions.hpp"

void RulesEngine::evaluate(const StreamMetrics& m) {
  ActionCtx ctx{act_cfg};
  for (auto& r : rules) if (r.predicate(m)) r.action(m, ctx);
}

HealthState RulesEngine::classify(const StreamMetrics& m) const {
  if (!m.audio_active) return HealthState::AudioIssue;
  if (m.dropped_frame_percent >= th.drop_percent_bad ||
      m.avg_encode_ms >= th.encode_ms_bad ||
      m.avg_render_ms >= th.render_ms_bad) return HealthState::VideoIssue;
  if (m.dropped_frame_percent >= th.drop_percent_warn) return HealthState::NetworkIssue;
  return HealthState::Good;
}
