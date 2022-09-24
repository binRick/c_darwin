monitor_srcs = [
  'monitor.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

monitor_include_dirs = [
  '.',
  inc,
]

monitor_deps = [
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

monitor_c_args = [
]

monitor_link_args = [
]

monitor_lib = library('monitor',
  monitor_srcs,
  install: false,
  dependencies: monitor_deps,
  c_args: monitor_c_args,
  include_directories: monitor_include_dirs
)

monitor_dep = declare_dependency(
  include_directories: monitor_include_dirs,
  link_with: monitor_lib,
  link_args: monitor_link_args,
)
subdir('utils')
