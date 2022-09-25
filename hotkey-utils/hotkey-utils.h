#pragma once
#ifndef HOTKEY_UTILS_H
#define HOTKEY_UTILS_H
//////////////////////////////////////
#include "libcyaml/include/cyaml/cyaml.h"
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
enum action_type_t {
  ACTION_TYPE_NONE,
  ////////////////////////////////////////////////////////
  ACTION_TYPE_ACTIVATE_APPLICATION,
  ACTION_TYPE_DEACTIVATE_APPLICATION,
  ACTION_TYPE_MINIMIZE_APPLICATION,
  ACTION_TYPE_FULLSCREEN_APPLICATION,
  ////////////////////////////////////////////////////////
  ACTION_TYPE_TOP_FOURTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_BOTTOM_FOURTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_TOP_FIFTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_BOTTOM_FIFTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_TOP_SIXTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_BOTTOM_SIXTY_PERCENT_FOCUSED_APPLICATION,
  ////////////////////////////////////////////////////////
  ACTION_TYPE_LEFT_FIFTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_RIGHT_FIFTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_LEFT_FOURTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_RIGHT_FOURTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_LEFT_SIXTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_RIGHT_SIXTY_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_LEFT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_RIGHT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_LEFT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION,
  ACTION_TYPE_RIGHT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION,
  ////////////////////////////////////////////////////////
  ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_FIVE_PERCENT,
  ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_FIVE_PERCENT,
  ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT,
  ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT,
  ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_FIVE_PERCENT,
  ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_FIVE_PERCENT,
  ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT,
  ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT,
  ////////////////////////////////////////////////////////
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
  const char   *name, *todo_app;
  const int    todo_width;
  struct key_t *keys;
  size_t       keys_count;
};
//////////////////////////////////////
static const cyaml_strval_t action_type_strings[] = {
  { "None",                                        ACTION_TYPE_NONE                                             },
  { "ActivateApplication",                         ACTION_TYPE_ACTIVATE_APPLICATION                             },
  { "DeactivateApplication",                       ACTION_TYPE_DEACTIVATE_APPLICATION                           },
  { "MinimizeApplication",                         ACTION_TYPE_MINIMIZE_APPLICATION,                            },
  { "FullscreenApplication",                       ACTION_TYPE_FULLSCREEN_APPLICATION                           },
  { "TopFiftyPercentFocusedApplication",           ACTION_TYPE_TOP_FIFTY_PERCENT_FOCUSED_APPLICATION            },
  { "BottomFiftyPercentFocusedApplication",        ACTION_TYPE_BOTTOM_FIFTY_PERCENT_FOCUSED_APPLICATION         },
  { "LeftFiftyPercentFocusedApplication",          ACTION_TYPE_LEFT_FIFTY_PERCENT_FOCUSED_APPLICATION           },
  { "RightFiftyPercentFocusedApplication",         ACTION_TYPE_RIGHT_FIFTY_PERCENT_FOCUSED_APPLICATION          },
  { "LeftFourtyPercentFocusedApplication",         ACTION_TYPE_LEFT_FOURTY_PERCENT_FOCUSED_APPLICATION          },
  { "RightFourtyPercentFocusedApplication",        ACTION_TYPE_RIGHT_FOURTY_PERCENT_FOCUSED_APPLICATION         },
  { "LeftSixtyPercentFocusedApplication",          ACTION_TYPE_LEFT_SIXTY_PERCENT_FOCUSED_APPLICATION           },
  { "RightSixtyPercentFocusedApplication",         ACTION_TYPE_RIGHT_SIXTY_PERCENT_FOCUSED_APPLICATION          },
  { "LeftSeventyFivePercentFocusedApplication",    ACTION_TYPE_LEFT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION    },
  { "RightSeventyFivePercentFocusedApplication",   ACTION_TYPE_RIGHT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION   },
  { "LeftTwentyFivePercentFocusedApplication",     ACTION_TYPE_LEFT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION     },
  { "RightTwentyFivePercentFocusedApplication",    ACTION_TYPE_RIGHT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION    },
  { "IncreaseFocusedApplicationWidthTenPercent",   ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT   },
  { "DecreaseFocusedApplicationWidthTenPercent",   ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT   },
  { "IncreaseFocusedApplicationHeightTenPercent",  ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT  },
  { "DecreaseFocusedApplicationHeightTenPercent",  ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT  },
  { "IncreaseFocusedApplicationWidthFivePercent",  ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_FIVE_PERCENT  },
  { "DecreaseFocusedApplicationWidthFivePercent",  ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_FIVE_PERCENT  },
  { "IncreaseFocusedApplicationHeightFivePercent", ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_FIVE_PERCENT },
  { "DecreaseFocusedApplicationHeightFivePercent", ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_FIVE_PERCENT },
  { "None",                                        ACTION_TYPES_QTY                                             },
};
#define GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(DIRECTION, FACTOR_TEXT, DIRECTION_LABEL, PERCENTAGE) \
  [ACTION_TYPE_ ## DIRECTION ## _ ## FACTOR_TEXT ## _PERCENT_FOCUSED_APPLICATION] = "Resize Focused Application ##DIRECTION_LABEL## by ##PERCENTAGE##%%"

////////////////////////////////////////////////////////////
static const char __attribute__((unused)) * action_type_descriptions[ACTION_TYPES_QTY + 1] = {
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(TOP,                                                            FIFTY,        Top,    50),
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(BOTTOM,                                                         FIFTY,        Bottom, 50),
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(LEFT,                                                           TWENTY_FIVE,  Left,   25),
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(RIGHT,                                                          TWENTY_FIVE,  Right,  25),
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(LEFT,                                                           FOURTY,       Left,   40),
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(RIGHT,                                                          FOURTY,       Right,  40),
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(LEFT,                                                           FIFTY,        Left,   50),
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(RIGHT,                                                          FIFTY,        Right,  50),
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(LEFT,                                                           SIXTY,        Left,   60),
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(RIGHT,                                                          SIXTY,        Right,  60),
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(LEFT,                                                           SEVENTY_FIVE, Left,   75),
  GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION(RIGHT,                                                          SEVENTY_FIVE, Right,  75),
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  [ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_FIVE_PERCENT] = "Increase Focused Application Width by 5%%",
  [ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_FIVE_PERCENT] = "Decrease Focused Application Width by 5%%",
  [ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT]  = "Increase Focused Application Width by 10%%",
  [ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT]  = "Decrease Focused Application Width by 10%%",
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  [ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_FIVE_PERCENT] = "Increase Focused Application Height by 5%%",
  [ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_FIVE_PERCENT] = "Decrease Focused Application Height by 5%%",
  [ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT]  = "Increase Focused Application Height by 10%%",
  [ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT]  = "Decrease Focused Application Height by 10%%",
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
};
#undef GENERATE_ACTION_TYPE_RESIZE_PERCENT_DESCRIPTION
////////////////////////////////////////////////////////////
//////////////////////////////////////
char *get_yaml_config_file_path(char **argv);
char *get_homedir_yaml_config_file_path(void);
struct hotkeys_config_t *load_yaml_config_file_path(char *config_file_path);
size_t get_config_file_hash(char *CONFIG_FILE_PATH);
int minimize_application(void *APPLICATION_NAME);
int activate_application(void *APPLICATION_NAME);
int fullscreen_application(void *APPLICATION_NAME);
int deactivate_application(void *APPLICATION_NAME);
////////////////////////////////////////////////////////////
#define GENERATE_RESIZE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(DIRECTION, TYPE, FACTOR_TEXT) \
  int DIRECTION ## _focused_application_ ## TYPE ## _ ## FACTOR_TEXT ## _percent(void);
////////////////////////////////////////////////////////////
GENERATE_RESIZE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(increase, width, five);
GENERATE_RESIZE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(decrease, width, five);
GENERATE_RESIZE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(increase, height, five);
GENERATE_RESIZE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(decrease, height, five);
GENERATE_RESIZE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(increase, width, ten);
GENERATE_RESIZE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(decrease, width, ten);
GENERATE_RESIZE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(increase, height, ten);
GENERATE_RESIZE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(decrease, height, ten);
////////////////////////////////////////////////////////////
#undef GENERATE_RESIZE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE
////////////////////////////////////////////////////////////
int right_percent_focused_application(float right_factor);
int left_percent_focused_application(float left_factor);
int bottom_percent_focused_application(float bottom_factor);
int top_percent_focused_application(float top_factor);
////////////////////////////////////////////////////////////
#define GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(DIRECTION, FACTOR_TEXT) \
  int DIRECTION ## _ ## FACTOR_TEXT ## _percent_focused_application(void);
//////////////////////////////////////////////////////////////////////////////////
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(left, twenty_five);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(right, twenty_five);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(left, fourty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(right, fourty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(left, fifty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(right, fifty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(left, sixty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(right, sixty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(left, seventy_five);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(right, seventy_five);
//////////////////////////////////////////////////////////////////////////////////
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(top, fourty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(bottom, fourty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(top, fifty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(bottom, fifty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(top, sixty);
GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE(bottom, sixty);
//////////////////////////////////////////////////////////////////////////////////
#undef GENERATE_PERCENT_FOCUSED_APPLICATION_PROTOTYPE
////////////////////////////////////////////////////////////
int handle_action(enum action_type_t action_type, void *action);
struct key_t *get_hotkey_config_key(struct hotkeys_config_t *cfg, char *key);
int execute_hotkey_config_key(struct key_t *key);
bool disable_hotkey_config_key(struct key_t *key);
bool enable_hotkey_config_key(struct key_t *key);
//////////////////////////////////////
static struct action_type_handler_t __attribute__((unused)) action_type_handlers[] = {
////////////////////////////////////////////////////////////
  [ACTION_TYPE_ACTIVATE_APPLICATION]   =                           { .fxn = activate_application,                             },
  [ACTION_TYPE_DEACTIVATE_APPLICATION] =                           { .fxn = deactivate_application,                           },
////////////////////////////////////////////////////////////
  [ACTION_TYPE_MINIMIZE_APPLICATION]   =                           { .fxn = minimize_application,                             },
  [ACTION_TYPE_FULLSCREEN_APPLICATION] =                           { .fxn = fullscreen_application,                           },
////////////////////////////////////////////////////////////
  [ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_FIVE_PERCENT]  = { .fxn = increase_focused_application_width_five_percent,  },
  [ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_FIVE_PERCENT]  = { .fxn = decrease_focused_application_width_five_percent,  },
  [ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_FIVE_PERCENT] = { .fxn = increase_focused_application_height_five_percent, },
  [ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_FIVE_PERCENT] = { .fxn = decrease_focused_application_height_five_percent, },
  [ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT]   = { .fxn = increase_focused_application_width_ten_percent,   },
  [ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_WIDTH_TEN_PERCENT]   = { .fxn = decrease_focused_application_width_ten_percent,   },
  [ACTION_TYPE_INCREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT]  = { .fxn = increase_focused_application_height_ten_percent,  },
  [ACTION_TYPE_DECREASE_FOCUSED_APPLICATION_HEIGHT_TEN_PERCENT]  = { .fxn = decrease_focused_application_height_ten_percent,  },
////////////////////////////////////////////////////////////
  [ACTION_TYPE_RIGHT_FOURTY_PERCENT_FOCUSED_APPLICATION]       =   { .fxn = right_fourty_percent_focused_application,         },
  [ACTION_TYPE_LEFT_FOURTY_PERCENT_FOCUSED_APPLICATION]        =   { .fxn = left_fourty_percent_focused_application,          },
  [ACTION_TYPE_RIGHT_FIFTY_PERCENT_FOCUSED_APPLICATION]        =   { .fxn = right_fifty_percent_focused_application,          },
  [ACTION_TYPE_LEFT_FIFTY_PERCENT_FOCUSED_APPLICATION]         =   { .fxn = left_fifty_percent_focused_application,           },
  [ACTION_TYPE_RIGHT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION]  =   { .fxn = right_twenty_five_percent_focused_application,    },
  [ACTION_TYPE_LEFT_TWENTY_FIVE_PERCENT_FOCUSED_APPLICATION]   =   { .fxn = left_twenty_five_percent_focused_application,     },
  [ACTION_TYPE_RIGHT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION] =   { .fxn = right_seventy_five_percent_focused_application,   },
  [ACTION_TYPE_LEFT_SEVENTY_FIVE_PERCENT_FOCUSED_APPLICATION]  =   { .fxn = left_seventy_five_percent_focused_application,    },
////////////////////////////////////////////////////////////
  [ACTION_TYPE_TOP_FOURTY_PERCENT_FOCUSED_APPLICATION]    =        { .fxn = top_fourty_percent_focused_application,           },
  [ACTION_TYPE_BOTTOM_FOURTY_PERCENT_FOCUSED_APPLICATION] =        { .fxn = bottom_fourty_percent_focused_application,        },
  [ACTION_TYPE_TOP_FIFTY_PERCENT_FOCUSED_APPLICATION]     =        { .fxn = top_fifty_percent_focused_application,            },
  [ACTION_TYPE_BOTTOM_FIFTY_PERCENT_FOCUSED_APPLICATION]  =        { .fxn = bottom_fifty_percent_focused_application,         },
  [ACTION_TYPE_TOP_SIXTY_PERCENT_FOCUSED_APPLICATION]     =        { .fxn = top_sixty_percent_focused_application,            },
  [ACTION_TYPE_BOTTOM_SIXTY_PERCENT_FOCUSED_APPLICATION]  =        { .fxn = bottom_sixty_percent_focused_application,         },
////////////////////////////////////////////////////////////
};
char *get_hotkey_type_action_name(enum action_type_t action);
struct Vector *get_config_keys_v();
#endif
