#pragma once
#ifndef SDL_UTILS_C
#define SDL_UTILS_C
#define LOCAL_DEBUG_MODE    SDL_UTILS_DEBUG_MODE
#define WINDOW_TITLE "my title"
#define WINDOW_POSITION_X 200
#define WINDOW_POSITION_Y 100
#include <SDL2/SDL.h>
#include "clamp/clamp.h"
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
#include "tempdir.c/tempdir.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "incbin/incbin.h"
INCBIN(qoi_file1,"assets/file1.qoi");
INCBIN(qoi_file2,"assets/file2.qoi");
INCBIN(qoi_file3,"assets/file3.qoi");
INCBIN(qoi_file4,"assets/file4.qoi");
INCBIN(qoi_file5,"assets/file5.qoi");
INCBIN(qoi_file6,"assets/file6.qoi");
////////////////////////////////////////////
static unsigned char *bufs[] = {
  gqoi_file1Data,
  gqoi_file2Data,
  gqoi_file3Data,
  gqoi_file4Data,
  gqoi_file5Data,
  gqoi_file6Data,
};
static int bufs_qty = (sizeof(bufs)/sizeof(bufs[0]));
struct su_main_t {
  SDL_Texture *tex;
  SDL_Renderer* renderer;
  SDL_Window* window;
  bool quit, resizable;
  int buf_index;
  char *qoi_path;
  int(^qoi_get_len)(void);
  unsigned char *(^qoi_get_buf)(void);
  size_t(^qoi_get_width)(void);
  size_t(^qoi_get_height)(void);
  char*(^qoi_get_file)(void);
  char*(^qoi_get_title)(void);
  QOIDecoder *qoi;
};
static struct su_main_t su_main = {
  .buf_index = 1,
  .quit = false,
  .qoi = NULL,
  .qoi_path = NULL,
  .resizable = false,
  .qoi_get_len = ^int(void){ 
    int buf_lens[] = {
      gqoi_file1Size,
      gqoi_file2Size,
      gqoi_file3Size,
      gqoi_file4Size,
      gqoi_file5Size,
      gqoi_file6Size,
    };
    return(buf_lens[clamp(su_main.buf_index,0,bufs_qty-1)]);
  },
  .qoi_get_buf = ^unsigned char *(void){ 
    return bufs[clamp(su_main.buf_index,0,bufs_qty-1)];
  },
  .qoi_get_width = ^size_t(void){ 
    if(!su_main.qoi)
      su_main.qoi = QOIDecoder_LoadFile(su_main.qoi_get_file());
    return QOIDecoder_GetWidth(su_main.qoi);
  },
  .qoi_get_height = ^size_t(void){ 
    if(!su_main.qoi)
      su_main.qoi = QOIDecoder_LoadFile(su_main.qoi_get_file());
    return QOIDecoder_GetHeight(su_main.qoi);
  },
  .qoi_get_title = ^char*(void){
    char *s;
    asprintf(&s,
        "%s | %s | %s Channel | %s Colorspace | %dx%d |"
        "%s", 
        basename(su_main.qoi_get_file()), 
        bytes_to_string(su_main.qoi_get_len()),
        QOIDecoder_HasAlpha(su_main.qoi) ? "Alpha" : "No Alpha",
        QOIDecoder_IsLinearColorspace(su_main.qoi) ? "Linear" : "Nonlinear",
        QOIDecoder_GetWidth(su_main.qoi),
        QOIDecoder_GetHeight(su_main.qoi),
        ""
        );
    return(s);
  },
  .qoi_get_file = ^char*(void){
    if(su_main.qoi_path && fsio_file_exists(su_main.qoi_path) && fsio_file_size(su_main.qoi_path)==su_main.qoi_get_len())
      return(su_main.qoi_path);
    asprintf(&su_main.qoi_path,"%s%d-%lld.qoi",gettempdir(),getpid(),timestamp());
    fsio_write_binary_file(su_main.qoi_path,su_main.qoi_get_buf(),su_main.qoi_get_len());
    return su_main.qoi_path; 
  },
};
static int sdl_utils_load_qoi(void);
static bool SDL_UTILS_DEBUG_MODE = false;
static void __attribute__((constructor)) __constructor__sdl_utils(void);
static int sdl_utils_setup(void);
static int sdl_utils_poll(void);
////////////////////////////////////////////
int sdl_utils_main(void) {
  if(sdl_utils_setup()!=EXIT_SUCCESS)
    goto err;
  if(sdl_utils_load_qoi()!=EXIT_SUCCESS)
    goto err;
  if(sdl_utils_poll()!=EXIT_SUCCESS)
    goto err;
err:
  log_error("Failed");
  return(EXIT_FAILURE);
}
#define SDL_ERROR(MSG,CODE)\
  SDL_Log("%s: %s", MSG, SDL_GetError());\
  return(CODE);
