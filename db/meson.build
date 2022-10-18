db_srcs = [
  'db.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

db_include_dirs = [
  '.',
  inc,
]

db_deps = [
  core_dep,
  core_utils_dep,core_dep,vips_dep,
  ansi_codes_dep,whereami_dep,core_utils_dep,space_utils_dep,
  window_utils_dep,hash_dep,
]

db_c_args = [
]

db_link_args = [
]

db_lib = library('db',
  db_srcs,
  install: false,
  dependencies: db_deps,
  c_args: db_c_args,
  include_directories: db_include_dirs
)

db_dep = declare_dependency(
  include_directories: db_include_dirs,
  link_with: db_lib,
  link_args: db_link_args,
)
