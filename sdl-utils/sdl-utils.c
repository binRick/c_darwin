#pragma once
#ifndef SDL_UTILS_C
#define SDL_UTILS_C
#define LOCAL_DEBUG_MODE    SDL_UTILS_DEBUG_MODE
#define WINDOW_TITLE "my title"
#define WINDOW_POSITION_X 200
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
INCBIN(qoi_file,"assets/file2.qoi");
////////////////////////////////////////////
struct su_main_t {
  SDL_Texture *tex;
  SDL_Renderer* renderer;
  SDL_Window* window;
  bool quit, resizable;
  char *qoi_buf, *qoi_path;
  size_t(^qoi_get_len)(void);
  size_t(^qoi_get_width)(void);
  size_t(^qoi_get_height)(void);
  char*(^qoi_get_file)(void);
  char*(^qoi_get_title)(void);
  QOIDecoder *qoi;
};
static struct su_main_t su_main = {
  .quit = false,
  .qoi = NULL,
  .qoi_path = NULL,
  .qoi_buf = gqoi_fileData,
  .resizable = false,
  .qoi_get_len = ^size_t(void){ return gqoi_fileSize; },
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
    fsio_write_binary_file(su_main.qoi_path,su_main.qoi_buf,su_main.qoi_get_len());
    return su_main.qoi_path; 
  },
};
static bool SDL_UTILS_DEBUG_MODE = false;
static size_t qoi_width=0, qoi_height=0;
static void __attribute__((constructor)) __constructor__sdl_utils(void);
static int sdl_utils_setup(void);
static int sdl_utils_poll(void);
////////////////////////////////////////////
int sdl_utils_main(void) {
  if(sdl_utils_setup()!=EXIT_SUCCESS)
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
      0, 
      su_main.qoi_get_width(), su_main.qoi_get_height(),
      su_main.resizable|SDL_WINDOW_ALLOW_HIGHDPI
  ))){
    SDL_Log("Unable to create window: %s", SDL_GetError());
    return 1;
  }
  if(!(su_main.renderer = SDL_CreateRenderer(su_main.window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC))){
    SDL_Log("Unable to create renderer: %s", SDL_GetError());
    return 1;
  }
  SDL_RenderSetLogicalSize(su_main.renderer, su_main.qoi_get_width(), su_main.qoi_get_height());
  if (!(su_main.tex = SDL_LoadQOI_Texture(su_main.renderer, su_main.qoi_get_file()))){
    SDL_Log("Unable to load file: %s", SDL_GetError());
    return 1;
  }
  fsio_remove(su_main.qoi_get_file());
  return(EXIT_SUCCESS);
}

static int sdl_utils_poll(){
  SDL_Event event;
  while (!su_main.quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: su_main.quit = true; break;
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
  return 0;
}
static void __attribute__((constructor)) __constructor__sdl_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_sdl_utils") != NULL) {
    log_debug("Enabling sdl-utils Debug Mode");
    SDL_UTILS_DEBUG_MODE = true;
  }
  if( (su_main.qoi = QOIDecoder_LoadFile(su_main.qoi_get_file()))){
    qoi_width=QOIDecoder_GetWidth(su_main.qoi);
    qoi_height=QOIDecoder_GetHeight(su_main.qoi);
    assert(qoi_width>0);
    assert(qoi_height>0);
  }
}

#undef LOCAL_DEBUG_MODE
#endif
