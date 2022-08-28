screen_utils_srcs = [
  'screen-utils.c',
  'screen-utils-module.c',
]
screen_utils_c_args = [
  '-Wno-address',
  ]
screen_utils_dirs = [
    '.',
    inc,
  ]
screen_utils_deps = [
  core_utils_dep,
  c_vector_dep,
  active_app_dep,
  c_stringfn_dep,
  parson_dep,
  logh_dep,
  ansi_codes_dep,
  timestamp_dep,
  bytes_dep, ms_dep,
  system_utils_dep,
  hash_dep,
  core_utils_dep,
  wrec_common_dep,
  wrec_capture_dep,
  module_dep,
  process_dep,
  timestamp_dep,
  wrec_image_dep,
]
screen_utils_link_flags = [
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

screen_utils_link_flags +=  ['-Wl,-framework,Foundation']
screen_utils_link_flags +=  ['-Wl,-framework,Security']
screen_utils_link_flags +=  ['-Wl,-framework,SkyLight']
screen_utils_link_flags +=  ['-Wl,-framework,ApplicationServices']
screen_utils_link_flags +=  ['-Wl,-framework,ScriptingBridge']
screen_utils_link_flags +=  ['-Wl,-framework,CoreFoundation']

screen_utils_link_flags +=  ['-Wl,-framework,SystemExtensions']
screen_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
screen_utils_link_flags +=  ['-Wl,-framework,Carbon']
screen_utils_link_flags +=  ['-Wl,-framework,Metal']
screen_utils_link_flags +=  ['-Wl,-framework,MetalKit']
screen_utils_link_flags +=  ['-Wl,-framework,QuartzCore']
screen_utils_link_flags +=  ['-Wl,-framework,ICADevices']
screen_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
screen_utils_link_flags +=  ['-Wl,-framework,AppKit']
screen_utils_link_flags +=  ['-Wl,-framework,IOKit']
screen_utils_link_flags +=  ['-F/System/Library/PrivateFrameworks']

screen_utils_lib = static_library('screen-utils',
  screen_utils_srcs,
  install: false,
  dependencies: screen_utils_deps,
  c_args: screen_utils_c_args,
  include_directories: screen_utils_dirs,
)


screen_utils_dep = declare_dependency(
  include_directories: screen_utils_dirs,
  link_with: screen_utils_lib,
  link_args: screen_utils_link_flags,
)
