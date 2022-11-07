#pragma once
#ifndef OBSERVE_C
#define OBSERVE_C
#define LOCAL_DEBUG_MODE    OBSERVE_DEBUG_MODE
////////////////////////////////////////////
#include "app/observe/observe.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "frameworks/frameworks.h"

////////////////////////////////////////////
static bool OBSERVE_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////


#define OBSERVER_CALLBACK(name) \
  void name(AXObserverRef observer, AXUIElementRef element, CFStringRef notification, void *context)
typedef OBSERVER_CALLBACK(observer_callback);
static OBSERVER_CALLBACK(application_notification_handler){
  log_debug("callback!");

}
#define array_count(a) (sizeof((a)) / sizeof(a[0]))
void app_observe_start(pid_t pid){
  AXObserverRef observer_ref;
  AXUIElementRef ref;
if (AXObserverCreate(pid, application_notification_handler, &observer_ref) == kAXErrorSuccess) {
        for (int i = 0; i < array_count(ax_application_notification); ++i) {
            AXError result = AXObserverAddNotification(observer_ref, ref, ax_application_notification[i], NULL);
            if (result == kAXErrorSuccess || result == kAXErrorNotificationAlreadyRegistered) {
//                application->notification |= 1 << i;
            } else {
//                if (result == kAXErrorCannotComplete) application->ax_retry = true;
  //              log_debug("%s: error '%s' for application '%s' and notification '%s'\n", __FUNCTION__, ax_error_str[-result], application->name, ax_application_notification_str[i]);
            }
        }

    //    application->is_observing = true;
        CFRunLoopAddSource(CFRunLoopGetMain(), AXObserverGetRunLoopSource(observer_ref), kCFRunLoopDefaultMode);
    }
log_debug("observing....");
}
static void __attribute__((constructor)) __constructor__observe(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_observe") != NULL) {
    log_debug("Enabling observe Debug Mode");
    OBSERVE_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
