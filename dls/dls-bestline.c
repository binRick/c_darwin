#pragma once
#ifndef DLS_BESTLINE_C
#define DLS_BESTLINE_C
#include "core/core.h"
#include "dls/dls.h"
#include "exec-fzf/exec-fzf.h"
#include "libforks/libforks.h"
#include "submodules/c_deps/submodules/bestline/bestline.h"
#define BESTLINE_HISTORY_FILE             ".bestline-test-history.txt"
#define DEBUG_CTRLQ_CODES                 false
#define RUN_HELP_ON_BESTLINE_START        true
#define BESTLINE_CURSOR_COLOR_CODE_FMT    "\x1b]12;%s\x1b\\"
#define GET_NEXT_INDEX(ITEMS, INDEX)    ((ITEMS[INDEX + 1]) ? (INDEX + 1) : 0)
#define MAX_ALIASES                       8
#define DEFAULT_EXEC_MODE                 BL_EXEC_MODE_REPROC
enum bl_command_handler_type_t {
  BL_COMMAND_HANDLER_TYPE_INTERNAL,
  BL_COMMAND_HANDLER_TYPE_HELP,
  BL_COMMAND_HANDLER_TYPE_PREFIXED,
  BL_COMMAND_HANDLER_TYPE_OPTPARSE,
  BL_COMMAND_HANDLER_TYPES_QTY,
};
enum bl_exec_mode_t {
  BL_EXEC_MODE_SUBPROCESS,
  BL_EXEC_MODE_FORK,
  BL_EXEC_MODE_REPROC,
};
enum bl_command_type_t {
  DLS_BESTLINE_COMMAND_TYPE_TOGGLE_CURSOR,
  DLS_BESTLINE_COMMAND_TYPE_TOGGLE_CURSOR_COLOR,
  DLS_BESTLINE_COMMAND_TYPE_TOGGLE_PREFIX,
  DLS_BESTLINE_COMMAND_TYPE_TOGGLE_PREFIX_COLOR,
  DLS_BESTLINE_COMMAND_TYPE_TOGGLE_PROMPT,
  DLS_BESTLINE_COMMAND_TYPE_TOGGLE_PROMPT_COLOR,
  DLS_BESTLINE_COMMAND_TYPE_EXIT,
  DLS_BESTLINE_COMMAND_TYPE_DEBUG,
  DLS_BESTLINE_COMMAND_TYPE_VERBOSE,
  DLS_BESTLINE_COMMAND_TYPE_FZF,
  DLS_BESTLINE_COMMAND_TYPES_QTY,
};
enum bl_command_prefix_type_t {
  BL_COMMAND_PREFIX_TYPE_WATCH,
  BL_COMMAND_PREFIX_TYPES_QTY,
};
typedef bool (^bl_line_validator_t)(char *line);
typedef bool (^bl_command_handler_t)(char *line);
typedef bool (^bl_prefix_handler_t)(bl_command_handler_t *handled_command);
typedef bool (*bl_function_handler_t)(char *line);
typedef bool (*bl_exec_t)(int argc, const char **argv);
typedef bool (^bl_cmd_callback_t)(void);
typedef bool (^dls_bl_cb)(char *);
struct bl_command_hint_t {
  const char *text;
  const char *ansi1, *ansi2;
};
struct bl_cfg_t {
  const char *history_file;
};
struct bl_command_prefix_t {
  char                *name, *aliases[32];
  bl_line_validator_t line_validator;
  bl_prefix_handler_t handler;
};
struct bl_command_handler_t {
  char                  *name;
  bl_function_handler_t fxn, validator;
  int                   priority;
};
struct bl_control_code_t {
  char      *name;
  size_t    len;
  char      *code;
  bool      enabled;
  dls_bl_cb callback;
};
struct bl_command_t {
  const char               *cmd;
  bl_cmd_callback_t        callback;
  const bool               add_history, save_history, mask_mode, hide_cursor;
  char                     *aliases[MAX_ALIASES];
  struct bl_command_hint_t hint;
  hash_t                   *aliases_hash;
};
static char *bl_offset_text(char *t);
static void bl_show_message(char *msg, int vertical_offset);
static bool bl_is_internal_command(const char *s);
static struct bl_command_t *bl_get_command_from_line(const char *s);
static bool bl_is_optparse_cmd_prefix(char *line);
static bool bl_is_valid_optparse_cmd(char *line);
static struct optparse_cmd *bl_get_optparse_cmd_prefix(char *line);
static struct optparse_cmd *bl_get_optparse_cmd(char *line);
static char *bl_wrap_command_in_shell(int argc, char *argv[]);
static hash_t *bl_fzf(char *header, int height, hash_t *hash, char *selected);
static bool bl_re_subprocess(int argc, char **argv);
static bool __dls_bestline_exited__ = false;
static void bl_toggle_mode(char *mode, bool *var, int vertical_offset);
static bool DLS_BESTLINE_DEBUG_MODE   = false;
static bool DLS_BESTLINE_VERBOSE_MODE = false;
static bool bl_handle_line(char *line);
static char *bl_remove_help_from_line(char *line);
static bool bl_line_is_help(char *line);
static bool bl_re_exec(int argc, const char **argv);
static hash_t *bl_get_command_hash(void);
static bool bl_handle_command_internal(const char *line);
static void __attribute__((constructor)) __constructor__dls_bl(void);
static enum bl_exec_mode_t       bl_exec_mode = DEFAULT_EXEC_MODE;
static hash_t                    *bl_cmd_strings, *bl_subcmds_h, *bl_help_strings_h;
static char                      *bl_help_strings[] = { "-h", "--help", "help", "HELP", "?", "/?", "/help", "/h", NULL, };
static size_t                    BESTLINE_PROMPT_INDEX = 1, BESTLINE_PREFIX_INDEX = 0, BESTLINE_CURSOR_INDEX = 1, BESTLINE_CURSOR_COLOR_INDEX = 0, BESTLINE_PREFIX_COLOR_INDEX = 0, BESTLINE_PROMPT_COLOR_INDEX = 0;
static hash_t                    *bl_cmds_h = NULL;
#include "dls/dls-bestline-static-assets.c"
static const struct bl_command_t commands[] = {
  [DLS_BESTLINE_COMMAND_TYPE_FZF] =                 {
    .cmd      = "/fzf",     .aliases      =                       {
      "fzf",
      NULL,
    }, .add_history= true,       .save_history = true,
    .callback = ^ bool ()                           {
      bl_fzf("Command", 25, bl_cmds_h, strdup(""));
      bl_fzf("Subcommand", 25, bl_subcmds_h, strdup(""));
      bl_fzf("Command String", 25, bl_cmd_strings, strdup("win"));
      return(true);
    },
    .hint     =                                         {
      .text  = "FZF Test",
      .ansi1 = AC_GREEN,
      .ansi2 = NULL,
    },
  },
  [DLS_BESTLINE_COMMAND_TYPE_VERBOSE] =             {
    .cmd      = "/verbose",            .aliases      =                   {
      "verbose", "verb", "/verb",
      NULL,
    }, .add_history= false,                 .save_history = false,
    .callback = ^ bool ()                           {
      bl_toggle_mode("Verbose", &DLS_BESTLINE_VERBOSE_MODE, 1);
      return(true);
    },
    .hint     =                                         {
      .text  = "Toggle Verbose Mode",
      .ansi1 = AC_GREEN,
      .ansi2 = NULL,
    },
  },
  [DLS_BESTLINE_COMMAND_TYPE_DEBUG] =               {
    .cmd      = "/debug",            .aliases      =                     {
      "debug",
      NULL,
    }, .add_history= true,                .save_history = true,
    .callback = ^ bool ()                           {
      bl_toggle_mode("Debug", &DLS_BESTLINE_DEBUG_MODE, 2);
      return(true);
    },
    .hint     =                                         {
      .text  = "Toggle Debug Mode",
      .ansi1 = AC_GREEN,
      .ansi2 = NULL,
    },
  },
  [DLS_BESTLINE_COMMAND_TYPE_EXIT] =                {
    .cmd                      = "/exit",               .aliases      =                      {
      "quit", "/q", "exit", "/quit", "q", "x", "ex",
      NULL,
    }, .add_history           = false,                 .save_history = false,
    .callback                 = ^ bool ()                           {
      __dls_bestline_exited__ = true;
      return(true);
    },
    .hint                     =                                         {
      .text  = "Change Cursor Color",
      .ansi1 = AC_GREEN,
      .ansi2 = NULL,
    },
  },
  [DLS_BESTLINE_COMMAND_TYPE_TOGGLE_CURSOR_COLOR] = {
    .cmd                          = "/cursorcolor",                        .aliases      =               {
      "/curc",
      NULL,
    }, .add_history               = true,                                  .save_history = true,
    .callback                     = ^ bool ()                           {
      BESTLINE_CURSOR_COLOR_INDEX = GET_NEXT_INDEX(BESTLINE_CURSOR_COLORS, BESTLINE_CURSOR_COLOR_INDEX);
      return(true);
    },
    .hint                         =                                         {
      .text  = "Change Cursor Color",
      .ansi1 = AC_GREEN,
      .ansi2 = NULL,
    },
  },
  [DLS_BESTLINE_COMMAND_TYPE_TOGGLE_CURSOR] =       {
    .cmd                    = "/cursor",                       .aliases      =                    {
      "/cur",
      NULL,
    }, .add_history         = true,                            .save_history = true,
    .callback               = ^ bool ()                           {
      BESTLINE_CURSOR_INDEX = GET_NEXT_INDEX(BESTLINE_CURSORS, BESTLINE_CURSOR_INDEX);
      return(true);
    },
    .hint                   =                                         {
      .text  = "Change Cursor",
      .ansi1 = AC_GREEN,
      .ansi2 = NULL,
    },
  },
  [DLS_BESTLINE_COMMAND_TYPE_TOGGLE_PREFIX_COLOR] = {
    .cmd                          = "/prefixcolor",                        .aliases      =               {
      "/prec",
      NULL,
    }, .add_history               = true,                                  .save_history = true,
    .callback                     = ^ bool ()                           {
      BESTLINE_PREFIX_COLOR_INDEX = GET_NEXT_INDEX(BESTLINE_PREFIX_COLORS, BESTLINE_PREFIX_COLOR_INDEX);
      return(true);
    },
    .hint                         =                                         {
      .text  = "Change Prefix Color",
      .ansi1 = AC_GREEN,
      .ansi2 = NULL,
    },
  },
  [DLS_BESTLINE_COMMAND_TYPE_TOGGLE_PREFIX] =       {
    .cmd                    = "/prefix",                        .aliases      =                    {
      "/pre",
      NULL,
    }, .add_history         = true,                             .save_history = true,
    .callback               = ^ bool ()                           {
      BESTLINE_PREFIX_INDEX = GET_NEXT_INDEX(BESTLINE_PREFIXES, BESTLINE_PREFIX_INDEX);
      return(true);
    },
    .hint                   =                                         {
      .text  = "Change Prefix",
      .ansi1 = AC_GREEN,
      .ansi2 = NULL,
    },
  },
  [DLS_BESTLINE_COMMAND_TYPE_TOGGLE_PROMPT_COLOR] = {
    .cmd                          = "/promptcolor",                        .aliases      =               {
      "/proc",
      NULL,
    }, .add_history               = true,                                  .save_history = true,
    .callback                     = ^ bool ()                           {
      BESTLINE_PROMPT_COLOR_INDEX = GET_NEXT_INDEX(BESTLINE_PROMPT_COLORS, BESTLINE_PROMPT_COLOR_INDEX);
      return(true);
    },
    .hint                         =                                         {
      .text  = "Change Prompt Color",
      .ansi1 = AC_GREEN,
      .ansi2 = NULL,
    },
  },
  [DLS_BESTLINE_COMMAND_TYPE_TOGGLE_PROMPT] =       {
    .cmd                    = "/prompt",                       .aliases      =                    {
      "/pro",
      NULL,
    }, .add_history         = true,                            .save_history = true,
    .callback               = ^ bool ()                           {
      BESTLINE_PROMPT_INDEX = GET_NEXT_INDEX(BESTLINE_PROMPTS, BESTLINE_PROMPT_INDEX);
      return(true);
    },
    .hint                   =                                         {
      .text  = "Change Prompt",
      .ansi1 = AC_GREEN,
      .ansi2 = NULL,
    },
  },
};

