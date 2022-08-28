#include "core-utils/core-utils.h"
#include "window-utils/window-utils.h"
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

int get_space_id(void);
void set_space_by_index(int space);
int is_full_screen(void);
int total_spaces(void);
int get_space_via_keywin(void);

void CoreDockGetWorkspacesCount(int *rows, int *cols);
int space_display_id(int sid);

uint32_t *space_window_list_for_connection(uint64_t *space_list, int space_count, int cid, int *count, bool include_minimized);
uint32_t *space_window_list(uint64_t sid, int *count, bool include_minimized);
bool space_is_user(uint64_t sid);
bool space_is_fullscreen(uint64_t sid);
bool space_is_system(uint64_t sid);
bool space_is_visible(uint64_t sid);
CGPoint display_center(uint32_t did);
CGRect display_bounds(uint32_t did);
CFStringRef display_uuid(uint32_t did);
uint32_t display_id(CFStringRef uuid);
void *ts_alloc_aligned(uint64_t elem_size, uint64_t elem_count);
uint64_t *display_space_list(uint32_t did, int *count);
uint32_t *space_minimized_window_list(uint64_t sid, int *count);
int get_window_id_space_id(int window_id);
