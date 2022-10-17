subdir('info')
subdir('utils')

window_srcs = [
  'window.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

window_include_dirs = [
  '.',
  inc,
]

window_deps = [
  core_dep,
  window_info_dep,window_utils_dep,
]

window_c_args = [
  '-Wno-unused-variable',
]

window_link_args = [
]

window_lib = library('window',
  window_srcs,
  install: false,
  dependencies: window_deps,
  c_args: window_c_args,
  include_directories: window_include_dirs
)

window_dep = declare_dependency(
  include_directories: window_include_dirs,
  link_with: window_lib,
  link_args: window_link_args,
)
subdir('utils/test')
