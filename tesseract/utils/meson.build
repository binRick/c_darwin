tesseract_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

tesseract_utils_include_dirs = [
  '.',
  '/usr/local/include/tesseract',
  inc,
]

tesseract_utils_deps = [
  core_utils_dep, core_image_dep,
  ansi_codes_dep,capture_utils_dep,tempdir_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  tesseract_dep,
  leptonica_dep,wildcardcmp_dep,
  c_workqueue_dep,chan_dep,
  stb_dep,
  space_utils_dep,
  display_utils_dep,
  window_utils_dep,timg_utils_dep,vips_dep,
  app_utils_dep,image_utils_dep,process_utils_dep,display_utils_dep,
  c_img_dep,
]

tesseract_utils_c_args = [
  tesseract_cflags,
  leptonica_cflags,
]

tesseract_utils_link_args = [
  tesseract_libs,
  leptonica_libs,
]

tesseract_utils_lib = library('tesseract-utils',
  tesseract_utils_srcs,
  install: false,
  dependencies: tesseract_utils_deps,
  c_args: tesseract_utils_c_args,
  link_args: tesseract_utils_link_args,
  include_directories: tesseract_utils_include_dirs
)

tesseract_utils_dep = declare_dependency(
  include_directories: tesseract_utils_include_dirs,
  link_with: tesseract_utils_lib,
  link_args: tesseract_utils_link_args,
)
