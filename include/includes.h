#ifndef INCLUDES_H
#define INCLUDES_H
/**********************************/
#define _GNU_SOURCE
/**********************************/
#include <assert.h>
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <libgen.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <mach/machine.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
/**********************************/
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/IOKitLib.h>
/**********************************/
/**********************************/
//#include "../deps/libuv/include/uv/threadpool.h"
#include "../deps/libuv/include/uv.h"
#include "../deps/uptime/include/uptime/uptime.h"
//#include "../deps/libuv/src/uv-common.h"
/**********************************/
#include "../deps/str-truncate.c/src/str-truncate.h"
#include "../deps/timestamp/timestamp.h"
#include "../include/ansicodes.h"
/**********************************/
#include "../deps/bytes/bytes.h"
/**********************************/
#include "../deps/parson/parson.h"
#include "../include/timequick.h"
/**********************************/
#include "../deps/libconfuse/src/confuse.h"
#include "../deps/list/list.h"
/**********************************/
#include "../deps/ms/ms.h"
#include "../deps/trim/trim.h"
/**********************************/
#include "../deps/c_fsio/include/fsio.h"
/**********************************/
#include "../include/sql.h"
/**********************************/
#include "../include/log.h"
/**********************************/
#include "../include/color.h"
/**********************************/
#include "../include/term.h"
/**********************************/
#include "../deps/c_string_buffer/include/stringbuffer.h"
/**********************************/
#include "../deps/c_scriptexec/include/scriptexec.h"
/**********************************/
#include "../include/mouse.h"
/**********************************/
#endif
