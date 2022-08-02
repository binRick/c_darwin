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
#include "assertf/assertf.h"
#include "b64.c/b64.h"
#include "bench/bench.h"
#include "bytes/bytes.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "capture/capture.h"
#include "chan/src/chan.h"
#include "chan/src/queue.h"
#include "dbg/dbg.h"
#include "generic-print/print.h"
#include "libterminput/libterminput.h"
#include "ms/ms.h"
#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif
#include "progress.c/progress.h"
#include "rwimg/readimage.h"
#include "rwimg/writeimage.h"
#include "strdup/strdup.h"
#include "subprocess.h/subprocess.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "which/src/which.h"
#include "wildcardcmp/wildcardcmp.h"
#include "wrec-cli/wrec-cli.h"
#define PROGRESS_ENABLED      false
#define FFMPEG_LOOP_QTY       -1
#define FFMPEG_CROP_HEIGHT    50
#define READ_BUFFER_SIZE      1024 * 16
////////////////////////////////////////////////////////////
static void db_progress_start(progress_data_t *data);
static void db_progress(progress_data_t *data);
static void db_progress_end(progress_data_t *data);

static const char *tempdir_path = NULL;

////////////////////////////////////////////////////////////
struct args_t   execution_args;
pthread_mutex_t *capture_config_mutex;
pthread_t       capture_thread, wait_ctrl_d_thread;
chan_t          *done_chan;
////////////////////////////////////////////////////////////

struct animated_gif_conversion_config_t {
  struct Vector *images_v;
  char          *animated_gif_file;
  size_t        frames_per_second;
  size_t        window_id;
  char          *ffmpeg_log_level;
  int           ffmpeg_crop_height;
  int           ffmpeg_loop_qty;
  bool          debug_mode;
};

struct animated_gif_conversion_result_t {
  struct Vector        *images_v;
  char                 *ffmpeg_path, *ffmpeg_cmd;
  size_t               window_id;
  struct subprocess_s  subprocess;
  int                  subprocess_result;
  int                  subprocess_join_result;
  int                  subprocess_exited;
  bool                 animated_gif_file_created;
  char                 *READ_STDOUT, *READ_STDERR;
  struct  StringBuffer *STDOUT_BUFFER, *STDERR_BUFFER;
  size_t               stdout_bytes_read, stderr_bytes_read;
  char                 stdout_buffer[READ_BUFFER_SIZE], stderr_buffer[READ_BUFFER_SIZE];
};


