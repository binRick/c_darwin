capture_window_srcs = [
  'window.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

capture_window_include_dirs = [
  '.',
  inc,
]

capture_window_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  imagequant_dep,timg_utils_dep,tempdir_dep,capture_utils_dep,image_utils_dep,qoi_ci_dep,qoi_dep,chan_dep,
  window_utils_dep,clamp_dep,stb_dep,msf_gif_dep,gifdec_dep,
  leptonica_dep,
  timg_utils_dep,c_workqueue_dep,vips_dep,
  core_dep,core_debug_dep,core_utils_dep,
]

capture_window_c_args = [
]

capture_window_link_args = [
]

capture_window_lib = library('capture-window',
  capture_window_srcs,
  install: false,
  dependencies: capture_window_deps,
  c_args: capture_window_c_args,
  include_directories: capture_window_include_dirs
)

capture_window_dep = declare_dependency(
  include_directories: capture_window_include_dirs,
  link_with: capture_window_lib,
  link_args: capture_window_link_args,
)
