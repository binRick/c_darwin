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
bool write_cgimage_ref_to_tif_file_path(CGImageRef im, char *tif_file_path);
CGImageRef cgimageref_to_grayscale(CGImageRef im);
CGImageRef resize_cgimage_factor(CGImageRef imageRef, double resize_factor);
CGImageRef resize_png_file_factor(FILE *fp, double resize_factor);
CGImageRef rgb_pixels_to_png_cgimage_ref(unsigned char *rgb_pixels, int width, int height);
CGImageRef png_file_to_grayscale_cgimage_ref_resized(FILE *fp, double resize_factor);

#endif
