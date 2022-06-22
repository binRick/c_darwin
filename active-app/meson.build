active_app_srcs = [
  'active-app.c',
]
active_app_deps = [
  c_stringfn_dep,
]
active_app_dirs = [
    inc,
    '../',
    '../submodules/meson_deps',
    '../submodules/meson_deps/submodules',
  ]
active_app_lib = static_library('active-app',
  active_app_srcs,
  install: false,
  dependencies: active_app_deps,
  include_directories: active_app_dirs,
)


active_app_dep = declare_dependency(
  include_directories: active_app_dirs,
  link_with: active_app_lib,
)
