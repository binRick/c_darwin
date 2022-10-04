window_db_srcs = [
  'db.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

window_db_include_dirs = [
  '.',
  inc,
]

window_db_deps = [
  ansi_codes_dep,whereami_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  sqldbal_dep,
]

window_db_c_args = [
]

window_db_link_args = [
]

window_db_lib = library('window_db',
  window_db_srcs,
  install: false,
  dependencies: window_db_deps,
  c_args: window_db_c_args,
  include_directories: window_db_include_dirs
)

window_db_dep = declare_dependency(
  include_directories: window_db_include_dirs,
  link_with: window_db_lib,
  link_args: window_db_link_args,
)
