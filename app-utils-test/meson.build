app_utils_test_srcs = [
  'app-utils-test.c',
]
app_utils_test_include_dirs = [
  inc,
  '../window-utils',
  '../app-utils',
]
app_utils_test_deps = [
  greatest_dep,
  c_stringfn_dep,
  app_utils_dep,
  window_utils_dep,
  logh_dep,
  greatest_dep,
]

if get_option('enable-binaries')
  app_utils_test_exec = executable('app-utils-test',
     app_utils_test_srcs,
     dependencies: app_utils_test_deps,
     include_directories: app_utils_test_include_dirs,
  )
  if get_option('enable-binary-tests')
    test('app-utils-test', passh_program, args:[app_utils_test_exec,'-v'])
  endif
endif
