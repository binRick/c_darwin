kitty_msg_srcs = [
  'msg.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

kitty_msg_include_dirs = [
  '.',
  inc,
]

kitty_msg_deps = [
  core_dep,
  vips_dep,
  qoi_ci_dep,
]

kitty_msg_c_args = [
]

kitty_msg_link_args = [
]

kitty_msg_lib = static_library('kitty-msg',
  kitty_msg_srcs,
  install: false,
  dependencies: kitty_msg_deps,
  c_args: kitty_msg_c_args,
  include_directories: kitty_msg_include_dirs
)

kitty_msg_dep = declare_dependency(
  include_directories: kitty_msg_include_dirs,
  link_with: kitty_msg_lib,
  link_args: kitty_msg_link_args,
)
