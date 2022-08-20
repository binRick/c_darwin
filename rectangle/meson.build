rectangle_srcs = [
  'rectangle.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
rectangle_dirs = [
  '.',
  inc,
]
rectangle_deps = [
  c_vector_dep,
  c_fsio_dep,
  osx_keys_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  debug_memory_dep,
  subprocess_dep,
  wildcardcmp_dep,
  process_dep,
  c_dbg_dep,
  which_dep,
  timestamp_dep,
  str_flatten_c_dep,
]
rectangle_c_args = [
]

rectangle_dir = join_paths(meson.current_source_dir(), '..')

rectangle_lib = static_library('rectangle',
  rectangle_srcs,
  dependencies: rectangle_deps,
  c_args: rectangle_c_args,
  include_directories: rectangle_dirs,
)
rectangle_dep = declare_dependency(
  include_directories: rectangle_dirs,
  link_with: rectangle_lib,
)
