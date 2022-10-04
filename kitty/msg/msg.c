#pragma once
#ifndef KITTY_MSG_C
#define KITTY_MSG_C
#include "kitty/msg/msg.h"
//#include "qoi_ci/QOI-stdio.h"
#include "qoi/qoi.h"
#include "qoi_ci/transpiled/QOI.h"
#include "vips/vips.h"
#define KITTY_MSG_FILE_PREFIX                              "tty-graphics-protocol-kitty-msg-vips-image"
#define KITTY_SHM_SHM_ENCODED_PNG_IMAGE_PATH_DIMENSIONS    "\x1b_Ga=T,f=100,s=%d,v=%d,t=t;%s\x1b\\"
#define KITTY_DELETE_IMAGES                                "\x1b_Ga=d\x1b\\"
#define KITTY_DELETE_IMAGE_ID                              "\x1b_Ga=d=Z,i=%lu\x1b\\"
#define LOCAL_DEBUG_MODE                                   KITTY_MSG_DEBUG_MODE
#define KITTYQUERY                                         "\x1b_Gi=1,a=q;\x1b\\"
// request kitty keyboard protocol features 1, 2, and 8, first pushing current.
// see https://sw.kovidgoyal.net/kitty/keyboard-protocol/#progressive-enhancement
#define KKBDSUPPORT                                        "\x1b[=11u"
// the kitty keyboard protocol allows unambiguous, complete identification of
// input events. this queries for the level of support. we want to do this
// because the "keyboard pop" control code is mishandled by kitty < 0.20.0.
#define KKBDQUERY    "\x1b[?u"
////////////////////////////////////////////
static bool KITTY_MSG_DEBUG_MODE = false;
static char *kitty_msg_get_vips_image_msg(VipsImage *image);
static bool kitty_msg_resize_image(VipsImage **image, enum kitty_msg_resize_type_t resize_type, size_t resize_value);
static VipsImage *image_path_to_vips_image(char *image_path);
static VipsImage *image_buffer_to_vips_image(unsigned char *buf, size_t len);
static char *save_restore_msg(char *msg, int row, int col);
static bool kitty_write_msg(char *msg);

  static int kitty_fprintf(FILE *fd, char *fmt, char *msg, ...){
  va_list vargs;
  va_start(vargs, msg);
  va_end(vargs);
    int len = fprintf(fd, fmt, msg, vargs);
    fflush(fd);
    return(len);
  }

static bool kitty_write_msg(char *msg){
  return(kitty_fprintf(stdout,"%s\n",msg) > 0 ? true : false);
}
static void kitty_set_position(int x, int y){
  printf("\x1B[%d;%dH", y, x);
  fflush(stdout);
}
struct pos { int x, y; };

static struct pos kitty_get_position(){
  struct pos p;

  // line l = kitty_send_term("\x1B[6n");
  // int r = sscanf(l.buf+1, "[%d;%dR", &p.y, &p.x);
  return(p);
}

static struct winsize *kitty_get_terminal_size(void){
  struct winsize *sz = calloc(1, sizeof(struct winsize));
  ioctl(STDOUT_FILENO, TIOCGWINSZ, sz);
  return(sz);
}

static void kitty_hide_cursor(){
  puts("\x1B[?25l");
}

static void kitty_show_cursor(){
  puts("\x1B[?25h");
}

///////////////////////////////////////////////////////////////////////
bool kitty_write_terminal_corner(int corner, char *msg){
        unsigned long ts = timestamp();
        struct winsize *ws = get_terminal_size();
        Dbg(ws->ws_ypixel,%d);
        Dbg(ws->ws_xpixel,%d);
        Dbg(ws->ws_col,%d);
        Dbg(ws->ws_row,%d);
        Dbg(milliseconds_to_string(timestamp()-ts),%s);
}

bool kitty_display_image_path(char *image_path){
  return(kitty_write_msg(kitty_msg_display_image_path(image_path)));
}

bool kitty_display_image_path_resized_height(char *image_path, size_t height){
  return(kitty_write_msg(kitty_msg_display_image_path_resized_height(image_path, height)));
}

bool kitty_display_image_path_resized_width(char *image_path, size_t width){
  return(kitty_write_msg(kitty_msg_display_image_path_resized_width(image_path, width)));
}

bool kitty_display_image_buffer(unsigned char *buf, size_t len){
  return(kitty_write_msg(kitty_msg_display_image_buffer(buf, len)));
}

bool kitty_display_image_buffer_resized_width(unsigned char *buf, size_t len, size_t width){
  return(kitty_write_msg(kitty_msg_display_image_buffer_resized_width(buf, len, width)));
}

bool kitty_display_image_buffer_resized_height(unsigned char *buf, size_t len, size_t height){
  return(kitty_write_msg(kitty_msg_display_image_buffer_resized_height(buf, len, height)));
}

