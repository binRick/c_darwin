monitor_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

monitor_utils_include_dirs = [
  '.',
  inc,
]

monitor_utils_deps = [
  core_dep,
  libmonitors_dep,
]

monitor_utils_c_args = [
]

monitor_utils_link_args = [
]

monitor_utils_lib = library('monitor-utils',
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
