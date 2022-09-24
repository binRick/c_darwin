mouse_utils_test_srcs = [
  'test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
mouse_utils_test_include_dirs = [
  '.',
  inc,
]
mouse_utils_test_deps = [
  c_greatest_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  mouse_utils_dep,
  frameworks_dep,
  bytes_dep,
]
mouse_utils_test_c_args = [
]
mouse_utils_test_link_args = [
]

mouse_utils_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  mouse_utils_test_exec = executable('mouse-utils-test',
     mouse_utils_test_srcs,
     dependencies: mouse_utils_test_deps,
     include_directories: mouse_utils_test_include_dirs,
     link_args: mouse_utils_test_link_args,
     c_args: mouse_utils_test_c_args,
     install: false,
  )
  test('mouse-utils-test', 
    mouse_utils_test_exec, args: ['-v','-a'],
    workdir: mouse_utils_test_test_dir,
  )
endif
