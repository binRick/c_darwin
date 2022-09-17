hotkey_utils_test_srcs = [
  'hotkey-utils-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
hotkey_utils_test_include_dirs = [
  '.',
  inc,
]
hotkey_utils_test_deps = [
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
  hotkey_utils_dep,
]
hotkey_utils_test_c_args = [
]
hotkey_utils_test_link_args = [
]

hotkey_utils_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  hotkey_utils_test_exec = executable('hotkey-utils-test',
     hotkey_utils_test_srcs,
     dependencies: hotkey_utils_test_deps,
     include_directories: hotkey_utils_test_include_dirs,
     link_args: hotkey_utils_test_link_args,
     c_args: hotkey_utils_test_c_args,
     install: false,
  )
  test('hotkey-utils-test', 
    hotkey_utils_test_exec, args: ['-v','-a'],
    workdir: hotkey_utils_test_test_dir,
  )
endif
