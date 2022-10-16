capture_type_srcs = [
  'type.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

capture_type_include_dirs = [
  '.',
  inc,
]

capture_type_deps = [
  core_dep,
  kitty_msg_dep,qoi_ci_dep,
  imagequant_dep,capture_utils_dep,image_utils_dep,
  capture_utils_dep,stb_dep,msf_gif_dep,gifdec_dep,
  leptonica_dep,
  vips_dep,
  core_debug_dep,core_utils_dep,
]

capture_type_c_args = [
]

capture_type_link_args = [
]

capture_type_lib = library('capture-type',
  capture_type_srcs,
  install: false,
  dependencies: capture_type_deps,
  c_args: capture_type_c_args,
  include_directories: capture_type_include_dirs
)

capture_type_dep = declare_dependency(
  include_directories: capture_type_include_dirs,
  link_with: capture_type_lib,
  link_args: capture_type_link_args,
)
