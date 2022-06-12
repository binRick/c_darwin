#pragma once
#include <CoreFoundation/CoreFoundation.h>
#include <dirent.h>
#include <getopt.h>
#include <IOKit/IOKitLib.h>
#include <mach/mach.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/statvfs.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <time.h>

enum sysctls {
  MODEL = 0,
  CPU,
  MEM,
  OSTYPE,
  OSREL,
  HOSTNAME
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
