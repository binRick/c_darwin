font_utils_srcs = [
  'font-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

font_utils_include_dirs = [
  '.',
  inc,
]

font_utils_deps = [
  ansi_codes_dep,systemprofiler_dep,parson_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
]

font_utils_c_args = [
]

font_utils_link_args = [
]

font_utils_lib = library('font-utils',
  font_utils_srcs,
  install: false,
  dependencies: font_utils_deps,
  c_args: font_utils_c_args,
  include_directories: font_utils_include_dirs
)

font_utils_dep = declare_dependency(
  include_directories: font_utils_include_dirs,
  link_with: font_utils_lib,
  link_args: font_utils_link_args,
)
