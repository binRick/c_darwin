#define DEFAULT_WINDOW_NAME_PATTERN    "kitty"
#ifndef LOGLEVEL
#define LOGLEVEL                       DEFAULT_LOGLEVEL
#endif
#define DEBUG_LOGGED_EVENTS            false
////////////////////////////////////////////////////////
#include <assert.h>
////////////////////////////////////////////////////////
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "greatest/greatest.h"
#include "keylogger-db/keylogger-db.h"
#include "log.h/log.h"
#include "pasteboard/pasteboard.h"
#include "system-utils/system-utils.h"
#include "window-utils-test/window-utils-test.h"
#include "window-utils/window-utils.h"
////////////////////////////////////////////////////////
static bool exited = false, was_icanon = false;
static int                focused_pid = 0, windows_qty = 0, display_qty = 0;
static struct screen_size ss = { 0, 0, 0, 0 };
static struct sqldbal_db  *db;
static unsigned long      last_qty_checks = 0, last_qty_check_ts = 0, last_clipboard_check_ts = 0,
                          check_qty_interval_ms = 1000,
                          check_clipboard_interval_ms = 2000;
static size_t            recorded_qty = 0, inserted_events_qty = 0, table_size_bytes = 0;
static struct Vector     *pids_v;
static clipboard_event_t CLIPBOARD_EVENT;
#define ASSERT_SQLDB_RESULT()          \
  { do {                               \
      assert(rc == SQLDBAL_STATUS_OK); \
    } while (0); }
#define NEW_DB_STATEMENT()    { \
    .rc   = -1,                 \
    .stmt = NULL,               \
    .qty  = -1,                 \
}
#define ASSERT_DB_STATEMENT(DB_STATEMENT)           \
  { do {                                            \
      assert(DB_STATEMENT.rc == SQLDBAL_STATUS_OK); \
    } while (0); }
#define EXEC_AND_ASSERT_DB_STATEMENT(DB_STATEMENT)        \
  { do {                                                  \
      db_st.rc = sqldbal_stmt_execute(DB_STATEMENT.stmt); \
      ASSERT_DB_STATEMENT(DB_STATEMENT);                  \
    } while (0); }


void __at_exit(void){
  if (exited) {
    return(0);
  }
  exited = true;
  bool ic = seticanon(was_icanon, true);
  fprintf(stdout, AC_SHOW_CURSOR);
  exit(0);
}


int keylogger_db_delete(void){
  db_statement_t db_st = NEW_DB_STATEMENT();

  db_st.rc = sqldbal_stmt_prepare(db, "DELETE FROM events", -1, &db_st.stmt);
  EXEC_AND_ASSERT_DB_STATEMENT(db_st);
  return(0);
}


int keylogger_create_db(void){
  db_statement_t db_st = NEW_DB_STATEMENT();

  db_st.rc = sqldbal_exec(db, "PRAGMA foreign_keys = ON", NULL, NULL);
  ASSERT_DB_STATEMENT(db_st);
  db_st.rc = sqldbal_exec(db,
                          "\
CREATE TABLE IF NOT EXISTS events(\
  _id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT\
, _ts  DATETIME NOT NULL DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime'))\
, ts INTEGER NOT NULL\
, key_code INTEGER NOT NULL\
, key_string VARCHAR(20) NOT NULL\
, action VARCHAR(20) NOT NULL\
, mouse_x INTEGER NOT NULL\
, mouse_y INTEGER NOT NULL\
, input_type VARCHAR(20) NOT NULL\
, windows_qty INTEGER NOT NULL\
, display_qty INTEGER NOT NULL\
, focused_pid INTEGER NOT NULL\
, pids_qty INTEGER NOT NULL\
)",
                          NULL,
                          NULL);
  ASSERT_DB_STATEMENT(db_st);
  return(0);
}


int keylogger_init_db(void){
  db_statement_t db_st = NEW_DB_STATEMENT();

  db_st.rc = sqldbal_open(SQLDBAL_DRIVER_SQLITE, LOCATION, NULL, NULL, NULL, NULL, FLAGS, NULL, 0, &(db));
  ASSERT_DB_STATEMENT(db_st);
  return(0);
}


size_t keylogger_get_db_size(){
  int64_t        page_size = 0, page_count = 0, size_bytes = 0;
  char           sql0[] = "PRAGMA PAGE_SIZE", sql1[] = "PRAGMA PAGE_COUNT";
  db_statement_t db_st = NEW_DB_STATEMENT();

  db_st.rc = sqldbal_stmt_prepare(db, sql0, -1, &db_st.stmt);
  ASSERT_DB_STATEMENT(db_st);
  EXEC_AND_ASSERT_DB_STATEMENT(db_st);
  while (sqldbal_stmt_fetch(db_st.stmt) == SQLDBAL_FETCH_ROW) {
    db_st.rc = sqldbal_stmt_column_int64(db_st.stmt, 0, &page_size);
    ASSERT_DB_STATEMENT(db_st);
  }
  db_st.rc = sqldbal_stmt_close(db_st.stmt);
  ASSERT_DB_STATEMENT(db_st);
  db_st.rc = sqldbal_stmt_prepare(db, sql1, -1, &db_st.stmt);
  ASSERT_DB_STATEMENT(db_st);
  EXEC_AND_ASSERT_DB_STATEMENT(db_st);
  while (sqldbal_stmt_fetch(db_st.stmt) == SQLDBAL_FETCH_ROW) {
    db_st.rc = sqldbal_stmt_column_int64(db_st.stmt, 0, &page_count);
    ASSERT_DB_STATEMENT(db_st);
  }
  db_st.rc = sqldbal_stmt_close(db_st.stmt);
  ASSERT_DB_STATEMENT(db_st);
  size_bytes = (page_size * page_count);
  return(size_bytes);
}


