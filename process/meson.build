process_srcs = [
  'process.c',
]
process_link_flags = []
process_c_args = []
process_deps = [
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
  #debug_memory_dep,  
]
process_dirs = [
  '.',
  '../submodules/c_deps/submodules/c_fsio/include',
  '../submodules/c_deps/submodules/c_fsio',
  inc,
]
process_lib = static_library('process',
  process_srcs,
  dependencies: process_deps,
  c_args: process_c_args,
  include_directories: process_dirs,
)
process_dep = declare_dependency(
  include_directories: process_dirs,
  link_with: process_lib,
  link_args: process_link_flags,
)
