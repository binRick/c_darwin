#pragma once
#ifndef DB_C
#define DB_C
#include "capture/utils/utils.h"
#include "core/core.h"
#include "db/db.h"
#include "sqldbal/src/sqldbal.h"
#define DB_DRIVER           SQLDBAL_DRIVER_SQLITE
#define DB_PORT             NULL
#define DB_USERNAME         NULL
#define DB_PASSWORD         NULL
#define DB_DATABASE         NULL
#define DB_FILE             ".sqldbal-db-1.db"
#define LOCAL_DEBUG_MODE    DB_DEBUG_MODE
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "capture/type/type.h"
#include "capture/utils/utils.h"
#include "core/core.h"
#include "core/utils/utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "space/utils/utils.h"
#include "timestamp/timestamp.h"
#include "whereami/src/whereami.h"
#include "window/utils/utils.h"
#include <sqlite3.h>
static bool DB_DEBUG_MODE = false;
static int                          DB_FLAGS = (SQLDBAL_FLAG_SQLITE_OPEN_CREATE | SQLDBAL_FLAG_SQLITE_OPEN_READWRITE);
static bool db_create_tables(void);
static char                         *db_file                   = NULL;
static struct sqldbal_db            *db                        = NULL;
static const db_loader_fxn          db_loaders[DB_LOADERS_QTY] = {
  [DB_LOADER_SPACES]  = space_db_load,
  [DB_LOADER_WINDOWS] = window_db_load,
//  [DB_LOADER_KEYS] = keys_db_load,
//  [DB_LOADER_CAPTURES] = capture_db_load,
};

static const char                   *db_loader_names[DB_LOADERS_QTY] = {
  [DB_LOADER_SPACES]  = "spaces",
  [DB_LOADER_WINDOWS] = "windows",
//  [DB_LOADER_KEYS] = "keys",
//  [DB_LOADER_CAPTURES] = "captures",
};
static const enum capture_type_id_t db_loader_capture_types[DB_LOADERS_QTY] = {
  [DB_LOADER_SPACES]  = CAPTURE_TYPE_SPACE,
  [DB_LOADER_WINDOWS] = CAPTURE_TYPE_WINDOW,
};

enum db_loader_t db_loader_name_id(char *name){
  for (size_t i = 0; i < DB_LOADERS_QTY; i++)
    if (strcmp(db_loader_names[i],
               name) == 0)
      return(i);

  return(-1);
}
enum db_capture_field_type_t {
  DB_CAPTURE_FIELD_INT,
  DB_CAPTURE_FIELD_TEXT,
  DB_CAPTURE_FIELD_BLOB,
  DB_CAPTURE_FIELDS_QTY,
};
struct db_capture_field_t {
  char                         *field;
  enum db_capture_field_type_t type;
};
static struct db_capture_field_t db_cap_fields[] = {
  { .field = "id",          .type = DB_CAPTURE_FIELD_INT,  },
  { .field = "type",        .type = DB_CAPTURE_FIELD_INT,  },
  { .field = "format",      .type = DB_CAPTURE_FIELD_INT,  },
  { .field = "width",       .type = DB_CAPTURE_FIELD_INT,  },
  { .field = "height",      .type = DB_CAPTURE_FIELD_INT,  },
  { .field = "len",         .type = DB_CAPTURE_FIELD_INT,  },
  { .field = "type_name",   .type = DB_CAPTURE_FIELD_TEXT, },
  { .field = "format_name", .type = DB_CAPTURE_FIELD_TEXT, },
  { .field = "pixels",      .type = DB_CAPTURE_FIELD_BLOB, },
  { .field = "pixels_len",  .type = DB_CAPTURE_FIELD_INT,  },
  { 0 },
};