bool kitty_display_image_path_at_row_col(char *image_path, int row, int col){
  return(kitty_write_msg(kitty_msg_display_image_path_at_row_col(image_path, row, col)));
}

bool kitty_display_image_path_resized_height_at_row_col(char *image_path, size_t height, int row, int col){
  return(kitty_write_msg(kitty_msg_display_image_path_resized_height_at_row_col(image_path, height, row, col)));
}

bool kitty_display_image_path_resized_width_at_row_col(char *image_path, size_t width, int row, int col){
  return(kitty_write_msg(kitty_msg_display_image_path_resized_width_at_row_col(image_path, width, row, col)));
}

bool kitty_display_image_buffer_at_row_col(unsigned char *buf, size_t len, int row, int col){
  return(kitty_write_msg(kitty_msg_display_image_buffer_at_row_col(buf, len, row, col)));
}

bool kitty_display_image_buffer_resized_width_at_row_col(unsigned char *buf, size_t len, size_t width, int row, int col){
  return(kitty_write_msg(kitty_msg_display_image_buffer_resized_width_at_row_col(buf, len, width, row, col)));
}

bool kitty_display_image_buffer_resized_height_at_row_col(unsigned char *buf, size_t len, size_t height, int row, int col){
  return(kitty_write_msg(kitty_msg_display_image_buffer_resized_height_at_row_col(buf, len, height, row, col)));
}

char *kitty_msg_display_image_path_resized_height_at_row_col(char *image_path, size_t height, int row, int col){
  VipsImage *image = image_path_to_vips_image(image_path);

  kitty_msg_resize_image(&image, KITTY_MSG_RESIZE_TYPE_HEIGHT, height);
  return(save_restore_msg(kitty_msg_get_vips_image_msg(image), row, col));
}

char *kitty_msg_display_image_path_resized_width_at_row_col(char *image_path, size_t width, int row, int col){
  VipsImage *image = image_path_to_vips_image(image_path);

  kitty_msg_resize_image(&image, KITTY_MSG_RESIZE_TYPE_WIDTH, width);
  return(save_restore_msg(kitty_msg_get_vips_image_msg(image), row, col));
}

char *kitty_msg_display_image_buffer_at_row_col(unsigned char *buf, size_t len, int row, int col){
  VipsImage *image = image_buffer_to_vips_image(buf, len);

  return(save_restore_msg(kitty_msg_get_vips_image_msg(image), row, col));
}

char *kitty_msg_display_image_buffer_resized_width_at_row_col(unsigned char *buf, size_t len, size_t width, int row, int col){
  VipsImage *image = image_buffer_to_vips_image(buf, len);

  kitty_msg_resize_image(&image, KITTY_MSG_RESIZE_TYPE_WIDTH, width);
  return(save_restore_msg(kitty_msg_get_vips_image_msg(image), row, col));
}

char *kitty_msg_display_image_buffer_resized_height_at_row_col(unsigned char *buf, size_t len, size_t height, int row, int col){
  VipsImage *image = image_buffer_to_vips_image(buf, len);

  kitty_msg_resize_image(&image, KITTY_MSG_RESIZE_TYPE_HEIGHT, height);
  return(save_restore_msg(kitty_msg_get_vips_image_msg(image), row, col));
}

char *kitty_msg_display_image_path_at_row_col(char *image_path, int row, int col){
  VipsImage *image = image_path_to_vips_image(image_path);

  return(save_restore_msg(kitty_msg_get_vips_image_msg(image), row, col));
}

char *kitty_msg_delete_images(void){
  return(KITTY_DELETE_IMAGES);
}

char *kitty_msg_delete_image_id(size_t id){
  char *msg;

  asprintf(&msg, KITTY_DELETE_IMAGE_ID, id);
  return(msg);
}

char *kitty_msg_display_image_buffer_resized_width(unsigned char *buf, size_t len, size_t width){
  VipsImage *image = image_buffer_to_vips_image(buf, len);

  kitty_msg_resize_image(&image, KITTY_MSG_RESIZE_TYPE_WIDTH, width);
  return(kitty_msg_get_vips_image_msg(image));
}

char *kitty_msg_display_image_buffer_resized_height(unsigned char *buf, size_t len, size_t height){
  VipsImage *image = image_buffer_to_vips_image(buf, len);

  kitty_msg_resize_image(&image, KITTY_MSG_RESIZE_TYPE_HEIGHT, height);
  return(kitty_msg_get_vips_image_msg(image));
}

char *kitty_msg_display_image_buffer(unsigned char *buf, size_t len){
  VipsImage *image = image_buffer_to_vips_image(buf, len);

  return(kitty_msg_get_vips_image_msg(image));
}

