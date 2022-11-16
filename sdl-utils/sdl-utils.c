#pragma once
#ifndef SDL_UTILS_C
#define SDL_UTILS_C
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define SU_BYTEORDER            0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF
#else
#define SU_BYTEORDER            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
#endif
#define QOIR_IMPLEMENTATION
#define LOCAL_DEBUG_MODE        SDL_UTILS_DEBUG_MODE
#define SU_CG_TO_VIPS_FORMAT    IMAGE_TYPE_GIF
#define WINDOW_TITLE            "my title"
//#include "qoir/src/qoir.h"
#define WINDOW_POSITION_X       200
#define WINDOW_POSITION_Y       100
#define SDL_ERROR(MSG, CODE)              \
  SDL_Log("%s: %s", MSG, SDL_GetError()); \
  return(CODE);

#include "capture/type/type.h"
#include "capture/utils/utils.h"
#include "core/core.h"
#include "core/core.h"
#include "display/utils/utils.h"
#include "frameworks/frameworks.h"
#include "image/utils/utils.h"
#include "match/match.h"
#include "module/def.h"
#include "module/module.h"
#include "module/require.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "vips/vips.h"
#include "window/utils/utils.h"
#include <Carbon/Carbon.h>
#include <pthread.h>
#include <SDL2/SDL.h>
////////////////////////////////////////////
#include "qoi/qoi.h"
////////////////////////////////////////////
#include "qoi_ci/transpiled/QOI.h"
////////////////////////////////////////////
#include "qoi_ci/QOI-stdio.h"
////////////////////////////////////////////
#include "submodules/SDL_QOI/SDL_QOI.h"
////////////////////////////////////////////
#include "sdl-utils/sdl-utils.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "clamp/clamp.h"
#include "incbin/incbin.h"
#include "log/log.h"
#include "ms/ms.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"

#define INC(NAME, PATH)                                      \
  INCBIN(NAME, PATH);                                        \
  size_t NAME ## _length(void){ return(g ## NAME ## Size); } \
  size_t NAME ## _data(void){ return(g ## NAME ## Data); }
#define END_INCS

INCBIN(qoi_file1, "assets/file1.qoi");
INCBIN(qoi_file2, "assets/file2.qoi");
INCBIN(qoi_file3, "assets/file3.qoi");
INCBIN(qoi_file4, "assets/file4.qoi");
INCBIN(qoi_file5, "assets/file5.qoi");
INCBIN(qoi_file6, "assets/file6.qoi");
static unsigned char *bufs[] = {
  gqoi_file1Data,
  gqoi_file2Data,
  gqoi_file3Data,
  gqoi_file4Data,
  gqoi_file5Data,
  gqoi_file6Data,
};
////////////////////////////////////////////
typedef VipsImage *(^su_image_source_provider)(void *dat);
enum su_image_source_type_t {
  SU_SOURCE_EMBEDDED,
  SU_SOURCE_WINDOW_ID,
  SU_SOURCE_SPACE,
  SU_SOURCE_DISPLAY,
  SU_SOURCE_BUFFER,
  SU_SOURCE_PATH,
  SU_SOURCE_URL,
  SU_SOURCE_CALLBACK,
  SU_SOURCES_QTY,
};
struct su_rgb_t {
  unsigned char *buf;
  size_t        len;
  int           width, height, bytes_per_row,
                depth, //32
                pitch; //stride
};

void xxxxxxx(){
  size_t             len;
  void               *ptr;
  qoir_decode_result decode;

  decode = qoir_decode(ptr, len, &(qoir_decode_options){ .pixfmt = QOIR_PIXEL_FORMAT__BGRA_PREMUL });
}

void su_window_from_rgb(struct su_rgb_t *rgb, SDL_Window *window);

void su_window_from_qoir(qoir_decode_result *qoir, SDL_Window *window){
  return(su_window_from_rgb(&((struct su_rgb_t){
    .buf = qoir->dst_pixbuf.data,
    .width = qoir->dst_pixbuf.pixcfg.width_in_pixels,
    .height = qoir->dst_pixbuf.pixcfg.height_in_pixels,
    .depth = 32,
    .pitch = qoir->dst_pixbuf.stride_in_bytes,
  }), window));
}

void su_window_from_file(char *path, SDL_Window *window){
  size_t    len;
  void      *ptr;
  size_t    n;
  SDL_RWops *rw = SDL_RWFromFile(path, "rb");

  len = rw->size(rw);
  ptr = malloc(len);
  n   = rw->read(rw, ptr, 1, len);
  rw->close(rw);
}

