#pragma once
#include "active-app.h"


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