static int sdl_utils_setup(void){
  if (SDL_Init(SDL_INIT_VIDEO) != EXIT_SUCCESS) {
    SDL_Log("Unable to initialise SDL2: %s", SDL_GetError());
    return 1;
  }
  if(!(su_main.window = SDL_CreateWindow(
      su_main.qoi_get_title(),
      WINDOW_POSITION_X, 
      WINDOW_POSITION_Y, 
      su_main.qoi_get_width(), su_main.qoi_get_height(),
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL 
      | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
      | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP
  ))){
    SDL_Log("Unable to create window: %s", SDL_GetError());
    return 1;
  }
  if(!(su_main.renderer = SDL_CreateRenderer(su_main.window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC))){
    SDL_Log("Unable to create renderer: %s", SDL_GetError());
    return 1;
  }
  return(EXIT_SUCCESS);
}

static bool dec_index(){
  int i = su_main.buf_index;
  su_main.buf_index = clamp(su_main.buf_index-1,0,bufs_qty-1);
  if(i!=su_main.buf_index)
    sdl_utils_load_qoi();
}
static bool inc_index(){
  int i = su_main.buf_index;
  su_main.buf_index = clamp(su_main.buf_index+1,0,bufs_qty-1);
  if(i!=su_main.buf_index)
    sdl_utils_load_qoi();
}

static int sdl_utils_load_qoi(){
  su_main.qoi = NULL;
  su_main.qoi_path = NULL;
  SDL_SetWindowResizable(su_main.window,SDL_TRUE);
  SDL_RenderSetLogicalSize(su_main.renderer, su_main.qoi_get_width(), su_main.qoi_get_height());
  SDL_SetWindowSize(su_main.window,su_main.qoi_get_width(), su_main.qoi_get_height());
  if (!(su_main.tex = SDL_LoadQOI_Texture(su_main.renderer, su_main.qoi_get_file()))){
    SDL_Log("Unable to load file: %s", SDL_GetError());
    return 1;
  }
  fsio_remove(su_main.qoi_get_file());
  return(EXIT_SUCCESS);
}

static int sdl_utils_poll(){
  SDL_Event event;
  errno=0;
  while (!su_main.quit) {
    errno=0;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_WINDOWEVENT:
        switch(event.window.event){
        case SDL_WINDOWEVENT_ENTER:Dbg("enter",%s);break;
        case SDL_WINDOWEVENT_LEAVE:Dbg("leave",%s);break;
        case SDL_WINDOWEVENT_MOVED:Dbg("moved",%s);break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:Dbg("got focus",%s);break;
        case SDL_WINDOWEVENT_FOCUS_LOST:Dbg("lost focus",%s);break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:Dbg("size changed",%s);break;
        case SDL_WINDOWEVENT_RESIZED:
          Dbg("resized",%s);
          break;
        }
      break;
      case SDL_MOUSEWHEEL:
          Dbg("mouse wheel",%s);
          break;
      case SDL_MOUSEBUTTONDOWN:
          Dbg("mouse button down",%s);
          break;
      case SDL_MOUSEMOTION:
        Dbg(event.motion.x,%d);
        Dbg(event.motion.y,%d);
      break;        
      case SDL_KEYDOWN:
      switch(event.key.keysym.sym){
       case SDLK_END:
        Dbg("end",%s);
       break;
       case SDLK_PAGEDOWN:
        Dbg("pagedown",%s);
       break;
       case SDLK_PAGEUP:
        Dbg("pageup",%s);
       break;
       case SDLK_RETURN:
        Dbg("return",%s);
       break;
       case SDLK_HOME:
        Dbg("home",%s);
        su_main.buf_index = 0;
       break;
       case SDLK_DOWN:
        Dbg("down",%s);
        dec_index();
       break;
       case SDLK_UP:
        Dbg("up",%s);
        inc_index();
       break;
       case SDLK_LEFT:
        Dbg("left",%s);
        dec_index();
       break;
       case SDLK_RIGHT:
        Dbg("right",%s);
        inc_index();
       break;
       case SDLK_q:
        su_main.quit = true; 
       break;
       case SDLK_s:
        Dbg("s",%s);
       break;
       default:
       break;
      }
      break;
      case SDL_QUIT: 
        su_main.quit = true; 
      break;
      }
    }
    SDL_RenderClear(su_main.renderer);
    SDL_RenderCopy(su_main.renderer, su_main.tex, NULL, NULL);
    SDL_RenderPresent(su_main.renderer);
  }

  SDL_DestroyTexture(su_main.tex);
  SDL_DestroyRenderer(su_main.renderer);
  SDL_DestroyWindow(su_main.window);
  SDL_Quit();
  errno=0;
  return 0;
}
static void __attribute__((constructor)) __constructor__sdl_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_sdl_utils") != NULL) {
    log_debug("Enabling sdl-utils Debug Mode");
    SDL_UTILS_DEBUG_MODE = true;
  }
  su_main.qoi = QOIDecoder_LoadFile(su_main.qoi_get_file());
}

#undef LOCAL_DEBUG_MODE
#endif
