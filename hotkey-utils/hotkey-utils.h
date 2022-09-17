#pragma once
#ifndef HOTKEY_UTILS_H
#define HOTKEY_UTILS_H
//////////////////////////////////////
#include "libcyaml/include/cyaml/cyaml.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define HOTKEYS_CONFIG_FILE_NAME    "config.yaml"
enum action_type_t {
  ACTION_TYPE_NONE,
  ACTION_TYPE_ACTIVATE_APPLICATION,
  ACTION_TYPE_DEACTIVATE_APPLICATION,
  ACTION_TYPE_MINIMIZE_APPLICATION,
  ACTION_TYPE_FULLSCREEN_APPLICATION,
  ACTION_TYPE_TOP_FIFTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_BOTTOM_FIFTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_LEFT_FIFTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_RIGHT_FIFTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_LEFT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_RIGHT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_LEFT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_RIGHT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT,
  ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT,
  ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT,
  ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT,
  ACTION_TYPES_QTY,
};
struct action_type_handler_t {
  int (*fxn)(void *);
};
struct key_t {
  const char         *name, *key, *action;
  bool               enabled;
  enum action_type_t action_type;
};
struct hotkeys_config_t {
  const char   *name;
  struct key_t *keys;
  size_t       keys_count;
};
//////////////////////////////////////
static const cyaml_strval_t action_type_strings[] = {
  { "None",                                       ACTION_TYPE_NONE                                            },
  { "ActivateApplication",                        ACTION_TYPE_ACTIVATE_APPLICATION                            },
  { "DeactivateApplication",                      ACTION_TYPE_DEACTIVATE_APPLICATION                          },
  { "MinimizeApplication",                        ACTION_TYPE_MINIMIZE_APPLICATION,                           },
  { "FullscreenApplication",                      ACTION_TYPE_FULLSCREEN_APPLICATION                          },
  { "TopFiftyPercentFocusedApplication",          ACTION_TYPE_TOP_FIFTY_PERCENT_FOCUSED_APPLICATION           },
  { "BottomFiftyPercentFocusedApplication",       ACTION_TYPE_BOTTOM_FIFTY_PERCENT_FOCUSED_APPLICATION        },
  { "LeftFiftyPercentFocusedApplication",         ACTION_TYPE_LEFT_FIFTY_PERCENT_FOCUSED_APPLICATION          },
  { "RightFiftyPercentFocusedApplication",        ACTION_TYPE_RIGHT_FIFTY_PERCENT_FOCUSED_APPLICATION         },
  { "LeftSeventyFivePercentFocusedApplication",   ACTION_TYPE_LEFT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION   },
  { "RightSeventyFivePercentFocusedApplication",  ACTION_TYPE_RIGHT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION  },
  { "LeftTwentyFivePercentFocusedApplication",    ACTION_TYPE_LEFT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION    },
  { "RightTwentyFivePercentFocusedApplication",   ACTION_TYPE_RIGHT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION   },
  { "IncreaseFocusedApplicationWidthTenPercent",  ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT  },
  { "DecreaseFocusedApplicationWidthTenPercent",  ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT  },
  { "IncreaseFocusedApplicationHeightTenPercent", ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT },
  { "DecreaseFocusedApplicationHeightTenPercent", ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT },
  { "None",                                       ACTION_TYPES_QTY                                            },
};
//////////////////////////////////////
char *get_yaml_config_file_path(char **argv);
char *get_homedir_yaml_config_file_path(void);
struct hotkeys_config_t *load_yaml_config_file_path(char *config_file_path);
int minimize_application(void *APPLICATION_NAME);
int activate_application(void *APPLICATION_NAME);
int fullscreen_application(void *APPLICATION_NAME);
int deactivate_application(void *APPLICATION_NAME);
int increase_focused_application_width_ten_percent(void);
int decrease_focused_application_width_ten_percent(void);
int increase_focused_application_height_ten_percent(void);
int decrease_focused_application_height_ten_percent(void);
int right_percent_focused_application(float right_factor);
int left_percent_focused_application(float left_factor);
int top_fifty_percent_focused_application(void);
int bottom_fifty_percent_focused_application(void);
int right_fifty_percent_focused_application(void);
int left_fifty_percent_focused_application(void);
int right_twenty_five_percent_focused_application(void);
int left_twenty_five_percent_focused_application(void);
int right_seventy_five_percent_focused_application(void);
int left_seventy_five_percent_focused_application(void);
int handle_action(enum action_type_t action_type, void *action);
struct key_t *get_hotkey_config_key(struct hotkeys_config_t *cfg, char *key);
int execute_hotkey_config_key(struct key_t *key);
int bottom_percent_focused_application(float bottom_factor);
int top_percent_focused_application(float top_factor);
//////////////////////////////////////
static struct action_type_handler_t action_type_handlers[] = {
  [ACTION_TYPE_ACTIVATE_APPLICATION]                            = { .fxn = activate_application,                            },
  [ACTION_TYPE_DEACTIVATE_APPLICATION]                          = { .fxn = deactivate_application,                          },
  [ACTION_TYPE_MINIMIZE_APPLICATION]                            = { .fxn = minimize_application,                            },
  [ACTION_TYPE_FULLSCREEN_APPLICATION]                          = { .fxn = fullscreen_application,                          },
  [ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT]  = { .fxn = increase_focused_application_width_ten_percent,  },
  [ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT]  = { .fxn = decrease_focused_application_width_ten_percent,  },
  [ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT] = { .fxn = increase_focused_application_height_ten_percent, },
  [ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT] = { .fxn = decrease_focused_application_height_ten_percent, },
  [ACTION_TYPE_RIGHT_FIFTY_PERCENT_FOCUSED_APPLICATION]         = { .fxn = right_fifty_percent_focused_application,         },
  [ACTION_TYPE_LEFT_FIFTY_PERCENT_FOCUSED_APPLICATION]          = { .fxn = left_fifty_percent_focused_application,          },
  [ACTION_TYPE_RIGHT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION]   = { .fxn = right_twenty_five_percent_focused_application,   },
  [ACTION_TYPE_LEFT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION]    = { .fxn = left_twenty_five_percent_focused_application,    },
  [ACTION_TYPE_RIGHT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION]  = { .fxn = right_seventy_five_percent_focused_application,  },
  [ACTION_TYPE_LEFT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION]   = { .fxn = left_seventy_five_percent_focused_application,   },
  [ACTION_TYPE_TOP_FIFTY_PERCENT_FOCUSED_APPLICATION]           = { .fxn = top_fifty_percent_focused_application,           },
  [ACTION_TYPE_BOTTOM_FIFTY_PERCENT_FOCUSED_APPLICATION]        = { .fxn = bottom_fifty_percent_focused_application,        },
};
//////////////////////////////////////
#endif
