#pragma once
#include "active-app.h"


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

