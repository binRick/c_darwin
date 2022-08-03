keylogger_config_test_srcs = [
  'keylogger-config-test.c',
]
keylogger_config_test_inc = [
 '../keylogger',
 '../keylogger-config-test',
 '.',
 inc,
]
keylogger_config_test_deps = [
 c_string_buffer_dep,
 c_greatest_dep,
 module_dep,
 keylogger_config_dep,
 ansi_codes_dep,
]

if get_option('enable-binaries')
  keylogger_config_test = executable('keylogger-config-test',
     keylogger_config_test_srcs,
     dependencies: keylogger_config_test_deps,
     include_directories: keylogger_config_test_inc,
  )
  test('keylogger-config-test', keylogger_config_test, args: [])
endif
