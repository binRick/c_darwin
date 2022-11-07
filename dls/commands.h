#pragma once
#ifndef DLS_COMMANDS_H
#define DLS_COMMANDS_H
#ifndef __XS
#define __XS(X)    #X
#endif
#ifndef _XS
#define _XS(X)     __XS(X)
#endif
#define COMMAND_TEST_ADD_TEST_PROTOTYPE(COMMAND_NAME, SUBCOMMAND_NAME) \
  int __command_ ## COMMAND_NAME ## _test_ ## SUBCOMMAND_NAME();
#define COMMAND_TEST_ADD_TEST_FUNCTION(COMMAND_NAME, SUBCOMMAND_NAME, SUITE_FILTER, TEST_FILTER, VERBOSITY, DESC)            \
  int _command_ ## COMMAND_NAME ## _test_ ## SUBCOMMAND_NAME(){                                                              \
    initialize_args(args);                                                                                                   \
    debug_dls_arguments();                                                                                                   \
    if (__command_ ## COMMAND_NAME ## _test_ ## SUBCOMMAND_NAME(DLS_EXECUTABLE_ARGC, DLS_EXECUTABLE_ARGV) != EXIT_SUCCESS) { \
      log_warn("test run failed");                                                                                           \
      exit(EXIT_FAILURE);                                                                                                    \
    }                                                                                                                        \
    exit(EXIT_SUCCESS);                                                                                                      \
  }                                                                                                                          \
  int __command_ ## COMMAND_NAME ## _test_ ## SUBCOMMAND_NAME(int argc, char **argv){                                        \
    GREATEST_MAIN_BEGIN();                                                                                                   \
    char *s[10];                                                                                                             \
    asprintf(&(s[0]), "s_%s%s", _XS(SUBCOMMAND_NAME), SUITE_FILTER);                                                         \
    asprintf(&(s[1]), "t_%s%s", _XS(SUBCOMMAND_NAME), TEST_FILTER);                                                          \
    greatest_set_suite_filter(s[0]);                                                                                         \
    greatest_set_test_filter(s[1]);                                                                                          \
    greatest_set_verbosity(VERBOSITY);                                                                                       \
    if (args->verbose_mode) greatest_set_verbosity(2);                                                                       \
    if (args->verbose_mode) log_info("Suite Filter: '%s', Test Filter: '%s'", s[0], s[1]);                                   \
    if (args->abort_tests_mode) greatest_abort_on_fail();                                                                    \
    if (args->list_suites_mode) greatest_list_suites_only();                                                                 \
    if (args->list_tests_mode) greatest_list_tests_only();                                                                   \
    RUN_SUITE(s_ ## SUBCOMMAND_NAME);                                                                                        \
    GREATEST_MAIN_END();                                                                                                     \
    free(s[0]);                                                                                                              \
    free(s[1]);                                                                                                              \
  }                                                                                                                          \
//////////////////////////////////////////////////////
#define ADD_COMMAND_ENUMS() \
  ADD_TEST_COMMAND_ENUMS()  \
//////////////////////////////////////////////////////
#define ADD_COMMAND_PROTOTYPES() \
  ADD_TEST_COMMAND_PROTOTYPES()  \
//////////////////////////////////////////////////////
/*
 */
#include "dls/command-animate.h"
#include "dls/command-capture.h"
#include "dls/command-clipboard.h"
#include "dls/command-display.h"
#include "dls/command-extract.h"
#include "dls/command-layout.h"
#include "dls/command-prompt.h"
#include "dls/command-terminal.h"
#include "dls/command-test.h"
//////////////////////////////////////////////////////
#define ADD_COMMAND_OPTIONS()   \
  ADD_CAPTURE_COMMAND_OPTIONS() \
//////////////////////////////////////////////////////
/*
 */
//////////////////////////////////////////////////////
#define ADD_COMMANDS()     \
  ADD_CAPTURE_COMMANDS()   \
  ADD_EXTRACT_COMMANDS()   \
  ADD_ANIMATE_COMMANDS()   \
  ADD_CLIPBOARD_COMMANDS() \
  ADD_LAYOUT_COMMANDS()    \
  ADD_PROMPT_COMMANDS()    \
  ADD_TEST_COMMANDS()      \
  ADD_DISPLAY_COMMANDS()   \
  ADD_TERMINAL_COMMANDS()  \
//////////////////////////////////////////////////////
/*
 */
//////////////////////////////////////////////////////
#define ADD_SUBCOMMANDS()     \
  ADD_TEST_SUBCOMMANDS()      \
  ADD_CAPTURE_SUBCOMMANDS()   \
  ADD_EXTRACT_SUBCOMMANDS()   \
  ADD_ANIMATE_SUBCOMMANDS()   \
  ADD_CLIPBOARD_SUBCOMMANDS() \
  ADD_LAYOUT_SUBCOMMANDS()    \
  ADD_PROMPT_SUBCOMMANDS()    \
  ADD_DISPLAY_SUBCOMMANDS()   \
  ADD_TERMINAL_SUBCOMMANDS()  \
/////////////////////////////////////////////////////
/*
 */
/////////////////////////////////////////////////////
#endif
