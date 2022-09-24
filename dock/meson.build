dock_srcs = [
  'dock.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

dock_include_dirs = [
  '.',
  inc,
]

dock_deps = [
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

dock_c_args = [
]

dock_link_args = [
]

dock_lib = library('dock',
  dock_srcs,
  install: false,
  dependencies: dock_deps,
  c_args: dock_c_args,
  include_directories: dock_include_dirs
)

dock_dep = declare_dependency(
  include_directories: dock_include_dirs,
  link_with: dock_lib,
  link_args: dock_link_args,
)

subdir('utils')
