/**************************************************/
#define SQL_SELECT    "SELECT * from people;"
/**************************************************/
/*******************************************/
/*******************************************/
#include "../include/includes.h"
/*******************************************/


static int sql_select_callback(__attribute__((__unused__)) void *NotUsed,
                               int argc, char **argv, char **azColName){
  int i;

  for (i = 0; i < argc; i++) {
    log_info("sql_select_callback> %s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  return(0);
}


int dump_columns(sqlite3_stmt *ppStmt) {
  const unsigned char *name;
  int                 age;

  name = sqlite3_column_text(ppStmt, 0);
  age  = sqlite3_column_int(ppStmt, 1);
  log_info("name: %s age %d", name, age);
  return(0);
}


int sql_select(){
  sqlite3 *db      = NULL;
  char    *zErrMsg = 0;
  int     rc;

  rc = sqlite3_open(DATABASE_FILE, &db);
  if (rc) {
    log_fatal("Can't open database: %s", sqlite3_errmsg(db));
    sqlite3_close(db);
    return(1);
  }
  rc = sqlite3_exec(db, SQL_SELECT, sql_select_callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    log_fatal("SQL error: %s", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  sqlite3_close(db);
  return(0);
}


int sql_select_statement(){
  sqlite3      *db = NULL;
  sqlite3_stmt *ppStmt;
  int          rc = -1, sc, more;

  sc = sqlite3_open(DATABASE_FILE, &db);
  if (sc) {
    log_fatal("sqlite3_open: %s", sqlite3_errmsg(db));
    goto done;
  }

  sc = sqlite3_prepare_v2(db, SQL_SELECT, -1, &ppStmt, NULL);
  if (sc != SQLITE_OK) {
    log_fatal("sqlite3_prepare: %s", sqlite3_errstr(sc));
    goto done;
  }

  more = 1;
  while (more) {
    sc = sqlite3_step(ppStmt);
    switch (sc) {
    case SQLITE_ROW: dump_columns(ppStmt); break;
    case SQLITE_DONE:
      more = 0;
      sc   = sqlite3_finalize(ppStmt);
      if (sc != SQLITE_OK) {
        log_fatal("sqlite3_finalize: %s", sqlite3_errstr(sc));
      }
      break;
    default:
      log_fatal("sqlite3_step: error %d", sc);
      more = 0;
      break;
    }
  }

  rc = 0;

done:
  sqlite3_close(db); // NULL allowed
  return(rc);
} /* sql_select */
