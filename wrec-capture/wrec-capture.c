#pragma once
static void wait_for_control_d();
#include <termios.h>
///////////////////////////////////////////////////////
#include "c_string_buffer/include/stringbuffer.h"
#include "chan/src/chan.h"
#include "libterminput/libterminput.h"
#include "log.h/log.h"
#include "tempdir.c/tempdir.h"
#include "window-utils/window-utils.h"
#include "wrec-common/wrec-common.h"
#include "wrec-utils/wrec-utils.h"
///////////////////////////////////////////////////////
static bool            DEBUG_IMAGE_RESIZE         = false;
static bool            VERBOSE_DEBUG_IMAGE_RESIZE = false;
static struct args_t   execution_args;
static pthread_mutex_t *capture_config_mutex;
static pthread_t       capture_thread, wait_ctrl_d_thread;
static chan_t          *done_chan;
///////////////////////////////////////////////////////
#include "wrec-common/extern.h"
///////////////////////////////////////////////////////

CGImageRef capture_window_id_cgimageref(const int WINDOW_ID){
  assert(WINDOW_ID > 0);
  CFDictionaryRef W = window_id_to_window(WINDOW_ID);
  if (W == NULL) {
    fprintf(stderr, "Window #%d seems to have disappeared\n", WINDOW_ID);
    return(NULL);
  }
  int WID = (int)CFDictionaryGetInt(W, kCGWindowNumber);
  assert(WID > 0);
  assert(WID == WINDOW_ID);
  CGSize          W_SIZE = CGWindowGetSize(W);

  CGRect          frame = {};
  struct window_t *w    = get_window_id(WINDOW_ID);
  //log_info("Capturing window with pid %d", w->pid);

  // int window_connection = get_window_id_connection
  CGSGetWindowBounds(w->connection_id, WID, &frame);
  int capture_rect_x      = frame.origin.x;
  int capture_rect_y      = frame.origin.y;
  int capture_rect_width  = frame.size.width;     //(int)W_SIZE.width - frame.origin.x;
  int capture_rect_height = frame.size.height;    // (int)W_SIZE.height - frame.origin.y;
  if (true == DEBUG_WINDOWS) {
    log_debug(
      "\n\t|window size          :   %dx%d"
      "\n\t|frame  size          :   %dx%d"
      "\n\t|frame  origin        :   %dx%d"
      "\n\t|capture rect"
      "\n\t               x      :    %d"
      "\n\t               y      :    %d"
      "\n\t               width  :    %d"
      "\n\t               height :    %d"
      "\n",
      (int)W_SIZE.height, (int)W_SIZE.width,
      (int)frame.size.width, (int)frame.size.height,
      (int)frame.origin.x, (int)frame.origin.y,
      capture_rect_x, capture_rect_y,
      capture_rect_width, capture_rect_height
      );
  }

  CGImageRef img = CGWindowListCreateImage(CGRectMake(capture_rect_x, capture_rect_y, capture_rect_width, capture_rect_height),
                                           kCGWindowListOptionIncludingWindow,
                                           WID,
                                           kCGWindowImageBoundsIgnoreFraming | kCGWindowImageBestResolution
                                           );
  if (img == NULL) {
    return(NULL);
  }
  CGContextRelease(W);
  return(img);
} /* capture_window_id_cgimageref */

int read_captured_frames(struct capture_config_t *capture_config) {
  struct Vector *images_v = vector_new();

  for (size_t i = 0; i < vector_size(capture_config->recorded_frames_v); i++) {
    struct recorded_frame_t *f = vector_get(capture_config->recorded_frames_v, i);
    f->file_size = fsio_file_size(f->file);
    log_info(AC_RESETALL AC_BLUE " - #%.5lu @%" PRIu64 " [%s] <%s> dur:%lu\n",
             i,
             f->timestamp,
             bytes_to_string(fsio_file_size(f->file)),
             f->file,
             f->record_dur
             );
    vector_push(images_v, (char *)f->file);
  }
  return(0);
}

