#define DEBUG_MODE                false
#define DEBUG_KITTEN_LISTEN_ON    false
#include "dbg.h"
//#include "debug-memory/debug_memory.h"
#include "print.h"
#include "process-test.h"
#define CT_STOP_AND_DEBUG(COLOR)    { do {                                                                             \
                                        PRINT(AC_RESETALL "\t" COLOR "Duration: " AC_RESETALL, ct_stop(__FUNCTION__)); \
                                      }while (0); }
const char KITTY_LS_CMD[]                        = "\x1bP@kitty-cmd{\"cmd\":\"ls\",\"version\":[0,25,2],\"no_response\":false}\x1b\\";
const char KITTY_LS_ALL_ENV_CMD[]                = "\x1bP@kitty-cmd{\"cmd\":\"ls\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"all_env_vars\":true}}\x1b\\";
const char KITTY_QUERY_TERMINAL_CMD[]            = "\x1bP@kitty-cmd{\"cmd\":\"kitten\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"kitten\":\"query_terminal\"}}\x1b\\";
const char KITTY_LS_NO_ENV_CMD[]                 = "\x1bP@kitty-cmd{\"cmd\":\"ls\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"all_env_vars\":false}}\x1b\\";
const char KITTY_GET_TEXT[]                      = "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false}\x1b\\";
const char KITTY_GET_ANSI_TEXT[]                 = "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":true}}\x1b\\";
const char KITTY_GET_ANSI_CURSOR_TEXT[]          = "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":true,\"cursor\":false}}\x1b\\";
const char KITTY_GET_ANSI_CURSOR_ALL_TEXT[]      = "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":true,\"cursor\":false,\"extent\":\"all\"}}\x1b\\";
const char KITTY_GET_ANSI_CURSOR_SCREEN_TEXT[]   = "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":true,\"cursor\":false,\"extent\":\"screen\"}}\x1b\\";
const char KITTY_GET_ANSI_CURSOR_LAST_CMD_TEXT[] = "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":true,\"cursor\":false,\"extent\":\"last_non_empty_output\"}}\x1b\\";
const char KITTY_GET_NO_ANSI_TEXT[]              = "\x1bP@kitty-cmd{\"cmd\":\"get-text\",\"version\":[0,25,2],\"no_response\":false,\"payload\":{\"ansi\":false}}\x1b\\";


TEST t_pid_cwd(void){
  ct_start(NULL);
  char *cwd = get_process_cwd((int)getpid());

  ASSERT_NEQ(cwd, NULL);
  ASSERT_GTE(strlen(cwd), 0);
  dbg(cwd, %s);
  if (cwd) {
    free(cwd);
  }
  CT_STOP_AND_DEBUG(AC_BLUE);
  PASS();
}


TEST t_get_process_cmdline(void){
  ct_start(NULL);
  struct Vector *cmdline_v = get_process_cmdline((int)getpid());

  ASSERT_NEQ(cmdline_v, NULL);
  size_t CMDS_QTY = vector_size(cmdline_v);

  ASSERT_GTE(CMDS_QTY, 1);
  for (size_t i = 0; i < CMDS_QTY; i++) {
    char *CMD = (char *)vector_get(cmdline_v, i);
    ASSERT_NEQ(CMD, NULL);
    ASSERT_GTE(strlen(CMD), 0);
    dbg(CMD, %s);
  }
  dbg(CMDS_QTY, %lu);
  vector_release(cmdline_v);
  CT_STOP_AND_DEBUG(AC_RED);
  PASS();
}


TEST t_kitty_pids_connect(void){
  ct_start(NULL);
  struct Vector *KittyProcesses_v = get_kitty_pids();
  dbg(vector_size(KittyProcesses_v), %lu);
  ASSERT_GTE(vector_size(KittyProcesses_v), 0);
  struct Vector *ConnectedKittyProcess_v = connect_kitty_processes(KittyProcesses_v);
  dbg(vector_size(ConnectedKittyProcess_v), %lu);
  CT_STOP_AND_DEBUG(AC_RED);
  PASS();
} /* t_kitty_pids */


