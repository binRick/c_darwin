record_focused_window_test_srcs = [
  'record-focused-window-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
record_focused_window_test_include_dirs = [
  '.',
  inc,
]
record_focused_window_test_deps = [
  c_greatest_dep,
  wrec_capture_dep,
  c_vector_dep,
  ansi_codes_dep,
  c_string_buffer_dep,
  c_stringfn_dep,
  c_fsio_dep,
  bytes_dep,
  ms_dep,
  tempdir_dep,
  timestamp_dep,
]
record_focused_window_test_c_args = [
]
record_focused_window_test_link_args = [
]

record_focused_window_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  record_focused_window_test_exec = executable('record-focused-window-test',
     record_focused_window_test_srcs,
     dependencies: record_focused_window_test_deps,
     include_directories: record_focused_window_test_include_dirs,
     link_args: record_focused_window_test_link_args,
     c_args: record_focused_window_test_c_args,
     install: false,
  )
  #test('record-focused-window-test', 
  #  record_focused_window_test_exec, args: ['-v','-a'],
  #  workdir: record_focused_window_test_test_dir,
  #)
endif
