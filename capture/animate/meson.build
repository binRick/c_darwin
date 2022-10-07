capture_animate_srcs = [
  'animate.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

capture_animate_include_dirs = [
  '.',
  inc,
]

capture_animate_deps = [
  ansi_codes_dep,
  kitty_msg_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  imagequant_dep,timg_utils_dep,tempdir_dep,capture_utils_dep,image_utils_dep,qoi_ci_dep,qoi_dep,chan_dep,
  capture_utils_dep,clamp_dep,stb_dep,msf_gif_dep,gifdec_dep,
  leptonica_dep,glib_dep,
  timg_utils_dep,c_workqueue_dep,vips_dep,
  core_dep,core_debug_dep,core_utils_dep,
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
