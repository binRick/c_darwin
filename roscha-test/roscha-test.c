#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "log/log.h"
#include "ms/ms.h"
#include "roscha-test/roscha-test.h"
#include "roscha/include/hmap.h"
#include "roscha/include/object.h"
#include "roscha/include/parser.h"
#include "roscha/include/roscha.h"
#include "roscha/include/token.h"
#include "roscha/include/vector.h"
#include "timestamp/timestamp.h"
static struct roscha_env    *env;
static struct roscha_object *items;
static char                 *input = "name={{name}}\n"
                                     "var1={{var1}}\n";

TEST t_roscha_test_add_template(){
  //roscha_env_add_template(env, strdup("test"), input);
  PASS();
}

TEST t_roscha_test_vector(){
  items = roscha_object_new(roscha_vector_new());
  roscha_vector_push_new(items, (slice_whole("item0")));
  roscha_vector_push_new(items, (slice_whole("item1")));
  roscha_hmap_set(env->vars, "items", items);
  PASS();
}

TEST t_roscha_test_hash(){
  env->vars = roscha_object_new(hmap_new());
  roscha_hmap_set_new(env->vars, "name", "world");
  roscha_hmap_set_new(env->vars, "var1", 1);
  PASS();
}

SUITE(s_roscha_test) {
  RUN_TEST(t_roscha_test_hash);
  RUN_TEST(t_roscha_test_vector);
  RUN_TEST(t_roscha_test_add_template);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  env = roscha_env_new();
  RUN_SUITE(s_roscha_test);
  GREATEST_MAIN_END();
}
