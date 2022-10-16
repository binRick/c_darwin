systemprofiler_srcs = [
  'systemprofiler.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

systemprofiler_include_dirs = [
  '.',
  inc,
]

systemprofiler_deps = [
  core_dep,
]

systemprofiler_c_args = [
]

systemprofiler_link_args = [
]

systemprofiler_lib = library('systemprofiler',
  systemprofiler_srcs,
  install: false,
  dependencies: systemprofiler_deps,
  c_args: systemprofiler_c_args,
  include_directories: systemprofiler_include_dirs
)

systemprofiler_dep = declare_dependency(
  include_directories: systemprofiler_include_dirs,
  link_with: systemprofiler_lib,
  link_args: systemprofiler_link_args,
)
