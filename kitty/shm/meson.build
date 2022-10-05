kitty_shm_srcs = [
  'shm.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

kitty_shm_include_dirs = [
  '.',
  inc,
]

kitty_shm_deps = [
  b64_dep,ansi_utils_dep,
  ansi_codes_dep,
  c_vector_dep,vips_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,
]

kitty_shm_c_args = [
]

kitty_shm_link_args = [
]

kitty_shm_lib = library('kitty_shm',
  kitty_shm_srcs,
  install: false,
  dependencies: kitty_shm_deps,
  c_args: kitty_shm_c_args,
  include_directories: kitty_shm_include_dirs
)

kitty_shm_dep = declare_dependency(
  include_directories: kitty_shm_include_dirs,
  link_with: kitty_shm_lib,
  link_args: kitty_shm_link_args,
)
