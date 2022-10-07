layout_test_srcs = [
  'layout-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
layout_test_include_dirs = [
  '.',
  inc,
]
layout_test_deps = [
  c_greatest_dep,
  layout_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,
]
layout_test_c_args = [
  '-DLAY_IMPLEMENTATION',
]
layout_test_link_args = [
]

layout_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  layout_test_exec = executable('layout-test',
     layout_test_srcs,
     dependencies: layout_test_deps,
     include_directories: layout_test_include_dirs,
     link_args: layout_test_link_args,
     c_args: layout_test_c_args,
     install: false,
  )
  test('layout-test', 
    layout_test_exec, args: ['-v','-a'],
    workdir: layout_test_test_dir,
  )
endif
