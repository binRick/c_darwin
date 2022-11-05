app_utils_srcs = [
  'utils.c',
]
app_utils_c_args = [
  '-Wno-address',
]
app_utils_deps = [
  core_dep,
  process_utils_dep,
  systemprofiler_dep,
  icon_utils_dep,
  kitty_msg_dep,glib_dep,vips_dep,
]
app_utils_link_flags = ['-Wl,-framework,CoreFoundation','-Wl,-framework,Carbon','-Wl,-framework,Foundation', '-Wl,-framework,AppKit']
app_utils_lib = library('app-utils',
  app_utils_srcs,
  install: false,
  dependencies: app_utils_deps,
  c_args: app_utils_c_args,
  include_directories: [
    inc,'.',
  ],
)


app_utils_dep = declare_dependency(
  include_directories: [
    inc,  
    '.',
  ],
  link_with: app_utils_lib,
  link_args: app_utils_link_flags,
)