static void CGImageDumpInfo(CGImageRef image) {
  size_t width               = CGImageGetWidth(image),
         height              = CGImageGetHeight(image),
         bytesPerRow         = CGImageGetBytesPerRow(image),
         bitsPerPixel        = CGImageGetBitsPerPixel(image),
         bitsPerComponent    = CGImageGetBitsPerComponent(image),
         componentsPerPixel  = bitsPerPixel / bitsPerComponent;
  CGColorSpaceRef colorSpace = CGImageGetColorSpace(image);
  CGBitmapInfo    bitmapInfo = CGImageGetBitmapInfo(image);

  printf("==================================================================================================\n");
  printf("CGImageGetWidth: %lu\n", width);
  printf("CGImageGetHeight: %lu\n", height);
  printf("CGColorSpaceModel: %d\n", CGColorSpaceGetModel(colorSpace));
  printf("CGImageGetBytesPerRow: %lu\n", bytesPerRow);
  printf("CGImageGetBitsPerPixel: %lu\n", bitsPerPixel);
  printf("CGImageGetBitsPerComponent: %lu\n", bitsPerComponent);
  printf("components per pixel: %lu\n", componentsPerPixel);
  printf("CGImageGetBitmapInfo: 0x%.8X\n", (unsigned)bitmapInfo);
  printf("\t->kCGBitmapAlphaInfoMask = %s\n", (bitmapInfo & kCGBitmapAlphaInfoMask) ? "YES" : "NO");
  printf("\t->kCGBitmapFloatComponents = %s\n", (bitmapInfo & kCGBitmapFloatComponents) ? "YES" : "NO");
  printf("\t->kCGBitmapByteOrderMask = 0x%.8X\n", (bitmapInfo & kCGBitmapByteOrderMask));
  printf("\t->kCGBitmapByteOrderDefault = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrderDefault) ? "YES" : "NO");
  printf("\t->kCGBitmapByteOrder16Little = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder16Little) ? "YES" : "NO");
  printf("\t->kCGBitmapByteOrder32Little = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder32Little) ? "YES" : "NO");
  printf("\t->kCGBitmapByteOrder16Big = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder16Big) ? "YES" : "NO");
  printf("\t->kCGBitmapByteOrder32Big = %s\n",
         ((bitmapInfo & kCGBitmapByteOrderMask) == kCGBitmapByteOrder32Big) ? "YES" : "NO");
  printf("==================================================================================================\n");
} /* CGImageDumpInfo */

CGImageRef resize_cgimage(CGImageRef imageRef, int width, int height) {
  CGRect       newRect = CGRectIntegral(CGRectMake(0, 0, width, height));
  CGContextRef context = CGBitmapContextCreate(NULL, width, height,
                                               CGImageGetBitsPerComponent(imageRef),
                                               CGImageGetBytesPerRow(imageRef),
                                               CGImageGetColorSpace(imageRef),
                                               CGImageGetBitmapInfo(imageRef));

  CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
  CGContextDrawImage(context, newRect, imageRef);
  CGImageRef newImageRef = CGBitmapContextCreateImage(context);

  CGContextRelease(context);
  return(newImageRef);
}
///////////////////////////////////////////////////////
#define COMMON_RESIZE_AND_SAVE(RESIZE_TYPE, RESIZE_VALUE, RESIZE_HEIGHT) \
  resized_img  = resize_cgimage(img, RESIZE_WIDTH, RESIZE_HEIGHT);       \
  save_started = timestamp();                                            \
  bool ok = cgimage_save_png(resized_img, FILE_NAME);                    \
  save_dur = timestamp() - save_started;                                 \
  assert(ok == true);                                                    \
  if (true == DEBUG_IMAGE_RESIZE) {                                      \
    debug_resize(                                                        \
      WINDOW_ID,                                                         \
      FILE_NAME,                                                         \
      RESIZE_TYPE, RESIZE_VALUE,                                         \
      width, height,                                                     \
      capture_dur, save_dur                                              \
      );                                                                 \
  }                                                                      \
  if (true == VERBOSE_DEBUG_IMAGE_RESIZE) {                              \
    CGImageDumpInfo(resized_img);                                        \
  }

#define COMMON_CAPTURE()                                                                  \
  size_t width = 0, height = 0;                                                           \
  long unsigned capture_dur = -1, save_dur = -1, capture_started = -1, save_started = -1; \
  DEBUG_IMAGE_RESIZE         = (getenv("DEBUG") != NULL) ? true : false;                  \
  VERBOSE_DEBUG_IMAGE_RESIZE = (getenv("VERBOSE_DEBUG") != NULL) ? true : false;          \
  CGImageRef resized_img;                                                                 \
  capture_started = timestamp();                                                          \
  CGImageRef img = capture_window_id_cgimageref(WINDOW_ID);                               \
  if (img == NULL) {                                                                      \
    return(NULL);                                                                         \
  };                                                                                      \
  capture_dur = timestamp() - capture_started;                                            \
  assert(img != NULL);                                                                    \
  width  = CGImageGetWidth(img);                                                          \
  height = CGImageGetHeight(img);                                                         \
  assert(width > 0);                                                                      \
  assert(height > 0);
#define COMMON_RESIZE_RETURN() \
  return((fsio_file_exists(FILE_NAME) && fsio_file_size(FILE_NAME) > 64));

///////////////////////////////////////////////////////

static bool cgimage_save_png(const CGImageRef image, const char *filename) {
  bool success = false; CFStringRef path; CFURLRef url; CGImageDestinationRef destination;
  {
    path        = CFStringCreateWithCString(NULL, filename, kCFStringEncodingUTF8);
    url         = CFURLCreateWithFileSystemPath(NULL, path, kCFURLPOSIXPathStyle, 0);
    destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
    CGImageDestinationAddImage(destination, image, nil);
    success = CGImageDestinationFinalize(destination);
  }

  CFRelease(url); CFRelease(path); CFRelease(destination);
  return((success == true) && fsio_file_exists(filename));
}

struct capture_result_t *request_window_capture(struct capture_request_t *capture_request){
  struct capture_result_t *res = calloc(1, sizeof(struct capture_result_t));
  {
    res->data                = calloc(1, sizeof(struct capture_result_data_t));
    res->data->file_path     = NULL;
    res->request             = capture_request;
    res->success             = false;
    res->save_file_success   = false;
    res->capture_dur         = 0;
    res->resize_dur          = 0;
    res->write_file_dur      = 0;
    res->total_dur           = 0;
    res->request_received    = timestamp();
    res->request->debug_mode = (res->request->debug_mode == true)
                      ? true
                      : (getenv("DEBUG") != NULL)
                        ? true
                        : false;
  }

  {
    res->capture_started          = timestamp();
    res->data->captured_image_ref = capture_window_id_cgimageref(res->request->window_id);
    res->data->captured_width     = CGImageGetWidth(res->data->captured_image_ref);
    res->data->captured_height    = CGImageGetHeight(res->data->captured_image_ref);
    res->capture_dur              = timestamp() - res->capture_started;
  }
  {
    if (false) {
      res->data->resized_image_ref = resize_cgimage(
        res->data->captured_image_ref, res->data->resized_width, res->data->resized_height
        );
    }
    res->data->resized_width  = CGImageGetWidth(res->data->resized_image_ref);
    res->data->resized_height = CGImageGetWidth(res->data->resized_image_ref);
    res->resize_dur           = timestamp() - res->resize_started;
  }
  {
    switch (res->request->resize_type) {
    case RESIZE_BY_FACTOR:
      res->resize_started          = timestamp();
      res->request->width          = res->data->captured_width / res->request->resize_factor;
      res->request->height         = res->data->captured_height / res->request->resize_factor;
      res->data->resized_image_ref = resize_cgimage(res->data->captured_image_ref,
                                                    res->request->width, res->request->height
                                                    );
      res->data->resized_width  = CGImageGetWidth(res->data->resized_image_ref);
      res->data->resized_height = CGImageGetWidth(res->data->resized_image_ref);
      break;

    case RESIZE_BY_NONE:
      res->data->resized_image_ref = res->data->captured_image_ref;
    case RESIZE_BY_WIDTH:
    case RESIZE_BY_HEIGHT:
    default:
      goto RETURN_FAILURE;
      break;
    }
  }
  {
    switch (res->request->mode) {
    case CAPTURE_REQUEST_MODE_WRITE_FILE_RETURN_RESULT:
    case CAPTURE_REQUEST_MODE_WRITE_FILE_CALLBACK_RESULT:
      res->write_file_started = timestamp();
      res->save_file_success  = cgimage_save_png(res->data->resized_image_ref, res->request->file_path);
      res->write_file_dur     = timestamp() - res->write_file_started;
      if (res->save_file_success != true || fsio_file_exists(res->request->file_path) != true) {
        goto RETURN_FAILURE;
      }
      res->data->file_path = res->request->file_path;
      res->data->file_size = fsio_file_size(res->data->file_path);
      res->success         = true;
      if (res->request->mode == CAPTURE_REQUEST_MODE_CALLBACK_RESULT) {
        goto CALLBACK_RESULT;
      }
      break;

    case CAPTURE_REQUEST_MODE_RETURN_RESULT:
      if (1 != 1) {
        goto RETURN_FAILURE;
      }

      res->success = true;
      break;

    case CAPTURE_REQUEST_MODE_CALLBACK_RESULT:
      if (1 != 1) {
        goto RETURN_FAILURE;
      }
      res->success = true;
      break;

    default:
      goto RETURN_FAILURE;
      break;
    }
  }

  res->total_dur = timestamp() - res->request_received;
  return(res);

CALLBACK_RESULT:
  res->total_dur = res->request_received - timestamp();
  res->request->callback_function(res);
  return(res);

RETURN_FAILURE:
  res->success = false;
  return(res);
} /* request_capture */

bool capture_to_file_image_resize_factor(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_FACTOR){
  COMMON_CAPTURE()
  int RESIZE_WIDTH = width / RESIZE_FACTOR;
  int RESIZE_HEIGHT = height / RESIZE_FACTOR;
  COMMON_RESIZE_AND_SAVE(RESIZE_FACTOR, RESIZE_WIDTH, RESIZE_HEIGHT)
  COMMON_RESIZE_RETURN()
}

bool capture_to_file_image_resize_height(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_HEIGHT){
  COMMON_CAPTURE()
  int RESIZE_FACTOR = height / RESIZE_HEIGHT;
  int RESIZE_WIDTH = width / RESIZE_FACTOR;
  COMMON_RESIZE_AND_SAVE(RESIZE_HEIGHT, RESIZE_WIDTH, RESIZE_HEIGHT)
  COMMON_RESIZE_RETURN()
}

bool capture_to_file_image_resize_width(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_WIDTH){
  COMMON_CAPTURE()
  int RESIZE_FACTOR = width / RESIZE_WIDTH;
  int RESIZE_HEIGHT = height / RESIZE_FACTOR;
  COMMON_RESIZE_AND_SAVE(RESIZE_BY_WIDTH, RESIZE_WIDTH, RESIZE_HEIGHT)
  COMMON_RESIZE_RETURN()
}

bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME){
  COMMON_CAPTURE()
  int RESIZE_WIDTH = width;
  int RESIZE_HEIGHT = height;
  COMMON_RESIZE_AND_SAVE(RESIZE_BY_NONE, RESIZE_WIDTH, RESIZE_HEIGHT)
  COMMON_RESIZE_RETURN()
}

///////////////////////////////////////////////////////
void do_capture(void *CAPTURE_CONFIG){
  char                    *tempdir_path   = gettempdir();
  struct capture_config_t *capture_config = (struct capture_config_t *)CAPTURE_CONFIG;

  pthread_mutex_lock(capture_config_mutex);
  bool active = capture_config->active;

  if (execution_args.verbose == true) {
    log_info("capturing max %d frames, %d seconds.......", capture_config->max_record_frames_qty, capture_config->max_record_duration_seconds);
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
        log_info("active changed to false.....");
      }
      break;
    }
    size_t since    = get_ms_since_last_recorded_frame(capture_config);
    size_t sleep_ms = get_ms_until_next_frame(capture_config);
    sleep_ms = (since > 0) ? (sleep_ms - since) : sleep_ms;
    sleep_ms = (sleep_ms > 10000000) ? 0 : sleep_ms;
    sleep_ms = (size_t)(((float)(sleep_ms * 1000) * (float)(.98)) / 1000);

    if (execution_args.verbose == true) {
      log_info(AC_RESETALL AC_YELLOW AC_BOLD "    [Frame Capture]     latest frame ts: %" PRIu64 " (running for %lu/%lums) (%lu/%d frames recorded) (%lums since latest frame)- sleeping for %lums" AC_RESETALL "",
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
    log_info("Capture Completed");
  }
  chan_send(done_chan, (void *)NULL);
} /* do_capture */

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
    log_info(AC_RESETALL AC_GREEN "Capture Stopped");
  }
  pthread_mutex_lock(capture_config_mutex);

  capture_config->active = false;
  pthread_mutex_unlock(capture_config_mutex);
  pthread_join(&capture_thread, NULL);
  if (execution_args.verbose == true) {
    log_info(AC_RESETALL AC_GREEN "Captured %lu frames", vector_size(capture_config->recorded_frames_v));
  }

  return(read_captured_frames(capture_config));
} /* capture_window */

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

  log_info(AC_RESETALL AC_YELLOW "Control+d to stop capture" AC_RESETALL "");
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
        log_info("keypress:'%s'", stringbuffer_to_string(sb));
      }
      if (strcmp(stringbuffer_to_string(sb), "ctrl+D") == 0) {
        if (execution_args.verbose == true) {
          log_info("stopping........");
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

char *resize_type_name(const int RESIZE_TYPE){
  char *RESIZE_TYPE_NAME = "UNKNOWN";

  switch (RESIZE_TYPE) {
  case RESIZE_BY_WIDTH:  RESIZE_TYPE_NAME  = "WIDTH"; break;
  case RESIZE_BY_HEIGHT:  RESIZE_TYPE_NAME = "HEIGHT"; break;
  case RESIZE_BY_FACTOR:  RESIZE_TYPE_NAME = "FACTOR"; break;
  case RESIZE_BY_NONE:  RESIZE_TYPE_NAME   = "NONE"; break;
  }
  return(RESIZE_TYPE_NAME);
}
