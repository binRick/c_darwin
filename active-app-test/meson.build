active_app_test_srcs = [
  'active-app-test.c',
]
active_app_include_dirs = [
  '.',
  inc,
]
active_app_test_deps = [
  active_app_dep,
  c_stringfn_dep,
  app_utils_dep,process_utils_dep,app_utils_dep,
  window_utils_dep,
  log_dep,
  c_greatest_dep,
]

if get_option('enable-binaries')
  active_app_test_exec = executable('active-app-test',
      active_app_test_srcs,
     dependencies: active_app_test_deps,
     include_directories: active_app_include_dirs,
  )
  if get_option('enable-binary-tests')
    test('active-app-test', passh_program, args: [active_app_test_exec,'-v'])
  endif
endif
