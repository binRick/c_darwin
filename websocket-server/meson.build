websocket_server_srcs = [
  'websocket-server.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

websocket_server_include_dirs = [
  '.',
  inc,
]

websocket_server_deps = [
  ansi_codes_dep,
  c_vector_dep,
  nettle_dep,uuid4_dep,
  c_fsio_dep,
  c_stringfn_dep,socket_protocol_dep,libforks_dep,
  c_string_buffer_dep,chan_dep,
  timestamp_dep,flingfd_dep,
  log_dep,
  ms_dep,
  bytes_dep,
]

websocket_server_c_args = [
]

websocket_server_link_args = [
]

websocket_server_lib = static_library('websocket-server',
  websocket_server_srcs,
  install: false,
  dependencies: websocket_server_deps,
  c_args: websocket_server_c_args,
  include_directories: websocket_server_include_dirs
)

websocket_server_dep = declare_dependency(
  include_directories: websocket_server_include_dirs,
  link_with: websocket_server_lib,
  link_args: websocket_server_link_args,
)
