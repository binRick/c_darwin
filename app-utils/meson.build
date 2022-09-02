app_utils_srcs = [
  'app-utils.c',
]
app_utils_c_args = [
  '-Wno-address',
]
app_utils_deps = [
  timestamp_dep,
  bytes_dep,ms_dep,
  c_stringfn_dep,
  logh_dep,
]
app_utils_link_flags = ['-Wl,-framework,CoreFoundation','-Wl,-framework,Carbon','-Wl,-framework,Foundation', '-Wl,-framework,AppKit']
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
  link_args: app_utils_link_flags,
)
