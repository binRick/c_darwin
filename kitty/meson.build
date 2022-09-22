kitty_srcs = [
  'kitty.c',
  'kitty-module.c',
  'kitty-color.c',
]
kitty_link_flags = []
kitty_c_args = []
kitty_deps = [
  c_vector_dep,
  djbhash_dep,
  socket99_dep,
  c_stringfn_dep,
  wildcardcmp_dep,
  c_fsio_dep,
  c_string_buffer_dep,
  subprocess_dep,
  which_dep,
  process_dep,
  ansi_codes_dep,
  parson_dep,
]
kitty_dirs = [
  '.',
  inc,
]
kitty_lib =static_library('kitty',
  kitty_srcs,
  install: false,
  dependencies: kitty_deps,
  c_args: kitty_c_args,
  include_directories: kitty_dirs,
)
kitty_dep = declare_dependency(
  include_directories: kitty_dirs,
  link_with: kitty_lib,
  link_args: osx_ldflags,
)
