#pragma once
#ifndef SDL_UTILS_H
#define SDL_UTILS_H
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <unistd.h>
//////////////////////////////////////
typedef struct _VipsImage VipsImage;
int sdl_utils_main(void);
int su_vi(VipsImage *vi);

SDL_Surface *vips_to_surface(VipsImage *vi);

#endif
