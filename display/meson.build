display_srcs = [
  'display.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

display_include_dirs = [
  '.',
  inc,
]

display_deps = [
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

display_c_args = [
]

display_link_args = [
]

display_lib = library('display',
  display_srcs,
  install: false,
  dependencies: display_deps,
  c_args: display_c_args,
  include_directories: display_include_dirs
)

display_dep = declare_dependency(
  include_directories: display_include_dirs,
  link_with: display_lib,
  link_args: display_link_args,
)

subdir('utils')