char *kitty_msg_display_image_path(char *image_path){
  VipsImage *image = image_path_to_vips_image(image_path);

  return(kitty_msg_get_vips_image_msg(image));
}

char *kitty_msg_display_image_path_resized_height(char *image_path, size_t height){
  VipsImage *image = image_path_to_vips_image(image_path);

  kitty_msg_resize_image(&image, KITTY_MSG_RESIZE_TYPE_HEIGHT, height);
  return(kitty_msg_get_vips_image_msg(image));
}

char *kitty_msg_display_image_path_resized_width(char *image_path, size_t width){
  VipsImage *image = image_path_to_vips_image(image_path);

  kitty_msg_resize_image(&image, KITTY_MSG_RESIZE_TYPE_WIDTH, width);
  return(kitty_msg_get_vips_image_msg(image));
}

///////////////
static bool kitty_msg_resize_image(VipsImage **image, enum kitty_msg_resize_type_t resize_type, size_t resize_value){
  float     resize_ratio;
  VipsImage *resized;

  switch (resize_type) {
  case KITTY_MSG_RESIZE_TYPE_WIDTH:
    resize_ratio = (float)(resize_value) / (float)(vips_image_get_width(*image));
    break;
  case KITTY_MSG_RESIZE_TYPE_HEIGHT:
    resize_ratio = (float)(resize_value) / (float)(vips_image_get_height(*image));
    break;
  default: return(false); break;
  }
  errno = 0;
  if (vips_resize(*image, &resized, resize_ratio, NULL)) {
    log_error("Failed to resize image");
    return(NULL);
  }
  g_object_unref(*image);
  vips_copy(resized, image, NULL);
  return(true);
}

static char *kitty_msg_get_vips_image_msg(VipsImage *image){
  char *msg, *file;

  asprintf(&file, "%s%s-%d-%lld.png", gettempdir(), KITTY_MSG_FILE_PREFIX, getpid(), timestamp());
  if (vips_image_write_to_file(image, file, NULL)) {
    log_error("failed to write");
    return(NULL);
  }
  asprintf(&msg, KITTY_SHM_SHM_ENCODED_PNG_IMAGE_PATH_DIMENSIONS, vips_image_get_width(image), vips_image_get_height(image), b64_encode(file, strlen(file)));
  g_object_unref(image);
  free(file);
  return(msg);
}
static void __attribute__((constructor)) __constructor__kitty_msg(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_kitty_msg") != NULL) {
    log_debug("Enabling kitty_msg Debug Mode");
    KITTY_MSG_DEBUG_MODE = true;
  }
}

static VipsImage *image_buffer_to_vips_image(unsigned char *buf, size_t len){
  VipsImage *image;

  errno = 0;
  if (!(image = vips_image_new_from_buffer(buf, len, "", "access", VIPS_ACCESS_SEQUENTIAL, NULL))) {
    log_error("Failed to read image buffer");
    return(NULL);
  }
  return(image);
}

static VipsImage *image_path_to_vips_image(char *image_path){
  VipsImage *image;
  if (stringfn_equal(fsio_file_extension(image_path), ".qoi")) {
    char *qoi_file;
    errno = 0;
    QOIDecoder *qoi = QOIDecoder_New();
    if (!QOIDecoder_Decode(qoi, fsio_read_binary_file(image_path), fsio_file_size(image_path))) {
      log_error("QOI Loader failed");
      return(NULL);
    }
    errno = 0;
    unsigned char *rgb = QOIDecoder_GetPixels(qoi);
    int           bands = QOIDecoder_HasAlpha(qoi) ? 4 : 3;
    int           width = QOIDecoder_GetWidth(qoi), height = QOIDecoder_GetHeight(qoi);
    int           len = width * height * bands;
    if (!rgb || len < 1) {
      log_error("QOI Decoder failed");
      return(NULL);
    }
    Dbg(len, % d);
    asprintf(&qoi_file, "%s.png", stringfn_substring(image_path, 0, strlen(image_path) - 4));
    QOIDecoder_Delete(qoi);

    errno = 0;
    if (!(image = vips_image_new_from_memory(rgb, len, width, height, bands, VIPS_FORMAT_UCHAR))) {
      log_error("Failed to read file %s", image_path); \
      return(NULL);
    }
  }else{
    errno = 0;
    if (!(image = vips_image_new_from_file(image_path, "access", VIPS_ACCESS_SEQUENTIAL, NULL))) { \
      log_error("Failed to read file %s", image_path);                                             \
      return(NULL);
    }
  }

  return(image);
} /* image_path_to_vips_image */

static char *save_restore_msg(char *msg, int row, int col){
  char *s;

  asprintf(&s, "\x1b[s\x1b[%d;%dH%s\x1b[u", row, col, msg);
  free(msg);
  return(s);
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
