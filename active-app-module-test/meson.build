active_app_module_test_srcs = [
  'active-app-module-test.c',
]
active_app_module_include_dirs = [
  inc,
]
active_app_module_test_deps = [
  c_greatest_dep,
  logh_dep,
  c_stringfn_dep,
  active_app_module_dep,
]

if get_option('enable-binaries')
  active_app_module_test_exec = executable('active-app-module-test',
     [
      active_app_module_test_srcs,
     ],
     dependencies: active_app_module_test_deps,
     include_directories: [
       active_app_module_include_dirs,
       inc,
     ],
  )
  if get_option('enable-binary-tests')
    test('active-app-module-test', active_app_module_test_exec, args: [])
  endif
endif
