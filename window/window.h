#pragma once
#ifndef WINDOW_H
#define WINDOW_H
//////////////////////////////////////
#include "frameworks/frameworks.h"
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
static int window_event_types[] = {
  kEventWindowHidden,
  kEventWindowActivated,
  kEventWindowBoundsChanged,
  kEventWindowResizeStarted,
  kEventWindowResizeCompleted,
  kEventWindowClose,
  kEventWindowCollapsed,
  kEventWindowDeactivated,
};
#endif
