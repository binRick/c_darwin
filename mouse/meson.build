subdir('utils')
mouse_srcs = [
  'mouse.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

mouse_include_dirs = [
  '.',
  inc,
]

mouse_deps = [
  core_dep,
  mouse_utils_dep,
]

mouse_c_args = [
]

mouse_link_args = [
]

mouse_lib = library('mouse',
  mouse_srcs,
  install: false,
  dependencies: mouse_deps,
  c_args: mouse_c_args,
  include_directories: mouse_include_dirs
)

mouse_dep = declare_dependency(
  include_directories: mouse_include_dirs,
  link_with: mouse_lib,
  link_args: mouse_link_args,
)
