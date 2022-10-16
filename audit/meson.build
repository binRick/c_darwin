audit_srcs = [
  'audit.c',
]
audit_link_flags = []
audit_c_args = []
audit_deps = [
  core_dep,
  kitty_dep,  
]
audit_dirs = [
  '.',
  inc,
]
audit_lib = library('audit',
  audit_srcs,
  install: false,
  dependencies: audit_deps,
  c_args: audit_c_args,
  include_directories: audit_dirs,
)
audit_dep = declare_dependency(
  include_directories: audit_dirs,
  link_with: audit_lib,
  link_args: audit_link_flags,
)
