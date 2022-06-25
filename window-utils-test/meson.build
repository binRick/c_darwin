window_utils_test_srcs = [
  'window-utils-test.c',
]
window_utils_test_include_dirs = [
       inc,
       '../window-utils',
       '../app-utils',
]
window_utils_test_deps = [
  c_stringfn_dep,
  app_utils_dep,
  window_utils_dep,
  logh_dep,
  greatest_dep,
]

if get_option('enable-binaries')
  window_utils_test_exec = executable('window-utils-test',
     window_utils_test_srcs,
     dependencies: window_utils_test_deps,
     include_directories: window_utils_test_include_dirs,
  )
  if get_option('enable-binary-tests')
    test('window-utils-test', passh_program, args:[ window_utils_test_exec])
  endif
endif
