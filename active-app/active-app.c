#pragma once
#include "active-app.h"
#include "app-utils/app-utils.h"
#include "core-utils/core-utils.h"
#include "log.h/log.h"
#include "process-utils/process-utils.h"
static bool ACTIVE_APP_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__active_app_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_ACTIVE_APP_UTILS") != NULL) {
    log_debug("Enabling Active App Utils Debug Mode");
    ACTIVE_APP_UTILS_DEBUG_MODE = true;
  }
}

void resize_current_window(int p1, int p2, int p3, int p4){
  int pid = get_focused_pid();

  if (pid == -1) {
    return;
  }

  AXUIElementRef app = AXUIElementCreateApplication(pid);
  AXUIElementRef win;

  AXUIElementCopyAttributeValue(app, kAXMainWindowAttribute, (CFTypeRef *)&win);

  CGPoint   point = { p1, p2 };
  CGSize    size  = { p1 + p3, p2 + p4 };

  CFTypeRef objc_point = (CFTypeRef)(AXValueCreate(kAXValueCGPointType, (void *)&point));
  CFTypeRef objc_size  = (CFTypeRef)(AXValueCreate(kAXValueCGSizeType, (void *)&size));

  AXUIElementSetAttributeValue(win, kAXPositionAttribute, objc_point);
  AXUIElementSetAttributeValue(win, kAXSizeAttribute, objc_size);
}

struct screen_size get_window_size(){
  int pid = get_focused_pid();

  if (pid == -1) {
    return((struct screen_size){ 0, 0, 0, 0 });
  }

  AXUIElementRef win;

  CGPoint        point;
  CFTypeRef      objc_point;

  CGSize         size;
  CFTypeRef      objc_size;

  AXUIElementCopyAttributeValue(win, kAXPositionAttribute, &objc_point);
  AXValueGetValue(objc_point, kAXValueCGPointType, &point);

  AXUIElementCopyAttributeValue(win, kAXSizeAttribute, &objc_size);
  AXValueGetValue(objc_size, kAXValueCGSizeType, &size);

  struct screen_size ret = {
    .x = point.x,
    .y = point.y,
    .w = size.width,
    .h = size.height,
  };

  return(ret);
}

bool set_focused_pid(int pid){
  return(set_focused_ProcessSerialNumber(PID2PSN((pid_t)pid)));
}

bool set_focused_ProcessSerialNumber(ProcessSerialNumber PSN){
  OSErr err = SetFrontProcess(&PSN);

  return((err == noErr));
}

ProcessSerialNumber get_focused_ProcessSerialNumber(){
  char                *processName = malloc(1024);
  ProcessSerialNumber psn;
  ProcessInfoRec      info;

  bzero(processName, 1024);
  info.processInfoLength = sizeof(ProcessInfoRec);
  info.processName       = processName;
  GetFrontProcess(&psn);
  GetProcessInformation(&psn, &info);
  free(processName);
  return(psn);
}

focused_t * get_focused_process(){
  focused_t           *f           = malloc(sizeof(focused_t));
  char                *processName = malloc(1024);
  ProcessSerialNumber psn;
  OSErr               err = GetFrontProcess(&psn);

  if (err == noErr) {
    ProcessInfoRec info;
    if (processName) {
      bzero(processName, 1024);
      info.processInfoLength = sizeof(ProcessInfoRec);
      info.processName       = processName;
      err                    = GetProcessInformation(&psn, &info);
      if (err == noErr) {
        f->name = stringfn_trim(strdup(processName));
        GetProcessPID(&psn, &(f->pid));
        return(f);
      }
      free(processName);
    }
  }
  return(f);
}
