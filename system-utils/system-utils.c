#include "system-utils.h"
#include <ApplicationServices/ApplicationServices.h>
#include <Availability.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <dirent.h>
#include <getopt.h>
#include <IOKit/graphics/IOFramebufferShared.h>
#include <IOKit/graphics/IOGraphicsInterface.h>
#include <IOKit/Graphics/IOGraphicsLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOReturn.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/usb/USBSpec.h>
#include <libproc.h>
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
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <mach/host_info.h>
#include <mach/host_priv.h>
#include <mach/kern_return.h>
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARRAY_SIZE(a)    (sizeof(a) / sizeof((a)[0]))
CGDirectDisplayID get_display_id(uint32_t id);
CGDirectDisplayID get_current_display_id();

static const struct {
  const char *ctls;
  const char *names;
} SYSCTL_VALUES[] = {
  { "hw.model",                 "Model"     },
  { "machdep.cpu.brand_string", "Processor" },
  { "hw.memsize",               "Memory"    },
  { "kern.ostype",              "OS"        },
  { "kern.osrelease",           "Release"   },
  { "kern.hostname",            "Hostname"  },
};

size_t get_devices_count(){
  IOHIDManagerRef mgr;
  int             i;

  mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
  IOHIDManagerSetDeviceMatching(mgr, NULL);
  IOHIDManagerOpen(mgr, kIOHIDOptionsTypeNone);

  CFSetRef device_set = IOHIDManagerCopyDevices(mgr);

  CFIndex  num_devices = CFSetGetCount(device_set);

  return(num_devices);
}

CGImageRef display_screen_snapshot(void) {
  CGDirectDisplayID current_display = get_current_display_id();

  return(CGDisplayCreateImage(current_display));
}

/*
 * pid_stats_t * get_pid_stats(const int pid){
 * pid_stats_t *ps = malloc(sizeof(pid_stats_t));
 *
 * ps->connections_qty     = 0;
 * ps->open_files_qty      = 0;
 * ps->fds_qty             = 0;
 * ps->sockets_qty         = 0;
 * ps->listening_ports_qty = 0;
 *
 * if (pid == 0) {
 *  printf(INVALID_PID, pid);
 *  return(ps);
 * }
 *
 * int bufferSize = proc_pidinfo(pid, PROC_PIDLISTFDS, 0, 0, 0);
 *
 * if (bufferSize == -1) {
 *  printf(UNABLE_TO_GET_PROC_FDS, pid);
 *  return(ps);
 * }
 *
 * // Get the list of open FDs
 * struct proc_fdinfo *procFDInfo = (struct proc_fdinfo *)malloc(bufferSize);
 *
 * if (!procFDInfo) {
 *  printf(OUT_OF_MEMORY, bufferSize);
 *  return(ps);
 * }
 * proc_pidinfo(pid, PROC_PIDLISTFDS, 0, procFDInfo, bufferSize);
 * ps->fds_qty = bufferSize / PROC_PIDLISTFD_SIZE;
 * for (int i = 0; i < ps->fds_qty; i++) {
 *  if (procFDInfo[i].proc_fdtype == PROX_FDTYPE_VNODE) {
 *    struct vnode_fdinfowithpath vnodeInfo;
 *    int                         bytesUsed = proc_pidfdinfo(pid, procFDInfo[i].proc_fd, PROC_PIDFDVNODEPATHINFO, &vnodeInfo, PROC_PIDFDVNODEPATHINFO_SIZE);
 *    if (bytesUsed == PROC_PIDFDVNODEPATHINFO_SIZE) {
 *      ps->open_files_qty++;
 *      if (DEBUG_PID_STATS) {
 *        printf(OPEN_FILE, vnodeInfo.pvip.vip_path);
 *      }
 *    }
 *  } else if (procFDInfo[i].proc_fdtype == PROX_FDTYPE_SOCKET) { // A socket is open
 *    ps->sockets_qty++;
 *    struct socket_fdinfo socketInfo;
 *    int                  bytesUsed = proc_pidfdinfo(pid, procFDInfo[i].proc_fd, PROC_PIDFDSOCKETINFO, &socketInfo, PROC_PIDFDSOCKETINFO_SIZE);
 *    if (bytesUsed == PROC_PIDFDSOCKETINFO_SIZE) {
 *      if (socketInfo.psi.soi_family == AF_INET && socketInfo.psi.soi_kind == SOCKINFO_TCP) {
 *        int localPort  = (int)ntohs(socketInfo.psi.soi_proto.pri_tcp.tcpsi_ini.insi_lport);
 *        int remotePort = (int)ntohs(socketInfo.psi.soi_proto.pri_tcp.tcpsi_ini.insi_fport);
 *        if (remotePort == 0) { // Remote port will be 0 when the FD represents a listening socket
 *          ps->listening_ports_qty++;
 *          if (DEBUG_PID_STATS) {
 *            printf(LISTENING_ON_PORT, localPort);
 *          }
 *        } else { // Remote port will be non-0 when the FD represents communication with a remote socket
 *          ps->connections_qty++;
 *          if (DEBUG_PID_STATS) {
 *            printf(OPEN_SOCKET, localPort, remotePort);
 *          }
 *        }
 *      }
 *    }
 *  }
 * }
 *
 * return(ps);
 * }
 */

