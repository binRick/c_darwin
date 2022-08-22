msf_gif_test_srcs = [
  'msf-gif-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
msf_gif_test_include_dirs = [
  '.',
  inc,
]
msf_gif_test_deps = [
  c_greatest_dep,
  c_vector_dep,
  c_fsio_dep,
  msf_gif_dep,
  stb_dep,
  c_img_dep,
  timestamp_dep,
  ms_dep,
  bytes_dep,
  c_stringfn_dep,
  tinydir_dep,
  logh_dep,
  ansi_codes_dep,
]
msf_gif_test_c_args = [
]
msf_gif_test_link_args = [
]

msf_gif_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  msf_gif_test_exec = executable('msf-gif-test',
     msf_gif_test_srcs,
     dependencies: msf_gif_test_deps,
     include_directories: msf_gif_test_include_dirs,
     link_args: msf_gif_test_link_args,
     c_args: msf_gif_test_c_args,
     install: false,
  )
  #test('msf-gif-test', 
  #  msf_gif_test_exec, args: ['-v','-a'],
  #  workdir: msf_gif_test_test_dir,
  #)
endif
