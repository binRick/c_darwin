event_srcs = [
  'event.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

event_include_dirs = [
  '.',
  inc,
]

event_deps = [
  core_utils_dep,
]

event_c_args = [
]

event_link_args = [
]

event_lib = library('event',
  event_srcs,
  install: false,
  dependencies: event_deps,
  c_args: event_c_args,
  include_directories: event_include_dirs
)

event_dep = declare_dependency(
  include_directories: event_include_dirs,
  link_with: event_lib,
  link_args: event_link_args,
)
