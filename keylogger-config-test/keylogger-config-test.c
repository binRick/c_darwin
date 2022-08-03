//////////////////////////////////////////////
#include <stdbool.h>
#include <stdio.h>

//////////////////////////////////////////////
#include "ansi-codes//ansi-codes.h"
#include "keylogger-config-test.h"
#include "keylogger-config.h"
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
//////////////////////////////////////////////
#include "c_greatest/greatest/greatest.h"
//////////////////////////////////////////////
__attribute__((constructor))static void __test_init(void);

__attribute__((destructor))static void __test_deinit(void);

module(keylogger_config) * keylogger_config;


//////////////////////////////////////////////


TEST t_keylogger_config_stop(){
  bool ok = keylogger_config->stop();

  ASSERT_EQm("Keylogger Failed to Stop", ok, true);
  PASS();
}


TEST t_keylogger_config_start(){
  bool ok = keylogger_config->start();

  ASSERT_EQm("Keylogger Failed to Start", ok, true);
  PASS();
}


TEST t_keylogger_config_validate_active_keybinds(){
  size_t qty = keylogger_config->get_active_keybinds_qty();

  printf(AC_GREEN "keylogger config parsed %lu active key bindings\n" AC_RESETALL, qty);
  ASSERT_GTEm("Keylogger Keybindings failed to load", qty, 1);
  PASS();
}


TEST t_keylogger_config_validate_inactive_keybinds(){
  size_t qty = keylogger_config->get_inactive_keybinds_qty();

  printf(AC_GREEN "keylogger config parsed %lu inactive key bindings\n" AC_RESETALL, qty);
  ASSERT_GTEm("Keylogger Keybindings failed to load", qty, 1);
  PASS();
}


TEST t_keylogger_config_validate_configured_keybinds(){
  size_t qty          = keylogger_config->get_keybinds_qty();
  size_t active_qty   = keylogger_config->get_active_keybinds_qty();
  size_t inactive_qty = keylogger_config->get_inactive_keybinds_qty();

  printf(AC_GREEN "keylogger config parsed %lu key bindings\n" AC_RESETALL, qty);
  ASSERT_GTm("Keylogger Keybindings failed to load", qty, 1);
  ASSERT_EQ(active_qty + inactive_qty, qty);

  PASS();
}


TEST t_keylogger_config_parse_config(){
  bool ok = keylogger_config->parse_config();

  ASSERT_EQm("Keylogger Failed to Parse Config", ok, true);
  printf(AC_GREEN "keylogger config parsed OK!\n" AC_RESETALL);
  PASS();
}


TEST t_keylogger_config_read_config(){
  bool ok = keylogger_config->read_config();

  ASSERT_EQm("Keylogger Failed to Read Config", ok, true);
  printf(AC_GREEN "keylogger config read OK!\n" AC_RESETALL);
  PASS();
}


TEST t_keylogger_config_check_started_ts(){
  if (keylogger_config->get_is_started() == false) {
    bool ok = keylogger_config->start();
    ASSERT_EQm("Keylogger Failed to Start", ok, true);
  }

  size_t ts = keylogger_config->get_started_ts();
  printf(AC_GREEN "keylogger started @%lu!\n" AC_RESETALL, ts);
  ASSERT_GTm("Keylogger Failed to Start", ts, 0);
  PASS();
}


TEST t_keylogger_config_check_pid(){
  pid_t pid = keylogger_config->get_pid();

  ASSERT_GTm("Keylogger Failed to Start", pid, 0);
  printf(AC_GREEN "keylogger pid %d OK!\n" AC_RESETALL, pid);
  PASS();
}


TEST t_keylogger_config_check_started(){
  bool started = keylogger_config->get_is_started();

  printf(AC_GREEN "keylogger is started!\n" AC_RESETALL);
  ASSERT_EQm("Keylogger Failed to Start", started, true);
  PASS();
}


SUITE(s_keylogger_config){
  RUN_TEST(t_keylogger_config_read_config);
  RUN_TEST(t_keylogger_config_parse_config);
  RUN_TEST(t_keylogger_config_validate_configured_keybinds);
  RUN_TEST(t_keylogger_config_validate_active_keybinds);
  RUN_TEST(t_keylogger_config_validate_inactive_keybinds);
  RUN_TEST(t_keylogger_config_start);
  RUN_TEST(t_keylogger_config_check_started);
  RUN_TEST(t_keylogger_config_check_started_ts);
  RUN_TEST(t_keylogger_config_check_pid);
  RUN_TEST(t_keylogger_config_stop);
}

GREATEST_MAIN_DEFS();


int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_keylogger_config);
  GREATEST_MAIN_END();
}

__attribute__((constructor))static void __test_init(void){
  fprintf(stdout, "=======================TEST INIT=============================\n");
  keylogger_config = init_keylogger_config();
  keylogger_config = init_keylogger_config();
}
__attribute__((destructor))static void __test_deinit(void){
  fprintf(stdout, "=======================TEST DEINIT=============================\n");
  deinit_keylogger_config(keylogger_config);
}
