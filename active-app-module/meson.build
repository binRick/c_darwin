active_app_module_srcs = [
  'active-app-module.c',
]
active_app_module_deps = [
  c_stringfn_dep,
  active_app_dep,
]
active_app_module_link_args = [
  '-Wno-deprecated-declarations',
  ]
active_app_module_c_args = [
  ]
active_app_module_lib = static_library('active-app-module',
  active_app_module_srcs,
  install: false,
  dependencies: active_app_module_deps,
  include_directories: [
    inc,
  ],
)


active_app_module_dep = declare_dependency(
  include_directories: [
    inc,
  ],
  link_with: active_app_module_lib,
  link_args: active_app_module_link_args,
)