void su_window_from_rgb(struct su_rgb_t *rgb, SDL_Window *window){
  SDL_Surface *ws = SDL_GetWindowSurface(window);

  SDL_FillRect(ws, NULL, SDL_MapRGB(ws->format, 0x00, 0x00, 0x00));
//  SDL_BlitSurface(surface, NULL, ws, NULL);
  SDL_UpdateWindowSurface(window);
  return(SDL_CreateRGBSurfaceFrom(rgb->buf, rgb->width, rgb->height, rgb->depth, rgb->pitch, SU_BYTEORDER));
}
su_image_source_provider su_image_source_providers[] = {
  [SU_SOURCE_CALLBACK] = ^ VipsImage *(void *dat) { VipsImage *V;
                                                    su_image_source_provider cb = (su_image_source_provider)dat;
                                                    if (!(V = cb(dat)))
                                                      goto FAILED_FILE_TO_VIPS;

                                                    FAILED_FILE_TO_VIPS :
                                                    return(NULL); },
  [SU_SOURCE_PATH] = ^ VipsImage *(void *dat)     { VipsImage *V;
                                                    char *p, *image_loader_name;

                                                    p = (char *)dat;
                                                    if (!(image_loader_name = vips_foreign_find_load(p)))
                                                      goto FAILED_MISSING_LOADER;
                                                    if (!(V = vips_image_new_from_file(p, "", NULL)))
                                                      goto FAILED_FILE_TO_VIPS;

                                                    FAILED_MISSING_LOADER :
                                                    FAILED_FILE_NOT_PRESENT :
                                                    FAILED_FILE_TO_VIPS :
                                                    return(NULL); },
  [SU_SOURCE_EMBEDDED] = ^ VipsImage *(void *dat) {
    size_t i;
    VipsImage *V;

    i = (size_t)dat;

    if (!bufs[i])
      goto FAILED_BUFFER_INDEX_NOT_PRESENT;
//    if(!(image_loader_name = vips_foreign_find_load_buffer(bufs[i],buf_lens[i])))
//      goto FAILED_MISSING_LOADER;
//    if(!(i=vips_image_new_from_buffer(buf,buf_len,"",NULL)))
//      goto FAILED_BUFFER_TO_VIPS;
    return(V);

    FAILED_BUFFER_INDEX_NOT_PRESENT :
    return(NULL);
  },
  [SU_SOURCE_WINDOW_ID] = ^ VipsImage *(void *dat){
    CGImageRef cg;
    VipsImage *i;
    unsigned long durs[10];
    size_t id, width, height, buf_len;
    unsigned char *buf;
    char *image_loader_name = NULL;

    id = (size_t)dat;

    errno = 0;
    if (!id)
      goto FAILED_INVALID_ID;
    if (!(cg = capture_type_width(CAPTURE_TYPE_WINDOW, id, width)))
      goto FAILED_CAPTURE;
    if (!(buf = save_cgref_to_image_type_memory(SU_CG_TO_VIPS_FORMAT, cg, &buf_len)) || !buf_len || !buf)
      goto FAILED_CG_TO_BUFFER;
    if (!(image_loader_name = vips_foreign_find_load_buffer(buf, buf_len)))
      goto FAILED_MISSING_LOADER;
    if (!(i = vips_image_new_from_buffer(buf, buf_len, "", NULL)))
      goto FAILED_BUFFER_TO_VIPS;
    return(i);

    FAILED_CAPTURE :
    log_error("INVALID ID> Failed to acquire Window");
    FAILED_CG_TO_BUFFER :
    log_error("INVALID ID> Failed to acquire Window");
    FAILED_BUFFER_TO_VIPS :
    log_error("INVALID ID> Failed to acquire Window");
    FAILED_INVALID_ID :
    log_error("INVALID ID> Failed to acquire Window");
    FAILED_MISSING_LOADER :
    log_error("LOADER> Failed to acquire Window");

    return(NULL);
  },
};
typedef size_t (^qty_fxn)(void);
typedef size_t (^tp_bufs_qty_fxn)(void);
typedef struct Vector *(^tp_bufs_get_fxn)(void);
typedef bool (^tp_bufs_set_fxn)(struct Vector *v);
typedef unsigned char *(^tp_buf_get_fxn)(int id, size_t *len);
typedef size_t (^tp_buf_size_fxn)(int id);
typedef unsigned char *(^tp_buf_from_file)(const char *path, size_t *buf_len);
typedef unsigned char *(^tp_buf_from_buffer)(const unsigned char *buffer, size_t len, size_t *buf_len);
typedef bool (^tp_from_file)(int id, const char *path, size_t len);
typedef bool (^tp_buf_exists_fxn)(int id);
typedef bool (^tp_buf_set_fxn)(int id, unsigned char *buf, size_t len);
#define TP_REN        su_main.sdl.renderer
#define TP_WIN        su_main.sdl.window
#define TP_TEX        su_main.sdl.tex
#define TP_VEC        su_main.bufs.bufs_v
#define TP_INDEX      (su_main.buf_index)
#define TP_VEC_QTY    vector_size(TP_VEC)
#define bufs_qty()        TP_VEC_QTY
#define TP_MUTEX      &(__tp_mutex__)
#define TP_LOCKED(FXN)    { do { pthread_mutex_lock(TP_MUTEX); { FXN }; pthread_mutex_unlock(TP_MUTEX); } while (0); }
static pthread_mutex_t __tp_mutex__;
struct qoi_vec_t { size_t len; unsigned char *pixels; };
struct su_main_t {
  QOIDecoder *qoi;
  struct {
    int space, display, window;
  }          indexes;
  struct {
    struct Vector *spaces, *displays, *windows;
  }          vecs;
  struct {
    struct Vector   *bufs_v;
    tp_bufs_get_fxn get;
    tp_bufs_set_fxn set;
    tp_bufs_qty_fxn qty;
  } bufs;
  struct {
    tp_buf_set_fxn    set;
    tp_buf_get_fxn    get;
    tp_buf_size_fxn   size;
    tp_buf_exists_fxn exits;
  } buf;
  struct {
    SDL_Texture  *tex;
    SDL_Renderer *renderer;
    SDL_Window   *window;
  } sdl;
  bool
    quit, resizable;
  void
    (^qoi_vec_load_index)(size_t index);
  char
    *qoi_path,
  *(^qoi_get_file)(void),
  *(^qoi_get_window_id_path)(int id, int width),
  *(^qoi_get_display_index_path)(int id, int width),
  *(^qoi_get_space_index_path)(int id, int width),
  *(^qoi_get_title)(void),
  *(^qoi_get_vec_index_path)(size_t index);
  unsigned char
  (^qoi_get_buffer_colorspace)(unsigned char *buf, size_t len),
  (^qoi_get_path_colorspace)(char *path);
  int
    buf_index, window_options,
  (^qoi_get_len)(void),
  (^qoi_get_path_width)(char *path),
  (^qoi_get_path_height)(char *path),
  (^qoi_get_buffer_height)(unsigned char *buf, size_t len),
  (^qoi_get_buffer_channels)(unsigned char *buf, size_t len),
  (^qoi_get_path_channels)(char *path),
  (^qoi_get_buffer_width)(unsigned char *buf, size_t len);
  size_t
  (^qoi_get_width)(void),
  (^qoi_get_height)(void),
  (^qoi_get_vec_index_height)(size_t index),
  (^qoi_get_vec_index_width)(size_t index),
  (^qoi_get_vec_index_len)(size_t index),
  (^qoi_get_path_len)(char *path),
  (^qoi_get_buffer_len)(unsigned char *buf, size_t len);
  qoi_desc
  (^qoi_get_buffer_desc)(unsigned char *buf, size_t len),
  (^qoi_get_path_desc)(char *path);
  unsigned char
  *(^qoi_get_window_id_pixels)(int id, int width, size_t *len),
  *(^qoi_get_display_index_pixels)(int index, int width, size_t *len),
  *(^qoi_get_space_index_pixels)(int index, int width, size_t *len),
  *(^qoi_get_vec_index_pixels)(size_t index, size_t *len),
  *(^qoi_get_buf)(void);
};
static struct su_main_t su_main = {
  .buf_index = 1,
  .bufs      = {
    .qty                        = ^ size_t (void){ return((size_t)(vector_size(TP_VEC))); },
  },
  .buf            = {
  },
  .window_options = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
                    | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
  ,
//      | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP
  .quit        = false,
  .qoi         = NULL,
  .qoi_path    = NULL,
  .resizable   = false,
  .qoi_get_len = ^ int (void){
    int buf_lens[]              = {
      gqoi_file1Size,
      gqoi_file2Size,
      gqoi_file3Size,
      gqoi_file4Size,
      gqoi_file5Size,
      gqoi_file6Size,
    };
    return(buf_lens[clamp(su_main.buf_index, 0, bufs_qty() - 1)]);
  },
  .qoi_get_buf                  = ^ unsigned char *(void){
    return(bufs[clamp(su_main.buf_index, 0, bufs_qty() - 1)]);
  },
  .qoi_get_vec_index_path       = ^ char *(size_t index){
    char *p;
    asprintf(&p, "%s.sdl-utils-%d-%lu.qoi", gettempdir(), getpid(), index);
    size_t len                  = 0;
    fsio_write_binary_file(p, su_main.qoi_get_vec_index_pixels(index, &len), su_main.qoi_get_vec_index_len(index));
    return(p);
  },
  .qoi_vec_load_index           = ^ void (size_t index){
    size_t h                    = 0,                                         w= 0, len = 0; char *path = NULL;
    h    = su_main.qoi_get_vec_index_height(index);
    w    = su_main.qoi_get_vec_index_width(index);
    path = su_main.qoi_get_vec_index_path(index);
    len  = su_main.qoi_get_vec_index_len(index);
    log_info("%lu> h:%lu,w:%lu,len:%lu,path:%s", index, h, w, len, path);
  },
  .qoi_get_width                = ^ size_t (void){
    if (!su_main.qoi)
      su_main.qoi               = QOIDecoder_LoadFile(su_main.qoi_get_file());
    return(QOIDecoder_GetWidth(su_main.qoi));
  },
  .qoi_get_vec_index_len        = ^ size_t (size_t index){
    return(0);
    //QOIEncoder_GetEncodedSize(QOIDecoder_LoadFile(su_main.qoi_get_vec_index_path(index)));
  },
  .qoi_get_vec_index_pixels     = ^ unsigned char *(size_t index,            size_t *len){
    return(QOIDecoder_GetPixels(QOIDecoder_LoadFile(su_main.qoi_get_vec_index_path(index))));
  },
  .qoi_get_vec_index_height     = ^ size_t (size_t index){
    return(QOIDecoder_GetHeight(QOIDecoder_LoadFile(su_main.qoi_get_vec_index_path(index))));
  },
  .qoi_get_vec_index_width      = ^ size_t (size_t index){
    return(QOIDecoder_GetWidth(QOIDecoder_LoadFile(su_main.qoi_get_vec_index_path(index))));
  },
  .qoi_get_height               = ^ size_t (void){
    if (!su_main.qoi)
      su_main.qoi               = QOIDecoder_LoadFile(su_main.qoi_get_file());
    return(QOIDecoder_GetHeight(su_main.qoi));
  },
  .qoi_get_title                = ^ char *(void){
    return("x");

    char *s;
    asprintf(&s,
             "%s | %s | %s Channel | %s Colorspace | %dx%d |"
             "%s",
             "x", "x",
//        basename(su_main.qoi_get_file()),
//        bytes_to_string(su_main.qoi_get_len()),
             QOIDecoder_HasAlpha(su_main.qoi) ? "Alpha" : "No Alpha",
             QOIDecoder_IsLinearColorspace(su_main.qoi) ? "Linear" : "Nonlinear",
             QOIDecoder_GetWidth(su_main.qoi),
             QOIDecoder_GetHeight(su_main.qoi),
             ""
             );
    return(s);
  },
  .qoi_get_file                 = ^ char *(void){
    return(su_main.qoi_get_window_id_path(261, 450));
  },
  .qoi_get_window_id_pixels     = ^ unsigned char *(int id,                  int width, size_t *len){
    unsigned char *pixels       = NULL;
    CGImageRef img;
    if ((img                    = capture_type_width(CAPTURE_TYPE_WINDOW,    id, width)))
      if ((pixels               = save_cgref_to_qoi_memory(img,              len)) && *len)
        (pixels                 = save_cgref_to_qoi_memory(img,              len));
    if (img)
      CGImageRelease(img);
    return(pixels);
  },
  .qoi_get_space_index_pixels   = ^ unsigned char *(int index,               int width, size_t *len){
    unsigned char *pixels       = NULL;
    CGImageRef img;
    struct Vector *ids_v = get_space_ids_v();
    int id               = (int)(size_t)vector_get(ids_v,             index);
    Dbg(id, %d);
    if (id)
      if ((img                  = capture_type_width(CAPTURE_TYPE_SPACE,     id, width)))
        (pixels                 = save_cgref_to_qoi_memory(img,              len));
    if (img)
      CGImageRelease(img);
    return(pixels);
  },
  .qoi_get_display_index_pixels = ^ unsigned char *(int index,               int width, size_t *len){
    unsigned char *pixels       = NULL;
    CGImageRef img;
    int id                      = get_display_index_id(index);
    Dbg(id, %d);
    if (id)
      if ((img                  = capture_type_width(CAPTURE_TYPE_DISPLAY,   id, width)))
        (pixels                 = save_cgref_to_qoi_memory(img,              len));
    if (img)
      CGImageRelease(img);
    return(pixels);
  },
  .qoi_get_buffer_width         = ^ int (unsigned char *buf,                 size_t len){
    return(su_main.qoi_get_buffer_desc(buf, len).width);
  },
  .qoi_get_buffer_channels      = ^ int (unsigned char *buf,                 size_t len){
    return((int)(su_main.qoi_get_buffer_desc(buf, len).channels));
  },
  .qoi_get_buffer_colorspace    = ^ unsigned char (unsigned char *buf,       size_t len){
    return(su_main.qoi_get_buffer_desc(buf, len).colorspace);
  },
  .qoi_get_buffer_height        = ^ int (unsigned char *buf,                 size_t len){
    return(su_main.qoi_get_buffer_desc(buf, len).height);
  },
  .qoi_get_path_colorspace      = ^ unsigned char (char *path){
    return(su_main.qoi_get_buffer_channels(
             fsio_read_binary_file(path), fsio_file_size(path)));
  },
  .qoi_get_path_channels        = ^ int (char *path){
    return(su_main.qoi_get_buffer_channels(
             fsio_read_binary_file(path), fsio_file_size(path)));
  },
  .qoi_get_path_height          = ^ int (char *path){
    return(su_main.qoi_get_buffer_height(
             fsio_read_binary_file(path), fsio_file_size(path)));
  },
  .qoi_get_path_width           = ^ int (char *path){
    return(su_main.qoi_get_buffer_width(
             fsio_read_binary_file(path), fsio_file_size(path)));
  },
  .qoi_get_path_desc            = ^ qoi_desc (char *path){
    return(su_main.qoi_get_buffer_desc(
             fsio_read_binary_file(path), fsio_file_size(path)));
  },
  .qoi_get_buffer_desc          = ^ qoi_desc (unsigned char *buf,            size_t len){
    qoi_desc desc;
    unsigned char *pixels       = qoi_decode(buf,                            len, &desc, 4);
    if (!pixels) return((qoi_desc){ 0 });

    free(pixels);
    return(desc);
  },
  .qoi_get_space_index_path     = ^ char *(int index,                        int width){
    char *p               = NULL;
    size_t len            = 0;
    unsigned char *pixels = NULL;
    if ((pixels                 = su_main.qoi_get_space_index_pixels(index,  width, &len)) && len) {
      Dbg(len, %u);
      asprintf(&p, "%s%d-space-%d-%dx-%lld.qoi", gettempdir(), getpid(), index, width, timestamp());
      qoi_desc desc             = su_main.qoi_get_path_desc(p);
      Dbg(desc.height, %d);
      fsio_write_binary_file(p, pixels, len);
      free(pixels);
      if (fsio_file_exists(p))
        return(p);
    }
    return(NULL);
  },
  .qoi_get_display_index_path   = ^ char *(int index,                        int width){
    char *p               = NULL;
    size_t len            = 0;
    unsigned char *pixels = NULL;
    if ((pixels                 = su_main.qoi_get_display_index_pixels(index,width, &len)) && len) {
      Dbg(len, %u);
      asprintf(&p, "%s%d-display-%d-%dx-%lld.qoi", gettempdir(), getpid(), index, width, timestamp());
      qoi_desc desc             = su_main.qoi_get_path_desc(p);
      Dbg(desc.height, %d);
      fsio_write_binary_file(p, pixels, len);
      free(pixels);
      if (fsio_file_exists(p))
        return(p);
    }
    return(NULL);
  },
  .qoi_get_window_id_path       = ^ char *(int id,                           int width){
    char *p               = NULL;
    size_t len            = 0;
    unsigned char *pixels = NULL;
    if ((pixels                 = su_main.qoi_get_window_id_pixels(id,       width, &len)) && len) {
      asprintf(&p, "%s%d-window-%d-%dx-%lld.qoi", gettempdir(), getpid(), id, width, timestamp());
      fsio_write_binary_file(p, pixels, len);
      free(pixels);
      if (fsio_file_exists(p))
        return(p);
    }
    return(NULL);
  },
};
static int sdl_utils_load_qoi(void);
static bool             SDL_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__sdl_utils(void);
static int sdl_utils_setup(void);
static int sdl_utils_poll(void);
static void su_qoir();

