subdir('msg')
kitty_srcs = [
  'kitty.c',
  'kitty-module.c',
  'kitty-color.c',
]
kitty_link_flags = []
kitty_c_args = []
kitty_deps = [
  core_dep,
  image_dep,
  process_dep,
  kitty_msg_dep,
]
kitty_dirs = [
  '.',
  inc,
]
kitty_lib = library('kitty',
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
