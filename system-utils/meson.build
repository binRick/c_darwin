system_utils_srcs = [
  'system-utils.c',
]
system_utils_deps = [
  core_utils_dep,
  c_stringfn_dep,
  pasteboard_dep,
]

system_utils_link_args = [
  '-Wl,-framework,CoreFoundation',
  '-Wl,-framework,Carbon',
  '-Wl,-framework,Foundation',
  '-Wl,-framework,AppKit'
]

system_utils_lib = static_library('system-utils',
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