void bl_completion(const char *buf, bestlineCompletions *lc) {
  struct bl_command_t *cmd;

  hash_each(bl_cmd_strings, {
    if (
      (stringfn_starts_with(key, buf))
      )
      bestlineAddCompletion(lc, key);
  });
  hash_each(bl_cmds_h, {
    if (stringfn_starts_with(key, buf)) {
      cmd = (struct bl_command_t *)val;
      bestlineAddCompletion(lc, cmd->cmd);
    }
  });
}

static char *bl_offset_text(char *t){
  char *s;

  asprintf(&s, "        %s", t);
  return(s);
}
static const struct bl_control_code_t control_codes[] = {
  { "control u",    6, "\\x15",      true, ^ bool (char *s){
                        char *m;
                        asprintf(&m, "%s @ %lld", "control u", timestamp());
                        bl_show_message(m, 1);
                        return(true);
                      }, },
  { "control y",    6, "\\x19",      true,
                      ^ bool (char *s){
                        char *m;
                        asprintf(&m, "%s @ %lld", "control y", timestamp());
                        bl_show_message(m, 2);
                        return(true);
                      }, },
  { "control b",    6, "\\x02",      true,
                      ^ bool (char *s){
                        char *m;
                        hash_t *res = bl_fzf("Control B", 25, bl_cmds_h, strdup(""));
                        /*
                         * if(res)
                         * hash_each(res,{
                         *    Dbg(key,%s);
                         * });
                         */
                        asprintf(&m, "%s @ %lld", "control b", timestamp());
                        bl_show_message(m, 3);
                        return(true);
                      }, },
  { "control v",    6, "\\x16",      true,
                      ^ bool (char *s){
                        char *m;
                        asprintf(&m, "Control+%s @ %lld", "v", timestamp());
                        bl_show_message(m, 2);
                        return(true);
                      }, },
  { "control x",    6, "\\x18",      true,
                      ^ bool (char *s){
                        char *m;
                        asprintf(&m, "Control+%s @ %lld", "v", timestamp());
                        bl_show_message(m, 2);
                        return(true);
                      }, },
  { "control e",    6, "\\x05",      true,
                      ^ bool (char *s){
                        char *m;
                        asprintf(&m, "%s @ %lld", "control e", timestamp());
                        bl_show_message(m, 4);
                        return(true);
                      }, },
  { "control w",    6, "\\x17",      true,
                      ^ bool (char *s){
                        char *m;
                        asprintf(&m, "%s @ %lld", "control w", timestamp());
                        bl_show_message(m, 5);
                        return(true);
                      }, },
  { "control t",    6, "\\x14",      true,
                      ^ bool (char *s){
                        char *m;
                        asprintf(&m, "%s @ %lld", "control t", timestamp());
                        bl_show_message(m, 6);
                        return(true);
                      }, },
  { "control /",    6, "\\x1f",      true,
                      ^ bool (char *s){
                        char *m;
                        asprintf(&m, "%s @ %lld", "control /", timestamp());
                        bl_show_message(m, 7);
                        return(true);
                      }, },
  { "control home", 9, "\\x1b[1;5H", true, },
  { "control end",  9, "\\x1b[1;5F", true, },
  { "control del",  9, "\\x1b[3;5~", true, },
};

