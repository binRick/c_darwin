type_srcs = [
  'type.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

type_include_dirs = [
  '.',
  inc,
]

type_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  vips_dep,
]

type_c_args = [
]

type_link_args = [
]

type_lib = library('type',
  type_srcs,
  install: false,
  dependencies: type_deps,
  c_args: type_c_args,
  include_directories: type_include_dirs
)

type_dep = declare_dependency(
  include_directories: type_include_dirs,
  link_with: type_lib,
  link_args: type_link_args,
)
