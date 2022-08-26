wrec_capture_srcs = [
  'wrec-capture.c',
]
wrec_capturelink_flags = []
wrec_capturec_args = []
wrec_capture_deps = [
  ansi_codes_dep,
  ms_dep,
  logh_dep,
  app_utils_dep,
  window_utils_dep,
  c_stringfn_dep,
  wrec_utils_dep,
  wrec_common_dep,
  c_vector_dep,
  c_string_buffer_dep,
  c_stringfn_dep,
  c_fsio_dep,  
]
wrec_capture_dirs = [
  inc,
]
wrec_capture_lib =static_library('wrec-capture',
  wrec_capture_srcs,
  install: false,
  dependencies: wrec_capture_deps,
  c_args: wrec_capturec_args,
  include_directories: wrec_capture_dirs,
)
wrec_capture_dep = declare_dependency(
  include_directories: wrec_capture_dirs,
  link_with: wrec_capture_lib,
  link_args: wrec_capturelink_flags,
)