////////////////////////////////////////////
static void su_qoir(){
  qoir_decode_options opts    = { 0 };
  unsigned char       *pixels = NULL;
  size_t              len     = 0;
  /*
     pixels  = save_cgref_to_qoi_memory(capture_type_capture(CAPTURE_TYPE_WINDOW,get_first_window_id_by_name("richard")), &len);
     qoir_decode_result res = qoir_decode(pixels, len, &opts);
     Dbg(res.status_message,%s);
   */
}

int sdl_utils_main(void) {
  if (sdl_utils_setup() != EXIT_SUCCESS)
    goto err;
  if (sdl_utils_load_qoi() != EXIT_SUCCESS)
    goto err;
  if (sdl_utils_poll() != EXIT_SUCCESS)
    goto err;
err:
  if (su_main.quit)
    return(EXIT_SUCCESS);

  log_error("Failed");
  return(EXIT_FAILURE);
}

static int sdl_utils_setup(void){
  if (SDL_Init(SDL_INIT_VIDEO) != EXIT_SUCCESS) {
    SDL_Log("Unable to initialise SDL2: %s", SDL_GetError());
    return(1);
  }
  if (!(TP_WIN = SDL_CreateWindow(
          su_main.qoi_get_title(),
          WINDOW_POSITION_X,
          WINDOW_POSITION_Y,
          200, 400,
          su_main.window_options
          ))) {
    SDL_Log("Unable to create window: %s", SDL_GetError());
    return(1);
  }

  if (!(TP_REN = SDL_CreateRenderer(TP_WIN, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))) {
    SDL_Log("Unable to create renderer: %s", SDL_GetError());
    return(1);
  }
  return(EXIT_SUCCESS);
}

