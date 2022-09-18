
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "hotkey-utils-test/hotkey-utils-test.h"
#include "hotkey-utils/hotkey-utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
struct test_cfg_t {
  const char                    **argv;
  const char                    *config_path;
  const struct hotkeys_config_t *cfg;
};
static struct test_cfg_t *test_cfg;

void ensure_test_cfg(){
  if (test_cfg->config_path == NULL) {
    test_cfg->config_path = get_yaml_config_file_path(test_cfg->argv);
  }
  if (test_cfg->cfg == NULL) {
    test_cfg->cfg = load_yaml_config_file_path(test_cfg->config_path);
  }
}

TEST t_hotkey_utils_execute_hotkey_config_key(void *KEY){
  ensure_test_cfg();
  struct key_t *key = get_hotkey_config_key(test_cfg->cfg, (char *)KEY);
  ASSERT_NEQm("Key is NULL", key, NULL);
  log_info("Got key action %d/%s for key %s", key->action_type, key->action, (char *)KEY);
  int rc = execute_hotkey_config_key(key);
  ASSERT_EQ(rc, 0);
  PASS();
}

TEST t_hotkey_utils_get_hotkey_config_key(void *KEY){
  ensure_test_cfg();
  struct key_t *key = get_hotkey_config_key(test_cfg->cfg, (char *)KEY);
  ASSERT_NEQm("Key is NULL", key, NULL);
  log_info("Got key action %d/%s for key %s", key->action_type, key->action, (char *)KEY);
  PASS();
}

TEST t_hotkey_utils_activate_application(void *APPLICATION_NAME){
  ensure_test_cfg();
  int rc = handle_action(ACTION_TYPE_ACTIVATE_APPLICATION, APPLICATION_NAME);
  ASSERT_EQ(rc, 0);
  PASS();
}

TEST t_hotkey_utils_load_config(){
  ensure_test_cfg();
  ASSERT_NEQm("Config is NULL", test_cfg->cfg, NULL);
  log_info("Config Name: %s", test_cfg->cfg->name);
  log_info("# Hotkeys: %lu", test_cfg->cfg->keys_count);
  for (size_t i = 0; i < test_cfg->cfg->keys_count; i++) {
    log_info("Hotkey #%lu %s %s -> %s [Enabled: %s] [Action Type: %s (%d)]",
             i,
             test_cfg->cfg->keys[i].name,
             test_cfg->cfg->keys[i].key,
             test_cfg->cfg->keys[i].action,
             (test_cfg->cfg->keys[i].enabled == true) ? "Yes" : "No",
             action_type_strings[test_cfg->cfg->keys[i].action_type].str,
             test_cfg->cfg->keys[i].action_type
             );
  }
  PASS();
}

TEST t_hotkey_utils_config_path(){
  test_cfg->config_path = get_yaml_config_file_path(test_cfg->argv);
  log_info("config path: %s", test_cfg->config_path);
  PASS();
}

SUITE(s_hotkey_utils_test) {
  RUN_TEST(t_hotkey_utils_config_path);
  RUN_TEST(t_hotkey_utils_load_config);
//  RUN_TESTp(t_hotkey_utils_activate_application, (void *)"Alacritty");
  RUN_TESTp(t_hotkey_utils_get_hotkey_config_key, (void *)"ctrl-5");
  RUN_TESTp(t_hotkey_utils_get_hotkey_config_key, (void *)"alt-shift-[");
//  RUN_TESTp(t_hotkey_utils_execute_hotkey_config_key, (void*)"ctrl-5");
  //RUN_TESTp(t_hotkey_utils_execute_hotkey_config_key, (void *)"ctrl-7");
  RUN_TESTp(t_hotkey_utils_execute_hotkey_config_key, (void *)"alt-shift-]");
  //RUN_TESTp(t_hotkey_utils_execute_hotkey_config_key, (void *)"alt-shift-[");
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  test_cfg       = calloc(1, sizeof(struct test_cfg_t));
  test_cfg->argv = argv;
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_hotkey_utils_test);
  GREATEST_MAIN_END();
}
