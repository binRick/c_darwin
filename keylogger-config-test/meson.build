keylogger_config_test_srcs = [
  'keylogger-config-test.c',
]
keylogger_config_test_inc = [
 '../keylogger',
 '../keylogger-config-test',
 '.',
 inc,
]
keylogger_config_test_deps = [
 c_string_buffer_dep,
 c_greatest_dep,
 module_dep,
 keylogger_config_dep,
 ansi_codes_dep,
]

if get_option('enable-binaries')
    test('Keylogger|Convert YAML Keybind Config File to JSON',
      yaml2json_program, 
      args: [
          'keybinds.yaml',
          'keybinds.json',
        ],
      workdir: join_paths(meson.current_source_dir(),'../etc'),
      #      is_parallel: false,
    )
    keylogger_config_test = executable('keylogger-config-test',
       keylogger_config_test_srcs,
       dependencies: keylogger_config_test_deps,
       include_directories: keylogger_config_test_inc,
       #is_parallel: false,
    )
  if get_option('enable-keylogger-config-tests')
    test('Keylogger|List Keylogger Suites and Tests',keylogger_config_test, args: ['-l','-v'],
      #is_parallel: false,
      )
    test('Keylogger|Read Keylogger Config',keylogger_config_test, args: ['-v','-t','read_config'],
      #is_parallel: false,
      )
    test('Keylogger|Parse Keylogger Config',keylogger_config_test, args: ['-v','-t','parse_config'],
      #is_parallel: false,
      )
    test('Keylogger|Start and Check Keylogger',keylogger_config_test, args: ['-v','-t','_start'],
      #is_parallel: false,
      )
    test('Keylogger|Check Keylogger PID',keylogger_config_test, args: ['-v','-t','_check_pid'],
      is_parallel: false,
      )
    test('Keylogger|Validate Keylogger Key Bindings',keylogger_config_test, args: ['-v','-t','_validate_configured_keybinds'],
      #is_parallel: false,
      )
    test('Keylogger|Extract Active Keylogger Key Bindings',keylogger_config_test, args: ['-v','-t','_active_keybinds'],
      #is_parallel: false,
      )
    test('Keylogger|Extract Inactive Keylogger Key Bindings',keylogger_config_test, args: ['-v','-t','_inactive_keybinds'],
      #is_parallel: false,
      )
    test('Keylogger|Test Keylogger Config Logic',keylogger_config_test, args: ['-a','-v'],
      #is_parallel: false,
      )
    test('Keylogger|Stop Keylogger',keylogger_config_test, args: ['-t','_stop'],
      #is_parallel: false,
      )
    test('Keylogger|View Keybinds JSON Config File',
      bat_program, args: [
        '-pp', '--file-name','Keylogger JSON Config','--italic-text','always','--line-range',':35',
        '--theme','Monokai Extended Bright','--force-colorization','--paging=never','keybinds.json',
        ],
      workdir: join_paths(meson.current_source_dir(),'../etc'),
      #is_parallel: false,
      )
    test('Keylogger|View Keybinds YAML Config File',
      bat_program, args: 
      [
        '-pp', '--file-name','Keylogger YAML Config','--italic-text','always','--line-range',':35',
        '--theme','Monokai Extended Bright','--force-colorization','--paging=never','keybinds.yaml',
        ],
      workdir: join_paths(meson.current_source_dir(),'../etc'),
      #is_parallel: false,
      )
  endif
endif