TEST t_kitty_listen_ons(void){
  struct Vector *kitty_listen_ons = get_kitty_listen_ons();

  for (size_t i = 0; i < vector_size(kitty_listen_ons); i++) {
    fprintf(stderr,
            AC_RESETALL AC_BLUE "Listen on #%lu/%lu: %s\n" AC_RESETALL,
            i + 1, vector_size(kitty_listen_ons),
            vector_get(kitty_listen_ons, i)
            );
    char              *LO  = vector_get(kitty_listen_ons, i);
    kitty_listen_on_t *KLO = parse_kitten_listen_on(LO);
    if (DEBUG_KITTEN_LISTEN_ON) {
      dbg(KLO->protocol, %s);
      dbg(KLO->host, %s);
      dbg(KLO->port, %d);
    }
    if (strcmp(KLO->protocol, "tcp") == 0) {
/*
 *    char *kitty_colors = kitty_cmd_data(kitty_tcp_cmd((const char *)KLO->host, KLO->port, __KITTY_GET_COLORS_CMD__));
 *    //dbg(kitty_colors, %s);
 *    dbg(strlen(kitty_colors), %lu);
 *
 *    char *kitty_text = kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_GET_TEXT);
 * //    dbg(kitty_text, %s);
 *    dbg(strlen(kitty_text), %lu);
 *
 *    char *kitty_ls_no_env = kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_LS_NO_ENV_CMD);
 *   // dbg(kitty_ls_no_env, %s);
 *    dbg(strlen(kitty_ls_no_env), %lu);
 *
 *    char *kitty_ls_all_env = kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_LS_ALL_ENV_CMD);
 * //   dbg(kitty_ls_all_env, %s);
 *    dbg(strlen(kitty_ls_all_env), %lu);
 *    char *kitty_text_no_ansi = kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_GET_NO_ANSI_TEXT);
 *   // dbg(kitty_text_no_ansi, %s);
 *    dbg(strlen(kitty_text_no_ansi), %lu);
 *
 *    char *kitty_text_ansi_cursor = kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_GET_ANSI_CURSOR_TEXT);
 *   // dbg(kitty_text_ansi_cursor, %s);
 *    dbg(strlen(kitty_text_ansi_cursor), %lu);
 *
 *    char *kitty_text_ansi = kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_GET_ANSI_TEXT);
 *   // dbg(kitty_text_ansi, %s);
 *    dbg(strlen(kitty_text_ansi), %lu);
 *
 *
 *    char *kitty_text_ansi_cursor_last_cmd = kitty_cmd_data(kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_GET_ANSI_CURSOR_LAST_CMD_TEXT));
 *
 *    dbg(strlen(kitty_text_ansi_cursor_last_cmd), %lu);
 *    char *kitty_text_ansi_cursor_screen = kitty_cmd_data(kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_GET_ANSI_CURSOR_SCREEN_TEXT));
 *    // dbg(kitty_text_ansi_cursor_screen, %s);
 *    dbg(strlen(kitty_text_ansi_cursor_screen), %lu);
 */

      char                   *kitty_text = kitty_cmd_data(kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_GET_ANSI_CURSOR_SCREEN_TEXT));
      struct StringFNStrings Lines       = stringfn_split_lines(kitty_text);
      fprintf(stdout, AC_RESETALL "\nkitty_text:\n%s" AC_RESETALL "\n", kitty_text);
      dbg(strlen(kitty_text), %lu);
      dbg(Lines.count, %d);


      struct Vector *COLOR_TYPES_V = kitty_get_color_types(KLO->host, KLO->port);
      dbg(vector_size(COLOR_TYPES_V), %lu);
      for (size_t ci = 0; ci < vector_size(COLOR_TYPES_V); ci++) {
//          PRINT("#",ci,"/", (size_t)vector_size(COLOR_TYPES), (char *)vector_get(COLOR_TYPES,ci), "=>", (char *)kitty_get_color(vector_get(COLOR_TYPES,ci),KLO->host,KLO->port));
      }
      vector_release(COLOR_TYPES_V);

      char *CURSOR_COLOR = kitty_get_color("cursor", KLO->host, KLO->port);
      dbg(CURSOR_COLOR, %s);

      char *BACKGROUND_COLOR = kitty_get_color("background", KLO->host, KLO->port);
      dbg(BACKGROUND_COLOR, %s);

      char *kitty_ls = kitty_cmd_data(kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_LS_CMD));
      dbg(strlen(kitty_ls), %lu);

      struct Vector *kitty_procs_v = get_kitty_procs(kitty_ls);
      dbg(vector_size(kitty_procs_v), %lu);
      for (size_t ci = 0; ci < vector_size(kitty_procs_v); ci++) {
        struct kitty_proc_t *kp = (struct kitty_proc_t *)vector_get(kitty_procs_v, ci);
        PRINT(
          "#", ci, "/", (size_t)vector_size(kitty_procs_v),
          "id:", (int)((struct kitty_proc_t *)vector_get(kitty_procs_v, ci))->id,
          "window id:", (int)((struct kitty_proc_t *)vector_get(kitty_procs_v, ci))->window_id,
          "# tabs:", (int)((struct kitty_proc_t *)vector_get(kitty_procs_v, ci))->tabs_qty,
          "is_focused:", (bool)((struct kitty_proc_t *)vector_get(kitty_procs_v, ci))->is_focused
          );
        for (size_t t = 0; t < vector_size(kp->tabs_v); t++) {
          struct kitty_tab_t *kt = (struct kitty_tab_t *)vector_get(kp->tabs_v, t);
          PRINT(
            "  >tab #", t,
            "id:", (int)kt->id,
            "title:", (char *)kt->title,
            "layout:", (char *)kt->layout,
            "is_focused:", (bool)kt->is_focused,
            "# windows:", (int)kt->windows_qty
            );
          for (size_t w = 0; w < vector_size(kt->windows_v); w++) {
            struct kitty_window_t *kw = (struct kitty_window_t *)vector_get(kt->windows_v, w);
            PRINT(
              "    >window #", w,
              "id:", (int)kw->id,
              "lines:", (int)kw->lines,
              "is_focused:", (bool)kw->is_focused,
              "columns:", (int)kw->columns,
              "pid:", (int)kw->pid,
              "title:", (char *)kw->title,
              "cwd:", (char *)kw->cwd,
              "foreground_processes_qty:", (int)kw->foreground_processes_qty
              );
            for (size_t c = 0; c < vector_size(kw->foreground_processes); c++) {
              PRINT("          >", vector_get(kw->foreground_processes, c));
            }
            free(kw);
          }
          free(kt);
        }
        free(kp);
      }
    }

    /*
     * char *kitty_query_terminal = kitty_tcp_cmd((const char *)KLO->host, KLO->port, KITTY_QUERY_TERMINAL_CMD);
     * dbg(kitty_query_terminal, %s);
     * dbg(strlen(kitty_query_terminal), %lu);
     */
  }
  ASSERT_GTE(vector_size(kitty_listen_ons), 0);
  PASS();
} /* t_kitty_listen_ons */


