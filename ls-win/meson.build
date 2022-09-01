ls_win_srcs = [
  'ls-win.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
ls_win_include_dirs = [
  '.',
  inc,
]
ls_win_deps = [
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  logh_dep,
  timestamp_dep,
  ms_dep,
  bytes_dep,
  optparse99_dep,
  core_utils_dep,
  window_utils_dep,
  app_utils_dep,
  wrec_utils_dep,
  wrec_common_dep,
  wrec_capture_dep,
  process_dep,
]
ls_win_c_args = [
  '-Wno-missing-field-initializers',
]
ls_win_link_args = [
]

ls_win_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  ls_win_exec = executable('ls-win',
     ls_win_srcs,
     dependencies: ls_win_deps,
     include_directories: ls_win_include_dirs,
     link_args: ls_win_link_args,
     c_args: ls_win_c_args,
     install: false,
  )
  test('ls-win help', ls_win_exec, args: ['--help'], workdir: ls_win_dir)
  test('ls-win windows', ls_win_exec, args: ['windows'], workdir: ls_win_dir)
  test('ls-win current space windows', ls_win_exec, args: ['windows','-c'], workdir: ls_win_dir)
endif
