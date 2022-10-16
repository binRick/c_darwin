process_test_srcs = [
  'process-test.c',
  'process-test.h',
]
process_include_dirs = [
  '.',
  inc,
]
process_include_cargs = [
  '-Wno-unused-parameter',
  '-Wno-unused-function',
  ]
process_test_deps = [
  core_dep,
  c_greatest_dep,
  process_dep,
  kitty_dep,
  process_utils_dep,window_info_dep,app_utils_dep,
  window_utils_dep,app_utils_dep,core_utils_dep,window_utils_dep,
]
if get_option('enable-binaries')
  process_test_exec = executable('process-test',
     process_test_srcs,
     dependencies: process_test_deps,
     include_directories: process_include_dirs,
     c_args: process_include_cargs,
     link_args: osx_ldflags,
  )
  test('process-test', passh_program, args: [process_test_exec])
endif
