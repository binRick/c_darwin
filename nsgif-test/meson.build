nsgif_test_srcs = [
  'nsgif-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
nsgif_test_include_dirs = [
  '.',
  inc,
]
nsgif_test_deps = [
  c_greatest_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,
  libnsgif_dep,
]
nsgif_test_c_args = [
]
nsgif_test_link_args = [
]

nsgif_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  nsgif_test_exec = executable('nsgif-test',
     nsgif_test_srcs,
     dependencies: nsgif_test_deps,
     include_directories: nsgif_test_include_dirs,
     link_args: nsgif_test_link_args,
     c_args: nsgif_test_c_args,
     install: false,
  )
  #test('nsgif-test', 
  #  nsgif_test_exec, args: ['-v','-a'],
  #  workdir: nsgif_test_test_dir,
  #)
endif
