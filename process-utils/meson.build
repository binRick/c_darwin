process_utils_srcs = [
  'process-utils.c',
]
process_utils_c_args = [
  '-Wno-address',
  ]
process_utils_dirs = [
    '.',
    inc,
  ]
process_utils_deps = [
  c_stringfn_dep,
  parson_dep,
  log_dep,
  ansi_codes_dep,
  c_vector_dep,
  fort_dep,
  bytes_dep,
  ms_dep,
  process_dep,
  string_utils_dep,string_utils_dep,frameworks_dep,
  timestamp_dep,
  wildcardcmp_dep,
  reproc_dep,which_dep,
]


process_utils_link_flags = osx_ldflags




process_utils_lib = library('process-utils',
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