int db_capture_save(hash_t *map){
  struct sqldbal_stmt *stmt;
  struct StringBuffer *sb = stringbuffer_new(), *sb_val = stringbuffer_new();
  char                *sql[3];

  sql[0] = "INSERT INTO captures ( \n";
  sql[1] = "\n\t) VALUES ( \n";
  sql[2] = "\n);";
  size_t qty = 0;

  stringbuffer_append_string(sb, sql[0]);
  char *fl;

  for (struct db_capture_field_t *c = &(db_cap_fields[0]); c->field; c++) {
    asprintf(&fl, "%s_len", c->field);
    if (hash_has((hash_t *)(hash_get(map, "int")), c->field)) {
      if (qty > 0)
        stringbuffer_append_string(sb, ", ");
      stringbuffer_append_string(sb, c->field);
      if (qty > 0)
        stringbuffer_append_string(sb_val, ", ");
      stringbuffer_append_string(sb_val, "?");
      qty++;
    }else if (hash_has((hash_t *)(hash_get(map, "text")), c->field)) {
      if (qty > 0)
        stringbuffer_append_string(sb, ", ");
      stringbuffer_append_string(sb, c->field);
      if (qty > 0)
        stringbuffer_append_string(sb_val, ", ");
      stringbuffer_append_string(sb_val, "?");
      qty++;
    }else if (
      hash_has((hash_t *)(hash_get(map, "blob")), c->field)
      && hash_has((hash_t *)(hash_get(map, "blob")), fl)
      ) {
      if (qty > 0)
        stringbuffer_append_string(sb, ", ");
      stringbuffer_append_string(sb, c->field);
      if (qty > 0)
        stringbuffer_append_string(sb_val, ", ");
      stringbuffer_append_string(sb_val, "?");
    }
  }
  stringbuffer_append_string(sb, sql[1]);
  stringbuffer_append_string(sb, stringbuffer_to_string(sb_val));
  stringbuffer_append_string(sb, sql[2]);
  char *SQL = stringbuffer_to_string(sb);

  errno = 0;
  if (sqldbal_stmt_prepare(db, SQL, -1, &stmt) != SQLDBAL_STATUS_OK) {
    log_error("SQL BIND FAIL");
    exit(1);
  }
  qty = 0;
  for (struct db_capture_field_t *c = &(db_cap_fields[0]); c->field; c++) {
    asprintf(&fl, "%s_len", c->field);
    if (hash_has((hash_t *)(hash_get(map, "int")), c->field)) {
      int64_t val = (int64_t)(hash_get((hash_t *)(hash_get(map, "int")), c->field));
      if (sqldbal_stmt_bind_int64(stmt, qty++, val)) {
        log_error("int err: k:%s|val:%lld|qty:%lu", c->field, val, qty);
        exit(1);
      }
    }else if (hash_has((hash_t *)(hash_get(map, "text")), c->field)) {
      char *val = (char *)(hash_get((hash_t *)(hash_get(map, "text")), c->field));
      if (SQLDBAL_STATUS_OK != sqldbal_stmt_bind_text(stmt, qty++, val, -1)) {
        log_error("text err");
        exit(1);
      }
    }else if (
      hash_has((hash_t *)(hash_get(map, "blob")), c->field)
      && hash_has((hash_t *)(hash_get(map, "blob")), fl)
      ) {
      size_t len  = (size_t)(hash_get((hash_t *)(hash_get(map, "blob")), fl));
      char   *val = b64_encode((void *)(hash_get((hash_t *)(hash_get(map, "blob")), c->field)), len);
      len = strlen(val);
      if (SQLDBAL_STATUS_OK != sqldbal_stmt_bind_blob(stmt, qty++, val, len)) {
        log_error("blob err");
        exit(1);
      }
    }
  }

  stringbuffer_release(sb);
  stringbuffer_release(sb_val);
  if (sqldbal_stmt_execute(stmt) != SQLDBAL_STATUS_OK) {
    log_error("Execute error");
    exit(1);
  }else{
    if (sqldbal_stmt_close(stmt) != SQLDBAL_STATUS_OK) {
      log_error("Close error");
      exit(1);
    }else{
      int row_id = -1;
      if (sqldbal_last_insert_id(db, "id", &row_id) != SQLDBAL_STATUS_OK) {
        log_error("last insert id error");
        exit(1);
      }
      return(row_id);
    }
    return(-1);
  }
} /* db_capture_save */

bool db_loader_id(enum db_loader_t type){
  if (db_loaders[type](db))
    return(true);

  return(false);
}

bool db_loader_name(char *name){
  return(db_loader_id(db_loader_name_id(name)));
}

///////////////////////////////////////////////////////////////////////
struct sqldbal_db *db_init(void){
  static struct sqldbal_db *_db = 0;

