animated_gif_srcs = [
  'animated-gif.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
animated_gif_include_dirs = [
  '.',
  inc,
]
animated_gif_deps = [
  c_vector_dep,
  c_fsio_dep,
  msf_gif_dep,
  stb_dep,
  c_img_dep,
  timestamp_dep,
  ms_dep,
  bytes_dep,
  tempdir_dep,
  c_stringfn_dep,
  tinydir_dep,
  ansi_codes_dep,
]

animated_gif_test_dir = join_paths(meson.current_source_dir(), '..')

animated_gif_lib = static_library('animated-gif',
  animated_gif_srcs,
  install: false,
  dependencies: animated_gif_deps,
  include_directories: [
    inc,
    '../animated-gif',
  ],
)


animated_gif_dep = declare_dependency(
  include_directories: [
    inc,
    '../animated-gif',
  ],
  link_with: [animated_gif_lib],
)
