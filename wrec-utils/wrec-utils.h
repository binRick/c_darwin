#pragma once
//////////////////////////////////////////
#include "wrec-common/wrec-common.h"
//////////////////////////////////////////
struct recorded_frame_t *get_first_recorded_frame(struct capture_config_t *capture_config);
struct recorded_frame_t *get_latest_recorded_frame(struct capture_config_t *capture_config);
size_t get_ms_since_last_recorded_frame(struct capture_config_t *capture_config);
size_t get_next_frame_interval_ms(struct capture_config_t *capture_config);
size_t get_next_frame_timestamp(struct capture_config_t *capture_config);
size_t get_ms_until_next_frame(struct capture_config_t *capture_config);
size_t get_recorded_duration_ms(struct capture_config_t *capture_config);
size_t get_ms_until_next_frame(struct capture_config_t *capture_config);
