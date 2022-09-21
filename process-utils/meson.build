process_utils_srcs = [
  'process-utils.c',
]
process_utils_c_args = [
  '-Wno-address',
  ]
process_utils_dirs = [
    '.',
    inc,
  ]
process_utils_deps = [
  c_stringfn_dep,
  parson_dep,
  log_dep,
  ansi_codes_dep,
  c_vector_dep,
  fort_dep,
  bytes_dep,
  ms_dep,
  process_dep,
  string_utils_dep,string_utils_dep,frameworks_dep,
  timestamp_dep,
  reproc_dep,which_dep,
]
process_utils_link_flags = [
  osx_link_args,
  '-F/System/Library/PrivateFrameworks',
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



process_utils_link_flags +=  ['-Wl,-framework,Foundation']
process_utils_link_flags +=  ['-Wl,-framework,Security']
process_utils_link_flags +=  ['-Wl,-framework,SkyLight']
process_utils_link_flags +=  ['-Wl,-framework,ApplicationServices']
process_utils_link_flags +=  ['-Wl,-framework,ScriptingBridge']
process_utils_link_flags +=  ['-Wl,-framework,CoreFoundation']

process_utils_link_flags +=  ['-Wl,-framework,SystemExtensions']
process_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
process_utils_link_flags +=  ['-Wl,-framework,Carbon']
process_utils_link_flags +=  ['-Wl,-framework,Metal']
process_utils_link_flags +=  ['-Wl,-framework,MetalKit']
process_utils_link_flags +=  ['-Wl,-framework,QuartzCore']
process_utils_link_flags +=  ['-Wl,-framework,ICADevices']
process_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
process_utils_link_flags +=  ['-Wl,-framework,AppKit']
process_utils_link_flags +=  ['-Wl,-framework,IOKit']

process_utils_lib = static_library('process-utils',
  process_utils_srcs,
  install: false,
  dependencies: process_utils_deps,
  c_args: process_utils_c_args,
  include_directories: process_utils_dirs,
)

process_utils_dep = declare_dependency(
  include_directories: process_utils_dirs,
  link_with: process_utils_lib,
  link_args: process_utils_link_flags,
)
