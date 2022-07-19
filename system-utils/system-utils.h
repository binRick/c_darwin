#pragma once
#include "core-utils/core-utils.h"
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
///////////////////////////
typedef struct DarwinDisplaySize       DarwinDisplaySize;
typedef struct DarwinMouseLocation_t   DarwinMouseLocation_t;
///////////////////////////
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
struct DarwinMouseLocation_t {
  int x; int y;
};
///////////////////////////

static void get_sysctl(enum sysctls);
void get_disk(void);
static void get_pkg_count(void);
static void get_uptime(time_t *t);
void get_gpu(void);
double get_cpu();
double get_cpu_time(void);
static void curtime(void);
static void host(void);
static void kernel(void);
void get_os(void);
static void cpu(void);
static void battery(void);
size_t get_devices_count();

uint32_t get_display_count();
CGDirectDisplayID get_display_id(uint32_t id);
struct DarwinDisplayResolution * get_display_resolution(CGDirectDisplayID display_id);


#define C0     "\x1B[0m"    /* Reset  */
#define C1     "\x1B[0;32m" /* Green  */
#define C2     "\x1B[0;33m" /* Yellow */
#define C3     "\x1B[0;31m" /* Red    */
#define C4     "\x1B[0;35m" /* Purple */
#define C5     "\x1B[0;36m" /* Blue   */
#define RED    C3
#define NOR    C0


typedef struct mem {
  uint64_t total;
  uint64_t used;
  uint64_t free;
  uint64_t active;
  uint64_t inactive;
  uint64_t wired;
} mem_t;

void get_mem(void);
void get_model(void);