double get_cpu_time( ){
#if defined (_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
  /* Prefer high-res POSIX timers, when available. */
  {
    clockid_t       id;
    struct timespec ts;
#if _POSIX_CPUTIME > 0
    /* Clock ids vary by OS.  Query the id, if possible. */
    if (clock_getcpuclockid(0, &id) == -1)
#endif
#if defined (CLOCK_PROCESS_CPUTIME_ID)
    /* Use known clock id for AIX, Linux, or Solaris. */
    id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined (CLOCK_VIRTUAL)
    /* Use known clock id for BSD or HP-UX. */
    id = CLOCK_VIRTUAL;
#else
    id = (clockid_t)-1;
#endif
    if (id != (clockid_t)-1 && clock_gettime(id, &ts) != -1) {
      return((double)ts.tv_sec +
             (double)ts.tv_nsec / 1000000000.0);
    }
  }
#endif

#if defined (RUSAGE_SELF)
  {
    struct rusage rusage;
    if (getrusage(RUSAGE_SELF, &rusage) != -1) {
      return((double)rusage.ru_utime.tv_sec +
             (double)rusage.ru_utime.tv_usec / 1000000.0);
    }
  }
#endif

#if defined (CLOCKS_PER_SEC)
  {
    clock_t cl = clock( );
    if (cl != (clock_t)-1) {
      return((double)cl / (double)CLOCKS_PER_SEC);
    }
  }
#endif
  return(-1);                   /* Failed. */
} /* getCPUTime */

double get_cpu() {
  static double  lastT       = -1.0;
  static double  lastCPUTime = -1.0;
  struct timeval tv;
  double         nowCPUTime;
  double         nowT;
  double         percent = 0.0;

  if (gettimeofday(&tv, NULL) != 0) {
    return(-1);
  }
  nowCPUTime = get_cpu_time();
  if (nowCPUTime < 0) {
    return(-2);
  }
  nowT = (double)tv.tv_sec + (double)(tv.tv_usec / 1000000.0);
  if (lastT <= 0) {
    lastT       = nowT;
    lastCPUTime = nowCPUTime;
    return(0);
  }
  percent     = (nowCPUTime - lastCPUTime) / (nowT - lastT);
  lastT       = nowT;
  lastCPUTime = nowCPUTime;
  return(percent);
}

uint32_t os_get_core_count() {
  uint32_t count;
  size_t   size = sizeof(count);

  sysctlbyname("hw.logicalcpu", &count, &size, NULL, 0);
  return(count);
}

static void get_sysctl(enum sysctls ctl) {
  size_t len;

  if (ctl == MEM) {
    get_mem();
  } else {
    sysctlbyname(SYSCTL_VALUES[ctl].ctls, NULL, &len, NULL, 0);
    char *type = malloc(len);
    sysctlbyname(SYSCTL_VALUES[ctl].ctls, type, &len, NULL, 0);
    printf("%-10s: " "%s\n", SYSCTL_VALUES[ctl].names, type);
    free(type);
  }
}

