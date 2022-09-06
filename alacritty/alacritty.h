#pragma once
#ifndef ALACRITTY_H
#define ALACRITTY_H
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
struct alacritty_t {
  pid_t pid;
  char  *path;
  char  *version;
  char  *socket;
  char  *log;
  char  *config;
  int   window_id;
};
#endif
