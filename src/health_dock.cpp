#include "health_dock.hpp"
#include "actions.hpp"
#include "overlay_source.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QString>

static QString colorFor(HealthState s) {
  switch (s) {
    case HealthState::Good:         return "#1db954";
    case HealthState::VideoIssue:   return "#e11d48";
    case HealthState::AudioIssue:   return "#2563eb";
    case HealthState::NetworkIssue: return "#f59e0b";
  }
  return "#1db954";
}

HealthDock::HealthDock(QWidget* parent) : QWidget(parent) {
  auto* layout = new QVBoxLayout(this);
  auto* label  = new QLabel(tr("Stream Health Dashboard"), this);
  label->setObjectName("title");
  layout->addWidget(label);
  setLayout(layout);

  connect(&timer, &QTimer::timeout, this, &HealthDock::onTick);
  timer.setInterval(1000);
  updateBorder(HealthState::Good);
}

void HealthDock::start() { collector.start(); timer.start(); }
void HealthDock::stop()  { timer.stop(); collector.stop(); }

void HealthDock::onTick() {
  auto m = collector.sample();
  auto s = rules.classify(m);
  if (s != last_state) {
    updateBorder(s);
    shg_set_overlay_state(s);
    last_state = s;
  }
  tick_rules(m);
  renderSummary(m);
}

void HealthDock::updateBorder(HealthState s) {
  setStyleSheet(QString(
    "HealthDock { border: 3px solid %1; } QWidget { background: #1e1e1e; color: #eee; } #title { font-weight: 600; }"
  ).arg(colorFor(s)));
}

void HealthDock::tick_rules(const StreamMetrics& m) {
  // Core rule: if drop % >= threshold, perform downshift
  rules.add_rule({
    "DownshiftOnDrops",
    [t=rules.thresholds()](const StreamMetrics& sm){ return sm.dropped_frame_percent >= t.downshift_drop_percent; },
    [this](const StreamMetrics&, ActionCtx& ctx){
      if (ui.downshift.policy == DownshiftPolicy::Confirm) {
        blog(LOG_INFO, "[StreamHealthGuardian] Downshift suggested (Confirm policy)");
        // In a later iteration, present a confirm dialog; for now, log only.
      } else {
        Actions::attempt_bitrate_downshift(ui.downshift);
      }
      if (ui.scene_on_problem[0]) Actions::switch_scene(ui.scene_on_problem);
      if (ui.overlay_source_name[0]) Actions::show_overlay(ui.overlay_source_name, true);
    }
  });

  rules.evaluate(m);
}

void HealthDock::renderSummary(const StreamMetrics& m) {
  setToolTip(QString("Dropped: %1 (%.2f%%)\nEncode: %.2f ms\nRender: %.2f ms\nBitrate: %.0f kbps\nAudio: %2")
    .arg(m.dropped_frames).arg(m.dropped_frame_percent).arg(m.avg_encode_ms).arg(m.avg_render_ms)
    .arg(m.output_bitrate_kbps).arg(m.audio_active ? "active" : "silent"));
}

obs_properties_t* HealthDock::properties() { return shg_create_properties(ui); }
void HealthDock::update_settings(obs_data_t* s) {
  shg_update_from_settings(ui, s);
  rules.set_thresholds(ui.thresholds);
  rules.set_actions(ActionsConfig{ui.downshift, ui.overlay_source_name, ui.scene_on_problem});
}
