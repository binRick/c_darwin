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
  c_stringfn_dep,hotkey_utils_dep,
  parson_dep,
  log_dep,
  ansi_codes_dep,
  c_vector_dep,
  process_dep,
  fort_dep,
  bytes_dep,
  ms_dep,
  string_utils_dep,
  timestamp_dep,
  window_info_dep,window_sort_dep,wildcardcmp_dep,space_utils_dep,display_utils_dep,process_utils_dep,
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
