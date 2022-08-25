#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <termios.h>
/////////////////////////////////////
#include "wrec-capture/wrec-capture.h"
#include "wrec-common/wrec-common.h"
/////////////////////////////////////
#include "c_string_buffer/include/stringbuffer.h"
#include "chan/src/chan.h"
#include "chan/src/queue.h"
#include "libfort/lib/fort.h"
#include "libterminput/libterminput.h"
#include "tempdir.c/tempdir.h"
#include "wildcardcmp/wildcardcmp.h"
#include "wrec-common/extern.h"
////////////////////////////////////////////////////////////
static const char *EXCLUDED_WINDOW_APP_NAMES[] = {
  "Control Center",
  "Rectangle",
  "SystemUIServer",
  "Window Server",
  NULL,
};
struct recorded_frame_t *get_first_recorded_frame(struct capture_config_t *capture_config){
  if (vector_size(capture_config->recorded_frames_v) == 0) {
    return(&(struct recorded_frame_t){
      .timestamp = 0,
    });
  }
  return((struct recorded_frame_t *)vector_get(capture_config->recorded_frames_v, 0));
}
struct recorded_frame_t *get_latest_recorded_frame(struct capture_config_t *capture_config){
  if (vector_size(capture_config->recorded_frames_v) == 0) {
    return(&(struct recorded_frame_t){
      .timestamp = 0,
    });
  }
  return((struct recorded_frame_t *)vector_get(capture_config->recorded_frames_v, vector_size(capture_config->recorded_frames_v) - 1));
}

size_t get_ms_since_last_recorded_frame(struct capture_config_t *capture_config){
  struct recorded_frame_t *latest_recoded_frame = get_latest_recorded_frame(capture_config);

  if (latest_recoded_frame->timestamp == 0) {
    return(0);
  }
  return(timestamp() - latest_recoded_frame->timestamp + latest_recoded_frame->record_dur);
}

size_t get_next_frame_interval_ms(struct capture_config_t *capture_config){
  return(1000 / capture_config->frames_per_second);
}

size_t get_next_frame_timestamp(struct capture_config_t *capture_config){
  struct recorded_frame_t *latest_recoded_frame = get_latest_recorded_frame(capture_config);

  if (latest_recoded_frame->timestamp == 0) {
    return(timestamp());
  }
  return(get_latest_recorded_frame(capture_config)->timestamp + get_next_frame_interval_ms(capture_config));
}

size_t get_ms_until_next_frame(struct capture_config_t *capture_config){
  struct recorded_frame_t *latest_recoded_frame = get_latest_recorded_frame(capture_config);

  if (latest_recoded_frame->timestamp == 0) {
    return(0);
  }
  return(get_next_frame_timestamp(capture_config) - timestamp());
}

size_t get_recorded_duration_ms(struct capture_config_t *capture_config){
  struct recorded_frame_t *first_recorded_frame = get_first_recorded_frame(capture_config);

  if (first_recorded_frame->timestamp == 0) {
    return(0);
  }
  return(timestamp() - first_recorded_frame->timestamp);
}

int compare_file_time_items(struct file_time_t *e1, struct file_time_t *e2){
  int ret = (e1->file_creation_ts > e2->file_creation_ts)
              ? 1
              : (e1->file_creation_ts < e2->file_creation_ts)
                ? -1
                : 0;

  return(ret);
}

