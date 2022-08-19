#pragma once
///////////////////////////////////////////
#include "ls-proc.h"
///////////////////////////////////////////
#define OUTPUT_MODE_ID_NAME(OUTPUT_MODE_ID)    (output_modes[OUTPUT_MODE_ID].name)
#define FLATTEN_VECTOR(VECTOR)                 ((VECTOR == NULL) \
    ? ""                                                         \
    : (vector_size(VECTOR) == 0)                                 \
      ? ""                                                       \
      : str_flatten(vector_to_array(VECTOR), 0, vector_size(VECTOR)))

///////////////////////////////////////////
enum output_field_t {
  OUTPUT_FIELD_PID,
  OUTPUT_FIELD_PPIDS,
  OUTPUT_FIELD_CWD,
  OUTPUT_FIELD_CMDLINE,
  OUTPUT_FIELD_ENV_KEY,
  OUTPUT_FIELD_ENV_VAL,
  OUTPUT_FIELD_ENV,
  OUTPUT_FIELDS_QTY,
};

enum output_mode_t {
  OUTPUT_MODE_TEXT,
  OUTPUT_MODE_JSON,
  OUTPUT_MODES_QTY,
};

struct output_field_name_t {
  char *name;
};

struct output_mode_type_name_t {
  char *name;
};

struct ctx_t {
  int                (*mode)(void);
  bool               verbose;
  enum output_mode_t output_mode;
  struct Vector      *env_keys, *env_vals, *cwds, *pids, *cmd_lines, *ppids, *output_fields;
};

struct ctx_t ctx;
///////////////////////////////////////////
int parse_args(int argc, char *argv[]);
int get_output_name_mode_id(char *OUTPUT_MODE_NAME);
