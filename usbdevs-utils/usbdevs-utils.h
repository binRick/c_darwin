#pragma once
#ifndef USBDEVS_UTILS_H
#define USBDEVS_UTILS_H
//////////////////////////////////////
#include "c_vector/vector/vector.h"
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
struct usbdev_t {
  char           type[4][2];
  char           *usage, *usage_page, *release_number_s;
  int            release_number;
  unsigned short vendor_id, product_id;
  char           *product_string, *manufacturer_string, *serial_number, *path;
  int            interface;
};

struct Vector *get_usb_devices_v(void);
#endif
