subdir('utils')
keyboard_srcs = [
  'keyboard.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

keyboard_include_dirs = [
  '.',
  inc,
]

keyboard_deps = [
  core_dep,
  keyboard_utils_dep,
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
