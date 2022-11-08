tp_test_srcs = [
  'tp-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
tp_test_include_dirs = [
  '.',
  inc,
]
tp_test_deps = [
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
  tp_dep,
]
tp_test_c_args = [
]
tp_test_link_args = [
]

tp_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  tp_test_exec = executable('tp-test',
     tp_test_srcs,
     dependencies: tp_test_deps,
     include_directories: tp_test_include_dirs,
     link_args: tp_test_link_args,
     c_args: tp_test_c_args,
     install: false,
  )
  test('tp-test', 
    tp_test_exec, args: ['-v','-a'],
    workdir: tp_test_test_dir,
  )
endif
