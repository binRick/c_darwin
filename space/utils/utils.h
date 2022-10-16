#pragma once
#ifndef SPACE_UTILS_H
#define SPACE_UTILS_H
#include "core/utils/utils.h"
#include "frameworks/frameworks.h"
//#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
struct space_t {
  int           id;
  bool          is_current;
  struct Vector *window_ids_v;
};
struct sqldbal_db;
/////////////////////////////////////////////////
//uint32_t get_display_id_for_space(uint32_t sid);
uint64_t get_display_space_id(uint32_t did);
int get_space_display_id(int sid);
/////////////////////////////////////////////////
bool space_db_load(struct sqldbal_db *db);
struct Vector *get_space_window_ids_v(size_t space_id);
struct Vector *get_space_ids_v();
struct Vector *get_spaces_v();
/////////////////////////////////////////////////
bool get_space_is_user(uint64_t sid);
bool get_space_is_fullscreen(uint64_t sid);
bool get_space_is_system(uint64_t sid);
bool get_space_is_visible(uint64_t sid);
/////////////////////////////////////////////////
int get_current_space_id(void);
CGImageRef capture_space_id_rect(size_t space_id, CGRect rect);
CGImageRef capture_space_id_width(size_t space_id, size_t width);
CGImageRef capture_space_id_height(size_t space_id, size_t height);
int get_total_spaces(void);
uint32_t *get_space_window_list(uint64_t sid, int *count, bool include_minimized);
uint32_t *get_space_window_list_for_connection(uint64_t *space_list, int space_count, int cid, int *count, bool include_minimized);
int get_space_type(uint64_t sid);
char *get_space_uuid(uint64_t sid);
void set_space_by_index(int space);
CGImageRef capture_space_id(uint32_t sid);
uint32_t *get_space_minimized_window_list(uint64_t sid, int *count);
struct Vector *get_space_minimized_window_ids_v(size_t space_id);
struct Vector *get_space_non_minimized_window_ids_v(size_t space_id);
int get_space_management_mode(int space_id);
bool get_space_is_active(int space_id);
bool get_space_can_create_tile(int space_id);
CGRect get_space_rect(int space_id);
struct Vector *get_space_owners(int space_id);
CGImageRef preview_space_id(uint32_t sid);

#endif
