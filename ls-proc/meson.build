ls_proc_srcs = [
  'ls-proc.c',
  'ls-proc.h',
]
process_include_dirs = [
  '.',
  '../process',
  inc,
]
process_include_cargs = [
  ]
ls_proc_deps = [
  process_dep,
  timestamp_dep,
  c_vector_dep,
  cargs_dep,
  c_stringfn_dep,
  wildcardcmp_dep,
]
if get_option('enable-binaries')
  ls_proc_exec = executable('ls-proc',
     ls_proc_srcs,
     dependencies: ls_proc_deps,
     include_directories: process_include_dirs,
     c_args: process_include_cargs,
     install: true,
     install_dir: '/usr/local/bin',
  )
  if get_option('enable-binary-tests')
    #    test('ls-proc Help', passh_program, args: [ls_proc_exec])
  endif
endif