static void bl_ctrlq_callback(char *s){
  bool ok = false;

  for (size_t i = 0; !ok && i < 99 && control_codes[i].name; i++)
    if (
      (control_codes[i].enabled && control_codes[i].len == strlen(s))
      &&
      (stringfn_equal(control_codes[i].code, stringfn_substring(s, 0, strlen(control_codes[i].code)))
      ))
      if (control_codes[i].callback && !(ok = control_codes[i].callback(s)))
        log_error("cb failed");
  if (!ok && DEBUG_CTRLQ_CODES)
    log_debug(">>%lu>> %s", strlen(s), s);

  return;
}

char *hints(const char *buf, const char **ansi1, const char **ansi2) {
  struct bl_command_t *cmd;
  struct optparse_cmd *c;

  hash_each(bl_cmds_h, {
    if (stringfn_equal(key, buf)) {
      cmd = (struct bl_command_t *)val;
      if (cmd->hint.ansi1)
        *ansi1 = strdup(cmd->hint.ansi1);
      if (cmd->hint.ansi2)
        *ansi2 = strdup(cmd->hint.ansi1);
      if (cmd->hint.text)
        return(bl_offset_text(cmd->hint.text));
    }
  });
  hash_each(bl_cmd_strings, {
    if (stringfn_equal(key, buf)) {
      c      = (struct optparse_cmd *)val;
      *ansi1 = strdup(AC_GREEN);
      return(bl_offset_text(c->description));
    }
  });
  if (buf[0] == '\0') {
    *ansi1 = strdup(AC_GREEN);
    *ansi2 = strdup(AC_BLACK);
    return("     [Tab for modes]");
  }
  return(NULL);
}

