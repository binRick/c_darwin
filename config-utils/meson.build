config_utils_srcs = [
  'config-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

config_utils_include_dirs = [
  '.',
  inc,
]

config_utils_deps = [
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

config_utils_c_args = [
]

config_utils_link_args = [
]

config_utils_lib = static_library('config-utils',
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
