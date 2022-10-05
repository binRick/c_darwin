image_srcs = [
  'image.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

image_include_dirs = [
  '.',
  inc,
]

image_deps = [
  ansi_codes_dep,glib_dep,vips_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,
]

image_c_args = [
]

image_link_args = [
]

image_lib = library('image',
  image_srcs,
  install: false,
  dependencies: image_deps,
  c_args: image_c_args,
  include_directories: image_include_dirs
)

image_dep = declare_dependency(
  include_directories: image_include_dirs,
  link_with: image_lib,
  link_args: image_link_args,
)
subdir('utils')
subdir('type')
