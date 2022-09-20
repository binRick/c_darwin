httpserver_srcs = [
  'httpserver.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

httpserver_include_dirs = [
  '.',
  inc,
]

httpserver_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  httpserver_h_deps,
  wslay_dep,
  websocket_server_dep,
  libforks_dep,url_h_dep,chan_dep,
]

httpserver_c_args = [
]

httpserver_link_args = [
]

httpserver_lib = static_library('httpserver',
  httpserver_srcs,
  install: false,
  dependencies: httpserver_deps,
  c_args: httpserver_c_args,
  include_directories: httpserver_include_dirs
)

httpserver_dep = declare_dependency(
  include_directories: httpserver_include_dirs,
  link_with: httpserver_lib,
  link_args: httpserver_link_args,
)
