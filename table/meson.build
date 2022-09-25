subdir('sort')
subdir('utils')
table_srcs = [
  'table.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

table_include_dirs = [
  '.',
  inc,
]

table_deps = [
  ansi_codes_dep,font_utils_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,table_utils_dep,table_sort_dep,
]

table_c_args = [
]

table_link_args = [
]

table_lib = library('table',
  table_srcs,
  install: false,
  dependencies: table_deps,
  c_args: table_c_args,
  include_directories: table_include_dirs
)

table_dep = declare_dependency(
  include_directories: table_include_dirs,
  link_with: table_lib,
  link_args: table_link_args,
)

