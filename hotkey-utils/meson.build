hotkey_utils_srcs = [
  'hotkey-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

hotkey_utils_include_dirs = [
  '.',
  inc,
]

hotkey_utils_deps = [
  core_dep,
  keylogger_dep,layout_utils_dep,
  display_utils_dep,
  path_module_dep,window_utils_dep,app_utils_dep,vips_dep,
  capture_utils_dep,capture_type_dep,capture_dep,image_utils_dep,
  hash_dep,c_exception_dep,
]

hotkey_utils_c_args = [
]

hotkey_utils_link_args = [
]

hotkey_utils_lib = library('hotkey-utils',
  hotkey_utils_srcs,
  install: false,
  dependencies: hotkey_utils_deps,
  c_args: hotkey_utils_c_args,
  include_directories: hotkey_utils_include_dirs
)

hotkey_utils_dep = declare_dependency(
  include_directories: hotkey_utils_include_dirs,
  link_with: hotkey_utils_lib,
  link_args: hotkey_utils_link_args,
)
