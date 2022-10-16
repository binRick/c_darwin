screen_srcs = [
  'screen.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

screen_include_dirs = [
  '.',
  inc,
]

screen_deps = [
  core_dep,
]

screen_c_args = [
]

screen_link_args = [
]

screen_lib = library('screen',
  screen_srcs,
  install: false,
  dependencies: screen_deps,
  c_args: screen_c_args,
  include_directories: screen_include_dirs
)

screen_dep = declare_dependency(
  include_directories: screen_include_dirs,
  link_with: screen_lib,
  link_args: screen_link_args,
)
subdir('utils')
