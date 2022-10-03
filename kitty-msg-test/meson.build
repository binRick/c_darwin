kitty_msg_test_srcs = [
  'kitty-msg-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
kitty_msg_test_include_dirs = [
  '.',
  inc,
]
kitty_msg_test_deps = [
  incbin_dep,ansi_utils_dep,clamp_dep,
  c_greatest_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  kitty_msg_dep,
]
kitty_msg_test_c_args = [
]
kitty_msg_test_link_args = [
]

kitty_msg_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  kitty_msg_test_exec = executable('kitty-msg-test',
     kitty_msg_test_srcs,
     dependencies: kitty_msg_test_deps,
     include_directories: kitty_msg_test_include_dirs,
     link_args: kitty_msg_test_link_args,
     c_args: kitty_msg_test_c_args,
     install: false,
  )
  test('kitty-msg-test', 
    kitty_msg_test_exec, args: ['-v','-a'],
    workdir: kitty_msg_test_test_dir,
  )
endif
