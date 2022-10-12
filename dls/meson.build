darwin_ls_srcs = [
  'dls.c',
  'dls-commands.c',
  'dls-aliases.c',
]
inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
darwin_ls_include_dirs = [
  '.',
  inc,
]
darwin_ls_deps = [
  kitty_msg_dep,
  core_utils_dep,
  core_image_dep,
  optparse99_dep,
  font_utils_dep,
  memory_leak_detector_dep,kitty_dep,alacritty_dep,c_stringfn_dep,c_string_buffer_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  ansi_codes_dep,
  log_dep,core_dep,
  timestamp_dep,
  ms_dep,
  bytes_dep,
  core_utils_dep,c_forever_dep,
  window_utils_dep,
  app_utils_dep,font_utils_dep,
  capture_type_dep,
  focused_dep,
  space_utils_dep,tinydir_dep,system_utils_dep,
  display_utils_dep,
  dock_utils_dep,output_utils_dep,
  menu_bar_utils_dep, table_utils_dep,string_utils_dep,process_utils_dep,window_info_dep,app_utils_dep,kitty_dep,vips_dep,clamp_dep,
  httpserver_h_dep,usbdevs_utils_dep,monitor_utils_dep,
  icon_utils_dep,
  capture_utils_dep,
  process_dep,keylogger_dep,hotkey_utils_dep,
  url_h_dep,pasteboard_dep,
  querystring_c_dep,tempdir_dep,vips_dep,
  b64_dep,screen_utils_dep,msf_gif_dep,
  leptonica_dep,
  c_img_dep,image_utils_dep,config_utils_dep,tesseract_dep,tesseract_utils_dep,
  httpserver_utils_dep,httpserver_dep,icon_utils_dep,capture_utils_dep,wildcardcmp_dep,table_sort_dep,
  capture_type_dep,
  capture_animate_dep,
  chan_dep,
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
  { 'name': 'List Up to 10 Spaces', 'command': 'spaces', 'args': ['--limit','10'], 'timeout': 5, },
  { 'name': 'Show Dock Info', 'command': 'dock', 'args': [], 'timeout': 5, },
  { 'name': 'Show Menu Bar Info', 'command': 'menu-bar', 'args': [],  'timeout': 5,},
  { 'name': 'List Up to 10 Displays', 'command': 'displays', 'args': ['--limit','10'], 'timeout': 5,  },
  { 'name': 'List Up to 10 Installed Applications Sorted By Name Ascending', 'command': 'apps', 'args': ['--sort-by','name','--asc','--limit','10'],   'timeout': 30,},
  { 'name': 'List 10 Largest Fonts', 'command': 'fonts', 'args': ['--sort-by','size','--desc','--limit','10'],   'timeout': 30,},
  { 'name': 'List 10 Newest Windows', 'command': 'windows', 'args': ['--sort-by','id','--desc','--limit','10'],   'timeout': 10,},
  { 'name': 'List Windows Not on Currently Focused Display', 'command': 'windows', 'args': ['--not-current-display'],   'timeout': 10,},
  { 'name': 'List Windows On Currently Focused Display', 'command': 'windows', 'args': ['--current-display'],   'timeout': 10,},
  { 'name': 'List Windows Not on Currently Focused Space', 'command': 'windows', 'args': ['--not-current-space'],   'timeout': 10,},
  { 'name': 'List Windows On Currently Focused Space', 'command': 'windows', 'args': ['--current-space'],   'timeout': 10,},
  { 'name': 'List Minimized Windows', 'command': 'windows', 'args': ['--minimized'],   'timeout': 3,},
  { 'name': 'List Non Minimized Windows', 'command': 'windows', 'args': ['--non-minimized'],   'timeout': 3,},
  { 'name': 'List USB Devices', 'command': 'usb', 'args': [],   'timeout': 3,},
  { 'name': 'List Monitors', 'command': 'monitors', 'args': [],   'timeout': 3,},
  { 'name': 'List Processes', 'command': 'processes', 'args': [],   'timeout': 30,},
  { 'name': 'Copy Contents to Clipboard', 'command': 'copy', 'args': ['--content','test123'],   'timeout': 10,},
  { 'name': 'Paste Clipboard Contents', 'command': 'paste', 'args': [],   'timeout': 10,},
  { 'name': 'List Kitty Processes', 'command': 'kittys', 'args': [],   'timeout': 10,},
  { 'name': 'List Alacritty Processes', 'command': 'alacrittys', 'args': [],   'timeout': 10,},
  { 'name': 'Show Focused Window Info', 'command': 'focused-window', 'args': [],   'timeout': 3,},
  { 'name': 'Show Focused Space Info', 'command': 'focused-space', 'args': [],   'timeout': 3,},
  { 'name': 'Show Focused PID Info', 'command': 'focused-pid', 'args': [],   'timeout': 3,},
  { 'name': 'Capture GIF Window','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--window','--all','--limit','1','--output-file','/tmp/dls-test/window-capture.gif','--format','gif'], 'timeout': 5, },
  { 'name': 'Capture PNG Window','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--window','--all','--limit','1','--output-file','/tmp/dls-test/window-capture.png','--format','png'], 'timeout': 5, },
  { 'name': 'Capture JPEG Window','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--window','--all','--limit','1','--output-file','/tmp/dls-test/window-capture.jpeg','--format','jpeg'], 'timeout': 5, },
  { 'name': 'Capture TIFF Window','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--window','--all','--limit','1','--output-file','/tmp/dls-test/window-capture.tiff','--format','tiff'], 'timeout': 5, },
  { 'name': 'Extract Text Content from 2 Window Images Concurrently', 'command':'extract', 'args': ['--window','--all','--limit','2','--concurrency','2'], 'timeout': 60, },
  { 'name': 'Open System Preferences and Configure Security', 'command':'security', 'args': ['--retries','3'], 'timeout': 30, },
  ]
xxxx = [
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
  foreach a : test_args
    test('dls ' + a['name'], darwin_ls_exec, args: [a['command'], a['args']], workdir: darwin_ls_dir, timeout: a['timeout'])
  endforeach
endif
