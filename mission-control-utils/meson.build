mission_control_utils_srcs = [
  'mission-control-utils.c',
]
mission_control_utils_c_args = [
  '-Wno-address',
  ]
mission_control_utils_dirs = [
    '.',
    '../app-utils',
    '../',
    inc,
  ]
mission_control_utils_deps = [
  c_stringfn_dep,
  parson_dep,
  log_dep,
  ansi_codes_dep,
  c_vector_dep,
  bytes_dep,
  ms_dep,
  string_utils_dep,
  timestamp_dep,
]
mission_control_utils_link_flags = [
  osx_link_args,
]


mission_control_utils_lib = static_library('mission-control-utils',
  mission_control_utils_srcs,
  install: false,
  dependencies: mission_control_utils_deps,
  c_args: mission_control_utils_c_args,
  include_directories: mission_control_utils_dirs,
)


mission_control_utils_dep = declare_dependency(
  include_directories: mission_control_utils_dirs,
  link_with: mission_control_utils_lib,
  link_args: mission_control_utils_link_flags,
)
