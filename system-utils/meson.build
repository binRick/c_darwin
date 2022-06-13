system_utils_srcs = [
  'system-utils.c',
]
system_utils_deps = [
  c_stringfn_dep,
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
)
