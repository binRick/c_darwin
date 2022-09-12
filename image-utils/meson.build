image_utils_srcs = [
  'image-utils.c',
]
image_utils_c_args = [
  '-Wno-address',
  ]
image_utils_dirs = [
    '.',
    inc,
  ]
image_utils_deps = [
  c_stringfn_dep,
  parson_dep,
  log_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_img_dep,
  fort_dep,
  bytes_dep,
  ms_dep,
  string_utils_dep,
  timestamp_dep,
  stb_dep,
]
image_utils_link_flags = [
  osx_link_args,
]


image_utils_lib = static_library('image-utils',
  image_utils_srcs,
  install: false,
  dependencies: image_utils_deps,
  c_args: image_utils_c_args,
  include_directories: image_utils_dirs,
)


image_utils_dep = declare_dependency(
  include_directories: image_utils_dirs,
  link_with: image_utils_lib,
  link_args: image_utils_link_flags,
)
