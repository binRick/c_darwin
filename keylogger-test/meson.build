keylogger_test_srcs = [
  'keylogger-test.c',
]
keylogger_test_inc = [
 '.',
 inc,
]
keylogger_test_deps = [
 c_string_buffer_dep,
 c_greatest_dep,
 keylogger_dep,
 window_utils_dep,app_utils_dep,
]

kitty_test_link_flags = osx_ldflags

kitty_test_link_flags +=  ['-Wl,-framework,Foundation']
kitty_test_link_flags +=  ['-Wl,-framework,Security']
kitty_test_link_flags +=  ['-Wl,-framework,SkyLight']
kitty_test_link_flags +=  ['-Wl,-framework,ApplicationServices']
kitty_test_link_flags +=  ['-Wl,-framework,ScriptingBridge']
kitty_test_link_flags +=  ['-Wl,-framework,CoreFoundation']

kitty_test_link_flags +=  ['-Wl,-framework,SystemExtensions']
kitty_test_link_flags +=  ['-Wl,-framework,DeviceCheck']
kitty_test_link_flags +=  ['-Wl,-framework,Carbon']
kitty_test_link_flags +=  ['-Wl,-framework,Metal']
kitty_test_link_flags +=  ['-Wl,-framework,MetalKit']
kitty_test_link_flags +=  ['-Wl,-framework,QuartzCore']
kitty_test_link_flags +=  ['-Wl,-framework,ICADevices']
kitty_test_link_flags +=  ['-Wl,-framework,DeviceCheck']
kitty_test_link_flags +=  ['-Wl,-framework,AppKit']
kitty_test_link_flags +=  ['-Wl,-framework,IOKit']
kitty_test_link_flags +=  ['-F/System/Library/PrivateFrameworks']
if get_option('enable-binaries')
  keylogger_test = executable('keylogger-test',
     keylogger_test_srcs,
     dependencies: keylogger_test_deps,
     include_directories: keylogger_test_inc,
     link_args: kitty_test_link_flags,
  )
  #if get_option('enable-binary-tests')
    #    test('keylogger-test', keylogger_test, args: [])
  #endif
endif
