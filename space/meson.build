space_srcs = [
  'space.c',
]
space_c_args = [
  '-Wno-address',
  ]
space_dirs = [
    '.',
    inc,
  ]
space_deps = [
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
  module_dep,
  process_dep,
  wrec_common_dep,
  wrec_capture_dep,
  timestamp_dep,
  window_utils_dep,
  core_utils_dep,
  wrec_image_dep,
]
space_link_flags = [
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

space_link_flags +=  ['-Wl,-framework,Foundation']
space_link_flags +=  ['-Wl,-framework,Security']
space_link_flags +=  ['-Wl,-framework,SkyLight']
space_link_flags +=  ['-Wl,-framework,ApplicationServices']
space_link_flags +=  ['-Wl,-framework,ScriptingBridge']
space_link_flags +=  ['-Wl,-framework,CoreFoundation']

space_link_flags +=  ['-Wl,-framework,SystemExtensions']
space_link_flags +=  ['-Wl,-framework,DeviceCheck']
space_link_flags +=  ['-Wl,-framework,Carbon']
space_link_flags +=  ['-Wl,-framework,Metal']
space_link_flags +=  ['-Wl,-framework,MetalKit']
space_link_flags +=  ['-Wl,-framework,QuartzCore']
space_link_flags +=  ['-Wl,-framework,ICADevices']
space_link_flags +=  ['-Wl,-framework,DeviceCheck']
space_link_flags +=  ['-Wl,-framework,AppKit']
space_link_flags +=  ['-Wl,-framework,IOKit']
space_link_flags +=  ['-F/System/Library/PrivateFrameworks']

space_lib = static_library('space',
  space_srcs,
  install: false,
  dependencies: space_deps,
  c_args: space_c_args,
  include_directories: space_dirs,
)


space_dep = declare_dependency(
  include_directories: space_dirs,
  link_with: space_lib,
  link_args: space_link_flags,
)
