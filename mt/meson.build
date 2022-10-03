mt_srcs = [
  'mt.c',
  'mt-embedded.c',
  'mt-args.c',
  'mt-globals.c',
  'mt-ema.c',
  'mt-market.c',
  'mt-commands.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

mt_include_dirs = [
  '.',
  inc,
]

mt_deps = [
  kitty_msg_dep,
  csv_parser_dep,timelib_dep,
  optparse99_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_string_buffer_dep,
  timestamp_dep,
  log_dep,
  ms_dep,
  bytes_dep,
  incbin_dep,
]

mt_c_args = [
  '-DINCBIN_SILENCE_BITCODE_WARNING',
  '-Wno-missing-field-initializers',
]

mt_link_args = [
]

mt_test_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  mt_exec = executable('mt',
     mt_srcs,
     dependencies: mt_deps,
     include_directories: mt_include_dirs,
     link_args: mt_link_args,
     c_args: mt_c_args,
     install: false,
  )
  #test('mt', 
  #  mt_exec, args: ['--help'],
  #  workdir: mt_test_dir,
  #)
endif
