#assets_hash = run_command('./.assets/hash.sh', check:true).stdout().strip()

httpserver_utils_srcs = [
  'httpserver-utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

httpserver_utils_include_dirs = [
  '.',
  inc,
]

httpserver_utils_deps = [
  core_dep,
  vips_dep,
  wslay_dep,
  httpserver_h_dep,
  capture_utils_dep,ms_dep,config_utils_dep,
  querystring_c_dep,hotkey_utils_dep,image_utils_dep,
  tesseract_dep,space_utils_dep,display_utils_dep,screen_utils_dep,
]

httpserver_utils_c_args = [
  #  '-DASSETS_HASH="'+assets_hash+'"',
  '-DINCBIN_EXTERN_SILENCE_BITCODE_WARNING',
]

httpserver_utils_link_args = [
]

httpserver_utils_lib = library('httpserver-utils',
  httpserver_utils_srcs,
  install: false,
  dependencies: httpserver_utils_deps,
  c_args: httpserver_utils_c_args,
  include_directories: httpserver_utils_include_dirs
)

httpserver_utils_dep = declare_dependency(
  include_directories: httpserver_utils_include_dirs,
  link_with: httpserver_utils_lib,
  link_args: httpserver_utils_link_args,
)
