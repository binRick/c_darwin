process_utils_test_srcs = [
  'test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
process_utils_test_include_dirs = [
  '.',
  inc,
]
process_utils_test_deps = [
  c_greatest_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  process_utils_dep,window_info_dep,
  process_dep,window_utils_dep,
  app_utils_dep,core_utils_dep,
]
process_utils_test_c_args = [
]
process_utils_test_link_args = [
]

process_utils_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  process_utils_test_exec = executable('process-utils-test',
     process_utils_test_srcs,
     dependencies: process_utils_test_deps,
     include_directories: process_utils_test_include_dirs,
     link_args: process_utils_test_link_args,
     c_args: process_utils_test_c_args,
     install: false,
  )
  test('process-utils-test', 
    process_utils_test_exec, args: ['-v','-a'],
    workdir: process_utils_test_test_dir,
  )
endif
