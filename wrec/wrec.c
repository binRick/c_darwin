#pragma once
/////////////////////////////////////
#include <assert.h>
#include <gd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
/////////////////////////////////////
#include "wrec-cli/wrec-cli.h"
#include "wrec/wrec.h"
/////////////////////////////////////
#include "assertf/assertf.h"
#include "b64.c/b64.h"
#include "bytes/bytes.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "capture/capture.h"
#include "chan/src/chan.h"
#include "chan/src/queue.h"
#include "dbg/dbg.h"
#include "generic-print/print.h"
#include "libterminput/libterminput.h"
#include "progress.c/progress.h"
#include "rwimg/readimage.h"
#include "rwimg/writeimage.h"
#include "strdup/strdup.h"
#include "subprocess.h/subprocess.h"
#include "timestamp/timestamp.h"
#include "which/src/which.h"
#include "wrec-cli/wrec-cli.h"
#define PROGRESS_ENABLED      false
#define FFMPEG_LOOP_QTY       0
#define FFMPEG_CROP_HEIGHT    50

////////////////////////////////////////////////////////////
static void db_progress_start(progress_data_t *data);
static void db_progress(progress_data_t *data);
static void db_progress_end(progress_data_t *data);

////////////////////////////////////////////////////////////
struct args_t   execution_args;
pthread_mutex_t *capture_config_mutex;
pthread_t       capture_thread, wait_ctrl_d_thread;
chan_t          *done_chan;
////////////////////////////////////////////////////////////

#define STDOUT_READ_BUFFER_SIZE    1024 * 16
//#define FFMPEG_LOG_LEVEL "debug"
//#define FFMPEG_LOG_LEVEL "verbose"
//#define FFMPEG_LOG_LEVEL "info"
#define FFMPEG_LOG_LEVEL    "warning"


