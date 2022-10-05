frameworks_srcs = [
  'frameworks.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

frameworks_include_dirs = [
  '.',
  inc,
]

frameworks_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,
]

frameworks_c_args = [
]

frameworks_link_args = [
]

frameworks_lib = library('frameworks',
  frameworks_srcs,
  install: false,
  dependencies: frameworks_deps,
  c_args: frameworks_c_args,
  include_directories: frameworks_include_dirs
)

frameworks_dep = declare_dependency(
  include_directories: frameworks_include_dirs,
  link_with: frameworks_lib,
  link_args: frameworks_link_args,
)
