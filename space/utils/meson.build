space_utils_srcs = [
  'utils.c',
]
space_utils_c_args = [
  '-Wno-address',
  ]
space_utils_dirs = [
    '.',
    inc,
  ]
space_utils_deps = [
  core_dep,
  c_vector_dep,vips_dep,
  c_stringfn_dep,
  parson_dep,
  string_utils_dep,
  image_utils_dep,
  log_dep,core_dep,
  ansi_codes_dep,
  timestamp_dep,
  bytes_dep, ms_dep,
  hash_dep,
  module_dep,
  process_dep,
  timestamp_dep,display_utils_dep,process_utils_dep,
]
space_utils_link_flags = [
  osx_ldflags,
]


space_utils_lib = library('space-utils',
  space_utils_srcs,
  install: false,
  dependencies: space_utils_deps,
  c_args: space_utils_c_args,
  include_directories: space_utils_dirs,
)


space_utils_dep = declare_dependency(
  include_directories: space_utils_dirs,
  link_with: space_utils_lib,
  link_args: space_utils_link_flags,
)