static bool convert_images_to_animated_gif_ffmpeg(struct animated_gif_conversion_config_t *config){
  if (tempdir_path == NULL) {
    tempdir_path = gettempdir();
  }
  struct animated_gif_conversion_result_t r = {
    .ffmpeg_path               = (char *)which_path("ffmpeg", getenv("PATH")),
    .ffmpeg_cmd                = NULL,
    .subprocess_result         = -1,
    .subprocess_exited         = -1,
    .subprocess                = NULL,
    .animated_gif_file_created = false,
    .STDOUT_BUFFER             = stringbuffer_new(),
    .STDERR_BUFFER             = stringbuffer_new(),
    .stdout_bytes_read         = 0,
    .stderr_bytes_read         = 0,
    .stdout_buffer             = { 0 },
    .stderr_buffer             = { 0 },
  };

  asprintf(&r.ffmpeg_cmd, "env AV_LOG_FORCE_COLOR=1 '%s' -stats -y -f image2 -framerate '%lu' -i '%s/window-%lu-%%d.png' -loglevel '%s' -vf 'crop=in_w:in_h-%d' -loop '%d' '%s'",
           r.ffmpeg_path,
           config->frames_per_second,
           tempdir_path,
           config->window_id,
           config->ffmpeg_log_level,
           FFMPEG_CROP_HEIGHT,
           FFMPEG_LOOP_QTY,
           config->animated_gif_file
           );
  const char *command_line[] = { "/bin/sh", "--norc", "--noprofile", "-c", r.ffmpeg_cmd, NULL };

  if (config->debug_mode) {
    dbg(r.ffmpeg_path, %s);
    dbg(config->animated_gif_file, %s);
    dbg(config->frames_per_second, %lu);
    dbg(vector_size(config->images_v), %lu);
    dbg(r.ffmpeg_cmd, %s);
  }

  r.subprocess_result = subprocess_create(command_line, 0, &r.subprocess);
  assert_eq(r.subprocess_result, 0, %d);

  size_t bytes_read = 0;

  do {
    bytes_read = subprocess_read_stdout(&r.subprocess, r.stdout_buffer, READ_BUFFER_SIZE - 1);
    stringbuffer_append_string(r.STDOUT_BUFFER, r.stdout_buffer);
    r.stdout_bytes_read += bytes_read;
  } while (bytes_read != 0);
  bytes_read = 0;
  do {
    bytes_read = subprocess_read_stderr(&r.subprocess, r.stderr_buffer, READ_BUFFER_SIZE - 1);
    stringbuffer_append_string(r.STDERR_BUFFER, r.stderr_buffer);
    r.stderr_bytes_read += bytes_read;
  } while (bytes_read != 0);

  r.subprocess_join_result    = subprocess_join(&r.subprocess, &r.subprocess_exited);
  r.animated_gif_file_created = fsio_file_exists(config->animated_gif_file);

  r.READ_STDOUT = stringbuffer_to_string(r.STDOUT_BUFFER);
  r.READ_STDERR = stringbuffer_to_string(r.STDERR_BUFFER);

  stringbuffer_release(r.STDOUT_BUFFER);
  stringbuffer_release(r.STDERR_BUFFER);

  dbg(r.subprocess_result, %d);
  dbg(strlen(r.READ_STDOUT), %lu);
  dbg(strlen(r.READ_STDERR), %lu);
  dbg(r.READ_STDOUT, %s);
  dbg(r.READ_STDERR, %s);
  dbg(r.animated_gif_file_created, %d);

  assert_eq(r.subprocess_result, 0, %d);
  assert_eq(r.subprocess_exited, 0, %d);
  assert_eq(r.animated_gif_file_created, true, %d);

  free(r.READ_STDOUT);
  free(r.READ_STDERR);

  return(r.animated_gif_file_created);
} /* convert_images_to_animated_gif_ffmpeg */

#define RECORDED_FRAMES_QTY    vector_size(capture_config->recorded_frames_v)
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
  if (tempdir_path == NULL) {
    tempdir_path = gettempdir();
  }
  struct capture_config_t *capture_config = (struct capture_config_t *)CAPTURE_CONFIG;

  pthread_mutex_lock(capture_config_mutex);
  bool active = capture_config->active;

  if (execution_args.verbose) {
    printf("capturing max %d frames, %d seconds.......\n", capture_config->max_record_frames_qty, capture_config->max_record_duration_seconds);
  }
  pthread_mutex_unlock(capture_config_mutex);
  if (PROGRESS_ENABLED) {
    /*
     * progress_t  *progress = progress_new(capture_config->max_record_duration_seconds, PROGRESS_BAR_WIDTH);
     * {
     * progress->fmt         = "    Progress (:percent) => {:bar} [:elapsed]";
     * progress->bg_bar_char = BG_PROGRESS_BAR_CHAR;
     * progress->bar_char    = PROGRESS_BAR_CHAR;
     * progress_on(progress, PROGRESS_EVENT_START, db_progress_start);
     * progress_on(progress, PROGRESS_EVENT_PROGRESS, db_progress);
     * progress_on(progress, PROGRESS_EVENT_END, db_progress_end);
     * }
     */
  }
  int cur = 0;

  while (true) {
    pthread_mutex_lock(capture_config_mutex);
    if (capture_config->started_timestamp == 0) {
      capture_config->started_timestamp = timestamp();
    }
    active =
      (capture_config->active)
      && (RECORDED_FRAMES_QTY <= capture_config->max_record_frames_qty)
      && ((size_t)(get_recorded_duration_ms(capture_config) / 1000) < ((size_t)capture_config->max_record_duration_seconds));
    pthread_mutex_unlock(capture_config_mutex);
    if (!active) {
      if (execution_args.verbose) {
        printf("active changed to false.....\n");
      }
      break;
    }
    size_t sleep_ms = get_ms_until_next_frame(capture_config);

    if (execution_args.verbose) {
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
    //MEASURE(measurement_capture_to_file_image)

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
    case RESIZE_NONE:
    default:
      ok = capture_to_file_image(capture_config->window_id, SCREENSHOT_FILE);
      break;
    }
    //END_MEASURE(measurement_capture_to_file_image)
    //MEASURE_SUMMARY(measurement_capture_to_file_image);
    assert(ok == true);
    struct recorded_frame_t *f = malloc(sizeof(struct recorded_frame_t));
    f->timestamp  = timestamp();
    f->record_dur = f->timestamp - ts;
    f->file       = SCREENSHOT_FILE;

    pthread_mutex_lock(capture_config_mutex);
    vector_push(capture_config->recorded_frames_v, f);
    if (PROGRESS_ENABLED) {
      //progress_value(progress, cur + 1);
      cur++;
    }
    pthread_mutex_unlock(capture_config_mutex);
  }
  if (execution_args.verbose) {
    printf("do capture done\n");
  }
  if (PROGRESS_ENABLED) {
    //progress_value(progress, capture_config->max_record_duration_seconds);
    //progress_free(progress);
  }
  chan_send(done_chan, (void *)NULL);
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

  chan_send(done_chan, (void *)NULL);
} /* wait_for_control_d */


