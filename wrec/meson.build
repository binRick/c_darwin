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
  generic_print_dep,
  bytes_dep,
  b64_dep,
  rwimg_dep,
  window_utils_dep,
  logh_dep,
  dbg_dep,
  c_timer_dep,
  gd_dep,
  progress_dep,
  capture_dep,
  bench_dep,
  ms_dep,
  tempdir_dep,
  fort_dep,  
]
wrec_dirs = [
  '../submodules/meson_deps/submodules/c_fsio/include',
  '../submodules/meson_deps/submodules/c_fsio',
  '.',
  inc,
]


wrec_lib = library('wrec',
  wrec_srcs,
  install: false,
  dependencies: wrec_deps,
  c_args: wrec_c_args,
  include_directories: wrec_dirs,
  link_args: wrec_link_flags,
)
wrec_dep = declare_dependency(
  include_directories: wrec_dirs,
  link_with: wrec_lib,
  link_args: wrec_link_flags,
)
