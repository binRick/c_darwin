system_utils_srcs = [
  'system-utils.c',
]
system_utils_deps = [
  c_stringfn_dep,
  pasteboard_dep,
  core_utils_dep,string_utils_dep,display_utils_dep,log_dep,
]

system_utils_link_args = [
  '-Wl,-framework,CoreFoundation',
  '-Wl,-framework,Carbon',
  '-Wl,-framework,Foundation',
  '-Wl,-framework,AppKit'
]

system_utils_lib = library('system-utils',
  system_utils_srcs,
  install: false,
  dependencies: system_utils_deps,
  include_directories: [
    inc,
  ],
)


system_utils_dep = declare_dependency(
  include_directories: [
    inc,
  ],
  link_with: system_utils_lib,
  link_args: system_utils_link_args,
)
