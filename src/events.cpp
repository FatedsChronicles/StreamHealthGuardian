#include "events.hpp"
#include "logging.hpp"

static void on_frontend_event(enum obs_frontend_event e, void*) {
  switch (e) {
    case OBS_FRONTEND_EVENT_STREAMING_STARTED: PLOG(LOG_INFO, "stream START"); break;
    case OBS_FRONTEND_EVENT_STREAMING_STOPPED: PLOG(LOG_INFO, "stream STOP");  break;
    default: break;
  }
}
void register_frontend_events() { obs_frontend_add_event_callback(on_frontend_event, nullptr); }
