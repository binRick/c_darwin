active_app_srcs = [
  'active-app.c',
]
active_app_deps = [
  c_stringfn_dep,
  core_utils_dep,process_utils_dep,app_utils_dep,
]
active_app_link_args = [
  '-Wno-deprecated-declarations',
  ]
active_app_c_args = [
  ]
active_app_dirs = [
    inc,
    '../',
  ]
active_app_lib = static_library('active-app',
  active_app_srcs,
  install: false,
  dependencies: active_app_deps,
  include_directories: active_app_dirs,
  c_args: active_app_c_args,
)


active_app_dep = declare_dependency(
  include_directories: active_app_dirs,
  link_with: active_app_lib,
  link_args: active_app_link_args,
)
