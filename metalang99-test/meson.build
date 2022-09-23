metalang99_test_srcs = [
  'metalang99-test.c',
  'frog.c',
  'droid.c',
  'shape.c',  
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
metalang99_test_include_dirs = [
  '.',
  inc,
]
metalang99_test_deps = [
  c_greatest_dep,
  
  metalang99_dep,
  interface99_dep,
  ansi_codes_dep,
  ms_dep,
  log_dep,
  app_utils_dep,
  window_utils_dep,
  c_stringfn_dep,
  
  
  c_vector_dep,
  c_string_buffer_dep,
  c_stringfn_dep,
  c_fsio_dep,  
]
metalang99_test_c_args = [
]
metalang99_test_link_args = [
]

metalang99_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  metalang99_test_exec = executable('metalang99-test',
     metalang99_test_srcs,
     dependencies: metalang99_test_deps,
     include_directories: metalang99_test_include_dirs,
     link_args: metalang99_test_link_args,
     c_args: metalang99_test_c_args,
     install: false,
  )
  test('metalang99-test', 
    metalang99_test_exec, args: ['-v','-a'],
    workdir: metalang99_test_test_dir,
  )
endif
