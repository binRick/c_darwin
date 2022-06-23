process_srcs = [
  'process.c',
  'process.h',
]
process_link_flags = []
process_c_args = []
process_deps = [
  c_stringfn_dep,
  c_vector_dep,
]
process_dirs = [
    '.',
    inc,
]
process_lib = shared_library('process',
  process_srcs,
  install: false,
  dependencies: process_deps,
  c_args: process_c_args,
  include_directories: process_dirs,
)
process_dep = declare_dependency(
  include_directories: process_dirs,
  link_with: process_lib,
  link_args: process_link_flags,
)
