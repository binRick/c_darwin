#include "system-utils.h"
#include <sys/sysctl.h>
#include <sys/types.h>

#include <Availability.h>
#include <libproc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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


static void curtime(void) {
  time_t t;

  t = time(NULL);

  printf("Time:     : " "%s", ctime(&t));
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

  sysctlbyname(values[2].ctls, &value64, &len, NULL, 0);
  if (host_page_size(mach_host_self(), &pageSize) == KERN_SUCCESS) {
    if ((ret = host_statistics64(myHost, HOST_VM_INFO64, (host_info64_t)&
                                 vm_stat, &count) == KERN_SUCCESS)) {
      printf("%s    : " "%llu MB of %.f MB\n",
             values[2].names,
             (uint64_t)(vm_stat.active_count +
                        vm_stat.inactive_count +
                        vm_stat.wire_count) * pageSize >> 20,
             value64 / 1e+06);
    }
  }
}


static void get_sysctl(enum sysctls ctl) {
  size_t len;

  if (ctl == MEM) {
    mem();
  } else {
    sysctlbyname(values[ctl].ctls, NULL, &len, NULL, 0);
    char *type = malloc(len);
    sysctlbyname(values[ctl].ctls, type, &len, NULL, 0);
    printf("%-10s: " "%s\n", values[ctl].names, type);
    free(type);
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
#define DBDIR    "/opt/pkg/var"


static void get_pkg_count(void) {
  int           pkgs = 0;
  struct dirent **namelist;

  printf("Packages  : ");
  pkgs = scandir(DBDIR, &namelist, NULL, NULL);

  if (pkgs < 0) {
    printf("no packages found\n");
    return;
  }

  for (int i = 0; i < pkgs; i++) {
    free(namelist[i]);
  }
  free(namelist);

  /* amount of directories found excluding . .. and the pkg.byfile.db */
  printf("%d\n", pkgs - 3);
}


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
  /* task_info(TASK_BASIC_INFO) cannot really fail. Anything other than
   * KERN_SUCCESS implies a libuv bug.
   */
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
