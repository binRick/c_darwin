table_sort_srcs = [
  'sort.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

table_sort_include_dirs = [
  '.',
  inc,
]

table_sort_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
]

table_sort_c_args = [
]

table_sort_link_args = [
]

table_sort_lib = library('window-sort',
  table_sort_srcs,
  install: false,
  dependencies: table_sort_deps,
  c_args: table_sort_c_args,
  include_directories: table_sort_include_dirs
)

table_sort_dep = declare_dependency(
  include_directories: table_sort_include_dirs,
  link_with: table_sort_lib,
  link_args: table_sort_link_args,
)
