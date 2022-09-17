keylogger_srcs = [
  'keylogger.c',
]

keylogger_cargs = [
  ]

keylogger_inc = [
 '.',
]

keylogger_deps = [
 c_string_buffer_dep,
 string_utils_dep,
 core_utils_dep,
 timestamp_dep,
 ansi_codes_dep,
 log_dep,
]


keylogger_lib = static_library('keylogger',
  keylogger_srcs,
  dependencies: keylogger_deps,
  c_args: keylogger_cargs,
  include_directories: keylogger_inc,
)

keylogger_dep = declare_dependency(
  include_directories: keylogger_inc,
  link_with: keylogger_lib,
)
