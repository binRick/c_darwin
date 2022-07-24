wrec_srcs = [
  'wrec.c',
]
wrec_link_flags = []
wrec_c_args = []
wrec_deps = [
  ansi_codes_dep,
  djbhash_dep,
  c_stringfn_dep,
  c_vector_dep,
  tiny_regex_c_dep,
  cargs_dep,
  c_string_buffer_dep,
  parson_dep,
  c_fsio_dep,  
  string_utils_dep,
  app_utils_dep,
  window_utils_dep,
  logh_dep,
  dbg_dep,
  c_timer_dep,
  capture_dep,
  fort_dep,  
]
wrec_dirs = [
  '../submodules/meson_deps/submodules/c_fsio/include',
  '../submodules/meson_deps/submodules/c_fsio',
  '.',
  inc,
]
wrec_lib = shared_library('wrec',
  wrec_srcs,
  install: false,
  dependencies: wrec_deps,
  c_args: wrec_c_args,
  include_directories: wrec_dirs,
)
wrec_dep = declare_dependency(
  include_directories: wrec_dirs,
  link_with: wrec_lib,
  link_args: wrec_link_flags,
)
