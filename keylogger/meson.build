keylogger_srcs = [
  'keylogger.c',
]

keylogger_cargs = [
  ]

keylogger_inc = [
 '.',
]

keylogger_deps = [
 core_dep,
 app_utils_dep,
]


keylogger_lib = library('keylogger',
  keylogger_srcs,
  dependencies: keylogger_deps,
  c_args: keylogger_cargs,
  include_directories: keylogger_inc,
)

keylogger_dep = declare_dependency(
  include_directories: keylogger_inc,
  link_with: keylogger_lib,
)
