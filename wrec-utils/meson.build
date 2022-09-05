wrec_utils_srcs = [
  'wrec-utils.c',
]
wrec_utils_link_flags = []
wrec_utils_c_args = []
wrec_utils_deps = [
  wrec_common_dep,
  ansi_codes_dep,
  c_stringfn_dep,
  window_utils_dep,
  c_vector_dep,
  c_string_buffer_dep,
  fort_dep,
  core_utils_dep,
  wildcardcmp_dep,
  c_fsio_dep,  
  libterminput_dep,
  bytes_dep,
  log_dep,
  bench_dep,
  ms_dep,
]
wrec_utils_dirs = [
  inc,
]


wrec_utils_lib = static_library('wrec-utils',
  wrec_utils_srcs,
  install: false,
  dependencies: wrec_utils_deps,
  c_args: wrec_utils_c_args,
  include_directories: wrec_utils_dirs,
  link_args: wrec_utils_link_flags,
)
wrec_utils_dep = declare_dependency(
  include_directories: wrec_utils_dirs,
  link_with: wrec_utils_lib,
  link_args: wrec_utils_link_flags,
)
