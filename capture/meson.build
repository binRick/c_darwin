capture_srcs = [
  'capture.c',
]
capture_link_flags = []
capture_c_args = []
capture_deps = [
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
capture_dirs = [
  '../submodules/c_deps/submodules/c_fsio/include',
  '.',
  '../submodules/c_deps/submodules/c_fsio',
  inc,
]
capture_lib =static_library('capture',
  capture_srcs,
  install: false,
  dependencies: capture_deps,
  c_args: capture_c_args,
  include_directories: capture_dirs,
)
capture_dep = declare_dependency(
  include_directories: capture_dirs,
  link_with: capture_lib,
  link_args: capture_link_flags,
)
