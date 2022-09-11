icon_utils_srcs = [
  'icon-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

icon_utils_include_dirs = [
  '.',
  inc,
]

icon_utils_deps = [
  ansi_codes_dep,string_utils_dep,
  c_vector_dep,icns_dep,stb_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
]

icon_utils_c_args = [
  '-I/usr/local/Cellar/libpng/1.6.37/include/libpng16',
]

icon_utils_link_args = [
]

icon_utils_lib = static_library('icon-utils',
  icon_utils_srcs,
  install: false,
  dependencies: icon_utils_deps,
  c_args: icon_utils_c_args,
  include_directories: icon_utils_include_dirs
)

icon_utils_dep = declare_dependency(
  include_directories: icon_utils_include_dirs,
  link_with: icon_utils_lib,
  link_args: icon_utils_link_args,
)
