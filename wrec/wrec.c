#pragma once
#include "c_string_buffer/include/stringbuffer.h"
#include "capture/capture.h"
#include "chan/src/chan.h"
#include "chan/src/queue.h"
#include "dbg/dbg.h"
#include "generic-print/print.h"
#include "libterminput/libterminput.h"
#include "timestamp/timestamp.h"
#include "wrec-cli/wrec-cli.h"
#include "wrec/wrec.h"
#include <pthread.h>
#include <termios.h>
#include <unistd.h>
////////////////////////////////////////////////////////////
struct args_t   execution_args;
////////////////////////////////////////////////////////////
pthread_mutex_t *capture_config_mutex;
pthread_t       capture_thread, wait_ctrl_d_thread;
chan_t          *done_chan;
////////////////////////////////////////////////////////////
#define RECORDED_FRAMES_QTY    vector_size(capture_config->recorded_frames_v)
static struct recorded_frame_t *get_first_recorded_frame(struct capture_config_t *capture_config){
  if (vector_size(capture_config->recorded_frames_v) == 0) {
    return &(struct recorded_frame_t){
             .timestamp = 0,
    }
  }
  return((struct recorded_frame_t *)vector_get(capture_config->recorded_frames_v, 0));
}
static struct recorded_frame_t *get_latest_recorded_frame(struct capture_config_t *capture_config){
  if (vector_size(capture_config->recorded_frames_v) == 0) {
    return &(struct recorded_frame_t){
             .timestamp = 0,
    }
  }
  return((struct recorded_frame_t *)vector_get(capture_config->recorded_frames_v, vector_size(capture_config->recorded_frames_v) - 1));
}


