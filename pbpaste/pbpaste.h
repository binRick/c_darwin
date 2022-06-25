#pragma once
#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
//////////////////////////////////////
typedef struct {
  char     *raw, *b64;
  char     compressed[2048];
  size_t   raw_size, b64_size, compressed_size;
  int      compression_level;
  uint32_t hash, seed;
} clipboard_event_t;

char *pbpaste_exec(void);


clipboard_event_t encode_cliboard_event(char *CLIPBOARD_CONTENT);
void dump_cliboard_event(const clipboard_event_t *E);
