core_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

core_utils_include_dirs = [
  '.',
  inc,
]

core_utils_deps = [
  ansi_codes_dep,
  cbars_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,glib_dep,vips_dep,
  ms_dep,
  bytes_dep,
  hashmap_h_dep,
  optparse99_dep,
  parson_dep,
  c_eventemitter_dep,
  glib_dep,b64_dep,whereami_dep,
  wildcardcmp_dep,
]

core_utils_c_args = [
]

core_utils_link_args = [
]

core_utils_lib = static_library('core-utils',
  core_utils_srcs,
  install: false,
  dependencies: core_utils_deps,
  c_args: core_utils_c_args,
  include_directories: core_utils_include_dirs
)

core_utils_dep = declare_dependency(
  include_directories: core_utils_include_dirs,
  link_with: core_utils_lib,
  link_args: core_utils_link_args,
)
