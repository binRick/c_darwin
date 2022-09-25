#pragma once
#ifndef USBDEVS_UTILS_C
#define USBDEVS_UTILS_C
////////////////////////////////////////////

////////////////////////////////////////////
#include "usbdevs-utils/usbdevs-utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "hidapi/hidapi/hidapi.h"
#include "hidapi/mac/hidapi_darwin.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
static bool USBDEVS_UTILS_DEBUG_MODE = false;
static void debug_usbdev(struct usbdev_t *usbdev);
static struct Vector *usbdevs_utils_iterate_devices(void);
typedef void (^usbdev_parser_b)(struct usbdev_t *usbdev, struct hid_device_info *device_info);
enum font_parser_type_t {
  USB_DEVICE_PARSER_TYPE_PATH = 1,
  USB_DEVICE_PARSER_TYPE_SERIAL_NUMBER,
  USB_DEVICE_PARSER_TYPE_VENDOR_ID,
  USB_DEVICE_PARSER_TYPE_PRODUCT_ID,
  USB_DEVICE_PARSER_TYPE_RELEASE_NUMBER,
  USB_DEVICE_PARSER_TYPE_USAGE_PAGE,
  USB_DEVICE_PARSER_TYPE_USAGE,
  USB_DEVICE_PARSER_TYPE_PRODUCT_STRING,
  USB_DEVICE_PARSER_TYPE_MANUFACTURER_STRING,
  USB_DEVICE_PARSER_TYPES_QTY,
};
struct usbdev_parser_t {
  bool enabled;
  void (^parser)(struct usbdev_t *usbdev, struct hid_device_info *device_info);
};

static struct usbdev_parser_t usbdev_parsers[USB_DEVICE_PARSER_TYPES_QTY + 1] = {
  [USB_DEVICE_PARSER_TYPE_PATH] =                                        { .enabled = true,
                                                                           .parser  = ^ void (struct usbdev_t *d, struct hid_device_info *i){ d->path = i->path; }, },
  [USB_DEVICE_PARSER_TYPE_SERIAL_NUMBER] =                               { .enabled = true,
                                                                           .parser  = ^ void (struct usbdev_t *d, struct hid_device_info *i){ asprintf(&d->serial_number, "%ls", i->serial_number); }, },
  [USB_DEVICE_PARSER_TYPE_VENDOR_ID] =                                   { .enabled = true,
                                                                           .parser  = ^ void (struct usbdev_t *d, struct hid_device_info *i){ d->vendor_id = i->vendor_id; }, },
  [USB_DEVICE_PARSER_TYPE_PRODUCT_ID] =                                  { .enabled = true,
                                                                           .parser  = ^ void (struct usbdev_t *d, struct hid_device_info *i){ d->product_id = i->product_id; }, },
  [USB_DEVICE_PARSER_TYPE_PRODUCT_STRING] =                              { .enabled = true,
                                                                           .parser  = ^ void (struct usbdev_t *d, struct hid_device_info *i){ asprintf(&d->product_string, "%ls", i->product_string); }, },
  [USB_DEVICE_PARSER_TYPE_USAGE] =                                       { .enabled = true,
                                                                           .parser  = ^ void (struct usbdev_t *d, struct hid_device_info *i){ asprintf(&d->usage, "0x%hx", i->usage); }, },
  [USB_DEVICE_PARSER_TYPE_USAGE_PAGE] =                                  { .enabled = true,
                                                                           .parser  = ^ void (struct usbdev_t *d, struct hid_device_info *i){ asprintf(&d->usage_page, "0x%hx", i->usage_page); }, },
  [USB_DEVICE_PARSER_TYPE_RELEASE_NUMBER] =                              { .enabled = true,
                                                                           .parser  = ^ void (struct usbdev_t *d, struct hid_device_info *i){
                                                                             asprintf(&d->release_number_s, "%hx", i->release_number);
                                                                             if (d->release_number_s) {
                                                                               d->release_number                      = atoi(d->release_number_s);
                                                                             }
                                                                           }, },
  [USB_DEVICE_PARSER_TYPE_MANUFACTURER_STRING] =                         { .enabled = true,
                                                                           .parser  = ^ void (struct usbdev_t *d, struct hid_device_info *i){ asprintf(&d->manufacturer_string, "%ls", i->manufacturer_string); }, },
  [USB_DEVICE_PARSER_TYPES_QTY] =                                        { 0 },
};

static void __attribute__((unused)) debug_usbdev(struct usbdev_t *usbdev){
  fprintf(stdout,
          "\n  Path                   :   %s"
          "\n  Serial Number          :   %s"
          "\n  Vendor ID              :   %d"
          "\n  Product ID             :   %d"
          "\n  Product String         :   %s"
          "\n  Usage                  :   %s"
          "\n  Usage Page             :   %s"
          "\n  Release Number         :   %d"
          "\n  Manufacturer String    :   %s"
          "%s",
          usbdev->path,
          usbdev->serial_number,
          usbdev->vendor_id,
          usbdev->product_id,
          usbdev->product_string,
          usbdev->usage,
          usbdev->usage_page,
          usbdev->release_number,
          usbdev->manufacturer_string,
          "\n"
          );
}

static void parse_usbdev(struct usbdev_t *usbdev, struct hid_device_info *device_info){
  for (size_t i = 0; i < USB_DEVICE_PARSER_TYPES_QTY; i++) {
    if (usbdev_parsers[i].enabled == true) {
      usbdev_parsers[i].parser(usbdev, device_info);
    }
  }
}
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__usbdevs_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_USBDEVS_UTILS") != NULL) {
    log_debug("Enabling usbdevs-utils Debug Mode");
    USBDEVS_UTILS_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
struct Vector *get_usb_devices_v(void){
  return(usbdevs_utils_iterate_devices());
}
static struct Vector *usbdevs_utils_iterate_devices(void){
  struct Vector          *_devs = vector_new();
  struct hid_device_info *devs, *cur_dev;

  hid_darwin_set_open_exclusive(0);
  devs    = hid_enumerate(0x0, 0x0);
  cur_dev = devs;
  while (cur_dev) {
    struct usbdev_t *usbdev = calloc(1, sizeof(struct usbdev_t));
    parse_usbdev(usbdev, cur_dev);
    vector_push(_devs, (void *)usbdev);
    cur_dev = cur_dev->next;
  }
  hid_free_enumeration(devs);
  return(_devs);
}
////////////////////////////////////////////
#endif
