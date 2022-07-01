core_utils_srcs = [
  'core-utils.c',
]
core_utils_dirs = [
  '.',
  '../',
  inc,
]
core_utils_deps = [
  c_stringfn_dep,
]

core_utils_link_args = [
  '-Wl,-framework,CoreFoundation',
  '-Wl,-framework,Carbon',
  '-Wl,-framework,Foundation',
  '-Wl,-framework,AppKit'  
]

core_utils_lib = static_library('core-utils',
  core_utils_srcs,
  install: false,
  dependencies: core_utils_deps,
  include_directories: core_utils_dirs,
)


core_utils_dep = declare_dependency(
  include_directories: core_utils_dirs,
  link_with: core_utils_lib,
  link_args: core_utils_link_args,
)
