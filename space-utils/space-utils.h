#include "core-utils/core-utils-extern.h"
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

bool space_is_user(uint64_t sid);
int get_space_id(void);
uint64_t dsid_from_sid(uint32_t sid);
uint32_t display_id_for_space(uint32_t sid);
uint64_t display_space_id(uint32_t did);
int space_display_id(int sid);
bool space_is_fullscreen(uint64_t sid);
bool space_is_system(uint64_t sid);
bool space_is_visible(uint64_t sid);
int total_spaces(void);
uint32_t *space_window_list(uint64_t sid, int *count, bool include_minimized);
uint32_t *space_window_list_for_connection(uint64_t *space_list, int space_count, int cid, int *count, bool include_minimized);
