wrec_image_test_srcs = [
  'wrec-image-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
wrec_image_test_include_dirs = [
  inc,
]
wrec_image_test_deps = [
  c_greatest_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  wrec_capture_dep,
  bench_dep,
  wrec_image_dep,
]
wrec_image_test_c_args = [
]
wrec_image_test_link_args = [
]

wrec_image_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  wrec_image_test_exec = executable('wrec-image-test',
     wrec_image_test_srcs,
     dependencies: wrec_image_test_deps,
     include_directories: wrec_image_test_include_dirs,
     link_args: wrec_image_test_link_args,
     c_args: wrec_image_test_c_args,
     install: false,
  )
  test('wrec-image-test', 
    wrec_image_test_exec, args: ['-v','-a'],
    workdir: wrec_image_test_test_dir,
  )
endif
