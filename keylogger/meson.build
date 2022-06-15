keylogger_srcs = [
  'keylogger.c',
]
keylogger_inc = [
 inc,
 '.',
 '../keylogger',
]
keylogger_deps = [
 c_string_buffer_dep,
 subprocess_dep,
 timestamp_dep,
 list_dep,
]


keylogger_lib = library('keylogger',
  keylogger_srcs,
  install: false,
  dependencies: keylogger_deps,
  c_args: osx_ldflags,
  include_directories: [
    keylogger_inc,
  ],
)

keylogger_dep = declare_dependency(
  include_directories: keylogger_inc,
  link_with: keylogger_lib,
)
