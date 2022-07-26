pasteboard_test_srcs = [
  'pasteboard-test.c',
]
pasteboard_include_dirs = [
  '.',
  '../pasteboard',
  inc,
]
pasteboard_test_deps = [
  greatest_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  pasteboard_dep,
  c_vector_dep,
  ansi_utils_dep,
  bytes_dep,
  bench_dep,
]

if get_option('enable-binaries')
  pasteboard_test_exec = executable('pasteboard-test',
     pasteboard_test_srcs,
     dependencies: pasteboard_test_deps,
     include_directories: pasteboard_include_dirs,
  )
  if get_option('enable-binary-tests')
    test('pasteboard-test', passh_program, args:[pasteboard_test_exec,'-v'])
  endif
endif
