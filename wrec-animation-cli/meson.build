wrec_animation_cli_srcs = [
  'wrec-animation-cli.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
wrec_animation_cli_include_dirs = [
  '.',
  inc,
]
wrec_animation_cli_deps = [
  c_greatest_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  ms_dep,
  timestamp_dep,
  optparse99_dep,
  core_utils_dep,
  debug_memory_dep,
]
wrec_animation_cli_c_args = [
]
wrec_animation_cli_link_args = [
]

wrec_animation_cli_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  wrec_animation_cli_exec = executable('wrec-animation-cli',
     wrec_animation_cli_srcs,
     dependencies: wrec_animation_cli_deps,
     include_directories: wrec_animation_cli_include_dirs,
     link_args: wrec_animation_cli_link_args,
     c_args: wrec_animation_cli_c_args,
     install: false,
  )
  test('wrec-animation-cli', 
    wrec_animation_cli_exec, args: ['--help'],
    workdir: wrec_animation_cli_test_dir,
  )
endif
