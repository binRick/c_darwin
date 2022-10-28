optparse_srcs = [
  'optparse.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

optparse_include_dirs = [
  '.',
  inc,
]

optparse_deps = [
  ansi_codes_dep,optparse99_dep,
  c_vector_dep,hash_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
]

optparse_c_args = [
  '-Wno-unused-but-set-variable',
]

optparse_link_args = [
]

optparse_lib = library('optparse',
  optparse_srcs,
  install: false,
  dependencies: optparse_deps,
  c_args: optparse_c_args,
  include_directories: optparse_include_dirs
)

optparse_dep = declare_dependency(
  include_directories: optparse_include_dirs,
  link_with: optparse_lib,
  link_args: optparse_link_args,
)
