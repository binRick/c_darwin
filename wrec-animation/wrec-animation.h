#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
///////////////////////////////////////
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
////////////////////////////////////////////
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_vector/vector/vector.h"
#include "libfort/lib/fort.h"
#include "msf_gif/msf_gif.h"
#include "string-utils/string-utils.h"
#include "submodules/tinydir/tinydir.h"
#include "tempdir.c/tempdir.h"
#include "window-utils/window-utils.h"
///////////////////////////////////
int load_pngs_create_animated_gif(const char *PATH);
///////////////////////////////////
