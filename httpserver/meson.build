httpserver_srcs = [
  'httpserver.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

httpserver_include_dirs = [
  '.',
  inc,
]

httpserver_deps = [
  core_dep,
  httpserver_utils_dep,
  httpserver_h_deps,
  wslay_dep,
  websocket_server_dep,
]

httpserver_c_args = [
]

httpserver_link_args = [
]

httpserver_lib = library('httpserver',
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
