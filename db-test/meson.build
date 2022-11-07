db_test_srcs = [
  'db-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
db_test_include_dirs = [
  '.',
  inc,
]
db_test_deps = [
  c_greatest_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  db_dep,
]
db_test_c_args = [
]
db_test_link_args = [
]

db_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  db_test_exec = executable('db-test',
     db_test_srcs,
     dependencies: db_test_deps,
     include_directories: db_test_include_dirs,
     link_args: db_test_link_args,
     c_args: db_test_c_args,
     install: false,
  )
  test('db-test', 
    db_test_exec, args: ['-v','-a'],
    workdir: db_test_test_dir,
  )
endif
