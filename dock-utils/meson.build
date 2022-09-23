dock_utils_srcs = [
  'dock-utils.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
dock_utils_dirs = [
  '.',
  inc,
]
dock_utils_deps = [
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  display_utils_dep,
  log_dep,
  timestamp_dep,
  ms_dep,
  bytes_dep,
]
dock_utils_c_args = [
]
dock_utils_link_args = osx_link_args

dock_utils__dir = join_paths(meson.current_source_dir(), '..')

dock_utils_lib = library('dock-utils',
  dock_utils_srcs,
  install: false,
  dependencies: dock_utils_deps,
  include_directories: dock_utils_dirs,
)


dock_utils_dep = declare_dependency(
  include_directories: dock_utils_dirs,
  link_with: dock_utils_lib,
  link_args: dock_utils_link_args,
)
