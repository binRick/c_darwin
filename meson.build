project('c_darwin', ['c'],default_options: ['warning_level=0','c_std=gnu99',],
  subproject_dir: 'submodules/c_deps/subprojects',
  version:'0.0.1')
fs = import('fs')
sudo_program = find_program('sudo', required : true)
git_program = find_program('git', required : true)
sh_program = find_program('sh', required : true)
bat_program = find_program('bat', required : true)
jq_program = find_program('jq', required : true)
yaml2json_program = find_program('yaml2json', required : true)
cc = meson.get_compiler('c')
pwd = meson.current_source_dir()
c_deps_dir = join_paths(meson.current_source_dir(),'submodules/c_deps')
if fs.is_dir('.git')
    git_rev_parse = run_command(git_program, 'rev-parse', '--short', '@', check: true)
    git_sha = git_rev_parse.stdout().strip()
else
    git_sha = ''
endif
version_info = configuration_data()
version_info.set('version', git_sha)
version_info.set('vcs_tag', git_sha)
subdir('meson')
inc = [include_directories(
  'submodules','submodules/c_deps',
  'submodules/c_deps/submodules/c_ansi',
  'submodules/c_deps/submodules/dbg.h',
  'submodules/c_deps/submodules/tiny-regex-c',
  'submodules/c_deps/submodules',
  'submodules/c_deps/submodules/iowow/src',
  'submodules/c_deps/submodules/iowow/src/kv',
  'submodules/c_deps/submodules/iowow/src/fs',
  'submodules/c_deps/submodules/iowow/src/log',
  'submodules/c_deps/submodules/iowow/src/json',
  'submodules/c_deps/submodules/iowow/src/platform',
  'submodules/c_deps/submodules/iowow',
  'submodules/c_deps/submodules/c_vector/src',
  'submodules/c_deps/submodules/c_vector/include',
  'app-utils',
  'system-utils',
  'submodules/c_deps/submodules/hidapi/hidapi',
  'submodules/c_deps/submodules/libusb/libusb',
  'submodules/c_deps/submodules/libusb/Xcode',
  'submodules/c_deps/submodules/libusb/libusb/os',
  'submodules/c_deps/submodules/generic-print',
  'submodules/c_deps/submodules/c_fsio',
  'submodules/c_deps/submodules/c_fsio/src',
  'submodules/c_deps/submodules/c_fsio/include',
  'submodules/c_deps/submodules/c_stringfn/include',
  'submodules/c_deps/submodules/parson',
  'submodules/c_deps/submodules/str-replace.c/src',
  'submodules/c_deps/submodules/occurrences.c',
  'window-utils',
  'active-app',
  'window-utils',
  'system-utils',
  'active-app-test',
  'window-utils-test',
  'active-app-module',
  'app-utils',
  'process',
  '.',
)]
#############################################################################
subdir('submodules')
#############################################################################
subdir('core-utils')
subdir('active-app')
subdir('app-utils')
subdir('active-app-module')
subdir('pbpaste')
subdir('pasteboard')
subdir('system-utils')
subdir('process')
subdir('kitty')
subdir('window-utils')
subdir('audit')
subdir('capture')
subdir('keylogger-db')
subdir('keylogger')
subdir('keylogger-config')
subdir('keylogger-config-test')
subdir('wrec-utils')
subdir('wrec-cli')
subdir('osx-keys')
#//////////////////////////////////////////////////////////////////////
subdir('system-utils-test')
subdir('active-app-test')
subdir('pasteboard-test')
subdir('app-utils-test')
subdir('window-utils-test')
subdir('active-app-module-test')
subdir('pbpaste-test')
subdir('process-test')
subdir('keylogger-test')
subdir('audit-test')
subdir('capture-test')
subdir('wrec-test')
subdir('kitty-test')
subdir('kitty-ls')
subdir('ls-proc')
subdir('animated-gif')
#//////////////////////////////////////////////////////////////////////
subdir('rectangle')
subdir('rectangle-test')
subdir('rectangle-cli')
subdir('mac-load-test')
#subdir('msf-gif-test')
subdir('animated-gif-test')
