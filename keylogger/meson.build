keylogger_srcs = [
  'keylogger.c',
]
keylogger_cargs = [
  '-Wno-unused-command-line-argument',
  ]
keylogger_inc = [
 inc,
 '.',
]
keylogger_deps = [
 c_string_buffer_dep,
 active_app_dep,
 system_utils_dep,process_utils_dep,string_utils_dep,
 core_utils_dep,
 subprocess_dep,
 timestamp_dep,
 dbg_dep,
 ansi_codes_dep,
 ansi_utils_dep,
 c_timer_dep,log_dep,
 djbhash_dep,
 window_utils_dep,
]


keylogger_lib = static_library('keylogger',
  keylogger_srcs,
  install: false,
  dependencies: keylogger_deps,
  c_args: keylogger_cargs,
  include_directories: [
    keylogger_inc,
  ],
)

keylogger_dep = declare_dependency(
  include_directories: keylogger_inc,
  link_with: keylogger_lib,
)
