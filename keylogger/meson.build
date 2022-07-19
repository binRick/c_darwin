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
 pbpaste_dep,
 active_app_dep,
 system_utils_dep,
 subprocess_dep,
 timestamp_dep,
 list_dep,
 dbg_dep,
 ansi_codes_dep,
 ansi_utils_dep,
 keylogger_db_dep,
 c_timer_dep,
 debug_memory_dep,
 djbhash_dep,
 window_utils_dep,
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
