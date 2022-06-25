#define DEBUG_LOGGED_EVENTS    true
#include "keylogger-db.h"
#include "log.h/log.h"
#include <assert.h>
////////////////////////////////////////////////////////
static struct sqldbal_db *db;
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


int keylogger_db_delete(void){
  db_statement_t           db_st = NEW_DB_STATEMENT();
  enum sqldbal_status_code rc;

  rc = sqldbal_stmt_prepare(db,
                            "DELETE FROM events",
                            -1,
                            &db_st.stmt);
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


int keylogger_select_db(void){
  db_statement_t db_st = NEW_DB_STATEMENT();

  db_st.rc = sqldbal_stmt_prepare(db,
                                  "SELECT COUNT(*) FROM events",
                                  -1,
                                  &db_st.stmt);
  ASSERT_DB_STATEMENT(db_st);
  EXEC_AND_ASSERT_DB_STATEMENT(db_st);
  while (sqldbal_stmt_fetch(db_st.stmt) == SQLDBAL_FETCH_ROW) {
    db_st.rc = sqldbal_stmt_column_int64(db_st.stmt, 0, &db_st.qty);
    ASSERT_DB_STATEMENT(db_st);
    printf(
      AC_RESETALL AC_YELLOW ">%llu event rows\n" AC_RESETALL,
      db_st.qty
      );
  }
  db_st.rc = sqldbal_stmt_close(db_st.stmt);
  ASSERT_DB_STATEMENT(db_st);
  return(0);
}


int keylogger_insert_db_row(logged_key_event_t *LOGGED_EVENT){
  db_statement_t db_st = NEW_DB_STATEMENT();
  int64_t        ins_id;

  for (size_t i = 0; i < LOGGED_EVENT->qty; i++) {
    if (DEBUG_LOGGED_EVENTS) {
      log_debug("ts:%lu", LOGGED_EVENT->ts);
      log_debug("kc:%lu", LOGGED_EVENT->key_code);
      log_debug("ks:%s", LOGGED_EVENT->key_string);
      log_debug("type:%s", LOGGED_EVENT->input_type);
      log_debug("mouse:%lux%lu", LOGGED_EVENT->mouse_x, LOGGED_EVENT->mouse_y);
    }
    db_st.rc = sqldbal_stmt_prepare(db,
                                    "INSERT INTO events\
      (ts, key_code, key_string, action, mouse_x, mouse_y, input_type) \
                                    VALUES\
      (?, ?, ?, ?, ?, ?, ?)\
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
    EXEC_AND_ASSERT_DB_STATEMENT(db_st);

    ins_id   = -1;
    db_st.rc = sqldbal_last_insert_id(db, "events_id_seq", &ins_id);
    ASSERT_DB_STATEMENT(db_st);
    db_st.rc = sqldbal_stmt_close(db_st.stmt);
    ASSERT_DB_STATEMENT(db_st);
    log_debug("inserted row #%lu", (size_t)ins_id);
  }
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

