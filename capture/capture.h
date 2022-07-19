#pragma once
#include <stdio.h>
#define DEBUG_PID_ENV    false
#include <sys/sysctl.h>
////////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app-utils/app-utils.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "djbhash/src/djbhash.h"
#include "kitty/kitty.h"
#include "parson.h"
#include "str-replace.h"
#include "submodules/log.h/log.h"
#include "submodules/subprocess.h/subprocess.h"
#include "tiny-regex-c/re.h"
#include "vector.h"
#include "window-utils/window-utils.h"


#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>

bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME);
bool capture_to_file_screencapture(const int WINDOW_ID, const char *FILE_NAME);