TEST t_kitty_pids(void){
  ct_start(NULL);
  struct Vector *kitty_pids_v = get_kitty_pids();
  dbg(vector_size(kitty_pids_v), %lu);
  ASSERT_GTE(vector_size(kitty_pids_v), 0);
  for (size_t i = 0; i < vector_size(kitty_pids_v); i++) {
    kitty_process_t *KP = get_kitty_process_t(vector_get(kitty_pids_v, i));
    if (KP->listen_on == NULL) {
      continue;
    }
    fprintf(stdout,
            "\t"
            AC_RESETALL AC_BLUE "Kitty PID #%lu/%lu: " AC_RESETALL AC_GREEN AC_REVERSED "%d" AC_RESETALL
            AC_RESETALL "\n\t\t" AC_RED "|Window #" AC_RESETALL AC_GREEN AC_REVERSED "%d" AC_RESETALL
            AC_RESETALL "\n\t\t" AC_RED "|Listen on:" AC_RESETALL AC_GREEN AC_REVERSED "%s" AC_RESETALL
            "\n",
            i + 1, vector_size(kitty_pids_v),
            KP->pid,
            KP->window_id,
            KP->listen_on
            );
  }
  CT_STOP_AND_DEBUG(AC_RED);
  PASS();
} /* t_kitty_pids */


