alacritty_srcs = [
  'alacritty.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

alacritty_include_dirs = [
  '.',
  inc,
]

alacritty_deps = [
  ansi_codes_dep,process_dep,
  c_vector_dep,wildcardcmp_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
]

alacritty_c_args = [
]

alacritty_link_args = [
]

alacritty_lib = library('alacritty',
  alacritty_srcs,
  install: false,
  dependencies: alacritty_deps,
  c_args: alacritty_c_args,
  include_directories: alacritty_include_dirs
)

alacritty_dep = declare_dependency(
  include_directories: alacritty_include_dirs,
  link_with: alacritty_lib,
  link_args: alacritty_link_args,
)