bool read_captured_frames(struct capture_config_t *capture_config) {
  if (tempdir_path == NULL) {
    tempdir_path = gettempdir();
  }
  struct Vector *images_v = vector_new();
  char          *animated_gif_file;

  asprintf(&animated_gif_file, "%s/window-%d-animation.gif", tempdir_path, capture_config->window_id);
  for (size_t i = 0; i < vector_size(capture_config->recorded_frames_v); i++) {
    struct recorded_frame_t *f = vector_get(capture_config->recorded_frames_v, i);
    f->file_size = fsio_file_size(f->file);
    fprintf(stdout, AC_RESETALL AC_BLUE " - #%.5lu @%" PRIu64 " [%s] <%s> dur:%lu\n",
            i,
            f->timestamp,
            bytes_to_string(fsio_file_size(f->file)),
            f->file,
            f->record_dur
            );

    vector_push(images_v, (char *)f->file);
  }
  if (fsio_file_exists(animated_gif_file)) {
    fsio_remove(animated_gif_file);
  }
  assert(fsio_file_exists(animated_gif_file) == false);
  struct animated_gif_conversion_config_t *animated_gif_conversion_config = malloc(sizeof(struct animated_gif_conversion_config_t));

  animated_gif_conversion_config->frames_per_second = capture_config->frames_per_second;
  animated_gif_conversion_config->animated_gif_file = animated_gif_file;
  animated_gif_conversion_config->images_v          = images_v;
  animated_gif_conversion_config->window_id         = capture_config->window_id;
  animated_gif_conversion_config->ffmpeg_log_level  = "warning";
  animated_gif_conversion_config->debug_mode        = execution_args.verbose;
  convert_images_to_animated_gif_ffmpeg(animated_gif_conversion_config);
  free(animated_gif_conversion_config);
  assert(fsio_file_exists(animated_gif_file) == true);
  return(true);
}


int capture_window(void *ARGS) {
  execution_args = *(struct args_t *)ARGS;
  struct Vector *window_ids = get_windows_ids();
  done_chan = chan_init(0);

  capture_config_mutex = malloc(sizeof(pthread_mutex_t));

  pthread_mutex_init(capture_config_mutex, NULL);
  struct capture_config_t *capture_config = malloc(sizeof(struct capture_config_t));

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

  chan_recv(done_chan, (void *)NULL);
  chan_dispose(done_chan);


  if (execution_args.verbose) {
    fprintf(stderr, AC_RESETALL AC_GREEN "Capture Stopped\n");
  }
  pthread_mutex_lock(capture_config_mutex);

  capture_config->active = false;
  pthread_mutex_unlock(capture_config_mutex);
  pthread_join(&capture_thread, NULL);
  if (execution_args.verbose) {
    fprintf(stderr, AC_RESETALL AC_GREEN "Captured %lu frames\n", vector_size(capture_config->recorded_frames_v));
  }

  int ok = read_captured_frames(capture_config);
  assert(ok == true);

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
              "Match Cmp"
              "Name Match"

              );

  struct Vector *windows = get_windows();

  for (size_t i = 0; i < vector_size(windows); i++) {
    window_t *w         = (window_t *)vector_get(windows, i);
    int      NAME_MATCH = wildcardcmp(execution_args.application_name_glob, strip_non_ascii(w->app_name));
    if (NAME_MATCH == 0) {
      continue;
    }
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
