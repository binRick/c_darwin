focused_srcs = [
  'focused.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
focused_include_dirs = [
  '.',
  inc,
]
focused_deps = [
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_eventemitter_dep,
  osx_keys_dep,
  core_utils_dep,
  app_utils_dep,
  ansi_codes_dep,
  logh_dep,
  timestamp_dep,
  ms_dep,
  bytes_dep,
]
focused_c_args = [
]
focused_link_args = [
]

focused_test_dir = join_paths(meson.current_source_dir(), '..')

focused_lib = static_library('focused',
  focused_srcs,
  install: false,
  dependencies: focused_deps,
  include_directories: focused_include_dirs,
)


focused_dep = declare_dependency(
  include_directories: focused_include_dirs,
  link_with: focused_lib,
  link_args: focused_link_args,
)
