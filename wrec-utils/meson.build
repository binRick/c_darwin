wrec_utils_srcs = [
  'wrec-utils.c',
]
wrec_utils_link_flags = []
wrec_utils_c_args = []
wrec_utils_deps = [
  wrec_common_dep,
  ansi_codes_dep,
  djbhash_dep,
  c_stringfn_dep,
  c_vector_dep,
  tiny_regex_c_dep,
  cargs_dep,
  c_string_buffer_dep,
  which_dep,
  subprocess_dep,
  strdup_dep,
  assertf_dep,
  wildcardcmp_dep,
  parson_dep,
  c_fsio_dep,  
  string_utils_dep,
  app_utils_dep,
  libterminput_dep,
  chan_dep,
  bytes_dep,
  b64_dep,
  rwimg_dep,
  window_utils_dep,
  logh_dep,
  dbg_dep,
  c_timer_dep,
  progress_dep,
 wrec_capture_dep,
  bench_dep,
  ms_dep,
  tempdir_dep,
  fort_dep,  
]
wrec_utils_dirs = [
  '../submodules/c_deps/submodules/c_fsio/include',
  '../submodules/c_deps/submodules/c_fsio',
  '.',
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
