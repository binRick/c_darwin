
window_utils_srcs = [
  'utils.c',
]
window_utils_c_args = [
  '-Wno-address',
  '-Wno-int-to-pointer-cast',
  ]
window_utils_dirs = [
    '.',
    inc,
  ]
window_utils_deps = [
  core_dep,
  frameworks_dep,
  sqldbal_dep,whereami_dep,core_utils_dep,
  parson_dep,
  space_utils_dep,
  log_dep,image_utils_dep,glib_dep,vips_dep,
  ansi_codes_dep,display_utils_dep,
  c_vector_dep,string_utils_dep,window_info_dep,process_utils_dep,
  process_dep,app_utils_dep,image_utils_dep,ms_dep,timestamp_dep,c_vector_dep,c_stringfn_dep,c_vector_dep,
]
window_utils_link_flags = [
  osx_ldflags,
]

window_utils_lib = library('window-utils',
  window_utils_srcs,
  install: false,
  dependencies: window_utils_deps,
  c_args: window_utils_c_args,
  include_directories: window_utils_dirs,
)


window_utils_dep = declare_dependency(
  include_directories: window_utils_dirs,
  link_with: window_utils_lib,
  link_args: window_utils_link_flags,
)
