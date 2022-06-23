process_environment_test_srcs = [
  'process-environment-test.c',
  'process-environment-test.h',
]
process_environment_include_dirs = [
  '.',
  '../process-environment',
  inc,
]
process_environment_test_deps = [
  greatest_dep,
  process_environment_dep,
  c_vector_dep,
]
if get_option('enable-binaries')
  process_environment_test_exec = executable('process-environment-test',
     [
      process_environment_test_srcs,
     ],
     dependencies: process_environment_test_deps,
     include_directories: [
       process_environment_include_dirs,
       inc,
     ],
  )
  if get_option('enable-binary-tests')
    test('process-environment-test', process_environment_test_exec, args: [])
  endif
endif
