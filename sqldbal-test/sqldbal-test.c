
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "sqldbal-test/sqldbal-test.h"
#include "sqldbal/src/sqldbal.h"
#include "timestamp/timestamp.h"

#define DRIVER      SQLDBAL_DRIVER_SQLITE
#define PORT        NULL
#define USERNAME    NULL
#define PASSWORD    NULL
#define DATABASE    NULL
#define FLAGS       (SQLDBAL_FLAG_DEBUG |              \
                     SQLDBAL_FLAG_SQLITE_OPEN_CREATE | \
                     SQLDBAL_FLAG_SQLITE_OPEN_READWRITE)

TEST t_sqldbal_test2(){
  PASS();
}

TEST t_sqldbal_test1(){
  const char               *LOCATION = ".sqldbal-test-1.db";

  enum sqldbal_status_code rc;
  struct sqldbal_db        *db;
  struct sqldbal_stmt      *stmt;
  int64_t                  i64;
  const char               *text;

  rc = sqldbal_open(DRIVER,
                    LOCATION,
                    PORT,
                    USERNAME,
                    PASSWORD,
                    DATABASE,
                    FLAGS,
                    NULL,
                    0,
                    &db);
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
  rc = sqldbal_exec(db,
                    "CREATE TABLE IF NOT EXISTS test(id INTEGER, str TEXT)",
                    NULL,
                    NULL);
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_prepare(db,
                            "INSERT INTO test(id, str) VALUES(?, ?)",
                            -1,
                            &stmt);
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_bind_int64(stmt, 0, timestamp());
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_bind_text(stmt, 1, "timestamp", -1);
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_execute(stmt);
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_close(stmt);
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_prepare(db,
                            "SELECT id, str FROM test",
                            -1,
                            &stmt);
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
  rc = sqldbal_stmt_execute(stmt);
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
  while (sqldbal_stmt_fetch(stmt) == SQLDBAL_FETCH_ROW) {
    rc = sqldbal_stmt_column_int64(stmt, 0, &i64);
    ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
    rc = sqldbal_stmt_column_text(stmt, 1, &text, NULL);
    ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
    printf("%d / %s\n", (int)i64, text);
  }
  rc = sqldbal_stmt_close(stmt);
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
  rc = sqldbal_close(db);
  ASSERT_EQ(rc, SQLDBAL_STATUS_OK);
//  ASSERT_EQ(remove(LOCATION), 0);
  PASS();
} /* t_sqldbal_test1 */

SUITE(s_sqldbal_test) {
  RUN_TEST(t_sqldbal_test1);
  if (isatty(STDOUT_FILENO)) {
    RUN_TEST(t_sqldbal_test2);
  }
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_sqldbal_test);
  GREATEST_MAIN_END();
}
