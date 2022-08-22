mac_load_test_srcs = [
  'mac-load-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
mac_load_test_include_dirs = [
  '.',
  inc,
]
mac_load_test_deps = [
  c_greatest_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  mac_load_dep,
  ansi_codes_dep,
  debug_memory_dep,
]
mac_load_test_c_args = [
]
mac_load_test_link_args = [
]

mac_load_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  mac_load_test_exec = executable('mac-load-test',
     mac_load_test_srcs,
     dependencies: mac_load_test_deps,
     include_directories: mac_load_test_include_dirs,
     link_args: mac_load_test_link_args,
     c_args: mac_load_test_c_args,
     install: false,
  )
  #test('mac-load-test', 
  #  mac_load_test_exec, args: ['-v','-a'],
  #  workdir: mac_load_test_test_dir,
  #)
endif
