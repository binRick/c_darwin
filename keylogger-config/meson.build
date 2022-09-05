keylogger_config_srcs = [
  'keylogger-config.c',
  'keylogger-config-types.c',
  'keylogger-config-logic.c',
]
keylogger_config_dirs = [
  '../submodules/c_deps',
  '../submodules/c_deps/submodules',
  '../keylogger',
  '../keylogger-db',
  '../pbpaste',
  '../submodules/c_deps',
  '../submodules',
  '.',
  '../',
  inc,
]
keylogger_config_deps = [
  hidapi_dep,
  keylogger_db_dep,
  parson_dep,
  module_dep,
  libusb_dep,
  pasteboard_dep,
  active_app_dep,
  process_dep,
  c_stringfn_dep,
  app_utils_dep,
  window_utils_dep,
  log_dep,
  pbpaste_dep,
  system_utils_dep,
  c_vector_dep,
  socket99_dep,
  timestamp_dep,
  c_stringfn_dep,
  c_fsio_dep,
  bytes_dep,
  c_string_buffer_dep,
  log_dep,
  c_forever_dep,
  c_eventemitter_dep,
  ansi_utils_dep,
]
keylogger_c_args = [
]
keylogger_config_args = [
]
keylogger_config_lib = static_library('keylogger-config',
  keylogger_config_srcs,
  install: false,
  dependencies: keylogger_config_deps,
  include_directories: keylogger_config_dirs,
)

keylogger_config_dep = declare_dependency(
  include_directories: keylogger_config_dirs,
  link_with: keylogger_config_lib,
)
