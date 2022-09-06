
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "cache-utils-test/cache-utils-test.h"
#include "cache-utils/cache-utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

TEST t_cache_utils_get_is_expired(){
  char *s = "abc123"; int ttl = 60;

  log_debug("%s -> %d", s, cache_utils_is_expired(s, ttl));
  s = "richard";
  log_debug("%s -> %d", s, cache_utils_is_expired(s, ttl));
  s = "r";
  log_debug("%s -> %d", s, cache_utils_is_expired(s, ttl));
  s = "etc_passwd";
  log_debug("%s -> %d", s, cache_utils_is_expired(s, ttl));
}

TEST t_cache_utils_get_size(){
  char *s = "abc123";

  log_debug("%s -> %lu", s, cache_utils_get_size(s));
  s = "richard";
  log_debug("%s -> %lu", s, cache_utils_get_size(s));
  s = "r";
  log_debug("%s -> %lu", s, cache_utils_get_size(s));
  s = "etc_passwd";
  log_debug("%s -> %lu", s, cache_utils_get_size(s));
}

TEST t_cache_utils_get_age(){
  char *s = "abc123";

  log_debug("%s -> %d", s, cache_utils_get_age(s));
  s = "richard";
  log_debug("%s -> %d", s, cache_utils_get_age(s));
  s = "r";
  log_debug("%s -> %d", s, cache_utils_get_age(s));
  s = "etc_passwd";
  log_debug("%s -> %d", s, cache_utils_get_age(s));
}

TEST t_cache_utils_set_item(){
  char *s = "abc123", *item_value = "xxxxxxxxx123";

  log_debug("%s -> %d", s, cache_utils_set_item(s, item_value));
  s = "richard";
  log_debug("%s -> %d", s, cache_utils_set_item(s, item_value));
  s = "r", item_value = "yyyyyyyyyyyyyyyyyyyy";
  log_debug("%s -> %d", s, cache_utils_set_item(s, item_value));
  s = "etc_passwd", item_value = fsio_read_text_file("/etc/passwd");
  log_debug("%s -> %d", s, cache_utils_set_item(s, item_value));
  PASS();
}

TEST t_cache_utils_exists(){
  char *s = "abc123";

  log_debug("%s -> %d", s, cache_utils_exists(s));
  s = "richard";
  log_debug("%s -> %d", s, cache_utils_exists(s));
  s = "r";
  log_debug("%s -> %d", s, cache_utils_exists(s));
  PASS();
}

TEST t_cache_utils_get_hash(){
  char *s = "abc123";

  log_debug("%s -> %lu", s, cache_utils_get_hash(s));
  s = "richard";
  log_debug("%s -> %lu", s, cache_utils_get_hash(s));
  s = "r";
  log_debug("%s -> %lu", s, cache_utils_get_hash(s));
  PASS();
}

SUITE(s_cache_utils_test) {
  RUN_TEST(t_cache_utils_get_hash);
  RUN_TEST(t_cache_utils_exists);
  RUN_TEST(t_cache_utils_set_item);
  RUN_TEST(t_cache_utils_get_age);
  RUN_TEST(t_cache_utils_get_size);
  RUN_TEST(t_cache_utils_get_is_expired);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_cache_utils_test);
  GREATEST_MAIN_END();
}
