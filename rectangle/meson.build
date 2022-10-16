rectangle_srcs = [
  'rectangle.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
rectangle_dirs = [
  '.',
  inc,
]
rectangle_deps = [
  core_dep,
  osx_keys_dep,
  process_dep,process_utils_dep,
]
rectangle_c_args = [
]

rectangle_dir = join_paths(meson.current_source_dir(), '..')

rectangle_lib = library('rectangle',
  rectangle_srcs,
  dependencies: rectangle_deps,
  c_args: rectangle_c_args,
  include_directories: rectangle_dirs,
)
rectangle_dep = declare_dependency(
  include_directories: rectangle_dirs,
  link_with: rectangle_lib,
)
