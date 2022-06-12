active_app_srcs = [
  'active-app.c',
]
active_app_deps = [
  c_stringfn_dep,
]
active_app_lib = static_library('active-app',
  active_app_srcs,
  install: false,
  dependencies: active_app_deps,
  include_directories: [
    inc,
  ],
)


active_app_dep = declare_dependency(
  include_directories: [
    inc,
  ],
  link_with: active_app_lib,
)
