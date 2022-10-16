save_srcs = [
  'save.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

save_include_dirs = [
  '.',
  inc,
]

save_deps = [
  core_dep,
  capture_type_dep,
  image_utils_dep,
  core_debug_dep,core_utils_dep,
  vips_dep,
]

save_c_args = [
]

save_link_args = [
]

save_lib = library('capture-save',
  save_srcs,
  install: false,
  dependencies: save_deps,
  c_args: save_c_args,
  include_directories: save_include_dirs
)

capture_save_dep = declare_dependency(
  include_directories: save_include_dirs,
  link_with: save_lib,
  link_args: save_link_args,
)
