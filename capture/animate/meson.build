capture_animate_srcs = [
  'animate.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

capture_animate_include_dirs = [
  '.',
  inc,
]

capture_animate_deps = [
  core_dep,
  kitty_msg_dep,
  capture_utils_dep,image_utils_dep,qoi_ci_dep,
  capture_utils_dep,
  msf_gif_dep,gifdec_dep,
  vips_dep,
  core_debug_dep,core_utils_dep,
  capture_type_dep,
]

capture_animate_c_args = [
]

capture_animate_link_args = [
]

capture_animate_lib = library('capture-animate',
  capture_animate_srcs,
  install: false,
  dependencies: capture_animate_deps,
  c_args: capture_animate_c_args,
  include_directories: capture_animate_include_dirs
)

capture_animate_dep = declare_dependency(
  include_directories: capture_animate_include_dirs,
  link_with: capture_animate_lib,
  link_args: capture_animate_link_args,
)