size_t keylogger_get_table_sql(const char *TABLE){
  db_statement_t db_st = NEW_DB_STATEMENT();
  char           *sql  = malloc(1024);

  sprintf(sql, "SELECT sql FROM sqlite_master WHERE name='%s'", TABLE);
  free(sql);
  return(db_st.qty);
}


size_t keylogger_count_table_rows(const char *TABLE){
  db_statement_t db_st = NEW_DB_STATEMENT();
  char           *sql  = malloc(1024);

  sprintf(sql, "SELECT COUNT(*) FROM %s", TABLE);
  db_st.rc = sqldbal_stmt_prepare(db, sql, -1, &db_st.stmt);
  ASSERT_DB_STATEMENT(db_st);
  EXEC_AND_ASSERT_DB_STATEMENT(db_st);
  while (sqldbal_stmt_fetch(db_st.stmt) == SQLDBAL_FETCH_ROW) {
    db_st.rc = sqldbal_stmt_column_int64(db_st.stmt, 0, &db_st.qty);
    ASSERT_DB_STATEMENT(db_st);
  }
  db_st.rc = sqldbal_stmt_close(db_st.stmt);
  ASSERT_DB_STATEMENT(db_st);
  last_qty_checks++;
  free(sql);
  return(db_st.qty);
}


int keylogger_select_db(void){
  db_statement_t db_st = NEW_DB_STATEMENT();

  db_st.rc = sqldbal_stmt_prepare(db,
                                  "SELECT COUNT(*) FROM events",
                                  -1,
                                  &db_st.stmt);
  ASSERT_DB_STATEMENT(db_st);
  EXEC_AND_ASSERT_DB_STATEMENT(db_st);
  ASSERT_DB_STATEMENT(db_st);
  while (sqldbal_stmt_fetch(db_st.stmt) == SQLDBAL_FETCH_ROW) {
    db_st.rc = sqldbal_stmt_column_int64(db_st.stmt, 0, &db_st.qty);
    ASSERT_DB_STATEMENT(db_st);
  }
  db_st.rc = sqldbal_stmt_close(db_st.stmt);
  ASSERT_DB_STATEMENT(db_st);
  recorded_qty = db_st.qty;
  return(0);
}

static size_t        updates_qty = 0;
static bool          initialized = false;
static unsigned long last_ts     = 0;