static bool convert_images_to_animated_gif_ffmpeg(struct Vector *images_v, char *ANIMATED_GIF_FILE, int FRAMES_PER_SECOND, int WINDOW_ID){
  char *cmd;
  char *images_args;

  asprintf(&images_args, "-i '/tmp/window-%d-%%d.png'", WINDOW_ID);
  char                 *PATH        = getenv("PATH");
  char                 *ffmpeg_path = (char *)which_path("ffmpeg", PATH);

  bool                 ANIMATED_GIF_FILE_CREATED = false;
  char                 *READ_STDOUT;
  char                 *READ_STDERR;
  int                  exited, result;
  struct subprocess_s  subprocess;
  char                 stdout_buffer[STDOUT_READ_BUFFER_SIZE] = { 0 };
  char                 stderr_buffer[STDOUT_READ_BUFFER_SIZE] = { 0 };
  struct  StringBuffer *SB                                    = stringbuffer_new_with_options(STDOUT_READ_BUFFER_SIZE, true);
  struct  StringBuffer *ERR                                   = stringbuffer_new_with_options(STDOUT_READ_BUFFER_SIZE, true);
  size_t               bytes_read                             = 0,
                       index                                  = 0,
                       err_bytes_read                         = 0;

  asprintf(&cmd, "env AV_LOG_FORCE_COLOR=1 '%s' -stats -y -f image2 -framerate '%d' %s -loglevel '%s' -vf 'crop=in_w:in_h-%d,smartblur=ls=-0.5' -loop '%d' '%s'",
           ffmpeg_path,
           FRAMES_PER_SECOND,
           images_args,
           FFMPEG_LOG_LEVEL,
           FFMPEG_CROP_HEIGHT,
           FFMPEG_LOOP_QTY,
           ANIMATED_GIF_FILE
           );
  const char *command_line[] = { "/bin/sh", "-c", cmd, NULL };

  dbg(images_args, %s);
  dbg(PATH, %s);
  dbg(ffmpeg_path, %s);
  dbg(ANIMATED_GIF_FILE, %s);
  dbg(FRAMES_PER_SECOND, %d);
  dbg(vector_size(images_v), %lu);
  dbg(cmd, %s);

  result = subprocess_create(command_line, 0, &subprocess);
  assert_eq(result, 0, %d);
  do {
    bytes_read = subprocess_read_stdout(&subprocess, stdout_buffer, STDOUT_READ_BUFFER_SIZE - 1);
    stringbuffer_append_string(SB, stdout_buffer);
    index += bytes_read;
  } while (bytes_read != 0);
  do {
    err_bytes_read = subprocess_read_stderr(&subprocess, stderr_buffer, STDOUT_READ_BUFFER_SIZE - 1);
    stringbuffer_append_string(ERR, stderr_buffer);
    index += err_bytes_read;
  } while (bytes_read != 0);

  result                    = subprocess_join(&subprocess, &exited);
  ANIMATED_GIF_FILE_CREATED = fsio_file_exists(ANIMATED_GIF_FILE);

  READ_STDOUT = stringbuffer_to_string(SB);
  READ_STDERR = stringbuffer_to_string(ERR);

  stringbuffer_release(SB);
  stringbuffer_release(ERR);

  dbg(exited, %d);
  dbg(command_line[2], %s);
  dbg(strlen(READ_STDOUT), %lu);
  dbg(strlen(READ_STDERR), %lu);
  dbg(READ_STDOUT, %s);
  dbg(READ_STDERR, %s);
  dbg(ANIMATED_GIF_FILE_CREATED, %d);


  assert_eq(result, 0, %d);
  assert_eq(exited, 0, %d);
  assert_eq(ANIMATED_GIF_FILE_CREATED, true, %d);

  return(ANIMATED_GIF_FILE_CREATED);
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


int do_gd(void){
  int        i;
  FILE       *out;

  gdImagePtr im;
  gdImagePtr prev = NULL;
  int        black;

  im = gdImageCreate(100, 100);
  if (!im) {
    fprintf(stderr, "can't create image");
    return(1);
  }

  out = fopen("/tmp/anim.gif", "wb");
  if (!out) {
    fprintf(stderr, "can't create file %s", "anim.gif");
    return(1);
  }

  gdImageColorAllocate(im, 255, 255, 255);   /* allocate white as side effect */
  gdImageGifAnimBegin(im, out, 1, -1);

  for (i = 0; i < 20; i++) {
    int r, g, b;
    im = gdImageCreate(100, 100);
    r  = rand() % 255;
    g  = rand() % 255;
    b  = rand() % 255;

    gdImageColorAllocate(im, 255, 255, 255);      /* allocate white as side effect */
    black = gdImageColorAllocate(im, r, g, b);
    printf("(%i, %i, %i)\n", r, g, b);
    gdImageFilledRectangle(im, rand() % 100, rand() % 100, rand() % 100, rand() % 100, black);
    gdImageGifAnimAdd(im, out, 1, 0, 0, 10, 1, prev);

    if (prev) {
      gdImageDestroy(prev);
    }
    prev = im;
  }

  gdImageGifAnimEnd(out);
  fclose(out);

  return(0);
} /* do_gd */


////////////////////////////////////////////////////////////
void do_capture(void *CAPTURE_CONFIG){
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
      && ((size_t)(get_recorded_duration_ms(capture_config) / 1000) <= ((size_t)capture_config->max_record_duration_seconds));
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
              get_recorded_duration_ms(capture_config) / 1000, (size_t)capture_config->max_record_duration_seconds,
              RECORDED_FRAMES_QTY, capture_config->max_record_frames_qty,
              get_ms_since_last_recorded_frame(capture_config),
              sleep_ms
              );
    }

    usleep(1000 * sleep_ms);

    char *SCREENSHOT_FILE;
    asprintf(&SCREENSHOT_FILE, "/tmp/window-%d-%lu.png", capture_config->window_id, RECORDED_FRAMES_QTY);
    bool ok;
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
    assert(ok == true);
    struct recorded_frame_t *f = malloc(sizeof(struct recorded_frame_t));
    f->timestamp = timestamp();
    f->file      = SCREENSHOT_FILE;

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

  printf("[wait for ctrl-d] sending done chan\n");
  chan_send(done_chan, (void *)NULL);
  printf("[wait for ctrl-d] sent done chan\n");
} /* wait_for_control_d */


bool read_captured_frames(struct capture_config_t *capture_config) {
  struct Vector *images_v = vector_new();
  char          *animated_gif_file;

  asprintf(&animated_gif_file, "/tmp/window-%d-animation.gif", capture_config->window_id);
  for (size_t i = 0; i < vector_size(capture_config->recorded_frames_v); i++) {
    struct recorded_frame_t *f = vector_get(capture_config->recorded_frames_v, i);
    fprintf(stdout, AC_RESETALL AC_BLUE " - #%.5lu @%" PRIu64 " [%s] <%s>\n",
            i,
            f->timestamp,
            bytes_to_string(fsio_file_size(f->file)),
            f->file
            );

    f->file_size = fsio_file_size(f->file);
    vector_push(images_v, (char *)f->file);
  }
  if (fsio_file_exists(animated_gif_file)) {
    fsio_remove(animated_gif_file);
  }
  assert(fsio_file_exists(animated_gif_file) == false);
  convert_images_to_animated_gif_ffmpeg(images_v, animated_gif_file, capture_config->frames_per_second, capture_config->window_id);
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
