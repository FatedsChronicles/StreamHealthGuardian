#pragma once
#include <obs-module.h>
#define PLOG(level, fmt, ...) blog(level, "[StreamHealthGuardian] " fmt, ##__VA_ARGS__)