char *generate_prompt(){
  printf("%s", BESTLINE_CURSORS[BESTLINE_CURSOR_INDEX]);
  printf(BESTLINE_CURSOR_COLOR_CODE_FMT, BESTLINE_CURSOR_COLORS[BESTLINE_CURSOR_COLOR_INDEX]);
  fflush(stdout);
  char *s;
  asprintf(&s,
           AC_RESETALL
           "%s"
           "%s"
           AC_RESETALL
           " "
           "%s"
           "%s"
           AC_RESETALL
           " "
           "%s",
           BESTLINE_PREFIX_COLORS[BESTLINE_PREFIX_COLOR_INDEX],
           BESTLINE_PREFIXES[BESTLINE_PREFIX_INDEX],
           BESTLINE_PROMPT_COLORS[BESTLINE_PROMPT_COLOR_INDEX],
           BESTLINE_PROMPTS[BESTLINE_PROMPT_INDEX],
           ""
           );
  return(s);
}

////////////////////////////////////////////
static void bl_show_message(char *msg, int vertical_offset){
  size_t w = (size_t)get_terminal_width();
  char   *m, *m1;

  asprintf(&m,
           "%s",
           msg
           );
  asprintf(&m1,
           AC_CURSOR_SAVE
           AC_HIDE_CURSOR
           "\033[%d;%dH"
           AC_CLEAR_EOL
           " "
           "%s%s" AC_RESETALL
           AC_CURSOR_RESTORE
           AC_SHOW_CURSOR,
           vertical_offset,
           (int)(w - strlen(m) - 1),
           AC_GREEN,
           m
           );
  fprintf(stdout, "%s", m1);
  fflush(stdout);
  free(m);
}

static void bl_toggle_mode(char *mode, bool *var, int vertical_offset){
  *var = !*var;
  size_t w = (size_t)get_terminal_width();
  char   *msg;
  asprintf(&msg,
           "%s Mode %s",
           mode,
           *var ? "Enabled" : "Disabled"
           );
  printf(
    AC_CURSOR_SAVE
    AC_HIDE_CURSOR
    "\033[%d;%dH"
    AC_CLEAR_EOL
    " "
    "%s%s" AC_RESETALL
    AC_CURSOR_RESTORE
    AC_SHOW_CURSOR,
    vertical_offset,
    (int)(w - strlen(msg) - 1),
    *var ? AC_GREEN : AC_RED,
    msg
    );
  free(msg);
}

