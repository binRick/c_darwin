#pragma once
#ifndef TIMG_UTILS_C
#define TIMG_UTILS_C
#define SYSTEM_TIMG_BINARY_SEARCH_PATH    "/usr/local/bin"
static void __attribute__((constructor)) __constructor__timg_utils(void);
static void __attribute__((destructor)) __destructor__timg_utils(void);
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "reproc/reproc/include/reproc/export.h"
#include "reproc/reproc/include/reproc/reproc.h"
#include "sha256.c/sha256.h"
#include "submodules/b64.c/b64.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include "timg/utils/utils.h"
#include "which/src/which.h"
static struct Vector *get_default_args_v();
#define USE_EMBEDDED_TIMG_BINARY      false
#define TIMG_BINARY_MAX_RUNTIME_MS    3000
#define TIMG_BINARY_HASH_SIZE         32
#define TIMG_BINARY_PATH              "../submodules/c_deps/submodules/timg/src/timg"
#define TIMG_BINARY_SIZE              530152
#define TIMG_BINARY_HASH              "Jf+m5iKA3BaeNkJjda3Oe9bvwTNgzNMMIPypPbna+Z8="
#ifdef INCBIN_STYLE
#undef INCBIN_STYLE
#endif
#define INCBIN_STYLE    INCBIN_STYLE_SNAKE
#ifdef INCBIN_PREFIX
#undef INCBIN_PREFIX
#endif
#define INCBIN_PREFIX    timg_utils_
#include "incbin/incbin.h"
INCBIN(timg_binary, TIMG_BINARY_PATH);
INCBIN(donald_png, "assets/donald.png");
INCBIN(donald_qoi, "assets/donald.qoi");
INCBIN(donald_jpg, "assets/donald.jpg");
INCBIN(donald_gif, "assets/donald.gif");
////////////////////////////////////////////
static bool TIMG_UTILS_DEBUG_MODE = false;
static char *get_binary_path();
static char *get_binary_hash();
static const char **get_default_args();
///////////////////////////////////////////////////////////////////////
struct timg_img_t {
  const unsigned char *data;
  const char          *format;
  const size_t        size;
};
static struct timg_img_t timg_imgs[] = {
  { .data = timg_utils_donald_png_data, .format = "png", .size = 123906, },
  { .data = timg_utils_donald_gif_data, .format = "gif", .size = 42823 },
  { .data = timg_utils_donald_qoi_data, .format = "qoi", .size = 152940 },
  { .data = timg_utils_donald_jpg_data, .format = "jpg", .size = 26503 },
};
////////////////////////////////////////////
#define TITLE_FILENAME    AC_GREEN "%b" AC_RESETALL
static const char *ARG_TITLE = "--title="
                               "[" AC_RESETALL AC_YELLOW "%D" AC_RESETALL "]"
                               " "
                               "(" AC_RESETALL AC_CYAN "%wx%h" AC_RESETALL ")"
                               " "
                               TITLE_FILENAME;
static const char *ARG_TITLE_FILENAME = "--title=" TITLE_FILENAME;
static const char *ARG_SCROLL         = "--scroll=60";
static const char *ARG_FIT_WIDTH      = "--fit-width";
static const char *ARG_CENTER         = "--center";
static const char *ARG_UPSCALE        = "--upscale";
static const char *ARG_COMPRESS       = "--compress";
static const char *ARG_QUARTER_PIXELS = "-pq";
static const char *ARG_AUTO_CROP      = "--auto-crop=100";
static const char *ARG_CLEAR          = "--clear";
static const char *ARG_X              = "-g5x --upscale";
static const char *ARG_Y              = "-gx5 --upscale";
static const char *ARG_XY             = "-g5x5 --upscale";
static const char *ARG_KITTY          = "-pk";
static const char *ARG_ITERM          = "-pi";
static int run_timg_cmd(char **cmd_a);

static struct Vector *get_default_args_v(){
  struct Vector *v = vector_new();

  if (getenv("ALACRITTY_WINDOW_ID") != NULL) {
    vector_push(v, (void *)ARG_QUARTER_PIXELS);
    setenv("TIMG_USE_UPPER_BLOCK", "1", 1);
  }else if (getenv("KITTY_PID") != NULL) {
    vector_push(v, (void *)ARG_KITTY);
  }else if (getenv("ITERM_SESSION_ID") != NULL) {
    vector_push(v, (void *)ARG_ITERM);
  }else{
    vector_push(v, (void *)ARG_QUARTER_PIXELS);
  }
  return(v);
}

static const char **get_default_args(){
  return(vector_to_array(get_default_args_v()));
}

