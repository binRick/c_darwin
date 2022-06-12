#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/////////////////////////////////////////////
#include "active-app/active-app.h"
/////////////////////////////////////////////
#include "submodules/module/def.h"
#include "submodules/module/module.h"
#include "submodules/module/require.h"
/////////////////////////////////////////////
#include "submodules/greatest/greatest.h"
/////////////////////////////////////////////
#include <assert.h>
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "submodules/c_stringfn/include/stringfn.h"
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>

typedef struct focused_t __focused_t;
struct __focused_t {
  char  *name;
  pid_t pid;
};


enum logger_mode {
  LOGGER_NONE,
  LOGGER_INFO,
  LOGGER_ERROR,
  LOGGER_DEBUG,
};

module(logger) {
  define(logger, CLIB_MODULE);

  enum logger_mode mode;
  int              PID;
  char             Name[32];
  focused_t        *_fp;

  int              (*Update)();
  int              (*GetPID)();
  char             * (*GetName)();
  void             (*info)(char *);
  void             (*debug)(char *);
  void             (*error)(char *);
};

int  logger_init(module(logger) * exports);
void logger_deinit(module(logger) * exports);

exports(logger) {
  .mode   = LOGGER_NONE,
  .Name   = "UNKNOWN",
  .PID    = -1,
  ._fp    = NULL,
  .init   = logger_init,
  .deinit = logger_deinit,
};