DarwinMouseLocation_t * get_darwin_mouse_location(){
  /*
   *   exports->location = malloc(sizeof(DarwinMouseLocation_t));
   * assert(exports->location != NULL);
   * exports->event = CGEventCreate(nil);
   * assert(exports->event != NULL);
   * dml_r->point = get_darwin_mouse_point();
   *
   * dml_r->location->x = dml_r->point.x;
   * dml_r->location->y = dml_r->point.y;
   *
   * dml_r->x = dml_r->point.x;
   * dml_r->y = dml_r->point.y;
   * DarwinMouseLocation *dml = InitDarwinMouseLocation();
   *
   * assert(dml->get() == 1);
   * assert(dml->x > -1 && dml->y > -1);
   * DarwinMouseLocation_t *l = malloc(sizeof(DarwinMouseLocation_t));
   *
   * l->x = dml->x;
   * l->y = dml->y;
   * FreeDarwinMouseLocation(dml);
   */
  //return(l);
}

static const char *display_name_from_displayID(CGDirectDisplayID displayID, CFStringRef countryCode) {
  char *displayProductName = 0;

  // get a CFDictionary with a key for the preferred name of the display
  CFDictionaryRef displayInfo = IODisplayCreateInfoDictionary(CGDisplayIOServicePort(displayID), kIODisplayOnlyPreferredName);
  // retrieve the display product name
  CFStringRef     displayProductNameKey = CFStringCreateWithCString(kCFAllocatorDefault, kDisplayProductName, kCFStringEncodingUTF8);
  CFDictionaryRef localizedNames        = CFDictionaryGetValue(displayInfo, displayProductNameKey);

  CFRelease(displayProductNameKey);
  // use the first name
  if (CFDictionaryGetCount(localizedNames) > 0) {
    if (!countryCode) {
      countryCode = CFSTR("en_US");
    }
    CFStringRef displayProductNameValue = (CFStringRef)CFDictionaryGetValue(localizedNames, countryCode);
    displayProductName = CFStringCopyUTF8String(displayProductNameValue);
    CFRelease(displayProductNameValue);
  }

  CFRelease(localizedNames);

  return(displayProductName);
}

CGDirectDisplayID get_current_display_id() {
  // displays[] Quartz display ID's
  CGDirectDisplayID *displays;
  CGDisplayCount    displayCount;

  // get number of active displays
  if (CGGetActiveDisplayList(0, NULL, &displayCount) != CGDisplayNoErr) {
    fprintf(stderr, "%s: Could not get the number of active displays.\n", __func__);
    return(-1);
  }

  // allocate enough memory to hold all the display IDs we have
  displays = calloc((size_t)displayCount, sizeof(CGDirectDisplayID));

  // get the list of all active displays
  if (CGGetActiveDisplayList(displayCount, displays, &displayCount) != CGDisplayNoErr) {
    fprintf(stderr, "%s: Could not get active display list.\n", __func__);
    return(-1);
  }

  CGDirectDisplayID mainDisplay             = CGMainDisplayID();
  const char        *mainDisplayProductName = display_name_from_displayID(mainDisplay, NULL);

  printf("%s: Main Display ID: %d has product name: %s.\n", __func__, mainDisplay, mainDisplayProductName);

  for (int i = 0; i < displayCount; i++) {
    const char *displayProductName = display_name_from_displayID(displays[i], NULL);
    printf("%s: Display ID: %d has product name: %s.\n", __func__, displays[i], displayProductName);
  }

  return(mainDisplay);
}

struct DarwinDisplayResolution * get_display_resolution(CGDirectDisplayID display_id){
  struct DarwinDisplayResolution *res = malloc(sizeof(struct DarwinDisplayResolution));

  CGDisplayModeRef               mode, current_mode; CFArrayRef mode_list;

  mode_list    = CGDisplayCopyAllDisplayModes(display_id, NULL);
  current_mode = CGDisplayCopyDisplayMode(display_id);
  mode         = (CGDisplayModeRef)CFArrayGetValueAtIndex(mode_list, 0);

  res->x      = CGDisplayModeGetWidth(mode);
  res->y      = CGDisplayModeGetHeight(mode);
  res->pixels = res->x * res->y;
  res->size   = CGDisplayScreenSize(display_id);

  CFRelease(current_mode);
  CFRelease(mode_list);

  return(res);
}

