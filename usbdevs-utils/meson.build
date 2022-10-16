usbdevs_utils_srcs = [
  'usbdevs-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

usbdevs_utils_include_dirs = [
  '.',
  inc,
]

usbdevs_utils_deps = [
  core_dep,
  hidapi_dep,
]

usbdevs_utils_c_args = [
]

usbdevs_utils_link_args = [
]

usbdevs_utils_lib = library('usbdevs-utils',
  usbdevs_utils_srcs,
  install: false,
  dependencies: usbdevs_utils_deps,
  c_args: usbdevs_utils_c_args,
  include_directories: usbdevs_utils_include_dirs
)

usbdevs_utils_dep = declare_dependency(
  include_directories: usbdevs_utils_include_dirs,
  link_with: usbdevs_utils_lib,
  link_args: usbdevs_utils_link_args,
)