static bool dec_index(){
  TP_LOCKED({
    int i             = su_main.buf_index;
    su_main.buf_index = clamp(su_main.buf_index - 1, 0, bufs_qty() - 1);
//  if(i==su_main.buf_index&&i==0)
//    su_main.buf_index=bufs_qty()-1;
    if (i != su_main.buf_index)
      sdl_utils_load_qoi();
  });
}

static bool inc_index(){
  TP_LOCKED({
    int i             = su_main.buf_index;
    su_main.buf_index = clamp(su_main.buf_index + 1, 0, bufs_qty() - 1);
//  if(i==su_main.buf_index&&i==bufs_qty()-1)
//    su_main.buf_index=0;
    if (i != su_main.buf_index)
      sdl_utils_load_qoi();
  });
}

static int sdl_utils_load_qoi(){
  TP_LOCKED({
    su_main.qoi      = NULL;
    su_main.qoi_path = NULL;
    SDL_SetWindowResizable(TP_WIN, SDL_TRUE);
    SDL_RenderSetLogicalSize(TP_REN, su_main.qoi_get_width(), su_main.qoi_get_height());
    SDL_SetWindowSize(TP_WIN, su_main.qoi_get_width(), su_main.qoi_get_height());
    if (!(TP_TEX = SDL_LoadQOI_Texture(TP_REN, su_main.qoi_get_file()))) {
      SDL_Log("Unable to load file: %s", SDL_GetError());
      return(1);
    }
    fsio_remove(su_main.qoi_get_file());
  });
  return(EXIT_SUCCESS);
}

