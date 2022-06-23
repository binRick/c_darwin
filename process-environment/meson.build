process_environment_srcs = [
  'process-environment.c',
  'process-environment.h',
]
process_environment_link_flags = []
process_environment_c_args = []
process_environment_deps = [
  c_stringfn_dep,
  c_vector_dep,
]
process_environment_dirs = [
    '.',
    inc,
]
process_environment_lib = shared_library('process-environment',
  process_environment_srcs,
  install: false,
  dependencies: process_environment_deps,
  c_args: process_environment_c_args,
  include_directories: process_environment_dirs,
)
process_environment_dep = declare_dependency(
  include_directories: process_environment_dirs,
  link_with: process_environment_lib,
  link_args: process_environment_link_flags,
)
