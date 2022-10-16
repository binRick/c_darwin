#pragma once
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "core/utils/utils.h"
#include "djbhash/src/djbhash.h"
#include "kitty/kitty.h"
#include "log/log.h"
#include "parson.h"
#include "process/process.h"
#include "socket99/socket99.h"
#include "subprocess.h/subprocess.h"
#include "tiny-regex-c/re.h"
#include "which/src/which.h"
#include "wildcardcmp/wildcardcmp.h"
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
static bool kitty_pid_is_kitty_process(int pid);
const size_t KITTY_TCP_BUFFER_SIZE                   = 8192;
const char   *KITTY_ESC_CODE_CLEAR                   = "\x1b_Ga=d,q=2\x1b\\";
const char   *KITTY_ESC_QUERY                        = "\x1b_Gi=1,a=q;\x1b\\";
const char   *KITTY_ESC_CHECK_TERMINAL_CAPS          = "\x1b_Ga=q,s=1,v=1,i=1;YWJjZA==\x1b\\";
const char   *KITTY_ESC_DELETE_ALL_VISIBLE_PLAYMENTS = "\x1b_Ga=d\x1b\\";
const char   *KITTY_ESC_DRAW_IMAGE                   = "\x1b_Ga=T,f=100,s=192,v=192,X=4,t=f;L1VzZXJzL3JpY2svdGlnci90aWdyLnBuZw==\x1b\\";
const char   *KITTY_ESC_DRAW_IMAGE1                  = "\x1b_Ga=T,f=100,s=192,v=192,X=4,t=f;L1VzZXJzL3JpY2svdGlnci90aWdyLnBuZw==\x1b\\";
const char   *KITTY_ESC_QUERY_BG_COLOR               = "\033]11;?\033\\";
static bool vector_contains_pid(struct Vector *pids_v, int pid);

