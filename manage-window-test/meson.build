manage_window_test_srcs = [
  'manage-window-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
manage_window_test_include_dirs = [
  '.',
  inc,
]
manage_window_test_deps = [
  c_greatest_dep,
  c_vector_dep,
  c_fsio_dep,
  window_utils_dep, core_utils_dep, app_utils_dep,
  c_stringfn_dep,process_utils_dep,
  ansi_codes_dep,
  bytes_dep, ms_dep,
  log_dep,
  timestamp_dep,
]
manage_window_test_c_args = [
]
manage_window_test_link_args = [
]

manage_window_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  manage_window_test_exec = executable('manage-window-test',
     manage_window_test_srcs,
     dependencies: manage_window_test_deps,
     include_directories: manage_window_test_include_dirs,
     link_args: manage_window_test_link_args,
     c_args: manage_window_test_c_args,
     install: false,
  )
  #test('manage-window-test', 
  #  manage_window_test_exec, args: ['-v','-a'],
  #  workdir: manage_window_test_test_dir,
  #)
endif
