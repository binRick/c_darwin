#pragma once
#include "c_string_buffer/include/stringbuffer.h"
#include "c_vector/vector/vector.h"
#include "ls-proc-args.h"
#include "str-flatten.c/src/str-flatten.h"

static struct output_field_name_t     output_fields[] = {
  [OUTPUT_FIELD_PID]     = { .name = "pid"     },
  [OUTPUT_FIELD_PPIDS]   = { .name = "ppids"   },
  [OUTPUT_FIELD_CWD]     = { .name = "cwd"     },
  [OUTPUT_FIELD_CMDLINE] = { .name = "cmdline" },
  [OUTPUT_FIELD_ENV_KEY] = { .name = "env_key" },
  [OUTPUT_FIELD_ENV_VAL] = { .name = "env_val" },
  [OUTPUT_FIELD_ENV]     = { .name = "env"     },
  [OUTPUT_FIELDS_QTY]    = { NULL },
};

static struct output_mode_type_name_t output_modes[] = {
  [OUTPUT_MODE_TEXT] = { .name = "text", },
  [OUTPUT_MODE_JSON] = { .name = "json", },
  [OUTPUT_MODES_QTY] = { NULL },
};

char *output_field_names(){
  struct StringBuffer            *sb  = stringbuffer_new();
  struct output_mode_type_name_t *tmp = output_fields;

  while (tmp != NULL && tmp->name != NULL) {
    stringbuffer_append_string(sb, tmp->name);
    tmp++;
    if (tmp->name != NULL)
      stringbuffer_append_string(sb, ", ");
  }

  char *s = stringbuffer_to_string(sb);

  stringbuffer_release(sb);
  return(s);
}

char *output_mode_type_names(){
  struct StringBuffer            *sb  = stringbuffer_new();
  struct output_mode_type_name_t *tmp = output_modes;

  while (tmp != NULL && tmp->name != NULL) {
    stringbuffer_append_string(sb, tmp->name);
    tmp++;
    if (tmp->name != NULL)
      stringbuffer_append_string(sb, ", ");
  }

  char *s = stringbuffer_to_string(sb);

  stringbuffer_release(sb);
  return(s);
}

int get_output_name_mode_id(char *OUTPUT_MODE_NAME){
  struct output_mode_type_name_t *tmp = output_modes;
  int                            i    = 0;

  while (tmp != NULL && tmp->name != NULL) {
    if (strcmp(OUTPUT_MODE_NAME, tmp->name) == 0)
      return(i);

    tmp++; i++;
  }
  return(0);
}

struct ctx_t ctx = {
  .verbose       = false,
  .mode          = NULL,
  .output_mode   = 0,
  .output_fields = NULL,
  .env_keys      = NULL,
  .env_vals      = NULL,
  .cwds          = NULL,
  .pids          = NULL,
  .ppids         = NULL,
  .cmd_lines     = NULL,
};

int mode_debug_args(void){
  fprintf(stderr, AC_RESETALL AC_YELLOW AC_BOLD "Debug Args\n" AC_RESETALL);
  fprintf(stderr, AC_RESETALL AC_GREEN          "\tDebug Mode: %s\n" AC_RESETALL,
          ctx.verbose ? "Yes" : "No"
          );
  fprintf(stderr, AC_RESETALL AC_BLUE           "\t%lu Env Keys: %s\n" AC_RESETALL,
          vector_size(ctx.env_keys),
          FLATTEN_VECTOR(ctx.env_keys)
          );
  fprintf(stderr, AC_RESETALL AC_CYAN           "\t%lu Env Vals: %s\n" AC_RESETALL,
          vector_size(ctx.env_vals),
          FLATTEN_VECTOR(ctx.env_vals)
          );
  fprintf(stderr, AC_RESETALL AC_GREEN           "\t%lu PPIDs: %s\n" AC_RESETALL,
          vector_size(ctx.ppids),
          FLATTEN_VECTOR(ctx.ppids)
          );
  fprintf(stderr, AC_RESETALL AC_RED           "\t%lu PIDs: %s\n" AC_RESETALL,
          vector_size(ctx.pids),
          FLATTEN_VECTOR(ctx.pids)
          );
  fprintf(stderr, AC_RESETALL AC_BLUE           "\t%lu Command Lines: %s\n" AC_RESETALL,
          vector_size(ctx.cmd_lines),
          FLATTEN_VECTOR(ctx.cmd_lines)
          );
  fprintf(stderr, AC_RESETALL AC_RED           "\t%lu Cwds: %s\n" AC_RESETALL,
          vector_size(ctx.cwds),
          FLATTEN_VECTOR(ctx.cwds)
          );
  fprintf(stderr, AC_RESETALL AC_RED           "\tOutput Mode (" AC_RESETALL AC_GREEN "%s" AC_RESETALL AC_RED "): %s\n" AC_RESETALL,
          output_mode_type_names(),
          OUTPUT_MODE_ID_NAME(ctx.output_mode)
          );
  fprintf(stderr, AC_RESETALL AC_RED           "\tOutput Fields (" AC_RESETALL AC_GREEN "%s" AC_RESETALL AC_RED "): %s\n" AC_RESETALL,
          output_field_names(),
          FLATTEN_VECTOR(ctx.output_fields)
          );
  exit(EXIT_SUCCESS);
}

