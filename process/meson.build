subdir('utils')
process_srcs = [
  'process.c',
]
process_link_flags = []
process_c_args = []
process_deps = [
  core_dep,
  process_utils_dep,
]
process_dirs = [
  '.',
  inc,
]
process_lib = static_library('process',
  process_srcs,
  dependencies: process_deps,
  c_args: process_c_args,
  include_directories: process_dirs,
)
process_dep = declare_dependency(
  include_directories: process_dirs,
  link_whole: process_lib,
  link_args: process_link_flags,
)
subdir('test')
