save_srcs = [
  'save.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

save_include_dirs = [
  '.',
  inc,
]

save_deps = [
  glib_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  capture_type_dep,
  image_utils_dep,
  chan_dep,clamp_dep,
  core_dep,core_debug_dep,core_utils_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  vips_dep,
]

save_c_args = [
]

save_link_args = [
]

save_lib = library('capture-save',
  save_srcs,
  install: false,
  dependencies: save_deps,
  c_args: save_c_args,
  include_directories: save_include_dirs
)

capture_save_dep = declare_dependency(
  include_directories: save_include_dirs,
  link_with: save_lib,
  link_args: save_link_args,
)
