kitty_shm_test_srcs = [
  'kitty-shm-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
kitty_shm_test_include_dirs = [
  '.',
  inc,
]
kitty_shm_test_deps = [
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
  kitty_shm_dep,
]
kitty_shm_test_c_args = [
]
kitty_shm_test_link_args = [
]

kitty_shm_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  kitty_shm_test_exec = executable('kitty-shm-test',
     kitty_shm_test_srcs,
     dependencies: kitty_shm_test_deps,
     include_directories: kitty_shm_test_include_dirs,
     link_args: kitty_shm_test_link_args,
     c_args: kitty_shm_test_c_args,
     install: false,
  )
  test('kitty-shm-test', 
    kitty_shm_test_exec, args: ['-v','-a'],
    workdir: kitty_shm_test_test_dir,
  )
endif
