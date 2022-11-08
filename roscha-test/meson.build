roscha_test_srcs = [
  'roscha-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
roscha_test_include_dirs = [
  '.',
  inc,
]
roscha_test_deps = [
  c_greatest_dep,
  ansi_codes_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  roscha_dep,
]
roscha_test_c_args = [
]
roscha_test_link_args = [
]

roscha_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  roscha_test_exec = executable('roscha-test',
     roscha_test_srcs,
     dependencies: roscha_test_deps,
     include_directories: roscha_test_include_dirs,
     link_args: roscha_test_link_args,
     c_args: roscha_test_c_args,
     install: false,
  )
  test('roscha-test', 
    roscha_test_exec, args: ['-v','-a'],
    workdir: roscha_test_test_dir,
  )
endif
