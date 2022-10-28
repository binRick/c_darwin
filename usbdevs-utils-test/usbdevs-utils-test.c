////////////////////////////////////////////
#include "hidapi/hidapi/hidapi.h"
#include "hidapi/mac/hidapi_darwin.h"
#include "libusb/libusb/libusb.h"
#include "libusb/libusb/os/darwin_usb.h"

////////////////////////////////////////////
#include "c_greatest/greatest/greatest.h"
#include "usbdevs-utils-test/usbdevs-utils-test.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

static void print_devs(libusb_device **devs){
  libusb_device *dev;
  int           i = 0, j = 0;
  uint8_t       path[8];

  while ((dev = devs[i++]) != NULL) {
    struct libusb_device_descriptor desc;
    int                             r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      fprintf(stderr, "failed to get device descriptor");
      return;
    }

    printf("%04x:%04x (bus %d, device %d)",
           desc.idVendor, desc.idProduct,
           libusb_get_bus_number(dev), libusb_get_device_address(dev));

    r = libusb_get_port_numbers(dev, path, sizeof(path));
    if (r > 0) {
      printf(" path: %d", path[0]);
      for (j = 1; j < r; j++)
        printf(".%d", path[j]);
    }
    printf("\n");
  }
}

////////////////////////////////////////////
TEST t_usbdevs_utils_test(){
  libusb_device **devs;
  int           r;
  ssize_t       cnt;

  r = libusb_init(NULL);
  if (r < 0)
    return(r);

  cnt = libusb_get_device_list(NULL, &devs);
  ASSERT_GT(cnt, 0);
  if (cnt < 0) {
    libusb_exit(NULL);
    return((int)cnt);
  }

  print_devs(devs);
  libusb_free_device_list(devs, 1);

  libusb_exit(NULL);
  PASS();
}

TEST t_hidapi(void){
  struct hid_device_info *devs, *cur_dev;

  hid_darwin_set_open_exclusive(0);
  devs    = hid_enumerate(0x0, 0x0);
  cur_dev = devs;
  while (cur_dev) {
    printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path,
           cur_dev->serial_number);
    printf("\n");
    printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
    printf("  Product:      %ls\n", cur_dev->product_string);
    printf("  Release:      %hx\n", cur_dev->release_number);
    printf("  Interface:    %d\n", cur_dev->interface_number);
    printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
    printf("\n");
    cur_dev = cur_dev->next;
  }
  hid_free_enumeration(devs);

  PASS();
}
SUITE(s_usbdevs_utils_test) {
  RUN_TEST(t_usbdevs_utils_test);
  RUN_TEST(t_hidapi);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_usbdevs_utils_test);
  GREATEST_MAIN_END();
}
