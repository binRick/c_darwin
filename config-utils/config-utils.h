#pragma once
#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H
//////////////////////////////////////
#include "parson/parson.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
JSON_Value *get_json_config();
char *get_config_response_body();
char *get_config_json_string();
char *get_config_json_pretty_string();
//////////////////////////////////////
#endif
