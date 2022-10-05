core_image_srcs = [
  'image.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

core_image_include_dirs = [
  '.',
  inc,
]

core_image_deps = [
  glib_dep,
  core_utils_dep,
  vips_dep,
  msf_gif_dep,
]

core_image_c_args = [
]

core_image_link_args = [
]

core_image_lib = library('core-image',
  core_image_srcs,
  install: false,
  dependencies: core_image_deps,
  c_args: core_image_c_args,
  include_directories: core_image_include_dirs
)

core_image_dep = declare_dependency(
  include_directories: core_image_include_dirs,
  link_with: core_image_lib,
  link_args: core_image_link_args,
)
