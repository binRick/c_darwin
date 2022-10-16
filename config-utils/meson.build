config_utils_srcs = [
  'config-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

config_utils_include_dirs = [
  '.',
  inc,
]

config_utils_deps = [
  core_dep,
  process_utils_dep,
  dock_utils_dep,menu_bar_utils_dep,hotkey_utils_dep,monitor_utils_dep,
  capture_utils_dep,
  display_utils_dep,
  space_utils_dep,
  window_utils_dep,vips_dep,
]

config_utils_c_args = [
]

config_utils_link_args = [
]

config_utils_lib = library('config-utils',
  config_utils_srcs,
  install: false,
  dependencies: config_utils_deps,
  c_args: config_utils_c_args,
  include_directories: config_utils_include_dirs
)

config_utils_dep = declare_dependency(
  include_directories: config_utils_include_dirs,
  link_with: config_utils_lib,
  link_args: config_utils_link_args,
)
