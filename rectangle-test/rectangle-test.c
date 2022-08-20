////////////////////////////////////////////

#ifdef DEBUG_MEMORY
#include "debug-memory/debug_memory.h"
#endif
////////////////////////////////////////////
#include "rectangle-test/rectangle-test.h"
#include "rectangle/rectangle.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/include/vector.h"

////////////////////////////////////////////
static char EXECUTABLE_PATH[PATH_MAX + 1] = { 0 };
static char *EXECUTABLE;
static char *EXECUTABLE_PATH_DIRNAME;
static bool DEBUG_MODE_ENABLED = false;
char        *starting_todo_app = NULL;

TEST t_rectangle_get_todo_keys(){
  struct keycode_modifier_t *kcm = rectangle_get_todo_keys();

  printf("keycode: %d\n", kcm->key_code);
  printf("key modifier: %d\n", kcm->key_modifier);
  ASSERT_GTEm("Rectangle keycode not valid", kcm->key_code, 0);
  ASSERT_GTEm("Rectangle keymodifier not valid", kcm->key_modifier, 0);

  printf("keycode_s: %s\n", kcm->key_code_s);
  printf("key_modifier_s: %s\n", kcm->key_modifier_s);
  printf("keys: %s\n", kcm->keys);

  PASS();
}

TEST t_rectangle_set_todo_app(void *NEW_APP){
  char       *msg     = NULL;
  const char *new_app = (const char *)NEW_APP;
  bool       ok       = rectangle_set_todo_app(new_app);

  ASSERT_EQm("Rectangle app set OK", ok, true);
  asprintf(&msg, "Todo App Set to: " AC_RESETALL AC_GREEN "%s" AC_RESETALL "\n",
           new_app
           );
  PASSm(msg);
}

TEST t_rectangle_set_todo_width(void *NEW_WIDTH){
  char *msg      = NULL;
  int  new_width = (int)(size_t)NEW_WIDTH;
  bool ok        = rectangle_set_todo_width(new_width);

  ASSERT_EQm("Rectangle width set OK", ok, true);
  int todo_width = rectangle_get_todo_width();

  ASSERT_EQm("Rectangle width set OK", new_width, todo_width);
  asprintf(&msg, "Todo Mode Set to Width: " AC_RESETALL AC_GREEN "%d" AC_RESETALL "\n",
           todo_width
           );
  PASSm(msg);
}

TEST t_rectangle_get_todo_width(void){
  char *msg       = NULL;
  int  todo_width = rectangle_get_todo_width();

  ASSERT_GTEm("Rectangle width not valid", todo_width, 0);
  ASSERT_LTEm("Rectangle width not valid", todo_width, 99999);

  asprintf(&msg, "Todo Mode Width: " AC_RESETALL AC_GREEN "%d" AC_RESETALL "\n",
           todo_width
           );
  PASSm(msg);
}

TEST t_rectangle_get_todo_app(void){
  char *msg      = NULL;
  char *todo_app = rectangle_get_todo_app();

  ASSERT_NEQm("Invalid todo mode", todo_app, NULL);
  asprintf(&msg, "Todo Mode App: " AC_RESETALL AC_GREEN "%s" AC_RESETALL "\n",
           todo_app
           );
  if (starting_todo_app == NULL) {
    starting_todo_app = todo_app;
  }
  PASSm(msg);
}

TEST t_rectangle_set_todo_mode_enabled(void){
  char *msg = NULL;
  bool ok   = rectangle_set_todo_mode_enabled();

  ASSERT_EQm("Rectangle Todo mode Enable Failure", ok, true);
  bool todo_mode_enabled = rectangle_get_todo_mode_enabled();

  ASSERT_EQm("Rectangle Todo mode Enable Failure", todo_mode_enabled, true);
  asprintf(&msg, "Todo Mode Enabled");
  PASSm(msg);
}

