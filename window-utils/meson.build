window_utils_srcs = [
  'window-utils.c',
]
window_utils_c_args = [
  '-Wbo-address',
  ]
window_utils_dirs = [
    '.',
    '../app-utils',
    '../',
    inc,
  ]
window_utils_deps = [
  core_utils_dep,
  active_app_dep,
  c_stringfn_dep,
  parson_dep,
  logh_dep,
  ansi_codes_dep,
]
window_utils_link_flags = ['-Wl,-framework,CoreFoundation','-Wl,-framework,Carbon','-Wl,-framework,Foundation', '-Wl,-framework,AppKit']
window_utils_lib = static_library('window-utils',
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
