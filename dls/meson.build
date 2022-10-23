darwin_ls_srcs = [
  'dls.c',
  'dls-commands.c',
  'dls-aliases.c',
]
if get_option('gpp_processor')
  message('GPP Processor Enabled')
  run_command('sh','-c','cat '+meson.current_source_dir()+'/vars/*.yaml|yaml2json|json2yaml > '+meson.current_source_dir()+'/.vars.yaml',check:true).stdout().strip()
  foreach f : run_command('find',meson.current_source_dir(),'-type','f','-name','*.gpp',check:true).stdout().strip().split('\n')
    fn_hash = ''
    fn_hash1 = ''
    temp_file = run_command('mktemp',check:true).stdout().strip()
    fn = join_paths(meson.current_source_dir(),'gpp-'+fs.stem(f))
    gpp_out = run_command('gpp','-x','-o',temp_file,f,check:true).stdout().strip()
    gpp_out_hash = fs.hash(temp_file,'sha256')
    if fs.exists(fn)
      fn_hash = fs.hash(fn,'sha256')
    endif
    if(not fs.exists(fn) or fn_hash != gpp_out_hash)
      run_command('cp',temp_file,fn,check:true).stdout().strip()
      fn_hash1 = fs.hash(fn,'sha256')
    endif
    message('GPP> '+f+'|'+fn+'|'+temp_file+'|',gpp_out_hash,'|',fn_hash,'|after copy:',fn_hash1)
    if not darwin_ls_srcs.contains(fn)
      darwin_ls_srcs += fn
    endif
  endforeach
endif

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))
darwin_ls_include_dirs = [
  '.',
  inc,
]
darwin_ls_deps = [
  core_dep,
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
  menu_bar_utils_dep, table_utils_dep,process_utils_dep,window_info_dep,app_utils_dep,
  usbdevs_utils_dep,monitor_utils_dep,
  icon_utils_dep,
  capture_utils_dep,
  process_dep,keylogger_dep,hotkey_utils_dep,
  pasteboard_dep,
  vips_dep,
  screen_utils_dep,
  image_utils_dep,config_utils_dep,tesseract_utils_dep,
  httpserver_utils_dep,httpserver_dep,icon_utils_dep,capture_utils_dep,table_sort_dep,
  capture_type_dep,
  capture_animate_dep,
  capture_save_dep,
  layout_utils_dep,
  window_dep,
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
timeout = 15
test_args = [
  #{ 'name': 'List Up to 10 Spaces', 'command': 'spaces', 'args': ['--limit','10'], 'timeout': 20, },
  #{ 'name': 'List Up to 10 Displays', 'command': 'displays', 'args': ['--limit','10'], 'timeout': timeout,  },
  #{ 'name': 'List Up to 10 Installed Applications Sorted By Name Ascending', 'command': 'apps', 'args': ['--sort-by','name','--asc','--limit','10'],   'timeout': 30,},
  #{ 'name': 'List 10 Largest Fonts', 'command': 'fonts', 'args': ['--sort-by','size','--desc','--limit','10'],   'timeout': 30,},
  #{ 'name': 'List 10 Newest Windows', 'command': 'windows', 'args': ['--sort-by','id','--desc','--limit','10'],   'timeout': timeout,},
  #{ 'name': 'List Windows Not on Currently Focused Display', 'command': 'windows', 'args': ['--not-current-display'],   'timeout': 10,},
  #{ 'name': 'List Windows On Currently Focused Display', 'command': 'windows', 'args': ['--current-display'],   'timeout': 10,},
  #{ 'name': 'List Windows Not on Currently Focused Space', 'command': 'windows', 'args': ['--not-current-space'],   'timeout': 10,},
  #{ 'name': 'List Windows On Currently Focused Space', 'command': 'windows', 'args': ['--current-space'],   'timeout': 10,},
  #{ 'name': 'List Minimized Windows', 'command': 'windows', 'args': ['--minimized'],   'timeout': timeout,},
  #  { 'name': 'List Non Minimized Windows', 'command': 'windows', 'args': ['--non-minimized'],   'timeout': 3,},
  #{ 'name': 'List USB Devices', 'command': 'usb', 'args': [],   'timeout': timeout,},
  #  { 'name': 'List Monitors', 'command': 'monitors', 'args': [],   'timeout': timeout,},
  #{ 'name': 'List Processes', 'command': 'processes', 'args': [],   'timeout': 30,},
  { 'name': 'Copy Contents to Clipboard', 'command': 'clipboard', 'args': ['copy','--content','test123'],   'timeout': timeout,},
  { 'name': 'Paste Clipboard Contents', 'command': 'clipboard', 'args': ['paste'],   'timeout': timeout,},
  #{ 'name': 'List Kitty Processes', 'command': 'kittys', 'args': [],   'timeout': 10,},
  #  { 'name': 'List Alacritty Processes', 'command': 'alacrittys', 'args': [],   'timeout': 10,},
  #  { 'name': 'Capture Windows','command':'capture', 'args': ['window','--write','--dir','/tmp/dls-test-windows','--all','--limit','5','--formats','all'], 'timeout': 15, },
  #  { 'name': 'Capture Spaces','command':'capture', 'args': ['space','--write','--dir','/tmp/dls-test-spaces','--all','--limit','5','--formats','all'], 'timeout': 15, },
  # { 'name': 'Capture Displays','command':'capture', 'args': ['display','--write','--dir','/tmp/dls-test-displays','--all','--limit','5','--formats','all'], 'timeout': 15, },
  #  { 'name': 'Extract Text Content from 2 Window Images Concurrently', 'command':'extract', 'args': ['--window','--all','--limit','2','--concurrency','2'], 'timeout': 60, },
  #  { 'name': 'Open System Preferences and Configure Security', 'command':'security', 'args': ['--retries','3'], 'timeout': 30, },
  ]
CaptureCommands = [
  { 'name':'Window','command':'window','plural':'Windows'},
  { 'name':'Display','command':'display','plural':'Displays'},
  { 'name':'Space','command':'space','plural':'Spaces'},
  ]
CaptureFormats = ['png','gif','webp','jpeg','qoi','tiff','all']
CaptureWidths = [
  {'name': '200 Pixels Wide','arg':'--width','value':'200','slug':'200x'},
  ]
CaptureModes = [
  { 'name': 'Uncompressed', 'flags': [], },
  { 'name': 'Compressed', 'flags': ['-z'], },
  ]
foreach w : CaptureWidths
foreach c : CaptureCommands
foreach m : CaptureModes
foreach format : CaptureFormats
  name = ' '.join(
            'Capture',
            w['name'],
            format.to_upper(),
            c['name'],
            m['name'],
            )
  dir = '/'.join(
            '/tmp',
            'dls-tests',
            c['name'].to_lower(),
            format.to_lower(),
            w['slug'],
            m['name'].to_lower().replace(' ','-'),
      )
  args = [
      c['command'],
      '--display',
      '--no-progress',
      '--purge','--write','--dir',dir,
      '--all','--concurrency','20',
      '--formats',format,
      w['arg'],w['value'],
      m['flags'],
  ]
  test_args += {
    'name': name.strip(),
    'command':'capture',
    'args': args,
    'timeout':30,
    }
endforeach
endforeach
endforeach
endforeach

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
