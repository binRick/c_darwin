#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <stdbool.h>
/////////////////////////////
CGImageRef capture_window_id_cgimageref(const int WINDOW_ID);
int capture_window();
bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME);
unsigned char *capture_to_file_image_data(const int WINDOW_ID);
bool capture_to_file_image_resize_factor(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_FACTOR);
bool capture_to_file_image_resize_width(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_WIDTH);
bool capture_to_file_image_resize_height(const int WINDOW_ID, const char *FILE_NAME, int RESIZE_HEIGHT);
/////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////
struct capture_result_t *request_window_capture(struct capture_request_t *capture_request);
int read_captured_frames(struct capture_config_t *capture_config);
////////////////////////////////////////////////////////////////////////////////////////////////