size_t get_ms_since_last_recorded_frame(struct capture_config_t *capture_config){
  struct recorded_frame_t *latest_recoded_frame = get_latest_recorded_frame(capture_config);

  if (latest_recoded_frame->timestamp == 0) {
    return(0);
  }
  return(timestamp() - latest_recoded_frame->timestamp);
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


////////////////////////////////////////////////////////////
void do_capture(void *CAPTURE_CONFIG){
  struct capture_config_t *capture_config = (struct capture_config_t *)CAPTURE_CONFIG;

  pthread_mutex_lock(capture_config_mutex);
  bool active = capture_config->active;

  if (execution_args.verbose) {
    printf("capturing max %d frames, %d seconds.......\n", capture_config->max_frames_qty, capture_config->max_duration_seconds);
  }
  pthread_mutex_unlock(capture_config_mutex);

  while (true) {
    pthread_mutex_lock(capture_config_mutex);
    if (capture_config->started_timestamp == 0) {
      capture_config->started_timestamp = timestamp();
    }
    active =
      (capture_config->active)
      && (RECORDED_FRAMES_QTY < capture_config->max_frames_qty)
      && ((size_t)(get_recorded_duration_ms(capture_config) / 1000) < ((size_t)capture_config->max_duration_seconds));
    pthread_mutex_unlock(capture_config_mutex);
    if (!active) {
      if (execution_args.verbose) {
        printf("active changed to false.....\n");
      }
      break;
    }
    size_t sleep_ms = get_ms_until_next_frame(capture_config);

    if (execution_args.verbose) {
      fprintf(stderr, "[active] capturing frame\n");
    }
    if (execution_args.verbose) {
      fprintf(stderr, "         latest frame ts: %" PRIu64 " (running for %lu/%lus) (%lu/%d frames recorded) (%lums since latest frame)- sleeping for %lums\n",
              get_latest_recorded_frame(capture_config)->timestamp,
              get_recorded_duration_ms(capture_config) / 1000, (size_t)capture_config->max_duration_seconds,
              RECORDED_FRAMES_QTY, capture_config->max_frames_qty,
              get_ms_since_last_recorded_frame(capture_config),
              sleep_ms
              );
    }

    usleep(1000 * sleep_ms);

    char *SCREENSHOT_FILE = malloc(1024);
    sprintf(SCREENSHOT_FILE, "/tmp/window-%d-%lu.png", capture_config->window_id, RECORDED_FRAMES_QTY);
    if (execution_args.verbose) {
      dbg(SCREENSHOT_FILE, %s);
    }
    bool                    ok = capture_to_file_image(capture_config->window_id, SCREENSHOT_FILE);
    struct recorded_frame_t *f = malloc(sizeof(struct recorded_frame_t));
    f->timestamp = timestamp();
    f->file      = SCREENSHOT_FILE;

    pthread_mutex_lock(capture_config_mutex);
    vector_push(capture_config->recorded_frames_v, f);
    pthread_mutex_unlock(capture_config_mutex);
  }
  if (execution_args.verbose) {
    printf("do capture done\n");
  }

  printf("[capture thread] sending done chan\n");
  chan_send(done_chan, (void *)NULL);
  printf("[capture thread] sent done chan\n");
} /* do_capture */


void wait_for_control_d(){
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
      if (execution_args.verbose) {
        printf("keypress:'%s'\n", stringbuffer_to_string(sb));
      }
      if (strcmp(stringbuffer_to_string(sb), "ctrl+D") == 0) {
        if (execution_args.verbose) {
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

  printf("[wait for ctrl-d] sending done chan\n");
  chan_send(done_chan, (void *)NULL);
  printf("[wait for ctrl-d] sent done chan\n");
} /* wait_for_control_d */


int capture_window(void *ARGS) {
  execution_args = *(struct args_t *)ARGS;
  struct Vector *window_ids = get_windows_ids();
  done_chan = chan_init(0);

  capture_config_mutex = malloc(sizeof(pthread_mutex_t));

  pthread_mutex_init(capture_config_mutex, NULL);
  struct capture_config_t *capture_config = malloc(sizeof(struct capture_config_t));

  pthread_mutex_lock(capture_config_mutex);
  capture_config->active               = true;
  capture_config->frames_per_second    = 2;
  capture_config->max_frames_qty       = 30;
  capture_config->max_duration_seconds = 2;
  capture_config->recorded_frames_v    = vector_new();
  capture_config->window_id            = execution_args.window_id;
  pthread_mutex_unlock(capture_config_mutex);

  if (execution_args.verbose) {
    dbg(execution_args.window_id, %d);
    dbg(vector_size(window_ids), %lu);
  }

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

  void *msg;
  printf("waiting for done chan\n");
  chan_recv(done_chan, &msg);
  printf("received done chan\n");
  chan_dispose(done_chan);


  if (execution_args.verbose) {
    fprintf(stderr, AC_RESETALL AC_GREEN "Capture Stopped\n");
  }

  fprintf(stderr, AC_RESETALL AC_GREEN "Locking capture_config_mutex\n");
  pthread_mutex_lock(capture_config_mutex);
  fprintf(stderr, AC_RESETALL AC_GREEN "Locked capture_config_mutex\n");

  capture_config->active = false;
  pthread_mutex_unlock(capture_config_mutex);
  if (execution_args.verbose) {
    fprintf(stderr, AC_RESETALL AC_GREEN "Joining capture thread\n");
  }
  pthread_join(&capture_thread, NULL);
  if (execution_args.verbose) {
    fprintf(stderr, AC_RESETALL AC_GREEN "Capture Thread Joined\n");
  }

  pthread_mutex_lock(capture_config_mutex);
  fprintf(stderr, AC_RESETALL AC_GREEN "Captured %lu frames\n", vector_size(capture_config->recorded_frames_v));
  for (size_t i = 0; i < vector_size(capture_config->recorded_frames_v); i++) {
    struct recorded_frame_t *f = vector_get(capture_config->recorded_frames_v, i);
    fprintf(stdout, AC_RESETALL AC_BLUE " - %s @ %" PRIu64 "\n",
            f->file,
            f->timestamp
            );
    pthread_mutex_unlock(capture_config_mutex);
  }
  return(0);
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
              "Minimized",
              "PPID"

              );

  struct Vector *windows = get_windows();

  for (size_t i = 0; i < vector_size(windows); i++) {
    window_t *w = (window_t *)vector_get(windows, i);
    ft_printf_ln(table,
                 "%.20s|%d|%d|%dx%d|%dx%d|%d|%s|%s|%s|%d",
                 strip_non_ascii(w->app_name),
                 w->pid,
                 w->window_id,
                 (int)w->size.height, (int)w->size.width,
                 (int)w->position.x, (int)w->position.y,
                 w->layer,
                 int_to_string(w->is_focused),
                 int_to_string(w->is_visible),
                 int_to_string(w->is_minimized),
                 w->pid_info.kp_eproc.e_ppid
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
    if (w->pid_info.kp_eproc.e_ppid > 1) {
      ft_set_cell_prop(table, i + 1, 9, FT_CPROP_CONT_FG_COLOR, FT_COLOR_GREEN);
    }else{
      ft_set_cell_prop(table, i + 1, 9, FT_CPROP_CONT_FG_COLOR, FT_COLOR_RED);
    }
  }

  printf("\n%s\n", ft_to_string(table));
  ft_destroy_table(table);

  return(0);
} /* list_windows */


int wrec0() {
  printf("wrec0............\n");
  return(0);
}
