wrec_animation_test_srcs = [
  'wrec-animation-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
wrec_animation_test_include_dirs = [
  '.',
  inc,
]
wrec_animation_test_deps = [
  c_greatest_dep,
  wrec_common_dep,
  stb_dep,
  bench_dep,
  c_stringfn_dep,
  c_fsio_dep,
  wrec_animation_dep,
  tempdir_dep,
]
wrec_animation_test_c_args = [
]
wrec_animation_test_link_args = [
]

wrec_animation_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  wrec_animation_test_exec = executable('wrec-animation-test',
     wrec_animation_test_srcs,
     dependencies: wrec_animation_test_deps,
     include_directories: wrec_animation_test_include_dirs,
     link_args: wrec_animation_test_link_args,
     c_args: wrec_animation_test_c_args,
     install: false,
  )
  #test('wrec-animation-test', 
  #  wrec_animation_test_exec, args: ['-v','-a'],
  #  workdir: wrec_animation_test_test_dir,
  #)
endif
