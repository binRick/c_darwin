keylogger_test_srcs = [
  'keylogger-test.c',
]
keylogger_test_inc = [
 '../keylogger',
 '../keylogger-test',
 '.',
 inc,
]
keylogger_test_deps = [
 c_string_buffer_dep,
 greatest_dep,
 keylogger_dep,
]

if get_option('enable-binaries')
  keylogger_test = executable('keylogger-test',
     keylogger_test_srcs,
     dependencies: keylogger_test_deps,
     include_directories: keylogger_test_inc,
  )
  if get_option('enable-binary-tests')
    #    test('keylogger-test', keylogger_test, args: [])
  endif
endif
