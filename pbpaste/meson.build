pbpaste_srcs = [
  'pbpaste.c',
]
pbpaste_inc = [
 '.',
 inc,
]
pbpaste_deps = [
 parson_dep,
 c_string_buffer_dep,
 subprocess_dep,
 c_greatest_dep,
 ansi_codes_dep,
 djbhash_dep,
 c_string_buffer_dep,
 c_fsio_dep,
 c_stringfn_dep,
 tiny_regex_c_dep,
 ansi_utils_dep,
 c_eventemitter_dep,
 chan_dep,
 c_vector_dep,
 murmurhash_dep,
 b64_dep,
 timestamp_dep,
 c_eventemitter_dep,
 smaz_dep, 
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
