process_test_srcs = [
  'process-test.c',
  'process-test.h',
]
process_include_dirs = [
  '.',
  '../process',
  inc,
]
process_include_cargs = [
  ]
process_test_deps = [
  greatest_dep,
  process_dep,
  c_vector_dep,
  c_timer_dep,
  ansi_codes_dep,
  tiny_regex_c_dep,
  socket99_dep,
  c_string_buffer_dep,
  c_stringfn_dep,
  c_fsio_dep,
]
if get_option('enable-binaries')
  process_test_exec = executable('process-test',
     process_test_srcs,
     dependencies: process_test_deps,
     include_directories: process_include_dirs,
     c_args: process_include_cargs,
  )
  if get_option('enable-binary-tests')
    test('process-test', passh_program, args: [process_test_exec])
  endif
endif
