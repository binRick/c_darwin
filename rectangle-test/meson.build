rectangle_test_srcs = [
  'rectangle-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
rectangle_test_include_dirs = [
  '.',
  inc,
]
rectangle_test_deps = [
  c_greatest_dep,
  c_vector_dep,
  rectangle_dep,
  c_fsio_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  
]
rectangle_test_c_args = [
]
rectangle_test_link_args = [
]

rectangle_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  rectangle_test_exec = executable('rectangle-test',
     rectangle_test_srcs,
     dependencies: rectangle_test_deps,
     include_directories: rectangle_test_include_dirs,
     link_args: rectangle_test_link_args,
     c_args: rectangle_test_c_args,
     install: false,
  )
  test('rectangle-test', 
    rectangle_test_exec, args: ['-v','-a'],
    workdir: rectangle_test_test_dir,
  )
endif
