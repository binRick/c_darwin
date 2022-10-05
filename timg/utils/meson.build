timg_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

timg_utils_include_dirs = [
  '.',
  inc,
]

timg_utils_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,tempdir_dep,b64_dep,
  bytes_dep,incbin_dep,sha256_c_dep,rotate_bits_dep,
  reproc_dep,which_dep,
]

timg_utils_c_args = [
  '-DINCBIN_SILENCE_BITCODE_WARNING',
]

timg_utils_link_args = [
]

timg_utils_lib = library('timg-utils',
  timg_utils_srcs,
  install: false,
  dependencies: timg_utils_deps,
  c_args: timg_utils_c_args,
  include_directories: timg_utils_include_dirs
)

timg_utils_dep = declare_dependency(
  include_directories: timg_utils_include_dirs,
  link_with: timg_utils_lib,
  link_args: timg_utils_link_args,
)
