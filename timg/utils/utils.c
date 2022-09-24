#pragma once
#ifndef TIMG_UTILS_C
#define TIMG_UTILS_C
////////////////////////////////////////////
#include "timg/utils/utils.h"
////////////////////////////////////////////
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
#include "which/src/which.h"
#define USE_EMBEDDED_TIMG_BINARY      false
#define TIMG_BINARY_MAX_RUNTIME_MS    15000
#define TIMG_BINARY_HASH_SIZE         32
#define TIMG_BINARY_PATH              "../submodules/c_deps/submodules/timg/src/timg"
#define TIMG_BINARY_SIZE              530152
#define TIMG_BINARY_HASH              "Jf+m5iKA3BaeNkJjda3Oe9bvwTNgzNMMIPypPbna+Z8="
#ifndef INCBIN_SILENCE_BITCODE_WARNING
#define INCBIN_SILENCE_BITCODE_WARNING
#endif
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
static bool redirect_parent_io    = true;
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

static const char **get_default_args(){
  struct Vector *v = vector_new();
  char          *o;

  //asprintf(&o,"-o%stimg-utils-output-%d.dat",gettempdir(),getpid());
  vector_push(v, (void *)o);

  if (getenv("ALACRITTY_WINDOW_ID") != NULL) {
    vector_push(v, (void *)ARG_QUARTER_PIXELS);
    setenv("TIMG_USE_UPPER_BLOCK", "1", 1);
  }else if (getenv("ITERM_SESSION_ID") != NULL) {
    vector_push(v, (void *)ARG_ITERM);
  }else if (getenv("KITTY_PID") != NULL) {
    vector_push(v, (void *)ARG_KITTY);
  }else{
    vector_push(v, (void *)ARG_QUARTER_PIXELS);
  }
  return(vector_to_array(v));
}

int timg_utils_test_images(){
  char          *p;
  size_t        qty = (sizeof(timg_imgs) / sizeof(timg_imgs[0]));
  char          *imgs_args[qty + 2];
  struct Vector *v      = vector_new();
  struct Vector *imgs_v = vector_new();

  imgs_args[0] = ARG_TITLE;
  char *a;

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
    p = which("timg");
    assert(fsio_file_exists(p) == true);
    return(p);
  }

  asprintf(&p, "%s.%s-%d", gettempdir(), "timg-binary", getpid());
  return(p);
}

int timg_utils_image(char *file){
  if (TIMG_UTILS_DEBUG_MODE) {
    log_info("%s", file);
  }

  char *args[] = { ARG_TITLE, file, NULL, };

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
    .redirect.parent = redirect_parent_io,
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

  size_t size_e = 0, size_o = 0;

  if (redirect_parent_io == false) {
    for ( ;;) {
      uint8_t buffer_o[4096];
      errno = 0;
      r     = reproc_read(process, REPROC_STREAM_OUT, buffer_o, sizeof(buffer_o));
      if (r < 0) {
        log_error("read error");
        goto finish;
        log_error("read error");
        break;
      }
      size_t bytes_read_o = (size_t)r;
      errno = 0;
      char   *result_o = realloc(output_o, size_o + bytes_read_o + 1);
      if (result_o == NULL) {
        log_error("memory error");
        r = REPROC_ENOMEM;
        log_error("memory error");
        goto finish;
      }
      output_o = result_o;
      memcpy(output_o + size_o, buffer_o, bytes_read_o);
      output_o[size_o + bytes_read_o] = '\0';
      size_o                         += bytes_read_o;
    }

    for ( ;;) {
      uint8_t buffer_e[4096];
      errno = 0;
      size_t  bytes_read_e = 0;
      r = reproc_read(process, REPROC_STREAM_ERR, buffer_e, sizeof(buffer_e));
      if (r < 0) {
        log_error("stderr read error, (read %s)", bytes_to_string(size_e));
        goto finish;
        break;
      }
      bytes_read_e = (size_t)r;
      errno        = 0;
      log_debug("%lu", bytes_read_e);
      if (bytes_read_e > 0) {
        char *result_e = realloc(output_e, size_e + bytes_read_e + 1);
        if (result_e == NULL) {
          r = REPROC_ENOMEM;
          log_error("memory error (wanted %s more bytes on top of %s)", bytes_to_string(bytes_read_e), bytes_to_string(size_e));
          goto finish;
        }
        output_e = result_e;
        memcpy(output_e + size_e, buffer_e, bytes_read_e);
        output_e[size_e + bytes_read_e] = '\0';
        size_e                         += bytes_read_e;
      }
    }
  }
  errno = 0;
  r     = reproc_wait(process, REPROC_INFINITE);
  if (r < 0) {
    log_error("process wait");
    goto finish;
  }
  log_info("process waited");

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
#undef INCBIN_PREFIX
#undef INCBIN_STYLE
#undef TIMG_BINARY_HASH
#endif
