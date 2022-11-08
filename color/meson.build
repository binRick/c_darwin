color_srcs = [
  'color.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

color_include_dirs = [
  '.',
  inc,
]

color_deps = [
  ansi_codes_dep,csv_parser_dep,rgba_dep,hsluv_c_dep,string_utils_dep,ansi_utils_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
]

color_c_args = [
  '-DINCBIN_SILENCE_BITCODE_WARNING',
]

color_link_args = [
]

color_lib = library('color',
  color_srcs,
  install: false,
  dependencies: color_deps,
  c_args: color_c_args,
  include_directories: color_include_dirs
)

color_dep = declare_dependency(
  include_directories: color_include_dirs,
  link_with: color_lib,
  link_args: color_link_args,
)
