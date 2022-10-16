type_srcs = [
  'type.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

type_include_dirs = [
  '.',
  inc,
]

type_deps = [
  core_dep,vips_dep,
]

type_c_args = [
]

type_link_args = [
]

type_lib = library('type',
  type_srcs,
  install: false,
  dependencies: type_deps,
  c_args: type_c_args,
  include_directories: type_include_dirs
)

image_type_dep = declare_dependency(
  include_directories: type_include_dirs,
  link_with: type_lib,
  link_args: type_link_args,
)