uint32_t get_display_count(){
  uint32_t          displays_count;

  CGDirectDisplayID displays[8];

  CGGetActiveDisplayList(8, displays, &displays_count);

  return(displays_count);
}

CGDirectDisplayID get_display_id(uint32_t id){
  uint32_t          displays_count;

  CGDirectDisplayID display_id;
  CGDisplayModeRef  display_mode = NULL;
  CGDirectDisplayID displays[8];

  CGGetActiveDisplayList(8, displays, &displays_count);
  display_id = displays[id];
  CGDisplayModeRelease(display_mode);

  return(display_id);
}

void get_disk(void) {
  struct statvfs info;

  if (!statvfs("/", &info)) {
    unsigned long left  = (info.f_bavail * info.f_frsize);
    unsigned long total = (info.f_blocks * info.f_frsize);
    unsigned long used  = total - left;
    float         perc  = (float)used / (float)total;
    printf("Disk      : " "%.2f%% of %.2f GB\n",
           perc * 100, (float)total / 1e+09);
  }
}

void get_gpu(void) {
  io_iterator_t Iterator;
  kern_return_t err = IOServiceGetMatchingServices(kIOMasterPortDefault,
                                                   IOServiceMatching("IOPCIDevice"), &Iterator);

  if (err != KERN_SUCCESS) {
    fprintf(stderr, "IOServiceGetMatchingServices failed: %u\n", err);
  }

  for (io_service_t Device; IOIteratorIsValid(Iterator)
       && (Device = IOIteratorNext(Iterator)); IOObjectRelease(Device)) {
    CFStringRef Name = IORegistryEntrySearchCFProperty(Device,
                                                       kIOServicePlane,
                                                       CFSTR("IOName"),
                                                       kCFAllocatorDefault,
                                                       kNilOptions);

    if (Name) {
      if (CFStringCompare(Name, CFSTR("display"), 0) == kCFCompareEqualTo) {
        CFDataRef Model = IORegistryEntrySearchCFProperty(Device,
                                                          kIOServicePlane,
                                                          CFSTR("model"),
                                                          kCFAllocatorDefault,
                                                          kNilOptions);

        if (Model) {
          bool      ValueInBytes = true;
          CFTypeRef VRAM         = IORegistryEntrySearchCFProperty(Device,
                                                                   kIOServicePlane,
                                                                   CFSTR("VRAM,totalsize"),
                                                                   kCFAllocatorDefault,
                                                                   kIORegistryIterateRecursively);

          if (!VRAM) {
            ValueInBytes = false;
            VRAM         = IORegistryEntrySearchCFProperty(Device,
                                                           kIOServicePlane,
                                                           CFSTR("VRAM,totalMB"),
                                                           kCFAllocatorDefault,
                                                           kIORegistryIterateRecursively);
          }

          if (VRAM) {
            mach_vm_size_t Size = 0;
            CFTypeID       Type = CFGetTypeID(VRAM);

            if (Type == CFDataGetTypeID()) {
              Size = (CFDataGetLength(VRAM) == sizeof(uint32_t) ?
                      (mach_vm_size_t)*(const uint32_t *)CFDataGetBytePtr(VRAM):
                      *(const uint64_t *)CFDataGetBytePtr(VRAM));
            } else if (Type == CFNumberGetTypeID()) {
              CFNumberGetValue(VRAM,
                               kCFNumberSInt64Type, &Size);
            }

            if (ValueInBytes) {
              Size >>= 20;
            }

            printf("Graphics  : " "%s @ %llu MB\n", CFDataGetBytePtr(Model), Size);
            CFRelease(Model);
          } else {
            printf("Graphic  : " "%s @ Unknown VRAM Size\n",
                   CFDataGetBytePtr(Model));
            CFRelease(Model);
          }
        }
        CFRelease(Name);
      }
    }
  }
} /* gpu */

void uv_loadavg(double avg[3]) {
  struct loadavg info;
  size_t         size    = sizeof(info);
  int            which[] = { CTL_VM, VM_LOADAVG };

  if (sysctl(which, ARRAY_SIZE(which), &info, &size, NULL, 0) < 0) {
    return;
  }

  avg[0] = (double)info.ldavg[0] / info.fscale;
  avg[1] = (double)info.ldavg[1] / info.fscale;
  avg[2] = (double)info.ldavg[2] / info.fscale;
}

