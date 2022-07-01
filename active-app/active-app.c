#pragma once
#include "active-app.h"


void resize_current_window(int p1, int p2, int p3, int p4){
  int pid = get_frontmost_application();

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


int get_frontmost_application(){
  CFArrayRef window_list = CGWindowListCopyWindowInfo(
    kCGWindowListExcludeDesktopElements | kCGWindowListOptionOnScreenOnly,
    kCGNullWindowID);

  int num_windows = CFArrayGetCount(window_list);

  for (int i = 0; i < num_windows; i++) {
    CFDictionaryRef dict              = CFArrayGetValueAtIndex(window_list, i);
    CFNumberRef     objc_window_layer = CFDictionaryGetValue(dict, kCGWindowLayer);

    int             window_layer;
    CFNumberGetValue(objc_window_layer, kCFNumberIntType, &window_layer);

    if (window_layer == 0) {
      CFNumberRef objc_window_pid = CFDictionaryGetValue(dict, kCGWindowOwnerPID);

      int         window_pid = 0;
      CFNumberGetValue(objc_window_pid, kCFNumberIntType, &window_pid);

      return(window_pid);
    }
  }

  return(-1);
}

struct screen_size get_window_size(){
  int pid = get_frontmost_application();

  if (pid == -1) {
    return((struct screen_size){ 0, 0, 0, 0 });
  }

  AXUIElementRef app = AXUIElementCreateApplication(pid);
  AXUIElementRef win;

  AXError        error = AXUIElementCopyAttributeValue(app, kAXMainWindowAttribute, (CFTypeRef *)&win);

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


ProcessSerialNumber PID2PSN(pid_t pid) {
  ProcessSerialNumber tempPSN;

  GetProcessForPID(pid, &tempPSN);
  return(tempPSN);
}


pid_t PSN2PID(ProcessSerialNumber psn) {
  pid_t tempPID;

  GetProcessPID(&psn, &tempPID);
  return(tempPID);
}


int get_focused_pid(){
  return((int)(PSN2PID(get_focused_ProcessSerialNumber())));
}


bool set_focused_pid(int pid){
  return(set_focused_ProcessSerialNumber(PID2PSN((pid_t)pid)));
}


bool set_focused_ProcessSerialNumber(ProcessSerialNumber PSN){
  OSErr err = SetFrontProcess(&PSN);

  return((err == noErr));
}


ProcessSerialNumber get_focused_ProcessSerialNumber(){
  focused_t           *f           = malloc(sizeof(focused_t));
  char                *processName = malloc(1024);
  ProcessSerialNumber psn;
  ProcessInfoRec      info;

  bzero(processName, 1024);
  info.processInfoLength = sizeof(ProcessInfoRec);
  info.processName       = processName;
  GetFrontProcess(&psn);
  GetProcessInformation(&psn, &info);
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

