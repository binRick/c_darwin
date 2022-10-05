system_utils_test_srcs = [
  'kitty-test.c',
]
system_utils_include_dirs = [
  '.',
  inc,
]
system_utils_test_deps = [
  c_greatest_dep,
  kitty_dep,
  system_utils_dep,
  c_string_buffer_dep,
  process_dep,
  log_dep,core_dep,
  c_stringfn_dep,
  module_dep,
  system_utils_dep,
  dbg_dep,
  c_vector_dep,
  app_utils_dep,
  window_utils_dep,
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
  system_utils_test_exec = executable('kitty-test',
     system_utils_test_srcs,
     dependencies: system_utils_test_deps,
     include_directories: system_utils_include_dirs,
     link_args: kitty_test_link_flags,
  )
  if get_option('enable-binary-tests')
    test('kitty-test', passh_program, args:[system_utils_test_exec,'-v'])
  endif
endif
