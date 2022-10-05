socket_protocol_srcs = [
  'socket-protocol.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

socket_protocol_include_dirs = [
  '.',
  inc,
]

socket_protocol_deps = [
  config_utils_dep,
  process_utils_dep,window_info_dep,parson_dep,window_utils_dep,
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

socket_protocol_c_args = [
]

socket_protocol_link_args = [
]

socket_protocol_lib = library('socket-protocol',
  socket_protocol_srcs,
  install: false,
  dependencies: socket_protocol_deps,
  c_args: socket_protocol_c_args,
  include_directories: socket_protocol_include_dirs
)

socket_protocol_dep = declare_dependency(
  include_directories: socket_protocol_include_dirs,
  link_with: socket_protocol_lib,
  link_args: socket_protocol_link_args,
)
