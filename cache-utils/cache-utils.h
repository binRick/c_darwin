#pragma once
#ifndef CACHE_UTILS_H
#define CACHE_UTILS_H
//////////////////////////////////////
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
char *cache_utils_get_item(char *ITEM_KEY, size_t CACHE_TTL);
int    cache_utils_set_item(char *ITEM_KEY, char *ITEM_CONTENT);
bool   cache_utils_exists(char *ITEM_KEY);
bool   cache_utils_is_expired(char *ITEM_KEY, int TTL);
size_t cache_utils_get_size(char *ITEM_KEY);
int cache_utils_get_age(char *ITEM_KEY);
size_t cache_utils_get_hash(char *STRING);
#endif
