capture_test_srcs = [
  'capture-test.c',
  'capture-test.h',
]
capture_include_dirs = [
  '.',
  '../capture',
  inc,
]
capture_include_cargs = [
  ]
capture_test_deps = [
  greatest_dep,
  capture_dep,
  c_vector_dep,
  kitty_dep,
  c_timer_dep,
  ansi_codes_dep,
  tiny_regex_c_dep,
  socket99_dep,
  c_string_buffer_dep,
  c_stringfn_dep,
  bench_dep,
  c_fsio_dep,
  bytes_dep,
  ms_dep,
  tempdir_dep,
  debug_memory_dep,
]
if get_option('enable-binaries')
  capture_test_exec = executable('capture-test',
     capture_test_srcs,
     dependencies: capture_test_deps,
     include_directories: capture_include_dirs,
     c_args: capture_include_cargs,
  )
  if get_option('enable-binary-tests')
    test('capture-test', passh_program, args: [capture_test_exec])
  endif
endif
