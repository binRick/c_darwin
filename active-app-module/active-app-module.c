#pragma once
#include "active-app-module/active-app-module.h"
/////////////////////////////////////////////////////////
static inline void logger_info(char *message);
static inline void logger_error(char *message);
static inline void logger_debug(char *message);
static inline int  logger_Update();
static inline int  logger_GetPID();
static inline char *logger_GetName();

static inline int logger_Update(){
  require(logger)->_fp = malloc(sizeof(__focused_t));
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
        //sprintf(require(logger)->Name, stringfn_trim(strdup(processName)));
        GetProcessPID(&psn, &(require(logger)->PID));
      }
    }
  }
  free(processName);
  int ok = (require(logger)->PID > 0) ? 0 : 1;

  return(ok);
}

int logger_init(module(logger) *exports) {
  clib_module_init(logger, exports);
  exports->mode    = LOGGER_NONE;
  exports->info    = logger_info;
  exports->error   = logger_error;
  exports->debug   = logger_debug;
  exports->Update  = logger_Update;
  exports->GetPID  = logger_GetPID;
  exports->GetName = logger_GetName;
  exports->_fp     = malloc(sizeof(__focused_t));
  return(0);
}

void logger_deinit(module(logger) *exports) {
  clib_module_deinit(logger);
}

static inline int logger_GetPID(){
  return(require(logger)->PID);
}

static inline char *logger_GetName(){
  return(require(logger)->Name);
}

static inline void logger_info(char *message) {
  if (require(logger)->mode >= LOGGER_INFO) {
    fprintf(stdout, " info: %s\n", message);
  }
}

static inline void logger_error(char *message) {
  if (require(logger)->mode >= LOGGER_ERROR) {
    fprintf(stderr, "error: %s\n", message);
  }
}

static inline void logger_debug(char *message) {
  if (require(logger)->mode >= LOGGER_DEBUG) {
    fprintf(stderr, "debug: %s\n", message);
  }
}
