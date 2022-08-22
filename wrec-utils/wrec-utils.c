#pragma once
/////////////////////////////////////
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
/////////////////////////////////////
#include "wrec-cli/wrec-cli.h"
#include "wrec-utils/wrec-utils.h"
/////////////////////////////////////
#include "bytes/bytes.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "capture/capture.h"
#include "chan/src/chan.h"
#include "chan/src/queue.h"
#include "libterminput/libterminput.h"
#include "ms/ms.h"
#include "strdup/strdup.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "wildcardcmp/wildcardcmp.h"
/////////////////////////////////////
#include "wrec-cli/wrec-cli.h"
////////////////////////////////////////////////////////////
#define RECORDED_FRAMES_QTY    vector_size(capture_config->recorded_frames_v)
static const char      *tempdir_path = NULL;
static struct args_t   execution_args;
static pthread_mutex_t *capture_config_mutex;
static pthread_t       capture_thread, wait_ctrl_d_thread;
static chan_t          *done_chan;
static const char      *EXCLUDED_WINDOW_APP_NAMES[] = {
  "Control Center",
  "Rectangle",
  "SystemUIServer",
  "Window Server",
  NULL,
};
////////////////////////////////////////////////////////////
static struct recorded_frame_t *get_first_recorded_frame(struct capture_config_t *capture_config){
  if (vector_size(capture_config->recorded_frames_v) == 0) {
    return(&(struct recorded_frame_t){
      .timestamp = 0,
    });
  }
  return((struct recorded_frame_t *)vector_get(capture_config->recorded_frames_v, 0));
}
static struct recorded_frame_t *get_latest_recorded_frame(struct capture_config_t *capture_config){
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

static size_t get_next_frame_interval_ms(struct capture_config_t *capture_config){
  return(1000 / capture_config->frames_per_second);
}

static size_t get_next_frame_timestamp(struct capture_config_t *capture_config){
  struct recorded_frame_t *latest_recoded_frame = get_latest_recorded_frame(capture_config);

  if (latest_recoded_frame->timestamp == 0) {
    return(timestamp());
  }
  return(get_latest_recorded_frame(capture_config)->timestamp + get_next_frame_interval_ms(capture_config));
}

static size_t get_ms_until_next_frame(struct capture_config_t *capture_config){
  struct recorded_frame_t *latest_recoded_frame = get_latest_recorded_frame(capture_config);

  if (latest_recoded_frame->timestamp == 0) {
    return(0);
  }
  return(get_next_frame_timestamp(capture_config) - timestamp());
}

static size_t get_recorded_duration_ms(struct capture_config_t *capture_config){
  struct recorded_frame_t *first_recorded_frame = get_first_recorded_frame(capture_config);

  if (first_recorded_frame->timestamp == 0) {
    return(0);
  }
  return(timestamp() - first_recorded_frame->timestamp);
}

////////////////////////////////////////////////////////////
static void do_capture(void *CAPTURE_CONFIG){
  if (tempdir_path == NULL) {
    tempdir_path = gettempdir();
  }
  struct capture_config_t *capture_config = (struct capture_config_t *)CAPTURE_CONFIG;

  pthread_mutex_lock(capture_config_mutex);
  bool active = capture_config->active;

  if (execution_args.verbose == true) {
    printf("capturing max %d frames, %d seconds.......\n", capture_config->max_record_frames_qty, capture_config->max_record_duration_seconds);
  }
  capture_config->started_timestamp = timestamp();
  pthread_mutex_unlock(capture_config_mutex);
  while (true) {
    pthread_mutex_lock(capture_config_mutex);
    active =
      (capture_config->active)
      && ((size_t)RECORDED_FRAMES_QTY <= (size_t)(capture_config->max_record_frames_qty))
      && ((size_t)(get_recorded_duration_ms(capture_config) / 1000) < ((size_t)capture_config->max_record_duration_seconds));
    pthread_mutex_unlock(capture_config_mutex);
    if (!active) {
      if (execution_args.verbose == true) {
        printf("active changed to false.....\n");
      }
      break;
    }
    size_t since    = get_ms_since_last_recorded_frame(capture_config);
    size_t sleep_ms = get_ms_until_next_frame(capture_config);
    sleep_ms = (since > 0) ? (sleep_ms - since) : sleep_ms;
    sleep_ms = (sleep_ms > 10000000) ? 0 : sleep_ms;
    sleep_ms = (size_t)(((float)(sleep_ms * 1000) * (float)(.98)) / 1000);

    if (execution_args.verbose == true) {
      fprintf(stderr, AC_RESETALL AC_YELLOW AC_BOLD "    [Frame Capture]     latest frame ts: %" PRIu64 " (running for %lu/%lums) (%lu/%d frames recorded) (%lums since latest frame)- sleeping for %lums" AC_RESETALL "\n",
              get_latest_recorded_frame(capture_config)->timestamp,
              get_recorded_duration_ms(capture_config), (size_t)capture_config->max_record_duration_seconds * 1000,
              RECORDED_FRAMES_QTY, capture_config->max_record_frames_qty,
              get_ms_since_last_recorded_frame(capture_config),
              sleep_ms
              );
    }

    usleep(1000 * sleep_ms);

    char   *SCREENSHOT_FILE;
    asprintf(&SCREENSHOT_FILE, "%s/window-%d-%lu.png", tempdir_path, capture_config->window_id, RECORDED_FRAMES_QTY);
    bool   ok;
    size_t ts = timestamp();
    if (execution_args.resize_type == RESIZE_BY_FACTOR && execution_args.resize_value == 1) {
      execution_args.resize_type = RESIZE_BY_NONE;
    }
    switch (execution_args.resize_type) {
    case RESIZE_BY_WIDTH:
      ok = capture_to_file_image_resize_width(capture_config->window_id, SCREENSHOT_FILE, execution_args.resize_value);
      break;
    case RESIZE_BY_HEIGHT:
      ok = capture_to_file_image_resize_height(capture_config->window_id, SCREENSHOT_FILE, execution_args.resize_value);
      break;
    case RESIZE_BY_FACTOR:
      ok = capture_to_file_image_resize_factor(capture_config->window_id, SCREENSHOT_FILE, execution_args.resize_value);
      break;
    case RESIZE_BY_NONE:
    default:
      ok = capture_to_file_image(capture_config->window_id, SCREENSHOT_FILE);
      break;
    }
    if (ok != true || fsio_file_size(SCREENSHOT_FILE) < 100) {
      continue;
    }
    struct recorded_frame_t *f = calloc(1, sizeof(struct recorded_frame_t));
    f->timestamp  = timestamp();
    f->record_dur = f->timestamp - ts;
    f->file       = SCREENSHOT_FILE;

    pthread_mutex_lock(capture_config_mutex);
    vector_push(capture_config->recorded_frames_v, f);
    pthread_mutex_unlock(capture_config_mutex);
  }
  if (execution_args.verbose == true) {
    printf("do capture done\n");
  }
  chan_send(done_chan, (void *)NULL);
} /* do_capture */

static void wait_for_control_d(){
  struct StringBuffer       *sb;
  struct libterminput_state ctx = { 0 };
  union libterminput_input  input;
  int                       r;
  struct termios            stty, saved_stty;

  if (tcgetattr(STDERR_FILENO, &stty)) {
    perror("tcgetattr STDERR_FILENO");
    return(1);
  }
  saved_stty    = stty;
  stty.c_lflag &= (tcflag_t) ~(ECHO | ICANON);
  if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &stty)) {
    perror("tcsetattr STDERR_FILENO TCSAFLUSH");
    return(1);
  }

  fprintf(stderr, AC_RESETALL AC_YELLOW "Control+d to stop capture" AC_RESETALL "\n");
  while ((r = libterminput_read(STDIN_FILENO, &input, &ctx)) > 0) {
    sb = stringbuffer_new_with_options(32, true);
    if (input.type == LIBTERMINPUT_KEYPRESS) {
      if (input.keypress.mods & LIBTERMINPUT_SHIFT) {
        stringbuffer_append_string(sb, "shift+");
      }
      if (input.keypress.mods & LIBTERMINPUT_CTRL) {
        stringbuffer_append_string(sb, "ctrl+");
      }
      stringbuffer_append_string(sb, input.keypress.symbol);
      if (execution_args.verbose == true) {
        printf("keypress:'%s'\n", stringbuffer_to_string(sb));
      }
      if (strcmp(stringbuffer_to_string(sb), "ctrl+D") == 0) {
        if (execution_args.verbose == true) {
          printf("stopping........\n");
        }
        break;
      }
    }
    stringbuffer_release(sb);
  }
  if (r < 0) {
    perror("libterminput_read STDIN_FILENO");
  }
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &saved_stty);

  chan_send(done_chan, (void *)NULL);
} /* wait_for_control_d */

