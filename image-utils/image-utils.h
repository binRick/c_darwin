#pragma once
#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H
#include <Carbon/Carbon.h>
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
///////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
///////////////////////////////////////////////////

CGImageRef resize_cgimage(CGImageRef imageRef, int width, int height);
void * CompressToPNG(int width, int height, const void *rgb, const void *mask, long *outsize);

#endif
