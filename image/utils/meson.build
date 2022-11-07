image_utils_srcs = [
  'utils.c',
]
image_utils_dirs = [
    '.',
    inc,
  ]
image_utils_deps = [
  core_dep,
  vips_dep,
  tesseract_dep,
  qoi_ci_dep,
  core_utils_dep,qoir_dep,
]
image_utils_link_flags = [
  osx_link_args,
  tesseract_libs,
  leptonica_libs,
]
image_utils_c_args = [
  '-DMSF_GIF_IMPL',
  '-DSTB_IMAGE_IMPLEMENTATION',
  '-DSTB_IMAGE_WRITE_IMPLEMENTATION',
  '-DSTB_IMAGE_RESIZE_IMPLEMENTATION',
  '-DQOI_IMPLEMENTATION',
  tesseract_cflags,
  leptonica_cflags,
]


image_utils_lib = library('image-utils',
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
