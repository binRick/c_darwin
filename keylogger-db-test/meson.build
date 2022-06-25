keylogger_db_test_srcs = [
  'keylogger-db-test.c',
  'keylogger-db-test.h',
]
keylogger_db_test_dirs = [
  '.',
  '../',
  '../submodules',
  inc,
]
keylogger_db_test_deps = [
  c_vector_dep,
  env_h_dep,
  throw_h_dep,
  pbpaste_dep,
  timestamp_dep,
  c_stringfn_dep,
  keylogger_db_dep,
  greatest_dep,
  c_fsio_dep,
  c_string_buffer_dep,
  logh_dep,
  sqldbal_dep,
  sqlite3_dep,
  c_forever_dep,
  dbg_dep,
  c_eventemitter_dep,
  dmt_dep,
  ansi_utils_dep,
]
keylogger_db_test_c_args = [
]
keylogger_db_test_link_args = [
]
if get_option('enable-binaries')
  keylogger_db_test_exec = executable('keylogger-db-test',
     keylogger_db_test_srcs,
     dependencies: keylogger_db_test_deps,
     include_directories: keylogger_db_test_dirs,
     link_args: keylogger_db_test_link_args,
     c_args: keylogger_db_test_c_args,
     install: true,
     pie: true,
  )
  if get_option('enable-binary-tests')
    test('keylogger-db-test', 
      passh_program, args: [keylogger_db_test_exec,'-v','-a'],
      timeout: 5,
      is_parallel: false,
      workdir: meson.current_source_dir(),
      verbose: true,
      )
  endif
endif