bool kitty_set_tab_color(char *MODE, char *COLOR){
  char *s;

  asprintf(&s, "%s=%s", (char *)MODE, (char *)COLOR);
  struct Vector *v = vector_new();

  vector_push(v, "@");
  vector_push(v, "set-tab_color");
  vector_push(v, s);
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

bool kitty_send_text(char *TEXT){
  char *s;

  asprintf(&s, "\"%s\"", (char *)TEXT);
  struct Vector *v = vector_new();

  vector_push(v, "@");
  vector_push(v, "send-text");
  vector_push(v, s);
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

bool kitty_set_layout(char *LAYOUT){
  struct Vector *v = vector_new();

  vector_push(v, "@");
  vector_push(v, "goto-layout");
  vector_push(v, LAYOUT);
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

char *kitty_list_fonts(){
  struct Vector *v = vector_new();

  vector_push(v, "+");
  vector_push(v, "list-fonts");
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

char *kitty_query_terminal(){
  struct Vector *v = vector_new();

  vector_push(v, "+");
  vector_push(v, "kitten");
  vector_push(v, "query_terminal");
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

char *kitty_ls_kittens(){
  struct Vector *v = vector_new();

  vector_push(v, "+");
  vector_push(v, "kitten");
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

char *kitty_get_ls(){
  struct Vector *v = vector_new();

  vector_push(v, "@");
  vector_push(v, "ls");
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

char *kitty_icat(char *file){
  struct Vector *v = vector_new();

  vector_push(v, "+kitten");
  vector_push(v, "icat");
  vector_push(v, file);
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

char *kitty_get_colors(){
  struct Vector *v = vector_new();

  vector_push(v, "@");
  vector_push(v, "get-colors");
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

char *kitty_get_last_cmd_output(){
  struct Vector *v = vector_new();

  vector_push(v, "@");
  vector_push(v, "get-text");
  vector_push(v, "--extent=last_non_empty_output");
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

char *kitty_get_text(){
  struct Vector *v = vector_new();

  vector_push(v, "@");
  vector_push(v, "get-text");
  vector_push(v, "--self");
  vector_push(v, "--ansi");
  vector_push(v, "--extent=all");
  //   vector_push(v, "--extent=last_non_empty_output");
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  char *r = strdup(cmd_result->stdout_buffer);

  return(r);
}

bool kitty_set_window_title(char *TAB_TITLE){
  struct Vector *v = vector_new();

  vector_push(v, "@");
  vector_push(v, "set-window-title");
  vector_push(v, TAB_TITLE);
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  return(true);
}

bool kitty_set_tab_title(char *TAB_TITLE){
  struct Vector *v = vector_new();

  vector_push(v, "@");
  vector_push(v, "set-tab-title");
  vector_push(v, TAB_TITLE);
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  return(true);
}

bool kitty_set_font_size(int FONT_SIZE){
  char *font_size_s;

  asprintf(&font_size_s, "%d", (int)FONT_SIZE);
  struct Vector *v = vector_new();

  vector_push(v, "@");
  vector_push(v, "set-font-size");
  vector_push(v, font_size_s);
  struct kitty_process_communication_result_t *cmd_result = kitty_process_communication(v);

  printf("%s\n", cmd_result->stdout_buffer);
  printf("%s\n", cmd_result->stderr_buffer);
  return(true);
}

struct kitty_process_communication_result_t *kitty_process_communication(struct Vector *CMD_VECTOR){
  struct kitty_process_communication_result_t *r = malloc(sizeof(struct kitty_process_communication_result_t));

  r->subprocess      = malloc(sizeof(struct subprocess_s));
  r->kitty_exec_path = "/usr/local/bin/kitty";
  //(char *)which_path("kitty", getenv("PATH"));
  struct Vector *v = vector_new();

  vector_push(v, r->kitty_exec_path);
  for (size_t i = 0; i < vector_size(CMD_VECTOR); i++) {
    vector_push(v, (char *)vector_get(CMD_VECTOR, i));
  }
  for (size_t i = 0; i < vector_size(v); i++) {
    fprintf(stderr, "cmd #%lu: %s\n", i, (char *)vector_get(v, i));
  }
  char **cmd_arr = vector_to_array(v);

  r->subprocess_result = -1;
  r->subprocess_exited = -1;
  r->STDOUT_BUFFER     = stringbuffer_new();
  r->STDERR_BUFFER     = stringbuffer_new();
  r->stderr_bytes_read = 0;
  r->stdout_bytes_read = 0;

  subprocess_create(cmd_arr, 0, r->subprocess);
  printf("created.\n");

  size_t bytes_read = 0;

  do {
    bytes_read = subprocess_read_stdout(r->subprocess, r->stdout_buffer, READ_BUFFER_SIZE - 1);
    stringbuffer_append_string(r->STDOUT_BUFFER, r->stdout_buffer);
    r->stdout_bytes_read += bytes_read;
  } while (bytes_read != 0);
  bytes_read = 0;
  printf("read %lu stdout bytes.\n", r->stdout_bytes_read);
  do {
    bytes_read = subprocess_read_stderr(r->subprocess, r->stderr_buffer, READ_BUFFER_SIZE - 1);
    stringbuffer_append_string(r->STDERR_BUFFER, r->stderr_buffer);
    r->stderr_bytes_read += bytes_read;
  } while (bytes_read != 0);
  printf("read %lu stderr bytes.\n", r->stderr_bytes_read);

  r->subprocess_join_result = subprocess_join(r->subprocess, &r->subprocess_exited);

  r->READ_STDOUT = stringbuffer_to_string(r->STDOUT_BUFFER);
  r->READ_STDERR = stringbuffer_to_string(r->STDERR_BUFFER);

  stringbuffer_release(r->STDOUT_BUFFER);
  stringbuffer_release(r->STDERR_BUFFER);
  puts(r->READ_STDOUT);
  puts(r->READ_STDERR);
  //assert(r->subprocess_result == 0);
  return(r);
} /* kitty_process_communication */

kitty_listen_on_t *parse_kitten_listen_on(char *KITTY_LISTEN_ON){
  struct StringFNStrings KittyListenOnSplit = stringfn_split(KITTY_LISTEN_ON, ':');

  assert(KittyListenOnSplit.count == 3);
  kitty_listen_on_t *KLO = malloc(sizeof(kitty_listen_on_t));

  KLO->protocol = strdup(KittyListenOnSplit.strings[0]);
  KLO->host     = strdup(KittyListenOnSplit.strings[1]);
  KLO->port     = atoi(KittyListenOnSplit.strings[2]);
  stringfn_release_strings_struct(KittyListenOnSplit);
  return(KLO);
}

struct Vector *kitty_get_color_types(const char *HOST, const int PORT){
  struct Vector          *color_types  = vector_new();
  char                   *kitty_colors = kitty_cmd_data(kitty_tcp_cmd((const char *)HOST, PORT, __KITTY_GET_COLORS_CMD__));
  struct StringFNStrings Lines         = stringfn_split_lines(kitty_colors);
  struct StringFNStrings LineWords;

  for (size_t i = 0; i < (size_t)Lines.count; i++) {
    LineWords = stringfn_split_words(Lines.strings[i]);
    if (LineWords.count != 2 || strlen(LineWords.strings[0]) < 1) {
      continue;
    }
    vector_push(color_types, strdup(LineWords.strings[0]));
  }
  if (LineWords.count > 0) {
    stringfn_release_strings_struct(LineWords);
  }
  if (Lines.count > 0) {
    stringfn_release_strings_struct(Lines);
  }
  return(color_types);
}

char *kitty_get_color(const char *COLOR_TYPE, const char *HOST, const int PORT){
  char                   *COLOR        = NULL;
  char                   *kitty_colors = kitty_cmd_data(kitty_tcp_cmd((const char *)HOST, PORT, __KITTY_GET_COLORS_CMD__));
  struct StringFNStrings Lines         = stringfn_split_lines(kitty_colors);
  struct StringFNStrings LineWords;

  for (size_t i = 0; i < (size_t)Lines.count; i++) {
    LineWords = stringfn_split_words(Lines.strings[i]);
    if (LineWords.count != 2) {
      continue;
    }
    if (strcmp(COLOR_TYPE, LineWords.strings[0]) == 0) {
      COLOR = strdup(LineWords.strings[1]);
      break;
    }
  }
  if (LineWords.count > 0) {
    stringfn_release_strings_struct(LineWords);
  }
  if (Lines.count > 0) {
    stringfn_release_strings_struct(Lines);
  }
  return(COLOR);
}

char *kitty_cmd_data(const char *CMD_OUTPUT){
  JSON_Value  *V = json_parse_string(CMD_OUTPUT);
  JSON_Object *O = json_value_get_object(V);

  assert(json_object_get_boolean(O, "ok") == 1);
  char *CMD_DATA = json_object_get_string(O, "data");

  return(CMD_DATA);
}

char *kitty_tcp_cmd(const char *HOST, const int PORT, const char *KITTY_MSG){
  socket99_config cfg = { .host = HOST, .port = PORT };
  socket99_result res;

  if (!socket99_open(&cfg, &res)) {
    fprintf(stderr, "Failed to connect to kitty @ %s:%d.\n", cfg.host, cfg.port);
    return(NULL);
  }
  struct StringBuffer *SB = stringbuffer_new_with_options(KITTY_TCP_BUFFER_SIZE, true);
  size_t              total_sent = 0, msg_size = strlen(KITTY_MSG), recvd = 0;

  do {
    total_sent += send(res.fd, KITTY_MSG, strlen(KITTY_MSG), 0);
  } while (total_sent < msg_size);
  do {
    char *buf = calloc(KITTY_TCP_BUFFER_SIZE + 1, 1);
    recvd = 0;
    recvd = recv(res.fd, buf, KITTY_TCP_BUFFER_SIZE, 0);
    if (recvd < 1) {
      break;
    }
    buf[recvd] = '\0';
    stringbuffer_append_string(SB, buf);
    free(buf);
    if ((int)(buf[strlen(buf) - 1]) == 92) {
      if ((int)(buf[strlen(buf) - 2]) == 27) {
        recvd = -1;
        close(res.fd);
        break;
      }
    }
  }while (recvd > 0);
  close(res.fd);
  char *KITTY_RESPONSE = calloc(stringbuffer_get_content_size(SB) + 1, 1);

  strncpy(KITTY_RESPONSE, stringbuffer_to_string(SB) + 12, stringbuffer_get_content_size(SB) - 3);
  stringbuffer_release(SB);
  return(KITTY_RESPONSE);
}

void kitty_command(const char *HOST, const int PORT, const char *KITTY_MSG){
  socket99_config cfg = { .host = HOST, .port = PORT };
  socket99_result res;

  if (!socket99_open(&cfg, &res)) {
    fprintf(stderr, "Failed to connect to kitty @ %s:%d\n", cfg.host, cfg.port);
    return;
  }
  struct StringBuffer *SB = stringbuffer_new_with_options(KITTY_TCP_BUFFER_SIZE, true);
  size_t              total_sent = 0, msg_size = strlen(KITTY_MSG), recvd = 0;

  do {
    total_sent += send(res.fd, KITTY_MSG, strlen(KITTY_MSG), 0);
  } while (total_sent < msg_size);
  do {
    char *buf = calloc(KITTY_TCP_BUFFER_SIZE + 1, 1);
    recvd = 0;
    recvd = recv(res.fd, buf, KITTY_TCP_BUFFER_SIZE, 0);
    if (recvd < 1) {
      break;
    }
    buf[recvd] = '\0';
    stringbuffer_append_string(SB, buf);
    free(buf);
    if ((int)(buf[strlen(buf) - 1]) == 92) {
      if ((int)(buf[strlen(buf) - 2]) == 27) {
        recvd = -1;
        close(res.fd);
        break;
      }
    }
  }while (recvd > 0);
  close(res.fd);
  char *Nb = calloc(stringbuffer_get_content_size(SB) + 1, 1);

  strncpy(Nb, stringbuffer_to_string(SB) + 12, stringbuffer_get_content_size(SB) - 3);
  stringbuffer_release(SB);
  JSON_Value *V = json_parse_string(Nb);

  free(Nb);
  JSON_Object *O0 = json_value_get_object(V);

  assert(json_object_get_boolean(O0, "ok") == 1);
  JSON_Value *S0V = json_parse_string(json_object_get_string(O0, "data"));
  JSON_Array *A0  = json_value_get_array(S0V);

  assert(json_array_get_count(A0) > 0);
  assert(json_type(json_array_get_value(A0, 0)) == 4);
  JSON_Object *V101 = json_value_get_object(json_array_get_value(A0, 0));

  assert(json_object_get_count(V101) > 0);
  JSON_Value *V_WM_NAME = json_object_get_value(V101, "wm_name");
  JSON_Value *V_ID      = json_object_get_value(V101, "id");

  assert(json_type(V_WM_NAME) == 2);
  assert(json_type(V_ID) == 3);
  assert(json_value_get_string(V_WM_NAME) != NULL);
  assert(json_value_get_number(V_ID) > 0);
  JSON_Value *V_IS_FOCUSED = json_object_get_value(V101, "is_focused");

  assert(json_type(V_IS_FOCUSED) == 6);
  JSON_Value *V_TABS = json_object_get_value(V101, "tabs");
  JSON_Array *TABS   = json_value_get_array(V_TABS);

  assert(json_array_get_count(TABS) > 0);
  for (size_t ii = 0; ii < json_array_get_count(TABS); ii++) {
    JSON_Value  *TABv = json_array_get_value(TABS, ii);
    assert(json_type(TABv) == 4);
    JSON_Object *TAB = json_value_get_object(TABv);
    assert(json_object_get_count(TAB) == 9);
    assert(json_object_has_value_of_type(TAB, "id", JSONNumber));
    assert(json_object_has_value_of_type(TAB, "is_focused", JSONBoolean));
    assert(json_object_has_value_of_type(TAB, "title", JSONString));
    assert(json_object_has_value_of_type(TAB, "layout", JSONString));
    assert(json_object_has_value_of_type(TAB, "layout_state", JSONObject));
    assert(json_object_has_value_of_type(TAB, "layout_opts", JSONObject));
    assert(json_object_has_value_of_type(TAB, "windows", JSONArray));
    assert(json_object_has_value_of_type(TAB, "enabled_layouts", JSONArray));
    assert(json_object_has_value_of_type(TAB, "active_window_history", JSONArray));
    //dbg(json_object_get_string(TAB, "title"), %s);
    //  dbg(json_object_get_boolean(TAB, "is_focused"), %d);
//    dbg(json_object_get_string(TAB, "layout"), %s);
    for (size_t iii = 0; iii < json_object_get_count(TAB); iii++) {
      // dbg(json_object_get_name(TAB, iii), %s);
    }
  }
} /* kitty_command */

struct Vector *connect_kitty_processes(__attribute__((unused)) struct Vector *KittyProcesses_v){
  struct Vector *ConnectedKittyProcesses_v = vector_new();

  return(ConnectedKittyProcesses_v);
} /* connect_kitty_processes */

struct Vector *get_kitty_listen_ons(){
  struct djbhash kitty_listen_ons_h;

  djbhash_init(&kitty_listen_ons_h);
  struct Vector *kitty_listen_ons = vector_new();
  struct Vector *kitty_pids_v     = get_kitty_pids();

  for (size_t i = 0; i < vector_size(kitty_pids_v); i++) {
    kitty_process_t *KP = get_kitty_process_t((size_t)vector_get(kitty_pids_v, i));
    if (KP->listen_on == NULL || strcmp(KP->listen_on, " ") == 0 || strcmp(KP->listen_on, "") == 0) {
      continue;
    }
    if (strlen(KP->listen_on) > 2) {
      if (djbhash_find(&kitty_listen_ons_h, KP->listen_on) == NULL) {
        djbhash_set(&kitty_listen_ons_h, KP->listen_on, &KP->listen_on, DJBHASH_STRING);
        vector_push(kitty_listen_ons, KP->listen_on);
      }
    }
  }
  djbhash_destroy(&kitty_listen_ons_h);
  return(kitty_listen_ons);
}

int get_kitty_pid_windowid(int PID){
  struct Vector *v = get_child_pids(PID);

  for (size_t i = 0; i < vector_size(v); i++) {
    int           pid = (int)(size_t)vector_get(v, i);
    struct Vector *PE = get_process_env(pid);
    for (size_t ii = 0; ii < vector_size(PE); ii++) {
      process_env_t *E = (process_env_t *)(vector_get(PE, ii));
      //printf("pid:%d, %s=>%s\n",pid,E->key,E->val);
      if (strcmp(E->key, "WINDOWID") == 0 && atoi(E->val) > 0) {
        return(atoi(E->val));
      }
    }
  }
  return(-1);
}

static bool vector_contains_pid(struct Vector *pids_v, int pid){
  for (size_t i = 0; i < vector_size(pids_v); i++) {
    if ((size_t)pid == (size_t)vector_get(pids_v, i)) {
      return(true);
    }
  }
  return(false);
}

int get_pid_kitty_pid(int pid){
  struct Vector *PE = get_process_env(pid);

  for (size_t ii = 0; ii < vector_size(PE); ii++) {
    process_env_t *E = (process_env_t *)(vector_get(PE, ii));
    if (strcmp(E->key, "KITTY_PID") == 0) {
      return(atoi(E->val));
    }
  }
  return(-1);
}

struct Vector *get_kittys(){
  struct Vector *v = vector_new(), *pids_v = get_kitty_pids();

  for (size_t i = 0; i < vector_size(pids_v); i++) {
    struct kitty_t *k = calloc(1, sizeof(struct kitty_t));
    k->pid = (pid_t)(size_t)vector_get(pids_v, i);
    vector_push(v, (void *)k);
  }
  return(v);
}
struct Vector *get_kitty_pids(){
  struct Vector *pids_v       = get_all_processes();
  struct Vector *kitty_pids_v = vector_new();

  for (size_t i = 0; i < vector_size(pids_v); i++) {
    int pid = (int)(long long)vector_get(pids_v, i);
    if (vector_contains_pid(kitty_pids_v, pid) == true) {
      continue;
    }
    get_process_cmdline(pid);
    int kp = get_pid_kitty_pid(pid);
    if (kp > 1 && (vector_contains_pid(kitty_pids_v, kp) == false)) {
      vector_push(kitty_pids_v, (void *)(size_t)kp);
    }
  }
  return(kitty_pids_v);
}

#define DEBUG_GET_KITTY_PROCESS_T    false

static bool kitty_pid_is_kitty_process(int pid){
  struct Vector *e;

  e = get_process_env(pid);
  if (!e) {
    return(false);
  }
  for (size_t ii = 0; ii < vector_size(e); ii++) {
    process_env_t *E = (process_env_t *)(vector_get(e, ii));
    if (strcmp(E->key, "KITTY_INSTALLATION_DIR") == 0
        || (strcmp(E->key, "KITTY_LISTEN_ON") == 0)
        || (strcmp(E->key, "KITTY_PID") == 0)
        || (strcmp(E->key, "KITTY_WINDOW_ID") == 0)
        ) {
      return(true);
    }
  }
  return(false);
}

kitty_process_t *get_kitty_process_t(const size_t PID){
  kitty_process_t *KP = malloc(sizeof(kitty_process_t));

  assert(KP != NULL);
  KP->pid = PID;
  struct Vector *PE = get_process_env(KP->pid);

  KP->window_id   = -1;
  KP->listen_on   = NULL;
  KP->install_dir = NULL;
  for (size_t ii = 0; ii < vector_size(PE); ii++) {
    process_env_t *E = (process_env_t *)(vector_get(PE, ii));
    if (DEBUG_GET_KITTY_PROCESS_T) {
      fprintf(stderr,
              AC_YELLOW "#%lu> %s->%s\n" AC_RESETALL,
              ii, E->key, E->val);
    }
    if (strcmp(E->key, "KITTY_INSTALLATION_DIR") == 0) {
      KP->install_dir = strdup(E->val);
    }
    if (strcmp(E->key, "KITTY_LISTEN_ON") == 0) {
      KP->listen_on = strdup(E->val);
    }
    if (strcmp(E->key, "KITTY_PID") == 0) {
      KP->pid = atoi(E->val);
    }
    if (strcmp(E->key, "KITTY_WINDOW_ID") == 0) {
      KP->window_id = atoi(E->val);
    }
    free(E->key);
    free(E->val);
  }
  vector_release(PE);

  return(KP);
}

struct Vector *get_kitty_procs(const char *KITTY_LS_RESPONSE){
  struct Vector *kitty_procs_v = vector_new();
  JSON_Value    *V             = json_parse_string(KITTY_LS_RESPONSE);
  JSON_Array    *A             = json_value_get_array(V);

  assert(json_array_get_count(A) > 0);
  JSON_Object *kitty_process_o;
  JSON_Value  *kitty_process_v;
  JSON_Value  *kitty_tabs_v;
  JSON_Array  *kitty_tabs_a;
  JSON_Value  *kitty_tab_v;
  JSON_Object *kitty_tab_o;
  JSON_Value  *kitty_windows_v;
  JSON_Array  *kitty_windows_a;
  JSON_Value  *kitty_foreground_process_v;
  JSON_Array  *kitty_foreground_process_a;
  JSON_Value  *kitty_cmdline_v;
  JSON_Object *kitty_cmdline_o;
  JSON_Array  *kitty_fg_cmdline_a;
  JSON_Value  *kitty_fg_cmdline_v;
  JSON_Object *kitty_fg_cmdline_o;
  JSON_Value  *kitty_window_v;
  JSON_Object *kitty_window_o;

  for (size_t kitty_process_index = 0; kitty_process_index < json_array_get_count(A); kitty_process_index++) {
    kitty_process_v = json_array_get_value(A, kitty_process_index);
    kitty_process_o = json_value_get_object(kitty_process_v);
    struct kitty_proc_t *kp = calloc(1, sizeof(struct kitty_proc_t));
    kp->id         = json_value_get_number(json_object_get_value(kitty_process_o, "id"));
    kp->window_id  = json_value_get_number(json_object_get_value(kitty_process_o, "platform_window_id"));
    kp->is_focused = json_value_get_boolean(json_object_get_value(kitty_process_o, "is_focused"));
    kp->tabs_qty   = json_array_get_count(json_value_get_array(json_object_get_value(kitty_process_o, "tabs")));
    kp->tabs_v     = vector_new();
    kitty_tabs_v   = json_object_get_value(kitty_process_o, "tabs");
    kitty_tabs_a   = json_value_get_array(kitty_tabs_v);
    for (size_t kitty_tab_index = 0; kitty_tab_index < (size_t)kp->tabs_qty; kitty_tab_index++) {
      kitty_tab_v = json_array_get_value(kitty_tabs_a, kitty_tab_index);
      kitty_tab_o = json_value_get_object(kitty_tab_v);
      struct kitty_tab_t *kt = calloc(1, sizeof(struct kitty_tab_t));
      kt->id          = json_object_get_number(kitty_tab_o, "id");
      kt->title       = json_object_get_string(kitty_tab_o, "title");
      kt->is_focused  = json_object_get_boolean(kitty_tab_o, "is_focused");
      kt->layout      = json_object_get_string(kitty_tab_o, "layout");
      kt->windows_qty = json_array_get_count(json_value_get_array(json_object_get_value(kitty_tab_o, "windows")));
      kt->windows_v   = vector_new();
      kitty_windows_v = json_object_get_value(kitty_tab_o, "windows");
      kitty_windows_a = json_value_get_array(kitty_windows_v);
      for (size_t kitty_window_index = 0; kitty_window_index < (size_t)kt->windows_qty; kitty_window_index++) {
        kitty_window_v             = json_array_get_value(kitty_windows_a, kitty_window_index);
        kitty_window_o             = json_value_get_object(kitty_window_v);
        kitty_foreground_process_v = json_object_get_value(kitty_window_o, "foreground_processes");
        kitty_foreground_process_a = json_value_get_array(kitty_foreground_process_v);
        struct kitty_window_t *kw = calloc(1, sizeof(struct kitty_window_t));
        kw->foreground_processes_qty = json_array_get_count(kitty_foreground_process_a);
        kw->foreground_processes     = vector_new();
        kw->id                       = json_object_get_number(kitty_window_o, "id");
        kw->lines                    = json_object_get_number(kitty_window_o, "lines");
        kw->columns                  = json_object_get_number(kitty_window_o, "columns");
        kw->pid                      = json_object_get_number(kitty_window_o, "pid");
        kw->is_focused               = json_object_get_boolean(kitty_window_o, "is_focused");
        kw->cwd                      = json_object_get_string(kitty_window_o, "cwd");
        kw->title                    = json_object_get_string(kitty_window_o, "title");
        for (size_t kitty_cmdline_index = 0; kitty_cmdline_index < (size_t)kw->foreground_processes_qty; kitty_cmdline_index++) {
          kitty_cmdline_v    = json_array_get_value(kitty_foreground_process_a, kitty_cmdline_index);
          kitty_cmdline_o    = json_value_get_object(kitty_cmdline_v);
          kitty_fg_cmdline_v = json_object_get_value(kitty_cmdline_o, "cmdline");
          kitty_fg_cmdline_o = json_value_get_object(kitty_fg_cmdline_v);
          kitty_fg_cmdline_a = json_value_get_array(kitty_fg_cmdline_v);
          for (size_t kitty_fg_cmdline_index = 0; kitty_fg_cmdline_index < json_array_get_count(kitty_fg_cmdline_a); kitty_fg_cmdline_index++) {
            //kitty_cmdline_s = json_array_get_string(kitty_fg_cmdline_a, kitty_fg_cmdline_index);
          }
        }
        vector_push(kt->windows_v, kw);
      }
      vector_push(kp->tabs_v, kt);
    }
    vector_push(kitty_procs_v, kp);
  }
  return(kitty_procs_v);
} /* get_kitty_procs */

bool kitty_draw_image(void){
  int wrote = fprintf(stdout, "%s\n", KITTY_ESC_DRAW_IMAGE);

  return((wrote > 0) ? true : false);
}

bool kitty_clear_screen(void){
  int wrote = fprintf(stdout, "%s", KITTY_ESC_CODE_CLEAR);

  return((wrote > 0) ? true : false);
}
