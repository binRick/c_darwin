tesseract_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

tesseract_utils_include_dirs = [
  '.',
  inc,
]

tesseract_utils_deps = [
  core_dep,
  pasteboard_dep,
  mouse_utils_dep,keyboard_utils_dep,
  core_utils_dep, core_image_dep,
  capture_utils_dep,
  tesseract_dep,
  space_utils_dep,
  display_utils_dep,
  window_utils_dep,
  vips_dep,
  kitty_msg_dep,
  app_utils_dep,image_utils_dep,process_utils_dep,display_utils_dep,capture_type_dep,
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
