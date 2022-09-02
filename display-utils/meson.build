display_utils_srcs = [
  'display-utils.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
display_utils_dirs = [
  '.',
  inc,
]
display_utils_deps = [
  c_fsio_dep,c_vector_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  logh_dep,
  timestamp_dep,
  ms_dep,
  bytes_dep,
]
display_utils_c_args = [
]
display_utils_link_args = osx_link_args

display_utils_dir = join_paths(meson.current_source_dir(), '..')

display_utils_lib = static_library('display-utils',
  display_utils_srcs,
  install: false,
  dependencies: display_utils_deps,
  include_directories: display_utils_dirs,
)


display_utils_dep = declare_dependency(
  include_directories: display_utils_dirs,
  link_with: display_utils_lib,
  link_args: display_utils_link_args,
)
