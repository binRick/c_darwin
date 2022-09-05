terminal_srcs = [
  'terminal.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

terminal_include_dirs = [
  '.',
  inc,
]

terminal_deps = [
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

terminal_c_args = [
]

terminal_link_args = [
]

terminal_lib = static_library('terminal',
  terminal_srcs,
  install: false,
  dependencies: terminal_deps,
  c_args: terminal_c_args,
  include_directories: terminal_include_dirs
)

terminal_dep = declare_dependency(
  include_directories: terminal_include_dirs,
  link_with: terminal_lib,
  link_args: terminal_link_args,
)
