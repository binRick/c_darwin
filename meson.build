project('c_darwin', ['c'],default_options: ['warning_level=0','c_std=gnu99',])
cc = meson.get_compiler('c')
pwd = meson.current_source_dir()
meson_deps_dir = join_paths(meson.current_source_dir(),'submodules/meson_deps')
inc = [include_directories(
  'submodules','submodules/meson_deps','submodules/meson_deps/submodules',
  'submodules/meson_deps/submodules/c_ansi',
  'app-utils',
  'window-utils',
  '.',
)]
#############################################################################
subdir('submodules')
#############################################################################
subdir('active-app')
subdir('app-utils')
subdir('window-utils')
subdir('active-app-module')
#//////////////////////////////////////////////////////////////////////
subdir('app-utils-test')
subdir('window-utils-test')
#//////////////////////////////////////////////////////////////////////
subdir('system-utils')
subdir('system-utils-test')
#//////////////////////////////////////////////////////////////////////
subdir('active-app-test')
#subdir('active-app-module-test')
#//////////////////////////////////////////////////////////////////////
subdir('pbpaste')
subdir('pbpaste-test')
#//////////////////////////////////////////////////////////////////////
subdir('keylogger')
subdir('keylogger-test')
#//////////////////////////////////////////////////////////////////////
subdir('process-environment')
subdir('process-environment-test')