TEST t_rectangle_set_todo_mode_disabled(void){
  char *msg = NULL;
  bool ok   = rectangle_set_todo_mode_disabled();

  ASSERT_EQm("Rectangle Todo mode disable Failure", ok, true);
  bool todo_mode_enabled = rectangle_get_todo_mode_enabled();

  ASSERT_EQm("Rectangle Todo mode Enable Failure", todo_mode_enabled, false);
  asprintf(&msg, "Todo Mode Disabled");
  PASSm(msg);
}

TEST t_rectangle_get_todo_mode_enabled(void){
  char *msg              = NULL;
  bool todo_mode_enabled = rectangle_get_todo_mode_enabled();

  asprintf(&msg, "Todo Mode Enabled: " AC_RESETALL "%s" AC_RESETALL "\n",
           todo_mode_enabled ? AC_GREEN "Yes" : AC_RED "No"
           );
  PASSm(msg);
}

TEST t_rectangle_get_config(void){
  char *msg    = NULL;
  char *config = rectangle_get_config();

  ASSERT_NEQm("Rectangle config not valid", config, NULL);
  ASSERT_GTEm("Rectangle config not found", strlen(config), 10);
  asprintf(&msg, "Test config :\n" AC_RESETALL AC_YELLOW "%s "AC_RESETALL "\n",
           config
           );
  PASSm(msg);
}

TEST t_rectangle_run(void){
  char *msg = NULL;
  bool ok   = rectangle_run();

  ASSERT_EQm("Rectangle run failed", ok, true);
  int pid = rectangle_get_pid();

  asprintf(&msg, "Test rectangle Started OK: %d", pid);
  PASSm(msg);
}

TEST t_rectangle_kill(void){
  char *msg = NULL;
  bool ok   = rectangle_kill();

  ASSERT_EQm("Rectangle kill failed", ok, true);
  asprintf(&msg, "Test rectangle Killed OK");
  PASSm(msg);
}

TEST t_rectangle_get_pid(void){
  char *msg = NULL;
  int  pid  = rectangle_get_pid();

  ASSERT_GTEm("Rectangle pid not found", pid, 2);
  asprintf(&msg, "Test rectangle pid OK: '%d'\n",
           pid
           );
  PASSm(msg);
}

SUITE(s_rectangle_test) {
  void *TEST_PARAM = 0;

  RUN_TEST(t_rectangle_get_todo_app);
  RUN_TEST(t_rectangle_get_config);
  RUN_TEST(t_rectangle_get_todo_width);
  RUN_TESTp(t_rectangle_set_todo_width, (void *)1950);
  RUN_TESTp(t_rectangle_set_todo_width, (void *)2050);
  RUN_TEST(t_rectangle_get_todo_app);
  RUN_TESTp(t_rectangle_set_todo_app, (void *)"org.alacritty");
  RUN_TEST(t_rectangle_get_todo_app);
  RUN_TESTp(t_rectangle_set_todo_app, (void *)"net.kovidgoyal.kitty");
  RUN_TEST(t_rectangle_get_todo_app);
  RUN_TESTp(t_rectangle_set_todo_app, (void *)"rich");
  RUN_TEST(t_rectangle_get_todo_app);
  RUN_TEST(t_rectangle_run);
  RUN_TEST(t_rectangle_get_pid);
  RUN_TEST(t_rectangle_get_todo_mode_enabled);
  RUN_TEST(rectangle_set_todo_mode_disabled);
  RUN_TEST(t_rectangle_get_todo_mode_enabled);
  RUN_TEST(rectangle_set_todo_mode_enabled);
  RUN_TEST(t_rectangle_get_todo_mode_enabled);
  RUN_TEST(t_rectangle_kill);
  RUN_TEST(t_rectangle_run);
  RUN_TEST(t_rectangle_get_todo_keys);
}

GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_rectangle_test);
  GREATEST_MAIN_END();
}
