keylogger_db_srcs = [
  'keylogger-db.c',
  'keylogger-db.h',
]
keylogger_db_dirs = [
  '../submodules/c_deps',
  '../submodules/c_deps/submodules',
  '../submodules/c_deps/submodules/dbg',
  '../keylogger',
  '../pbpaste',
  '../submodules/c_deps',
  '../submodules',
  '.',
  '../',
  inc,
]
keylogger_db_deps = [
  display_utils_dep,
  hidapi_dep,
  libusb_dep,
  pasteboard_dep,
  active_app_dep,
  process_dep,
  c_stringfn_dep,
  app_utils_dep,
  window_utils_dep,
  log_dep,
  c_greatest_dep,  
  pbpaste_dep,
  system_utils_dep,
  c_vector_dep,
  socket99_dep,
  timestamp_dep,
  c_stringfn_dep,
  c_fsio_dep,
  bytes_dep,
  c_string_buffer_dep,
  log_dep,
  sqldbal_dep,
  sqlite3_dep,
  c_forever_dep,
  dbg_dep,
  c_eventemitter_dep,
  dmt_dep,
  ansi_utils_dep,
]
keylogger_c_args = [
]
keylogger_db_args = [
]
keylogger_db_lib = static_library('keylogger-db',
  keylogger_db_srcs,
  install: false,
  dependencies: keylogger_db_deps,
  include_directories: keylogger_db_dirs,
)

keylogger_db_dep = declare_dependency(
  include_directories: keylogger_db_dirs,
  link_with: keylogger_db_lib,
)
