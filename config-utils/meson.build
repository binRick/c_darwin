config_utils_srcs = [
  'config-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

config_utils_include_dirs = [
  '.',
  inc,
]

config_utils_deps = [
  ansi_codes_dep,parson_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,b64_dep,process_utils_dep,
  timestamp_dep,dock_utils_dep,parson_dep,menu_bar_utils_dep,libcyaml_dep,hotkey_utils_dep,monitor_utils_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  capture_utils_dep,
  display_utils_dep,
  space_utils_dep,
  window_utils_dep,
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
