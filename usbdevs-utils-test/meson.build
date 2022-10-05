usbdevs_utils_test_srcs = [
  'usbdevs-utils-test.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
usbdevs_utils_test_include_dirs = [
  '.',
  inc,
]
usbdevs_utils_test_deps = [
  c_greatest_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,core_dep,
  ms_dep,
  bytes_dep,
  libusb_dep,
  hidapi_dep,
]
usbdevs_utils_test_c_args = [
]
usbdevs_utils_test_link_args = [
]

usbdevs_utils_test_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  usbdevs_utils_test_exec = executable('usbdevs-utils-test',
     usbdevs_utils_test_srcs,
     dependencies: usbdevs_utils_test_deps,
     include_directories: usbdevs_utils_test_include_dirs,
     link_args: usbdevs_utils_test_link_args,
     c_args: usbdevs_utils_test_c_args,
     install: false,
  )
  test('usbdevs-utils-test', 
    usbdevs_utils_test_exec, args: ['-v','-a'],
    workdir: usbdevs_utils_test_test_dir,
  )
endif
