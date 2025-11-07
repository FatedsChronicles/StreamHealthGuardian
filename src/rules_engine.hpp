#pragma once
#include "metrics_collector.hpp"
#include <functional>
#include <string>
#include <vector>

enum class HealthState { Good, VideoIssue, AudioIssue, NetworkIssue };
enum class DownshiftPolicy { Auto, Confirm };

struct Thresholds {
  double drop_percent_warn = 2.0;
  double drop_percent_bad  = 5.0;
  double encode_ms_warn    = 12.0;
  double encode_ms_bad     = 20.0;
  double render_ms_warn    = 12.0;
  double render_ms_bad     = 20.0;
  int    audio_silence_secs = 5;
  double downshift_drop_percent = 5.0;
};

struct DownshiftOptions {
  DownshiftPolicy policy = DownshiftPolicy::Confirm;
  int target_bitrate_kbps = 3500;
  int rescale_width = 1280;
  int rescale_height = 720;
};

struct ActionsConfig {
  DownshiftOptions downshift;
  std::string overlay_source_name;
  std::string scene_on_problem;
};

struct ActionCtx {
  ActionsConfig cfg;
};

using Action = std::function<void(const StreamMetrics&, ActionCtx&)>;

struct Rule {
  std::string name;
  std::function<bool(const StreamMetrics&)> predicate;
  Action action;
};

class RulesEngine {
public:
  void set_thresholds(const Thresholds& t) { th = t; }
  void set_actions(ActionsConfig a) { act_cfg = std::move(a); }
  const Thresholds& thresholds() const { return th; }
  void add_rule(Rule r) { rules.emplace_back(std::move(r)); }
  void evaluate(const StreamMetrics& m);
  HealthState classify(const StreamMetrics& m) const;

private:
  Thresholds th{};
  ActionsConfig act_cfg{};
  std::vector<Rule> rules;
};
