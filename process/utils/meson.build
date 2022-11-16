process_utils_srcs = [
  'utils.c',
]
process_utils_c_args = [
  '-Wno-address',
  ]
process_utils_dirs = [
    '.',
    inc,
  ]
process_utils_deps = [
  core_dep,
  async_dep,
]


process_utils_link_flags = [
  osx_ldflags
  ]




process_utils_lib = static_library('process-utils',
  process_utils_srcs,
  install: false,
  dependencies: process_utils_deps,
  c_args: process_utils_c_args,
  include_directories: process_utils_dirs,
)

process_utils_dep = declare_dependency(
  include_directories: process_utils_dirs,
  link_with: process_utils_lib,
  link_args: process_utils_link_flags,
)
