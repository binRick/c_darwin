space_utils_srcs = [
  'space-utils.c',
]
space_utils_c_args = [
  '-Wno-address',
  ]
space_utils_dirs = [
    '.',
    inc,
  ]
space_utils_deps = [
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
  timestamp_dep,
  core_utils_dep,
]
space_utils_link_flags = [
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

space_utils_link_flags +=  ['-Wl,-framework,Foundation']
space_utils_link_flags +=  ['-Wl,-framework,Security']
space_utils_link_flags +=  ['-Wl,-framework,SkyLight']
space_utils_link_flags +=  ['-Wl,-framework,ApplicationServices']
space_utils_link_flags +=  ['-Wl,-framework,ScriptingBridge']
space_utils_link_flags +=  ['-Wl,-framework,CoreFoundation']

space_utils_link_flags +=  ['-Wl,-framework,SystemExtensions']
space_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
space_utils_link_flags +=  ['-Wl,-framework,Carbon']
space_utils_link_flags +=  ['-Wl,-framework,Metal']
space_utils_link_flags +=  ['-Wl,-framework,MetalKit']
space_utils_link_flags +=  ['-Wl,-framework,QuartzCore']
space_utils_link_flags +=  ['-Wl,-framework,ICADevices']
space_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
space_utils_link_flags +=  ['-Wl,-framework,AppKit']
space_utils_link_flags +=  ['-Wl,-framework,IOKit']
space_utils_link_flags +=  ['-F/System/Library/PrivateFrameworks']

space_utils_lib = static_library('space-utils',
  space_utils_srcs,
  install: false,
  dependencies: space_utils_deps,
  c_args: space_utils_c_args,
  include_directories: space_utils_dirs,
)


space_utils_dep = declare_dependency(
  include_directories: space_utils_dirs,
  link_with: space_utils_lib,
  link_args: space_utils_link_flags,
)
