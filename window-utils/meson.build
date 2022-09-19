window_utils_srcs = [
  'window-utils.c',
]
window_utils_c_args = [
  '-Wno-address',
  ]
window_utils_dirs = [
    '.',
    inc,
  ]
window_utils_deps = [
  c_stringfn_dep,
  wildcardcmp_dep,
  parson_dep,
  space_utils_dep,
  log_dep,image_utils_dep,
  ansi_codes_dep,display_utils_dep,
  c_vector_dep,string_utils_dep,process_utils_dep,
  process_dep,app_utils_dep,
]
window_utils_link_flags = [
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

window_utils_link_flags +=  ['-Wl,-framework,Foundation']
window_utils_link_flags +=  ['-Wl,-framework,Security']
window_utils_link_flags +=  ['-Wl,-framework,SkyLight']
window_utils_link_flags +=  ['-Wl,-framework,ApplicationServices']
window_utils_link_flags +=  ['-Wl,-framework,ScriptingBridge']
window_utils_link_flags +=  ['-Wl,-framework,CoreFoundation']

window_utils_link_flags +=  ['-Wl,-framework,SystemExtensions']
window_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
window_utils_link_flags +=  ['-Wl,-framework,Carbon']
window_utils_link_flags +=  ['-Wl,-framework,Metal']
window_utils_link_flags +=  ['-Wl,-framework,MetalKit']
window_utils_link_flags +=  ['-Wl,-framework,QuartzCore']
window_utils_link_flags +=  ['-Wl,-framework,ICADevices']
window_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
window_utils_link_flags +=  ['-Wl,-framework,AppKit']
window_utils_link_flags +=  ['-Wl,-framework,IOKit']
window_utils_link_flags +=  ['-F/System/Library/PrivateFrameworks']

window_utils_lib = static_library('window-utils',
  window_utils_srcs,
  install: false,
  dependencies: window_utils_deps,
  c_args: window_utils_c_args,
  include_directories: window_utils_dirs,
)


window_utils_dep = declare_dependency(
  include_directories: window_utils_dirs,
  link_with: window_utils_lib,
  link_args: window_utils_link_flags,
)
