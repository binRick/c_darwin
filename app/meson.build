app_srcs = [
  'app.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

app_include_dirs = [
  '.',
  inc,
]

app_deps = [
  ansi_codes_dep,timelib_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  logh_dep,
  ms_dep,
  bytes_dep,
]

app_c_args = [
]

app_link_args = [
]

app_lib = static_library('app',
  app_srcs,
  install: false,
  dependencies: app_deps,
  c_args: app_c_args,
  include_directories: app_include_dirs
)

app_dep = declare_dependency(
  include_directories: app_include_dirs,
  link_with: app_lib,
  link_args: app_link_args,
)
