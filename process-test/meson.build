process_test_srcs = [
  'process-test.c',
  'process-test.h',
]
process_include_dirs = [
  '.',
  '../process',
  inc,
]
process_test_deps = [
  greatest_dep,
  process_dep,
  c_vector_dep,
]
if get_option('enable-binaries')
  process_test_exec = executable('process-test',
     [
      process_test_srcs,
     ],
     dependencies: process_test_deps,
     include_directories: [
       process_include_dirs,
       inc,
     ],
  )
  if get_option('enable-binary-tests')
    test('process-test', process_test_exec, args: [])
  endif
endif