static hash_t *bl_get_command_hash(){
  static hash_t *h = NULL;

  if (h)
    return(h);

  h = hash_new();
  for (size_t i = 0; i < DLS_BESTLINE_COMMAND_TYPES_QTY; i++)
    if (commands[i].cmd)
      hash_set(h, commands[i].cmd, &(commands[i]));
  return(h);
}

static hash_t *get_command_aliases_hash(struct bl_command_t *cmd){
  hash_t *h = hash_new();

  for (size_t ii = 0; ii < MAX_ALIASES && cmd->aliases[ii]; ii++)
    hash_set(h, cmd->aliases[ii], (void *)0);
  return(h);
}

static bool bl_is_internal_command(const char *s){
  return((bl_get_command_from_line(s) != NULL));
}
static struct bl_command_t *bl_get_command_from_line(const char *s){
  if (!s)
    return(NULL);

  struct bl_command_t *cmd;
  hash_each(bl_get_command_hash(), {
    cmd = (struct bl_command_t *)val;
    if (cmd->cmd && strcmp(s, cmd->cmd) == 0)
      return(cmd);

    hash_each(get_command_aliases_hash(cmd), {
      if (key && strcmp(s, key) == 0)
        return(cmd);
    });
  });
  return(NULL);
}

static bool bl_handle_command_internal(const char *line){
  struct bl_command_t *cmd = bl_get_command_from_line(line);

  if (cmd->hide_cursor)
    printf(AC_HIDE_CURSOR);
  if (cmd->mask_mode)
    bestlineMaskModeEnable();
  if (!cmd->callback()) {
    log_error("Failed running command %s", cmd->cmd);
    return(false);
  }
  if (cmd->hide_cursor)
    printf(AC_SHOW_CURSOR);
  if (cmd->mask_mode)
    bestlineMaskModeDisable();
  if (cmd->add_history)
    bestlineHistoryAdd(line);
  if (cmd->save_history)
    bestlineHistorySave(BESTLINE_HISTORY_FILE);
  return(true);
}

static bool bl_handle_command_optparse_prefix(const char *line){
  char *c;

  asprintf(&c, "%s %s", DLS_EXECUTABLE, line);
  struct StringFNStrings split = stringfn_split(c, ' ');

  if (!bl_re_exec(split.count, split.strings)) {
    log_error("Failed to run %s", c);
    return(false);
  }
  stringfn_release_strings_struct(split);
  return(true);
}

static char *bl_remove_help_from_line(char *line){
  while (bl_line_is_help(line))
    hash_each(bl_help_strings_h, {
      if (stringfn_ends_with(line, key))
        line = stringfn_substring(line, 0, strlen(line) - strlen(key));
    });
  return(stringfn_mut_trim(line));
}

static bool bl_line_is_help(char *line){
  char *s;

  hash_each(bl_help_strings_h, {
    asprintf(&s, "%s", key);
    if (stringfn_ends_with(line, s) || strcmp(line, key) == 0)
      return(true);
  });
  return(false);
}

static hash_t *get_help_strings_hash(){
  hash_t *h = hash_new();

  for (size_t ii = 0; ii < 1024 && bl_help_strings[ii]; ii++)
    hash_set(h, bl_help_strings[ii], (void *)0);
  return(h);
}

static int bl_forked(libforks_ServerConn c, int __bl_socket_fd) {
  assert(libforks_free_conn(c) == libforks_OK);
  char *msg, *r = calloc(1, 1024);
  read(__bl_socket_fd, r, 1024);
  asprintf(&msg, "%s", r);
  write(__bl_socket_fd, msg, strlen(msg));

  return(EXIT_SUCCESS);
}

static pid_t bl_fork_server_pid;

static bool bl_re_fork(int argc, const char **argv){
  libforks_ServerConn bl_conn;

  assert(libforks_start(&bl_conn) == libforks_OK);
  assert((bl_fork_server_pid = libforks_get_server_pid(bl_conn)) > 0);
  Dbg(bl_fork_server_pid, %d);
  int   bl_socket_fd = 0, bl_exit_fd = 0;
  pid_t bl_pid = 0;

  assert(libforks_fork(
           bl_conn,
           &bl_pid,
           &bl_socket_fd,
           &bl_exit_fd,
           bl_forked
           ) == libforks_OK);
  printf(AC_YELLOW "Parent> Created child with pid %d\n"AC_RESETALL, bl_pid);
  char *msg    = calloc(1, 1024);
  char *setup  = "xxx123";
  char *bl_cmd = stringfn_join(argv, " ", 0, argc);

  Dbg(bl_cmd, %s);
  bl_cmd = b64_encode(bl_cmd, strlen(bl_cmd));
  Dbg(bl_cmd, %s);
  write(bl_socket_fd, bl_cmd, strlen(setup));
  while (true) {
    int read_res = read(bl_socket_fd, msg, 1024);
    if (read_res == 0)
      break;
    log_debug(AC_RED "<PID %d>"AC_RESETALL " " AC_GREEN "%s" AC_RESETALL,
              bl_pid,
              (char *)msg
              );
  }
}

