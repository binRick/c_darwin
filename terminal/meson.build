subdir('utils')
terminal_srcs = [
  'terminal.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

terminal_include_dirs = [
  '.',
  inc,
]

terminal_deps = [
  core_dep,
  terminal_utils_dep,
]

terminal_c_args = [
]

terminal_link_args = [
]

terminal_lib = library('terminal',
  terminal_srcs,
  install: false,
  dependencies: terminal_deps,
  c_args: terminal_c_args,
  include_directories: terminal_include_dirs
)

terminal_dep = declare_dependency(
  include_directories: terminal_include_dirs,
  link_with: terminal_lib,
  link_args: terminal_link_args,
)
