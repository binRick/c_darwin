#pragma once
#ifndef DB_C
#define DB_C
#include "db/db.h"
#include "sqldbal/src/sqldbal.h"
#define DB_DRIVER      SQLDBAL_DRIVER_SQLITE
#define DB_PORT        NULL
#define DB_USERNAME    NULL
#define DB_PASSWORD    NULL
#define DB_DATABASE    NULL
#define DB_FILE ".sqldbal-db-1.db"
#define LOCAL_DEBUG_MODE    DB_DEBUG_MODE
////////////////////////////////////////////
#include "core/utils/utils.h"
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "capture/type/type.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "whereami/src/whereami.h"
#include "space/utils/utils.h"
#include "window/utils/utils.h"
#include "capture/utils/utils.h"
#include "core/core.h"
static bool DB_DEBUG_MODE = false;
static int DB_FLAGS =   (SQLDBAL_FLAG_SQLITE_OPEN_CREATE | SQLDBAL_FLAG_SQLITE_OPEN_READWRITE);
static bool db_create_tables(void);
static char *db_file=NULL;
static struct sqldbal_db        *db = NULL;
static const db_loader_fxn db_loaders[DB_LOADERS_QTY] = {
  [DB_LOADER_SPACES] = space_db_load,
  [DB_LOADER_WINDOWS] = window_db_load,
};

static const char * db_loader_names[DB_LOADERS_QTY] = {
  [DB_LOADER_SPACES] = "spaces",
  [DB_LOADER_WINDOWS] = "windows",
};
static const enum capture_type_id_t db_loader_capture_types[DB_LOADERS_QTY] = {
  [DB_LOADER_SPACES] = CAPTURE_TYPE_SPACE,
  [DB_LOADER_WINDOWS] = CAPTURE_TYPE_WINDOW,
};


enum db_loader_t db_loader_name_id(char *name){
  for(size_t i=0;i<DB_LOADERS_QTY;i++)
    if(strcmp(db_loader_names[i],name)==0)
      return(i);
  return(-1);
}

struct Vector *db_table_images_from_ids(enum capture_type_id_t type, struct Vector *ids){
  struct Vector *captures_v = NULL;
  captures_v =  db_tables_images(type,ids);
  Dbg(vector_size(captures_v),%lu);  
  return(captures_v);
}
bool db_loader_id(enum db_loader_t type){
  if(db_loaders[type](db)){
//    struct Vector *ids = vector_new();
//    vector_push(ids,(void*)14872);
//struct Vector *captures_v = db_table_images_from_ids(db_loader_capture_types[type],ids);
//  Dbg(vector_size(captures_v),%lu);  
    return(true);
  }
  return(false);
}

bool db_loader_name(char *name){
  return(db_loader_id(db_loader_name_id(name)));
}

///////////////////////////////////////////////////////////////////////
struct sqldbal_db *db_init(void){
  static struct sqldbal_db        *_db = 0;
  if(_db) goto ready;
  sqldbal_open(DB_DRIVER,db_file,DB_PORT,DB_USERNAME,DB_PASSWORD,DB_DATABASE,DB_FLAGS,NULL,0,&_db);
  assert(_db);
ready:
  return(_db);
}

static bool db_create_tables(void){
  enum sqldbal_status_code rc;
  char *sql;
#define CREATE_TABLE(TABLE,FIELDS){do{\
  asprintf(&sql,"CREATE TABLE IF NOT EXISTS %s"\
      "("\
      "  __id INTEGER PRIMARY KEY AUTOINCREMENT"\
      ", created DATETIME DEFAULT CURRENT_TIMESTAMP"\
      ", updated DATETIME DEFAULT CURRENT_TIMESTAMP"\
      ", ts INTEGER"\
      ", %s"\
      ")",TABLE,FIELDS);\
  errno=0;\
  rc = sqldbal_exec(db,sql,NULL,NULL);\
  if(rc != SQLDBAL_STATUS_OK){\
    log_error("Failed to create table %s", TABLE);\
  }\
}while(0);}
/////////////////////////////////////////////////////////////  
  CREATE_TABLE(TABLE_NAME_WINDOWS, TABLE_FIELDS_WINDOWS);
  CREATE_TABLE(TABLE_NAME_SPACES,  TABLE_FIELDS_SPACES);
  CREATE_TABLE(TABLE_NAME_PROCESSES,  TABLE_FIELDS_PROCESSES);
  CREATE_TABLE(TABLE_NAME_COLORS,  TABLE_FIELDS_COLORS);
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
                            "INSERT INTO "TABLE_NAME_WINDOWS"(ts, str) VALUES(?, ?)",
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
  for(size_t i=0; (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW); i++) {
    rc = sqldbal_stmt_column_int64(stmt, 0, &i64);
    rc = sqldbal_stmt_column_text(stmt, 1, &text, NULL);
  }
  rc = sqldbal_stmt_close(stmt);
  return(true);
}