static bool bl_re_proc(int argc, const char **argv){
  char       *c              = stringfn_join(argv, " ", 0, argc);
  const char *command_line[] = { "/bin/sh", "--norc", "--noprofile", "-c", c, NULL };
  reproc_t   *process        = NULL;
  int        r               = REPROC_ENOMEM;
  int        ec              = 0;

  errno = 0;
  if (!(process = reproc_new())) {
    ec = 100;
    goto finish;
  }
  errno = 0;
  if ((r = reproc_start(process, command_line, (reproc_options){
    .redirect.parent = true,
    .deadline = 15000,
  })
       ) < 0) {
    ec = 200;
    goto finish;
  }
  errno = 0;
  if ((r = reproc_wait(process, REPROC_INFINITE)) < 0) {
    ec = 300;
    goto finish;
  }
finish:
  reproc_destroy(process);
  if (r < 0) {
    log_error(AC_RED "Error Code: %d" AC_RESETALL "\n", ec);
    log_error(AC_RED "%d" AC_RESETALL "\n", r);
    log_error(AC_RED "%s" AC_RESETALL "\n", reproc_strerror(r));
  }else
    return(true);
}

static bool bl_re_exec(int argc, const char **argv){
  bl_exec_t _exec = NULL;

  switch (bl_exec_mode) {
  case BL_EXEC_MODE_FORK: _exec       = bl_re_fork; break;
  case BL_EXEC_MODE_REPROC: _exec     = bl_re_proc; break;
  case BL_EXEC_MODE_SUBPROCESS: _exec = bl_re_subprocess; break;
  }
  if (!_exec(argc, argv)) {
    log_error("Failed to %d", bl_exec_mode);
    return(false);
  }
  return(true);
}

bool bl_handle_command_help(const char *line){
  char *c, *l = strdup(line);

  if (stringfn_equal(l, "help"))
    asprintf(&c, "%s --help", DLS_EXECUTABLE);
  else{
    l = bl_remove_help_from_line(l);
    asprintf(&c, "%s help %s", DLS_EXECUTABLE, l);
  }
  stringfn_mut_trim(c);
  struct StringFNStrings split = stringfn_split(c, ' ');

  if (!bl_re_exec(split.count, split.strings)) {
    log_error("Failed to exec %s", c);
    return(false);
  }
  return(true);
}

static char *bl_wrap_command_in_shell(int argc, char *argv[]){
  char *command_line[] = { "/bin/sh", "--norc", "--noprofile", "-c", stringfn_join(argv, " ", 0, argc), NULL };
  char *cmd            = stringfn_join(command_line, " ", 0, 5);

  return(cmd);
}

static hash_t *bl_fzf(char *header, int height, hash_t *hash, char *selected){
  int               res    = -1;
  hash_t            *res_h = hash_new();
  struct fzf_exec_t *e     = exec_fzf_setup();
  {
    e->input_options      = vector_new();
    e->selected_options   = vector_new();
    e->fzf_keybinds_v     = vector_new();
    e->fzf_header_lines_v = vector_new();
  }
  {
    e->fzf_keybinds_sb     = stringbuffer_new();
    e->fzf_header_lines_sb = stringbuffer_new();
  }

  e->header              = header;
  e->debug_mode          = false;
  e->height              = height;
  e->fzf_pointer         = "->";
  e->fzf_marker          = "* ";
  e->fzf_reverse         = true;
  e->header_first        = true;
  e->border              = true;
  e->cycle               = true;
  e->ansi                = true;
  e->fzf_default_command = "";
  e->query_s             = selected ? selected : "";
  e->fzf_prompt          = ">";
  e->fzf_default_opts    = "";
  e->fzf_default_command = "";
  e->fzf_info            = "inline";
  e->fzf_history_file    = BESTLINE_HISTORY_FILE;
  e->height              = 30;
  e->header_lines        = 0;
  e->select_multiple     = true;
  e->top_margin          = 5;
  e->right_margin        = 0;
  e->left_margin         = 0;
  e->bottom_margin       = 0;
  e->top_padding         = 1;
  e->right_padding       = 0;
  e->left_padding        = 0;
  e->bottom_padding      = 0;
  e->preview_size        = 70;
  e->preview_type        = "right";
  asprintf(&(e->header), AC_YELLOW "[%s]" AC_RESETALL, header);
  asprintf(&(e->fzf_prompt), AC_YELLOW "Select a %s> " AC_RESETALL, header);
  hash_each(hash, {
    vector_push(e->input_options, key);
  });
  res = exec_fzf(e);
  if (vector_size(e->selected_options) > 0) {
    log_info("Selected %lu/%lu options", vector_size(e->selected_options), vector_size(e->input_options));
    /*
     * for (size_t i = 0; i < vector_size(e->selected_options); i++) {
     * char *n = (char *)(vector_get(e->selected_options,i));
     * Dbg(n,%s);
     * //      hash_set(res_h,n,(void*)0);
     * }*/
  }
  exec_fzf_release(e);
  return(res_h);
} /* bl_fzf */

