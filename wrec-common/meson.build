wrec_common_srcs = [
  'wrec-common.c',
]
wrec_common_link_flags = []
wrec_common_c_args = []
wrec_common_deps = [
  ansi_codes_dep,
  c_stringfn_dep,
  c_vector_dep,
  tiny_regex_c_dep,
  c_string_buffer_dep,
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
  c_img_dep,
  tempdir_dep,
]
wrec_common_dirs = [
  '.',
  '../',
  '../wrec-common',
]


wrec_common_lib = static_library('wrec-common',
  wrec_common_srcs,
  install: false,
  dependencies: wrec_common_deps,
  c_args: wrec_common_c_args,
  include_directories: wrec_common_dirs,
  link_args: wrec_common_link_flags,
)
wrec_common_dep = declare_dependency(
  include_directories: wrec_common_dirs,
  link_with: wrec_common_lib,
  link_args: wrec_common_link_flags,
)
