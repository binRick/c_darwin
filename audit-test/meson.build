audit_test_srcs = [
  'audit-test.c',
  'audit-test.h',
]
audit_include_dirs = [
  '.',
  '../audit',
  inc,
]
audit_include_cargs = [
  ]
audit_test_deps = [
  greatest_dep,
  audit_dep,
  c_vector_dep,
  kitty_dep,
  c_timer_dep,
  ansi_codes_dep,
  tiny_regex_c_dep,
  socket99_dep,
  c_string_buffer_dep,
  c_stringfn_dep,
  c_fsio_dep,
  debug_memory_dep,
  audit_dep,
]
if get_option('enable-binaries')
  audit_test_exec = executable('audit-test',
     audit_test_srcs,
     dependencies: audit_test_deps,
     include_directories: audit_include_dirs,
     c_args: audit_include_cargs,
  )
  if get_option('enable-binary-tests')
    #    test('audit-test', passh_program, args: [audit_test_exec])
  endif
endif
