capture_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

capture_utils_include_dirs = [
  '.',
  inc,
]

capture_utils_deps = [
  core_dep,
  space_utils_dep,window_utils_dep,display_utils_dep,screen_utils_dep,
  cache_utils_dep,image_utils_dep,
  vips_dep,
  process_dep,
]

capture_utils_c_args = [
]

capture_utils_link_args = [
]

capture_utils_lib = library('capture-utils',
  capture_utils_srcs,
  install: false,
  dependencies: capture_utils_deps,
  c_args: capture_utils_c_args,
  include_directories: capture_utils_include_dirs
)

capture_utils_dep = declare_dependency(
  include_directories: capture_utils_include_dirs,
  link_with: capture_utils_lib,
  link_args: capture_utils_link_args,
)
