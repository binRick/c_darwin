subdir('utils')
subdir('type')
capture_srcs = [
  'capture.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

capture_include_dirs = [
  '.',
  inc,
]

capture_deps = [
  ansi_codes_dep,timg_utils_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,tempdir_dep,
  capture_type_dep,
  capture_utils_dep,
]

capture_c_args = [
]

capture_link_args = [
]

capture_lib = library('capture',
  capture_srcs,
  install: false,
  dependencies: capture_deps,
  c_args: capture_c_args,
  include_directories: capture_include_dirs
)

capture_dep = declare_dependency(
  include_directories: capture_include_dirs,
  link_with: capture_lib,
  link_args: capture_link_args,
)