int timg_utils_test_images(){
  size_t        qty = (sizeof(timg_imgs) / sizeof(timg_imgs[0]));
  char          *imgs_args[qty + 2], *a, *p;
  struct Vector *v = vector_new(), *imgs_v = vector_new();

  imgs_args[0] = ARG_TITLE;
  asprintf(&a, "--grid=%lu", qty);
  vector_push(v, (void *)a);
  vector_push(v, (void *)ARG_CENTER);
  vector_push(v, (void *)ARG_TITLE_FILENAME);
  for (size_t i = 0; i < qty; i++) {
    asprintf(&p, "%stimg-%d-%lu.%s", gettempdir(), getpid(), i, timg_imgs[i].format);
    fsio_write_binary_file(p, timg_imgs[i].data, timg_imgs[i].size);
    imgs_args[i + 1] = p;
    vector_push(v, (void *)p);
    char *args[]  = { ARG_TITLE, ARG_CENTER, p, NULL, };
    char *args1[] = { ARG_TITLE, ARG_FIT_WIDTH, p, NULL, };
    char *args2[] = { ARG_TITLE, ARG_COMPRESS, p, NULL, };
    char *args3[] = { ARG_TITLE, ARG_QUARTER_PIXELS, p, NULL, };
    char *args4[] = { ARG_TITLE, ARG_AUTO_CROP, p, NULL, };
    char *args5[] = { ARG_TITLE, ARG_X, p, NULL, };
    char *args6[] = { ARG_TITLE, ARG_Y, p, NULL, };
    char *args8[] = { ARG_TITLE, ARG_XY, p, NULL, };
    char *args9[] = { ARG_TITLE, ARG_UPSCALE, p, NULL, };
    run_timg_cmd(args);
    run_timg_cmd(args1);
    run_timg_cmd(args2);
    run_timg_cmd(args3);
    run_timg_cmd(args4);
    run_timg_cmd(args5);
    run_timg_cmd(args6);
    run_timg_cmd(args8);
    run_timg_cmd(args9);
  }
  char **args = vector_to_array(v);

  run_timg_cmd(args);
  for (size_t i = 0; i < vector_size(imgs_v); i++) {
    fsio_remove((char *)vector_get(imgs_v, i));
  }
  return(EXIT_SUCCESS);
} /* timg_utils_test_images */

bool write_binary_path(){
  char *p = get_binary_path();

  if (fsio_file_exists(p) == false || strcmp(get_binary_hash(), TIMG_BINARY_HASH) != 0) {
    return(fsio_write_binary_file(p, timg_utils_timg_binary_data, timg_utils_timg_binary_size));
  }
  chmod(p, 0x750);
  return((strcmp(get_binary_hash(), TIMG_BINARY_HASH) == 0) ? true : false);
}

static char *get_binary_hash(){
  unsigned char buf[TIMG_BINARY_HASH_SIZE];
  char          *p = get_binary_path();

  if (fsio_file_exists(p) == false) {
    return(NULL);
  }
  unsigned char *input = fsio_read_binary_file(p);

  sha256_t      hash;

  sha256_init(&hash);
  sha256_update(&hash, (unsigned char *)input, strlen(input));
  sha256_final(&hash, buf);
  char *e = b64_encode(buf, TIMG_BINARY_HASH_SIZE);

  return(e);
}

static char *get_binary_path(){
  char *p;

  if (TIMG_UTILS_DEBUG_MODE) {
    log_debug("%d", USE_EMBEDDED_TIMG_BINARY);
  }

  if (USE_EMBEDDED_TIMG_BINARY == false) {
    char *ORIG_PATH = getenv("PATH");
    if (ORIG_PATH) {
      setenv("PATH", SYSTEM_TIMG_BINARY_SEARCH_PATH, true);
      p = which("timg");
      if (TIMG_UTILS_DEBUG_MODE) {
        log_debug("timg path: %s", p);
      }
      char                   *PATH = getenv("PATH");
      struct StringFNStrings paths = stringfn_split(PATH, ':');
      for (int i = 0; i < paths.count; i++) {
        if (TIMG_UTILS_DEBUG_MODE) {
          log_debug("%s", paths.strings[i]);
        }
      }
      stringfn_release_strings_struct(paths);
      setenv("PATH", ORIG_PATH, true);
      assert(fsio_file_exists(p) == true);
      return(p);
    }
  }

  asprintf(&p, "%s.%s-%d", gettempdir(), "timg-binary", getpid());
  return(p);
}

int timg_utils_images(struct Vector *v){
  struct Vector       *a  = vector_new();
  struct StringBuffer *sb = stringbuffer_new();

  stringbuffer_append_string(sb, "timg");
  struct Vector *defaults = get_default_args_v();

  for (size_t i = 0; i < vector_size(defaults); i++) {
    stringbuffer_append_string(sb, " ");
    stringbuffer_append_string(sb, (char *)vector_get(defaults, i));
  }

  char *args[vector_size(v) + 2];

  asprintf(&(args[0]), "--grid=%d", 3);
  vector_push(a, (void *)args[0]);
  struct StringBuffer *_s = stringbuffer_new();

  for (size_t i = 0; i < vector_size(v); i++) {
    char *img = (char *)vector_get(v, i);
    stringbuffer_clear(_s);
    stringbuffer_append_string(_s, " \"");
    stringbuffer_append_string(_s, img);
    stringbuffer_append_string(_s, "\"");
    vector_push(a, (void *)stringbuffer_to_string(_s));
    log_debug("adding img %s", img);
  }
  stringbuffer_release(_s);
  for (size_t i = 0; i < vector_size(a); i++) {
    log_debug("Arg #%lu> %s", i, (char *)vector_get(a, i));
    stringbuffer_append_string(sb, " ");
    stringbuffer_append_string(sb, (char *)vector_get(a, i));
  }
  char *cmd = stringbuffer_to_string(sb);

  stringbuffer_release(sb);
  log_debug("cmd:     \n" AC_YELLOW "%s" AC_RESETALL "\n", cmd);
  args[vector_size(v) + 2] = NULL;

  return(run_timg_cmd(args));

  return(0);
} /* timg_utils_images */