static const char *get_output_mode_desc(){
  struct StringBuffer *sb = stringbuffer_new();

  stringbuffer_append_string(sb, "Output Mode ");
  stringbuffer_append_string(sb, output_mode_type_names());
  char *s = stringbuffer_to_string(sb);

  stringbuffer_release(sb);
  return(s);
}

struct cag_option options[] = {
  { .identifier     = 'v',
    .access_letters = "v",
    .access_name    = "env-val",
    .value_name     = "ENVIRONMENT_VAR_VAL",
    .description    = "Environment Variable Value", },
  { .identifier     = 'P',
    .access_letters = "P",
    .access_name    = "ppids",
    .value_name     = "PPIDs",
    .description    = "PPID", },
  { .identifier     = 'p',
    .access_letters = "p",
    .access_name    = "pids",
    .value_name     = "PIDs",
    .description    = "PID", },
  { .identifier     = 'C',
    .access_letters = "C",
    .access_name    = "cmdlines",
    .value_name     = "COMMAND_LINES",
    .description    = "Command Lines", },
  { .identifier     = 'c',
    .access_letters = "c",
    .access_name    = "cwds",
    .value_name     = "CWD",
    .description    = "Current Working Directory", },
  { .identifier     = 'f',
    .access_letters = "f",
    .access_name    = "output-fields",
    .value_name     = "OUTPUT_FIELDS",
    .description    = "Output Field", },
  { .identifier     = 'o',
    .access_letters = "o",
    .access_name    = "output-mode",
    .value_name     = "OUTPUT_MODE",
    .description    = "Output Mode", },
  { .identifier     = 'k',
    .access_letters = "k",
    .access_name    = "env-key",
    .value_name     = "ENVIRONMENT_VAR_KEY",
    .description    = "Environment Variable Key", },
  { .identifier     = 'D',
    .access_letters = "D",
    .access_name    = "debug-args",
    .value_name     = NULL,
    .description    = "Debug Args Mode", },
  { .identifier     = 'V',
    .access_letters = "V",
    .access_name    = "verbose",
    .value_name     = NULL,
    .description    = "Verbose Mode", },
  { .identifier     = 'h',
    .access_letters = "h",
    .access_name    = "help",
    .value_name     = NULL,
    .description    = "Help Mode", },
};

int parse_args(int argc, char *argv[]){
  ctx.env_keys      = vector_new();
  ctx.env_vals      = vector_new();
  ctx.cwds          = vector_new();
  ctx.output_fields = vector_new();
  ctx.pids          = vector_new();
  ctx.ppids         = vector_new();
  ctx.cmd_lines     = vector_new();
  cag_option_context context;

  cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&context)) {
    char identifier = cag_option_get(&context);
    switch (identifier) {
    case 'f': vector_push(ctx.output_fields, (void *)(char *)cag_option_get_value(&context)); break;
    case 'P': vector_push(ctx.ppids, (void *)(char *)cag_option_get_value(&context)); break;
    case 'p': vector_push(ctx.pids, (void *)(char *)cag_option_get_value(&context)); break;
    case 'c': vector_push(ctx.cwds, (void *)(char *)cag_option_get_value(&context)); break;
    case 'C': vector_push(ctx.cmd_lines, (void *)(char *)cag_option_get_value(&context)); break;
    case 'k': vector_push(ctx.env_keys, (void *)(char *)cag_option_get_value(&context)); break;
    case 'v': vector_push(ctx.env_vals, (void *)(char *)cag_option_get_value(&context)); break;
    case 'V': ctx.verbose     = true; break;
    case 'D': ctx.mode        = mode_debug_args; break;
    case 'o': ctx.output_mode = get_output_name_mode_id(cag_option_get_value(&context)); break;
    case 'h':
      fprintf(stderr, AC_RESETALL AC_YELLOW AC_BOLD "Usage: ls-proc [OPTION]\n" AC_RESETALL);
      cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
      exit(EXIT_SUCCESS);
    }
  }
  if (ctx.mode)
    return(ctx.mode());

  return(EXIT_SUCCESS);
} /* parse_args */
