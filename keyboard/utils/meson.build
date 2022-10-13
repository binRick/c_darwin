keyboard_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

keyboard_utils_include_dirs = [
  '.',
  inc,
]

keyboard_utils_deps = [
  frameworks_dep,
  ansi_codes_dep,osx_keys_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,
]

keyboard_utils_c_args = [
]

keyboard_utils_link_args = [
]

keyboard_utils_lib = library('keyboard-utils',
  keyboard_utils_srcs,
  install: false,
  dependencies: keyboard_utils_deps,
  c_args: keyboard_utils_c_args,
  include_directories: keyboard_utils_include_dirs
)

keyboard_utils_dep = declare_dependency(
  include_directories: keyboard_utils_include_dirs,
  link_with: keyboard_utils_lib,
  link_args: keyboard_utils_link_args,
)
