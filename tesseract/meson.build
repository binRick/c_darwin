tesseract_srcs = [
  'tesseract.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

tesseract_include_dirs = [
  '.',
  inc,
]

tesseract_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
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
subdir('utils')
