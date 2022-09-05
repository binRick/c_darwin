osx_keys_srcs = [
  'osx-keys.c',
]
osx_keys_link_flags = []
osx_keys_c_args = []
osx_keys_deps = [
  log_dep,string_utils_dep,
]
osx_keys_dirs = [
  '.',
  inc,
]


osx_keys_lib = static_library('osx-keys',
  osx_keys_srcs,
  install: false,
  dependencies: osx_keys_deps,
  c_args: osx_keys_c_args,
  include_directories: osx_keys_dirs,
  link_args: osx_keys_link_flags,
)

osx_keys_dep = declare_dependency(
  include_directories: osx_keys_dirs,
  link_with: osx_keys_lib,
  link_args: osx_keys_link_flags,
)