static int sdl_utils_poll(){
  SDL_Event event;

  errno = 0;
  int      window_count;
  uint32_t *window_list;

  while (!su_main.quit) {
    errno = 0;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
        case SDL_WINDOWEVENT_ENTER: Dbg("enter", %s); break;
        case SDL_WINDOWEVENT_LEAVE: Dbg("leave", %s); break;
        case SDL_WINDOWEVENT_MOVED: Dbg("moved", %s); break;
        case SDL_WINDOWEVENT_FOCUS_GAINED: Dbg("got focus", %s); break;
        case SDL_WINDOWEVENT_FOCUS_LOST: Dbg("lost focus", %s); break;
        case SDL_WINDOWEVENT_SIZE_CHANGED: Dbg("size changed", %s); break;
        case SDL_WINDOWEVENT_RESIZED:
          Dbg("resized", %s);
          break;
        }
        break;
      case SDL_MOUSEWHEEL:
        Dbg("mouse wheel", %s);
        break;
      case SDL_MOUSEBUTTONDOWN:
        Dbg("mouse button down", %s);
        break;
      case SDL_MOUSEMOTION:
        Dbg(event.motion.x, %d);
        Dbg(event.motion.y, %d);
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_END:
          Dbg("end", %s);
          break;
        case SDLK_PAGEDOWN:
          Dbg("pagedown", %s);
          break;
        case SDLK_PAGEUP:
          Dbg("pageup", %s);
          break;
        case SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL):
          Dbg("left ctrl", %s);
          break;
        case '?':
          Dbg("help", %s);
          break;
        case SDLK_CAPSLOCK:
          Dbg("caps lock", %s);
          break;
        case SDLK_e:
          su_qoir();
          break;
        case SDLK_i:
          if (!(TP_TEX = SDL_LoadQOI_Texture(TP_REN, su_main.qoi_get_space_index_path(0, 3000)))) {
            SDL_Log("Unable to load file: %s", SDL_GetError());
            return(1);
          }
          SDL_SetWindowSize(TP_WIN, 1500, 1500);
          Dbg("u", %s);
          break;