  if (_db)
    goto ready;
  sqldbal_open(DB_DRIVER, db_file, DB_PORT, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_FLAGS, NULL, 0, &_db);
  assert(_db);
ready:
  return(_db);
}

static bool db_create_tables(void){
  enum sqldbal_status_code rc;
  char                     *sql;

#define CREATE_TABLE(TABLE, FIELDS)    { do{                                                             \
                                           asprintf(&sql, "CREATE TABLE IF NOT EXISTS %s"                \
                                                    "("                                                  \
                                                    "  __id INTEGER PRIMARY KEY AUTOINCREMENT"           \
                                                    ", created DATETIME DEFAULT CURRENT_TIMESTAMP"       \
                                                    ", updated DATETIME DEFAULT CURRENT_TIMESTAMP"       \
                                                    ", ts INTEGER"                                       \
                                                    ", %s"                                               \
                                                    ")", TABLE, FIELDS);                                 \
                                           errno = 0;                                                    \
                                           if (SQLDBAL_STATUS_OK != sqldbal_exec(db, sql, NULL, NULL)) { \
                                             log_error("Failed to create table %s", TABLE);              \
                                             exit(EXIT_FAILURE);                                         \
                                           }                                                             \
                                         }while (0); }
/////////////////////////////////////////////////////////////
  CREATE_TABLE(TABLE_NAME_WINDOWS, TABLE_FIELDS_WINDOWS);
  CREATE_TABLE(TABLE_NAME_SPACES, TABLE_FIELDS_SPACES);
  CREATE_TABLE(TABLE_NAME_PROCESSES, TABLE_FIELDS_PROCESSES);
  CREATE_TABLE(TABLE_NAME_COLORS, TABLE_FIELDS_COLORS);
  CREATE_TABLE(TABLE_NAME_CAPTURES, TABLE_FIELDS_CAPTURES);
  CREATE_TABLE(TABLE_NAME_APP_ICONS, TABLE_FIELDS_APP_ICONS);
  CREATE_TABLE(TABLE_NAME_KEYS, TABLE_FIELDS_KEYS);
/////////////////////////////////////////////////////////////
#undef CREATE_TABLE
  return(true);
}

