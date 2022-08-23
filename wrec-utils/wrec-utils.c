#pragma once
#include <termios.h>
/////////////////////////////////////
#include "wrec-common/wrec-common.h"
#include "wrec-capture/wrec-capture.h"
/////////////////////////////////////
#include "chan/src/chan.h"
#include "chan/src/queue.h"
#include "libterminput/libterminput.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "tempdir.c/tempdir.h"
////////////////////////////////////////////////////////////
extern const char *EXCLUDED_WINDOW_APP_NAMES[];
////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////

