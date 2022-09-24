system_srcs = [
  'system.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

system_include_dirs = [
  '.',
  inc,
]

system_deps = [
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

system_c_args = [
]

system_link_args = [
]

system_lib = static_library('system',
  system_srcs,
  install: false,
  dependencies: system_deps,
  c_args: system_c_args,
  include_directories: system_include_dirs
)

system_dep = declare_dependency(
  include_directories: system_include_dirs,
  link_with: system_lib,
  link_args: system_link_args,
)
subdir('utils')
