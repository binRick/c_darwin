process_test_srcs = [
  'test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
process_test_include_dirs = [
  '.',
  inc,
]
process_test_deps = [
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
  process_utils_dep,
]
process_test_c_args = [
]
process_test_link_args = [
]

process_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  process_test_exec = executable('process-test',
     process_test_srcs,
     dependencies: process_test_deps,
     include_directories: process_test_include_dirs,
     link_args: process_test_link_args,
     c_args: process_test_c_args,
     install: false,
  )
  #test('process-test', 
  #  process_test_exec, args: ['-v','-a'],
  #  workdir: process_test_dir,
  #)
endif
