#pragma once
#ifndef DB_H
#define DB_H
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "core/core.h"
#define DB_CAPTURE_STATEMENT_STRUCT struct sqldbal_stmt
#define DB_CAPTURE_INSERT_SETUP(DB,STATEMENT){ do {  \
    enum sqldbal_status_code RC;\
    RC = sqldbal_stmt_prepare(DB,DB_CAPTURE_TABLE_FIELDS_INSERT_STATEMENT,-1,&STATEMENT);\
    assert(RC == SQLDBAL_STATUS_OK);
//////////////////////////////////////
#define TABLE_NAME_WINDOWS        "windows"
#define TABLE_FIELDS_WINDOWS      "id INTEGER, is_focused INTEGER, qoi BLOB"
#define TABLE_NAME_SPACES         "spaces"
#define TABLE_FIELDS_SPACES       "id INTEGER, is_current INTEGER, qoi BLOB"
#define TABLE_NAME_PROCESSES      "processes"
#define TABLE_FIELDS_PROCESSES    "pid INTEGER"
#define TABLE_NAME_COLORS         "colors"
#define TABLE_FIELDS_COLORS       "hex TEXT, r INTEGER, g INTEGER, b INTEGER"
#define TABLE_NAME_CAPTURES         "captures"
#define TABLE_FIELDS_CAPTURES       "  id INTEGER"\
                                    ", type INTEGER"\
                                    ", format INTEGER"\
                                    ", width INTEGER"\
                                    ", height INTEGER"\
                                    ", type_name TEXT"\
                                    ", format_name TEXT"\
                                    ", qoi_len INTEGER"\
                                    ", pixels BLOB"\
                                    ", pixels_len INTEGER"

#define DB_CAPTURE_INSERT_BIND(DB,STATEMENT,HASHES){ do { \
    enum sqldbal_status_code RC;\
    size_t int_qty  = (size_t)(hash_size((hash_t *)(hash_get(HASHES,"int"),\
           text_qty = (size_t)(hash_size((hash_t *)(hash_get(HASHES,"char"))],\
           total_qty = int_qty + text_qty;\
    int ok[total_qty];\
    int64_t                  i64[int_qty];\
    const char               text[text_qty];\
        size_t index = 0;\
          hash_t *__db_capture_insert = (hash_t*)(hash_get(HASHES,"insert")),\
          each_hash((hash_t*)(hash_get((__db_capture_insert), {\
            each_hash((hash_t*)(hash_get(key,"text")),{\
                ok[index] = sqldbal_stmt_bind_text(STATEMENT,\
                     (int)(index),\
                     (char*)(hash_get(val),"name"),\
                     (int)(hash_get(val),"default"),\
                    );\
            })\
            each_hash((hash_t*)(hash_get(key,"int")),{\
               ok[index] = sqldbal_stmt_bind_int64(STATEMENT, index, (size_t)(hash_get((hash_t*)(val),"function")))); assert(ok[index] == SQLDBAL_STATUS_OK);\
            })\
            index++;\
          })\
}; while(0); }


#define DB_CAPTURE_INSERT_ROW(DB) \
    rc = sqldbal_stmt_execute(stmt);\
    rc = sqldbal_stmt_close(stmt);\
}; while(0); }
#define DB_CAPTURE_TABLE_FIELDS_INSERT_STATEMENT\
  "INSERT INTO " DB_CAPTURE_TABLE_NAME_CAPTURES "("\
                                "window_id"\
                                ",capture_ms"\
                                ",capture_ts"\
                                ",capture_format"\
                                ",capture_type"\
                                ",capture_size"\
                                ",qoi_len"\
                                ",qoi"\
                                ",gif_len"\
                                ",gif"\
                                ") "\
                          " VALUES "\
                              "("\
                                "  ?"/*window_id*/\
                                ", ?"/*capture_ms*/\
                                ", ?"/*capture_ts*/\
                                ", ?"/*capture_type*/\
                                ", ?"/*capture_size*/\
                                ", ?"/*qoi_len*/\
                                ", ?"/*qoi*/\
                                ", ?"/*gif_len*/\
                                ", ?"/*gif_len*/\
                              ")"
#define DB_CAPTURE_RUN(DB){ do { \
  int __ROWS[] = {\
    5,\
  };\
  DB_CAPTURE_STATEMENT_STRUCT *__DB_CAPTURE_STATEMENT;\
  DB_CAPTURE_INSERT_SETUP(DB,__DB_CAPTURE_STATEMENT);\
  DB_CAPTURE_INSERT_BIND(DB,__DB_CAPTURE_STATEMENT,__ROWS[0]);\
  \
} while(0); }
#if 0
    rc = sqldbal_stmt_prepare(db,
                              "INSERT INTO "TABLE_NAME_WINDOWS "(ts, str) VALUES(?, ?)",
                              -1,
                              &stmt);
    assert(rc == SQLDBAL_STATUS_OK);
    rc = sqldbal_stmt_bind_int64(stmt, 0, timestamp());
    rc = sqldbal_stmt_bind_text(stmt, 1, "timestamp", -1);
#endif
//////////////////////////////////////
#include "sqldbal/src/sqldbal.h"
enum capture_type_id_t;
enum db_loader_t {
  DB_LOADER_SPACES,
  DB_LOADER_WINDOWS,
  DB_LOADER_CAPTURES,
  DB_LOADERS_QTY,
};
typedef bool (*db_loader_fxn)(struct sqldbal_db *db);
bool db_test(void);
bool db_rows(char *table);
bool db_table(char *table);
struct Vector *db_table_ids_v(char *table);
struct Vector *db_tables_v(void);
enum db_loader_t db_loader_name_id(char *name);
struct Vector *db_table_images_from_ids(enum capture_type_id_t type, struct Vector *ids);
bool db_info();
bool db_loader_name(char *name);
bool db_loader_id(enum db_loader_t type);
bool db_tables(void);
bool db_capture_save(hash_t *map);
#endif
