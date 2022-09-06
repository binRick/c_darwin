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
  memory_leak_detector_dep,kitty_dep,alacritty_dep,
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
  dock_utils_dep,output_utils_dep,
  menu_bar_utils_dep, table_utils_dep,string_utils_dep,process_utils_dep,app_utils_dep,
  wrec_common_dep,httpserver_h_dep,usbdevs_utils_dep,monitor_utils_dep,
  wrec_capture_dep,icon_utils_dep,
  process_dep,
]
darwin_ls_c_args = [
  '-Wno-missing-field-initializers',
]
darwin_ls_link_args = [
]

darwin_ls_dir = join_paths(meson.current_source_dir(), '..')
test_args = [
  'spaces','args','dock','menu-bar','displays','apps',
  'fonts',
  'usb','monitors','processes',
  'kittys','alacrittys',
  #'windows',
  ]

if get_option('enable-binaries')
  darwin_ls_exec = executable('dls',
     darwin_ls_srcs,
     dependencies: darwin_ls_deps,
     include_directories: darwin_ls_include_dirs,
     link_args: darwin_ls_link_args,
     c_args: darwin_ls_c_args,
     install: false,
  )
  test('darwin-ls help 1', darwin_ls_exec, args: ['--help'], workdir: darwin_ls_dir,timeout: 1)
  test('darwin-ls help 2', darwin_ls_exec, args: ['-h'], workdir: darwin_ls_dir,timeout: 1)
  test('darwin-ls help 3', darwin_ls_exec, args: ['help'], workdir: darwin_ls_dir,timeout: 1)
  foreach a : test_args
    test('darwin-ls ' + a + ' help', darwin_ls_exec, args: ['help',a], workdir: darwin_ls_dir, timeout: 1)
    test('darwin-ls ' + a, darwin_ls_exec, args: [a], workdir: darwin_ls_dir, timeout: 25)
  endforeach
endif
