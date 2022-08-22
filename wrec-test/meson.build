wrec_test_srcs = [
  'wrec-test.c',
]
wrec_include_dirs = [
  '.',
  '../wrec-cli',
  inc,
]
wrec_include_cargs = [
  ]
wrec_test_deps = [
  c_greatest_dep,
  wrec_utils_dep,
  capture_dep,
  tempdir_dep,
  c_fsio_dep,
  c_stringfn_dep,
  timestamp_dep,
]
if get_option('enable-binaries')
  wrec_test_exec = executable('wrec-test',
     wrec_test_srcs,
     dependencies: wrec_test_deps,
     include_directories: wrec_include_dirs,
     c_args: wrec_include_cargs,
  )
  if get_option('enable-binary-tests')
    test('wrec-test', passh_program, args: [wrec_test_exec])
  endif
endif
