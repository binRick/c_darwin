subdir('utils')
space_srcs = [
  'space.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

space_include_dirs = [
  '.',
  inc,
]

space_deps = [
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

space_c_args = [
]

space_link_args = [
]

space_lib = static_library('space',
  space_srcs,
  install: false,
  dependencies: space_deps,
  c_args: space_c_args,
  include_directories: space_include_dirs
)

space_dep = declare_dependency(
  include_directories: space_include_dirs,
  link_with: space_lib,
  link_args: space_link_args,
)
