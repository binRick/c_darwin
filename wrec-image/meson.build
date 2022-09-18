wrec_image_srcs = [
  'wrec-image.c',
  'wrec-image-qoi.c',
]
wrec_image_link_flags = []
wrec_image_c_args = []
wrec_image_deps = [
  image_utils_dep,
  ansi_codes_dep,
  c_stringfn_dep,
  c_vector_dep,
  c_string_buffer_dep,
  qoi_dep,
  stb_dep,
  c_fsio_dep,  
  bytes_dep,
  log_dep,
  ms_dep,
  tempdir_dep,
]
wrec_image_dirs = [
  '.',inc,
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