static bool bl_re_subprocess(int argc, char **argv){
  int                 exited, result;
  struct subprocess_s subprocess;
  char                stdout_buffer[1024 * 16] = { 0 };
  char                stderr_buffer[1024 * 16] = { 0 };
  size_t              bytes_read = 0, bytes_read_err = 0;

  result = subprocess_create(bl_wrap_command_in_shell(argc, argv), 0, &subprocess);
  assert(result == 0);
  do {
    bytes_read = subprocess_read_stdout(&subprocess, stdout_buffer, 1);
    fprintf(stdout, "%s", stdout_buffer);
    bytes_read_err = subprocess_read_stderr(&subprocess, stderr_buffer, 1);
    fprintf(stderr, "%s", stderr_buffer);
  } while (bytes_read != 0);

  result = subprocess_join(&subprocess, &exited);
  assert(result == 0);
  assert(exited == 0);
  return(true);
}

static bool bl_handle_command_optparse(const char *line){
  if (bl_line_is_help(line))
    return(bl_handle_command_help(line));

  char *c;
  stringfn_mut_trim(line);
  asprintf(&c, "%s %s", DLS_EXECUTABLE, line);
  stringfn_mut_trim(c);
  struct StringFNStrings split = stringfn_split(c, ' ');
  char                   *cc[split.count + 1];
  for (size_t i = 0; i < split.count; i++)
    cc[i] = strdup(split.strings[i]);
  cc[split.count] = "\0";
  if (!bl_re_exec(split.count, cc))
    log_error("Failed to run %s", c);
  stringfn_release_strings_struct(split);
  bestlineHistoryAdd(line);
  bestlineHistorySave(BESTLINE_HISTORY_FILE);
  return(true);
}

static bool bl_is_optparse_cmd_prefix(char *line){
  return((bl_get_optparse_cmd_prefix(line) != NULL));
}

static struct optparse_cmd *bl_get_optparse_cmd_prefix(char *line){
  char *s;

  hash_each(bl_subcmds_h, {
    asprintf(&s, "%s ", key);
    bool m = stringfn_starts_with(line, s);
    if (m)
      return(val);

    free(s);
  });
  return(NULL);
}

static bool bl_is_valid_optparse_cmd(char *line){
  return((bl_get_optparse_cmd(line) != NULL));
}

static struct optparse_cmd *bl_get_optparse_cmd(char *line){
  hash_each(bl_cmd_strings, {
    if (stringfn_equal(key, line))
      return(val);
  });
  return(NULL);
}

static char *palette = "\x1b]11;#000000\x1b]10;#ffffff\x1b]12;#ff6c5c\x1b]4;0;#393939\x1b]4;1;#da4939\x1b]4;2;#9acc79\x1b]4;3;#d0d262\x1b]4;4;#6d9cbe\x1b]4;5;#9f5079";
//^[\^[]4;6;#435d75^[\^[]4;7;#c2c2c2^[\^[]4;8;#474747^[\^[]4;9;#da4939^[\^[]4;10;#9acc79^[\^[]4;11;#d0d26b^[\^[]4;12;#6d9cbe^[\^[]4;13;#9f5079^[\^[]4;14;#435d75^[\^[]4;15;#c2c2c2^[\^[[3321D";

void dls_bestline_loop(){
  if (!isatty(STDOUT_FILENO))
    return;

  char *line = NULL;
  bestlineClearScreen(fileno(stdout));
  bl_cmds_h         = bl_get_command_hash();
  bl_cmd_strings    = get_command_strings(dls_cmd);
  bl_subcmds_h      = get_subcommands_hash(dls_cmd);
  bl_help_strings_h = get_help_strings_hash();
  bestlineHistoryLoad(BESTLINE_HISTORY_FILE);
  if (args->prompt_commands_qty > 0)
    for (size_t i = 0; i < args->prompt_commands_qty; i++)
      bl_handle_line(args->prompt_commands[i]);
  else if (RUN_HELP_ON_BESTLINE_START)
    bl_handle_line("help");

  bestlineSetCompletionCallback(bl_completion);
  bestlineSetCtrlqCallback(bl_ctrlq_callback);
  bestlineSetHintsCallback(hints);
  printf(AC_SHOW_CURSOR);
  printf("%s", palette);
  fflush(stdout);
  while (
    (!__dls_bestline_exited__)
    &&
    ((line = stringfn_mut_trim(bestline(generate_prompt()))) != NULL)
    )
    bl_handle_line(line);
  free(line);
}

