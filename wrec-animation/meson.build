wrec_animation_srcs = [
  'wrec-animation.c',
]
wrec_animation_link_flags = []
wrec_animation_c_args = []
wrec_animation_deps = [
  ansi_codes_dep,image_utils_dep,
  wrec_common_dep,
  wrec_utils_dep,
  c_stringfn_dep,
  c_vector_dep,
  c_string_buffer_dep,
  c_fsio_dep,  
  bytes_dep,
  log_dep,
  ms_dep,
  msf_gif_dep,
  stb_dep,
  tempdir_dep,
]
wrec_animation_dirs = [
  '.',inc,
]


wrec_animation_lib = static_library('wrec-animation',
  wrec_animation_srcs,
  install: false,
  dependencies: wrec_animation_deps,
  c_args: wrec_animation_c_args,
  include_directories: wrec_animation_dirs,
  link_args: wrec_animation_link_flags,
)
wrec_animation_dep = declare_dependency(
  include_directories: wrec_animation_dirs,
  link_with: wrec_animation_lib,
  link_args: wrec_animation_link_flags,
)
