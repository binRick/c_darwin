darwin_ls_srcs = [
  'darwin-ls.c',
  'darwin-ls-commands.c',
  'darwin-ls-httpserver.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
darwin_ls_include_dirs = [
  '.',
  inc,
]
darwin_ls_deps = [
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  log_dep,
  timestamp_dep,
  ms_dep,
  bytes_dep,
  optparse99_dep,
  core_utils_dep,
  window_utils_dep,
  app_utils_dep,font_utils_dep,
  wrec_utils_dep,
  focused_dep,
  space_utils_dep,
  display_utils_dep,
  dock_utils_dep,
  menu_bar_utils_dep, table_utils_dep,string_utils_dep,process_utils_dep,app_utils_dep,
  wrec_common_dep,httpserver_h_dep,
  wrec_capture_dep,
  process_dep,
]
darwin_ls_c_args = [
  '-Wno-missing-field-initializers',
]
darwin_ls_link_args = [
]

darwin_ls_dir = join_paths(meson.current_source_dir(), '..')

if get_option('enable-binaries')
  darwin_ls_exec = executable('dls',
     darwin_ls_srcs,
     dependencies: darwin_ls_deps,
     include_directories: darwin_ls_include_dirs,
     link_args: darwin_ls_link_args,
     c_args: darwin_ls_c_args,
     install: false,
  )
  test('darwin-ls help', darwin_ls_exec, args: ['--help'], workdir: darwin_ls_dir)
  test('darwin-ls spaces', darwin_ls_exec, args: ['spaces'], workdir: darwin_ls_dir, timeout: 3)
  test('darwin-ls dock', darwin_ls_exec, args: ['dock'], workdir: darwin_ls_dir, timeout: 3)
  test('darwin-ls menu bar', darwin_ls_exec, args: ['menu-bar'], workdir: darwin_ls_dir, timeout: 3)
  test('darwin-ls displays', darwin_ls_exec, args: ['displays'], workdir: darwin_ls_dir, timeout: 3)
#  test('darwin-ls windows', darwin_ls_exec, args: ['apps'], workdir: darwin_ls_dir, timeout: 180)
#  test('darwin-ls windows', darwin_ls_exec, args: ['fonts'], workdir: darwin_ls_dir, timeout: 180)
#  test('darwin-ls windows', darwin_ls_exec, args: ['windows'], workdir: darwin_ls_dir)
#  test('darwin-ls current space windows', darwin_ls_exec, args: ['windows','-c'], workdir: darwin_ls_dir)
endif
