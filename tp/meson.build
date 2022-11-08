tp_srcs = [
  'tp.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

tp_include_dirs = [
  '.',
  inc,
]

tp_deps = [
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,clamp_dep,
  ms_dep,string_utils_dep,
  bytes_dep,termpaint_dep,
]

tp_c_args = [
]

tp_link_args = [
]

tp_lib = library('tp',
  tp_srcs,
  install: false,
  dependencies: tp_deps,
  c_args: tp_c_args,
  include_directories: tp_include_dirs
)

tp_dep = declare_dependency(
  include_directories: tp_include_dirs,
  link_with: tp_lib,
  link_args: tp_link_args,
)
