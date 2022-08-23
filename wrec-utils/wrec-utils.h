#pragma once
#include <stdio.h>
#define DEBUG_PID_ENV    false
#include <ctype.h>
#include <libproc.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/sysctl.h>
#include <sys/time.h>
////////////////////////////////////////////////////////
#include "wrec-common/wrec-common.h"
////////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_vector/vector/vector.h"
#include "djbhash/src/djbhash.h"
#include "fsio.h"
#include "parson.h"
#include "str-replace.h"
#include "string-utils/string-utils.h"
#include "stringbuffer.h"
#include "stringfn.h"
#include "submodules/libfort/src/fort.h"
#include "tiny-regex-c/re.h"
#include "window-utils/window-utils.h"
//////////////////////////////////////////
int list_windows();
int capture_window();
//////////////////////////////////////////
