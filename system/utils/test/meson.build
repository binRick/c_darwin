system_utils_test_srcs = [
  'test.c',
]
system_utils_include_dirs = [
  '.',
  inc,
]
system_utils_test_deps = [
  c_greatest_dep,
  system_utils_dep,
  process_dep,
  log_dep,
  c_stringfn_dep,process_utils_dep,window_info_dep,app_utils_dep,display_utils_dep,
  system_utils_dep,
  dbg_dep,
  c_vector_dep,window_utils_dep,
  frameworks_dep,
]

if get_option('enable-binaries')
  system_utils_test_exec = executable('system-utils-test',
     system_utils_test_srcs,
     dependencies: system_utils_test_deps,
     include_directories: system_utils_include_dirs,
     link_args: osx_ldflags,
  )
  if get_option('enable-binary-tests')
    test('system-utils-test', passh_program, args:[system_utils_test_exec,'-v'])
  endif
endif
