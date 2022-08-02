audit_srcs = [
  'audit.c',
]
audit_link_flags = []
audit_c_args = []
audit_deps = [
  ansi_codes_dep,
  djbhash_dep,
  c_stringfn_dep,
  c_vector_dep,
  tiny_regex_c_dep,
  socket99_dep,
  c_string_buffer_dep,
  c_stringfn_dep,
  str_replace_dep,
  occurrences_dep,
  parson_dep,
  c_fsio_dep,  
  debug_memory_dep,  
  kitty_dep,  
]
audit_dirs = [
  '../submodules/c_deps/submodules/c_fsio/include',
  '.',
  '../submodules/c_deps/submodules/c_fsio',
  inc,
]
audit_lib =static_library('audit',
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
