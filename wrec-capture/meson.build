wrec_capture_srcs = [
  'wrec-capture.c',
]
wrec_capturelink_flags = []
wrec_capturec_args = []
wrec_capture_deps = [
  wrec_common_dep,
  ansi_codes_dep,
  core_utils_dep,
  bench_dep,
  ms_dep,
  tempdir_dep,
  which_dep,
  subprocess_dep,
  assertf_dep,
  app_utils_dep,
  window_utils_dep,
  djbhash_dep,
  c_stringfn_dep,
  c_vector_dep,
  tiny_regex_c_dep,
  socket99_dep,
  c_string_buffer_dep,
  c_stringfn_dep,
  str_replace_dep,
  occurrences_dep,
  parson_dep,
  c_fsio_dep,  
  debug_memory_dep,  
  kitty_dep,  
]
wrec_capture_dirs = [
  '../submodules/c_deps/submodules/c_fsio/include',
  '.',
  '../submodules/c_deps/submodules/c_fsio',
  inc,
]
wrec_capture_lib =static_library('wrec-capture',
  wrec_capture_srcs,
  install: false,
  dependencies: wrec_capture_deps,
  c_args: wrec_capturec_args,
  include_directories: wrec_capture_dirs,
)
wrec_capture_dep = declare_dependency(
  include_directories: wrec_capture_dirs,
  link_with: wrec_capture_lib,
  link_args: wrec_capturelink_flags,
)
