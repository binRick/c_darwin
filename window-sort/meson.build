window_sort_srcs = [
  'window-sort.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

window_sort_include_dirs = [
  '.',
  inc,
]

window_sort_deps = [
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

window_sort_c_args = [
]

window_sort_link_args = [
]

window_sort_lib = library('window-sort',
  window_sort_srcs,
  install: false,
  dependencies: window_sort_deps,
  c_args: window_sort_c_args,
  include_directories: window_sort_include_dirs
)

window_sort_dep = declare_dependency(
  include_directories: window_sort_include_dirs,
  link_with: window_sort_lib,
  link_args: window_sort_link_args,
)