TEST t_pids_iterate(void){
  ct_start(NULL);
  struct Vector *pids_v = get_all_processes();
  ASSERT_NEQ(pids_v, NULL);
  size_t        PIDS_QTY = vector_size(pids_v);
  for (size_t i = 0; i < PIDS_QTY; i++) {
    int pid = (int)(long long)vector_get(pids_v, i);
    if (DEBUG_MODE) {
      dbg(pid, %d);
    }
    if (pid <= 1) {
      continue;
    }
    char *cwd = get_process_cwd(pid);
    if (cwd == NULL) {
      continue;
    }
    struct Vector *cmdline_v = get_process_cmdline(pid);
    if (cmdline_v == NULL) {
      if (cwd) {
        free(cwd);
      }
      continue;
    }
    struct Vector *PE = get_process_env(pid);
    if (PE == NULL) {
      if (cwd) {
        free(cwd);
      }
      continue;
    }
    size_t CMDS_QTY = vector_size(cmdline_v);
    size_t ENV_QTY  = vector_size(PE);
    if (DEBUG_MODE) {
      dbg(CMDS_QTY, %lu);
      dbg(ENV_QTY, %lu);
      dbg(cwd, %s);
    }
    for (size_t i = 0; i < ENV_QTY; i++) {
      char *ENV_KEY = ((process_env_t *)vector_get(PE, i))->key,
           *ENV_VAL = ((process_env_t *)vector_get(PE, i))->val;
      if (DEBUG_MODE) {
        dbg(ENV_KEY, %s);
        dbg(ENV_VAL, %s);
      }
      if (ENV_KEY) {
        free(ENV_KEY);
      }
      if (ENV_VAL) {
        free(ENV_VAL);
      }
      //  free(((process_env_t *)vector_get(PE, i)));
    }
    if (cwd) {
      free(cwd);
    }
    vector_release(PE);
    continue;
    vector_release(cmdline_v);
  }
  vector_release(pids_v);
  CT_STOP_AND_DEBUG(AC_CYAN);
} /* t_pids_iterate */


TEST t_pids(void){
  ct_start(NULL);
  struct Vector *pids_v = get_all_processes();

  ASSERT_NEQ(pids_v, NULL);
  size_t PIDS_QTY = vector_size(pids_v);

  ASSERT_GTE(PIDS_QTY, 0);
  for (size_t i = 0; i < PIDS_QTY; i++) {
    long long PID = (long long)vector_get(pids_v, i);
    ASSERT_GTE(PID, 0);
    if (DEBUG_MODE) {
      dbg(PID, %llu);
    }
  }
  dbg(PIDS_QTY, %lu);
  vector_release(pids_v);
  char *dur = ct_stop(__FUNCTION__);
  dbg(dur, %s);

  PASS();
}


TEST t_process_env(void){
  ct_start(NULL);
  struct Vector *PE = get_process_env((int)getpid());

  ASSERT_NEQ(PE, NULL);
  size_t ENV_QTY = vector_size(PE);

  ASSERT_GTE(ENV_QTY, 0);
  for (size_t i = 0; i < ENV_QTY; i++) {
    char *ENV_KEY = ((process_env_t *)vector_get(PE, i))->key,
         *ENV_VAL = ((process_env_t *)vector_get(PE, i))->val;
    ASSERT_GTE(strlen(ENV_KEY), 0);
    ASSERT_GTE(strlen(ENV_VAL), 0);
    if (DEBUG_MODE) {
      dbg(ENV_KEY, %s);
      dbg(ENV_VAL, %s);
    }
    if (ENV_KEY) {
      free(ENV_KEY);
    }
    if (ENV_VAL) {
      free(ENV_VAL);
    }
    free(((process_env_t *)vector_get(PE, i)));
  }
  dbg(ENV_QTY, %lu);
  char *dur = ct_stop(__FUNCTION__);
  dbg(dur, %s);
  vector_release(PE);
  PASS();
}


SUITE(s_process){
  ct_start(NULL);
  RUN_TEST(t_process_env);
  RUN_TEST(t_pids);
  RUN_TEST(t_pid_cwd);
  RUN_TEST(t_get_process_cmdline);
  RUN_TEST(t_pids_iterate);
  RUN_TEST(t_kitty_pids);
  RUN_TEST(t_kitty_listen_ons);
  //RUN_TEST(t_kitty_pids_connect);
  CT_STOP_AND_DEBUG(AC_RED_BLACK);
  PASS();
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  ct_start(NULL);
  RUN_SUITE(s_process);
  CT_STOP_AND_DEBUG(AC_BLUE_BLACK AC_ITALIC);
  // print_allocated_memory();
  GREATEST_MAIN_END();
  return(0);
}
