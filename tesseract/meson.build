subdir('utils')
tesseract_srcs = [
  'tesseract.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

tesseract_include_dirs = [
  '.',
  inc,
]

tesseract_deps = [
  core_dep,
  tesseract_utils_dep,
]

tesseract_c_args = [
]

tesseract_link_args = [
]

tesseract_lib = library('tesseract',
  tesseract_srcs,
  install: false,
  dependencies: tesseract_deps,
  c_args: tesseract_c_args,
  include_directories: tesseract_include_dirs
)

tesseract_dep = declare_dependency(
  include_directories: tesseract_include_dirs,
  link_with: tesseract_lib,
  link_args: tesseract_link_args,
)