int keylogger_insert_db_row(logged_key_event_t *LOGGED_EVENT){
  unsigned long cur_ts = timestamp();

  if (last_ts == 0) {
    last_ts = 0;
  }
  if (initialized == false) {
    initialized = true;
    was_icanon  = (seticanon(false, false) == true) ? true : false;
    fprintf(stdout, AC_HIDE_CURSOR);
    fflush(stdout);
    signal(SIGINT, __at_exit);
    signal(SIGTERM, __at_exit);
    signal(SIGQUIT, __at_exit);
    atexit(__at_exit);
  }
  db_statement_t db_st                 = NEW_DB_STATEMENT();
  unsigned long  updated_dur           = cur_ts - last_ts;
  unsigned long  clipboard_updated_dur = cur_ts - last_clipboard_check_ts;

  if (clipboard_updated_dur > check_clipboard_interval_ms) {
    char *clipboard_content = read_clipboard();
    if (clipboard_content == NULL) {
      printf("invalid clipboard content!");
      exit(1);
    }
    CLIPBOARD_EVENT         = encode_clipboard_event(clipboard_content);
    last_clipboard_check_ts = cur_ts;
  }
  if (updated_dur > check_qty_interval_ms) {
    recorded_qty      = keylogger_count_table_rows("events");
    table_size_bytes  = keylogger_get_db_size();
    windows_qty       = get_windows_qty();
    ss                = get_window_size();
    display_qty       = get_display_count();
    focused_pid       = get_frontmost_application();
    pids_v            = get_all_processes();
    last_qty_check_ts = timestamp();
    //focused_t *fp = get_focused_process();
  }
  if (updated_dur > 0) {
    fprintf(stdout,
            AC_CLS
            "\n\t  | Timestamp:     |%lu|"
            "\n\t  | updated ms:    |" AC_BOLD AC_UNDERLINE "%lu" AC_NOUNDERLINE AC_PLAIN "|"
            "\n\t  | keycode:       |%lu|"
            "\n\t  | action:        |%s|"
            "\n\t  | key:           |%s|"
            "\n\t  | type:          |" AC_BOLD AC_UNDERLINE "%s" AC_NOUNDERLINE AC_PLAIN "|"
            "\n\t  | # Down Keys:   |%lu: (%s)|"
            "\n\t  | # rows:        |%lu|"
            "\n\t  | table size:    |" AC_INVERSE "%s" AC_NOINVERSE "|"
            "\n\t  | # windows:     |%d|"
            "\n\t  | # displays:    |%d|"
            "\n\t  | focused pid:   |%d|"
            "\n\t  | # pids:        |%lu|"
            "\n\t  | window size:   |x:%d|y:%d|w:%d|h:%d|"
            "\n\t  | mouse loc:     |x:%lu|y:%lu|"
            "\n\t  | clipboard:     |" AC_BOLD "%s" AC_PLAIN
            "|" AC_UNDERLINE "%.30s" AC_NOUNDERLINE AC_PLAIN " " AC_UNDERLINE "..." AC_NOUNDERLINE
            "|" AC_ITALIC AC_YELLOW "%.30s" AC_RESETALL AC_PLAIN " " AC_UNDERLINE "..." AC_NOUNDERLINE
            "|"
            "%s",
            LOGGED_EVENT->ts, updated_dur,
            LOGGED_EVENT->key_code,
            LOGGED_EVENT->action,
            LOGGED_EVENT->key_string,
            LOGGED_EVENT->input_type,
            vector_size(LOGGED_EVENT->downkeys_v), LOGGED_EVENT->downkeys_csv,
            recorded_qty,
            bytes_to_string(table_size_bytes),
            windows_qty,
            display_qty,
            focused_pid,
            vector_size(pids_v),
            ss.x, ss.y, ss.w, ss.h,
            LOGGED_EVENT->mouse_x, LOGGED_EVENT->mouse_y,
            bytes_to_string(CLIPBOARD_EVENT.raw_size), CLIPBOARD_EVENT.b64, CLIPBOARD_EVENT.raw,
            "\n"
            );
  }
  for (size_t i = 0; i < LOGGED_EVENT->qty; i++) {
    db_st.rc = sqldbal_stmt_prepare(db,
                                    "INSERT INTO events\
      (ts, key_code, key_string, action, mouse_x, mouse_y, input_type, windows_qty, display_qty, focused_pid, pids_qty) \
                                    VALUES\
      (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)\
",
                                    -1,
                                    &db_st.stmt);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_int64(db_st.stmt, 0, LOGGED_EVENT->ts);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_int64(db_st.stmt, 1, LOGGED_EVENT->key_code);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_text(db_st.stmt, 2, LOGGED_EVENT->key_string, -1);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_text(db_st.stmt, 3, LOGGED_EVENT->action, -1);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_int64(db_st.stmt, 4, LOGGED_EVENT->mouse_x);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_int64(db_st.stmt, 5, LOGGED_EVENT->mouse_y);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_text(db_st.stmt, 6, LOGGED_EVENT->input_type, -1);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_int64(db_st.stmt, 7, windows_qty);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_int64(db_st.stmt, 8, display_qty);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_int64(db_st.stmt, 9, focused_pid);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_bind_int64(db_st.stmt, 10, vector_size(pids_v));
    ASSERT_DB_STATEMENT(db_st);
    EXEC_AND_ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_last_insert_id(db, "events_id_seq", &db_st.inserted_id);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_close(db_st.stmt);
    ASSERT_DB_STATEMENT(db_st);
    inserted_events_qty++;
  }
  if (DEBUG_LOGGED_EVENTS) {
    fprintf(stderr,
            AC_RESETALL "last recorded_qty check ts:%lu (%llums ago)" AC_RESETALL
            "\n\t|%lu checks| %lu records|%s Database|"
            "\n\t|pids:	%lu|"
            "\n\t|windows:	%d|"

            , last_qty_check_ts, (long long)(timestamp() - last_qty_check_ts),
            last_qty_checks, recorded_qty,
            bytes_to_string(table_size_bytes),
            vector_size(pids_v),
            windows_qty
            );
  }
  last_ts = cur_ts;
  return(0);
} /* keylogger_insert_db_row */


int keylogger_close_db(void){
  db_statement_t db_st = NEW_DB_STATEMENT();

  db_st.rc = sqldbal_close(db);
  ASSERT_DB_STATEMENT(db_st);
  return(0);
}


int keylogger_db_lifecycle(void){
  assert(keylogger_init_db() == 0);
  assert(keylogger_create_db() == 0);
  assert(keylogger_insert_db_row(&(logged_key_event_t){
    .ts         = timestamp(),
    .qty        = 8,
    .key_code   = 100,
    .key_string = "xxxxxxxx",
    .action     = "YYY",
    .mouse_x    = 123,
    .mouse_y    = 222,
    .input_type = "DUMMY",
  }) == 0);
  assert(keylogger_select_db() == 0);
  assert(keylogger_close_db() == 0);
  return(0);
} /* t_sqldb */

