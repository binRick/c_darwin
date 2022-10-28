#include "dls/defines.c"
#include "dls/types.h"
struct normalized_argv_t {
  char          *mode, *executable;
  struct Vector *pre_mode_arg_v, *post_mode_arg_v, *arg_v;
};
struct args_t *args = &(struct args_t){
  .limit        = 10, .concurrency = 1,
  .verbose_mode = false, .debug_mode = false,
  //.progress_bar_mode = DEFAULT_PROGRESS_BAR_ENABLED,
  .display_mode = false, .all_mode = false, .write_images_mode = false,
  //.capture_mode[DEFAULT_CAPTURE_TYPE] = true,
  //.output_mode = DEFAULT_OUTPUT_MODE,
  .clear_screen = false,
  .space_id     = -1, .display_id = -1, .id = -1,
  //.capture_type                       = DEFAULT_CAPTURE_TYPE,
  .sort_key                           = NULL, .sort_direction = "asc",
  .current_space_only                 = false, .minimized_only = false,
  .application_name                   = NULL,
  .width_greater                      = -1, .width_less = -1, .height_greater = -1, .height_less = -1, .width = -1, .height = -1,
  .output_file                        = NULL,
  .font_name                          = NULL, .font_family = NULL, .font_style = NULL,
  .exact_match                        = false, .case_sensitive = false,
  .retries                            = 0,
  .duration_seconds                   = 10,
  .purge_write_directory_before_write = false,
  .write_directory                    = "/tmp",
  .formats_v                          = NULL, .format_ids_v = NULL, .icon_sizes_v = NULL,
  .pid                                = -1, .windowids = NULL,
  .format_names_qty                   = 0, .hide_columns_qty = 0,
};
