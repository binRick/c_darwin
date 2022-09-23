rectangle_cli_srcs = [
  'rectangle-cli.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
rectangle_cli_include_dirs = [
  '.',
  inc,
]
rectangle_cli_deps = [
  c_vector_dep,
  rectangle_dep,
  c_fsio_dep,
  ansi_codes_dep,
  cargs_dep,process_utils_dep,window_info_dep,window_utils_dep,display_utils_dep,
  active_app_dep,
  app_utils_dep,
  generic_print_dep,
  window_utils_dep,
  timestamp_dep,
]
rectangle_cli_c_args = [
]
rectangle_cli_link_args = [
]

rectangle_cli_cli_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  rectangle_cli_exec = executable('rec',
     rectangle_cli_srcs,
     dependencies: rectangle_cli_deps,
     include_directories: rectangle_cli_include_dirs,
     link_args: rectangle_cli_link_args,
     c_args: rectangle_cli_c_args,
     install: false,
  )
endif
