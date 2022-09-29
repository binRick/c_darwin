capture_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

capture_utils_include_dirs = [
  '.',
  inc,
]

capture_utils_deps = [
  space_utils_dep,window_utils_dep,display_utils_dep,screen_utils_dep,
  ansi_codes_dep,cache_utils_dep,b64_dep,image_utils_dep,fs_c_dep,tempdir_dep,
  c_vector_dep,
  c_fsio_dep,timg_utils_dep,c_workqueue_dep,vips_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,window_utils_dep,process_utils_dep,
  ms_dep,
  bytes_dep,
]

capture_utils_c_args = [
]

capture_utils_link_args = [
]

capture_utils_lib = library('capture-utils',
  capture_utils_srcs,
  install: false,
  dependencies: capture_utils_deps,
  c_args: capture_utils_c_args,
  include_directories: capture_utils_include_dirs
)

capture_utils_dep = declare_dependency(
  include_directories: capture_utils_include_dirs,
  link_with: capture_utils_lib,
  link_args: capture_utils_link_args,
)
