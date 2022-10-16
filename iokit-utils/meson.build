iokit_utils_srcs = [
  'iokit-utils.c',
]
iokit_utils_c_args = [
  '-Wno-address',
  ]
iokit_utils_dirs = [
    '.',
    inc,
  ]
iokit_utils_deps = [
  core_dep,
]
iokit_utils_link_flags = [
  osx_link_args,
  '-Wl,-framework,IOKit',
]


iokit_utils_lib = library('iokit-utils',
  iokit_utils_srcs,
  install: false,
  dependencies: iokit_utils_deps,
  c_args: iokit_utils_c_args,
  include_directories: iokit_utils_dirs,
)


iokit_utils_dep = declare_dependency(
  include_directories: iokit_utils_dirs,
  link_with: iokit_utils_lib,
  link_args: iokit_utils_link_flags,
)
