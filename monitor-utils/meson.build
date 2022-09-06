monitor_utils_srcs = [
  'monitor-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

monitor_utils_include_dirs = [
  '.',
  inc,
]

monitor_utils_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  libmonitors_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
]

monitor_utils_c_args = [
]

monitor_utils_link_args = [
]

monitor_utils_lib = static_library('monitor-utils',
  monitor_utils_srcs,
  install: false,
  dependencies: monitor_utils_deps,
  c_args: monitor_utils_c_args,
  include_directories: monitor_utils_include_dirs
)

monitor_utils_dep = declare_dependency(
  include_directories: monitor_utils_include_dirs,
  link_with: monitor_utils_lib,
  link_args: monitor_utils_link_args,
)
