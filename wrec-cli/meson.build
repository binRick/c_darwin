wrec_cli_srcs = [
  'wrec-cli.c',
]
wrec_cli_link_flags = []
wrec_cli_c_args = []
wrec_cli_deps = [
  wrec_common_dep,
  wrec_utils_dep,
  ansi_codes_dep,
  core_utils_dep,
  table_utils_dep,
  wrec_capture_dep,
  cargs_dep,
  wrec_utils_dep,
  exec_fzf_dep,
]
wrec_cli_dirs = [
  inc,
]


if get_option('enable-binaries')
  wrec_cli_exec = executable('wrec',
     wrec_cli_srcs,
     dependencies: wrec_cli_deps,
     include_directories: wrec_cli_dirs,
     c_args: wrec_cli_c_args,
  )
  if get_option('enable-binary-tests')
    test('wrec help', passh_program, args: [wrec_cli_exec, '--help'])
    test('wrec list windows', passh_program, args: [wrec_cli_exec, '-m','list'])
  endif
endif
