table_utils_srcs = [
  'utils.c',
]
table_utils_c_args = [
  '-Wno-address',
  ]
table_utils_dirs = [
    '.',
    inc,
  ]
table_utils_deps = [
  core_dep,
  hotkey_utils_dep,
  app_utils_dep,font_utils_dep,usbdevs_utils_dep,kitty_dep,monitor_utils_dep,
  process_dep,
  window_utils_dep,
  window_info_dep,table_sort_dep,
  space_utils_dep,display_utils_dep,process_utils_dep,
  core_utils_dep,core_debug_dep,
  layout_utils_dep,
]
table_utils_link_flags = [
  osx_link_args,
  '-Wl,-framework,CoreFoundation',
  '-Wl,-framework,Carbon',
  '-Wl,-framework,ScriptingBridge',
  '-Wl,-framework,Foundation',
  '-Wl,-framework,CoreGraphics',
  '-Wl,-framework,Cocoa',
  '-Wl,-framework,Carbon',
  '-Wl,-framework,CoreServices',
  '-Wl,-framework,AppKit'
]
table_utils_link_flags +=  ['-Wl,-framework,Foundation']
table_utils_link_flags +=  ['-Wl,-framework,Security']
table_utils_link_flags +=  ['-Wl,-framework,SkyLight']
table_utils_link_flags +=  ['-Wl,-framework,ApplicationServices']
table_utils_link_flags +=  ['-Wl,-framework,ScriptingBridge']
table_utils_link_flags +=  ['-Wl,-framework,CoreFoundation']

table_utils_link_flags +=  ['-Wl,-framework,SystemExtensions']
table_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
table_utils_link_flags +=  ['-Wl,-framework,Carbon']
table_utils_link_flags +=  ['-Wl,-framework,Metal']
table_utils_link_flags +=  ['-Wl,-framework,MetalKit']
table_utils_link_flags +=  ['-Wl,-framework,QuartzCore']
table_utils_link_flags +=  ['-Wl,-framework,ICADevices']
table_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
table_utils_link_flags +=  ['-Wl,-framework,AppKit']
table_utils_link_flags +=  ['-Wl,-framework,IOKit']
table_utils_link_flags +=  ['-F/System/Library/PrivateFrameworks']


table_utils_lib = library('table-utils',
  table_utils_srcs,
  install: false,
  dependencies: table_utils_deps,
  c_args: table_utils_c_args,
  include_directories: table_utils_dirs,
)


table_utils_dep = declare_dependency(
  include_directories: table_utils_dirs,
  link_with: table_utils_lib,
  link_args: table_utils_link_flags,
)
