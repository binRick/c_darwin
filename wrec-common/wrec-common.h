#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
///////////////////////////////////////
#define MINIMUM_PNG_FILE_SIZE    67
#define MINIMUM_PNG_WIDTH        10
#define MINIMUM_PNG_HEIGHT       10
///////////////////////////////////////
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
////////////////////////////////////////////
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_img/src/img.h"
#include "c_vector/vector/vector.h"
#include "libfort/lib/fort.h"
#include "string-utils/string-utils.h"
#include "submodules/tinydir/tinydir.h"
#include "tempdir.c/tempdir.h"
#include "window-utils/window-utils.h"
///////////////////////////////////////
struct loaded_png_file_t {
  int                       width, height, stb_format, stb_req_format;
  unsigned char             *pixels;
  char                      *path;
  size_t                    size;
  unsigned long             dur;
  struct image_dimensions_t image_dimensions;
};
struct file_time_t {
  long unsigned             file_creation_ts, stb_render_ms, started_ms, frame_ms;
  char                      *file_path;
  size_t                    file_size, pixels_qty, colors_qty;
  int                       stb_format, stb_depth, stb_pitch;
  unsigned char             *stb_pixels;
  struct image_dimensions_t *image_dimensions;
  struct stat               *file_info;
};
int compare_file_time_t(struct file_time_t *e1, struct file_time_t *e2);
struct file_times_t {
  struct Vector      *files_v;
  struct file_time_t **files;
  long unsigned      render_ms, started_ms, total_ms, dur_ms, avg_ms;
  int                max_width, max_height;
  struct file_time_t *sorted_images;
  size_t             sorted_images_qty, sorted_images_size, pixels_qty, colors_qty, animated_gif_file_size;
  FILE               *fp;
  int                stb_req_format;
  tinydir_file       *td_file;
  tinydir_dir        *td_dir;
  char               *animated_gif_file_name;
  bool               success, verbose_mode;
  int                msf_gif_alpha_threshold;
};
enum resize_type_t {
  RESIZE_BY_WIDTH,
  RESIZE_BY_HEIGHT,
  RESIZE_BY_FACTOR,
  RESIZE_BY_NONE,
  RESIZE_TYPES_QTY
} resize_type_t;
enum capture_mode_type_t {
  CAPTURE_REQUEST_MODE_WRITE_FILE_RETURN_RESULT,
  CAPTURE_REQUEST_MODE_WRITE_FILE_CALLBACK_RESULT,
  CAPTURE_REQUEST_MODE_RETURN_RESULT,
  CAPTURE_REQUEST_MODE_CALLBACK_RESULT,
  CAPTURE_REQUEST_MODES_QTY,
};
struct capture_result_data_t {
  enum capture_mode_type_t capture_mode;
  size_t                   captured_width;
  size_t                   captured_height;
  size_t                   resized_width;
  size_t                   resized_height;
  int                      colorspace, bytes_per_row, bits_per_pixel;
  uint8_t                  *pixels;
  char                     *file_path;
  size_t                   file_size;
  CGImageRef               captured_image_ref;
  CGImageRef               resized_image_ref;
};
struct capture_request_t {
  enum capture_mode_type_t mode;
  enum resize_type_t       resize_type;
  int                      window_id, resize_factor;
  char                     *file_path;
  bool                     debug_mode;
  void                     *(*callback_function)(struct capture_result_data_t *);
  size_t                   width, height;
};

struct capture_result_t {
  int                          captured_width, captured_height, resized_width, resized_height;
  unsigned long                request_received, capture_started, resize_started, write_file_started;
  unsigned long                capture_dur, write_file_dur, resize_dur, total_dur;
  bool                         save_file_success, success;
  CGSize                       window_size;
  CGRect                       window_bounds;
  CGRect                       captured_rect;
  struct capture_request_t     *request;
  struct capture_result_data_t *data;
};
struct modes_t {
  char *name;
  char *description;
  int  (*handler)(void *);
} modes_t;
struct args_t {
  char   *mode;
  bool   verbose;
  int    window_id;
  size_t max_recorded_frames;
  size_t max_record_duration_seconds;
  int    frames_per_second;
  bool   mode_capture;
  bool   mode_list;
  bool   mode_debug_args;
  int    resize_type;
  int    resize_value;
  char   *application_name_glob;
};
struct recorded_frame_t {
  int64_t timestamp;
  char    *file, *hash_enc;
  size_t  file_size, record_dur;
};
struct capture_config_t {
  bool          active;
  int           max_record_frames_qty, max_record_duration_seconds, frames_per_second, window_id;
  int64_t       started_timestamp, ended_timestamp;
  int           resize_type;
  int           resize_value;
  struct Vector *recorded_frames_v;
};
///////////////////////////////////
int compare_file_time_items(struct file_time_t *e1, struct file_time_t *e2);
void wrec_common1();
int read_captured_frames(struct capture_config_t *capture_config);
int list_windows(void *ARGS);
char *resize_type_name(const int RESIZE_TYPE);
struct capture_result_t *request_window_capture(struct capture_request_t *capture_request);
bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME);
void debug_resize(int WINDOW_ID, char *FILE_NAME, int RESIZE_TYPE, int RESIZE_VALUE, int ORIGINAL_WIDTH, int ORIGINAL_HEIGHT, long unsigned CAPTURE_DURATION_MS, long unsigned SAVE_DURATION_MS);
CGImageRef capture_window_id_cgimageref(const int WINDOW_ID);
int load_pngs_create_animated_gif(const char *PATH);
////////////////////////////////////////////
extern int CGSMainConnectionID(void);
extern CGError CGSGetConnectionPSN(int cid, ProcessSerialNumber *psn);
extern CGError CGSSetWindowAlpha(int cid, uint32_t wid, float alpha);
extern CGError CGSSetWindowListAlpha(int cid, const uint32_t *window_list, int window_count, float alpha, float duration)
;
extern CGError CGSSetWindowLevelForGroup(int cid, uint32_t wid, int level);
extern OSStatus CGSMoveWindowWithGroup(const int cid, const uint32_t wid, CGPoint *point);
extern CGError CGSReassociateWindowsSpacesByGeometry(int cid, CFArrayRef window_list);
extern CGError CGSGetWindowOwner(int cid, uint32_t wid, int *window_cid);
extern CGError CGSSetWindowTags(int cid, uint32_t wid, const int tags[2], size_t maxTagSize);
extern CGError CGSClearWindowTags(int cid, uint32_t wid, const int tags[2], size_t maxTagSize);
extern CGError CGSGetWindowBounds(int cid, uint32_t wid, CGRect *frame);
extern CGError CGSGetWindowTransform(int cid, uint32_t wid, CGAffineTransform *t);
extern CGError CGSSetWindowTransform(int cid, uint32_t wid, CGAffineTransform t);
extern void CGSManagedDisplaySetCurrentSpace(int cid, CFStringRef display_ref, uint64_t spid);
extern uint64_t CGSManagedDisplayGetCurrentSpace(int cid, CFStringRef display_ref);
extern CFArrayRef CGSCopyManagedDisplaySpaces(const int cid);
extern CFStringRef CGSCopyManagedDisplayForSpace(const int cid, uint64_t spid);
extern void CGSShowSpaces(int cid, CFArrayRef spaces);
extern void CGSHideSpaces(int cid, CFArrayRef spaces);
///////////////////////////////////
