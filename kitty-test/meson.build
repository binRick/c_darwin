system_utils_test_srcs = [
  'kitty-test.c',
]
system_utils_include_dirs = [
  '.',
  '../kitty',
  inc,
]
system_utils_test_deps = [
  c_greatest_dep,
  kitty_dep,
  system_utils_dep,
  process_dep,
  logh_dep,
  c_stringfn_dep,
  module_dep,
  system_utils_dep,
  dbg_dep,
  c_vector_dep,
  app_utils_dep,
]

if get_option('enable-binaries')
  system_utils_test_exec = executable('kitty-test',
     system_utils_test_srcs,
     dependencies: system_utils_test_deps,
     include_directories: system_utils_include_dirs,
  )
  if get_option('enable-binary-tests')
    test('kitty-test', passh_program, args:[system_utils_test_exec,'-v'])
  endif
endif
