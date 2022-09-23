keychain_utils_srcs = [
  'keychain-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

keychain_utils_include_dirs = [
  '.',
  inc,
]

keychain_utils_deps = [
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

keychain_utils_c_args = [
]

keychain_utils_link_args = [
]

keychain_utils_lib = library('keychain-utils',
  keychain_utils_srcs,
  install: false,
  dependencies: keychain_utils_deps,
  c_args: keychain_utils_c_args,
  include_directories: keychain_utils_include_dirs
)

keychain_utils_dep = declare_dependency(
  include_directories: keychain_utils_include_dirs,
  link_with: keychain_utils_lib,
  link_args: keychain_utils_link_args,
)
