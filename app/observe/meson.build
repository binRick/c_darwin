observe_srcs = [
  'observe.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

observe_include_dirs = [
  '.',
  inc,
]

observe_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  frameworks_dep,
]

observe_c_args = [
]

observe_link_args = [
]

observe_lib = library('observe',
  observe_srcs,
  install: false,
  dependencies: observe_deps,
  c_args: observe_c_args,
  include_directories: observe_include_dirs
)

observe_dep = declare_dependency(
  include_directories: observe_include_dirs,
  link_with: observe_lib,
  link_args: observe_link_args,
)
