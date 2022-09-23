window_info_srcs = [
  'window-info.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

window_info_include_dirs = [
  '.',
  inc,
]

window_info_deps = [
  c_stringfn_dep,window_sort_dep,
  parson_dep,
  log_dep,
  ansi_codes_dep,
  c_vector_dep,
  fort_dep,
  bytes_dep,
  ms_dep,
  process_dep,
  string_utils_dep,string_utils_dep,frameworks_dep,
  timestamp_dep,
  wildcardcmp_dep,
  reproc_dep,which_dep,
]

window_info_c_args = [
]

window_info_link_args = [
  osx_link_args,
]

window_info_lib = library('window-info',
  window_info_srcs,
  install: false,
  dependencies: window_info_deps,
  c_args: window_info_c_args,
  include_directories: window_info_include_dirs
)

window_info_dep = declare_dependency(
  include_directories: window_info_include_dirs,
  link_with: window_info_lib,
  link_args: window_info_link_args,
)
