vips_basics_test_srcs = [
  'vips-basics-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
vips_basics_test_include_dirs = [
  '.',
  inc,
]
vips_basics_test_deps = [
  c_greatest_dep,kitty_msg_dep,c_workqueue_dep,chan_dep,fancy_progress_dep,
  ansi_codes_dep,
  module_dep,
  async_dep,str_dep,
  c_vector_dep,
  match_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,
  vips_dep,
  async_dep,
]
vips_basics_test_c_args = [
  '-DINCBIN_SILENCE_BITCODE_WARNING',
]
vips_basics_test_link_args = [
]

vips_basics_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  vips_basics_test_exec = executable('vips-basics-test',
     vips_basics_test_srcs,
     dependencies: vips_basics_test_deps,
     include_directories: vips_basics_test_include_dirs,
     link_args: vips_basics_test_link_args,
     c_args: vips_basics_test_c_args,
     install: false,
  )
  test('vips-basics-test', 
    vips_basics_test_exec, args: ['-v','-a'],
    workdir: vips_basics_test_test_dir,
  )
endif
