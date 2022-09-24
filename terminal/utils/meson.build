terminal_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

terminal_utils_include_dirs = [
  '.',
  inc,
]

terminal_utils_deps = [
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

terminal_utils_c_args = [
]

terminal_utils_link_args = [
]

terminal_utils_lib = library('terminal-utils',
  terminal_utils_srcs,
  install: false,
  dependencies: terminal_utils_deps,
  c_args: terminal_utils_c_args,
  include_directories: terminal_utils_include_dirs
)

terminal_utils_dep = declare_dependency(
  include_directories: terminal_utils_include_dirs,
  link_with: terminal_utils_lib,
  link_args: terminal_utils_link_args,
)
