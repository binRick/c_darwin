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
  core_dep,
  kitty_msg_dep,
  core_utils_dep,
  core_image_dep,
  font_utils_dep,
  kitty_dep,alacritty_dep,
  db_dep,
  window_utils_dep,
  app_utils_dep,font_utils_dep,
  capture_type_dep,
  focused_dep,
  space_utils_dep,system_utils_dep,
  display_utils_dep,space_utils_dep,
  dock_utils_dep,output_utils_dep,
  menu_bar_utils_dep, table_utils_dep,process_utils_dep,window_info_dep,app_utils_dep,kitty_dep,
  httpserver_h_dep,usbdevs_utils_dep,monitor_utils_dep,
  icon_utils_dep,
  capture_utils_dep,
  process_dep,keylogger_dep,hotkey_utils_dep,
  pasteboard_dep,
  querystring_c_dep,vips_dep,
  screen_utils_dep,msf_gif_dep,
  image_utils_dep,config_utils_dep,tesseract_utils_dep,
  httpserver_utils_dep,httpserver_dep,icon_utils_dep,capture_utils_dep,wildcardcmp_dep,table_sort_dep,
  capture_type_dep,
  capture_animate_dep,
  capture_save_dep,
  layout_utils_dep,
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
  { 'name': 'List Up to 10 Displays', 'command': 'displays', 'args': ['--limit','10'], 'timeout': 5,  },
  { 'name': 'List Up to 10 Installed Applications Sorted By Name Ascending', 'command': 'apps', 'args': ['--sort-by','name','--asc','--limit','10'],   'timeout': 30,},
  { 'name': 'List 10 Largest Fonts', 'command': 'fonts', 'args': ['--sort-by','size','--desc','--limit','10'],   'timeout': 30,},
  { 'name': 'List 10 Newest Windows', 'command': 'windows', 'args': ['--sort-by','id','--desc','--limit','10'],   'timeout': 10,},
  { 'name': 'List Windows Not on Currently Focused Display', 'command': 'windows', 'args': ['--not-current-display'],   'timeout': 10,},
  { 'name': 'List Windows On Currently Focused Display', 'command': 'windows', 'args': ['--current-display'],   'timeout': 10,},
  { 'name': 'List Windows Not on Currently Focused Space', 'command': 'windows', 'args': ['--not-current-space'],   'timeout': 10,},
  { 'name': 'List Windows On Currently Focused Space', 'command': 'windows', 'args': ['--current-space'],   'timeout': 10,},
  { 'name': 'List Minimized Windows', 'command': 'windows', 'args': ['--minimized'],   'timeout': 3,},
  #  { 'name': 'List Non Minimized Windows', 'command': 'windows', 'args': ['--non-minimized'],   'timeout': 3,},
  { 'name': 'List USB Devices', 'command': 'usb', 'args': [],   'timeout': 3,},
  { 'name': 'List Monitors', 'command': 'monitors', 'args': [],   'timeout': 3,},
  { 'name': 'List Processes', 'command': 'processes', 'args': [],   'timeout': 30,},
  { 'name': 'Copy Contents to Clipboard', 'command': 'clipboard', 'args': ['copy','--content','test123'],   'timeout': 10,},
  { 'name': 'Paste Clipboard Contents', 'command': 'clipboard', 'args': ['paste'],   'timeout': 10,},
  { 'name': 'List Kitty Processes', 'command': 'kittys', 'args': [],   'timeout': 10,},
  { 'name': 'List Alacritty Processes', 'command': 'alacrittys', 'args': [],   'timeout': 10,},
  #{ 'name': 'Show Focused Window Info', 'command': 'focused', 'args': ['--window'],   'timeout': 3,},
  #  { 'name': 'Show Focused Space Info', 'command': 'focused', 'args': ['--space'],   'timeout': 3,},
  #{ 'name': 'Show Focused PID Info', 'command': 'focused', 'args': ['--pid'],   'timeout': 3,},
  { 'name': 'Capture GIF Window','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--window','--all','--limit','1','--out','/tmp/dls-test/window-capture.gif','--formats','gif'], 'timeout': 5, },
  { 'name': 'Capture PNG Window','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--window','--all','--limit','1','--out','/tmp/dls-test/window-capture.png','--formats','png'], 'timeout': 5, },
  { 'name': 'Capture JPEG Window','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--window','--all','--limit','1','--out','/tmp/dls-test/window-capture.jpeg','--formats','jpeg'], 'timeout': 5, },
  { 'name': 'Capture TIFF Window','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--window','--all','--limit','1','--out','/tmp/dls-test/window-capture.tiff','--formats','tiff'], 'timeout': 5, },
  { 'name': 'Capture GIF Space','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--space','--all','--limit','1','--out','/tmp/dls-test/space-capture.gif','--formats','gif'], 'timeout': 5, },
  { 'name': 'Capture PNG Space','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--space','--all','--limit','1','--out','/tmp/dls-test/space-capture.png','--formats','png'], 'timeout': 5, },
  { 'name': 'Capture JPEG Space','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--space','--all','--limit','1','--out','/tmp/dls-test/space-capture.jpeg','--formats','jpeg'], 'timeout': 5, },
  { 'name': 'Capture TIFF Space','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--space','--all','--limit','1','--out','/tmp/dls-test/space-capture.tiff','--formats','tiff'], 'timeout': 5, },
  { 'name': 'Capture GIF Display','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--display','--all','--limit','1','--out','/tmp/dls-test/display-capture.gif','--formats','gif'], 'timeout': 5, },
  { 'name': 'Capture PNG Display','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--display','--all','--limit','1','--out','/tmp/dls-test/display-capture.png','--formats','png'], 'timeout': 5, },
  { 'name': 'Capture JPEG Display','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--display','--all','--limit','1','--out','/tmp/dls-test/display-capture.jpeg','--formats','jpeg'], 'timeout': 5, },
  { 'name': 'Capture TIFF Display','command':'capture', 'args': ['--write','--dir','/tmp/dls-test','--display','--all','--limit','1','--out','/tmp/dls-test/display-capture.tiff','--formats','tiff'], 'timeout': 5, },
  #  { 'name': 'Extract Text Content from 2 Window Images Concurrently', 'command':'extract', 'args': ['--window','--all','--limit','2','--concurrency','2'], 'timeout': 60, },
  #  { 'name': 'Open System Preferences and Configure Security', 'command':'security', 'args': ['--retries','3'], 'timeout': 30, },
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
