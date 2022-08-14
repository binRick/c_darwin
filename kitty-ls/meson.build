process_test_srcs = [
  'kitty-ls.c',
  'kitty-ls.h',
]
process_include_dirs = [
  '.',
  '../process',
  inc,
]
process_include_cargs = [
  ]
process_test_deps = [
  process_dep,
  timestamp_dep,
  c_vector_dep,
  wildcardcmp_dep,
]
if get_option('enable-binaries')
  process_test_exec = executable('kitty-ls',
     process_test_srcs,
     dependencies: process_test_deps,
     include_directories: process_include_dirs,
     c_args: process_include_cargs,
     install: true,
     install_dir: '/usr/local/bin',
  )
  if get_option('enable-binary-tests')
    #test('kitty-ls', passh_program, args: [process_test_exec])
  endif
endif
