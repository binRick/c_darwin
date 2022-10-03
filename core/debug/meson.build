core_debug_srcs = [
  'debug.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

core_debug_include_dirs = [
  '.',
]

core_debug_deps = [
  ansi_codes_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
]

core_debug_c_args = [
]

core_debug_link_args = [
]

core_debug_lib = static_library('core-debug',
  core_debug_srcs,
  install: false,
  dependencies: core_debug_deps,
  c_args: core_debug_c_args,
  include_directories: core_debug_include_dirs
)

core_debug_dep = declare_dependency(
  include_directories: core_debug_include_dirs,
  link_with: core_debug_lib,
  link_args: core_debug_link_args,
)
