keyboard_srcs = [
  'keyboard.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

keyboard_include_dirs = [
  '.',
  inc,
]

keyboard_deps = [
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

keyboard_c_args = [
]

keyboard_link_args = [
]

keyboard_lib = library('keyboard',
  keyboard_srcs,
  install: false,
  dependencies: keyboard_deps,
  c_args: keyboard_c_args,
  include_directories: keyboard_include_dirs
)

keyboard_dep = declare_dependency(
  include_directories: keyboard_include_dirs,
  link_with: keyboard_lib,
  link_args: keyboard_link_args,
)
subdir('utils')
