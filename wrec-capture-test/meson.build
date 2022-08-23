wrec_capture_test_srcs = [
  'wrec-capture-test.c',
]
wrec_capture_test_include_dirs = [
  '.',
  '../wrec-capture',
  inc,
]
wrec_capture_test_include_cargs = [
  ]
wrec_capture_test_deps = [
  wrec_common_dep,
  libbeaufort_dep,
  c_greatest_dep,
  metalang99_dep,
  interface99_dep,
 wrec_capture_dep,
  c_vector_dep,
  kitty_dep,
  c_timer_dep,
  ansi_codes_dep,
  tiny_regex_c_dep,
  c_dbg_dep,
  socket99_dep,
  c_string_buffer_dep,
  c_stringfn_dep,
  bench_dep,
  c_fsio_dep,
  bytes_dep,
  wrec_utils_dep,
  ms_dep,
  tempdir_dep,
  debug_memory_dep,
]
if get_option('enable-binaries')
  wrec_capture_test_exec = executable('wrec-capture-test',
     wrec_capture_test_srcs,
     dependencies: wrec_capture_test_deps,
     include_directories: wrec_capture_test_include_dirs,
     c_args: wrec_capture_test_include_cargs,
  )
  if get_option('enable-binary-tests')
    test('wrec-capture-test', passh_program, args: [wrec_capture_test_exec])
  endif
endif