static const struct bl_command_handler_t bl_command_handlers[] = {
  [BL_COMMAND_HANDLER_TYPE_OPTPARSE] = { .name = "optparse commands", .fxn = bl_handle_command_optparse,        .validator = bl_is_valid_optparse_cmd,  },
  [BL_COMMAND_HANDLER_TYPE_HELP]     = { .name = "help commands",     .fxn = bl_handle_command_help,            .validator = bl_line_is_help,           },
  [BL_COMMAND_HANDLER_TYPE_PREFIXED] = { .name = "prefixed commands", .fxn = bl_handle_command_optparse_prefix, .validator = bl_is_optparse_cmd_prefix, },
  [BL_COMMAND_HANDLER_TYPE_INTERNAL] = { .name = "internal commands", .fxn = bl_handle_command_internal,        .validator = bl_is_internal_command,    },
};
static const struct bl_command_prefix_t  command_prefixes[] = {
  { .name           = "watch", .aliases = { "poll", "retry", NULL },
    .line_validator = ^ bool (char *line){
      return(true);
    },
    .handler = ^ bool (bl_command_handler_t *handler){
      return(true);
    }, },
};

static bool bl_run_command_handlers(const char *line){
  return(true);

  size_t len = BL_COMMAND_HANDLER_TYPES_QTY;
  for (size_t i = 0; i < len; i++)
    if (bl_command_handlers[i].fxn(strdup(line)))
      return(true);

  return(false);
}

static bool bl_get_validation_results(const char *line, int *_okqty, int *_qty){
  if (!line || strlen(line) < 1)
    return(false);

  size_t        len = BL_COMMAND_HANDLER_TYPES_QTY;
  unsigned long started[len + 10], dur[len + 10];
  started[0] = timestamp();
  int           qty = 0, okqty = 0;
  for (size_t i = 0; i < len; i++) {
    bool bb = false;
    started[i + 1] = timestamp();
    bb             = bl_command_handlers[i].validator(strdup(line));
    if (bb)
      okqty++;
    dur[i + 1] = timestamp() - started[i + 1];
    if (bb)
      okqty++;
    qty++;
  }
  dur[0]  = timestamp() - started[0];
  *_okqty = okqty;
  *_qty   = qty;
  return(okqty > 0);
}

static bool line_is_encoded_c_literal(char *line){
  static const char *matches[] = {
    "\x1b[",
    NULL,
  };

  for (int i = 0; i < 99 && matches[i]; i++)
    Dbg(matches[i], %s);
  return(false);
}

static bool bl_handle_line(char *line){
//  if(line_is_encoded_c_literal(line))return false;
  if (!line || strlen(line) < 1)
    return(false);

  if (stringfn_starts_with(line, "dls "))
    line = stringfn_substring(line, strlen("dls "), strlen(line) - strlen("dls "));
  struct dls_bestline_command_t *cmd = NULL;
  struct optparse_cmd           *c   = NULL;
  bool                          ok   = false;
  /*
   * int qty=0,okqty=0;
   * if(!bl_get_validation_results(line,&okqty,&qty))
   * if(line&&strlen(line)>4)
   *  log_warn("Matched Handlers> %d/%d",okqty,qty);
   * if(!bl_run_command_handlers(line))
   * log_error("validation handlers failed");
   * else
   * if(!bl_run_command_handlers(line)){
   *  log_error("validation handlers failed");
   * }else{
   *  log_debug("okqty=%d,qty=%d",okqty,qty);
   *  goto done;
   * }*/
  if ((cmd = bl_get_command_from_line(line)) && (bl_handle_command_internal(line)))
    goto ok;
  if ((bl_line_is_help(line)) && (bl_handle_command_help(line)))
    goto ok;
  if ((c = bl_get_optparse_cmd_prefix(line)) && (bl_handle_command_optparse_prefix(line)))
    goto ok;
  if ((c = bl_get_optparse_cmd(line)) && (bl_handle_command_optparse(line)))
    goto ok;
  fprintf(stderr, AC_RED "Unrecognized command"AC_RESETALL ": "AC_RESETALL AC_YELLOW AC_REVERSED "%s"AC_RESETALL "\n", line);
  goto fail;
fail:
  ok = false;
  if (strlen(line) > 7) {
    bestlineHistoryAdd(line);
    bestlineHistorySave(BESTLINE_HISTORY_FILE);
  }
  goto done;
ok:
  ok = true;
  bestlineHistoryAdd(line);
  bestlineHistorySave(BESTLINE_HISTORY_FILE);
  goto done;
done:
  printf(AC_SHOW_CURSOR);
  return(ok);
} /* handle_line */
static void __attribute__((constructor)) __constructor__dls_bl(void){
}

#endif
