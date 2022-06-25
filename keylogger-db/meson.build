keylogger_db_srcs = [
  'keylogger-db.c',
  'keylogger-db.h',
]
keylogger_db_dirs = [
  '../submodules/meson_deps',
  '../submodules/meson_deps/submodules',
  '../submodules/meson_deps/submodules/dbg',
  '../keylogger',
  '../submodules/meson_deps',
  '../submodules',
  '.',
  '../',
  inc,
]
keylogger_db_deps = [
  c_vector_dep,
  generic_print_dep,
  timestamp_dep,
  c_stringfn_dep,
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
