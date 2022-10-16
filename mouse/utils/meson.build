mouse_utils_srcs = [
  'utils.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

mouse_utils_include_dirs = [
  '.',
  inc,
]

mouse_utils_deps = [
  core_dep,
]

mouse_utils_c_args = [
]

mouse_utils_link_flags = [
'-Wl,-framework,CoreFoundation',
'-Wl,-framework,Carbon',
]
mouse_utils_link_flags +=  ['-Wl,-framework,SkyLight']
mouse_utils_link_flags +=  ['-F/System/Library/PrivateFrameworks']


mouse_utils_lib = library('mouse-utils',
  mouse_utils_srcs,
  install: false,
  dependencies: mouse_utils_deps,
  c_args: mouse_utils_c_args,
  include_directories: mouse_utils_include_dirs
)

mouse_utils_dep = declare_dependency(
  include_directories: mouse_utils_include_dirs,
  link_with: mouse_utils_lib,
  link_args: mouse_utils_link_flags,

)
subdir('test')
