darwin_ls_srcs = [
  'dls.c',
  'dls-commands.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
darwin_ls_include_dirs = [
  '.',
  inc,
]
darwin_ls_deps = [
  font_utils_dep,
  memory_leak_detector_dep,kitty_dep,alacritty_dep,c_stringfn_dep,c_string_buffer_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  log_dep,
  timestamp_dep,
  ms_dep,
  bytes_dep,
  optparse99_dep,parson_dep,
  core_utils_dep,
  window_utils_dep,
  app_utils_dep,font_utils_dep,
  
  focused_dep,
  space_utils_dep,
  display_utils_dep,
  dock_utils_dep,output_utils_dep,
  menu_bar_utils_dep, table_utils_dep,string_utils_dep,process_utils_dep,window_info_dep,app_utils_dep,kitty_dep,
  httpserver_h_dep,usbdevs_utils_dep,monitor_utils_dep,
  icon_utils_dep,
  capture_utils_dep,
  process_dep,keylogger_dep,hotkey_utils_dep,
  url_h_dep,pasteboard_dep,
  querystring_c_dep,
  b64_dep,screen_utils_dep,
  leptonica_dep,timg_utils_dep,
  c_img_dep,image_utils_dep,config_utils_dep,tesseract_dep,tesseract_utils_dep,
  httpserver_utils_dep,httpserver_dep,icon_utils_dep,capture_utils_dep,wildcardcmp_dep,table_sort_dep,
]

darwin_ls_c_args = [
  '-Wno-missing-field-initializers',
  tesseract_cflags,
  leptonica_cflags,
]
darwin_ls_link_args = [
  tesseract_libs,
  leptonica_libs,
]

darwin_ls_dir = join_paths(meson.current_source_dir(), '..')
test_args = [
  ['spaces'],
  ['dock'],
  ['menu-bar'],
  ['displays'],
  ['apps','-l','5'],
  ['fonts','-l','5'],
  ['windows'],
  ['usb'],
  ['monitors'],
  ['processes'],
  ['paste'],
  ['copy','-c','test123'],
  ['paste'],
  ['kittys'],
  ['alacrittys'],
  ['focused-window'],
  ['focused-pid'],
  ['focused-space'],
  ['capture-window','-r','-o','/tmp/test-capture.png'],
  ['extract-window','-a','-c','10','-l','10'],
  ['security','-r','2'],
  ['app-icns-file','-P','/Applications/kitty.app'],
  ['get-icon-png','-P','/Applications/kitty.app','-s','512','-f','/tmp/kitty-icon-512.png'],
  ['get-icon','-P','/Applications/kitty.app','-f','/tmp/kitty.icns'],
  ['icon-info','-f','/tmp/kitty.icns'],
  ['info-plist','-P','/Applications/kitty.app'],
  ['image-conversions','-i','/tmp/test-capture.png'],
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
  test('dls help 1', darwin_ls_exec, args: ['--help'], workdir: darwin_ls_dir,timeout: 1)
  test('dls help 2', darwin_ls_exec, args: ['-h'], workdir: darwin_ls_dir,timeout: 1)
  test('dls help 3', darwin_ls_exec, args: ['help'], workdir: darwin_ls_dir,timeout: 1)
  foreach a : test_args
    test('dls ' + a[0] + ' help', darwin_ls_exec, args: ['help',a[0]], workdir: darwin_ls_dir, timeout: 1)
    test('dls ' + a[0], darwin_ls_exec, args: a, workdir: darwin_ls_dir, timeout: 30)
  endforeach
endif
