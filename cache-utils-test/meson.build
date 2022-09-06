cache_utils_test_srcs = [
  'cache-utils-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
cache_utils_test_include_dirs = [
  '.',
  inc,
]
cache_utils_test_deps = [
  c_greatest_dep,cache_utils_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
]
cache_utils_test_c_args = [
]
cache_utils_test_link_args = [
]

cache_utils_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  cache_utils_test_exec = executable('cache-utils-test',
     cache_utils_test_srcs,
     dependencies: cache_utils_test_deps,
     include_directories: cache_utils_test_include_dirs,
     link_args: cache_utils_test_link_args,
     c_args: cache_utils_test_c_args,
     install: false,
  )
  test('cache-utils-test', 
    cache_utils_test_exec, args: ['-v','-a'],
    workdir: cache_utils_test_test_dir,
  )
endif
