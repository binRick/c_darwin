subdir('debug')
subdir('utils')
subdir('image')

core_srcs = [
  'core.c',
]

inc = get_variable('inc', join_paths(meson.current_source_dir(),'..'))

core_include_dirs = [
  '.',
  inc,
]

core_deps = [
  csplit_dep,is_number_dep,roscha_dep,
  optparse99_dep,metalang99_dep,interface99_dep,reproc_dep,
  libspinner_dep,ck_dep,
  core_utils_dep,core_image_dep,core_debug_dep,incbin_dep,
  leptonica_dep,
  match_dep,
  async_dep,
  ansi_codes_dep,
  c_vector_dep,
  c_fsio_dep,
  c_stringfn_dep,
  c_img_dep,ansi_utils_dep,
  c_string_buffer_dep,optparse99_dep,string_utils_dep,reproc_dep,
  sha256_c_dep,qoir_dep,
  timestamp_dep,
  log_dep,chan_dep,clamp_dep,glib_dep,c_workqueue_dep,
  ms_dep,qoi_dep,
  spng_dep,msf_gif_dep,libforks_dep,
  chan_utils_dep,chan_dep,
  djbhash_dep,
  subprocess_dep,c_forever_dep,tinydir_dep,tiny_regex_c_dep,smaz_dep,c_timer_dep,uuid4_dep,
  str_replace_dep,module_dep,querystring_c_dep,
  occurrences_dep,
  which_dep,parson_dep,
  iowow_dep,murmurhash_dep,c_eventemitter_dep,libcyaml_dep,
  msgpack_c_dep,base64_simple_dep,hexify_c_dep,msgbox_dep,libspinner_dep,
  sqldbal_dep,str_flatten_c_dep,stb_dep,
  imagequant_dep,lodepng_dep,vips_dep,
  timelib_dep,date_c_dep,hash_dep,fort_dep,whereami_dep,
  socket99_dep,pngquant_dep,glib_dep,
  wildcardcmp_dep,
  bytes_dep,tempdir_dep,fs_c_dep,b64_dep,fancy_progress_dep,
  url_h_dep,vips_dep,hashmap_h_dep,bitfield_dep,levenshtein_dep,
  container_of_dep,
  bestline_dep,c_ringbuf_dep,
]

core_c_args = [
  '-DVN_UI_IMPLEMENTATION',
  '-DVN_COLOR',
  '-DVN_WIDGET',
]

core_link_args = [
]

core_lib = static_library('core',
  core_srcs,
  install: false,
  dependencies: core_deps,
  c_args: core_c_args,
  include_directories: core_include_dirs
)

core_dep = declare_dependency(
  include_directories: core_include_dirs,
  link_with: core_lib,
  link_args: core_link_args,
)