int uv_resident_set_memory(size_t *rss) {
  mach_msg_type_number_t count;
  task_basic_info_data_t info;
  kern_return_t          err;

  count = TASK_BASIC_INFO_COUNT;
  err   = task_info(mach_task_self(),
                    TASK_BASIC_INFO,
                    (task_info_t)&info,
                    &count);
  (void)&err;
  assert(err == KERN_SUCCESS);
  *rss = info.resident_size;

  return(0);
}

#define __unused    __attribute__((__unused__))

char *osdep_get_name(int fd, __unused char *tty) {
#ifdef __MAC_10_7
  struct proc_bsdshortinfo bsdinfo;
  pid_t                    pgrp;
  int                      ret;

  if ((pgrp = tcgetpgrp(fd)) == -1) {
    return(NULL);
  }

  ret = proc_pidinfo(pgrp, PROC_PIDT_SHORTBSDINFO, 0,
                     &bsdinfo, sizeof bsdinfo);
  if (ret == sizeof bsdinfo && *bsdinfo.pbsi_comm != '\0') {
    return(strdup(bsdinfo.pbsi_comm));
  }
#else
  int               mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, 0 };
  size_t            size;
  struct kinfo_proc kp;

  if ((mib[3] = tcgetpgrp(fd)) == -1) {
    return(NULL);
  }

  size = sizeof kp;
  if (sysctl(mib, 4, &kp, &size, NULL, 0) == -1) {
    return(NULL);
  }
  if (*kp.kp_proc.p_comm == '\0') {
    return(NULL);
  }

  return(strdup(kp.kp_proc.p_comm));
#endif
}

char *osdep_get_cwd(int fd) {
  static char               wd[PATH_MAX];
  struct proc_vnodepathinfo pathinfo;
  pid_t                     pgrp;
  int                       ret;

  if ((pgrp = tcgetpgrp(fd)) == -1) {
    return(NULL);
  }

  ret = proc_pidinfo(pgrp, PROC_PIDVNODEPATHINFO, 0,
                     &pathinfo, sizeof pathinfo);
  if (ret == sizeof pathinfo) {
    strlcpy(wd, pathinfo.pvi_cdir.vip_path, sizeof wd);
    return(wd);
  }
  return(NULL);
}

static const struct {
  const char *ctls;
  const char *names;
} values[] = {
  { "hw.model",                 "Model"     }, /* MODEL    */
  { "machdep.cpu.brand_string", "Processor" }, /* CPU       */
  { "hw.memsize",               "Memory"    }, /* MEM       */
  { "kern.ostype",              "OS"        }, /* OSTYPE   */
  { "kern.osrelease",           "Release"   }, /* OSREL    */
  { "kern.hostname",            "Hostname"  }, /* HOSTNAME */
};

void get_model(){
  size_t len = 0;

  if (!sysctlbyname("hw.model", NULL, &len, NULL, 0) && len) {
    char *model = malloc(len * sizeof(char));
    if (!sysctlbyname("hw.model", model, &len, NULL, 0)) {
      fprintf(stderr, "\nlen:%lu\n", len);
      fprintf(stderr, "\nmodel:%s\n", model);
    }
    free(model);
  }
}

void get_mem(void) {
  size_t                 len;
  mach_port_t            myHost;
  vm_statistics64_data_t vm_stat;
  vm_size_t              pageSize = 4096; /* set to 4k default */
  unsigned int           count    = HOST_VM_INFO64_COUNT;
  kern_return_t          ret;

  myHost = mach_host_self();
  uint64_t value64;

  len = sizeof(value64);

  sysctlbyname(values[2].ctls, &value64, &len, NULL, 0);
  if (host_page_size(mach_host_self(), &pageSize) == KERN_SUCCESS) {
    if ((ret = host_statistics64(myHost, HOST_VM_INFO64, (host_info64_t)&
                                 vm_stat, &count) == KERN_SUCCESS)) {
      printf(RED "%s    : "NOR "%llu MB of %.f MB\n",
             values[2].names,
             (uint64_t)(vm_stat.active_count +
                        vm_stat.inactive_count +
                        vm_stat.wire_count) * pageSize >> 20,
             value64 / 1e+06);
    }
  }
}
