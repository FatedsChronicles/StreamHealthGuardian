#pragma once
#include <QWidget>
#include <QTimer>
#include "metrics_collector.hpp"
#include "rules_engine.hpp"
#include "properties.hpp"

class HealthDock : public QWidget {
  Q_OBJECT
public:
  explicit HealthDock(QWidget* parent=nullptr);
  void start();
  void stop();

  obs_properties_t* properties();
  void update_settings(obs_data_t* s);

private slots:
  void onTick();

private:
  MetricsCollector collector;
  RulesEngine rules;
  UiConfig ui{};
  QTimer timer;
  HealthState last_state = HealthState::Good;

  void updateBorder(HealthState s);
  void tick_rules(const StreamMetrics& m);
  void renderSummary(const StreamMetrics& m);
};
