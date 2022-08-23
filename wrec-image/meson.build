wrec_image_srcs = [
  'wrec-image.c',
]
wrec_image_link_flags = []
wrec_image_c_args = []
wrec_image_deps = [
  ansi_codes_dep,
  c_stringfn_dep,
  c_vector_dep,
  tiny_regex_c_dep,
  c_string_buffer_dep,
  wrec_common_dep,
  qoi_dep,
  which_dep,
  subprocess_dep,
  strdup_dep,
  wildcardcmp_dep,
  parson_dep,
  c_fsio_dep,  
  chan_dep,
  bytes_dep,
  libbeaufort_dep,
  string_utils_dep,
  b64_dep,
  rwimg_dep,
  fort_dep,
  logh_dep,
  ms_dep,
  app_utils_dep,
  window_utils_dep,
  msf_gif_dep,
  stb_dep,
  tempdir_dep,
]
wrec_image_dirs = [
  '.',
  '../',
  '../wrec-image',
]


wrec_image_lib = static_library('wrec-image',
  wrec_image_srcs,
  install: false,
  dependencies: wrec_image_deps,
  c_args: wrec_image_c_args,
  include_directories: wrec_image_dirs,
  link_args: wrec_image_link_flags,
)
wrec_image_dep = declare_dependency(
  include_directories: wrec_image_dirs,
  link_with: wrec_image_lib,
  link_args: wrec_image_link_flags,
)
