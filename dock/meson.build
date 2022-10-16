subdir('utils')
dock_srcs = [
  'dock.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

dock_include_dirs = [
  '.',
  inc,
]

dock_deps = [
  core_dep,
  dock_utils_dep,
]

dock_c_args = [
]

dock_link_args = [
]

dock_lib = library('dock',
  dock_srcs,
  install: false,
  dependencies: dock_deps,
  c_args: dock_c_args,
  include_directories: dock_include_dirs
)

dock_dep = declare_dependency(
  include_directories: dock_include_dirs,
  link_with: dock_lib,
  link_args: dock_link_args,
)

