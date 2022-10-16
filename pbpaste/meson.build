pbpaste_srcs = [
  'pbpaste.c',
]
pbpaste_inc = [
 '.',
 inc,
]
pbpaste_deps = [
  core_dep,
]


pbpaste_lib = library('pbpaste',
  pbpaste_srcs,
  install: false,
  dependencies: pbpaste_deps,
  include_directories: [
    pbpaste_inc,
  ],
)

pbpaste_dep = declare_dependency(
  include_directories: pbpaste_inc,
  link_with: pbpaste_lib,
)
