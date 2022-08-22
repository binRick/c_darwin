animated_gif_test_srcs = [
  'animated-gif-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
animated_gif_test_include_dirs = [
  '.',
  inc,
]
animated_gif_test_deps = [
  c_greatest_dep,
  animated_gif_dep,
  tempdir_dep,
]
animated_gif_test_c_args = [
]
animated_gif_test_link_args = [
]

animated_gif_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  animated_gif_test_exec = executable('animated-gif-test',
     animated_gif_test_srcs,
     dependencies: animated_gif_test_deps,
     include_directories: animated_gif_test_include_dirs,
     link_args: animated_gif_test_link_args,
     c_args: animated_gif_test_c_args,
     install: false,
  )
  #test('animated-gif-test', 
  #  animated_gif_test_exec, args: ['-v','-a'],
  #  workdir: animated_gif_test_test_dir,
  #)
endif