struct Vector *db_table_ids_v(char *table){
  struct Vector *v = vector_new();
  struct sqldbal_stmt      *stmt;
  int64_t                  id;
  int rc;
  char *sql;
  asprintf(&sql,"SELECT __id from %s ORDER by __id DESC",table);
  rc = sqldbal_stmt_prepare(db,sql,-1,&stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_execute(stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  for(size_t i=0; (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW); i++) {
    rc = sqldbal_stmt_column_int64(stmt, 0, &id);
    assert(rc==SQLDBAL_STATUS_OK);
    vector_push(v,(void*)id);
  }
  rc = sqldbal_stmt_close(stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  return(v);
}

size_t db_table_rows(char *table){
  struct sqldbal_stmt      *stmt;
  size_t size = 0;
  int64_t                  i64;
  int rc;
  char *sql;
  asprintf(&sql,"SELECT COUNT(*) from %s",table);
  rc = sqldbal_stmt_prepare(db,sql,-1,&stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_execute(stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  for(size_t i=0; (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW); i++) {
    rc = sqldbal_stmt_column_int64(stmt, 0, &i64);
    assert(rc==SQLDBAL_STATUS_OK);
    size+=i64;
  }
  rc = sqldbal_stmt_close(stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  return(size);
}

size_t db_table_size(char *table){
  struct sqldbal_stmt      *stmt;
  size_t size = 0;
  int64_t                  i64;
  int rc;
  char *sql;
  return(123);
  assert(db);
  asprintf(&sql,"SELECT SUM(\"pgsize\") FROM \"dbstat\" WHERE name=\"%s\"",table);
  errno=0;
  rc = sqldbal_stmt_prepare(db,sql,-1,&stmt);
  if(rc!=SQLDBAL_STATUS_OK){
    log_error("Failed to prepare statement");
    exit(1);
  }
  rc = sqldbal_stmt_execute(stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  for(size_t i=0; (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW); i++) {
    rc = sqldbal_stmt_column_int64(stmt, 0, &i64);
    assert(rc==SQLDBAL_STATUS_OK);
    size+=i64;
  }
  rc = sqldbal_stmt_close(stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  return(size);
}

struct Vector *db_tables_v(){
  struct Vector *v = vector_new();
  struct sqldbal_stmt      *stmt;
  const char               *tbl;
  int rc;
  char *sql = "SELECT name FROM sqlite_master WHERE type = \"table\" and name not like \"sqlite_%\"";
  rc = sqldbal_stmt_prepare(db,sql,-1,&stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_execute(stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  for(size_t i=0; (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW); i++) {
    rc = sqldbal_stmt_column_text(stmt, 0, &tbl,NULL);
    assert(rc==SQLDBAL_STATUS_OK);
    vector_push(v,(void*)strdup(tbl));
  }
  rc = sqldbal_stmt_close(stmt);
  assert(rc==SQLDBAL_STATUS_OK);
  return(v);
}

bool db_info(){
  const char               *tbl;
  struct Vector *tbls = db_tables_v();
  for(size_t i = 0; i <vector_size(tbls);i++){
    tbl=(char*)vector_get(tbls,i);
    fprintf(stdout," - #%lu: %s: %s, %lu rows|\n",i+1,tbl,bytes_to_string(db_table_size(tbl)),db_table_rows(tbl));
    free(tbl);
  }
  vector_release(tbls);
  return(true);
}

bool db_table(char *table){
  log_info("Loading table %s",table);
  return(true);
}

bool db_rows(char *table){
  log_info("Loading table %s Rows",table);
  return(true);
}

static void __attribute__((constructor)) __constructor__db(void){
  asprintf(&db_file,"%s/%s",
      core_utils_whereami_report()->executable_directory,
      DB_FILE
      );
  if (getenv("DEBUG") != NULL || getenv("DEBUG_DB") != NULL) {
    log_debug("Enabling db Debug Mode");
    DB_DEBUG_MODE = true;
  }
  if(LOCAL_DEBUG_MODE)
    DB_FLAGS |= SQLDBAL_FLAG_DEBUG;
  assert((db = db_init()));
  assert(db_create_tables()==true);
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
