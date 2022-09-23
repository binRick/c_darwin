output_utils_srcs = [
  'output-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

output_utils_include_dirs = [
  '.',
  inc,
]

output_utils_deps = [
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

output_utils_c_args = [
]

output_utils_link_args = [
]

output_utils_lib = library('output-utils',
  output_utils_srcs,
  install: false,
  dependencies: output_utils_deps,
  c_args: output_utils_c_args,
  include_directories: output_utils_include_dirs
)

output_utils_dep = declare_dependency(
  include_directories: output_utils_include_dirs,
  link_with: output_utils_lib,
  link_args: output_utils_link_args,
)
