subdir('utils')
monitor_srcs = [
  'monitor.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

monitor_include_dirs = [
  '.',
  inc,
]

monitor_deps = [
  core_dep,
  monitor_utils_dep,
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
