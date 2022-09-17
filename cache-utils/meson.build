cache_utils_srcs = [
  'cache-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

cache_utils_include_dirs = [
  '.',
  inc,
]

cache_utils_deps = [
  ansi_codes_dep,
  c_vector_dep,reproc_dep,which_dep,tempdir_dep,
  c_fsio_dep,iowow_dep,murmurhash_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  string_utils_dep,
]

cache_utils_c_args = [
]

cache_utils_link_args = [
]

cache_utils_lib = static_library('cache-utils',
  cache_utils_srcs,
  install: false,
  dependencies: cache_utils_deps,
  c_args: cache_utils_c_args,
  include_directories: cache_utils_include_dirs
)

cache_utils_dep = declare_dependency(
  include_directories: cache_utils_include_dirs,
  link_with: cache_utils_lib,
  link_args: cache_utils_link_args,
)
