layout_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

layout_utils_include_dirs = [
  '.',
  inc,
]

layout_utils_deps = [
  core_dep,
  layout_dep,
]

layout_utils_c_args = [
]

layout_utils_link_args = [
]

layout_utils_lib = library('layout',
  layout_utils_srcs,
  install: false,
  dependencies: layout_utils_deps,
  c_args: layout_utils_c_args,
  include_directories: layout_utils_include_dirs
)

layout_utils_dep = declare_dependency(
  include_directories: layout_utils_include_dirs,
  link_with: layout_utils_lib,
  link_args: layout_utils_link_args,
)
