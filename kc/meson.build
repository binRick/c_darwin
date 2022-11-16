kc_srcs = [
  'kc.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

kc_include_dirs = [
  '.',
  inc,
]

kc_deps = [
  optparse99_dep,
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

kc_c_args = [
]

kc_link_args = [
]

kc_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  kc_exec = executable('kc',
     kc_srcs,
     dependencies: kc_deps,
     include_directories: kc_include_dirs,
     link_args: kc_link_args,
     c_args: kc_c_args,
     install: true,
  )
  test('kc', 
    kc_exec, args: ['--help'],
    workdir: kc_test_dir,
  )
endif
