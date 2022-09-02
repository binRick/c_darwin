table_utils_srcs = [
  'table-utils.c',
]
table_utils_c_args = [
  '-Wno-address',
  ]
table_utils_dirs = [
    '.',
    '../app-utils',
    '../',
    inc,
  ]
table_utils_deps = [
  c_stringfn_dep,
  parson_dep,
  logh_dep,
  ansi_codes_dep,
  c_vector_dep,
  process_dep,
  fort_dep,
  bytes_dep,
  ms_dep,
  string_utils_dep,
  timestamp_dep,
]
table_utils_link_flags = [
  osx_link_args,
]


table_utils_lib = static_library('table-utils',
  table_utils_srcs,
  install: false,
  dependencies: table_utils_deps,
  c_args: table_utils_c_args,
  include_directories: table_utils_dirs,
)


table_utils_dep = declare_dependency(
  include_directories: table_utils_dirs,
  link_with: table_utils_lib,
  link_args: table_utils_link_flags,
)
