process_utils_srcs = [
  'process-utils.c',
]
process_utils_c_args = [
  '-Wno-address',
  ]
process_utils_dirs = [
    '.',
    '../app-utils',
    '../',
    inc,
  ]
process_utils_deps = [
  c_stringfn_dep,
  parson_dep,
  logh_dep,
  ansi_codes_dep,
  c_vector_dep,
  fort_dep,
  bytes_dep,
  ms_dep,
  string_utils_dep,
  timestamp_dep,
]
process_utils_link_flags = [
  osx_link_args,
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