static int read_captured_frames(struct capture_config_t *capture_config) {
  if (tempdir_path == NULL) {
    tempdir_path = gettempdir();
  }
  struct Vector *images_v = vector_new();
  for (size_t i = 0; i < vector_size(capture_config->recorded_frames_v); i++) {
    struct recorded_frame_t *f = vector_get(capture_config->recorded_frames_v, i);
    f->file_size = fsio_file_size(f->file);
    if (execution_args.verbose == true) {
      fprintf(stdout, AC_RESETALL AC_BLUE " - #%.5lu @%" PRIu64 " [%s] <%s> dur:%lu\n",
              i,
              f->timestamp,
              bytes_to_string(fsio_file_size(f->file)),
              f->file,
              f->record_dur
              );
    }

    vector_push(images_v, (char *)f->file);
  }
  return(0);
}

int capture_window(void *ARGS) {
  execution_args = *(struct args_t *)ARGS;
  done_chan      = chan_init(0);

  capture_config_mutex = calloc(1, sizeof(pthread_mutex_t));

  pthread_mutex_init(capture_config_mutex, NULL);
  struct capture_config_t *capture_config = calloc(1, sizeof(struct capture_config_t));

  pthread_mutex_lock(capture_config_mutex);
  capture_config->active                      = true;
  capture_config->frames_per_second           = execution_args.frames_per_second;
  capture_config->max_record_frames_qty       = execution_args.max_recorded_frames;
  capture_config->max_record_duration_seconds = execution_args.max_record_duration_seconds;
  capture_config->recorded_frames_v           = vector_new();
  capture_config->window_id                   = execution_args.window_id;
  capture_config->resize_type                 = execution_args.resize_type;
  capture_config->resize_value                = execution_args.resize_value;
  pthread_mutex_unlock(capture_config_mutex);

  int error = pthread_create(&capture_thread, NULL, do_capture, (void *)capture_config);
  if (error) {
    perror("pthread_create Error");
    return(1);
  }

  error = pthread_create(&wait_ctrl_d_thread, NULL, wait_for_control_d, (void *)NULL);
  if (error) {
    perror("pthread_create Error");
    return(1);
  }

  chan_recv(done_chan, (void *)NULL);
  chan_dispose(done_chan);

  if (execution_args.verbose == true) {
    fprintf(stderr, AC_RESETALL AC_GREEN "Capture Stopped\n");
  }
  pthread_mutex_lock(capture_config_mutex);

  capture_config->active = false;
  pthread_mutex_unlock(capture_config_mutex);
  pthread_join(&capture_thread, NULL);
  if (execution_args.verbose == true) {
    fprintf(stderr, AC_RESETALL AC_GREEN "Captured %lu frames\n", vector_size(capture_config->recorded_frames_v));
  }

  return(read_captured_frames(capture_config));
} /* capture_window */

int list_windows(void *ARGS) {
  execution_args = *(struct args_t *)ARGS;
  ft_table_t *table = ft_create_table();
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
              "Size",
              "Position",
              "Layer",
              "Focused",
              "Visible",
              "Minimized"
              );

  struct Vector *windows = get_windows();
  for (size_t i = 0; i < vector_size(windows); i++) {
    window_t *w         = (window_t *)vector_get(windows, i);
    int      NAME_MATCH = wildcardcmp(execution_args.application_name_glob, strip_non_ascii(w->app_name));
    if (NAME_MATCH == 0) {
      continue;
    }
    if ((int)w->position.y < 25 && (int)w->size.height < 50 && w->layer > 0) {
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
                 "%.20s|%d|%d|%dx%d|%dx%d|%d|%s|%s|%s",
                 strip_non_ascii(w->app_name),
                 w->pid,
                 w->window_id,
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
} /* list_windows */