int timg_utils_titled_image(char *file){
  char *args[] = { ARG_TITLE, file, NULL, };

  return(run_timg_cmd(args));
}

int timg_utils_image(char *file){
  char *args[] = { file, NULL, };

  return(run_timg_cmd(args));
}

int timg_utils_run_help(){
  char *args[] = { "--help", NULL, };

  run_timg_cmd(args);
  char *args1[] = { "--version", NULL, };

  return(run_timg_cmd(args1));
}

int run_timg_cmd(char *args[]){
  char     *output_e = NULL, *output_o = NULL;
  reproc_t *process = NULL;
  int      r        = REPROC_ENOMEM;

  errno   = 0;
  process = reproc_new();
  if (process == NULL) {
    log_error("process init failed");
    goto finish;
  }
  struct Vector *cmd_v = vector_new();

  vector_push(cmd_v, (void *)get_binary_path());
  if (TIMG_UTILS_DEBUG_MODE) {
    log_debug("%s", get_binary_path());
  }
  char   **a = get_default_args();
  size_t i   = 0;

  while (a != NULL && a[i]) {
    if (TIMG_UTILS_DEBUG_MODE) {
      log_debug("%s", a[i]);
    }
    vector_push(cmd_v, (void *)a[i]);
    i++;
  }

  i = 0;
  while (args != NULL && args[i] != NULL) {
    if (TIMG_UTILS_DEBUG_MODE) {
      log_debug("%s", args[i]);
    }
    vector_push(cmd_v, (void *)args[i]);
    i++;
  }
  char **cmd_a = (char **)vector_to_array(cmd_v);

  errno = 0;
  r     = reproc_start(process, cmd_a,
                       (reproc_options){
    .redirect.parent = true,
    .deadline        = TIMG_BINARY_MAX_RUNTIME_MS,
  });
  if (r < 0) {
    log_error("result invalid");
    goto finish;
  }
  errno = 0;
  r     = reproc_close(process, REPROC_STREAM_IN);
  if (r < 0) {
    log_error("process stdin close failed");
    goto finish;
  }

  errno = 0;
  r     = reproc_wait(process, REPROC_INFINITE);
  if (r < 0) {
    log_error("process wait");
    goto finish;
  }
  if (TIMG_UTILS_DEBUG_MODE) {
    log_info("process waited");
  }

finish:
  errno = 0;
  reproc_destroy(process);
  if (r < 0) {
    switch (r) {
    case -32:
      if (TIMG_UTILS_DEBUG_MODE) {
        log_info(AC_YELLOW "<%d> [%d] Broken Pipe while Destroying Process (%s)" AC_RESETALL, getpid(), r, reproc_strerror(r));
      }
      break;
    default:
      log_error(AC_RED "destroy: %d> %s" AC_RESETALL, r, reproc_strerror(r));
      break;
    }
  }

  if (output_o) {
    log_error("OUT>%s", output_o);
  }
  if (output_e) {
    log_error("ERR>%s", output_e);
  }

  errno = 0;
  return(EXIT_SUCCESS);
} /* run_timg_cmd */
static void __attribute__((destructor)) __destructor__timg_utils(void){
  if (USE_EMBEDDED_TIMG_BINARY == true) {
    if (fsio_file_exists(get_binary_path()) == true) {
      fsio_remove(get_binary_path());
    }
  }
}

static void __attribute__((constructor)) __constructor__timg_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_timg_utils") != NULL) {
    log_debug("Enabling timg-utils Debug Mode");
    TIMG_UTILS_DEBUG_MODE = true;
  }
  assert(get_binary_path());
  if (USE_EMBEDDED_TIMG_BINARY == true) {
    assert(write_binary_path() == true);
  }
  if (TIMG_UTILS_DEBUG_MODE) {
    log_debug("%s|%s:%s|%s:%s", get_binary_path(), get_binary_hash(), TIMG_BINARY_HASH, bytes_to_string(fsio_file_size(get_binary_path())), bytes_to_string(TIMG_BINARY_SIZE));
  }
  if (USE_EMBEDDED_TIMG_BINARY == true) {
    assert(fsio_file_exists(get_binary_path()) == true);
    assert(strcmp(get_binary_hash(), TIMG_BINARY_HASH) == 0);
    assert(fsio_file_size(get_binary_path()) == TIMG_BINARY_SIZE);
    if (getenv("ALACRITTY_WINDOW_ID") != NULL) {
      setenv("TIMG_USE_UPPER_BLOCK", "1", 1);
    }
  }
}
#undef INCBIN_PREFIX
#undef INCBIN_STYLE
#undef TIMG_BINARY_HASH
#endif
