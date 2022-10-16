display_utils_srcs = [
  'utils.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
display_utils_dirs = [
  '.',
  inc,
]
display_utils_deps = [
  core_dep,
  process_utils_dep,
]
display_utils_c_args = [
]
display_utils_link_args = osx_ldflags

display_utils_dir = join_paths(meson.current_source_dir(), '..')

display_utils_lib = library('display-utils',
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
