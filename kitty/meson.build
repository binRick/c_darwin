kitty_srcs = [
  'kitty.c',
  'kitty.h',
]
kitty_link_flags = []
kitty_c_args = []
kitty_deps = [
  c_vector_dep,
  djbhash_dep,
  ansi_codes_dep,
  parson_dep,
]
kitty_dirs = [
  inc,
]
kitty_lib = shared_library('kitty',
  kitty_srcs,
  install: false,
  dependencies: kitty_deps,
  c_args: kitty_c_args,
  include_directories: kitty_dirs,
)
kitty_dep = declare_dependency(
  include_directories: kitty_dirs,
  link_with: kitty_lib,
  link_args: kitty_link_flags,
)
