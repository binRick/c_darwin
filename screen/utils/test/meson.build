screen_utils_test_srcs = [
  'test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
screen_utils_test_include_dirs = [
  '.',
  inc,
]
screen_utils_test_deps = [
  c_greatest_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  screen_utils_dep,
  log_dep,
]
screen_utils_test_c_args = [
]
screen_utils_test_link_args = [
]

screen_utils_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  screen_utils_test_exec = executable('screen-utils-test',
     screen_utils_test_srcs,
     dependencies: screen_utils_test_deps,
     include_directories: screen_utils_test_include_dirs,
     link_args: screen_utils_test_link_args,
     c_args: screen_utils_test_c_args,
     install: false,
  )
  #  test('screen-utils-test', 
  #  screen_utils_test_exec, args: ['-v','-a'],
  #  workdir: screen_utils_test_test_dir,
  #)
endif
