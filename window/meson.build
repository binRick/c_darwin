subdir('sort')
subdir('info')
subdir('utils')
subdir('utils/test')

window_srcs = [
  'window.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

window_include_dirs = [
  '.',
  inc,
]

window_deps = [
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

window_c_args = [
]

window_link_args = [
]

window_lib = static_library('window',
  window_srcs,
  install: false,
  dependencies: window_deps,
  c_args: window_c_args,
  include_directories: window_include_dirs
)

window_dep = declare_dependency(
  include_directories: window_include_dirs,
  link_with: window_lib,
  link_args: window_link_args,
)
