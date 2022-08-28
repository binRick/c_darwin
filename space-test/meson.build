space_test_srcs = [
  'space-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
space_test_include_dirs = [
  '.',
  inc,
]
space_test_deps = [
  c_greatest_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  space_dep,
  ansi_codes_dep,
  logh_dep,
  timestamp_dep,
  window_utils_dep,
  active_app_dep,
  core_utils_dep,
  process_dep,
]
space_test_c_args = [
]
space_test_link_args = [
]

space_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  space_test_exec = executable('space-test',
     space_test_srcs,
     dependencies: space_test_deps,
     include_directories: space_test_include_dirs,
     link_args: space_test_link_args,
     c_args: space_test_c_args,
     install: false,
  )
  test('space-test', 
    space_test_exec, args: ['-v','-a'],
    workdir: space_test_test_dir,
  )
endif