bool db_test(void){
  enum sqldbal_status_code rc;
  struct sqldbal_stmt      *stmt;
  int64_t                  i64;
  const char               *text;

  rc = sqldbal_stmt_prepare(db,
                            "INSERT INTO "TABLE_NAME_WINDOWS "(ts, str) VALUES(?, ?)",
                            -1,
                            &stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_bind_int64(stmt, 0, timestamp());
  rc = sqldbal_stmt_bind_text(stmt, 1, "timestamp", -1);
  rc = sqldbal_stmt_execute(stmt);
  rc = sqldbal_stmt_close(stmt);
  rc = sqldbal_stmt_prepare(db,
                            "SELECT ts, str FROM "TABLE_NAME_WINDOWS,
                            -1,
                            &stmt);
  rc = sqldbal_stmt_execute(stmt);
  for (size_t i = 0; (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW); i++) {
    rc = sqldbal_stmt_column_int64(stmt, 0, &i64);
    rc = sqldbal_stmt_column_text(stmt, 1, &text, NULL);
  }
  rc = sqldbal_stmt_close(stmt);
  return(true);
}

struct Vector *db_table_ids_v(char *table){
  struct Vector       *v = vector_new();
  struct sqldbal_stmt *stmt;
  int64_t             id;
  int                 rc;
  char                *sql;

  asprintf(&sql, "SELECT __id from %s ORDER by __id DESC", table);
  rc = sqldbal_stmt_prepare(db, sql, -1, &stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_execute(stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  for (size_t i = 0; (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW); i++) {
    rc = sqldbal_stmt_column_int64(stmt, 0, &id);
    assert(rc == SQLDBAL_STATUS_OK);
    vector_push(v, (void *)id);
  }
  rc = sqldbal_stmt_close(stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  return(v);
}

size_t db_table_rows(char *table){
  struct sqldbal_stmt *stmt;
  size_t              size = 0;
  int64_t             i64;
  int                 rc;
  char                *sql;

  asprintf(&sql, "SELECT COUNT(*) from %s", table);
  rc = sqldbal_stmt_prepare(db, sql, -1, &stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_execute(stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  for (size_t i = 0; (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW); i++) {
    rc = sqldbal_stmt_column_int64(stmt, 0, &i64);
    assert(rc == SQLDBAL_STATUS_OK);
    size += i64;
  }
  rc = sqldbal_stmt_close(stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  return(size);
}

struct Vector *db_tables_v(){
  struct Vector       *v = vector_new();
  struct sqldbal_stmt *stmt;
  const char          *tbl;
  int                 rc;
  char                *sql = "SELECT name FROM sqlite_master WHERE type = \"table\" and name not like \"sqlite_%\"";

  rc = sqldbal_stmt_prepare(db, sql, -1, &stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_execute(stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  for (size_t i = 0; (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW); i++) {
    rc = sqldbal_stmt_column_text(stmt, 0, &tbl, NULL);
    assert(rc == SQLDBAL_STATUS_OK);
    vector_push(v, (void *)strdup(tbl));
  }
  rc = sqldbal_stmt_close(stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  return(v);
}

bool db_info(){
  const char    *tbl;
  struct Vector *tbls = db_tables_v();

  for (size_t i = 0; i < vector_size(tbls); i++) {
    tbl = (char *)vector_get(tbls, i);
    fprintf(stdout, " - #%lu: %s: %s, %lu rows|\n", i + 1, tbl, bytes_to_string(db_table_size(tbl)), db_table_rows(tbl));
    free(tbl);
  }
  vector_release(tbls);
  return(true);
}

bool db_table(char *table){
  log_info("Loading table %s", table);
  return(true);
}

sqldbal_exec_callback_fp cb(void *u, size_t qty, char **r, size_t l){
  Dbg(qty, %u);
  Dbg(l, %u);
}

size_t db_table_size(char *table){
  char                *sql; size_t size = 0;
  struct sqldbal_stmt *stmt;

  return(size);
}

size_t db_rows(char *table){
  struct sqldbal_stmt *stmt;
  char                *sql;
  size_t              rows;

  asprintf(&sql, "SELECT COUNT(*) from %s", table);
  int rc = sqldbal_stmt_prepare(db, sql, -1, &stmt);

  assert(rc == SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_execute(stmt);
  assert(rc == SQLDBAL_STATUS_OK);
  for (size_t i = 0; (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW); i++) {
    rc = sqldbal_stmt_column_int64(stmt, 0, &rows);
    assert(rc == SQLDBAL_STATUS_OK);
  }
  return(rows);
}

static void __attribute__((constructor)) __constructor__db(void){
  asprintf(&db_file, "%s/%s",
           core_utils_whereami_report()->executable_directory,
           DB_FILE
           );
  if (getenv("DEBUG") != NULL || getenv("DEBUG_DB") != NULL) {
    log_debug("Enabling db Debug Mode");
    DB_DEBUG_MODE = true;
  }
  if (LOCAL_DEBUG_MODE)
    DB_FLAGS |= SQLDBAL_FLAG_DEBUG;
  assert((db = db_init()));
  assert(db_create_tables() == true);
}

bool Table_create(const VSelf) {
  VSELF(const DB);
  return(NULL);
}

bool Table_insert(const VSelf) {
  VSELF(const DB);
  return(NULL);
}

bool Table_delete_id(const VSelf, size_t id) {
  VSELF(const DB);
  return(false);
}

bool Table_delete_hash(const VSelf, hash_t *h) {
  VSELF(const DB);
  return(false);
}

char *Table_schema(const VSelf) {
  VSELF(const DB);
  return(NULL);
}

char *Table_hash(const VSelf) {
  VSELF(const DB);
  return(NULL);
}

bool Table_exists_id(const VSelf, size_t id) {
  VSELF(const DB);
  return(false);
}

bool Table_exists_hash(const VSelf, hash_t *h) {
  VSELF(const DB);
  return(false);
}

hash_t *Table_select_id(const VSelf, size_t id) {
  VSELF(const DB);
  return(NULL);
}

hash_t *Table_select_hash(const VSelf, hash_t *h) {
  VSELF(const DB);
  return(NULL);
}

typedef struct {
  const char *name;
} Table;

Table *Table_new(const char *name) {
  Table *self = malloc(sizeof *self);

  assert(self);
  self->name = name;
  return(self);
}
implExtern(DB, Table);

////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
