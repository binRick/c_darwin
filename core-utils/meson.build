core_utils_srcs = [
  'core-utils.c',
]
core_utils_dirs = [
  '.',
  inc,
]
core_utils_deps = [
  c_stringfn_dep,
  c_string_buffer_dep,
  c_img_dep,table_utils_dep,image_utils_dep,
  hash_dep,
  process_dep,
  iowow_dep,
  bytes_dep,
  c_vector_dep,
  parson_dep,
  djbhash_dep,
  display_utils_dep,
  menu_bar_utils_dep, iokit_utils_dep, window_utils_dep, app_utils_dep,mission_control_utils_dep,process_utils_dep,
  dock_utils_dep,
  space_utils_dep,
  string_utils_dep,
  osx_keys_dep,
  log_dep, ms_dep, timestamp_dep, c_fsio_dep,
]

core_utils_link_args = [
  '-Wl,-framework,CoreFoundation',
  '-Wl,-framework,Carbon',
  '-Wl,-framework,Foundation',
  '-Wl,-framework,AppKit'  
]

core_utils_link_args +=  ['-Wl,-framework,Foundation']
core_utils_link_args +=  ['-Wl,-framework,Security']
core_utils_link_args +=  ['-Wl,-framework,SkyLight']
core_utils_link_args +=  ['-Wl,-framework,ApplicationServices']
core_utils_link_args +=  ['-Wl,-framework,ScriptingBridge']
core_utils_link_args +=  ['-Wl,-framework,CoreFoundation']

core_utils_link_args +=  ['-Wl,-framework,SystemExtensions']
core_utils_link_args +=  ['-Wl,-framework,DeviceCheck']
core_utils_link_args +=  ['-Wl,-framework,Carbon']
core_utils_link_args +=  ['-Wl,-framework,Metal']
core_utils_link_args +=  ['-Wl,-framework,MetalKit']
core_utils_link_args +=  ['-Wl,-framework,QuartzCore']
core_utils_link_args +=  ['-Wl,-framework,ICADevices']
core_utils_link_args +=  ['-Wl,-framework,DeviceCheck']
core_utils_link_args +=  ['-Wl,-framework,AppKit']
core_utils_link_args +=  ['-Wl,-framework,IOKit']
core_utils_link_args +=  ['-F/System/Library/PrivateFrameworks']

core_utils_lib = library('core-utils',
  core_utils_srcs,
  install: false,
  dependencies: core_utils_deps,
  include_directories: core_utils_dirs,
)


core_utils_dep = declare_dependency(
  include_directories: core_utils_dirs,
  link_with: core_utils_lib,
  link_args: core_utils_link_args,
)
