sqldbal_test_srcs = [
  'sqldbal-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
sqldbal_test_include_dirs = [
  '.',
  inc,
]
sqldbal_test_deps = [
  c_greatest_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,
  sqldbal_dep,
]
sqldbal_test_c_args = [
]
sqldbal_test_link_args = [
]

sqldbal_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  sqldbal_test_exec = executable('sqldbal-test',
     sqldbal_test_srcs,
     dependencies: sqldbal_test_deps,
     include_directories: sqldbal_test_include_dirs,
     link_args: sqldbal_test_link_args,
     c_args: sqldbal_test_c_args,
     install: false,
  )
  test('sqldbal-test', 
    sqldbal_test_exec, args: ['-v','-a'],
    workdir: sqldbal_test_test_dir,
  )
endif
