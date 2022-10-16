subdir('debug')
subdir('utils')
subdir('image')

core_srcs = [
  'core.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

core_include_dirs = [
  '.',
  inc,
]

core_deps = [
  core_utils_dep,core_image_dep,core_debug_dep,layout_utils_dep,
]

core_c_args = [
]

core_link_args = [
]

core_lib = static_library('core',
  core_srcs,
  install: false,
  dependencies: core_deps,
  c_args: core_c_args,
  include_directories: core_include_dirs
)

core_dep = declare_dependency(
  include_directories: core_include_dirs,
  link_with: core_lib,
  link_args: core_link_args,
)
