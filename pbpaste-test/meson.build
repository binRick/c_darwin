pbpaste_test_srcs = [
  'pbpaste-test.c',
]
pbpaste_test_inc = [
 '../pbpaste',
 '../pbpaste-test',
 '.',
 inc,
]
pbpaste_test_deps = [
 c_string_buffer_dep,
 greatest_dep,
 pbpaste_dep,
]

if get_option('enable-binaries')
  pbpaste_test = executable('pbpaste-test',
     pbpaste_test_srcs,
     dependencies: pbpaste_test_deps,
     include_directories: pbpaste_test_inc,
  )
  if get_option('enable-binary-tests')
    test('pbpaste-test', pbpaste_test, args: [])
  endif
endif
