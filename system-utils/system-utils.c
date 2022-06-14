#include "system-utils.h"
#include <Availability.h>
#include <libproc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>
#define ARRAY_SIZE(a)    (sizeof(a) / sizeof((a)[0]))

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


static void get_sysctl(enum sysctls ctl) {
  size_t len;

  if (ctl == MEM) {
    mem();
  } else {
    sysctlbyname(SYSCTL_VALUES[ctl].ctls, NULL, &len, NULL, 0);
    char *type = malloc(len);
    sysctlbyname(SYSCTL_VALUES[ctl].ctls, type, &len, NULL, 0);
    printf("%-10s: " "%s\n", SYSCTL_VALUES[ctl].names, type);
    free(type);
  }
}


static inline CGPoint get_darwin_mouse_point() {
//  return(CGEventGetLocation(dml_r->event));
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


static void disk(void) {
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


static void mem(void) {
  size_t                 len;
  mach_port_t            myHost;
  vm_statistics64_data_t vm_stat;
  vm_size_t              pageSize = 4096; /* set to 4k default */
  unsigned int           count    = HOST_VM_INFO64_COUNT;
  kern_return_t          ret;

  myHost = mach_host_self();
  uint64_t value64;

  len = sizeof(value64);

  sysctlbyname(SYSCTL_VALUES[2].ctls, &value64, &len, NULL, 0);
  if (host_page_size(mach_host_self(), &pageSize) == KERN_SUCCESS) {
    if ((ret = host_statistics64(myHost, HOST_VM_INFO64, (host_info64_t)&
                                 vm_stat, &count) == KERN_SUCCESS)) {
      printf("%s    : " "%llu MB of %.f MB\n",
             SYSCTL_VALUES[2].names,
             (uint64_t)(vm_stat.active_count +
                        vm_stat.inactive_count +
                        vm_stat.wire_count) * pageSize >> 20,
             value64 / 1e+06);
    }
  }
}


static void gpu(void) {
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


static void uptime(time_t *nowp) {
  struct timeval boottime;
  time_t         uptime;
  int            days, hrs, mins, secs;
  int            mib[2];
  size_t         size;
  char           buf[256];

  if (strftime(buf, sizeof(buf), NULL, localtime(nowp))) {
    mib[0] = CTL_KERN;
  }

  mib[1] = KERN_BOOTTIME;
  size   = sizeof(boottime);

  if (sysctl(mib, 2, &boottime, &size, NULL, 0) != -1
      && boottime.tv_sec) {
    uptime = *nowp - boottime.tv_sec;

    if (uptime > 60) {
      uptime += 30;
    }

    days    = (int)uptime / 86400;
    uptime %= 86400;
    hrs     = (int)uptime / 3600;
    uptime %= 3600;
    mins    = (int)uptime / 60;
    secs    = uptime % 60;
    printf("Uptime    : ");

    if (days > 0) {
      printf("%d day%s", days, days > 1 ? "s " : " ");
    }

    if (hrs > 0 && mins > 0) {
      printf("%02d:%02d", hrs, mins);
    }else if (hrs == 0 && mins > 0) {
      printf("0:%02d", mins);
    }else{
      printf("0:00");
    }

    putchar('\n');
  }
} /* uptime */


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
