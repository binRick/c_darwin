pbpaste_test_srcs = [
  'pbpaste-test.c',
]
pbpaste_test_inc = [
 '../pbpaste',
 '../pbpaste-test',
 '.',
 inc,
]
pbpaste_test_deps = [
 c_string_buffer_dep,
 greatest_dep,
 pbpaste_dep,
 ansi_codes_dep,
 djbhash_dep,
 c_string_buffer_dep,
 parson_dep,
 c_fsio_dep,
 c_stringfn_dep,
 tiny_regex_c_dep,
 dbg_dep,
 ansi_utils_dep,
 c_eventemitter_dep,
 chan_dep,
 c_vector_dep,
 murmurhash_dep,
 b64_dep,
 libbeaufort_dep,
 timestamp_dep,
 c_eventemitter_dep,
 smaz_dep,
]

if get_option('enable-binaries')
  pbpaste_test = executable('pbpaste-test',
     pbpaste_test_srcs,
     dependencies: pbpaste_test_deps,
     include_directories: pbpaste_test_inc,
  )
  if get_option('enable-binary-tests')
    test('pbppaste-test', passh_program, args: [pbpaste_test])
  endif
endif
