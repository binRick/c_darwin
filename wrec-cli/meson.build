wrec_cli_srcs = [
  'wrec-cli.c',
]
wrec_cli_link_flags = []
wrec_cli_c_args = []
wrec_cli_deps = [
  ansi_codes_dep,
  cargs_dep,
  wrec_dep,
]
wrec_cli_dirs = [
  '../submodules/meson_deps/submodules/c_fsio/include',
  '.',
  '../submodules/meson_deps/submodules/c_fsio',
  inc,
]


if get_option('enable-binaries')
  wrec_cli_exec = executable('wrec-cli',
     wrec_cli_srcs,
     dependencies: wrec_cli_deps,
     include_directories: wrec_cli_dirs,
     c_args: wrec_cli_c_args,
  )
  if get_option('enable-binary-tests')
    #test('wrec-cli', passh_program, args: [wrec_cli_exec])
  endif
endif
