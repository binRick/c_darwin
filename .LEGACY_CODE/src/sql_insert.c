/*******************************************/
/*******************************************/
#include "../include/includes.h"
/*******************************************/


int reset(sqlite3_stmt *ppStmt) {
  int sc, rc = -1;

  sc = sqlite3_reset(ppStmt);
  if (sc != SQLITE_OK) {
    fprintf(stderr, "sqlite3_reset: %s\n", sqlite3_errstr(sc));
    goto done;
  }

  sc = sqlite3_clear_bindings(ppStmt);
  if (sc != SQLITE_OK) {
    fprintf(stderr, "sqlite3_clear_bindings: %s\n", sqlite3_errstr(sc));
    goto done;
  }

  rc = 0;

done:
  return(rc);
}


int do_insert(sqlite3_stmt *ppStmt, char *name, int age) {
  int sc, rc = -1;

  sc = sqlite3_bind_text(ppStmt, 1, name, -1, SQLITE_TRANSIENT);
  if (sc != SQLITE_OK) {
    fprintf(stderr, "sqlite3_bind_text: %s\n", sqlite3_errstr(sc));
    goto done;
  }

  sc = sqlite3_bind_int(ppStmt, 2, age);
  if (sc != SQLITE_OK) {
    fprintf(stderr, "sqlite3_bind_int: %s\n", sqlite3_errstr(sc));
    goto done;
  }

  sc = sqlite3_step(ppStmt);
  switch (sc) {
  case SQLITE_ROW:
    fprintf(stderr, "more results -- unexpected on insert!\n");
    break;
  case SQLITE_DONE:
    /* normal- reset but do not finalize til all inserts done */
    if (reset(ppStmt) < 0) {
      goto done;
    }
    break;
  default:
    fprintf(stderr, "sqlite3_step: error %s\n", sqlite3_errstr(sc));
    goto done;
    break;
  }

  rc = 0;

done:
  return(rc);
}


int sql_insert(){
  sqlite3_stmt *ppStmt;
  int          rc = -1, sc;
  sqlite3      *db = NULL;
  char         *sql;

  sc = sqlite3_open(DATABASE_FILE, &db);
  if (sc) {
    fprintf(stderr, "sqlite3_open: %s\n", sqlite3_errmsg(db));
    goto done;
  }

  /* prepare statement - we'll substitute values into */
  sql = "insert into people values ($NAME, $AGE);";
  sc  = sqlite3_prepare_v2(db, sql, -1, &ppStmt, NULL);
  if (sc != SQLITE_OK) {
    fprintf(stderr, "sqlite3_prepare: %s\n", sqlite3_errstr(sc));
    goto done;
  }

  /* insert rows */
  if (do_insert(ppStmt, "ben", 8) < 0) {
    goto done;
  }
  if (do_insert(ppStmt, "isaac", 13) < 0) {
    goto done;
  }

  /* done with statement */
  sc = sqlite3_finalize(ppStmt);
  if (sc != SQLITE_OK) {
    fprintf(stderr, "sqlite3_finalize: %s\n", sqlite3_errstr(sc));
    goto done;
  }

  rc = 0;

done:
  sqlite3_close(db); // NULL allowed
  return(rc);
} /* sql_insert */
