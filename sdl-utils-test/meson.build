sdl_utils_test_srcs = [
  'sdl-utils-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
sdl_utils_test_include_dirs = [
  '.',
  inc,
]
sdl_utils_test_deps = [
  c_greatest_dep,
  ansi_codes_dep,
  sdl_utils_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  glib_dep,
  vips_dep,
  incbin_dep,
]
sdl_utils_test_c_args = [
  '-DINCBIN_SILENCE_BITCODE_WARNING="1"',
]
sdl_utils_test_link_args = [
]

sdl_utils_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  sdl_utils_test_exec = executable('sdl-utils-test',
     sdl_utils_test_srcs,
     dependencies: sdl_utils_test_deps,
     include_directories: sdl_utils_test_include_dirs,
     link_args: sdl_utils_test_link_args,
     c_args: sdl_utils_test_c_args,
     install: false,
  )
  test('sdl-utils-test', 
    sdl_utils_test_exec, args: ['-v','-a'],
    workdir: sdl_utils_test_test_dir,
  )
endif
