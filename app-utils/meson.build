app_utils_srcs = [
  'app-utils.c',
]
app_utils_c_args = [
  '-Wbo-address',
]
app_utils_deps = [
  active_app_dep,
  c_stringfn_dep,
  logh_dep,
]
app_utils_lib = static_library('app-utils',
  app_utils_srcs,
  install: false,
  dependencies: app_utils_deps,
  c_args: app_utils_c_args,
  include_directories: [
    inc,
    '../window-utils',
  ],
)


app_utils_dep = declare_dependency(
  include_directories: [
    inc,  
    '../window-utils',
  ],
  link_with: app_utils_lib,
)
