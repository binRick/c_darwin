pasteboard_srcs = [
  'pasteboard.c',
]
pasteboard_dirs = [
  '.',
  '../',
  inc,
]
pasteboard_deps = [
  core_utils_dep,
  c_stringfn_dep,
  pbpaste_dep,
  bench_dep,
]

pasteboard_link_args = [
  '-Wl,-framework,CoreFoundation',
  '-Wl,-framework,IOKit',
]

pasteboard_lib = static_library('pasteboard',
  pasteboard_srcs,
  install: false,
  dependencies: pasteboard_deps,
  include_directories: pasteboard_dirs,
)


pasteboard_dep = declare_dependency(
  include_directories: pasteboard_dirs,
  link_with: pasteboard_lib,
  link_args: pasteboard_link_args,
)
