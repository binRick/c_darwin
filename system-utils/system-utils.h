#pragma once
#include <assert.h>
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <dirent.h>
#include <getopt.h>
#include <IOKit/IOKitLib.h>
#include <mach/mach.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/statvfs.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct DarwinDisplaySize DarwinDisplaySize;
enum sysctls {
  MODEL = 0,
  CPU,
  MEM,
  OSTYPE,
  OSREL,
  HOSTNAME
};

struct DarwinDisplayResolution {
  size_t x, y;
  CGSize size;
  int    pixels;
};

struct DarwinDisplaySize {
  double width; double height;
};

static void get_sysctl(enum sysctls);
static void disk(void);
static void get_pkg_count(void);
static void uptime(time_t *nowp);
static void gpu(void);
static void mem(void);
static void curtime(void);
static void host(void);
static void kernel(void);
static void os(void);
static void cpu(void);
static void battery(void);

uint32_t get_display_count();
CGDirectDisplayID get_display_id(uint32_t id);
struct DarwinDisplayResolution * get_display_resolution(CGDirectDisplayID display_id);
