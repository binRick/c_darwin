system_utils_test_srcs = [
  'system-utils-test.c',
]
system_utils_include_dirs = [
  '.',
  '../system-utils',
  inc,
]
system_utils_test_deps = [
  c_greatest_dep,
  system_utils_dep,
  process_dep,
  logh_dep,
  c_stringfn_dep,process_utils_dep,app_utils_dep,display_utils_dep,
  system_utils_dep,
  dbg_dep,
  c_vector_dep,
]

if get_option('enable-binaries')
  system_utils_test_exec = executable('system-utils-test',
     system_utils_test_srcs,
     dependencies: system_utils_test_deps,
     include_directories: system_utils_include_dirs,
  )
  if get_option('enable-binary-tests')
    test('system-utils-test', passh_program, args:[system_utils_test_exec,'-v'])
  endif
endif