#define SU_LOAD_QOI_BUFFER(BUFFER)    { do{                                                      \
                                          if (!(TP_TEX = SDL_LoadQOI_Texture(TP_REN, BUFFER))) { \
                                            SDL_Log("Unable to load file: %s", SDL_GetError());  \
                                            exit(EXIT_FAILURE);                                  \
                                          }                                                      \
                                        }                                                        \
                                        while (0);                                               \
}
#define SU_PRINT_INDEX(TYPE)          { do { Dbg(su_main.indexes.TYPE, %d); }while (0); }
#define SU_INCREMENT_INDEX(TYPE)      { do {                                                                           \
                                          int           i  = su_main.indexes.TYPE;                                     \
                                          struct Vector *v = su_main.vecs.TYPE ## s;                                   \
                                          i                    = clamp(i + 1, 0, vector_size(su_main.vecs.TYPE ## s)); \
                                          i                    = (i >= vector_size(su_main.vecs.TYPE ## s)) ? 0 : i;   \
                                          su_main.indexes.TYPE = i;                                                    \
                                          SU_PRINT_INDEX(TYPE);                                                        \
                                        } while (0); }
        case SDLK_p: SU_INCREMENT_INDEX(space); break;
        case SDLK_l: SU_INCREMENT_INDEX(display); break;
        case SDLK_m: SU_INCREMENT_INDEX(window); break;
        case SDLK_j:
          SU_PRINT_INDEX(space);
          SU_PRINT_INDEX(display);
          SU_PRINT_INDEX(window);
          break;
        case SDLK_u:
          SU_LOAD_QOI_BUFFER(su_main.qoi_get_space_index_path(clamp(su_main.indexes.space, 0, vector_size(su_main.vecs.spaces) - 1), 2000));
          SDL_SetWindowSize(TP_WIN, 2000, 700);
          log_info("space %d", su_main.indexes.space);
          break;
        case SDLK_y: {
          char *p = su_main.qoi_get_space_index_path(su_main.indexes.space, 1500);
          if (!(TP_TEX = SDL_LoadQOI_Texture(TP_REN, p))) {
            SDL_Log("Unable to load file: %s", SDL_GetError());
            return(1);
          }
          SDL_SetWindowSize(TP_WIN, 1500, 800);
          //su_main.qoi_get_path_width(p));
          Dbg("t", %s);
        }
        break;
        case SDLK_t:
          if (!(TP_TEX = SDL_LoadQOI_Texture(TP_REN, su_main.qoi_get_display_index_path(su_main.indexes.display, 1500)))) {
            SDL_Log("Unable to load file: %s", SDL_GetError());
            return(1);
          }
          SDL_SetWindowSize(TP_WIN, 1500, 1000);
          Dbg("t", %s);
          break;
        case SDLK_s:
          if (!(TP_TEX = SDL_LoadQOI_Texture(TP_REN, su_main.qoi_get_window_id_path(261, 500)))) {
            SDL_Log("Unable to load file: %s", SDL_GetError());
            return(1);
          }
          SDL_SetWindowSize(TP_WIN, 1450, 1000);
          Dbg("s", %s);
          break;
        case SDLK_x:
          if (!(TP_TEX = SDL_LoadQOI_Texture(TP_REN, su_main.qoi_get_window_id_path(261, 450)))) {
            SDL_Log("Unable to load file: %s", SDL_GetError());
            return(1);
          }
          SDL_SetWindowSize(TP_WIN, 450, 1000);
          Dbg("x", %s);
          break;
        case SDLK_z: {
          size_t               len = 0; unsigned char *pixels = NULL;
          struct window_info_t *w;
          if (false) {
            struct Vector *v = get_window_infos_v();
            for (size_t i = 0; i < vector_size(v); i++) {
              unsigned long ts = timestamp();
              len = 0; pixels = NULL;
              w   = (struct window_info_t *)vector_get(v, i);
              Dbg(w->window_id, %u);
              Dbg(w->name, %s);
              if (false) {
                CGImageRef img = capture_type_capture(CAPTURE_TYPE_WINDOW, ((struct window_info_t *)vector_get(v, i))->window_id);
                if (img) {
                  pixels = save_cgref_to_qoi_memory(img, &len);
                  Dbg(milliseconds_to_string(timestamp() - ts), %s);
                  Dbg(len, %u);
                }
              }
            }
          }
        }
        break;
        case SDLK_d: {
          if (!(TP_TEX = SDL_LoadQOI_Texture(TP_REN, su_main.qoi_get_file()))) {
            SDL_Log("Unable to load file: %s", SDL_GetError());
            return(1);
          }
          SDL_SetWindowSize(TP_WIN, su_main.qoi_get_width(), su_main.qoi_get_height());
        }
        break;
        case SDLK_w:
          if (!(window_list = front_process_window_list_for_active_space(&window_count)))
            log_error("stream> %s: could not get windows of front process! abort..", __FUNCTION__);
          Dbg(window_count, %d);
          size_t qoi_len            = 0;
          unsigned char *qoi_pixels = NULL;
          for (size_t i = 0; i < window_count; i++)
            Dbg(window_list[i], %d);
          struct Vector *ids_v = get_window_ids();
          for (size_t i = 0; i < window_count; i++)
            if ((qoi_pixels = save_cgref_to_qoi_memory(capture_type_capture(CAPTURE_TYPE_WINDOW, window_list[i]), &qoi_len)))
              Dbg(qoi_len, %u);
          unsigned char ws[10] = {
            //save_cgref_to_qoi_memory(capture_type_capture(CAPTURE_TYPE_WINDOW,get_first_window_id_by_name("richard")), &qoi_len) },
          };

          Dbg(vector_size(get_window_ids()), %u);
          Dbg(get_first_window_id_by_name("richard"), %u);
          Dbg(get_first_window_id_by_name("richard"), %u);
          Dbg(get_first_window_id_by_name("main"), %u);
          Dbg(get_first_window_id_by_name("Google Chrome"), %u);

          Dbg("w", %s);
          break;
        case SDLK_RETURN:
          Dbg("return", %s);
          break;
        case SDLK_HOME:
          Dbg("home", %s);
          su_main.buf_index = 0;
          break;
        case SDLK_DOWN:
          Dbg("down", %s);
          dec_index();
          break;
        case SDLK_UP:
          Dbg("up", %s);
          inc_index();
          break;
        case SDLK_LEFT:
          Dbg("left", %s);
          dec_index();
          break;
        case SDLK_RIGHT:
          Dbg("right", %s);
          inc_index();
          break;
        case SDLK_q:
          su_main.quit = true;
          break;
        default:
          break;
        } /* switch */
        break;
      case SDL_QUIT:
        su_main.quit = true;
        break;
      } /* switch */
    }
    SDL_RenderClear(TP_REN);
    SDL_RenderCopy(TP_REN, TP_TEX, NULL, NULL);
    SDL_RenderPresent(TP_REN);
  }

  SDL_DestroyTexture(TP_TEX);
  SDL_DestroyRenderer(TP_REN);
  SDL_DestroyWindow(TP_WIN);
  SDL_Quit();
  errno = 0;
  return(0);
} /* sdl_utils_poll */
static void __attribute__((constructor)) __constructor__sdl_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_sdl_utils") != NULL) {
    log_debug("Enabling sdl-utils Debug Mode");
    SDL_UTILS_DEBUG_MODE = true;
  }
  TP_LOCKED({
    su_main.vecs.spaces   = get_space_ids_v();
    su_main.vecs.displays = get_display_ids_v();
    su_main.vecs.windows  = get_window_ids();
  });
  /*
     TP_LOCKED({
     su_main.bufs.bufs_v = vector_new();
     su_main.qoi = QOIDecoder_LoadFile(su_main.qoi_get_file());
     });
   */
}

