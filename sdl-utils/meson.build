sdl_utils_srcs = [
  'sdl-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

sdl_utils_include_dirs = [
  '.',
  inc,
]

sdl_utils_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,qoir_dep,space_utils_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  qoi_dep,qoi_ci_dep,clamp_dep,glib_dep,vips_dep,core_dep,window_utils_dep,core_dep,module_dep,capture_utils_dep,image_utils_dep,display_utils_dep,
  sdl2_dep,tempdir_dep,
  sdl_qoi_dep,
]

sdl_utils_c_args = [
  '-DINCBIN_SILENCE_BITCODE_WARNING="1"',
]

sdl_utils_link_args = [
]

sdl_utils_lib = library('sdl-utils',
  sdl_utils_srcs,
  install: false,
  dependencies: sdl_utils_deps,
  c_args: sdl_utils_c_args,
  include_directories: sdl_utils_include_dirs
)

sdl_utils_dep = declare_dependency(
  include_directories: sdl_utils_include_dirs,
  link_with: sdl_utils_lib,
  link_args: sdl_utils_link_args,
)
