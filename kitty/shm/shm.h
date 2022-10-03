#pragma once
#ifndef KITTY_SHM_H
#define KITTY_SHM_H
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
int kitty_shm_get(void);
int kitty_shm_set(void);
char *kitty_shm_get_shm_image_path_msg(char *image_path);
char *kitty_msg_get_display_image_by_path(char *image_path);
char *kitty_shm_get_image_path_msg(char *image_path);
#endif