int su_vi(VipsImage *vi){
  log_info("su vi....");
  if (sdl_utils_setup() != EXIT_SUCCESS)
    goto err;
  SDL_SetWindowResizable(TP_WIN, SDL_TRUE);
  SDL_RenderSetLogicalSize(TP_REN, 
      vips_image_get_width(vi),
      vips_image_get_height(vi)
      );
  unsigned char *buf;
  size_t buf_len;
  char *tf;
  asprintf(&tf,"/tmp/a.qoi");
  if(vips_image_write_to_file(vi,tf,NULL)){
    log_error("Failed to convert vips image to qoir");
    return(1);
  }
  Dbg(fsio_file_size(tf),%lu);
  SDL_RenderSetLogicalSize(TP_REN, vips_image_get_width(vi), vips_image_get_height(vi));
  SDL_SetWindowSize(TP_WIN, vips_image_get_width(vi), vips_image_get_height(vi));
  if (!(TP_TEX = SDL_LoadQOI_Texture(TP_REN, tf))) {
      SDL_Log("Unable to load file: %s", SDL_GetError());
      return(1);
    }
  log_info("loading qoir.....");
  int shouldQuit = 0;
  SDL_Event event;
  while (!shouldQuit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: shouldQuit = 1; break;
      case SDLK_q: shouldQuit = 1; break;
      }
    }
    SDL_RenderClear(TP_REN);
    SDL_RenderCopy(TP_REN, TP_TEX, NULL, NULL);
    SDL_RenderPresent(TP_REN);
  }



  SDL_DestroyTexture(TP_TEX);
  SDL_DestroyRenderer(TP_REN);
  SDL_DestroyWindow(TP_WIN);
  SDL_Quit();

  log_info("polling.....");
  return(EXIT_SUCCESS);

err:
  if (su_main.quit)
    return(EXIT_SUCCESS);
}
SDL_Surface *vips_to_surface(VipsImage *vi){
  unsigned char *buf;
  size_t len,stride;
  if(vips_image_write_to_buffer(vi,".qoi",&buf,&len,NULL)){
    log_error("Failed to convert vips image to qoir");
    return(NULL);
  }
  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
      buf,
      vips_image_get_width(vi),
      vips_image_get_height(vi),
      32,
      stride,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF
#else
      0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
#endif
  );
  free(buf);
  return(surface);
}
#undef LOCAL_DEBUG_MODE
#endif
