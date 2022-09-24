tesseract_test_srcs = [
  'test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
tesseract_test_include_dirs = [
  '.',
  '/usr/local/include/tesseract',
  inc,
]
tesseract_test_deps = [
  c_greatest_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  log_dep,display_utils_dep,process_utils_dep,
  c_stringfn_dep,tempdir_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  leptonica_dep,capture_utils_dep,
  tesseract_dep,
  stb_dep,
  space_utils_dep,
  display_utils_dep,
  window_utils_dep,
  tesseract_utils_dep,
  app_utils_dep,image_utils_dep,
  c_img_dep,
]
tesseract_test_c_args = [
  tesseract_cflags,
  leptonica_cflags,
]
tesseract_test_link_args = [
  tesseract_libs,
  leptonica_libs,
]

tesseract_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  tesseract_test_exec = executable('tesseract-utils-test',
     tesseract_test_srcs,
     dependencies: tesseract_test_deps,
     include_directories: tesseract_test_include_dirs,
     link_args: tesseract_test_link_args,
     c_args: tesseract_test_c_args,
     install: false,
  )
  test('tesseract-utils-test', 
    tesseract_test_exec, args: ['-v','-a'],
    workdir: tesseract_test_test_dir,
    timeout: 180,
  )
endif

