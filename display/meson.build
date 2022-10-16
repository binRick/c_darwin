subdir('utils')
display_srcs = [
  'display.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

display_include_dirs = [
  '.',
  inc,
]

display_deps = [
  core_dep,
  display_utils_dep,
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

