capture_srcs = [
  'capture.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

capture_include_dirs = [
  '.',
  inc,
]

capture_deps = [
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

capture_c_args = [
]

capture_link_args = [
]

capture_lib = static_library('capture',
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
subdir('utils')