int list_windows_table(void *ARGS) {
  struct args_t execution_args = *(struct args_t *)ARGS;
  ft_table_t    *table         = ft_create_table();

  ft_set_border_style(table, FT_SOLID_ROUND_STYLE);
  ft_set_tbl_prop(table, FT_TPROP_LEFT_MARGIN, 0);
  ft_set_tbl_prop(table, FT_TPROP_TOP_MARGIN, 0);
  ft_set_tbl_prop(table, FT_TPROP_BOTTOM_MARGIN, 0);
  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_TEXT_ALIGN, FT_ALIGNED_CENTER);
  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_BOLD);
  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
  ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_CONT_BG_COLOR, FT_COLOR_BLACK);

  ft_write_ln(table,
              "Application",
              "PID",
              "Window ID",
              "Display ID",
              "Space ID",
              "Size",
              "Position",
              "Layer",
              "Focused",
              "Visible",
              "Minimized"
              );

  struct Vector *windows = get_windows();

  for (size_t i = 0; i < vector_size(windows); i++) {
    window_t *w = (window_t *)vector_get(windows, i);
    //fprintf(stderr,"#%d\n",w->window_id);
    int      NAME_MATCH = wildcardcmp(execution_args.application_name_glob, strip_non_ascii(w->app_name));
    if (NAME_MATCH == 0) {
      continue;
    }
    if ((int)w->position.y < 25 && (int)w->size.height < 50 && w->layer > 24) {
      continue;
    }
    char **tmp           = EXCLUDED_WINDOW_APP_NAMES;
    bool excluded_window = false;
    while (*tmp != NULL && excluded_window == false) {
      if (strcmp(*tmp, w->app_name) == 0) {
        excluded_window = true;
      }

      tmp++;
    }
    if (excluded_window) {
      continue;
    }
    ft_printf_ln(table,
                 "%.20s|%d|%d|%d|%d|%dx%d|%dx%d|%d|%s|%s|%s",
                 strip_non_ascii(w->app_name),
                 w->pid,
                 w->window_id,
                 w->display_id,
                 w->space_id,
                 (int)w->size.height, (int)w->size.width,
                 (int)w->position.x, (int)w->position.y,
                 w->layer,
                 int_to_string(w->is_focused),
                 int_to_string(w->is_visible),
                 int_to_string(w->is_minimized)
                 );

    ft_set_cell_prop(table, i + 1, 0, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_CYAN);
    ft_set_cell_prop(table, i + 1, 1, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_ITALIC);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_FG_COLOR, FT_COLOR_LIGHT_MAGENTA);
    ft_set_cell_prop(table, i + 1, 2, FT_CPROP_CONT_TEXT_STYLE, FT_TSTYLE_UNDERLINED);
    ft_set_cell_prop(table, i + 1, 3, FT_CPROP_CONT_FG_COLOR, FT_COLOR_YELLOW);

    if (w->position.x != 0 || w->position.y != 0) {
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 4, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
    if (w->layer != 0) {
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }else{
      ft_set_cell_prop(table, i + 1, 5, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }
    if (w->is_focused) {
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 6, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
    if (w->is_visible) {
      ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 7, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
    if (w->is_minimized) {
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 8, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
  }

  printf("\n%s\n", ft_to_string(table));
  ft_destroy_table(table);

  return(0);
} /* list_windows_table */

void debug_resize(int WINDOW_ID,
                  char *FILE_NAME,
                  int RESIZE_TYPE, int RESIZE_VALUE,
                  int ORIGINAL_WIDTH, int ORIGINAL_HEIGHT,
                  long unsigned CAPTURE_DURATION_MS, long unsigned SAVE_DURATION_MS) {
  char *msg;

  asprintf(&msg,
           "<%d> [debug_resize] "
           "\n\t|RESIZE_TYPE:%d"
           "|RESIZE_VALUE:%s"
           "\n\t|WINDOW_ID:%d"
           "|FILE_NAME:%s"
           "|FILE_SIZE:%ld"
           "\n\t|ORIGINAL_WIDTH:%d|ORIGINAL_HEIGHT:%d"
           "\n\t|CAPTURE_DUR_MS:%ld|SAVE_DURATION_MS:%ld",
           getpid(),
           RESIZE_TYPE,
           resize_type_name(RESIZE_VALUE),
           WINDOW_ID,
           FILE_NAME,
           fsio_file_size(FILE_NAME),
           ORIGINAL_WIDTH, ORIGINAL_HEIGHT,
           CAPTURE_DURATION_MS, SAVE_DURATION_MS
           );

  log_debug("%s\n", msg);

  free(msg);
}

////////////////////////////////////////////////////////////
