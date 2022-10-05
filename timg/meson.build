subdir('utils')
subdir('utils/test')
timg_srcs = [
  'timg.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

timg_include_dirs = [
  '.',
  inc,
]

timg_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,
]

timg_c_args = [
]

timg_link_args = [
]

timg_lib = library('timg',
  timg_srcs,
  install: false,
  dependencies: timg_deps,
  c_args: timg_c_args,
  include_directories: timg_include_dirs
)

timg_dep = declare_dependency(
  include_directories: timg_include_dirs,
  link_with: timg_lib,
  link_args: timg_link_args,
)
