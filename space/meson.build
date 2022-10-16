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
  core_dep,
  space_utils_dep,
]

space_c_args = [
]

space_link_args = [
]

space_lib = library('space',
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
