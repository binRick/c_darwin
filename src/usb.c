#include "../include/includes.h"


int get_usb_devices_qty(){
  IOHIDManagerRef mgr;
  int             i;

  mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
  IOHIDManagerSetDeviceMatching(mgr, NULL);
  IOHIDManagerOpen(mgr, kIOHIDOptionsTypeNone);

  CFSetRef device_set = IOHIDManagerCopyDevices(mgr);

  CFIndex  num_devices = CFSetGetCount(device_set);

  return((int)num_devices);
}
