image_utils_srcs = [
  'utils.c',
]
image_utils_dirs = [
    '.',
    inc,
  ]
image_utils_deps = [
  lodepng_dep,c_fsio_dep,timg_utils_dep,vips_dep,
  imagequant_dep,
  leptonica_dep,tempdir_dep,
  stb_dep,
  c_stringfn_dep,tesseract_dep,
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
  qoi_dep,
  timg_utils_dep,
  c_workqueue_dep,
  fancy_progress_dep,
  spng_dep,msf_gif_dep,
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
