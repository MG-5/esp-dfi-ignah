#pragma once

#include "wrappers/Task.hpp"

namespace util::wrappers::sync_events
{
constexpr EventBits_t ConfigurationLoaded = 1 << 0;
constexpr EventBits_t ConnectedToWifi = 1 << 1;
constexpr EventBits_t ConnectionFailed = 1 << 2;
constexpr EventBits_t TimeIsSynchronized = 1 << 3;
constexpr EventBits_t LedDriverStarted = 1 << 4;
constexpr EventBits_t NvmInitialized = 1 << 5;

} // namespace util::wrappers::sync_events
