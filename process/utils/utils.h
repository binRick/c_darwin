#pragma once
#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H
#include <fnmatch.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
///////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "app/utils/utils.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core/utils/utils.h"
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
#define DEBUG_PID_ENV    false
#include <sys/sysctl.h>
////////////////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_vector/vector/vector.h"
#include "kitty/kitty.h"
#include "stringfn.h"
//////////////////////////////////////////
int *get_all_pids();
int *get_app_pids();
int *get_window_pids();
struct kinfo_proc *proc_info_for_pid(pid_t pid);
int get_kinfo_proc(pid_t pid, struct kinfo_proc *kp);
char * pid_path(pid_t pid);
struct Vector *get_all_processes();
struct Vector *get_process_env(int process);
char *get_my_cwd();
int get_process_ppid(int pid);
struct Vector *get_process_ppids(int pid);
struct Vector *get_child_pids(int PID);
int get_pid_by_env_key_val(const char *key, const char *val);
char *get_pid_cwd(pid_t pid);
bool darwin_set_process_title(const char *title);
///////////////////////////////////////////////////
struct open_connection_t {
  int local_port, remote_port;
};
struct process_info_t {
  int           pid;
  bool          success;
  struct Vector *open_files_v, *open_connections_v, *open_ports_v, *ppids_v, *env_v, *child_pids_v;
  pid_t         ppid;
  unsigned long started, dur;
};
bool focus_pid(pid_t pid);
struct Vector *get_window_infos_v();
struct process_info_t *get_process_info(int pid);
void process_info_release(struct process_info_t *I);
pid_t PSN2PID(ProcessSerialNumber psn);
ProcessSerialNumber PID2PSN(pid_t pid);
int get_focused_pid();
struct Vector *get_all_process_infos_v();
void get_cputime(void);
struct Vector *get_window_infos_brief_named_v(char *NAMED);
struct Vector *get_window_infos_brief_by_id(size_t ID);
bool run_osascript(char *OSASCRIPT_CONTENTS);
int open_system_preferences_get_window_id();
int get_focused_window_id();
uint32_t ax_window_id(AXUIElementRef ref);
pid_t ax_window_pid(AXUIElementRef ref);
bool process_utils_move_directory_contents(const char *SOURCE_DIRECTORY, const char *DESTINATION_DIRECTORY);
struct Vector *get_window_infos_v();
struct Vector *get_window_infos_brief_v();
struct Vector *get_window_infos_id_v(size_t ID);
struct Vector *get_window_pid_infos(pid_t pid);
int get_window_id_display_id(size_t window_id);
#define SET_FILE                                           \
  "tell application \"System Events\"\n"                   \
  "    keystroke \"G\" using {command down, shift down}\n" \
  "    delay 1\n"                                          \
  "    keystroke \"%s\"\n"                                 \
  "    delay 1\n"                                          \
  "    keystroke return\n"                                 \
  "    delay 1\n"                                          \
  "    keystroke return\n"                                 \
  "    delay 1\n"                                          \
  "end tell\n"

#define CLOSE_SYSTEM_PREFERENCES                                         \
  "if running of application \"System Preferences\" then\n"              \
  "    try\n"                                                            \
  "        tell application \"System Preferences\" to quit\n"            \
  "    on error\n"                                                       \
  "        do shell script \"killall 'System Preferences'\"\n"           \
  "    end try\n"                                                        \
  "    delay 0.01\n"                                                     \
  "end if\n"                                                             \
  "repeat while running of application \"System Preferences\" is true\n" \
  "    delay 0.01\n"                                                     \
  "end repeat\n"
#define OPEN_SYSTEM_PREFERENCES_PRIVACY_ACCESSIBILITY_WINDOW_OSASCRIPT_CMD \
  "tell application \"System Preferences\"\n"                              \
  "    set securityPane to pane id \"com.apple.preference.security\"\n"    \
  "    tell securityPane to reveal anchor \"Privacy_Accessibility\"\n"     \
  "    activate\n"                                                         \
  "end tell\n"

///////////////////////////////////////////////////////////////////////////////
#define CLOSE_SYSTEM_PREFERENCES_AND_OPEN_SYSTEM_PREFERENCES_PRIVACY_ACCESSIBILITY_WINDOW_OSASCRIPT_CMD \
  CLOSE_SYSTEM_PREFERENCES OPEN_SYSTEM_PREFERENCES_PRIVACY_ACCESSIBILITY_WINDOW_OSASCRIPT_CMD
bool run_cmd_in_parent(char **cmd_a);
#endif
