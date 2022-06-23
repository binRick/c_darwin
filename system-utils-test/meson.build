system_utils_test_srcs = [
  'system-utils-test.c',
]
system_utils_include_dirs = [
  inc,
  '../system-utils',
]
system_utils_test_deps = [
  greatest_dep,
  logh_dep,
  c_stringfn_dep,
  system_utils_dep,
  dbg_dep,
  c_vector_dep,
]

if get_option('enable-binaries')
  system_utils_test_exec = executable('system-utils-test',
     [
      system_utils_test_srcs,
     ],
     dependencies: system_utils_test_deps,
     include_directories: [
       system_utils_include_dirs,
       inc,
     ],
  )
  if get_option('enable-binary-tests')
    test('system-utils-test', system_utils_test_exec, args: [])
  endif
endif
