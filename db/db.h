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
//////////////////////////////////////
#define TABLE_NAME_WINDOWS        "windows"
#define TABLE_FIELDS_WINDOWS      "id INTEGER, is_focused INTEGER, qoi BLOB"
#define TABLE_NAME_SPACES         "spaces"
#define TABLE_FIELDS_SPACES       "id INTEGER, is_current INTEGER, qoi BLOB"
#define TABLE_NAME_PROCESSES      "processes"
#define TABLE_FIELDS_PROCESSES    "pid INTEGER"
#define TABLE_NAME_COLORS         "colors"
#define TABLE_FIELDS_COLORS       "hex TEXT, r INTEGER, g INTEGER, b INTEGER"
//////////////////////////////////////
#include "sqldbal/src/sqldbal.h"
enum capture_type_id_t;
enum db_loader_t {
  DB_LOADER_SPACES,
  DB_LOADER_WINDOWS,
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
#endif
