window_utils_srcs = [
  'window-utils.c',
]
window_utils_c_args = [
  '-Wbo-address',
  ]
window_utils_deps = [
  active_app_dep,
  c_stringfn_dep,
  parson_dep,
  logh_dep,
  ansicodes_dep,
]
window_utils_lib = static_library('window-utils',
  window_utils_srcs,
  install: false,
  dependencies: window_utils_deps,
  c_args: window_utils_c_args,
  include_directories: [
    inc,
    '../',
    '../app-utils',
  ],
)


window_utils_dep = declare_dependency(
  include_directories: [
    inc,
  ],
  link_with: window_utils_lib,
)
