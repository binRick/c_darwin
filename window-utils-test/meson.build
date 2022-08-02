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
  c_string_buffer_dep,
  string_utils_dep,
  app_utils_dep,
  window_utils_dep,
  logh_dep,
  c_greatest_dep,
  dbg_dep,
  c_timer_dep,
  capture_dep,
  c_fsio_dep,
  fort_dep,
]

if get_option('enable-binaries')
  window_utils_test_exec = executable('window-utils-test',
     window_utils_test_srcs,
     dependencies: window_utils_test_deps,
     include_directories: window_utils_test_include_dirs,
  )
  if get_option('enable-binary-tests')
    test('window-utils-test', window_utils_test_exec, args:['-v','-a'])
  endif
endif
