menu_bar_utils_srcs = [
  'menu-bar-utils.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
menu_bar_utils_dirs = [
  '.',
  inc,
]
menu_bar_utils_deps = [
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  display_utils_dep,
  logh_dep,
  timestamp_dep,
  ms_dep,
  space_utils_dep,
  bytes_dep,
]
menu_bar_utils_c_args = [
]
menu_bar_utils_link_args = osx_link_args

menu_bar_utils_dir = join_paths(meson.current_source_dir(), '..')

menu_bar_utils_lib = static_library('menu-bar-utils',
  menu_bar_utils_srcs,
  install: false,
  dependencies: menu_bar_utils_deps,
  include_directories: menu_bar_utils_dirs,
)


menu_bar_utils_dep = declare_dependency(
  include_directories: menu_bar_utils_dirs,
  link_with: menu_bar_utils_lib,
  link_args: menu_bar_utils_link_args,
)
