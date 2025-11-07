#pragma once
#include <chrono>
#include "logging.hpp"

struct ScopeTimer {
  const char* label;
  std::chrono::high_resolution_clock::time_point t0;
  explicit ScopeTimer(const char* l) : label(l), t0(std::chrono::high_resolution_clock::now()) {}
  ~ScopeTimer() {
    using namespace std::chrono;
    auto ms = duration_cast<duration<double,std::milli>>(high_resolution_clock::now() - t0).count();
    PLOG(LOG_INFO, "perf %s: %.3f ms", label, ms);
  }
};
