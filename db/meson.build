db_srcs = [
  'db.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

db_include_dirs = [
  '.',
  inc,
]

db_deps = [
  core_utils_dep,core_dep,vips_dep,capture_type_dep,capture_utils_dep,
  ansi_codes_dep,whereami_dep,core_utils_dep,space_utils_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  sqldbal_dep,window_utils_dep,
]

db_c_args = [
]

db_link_args = [
]

db_lib = library('db',
  db_srcs,
  install: false,
  dependencies: db_deps,
  c_args: db_c_args,
  include_directories: db_include_dirs
)

db_dep = declare_dependency(
  include_directories: db_include_dirs,
  link_with: db_lib,
  link_args: db_link_args,
)
