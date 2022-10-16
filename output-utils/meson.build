output_utils_srcs = [
  'output-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

output_utils_include_dirs = [
  '.',
  inc,
]

output_utils_deps = [
  core_dep,
  glib_dep,vips_dep,
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
