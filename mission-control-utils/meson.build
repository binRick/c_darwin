mission_control_utils_srcs = [
  'mission-control-utils.c',
]
mission_control_utils_c_args = [
  '-Wno-address',
  ]
mission_control_utils_dirs = [
    '.',
    inc,
  ]
mission_control_utils_deps = [
  c_stringfn_dep,
  frameworks_dep,
  parson_dep,
  log_dep,
  ansi_codes_dep,
  c_vector_dep,
  bytes_dep,
  ms_dep,
  string_utils_dep,
  timestamp_dep,
]
_mission_control_utils_link_flags = [
  osx_link_args,
]


mission_control_utils_link_flags = [
  osx_ldflags,
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

mission_control_utils_link_flags +=  ['-Wl,-framework,Foundation']
mission_control_utils_link_flags +=  ['-Wl,-framework,Cocoa']
mission_control_utils_link_flags +=  ['-Wl,-framework,CoreGraphics']
mission_control_utils_link_flags +=  ['-Wl,-framework,CoreServices']
mission_control_utils_link_flags +=  ['-Wl,-framework,Security']
mission_control_utils_link_flags +=  ['-Wl,-framework,SkyLight']
mission_control_utils_link_flags +=  ['-Wl,-framework,ApplicationServices']
mission_control_utils_link_flags +=  ['-Wl,-framework,ScriptingBridge']
mission_control_utils_link_flags +=  ['-Wl,-framework,CoreFoundation']

mission_control_utils_link_flags +=  ['-Wl,-framework,SystemExtensions']
mission_control_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
mission_control_utils_link_flags +=  ['-Wl,-framework,Carbon']
mission_control_utils_link_flags +=  ['-Wl,-framework,Metal']
mission_control_utils_link_flags +=  ['-Wl,-framework,MetalKit']
mission_control_utils_link_flags +=  ['-Wl,-framework,QuartzCore']
mission_control_utils_link_flags +=  ['-Wl,-framework,ICADevices']
mission_control_utils_link_flags +=  ['-Wl,-framework,DeviceCheck']
mission_control_utils_link_flags +=  ['-Wl,-framework,AppKit']
mission_control_utils_link_flags +=  ['-Wl,-framework,IOKit']
mission_control_utils_link_flags +=  ['-F/System/Library/PrivateFrameworks']


mission_control_utils_lib = library('mission-control-utils',
  mission_control_utils_srcs,
  install: false,
  dependencies: mission_control_utils_deps,
  c_args: mission_control_utils_c_args,
  include_directories: mission_control_utils_dirs,
)


mission_control_utils_dep = declare_dependency(
  include_directories: mission_control_utils_dirs,
  link_with: mission_control_utils_lib,
  link_args: mission_control_utils_link_flags,
)
