#pragma once
#ifndef KITTY_MSG_H
#define KITTY_MSG_H
//////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "ansi-utils/ansi-utils.h"
#include "b64.c/b64.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "tempdir.c/tempdir.h"
#include "timestamp/timestamp.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
enum kitty_corner_t {
  KITTY_TERMINAL_CORNER_TYPE_TOP_RIGHT,
  KITTY_TERMINAL_CORNER_TYPE_TOP_LEFT,
  KITTY_TERMINAL_CORNER_TYPE_BOTTOM_RIGHT,
  KITTY_TERMINAL_CORNER_TYPE_BOTTOM_LEFT,
  KITTY_TERMINAL_CORNER_TYPES_QTY,
};
enum kitty_msg_resize_type_t {
  KITTY_MSG_RESIZE_TYPE_WIDTH,
  KITTY_MSG_RESIZE_TYPE_HEIGHT,
  KITTY_MSG_RESIZE_TYPES_QTY,
};
struct kitty_position_pixel_t {
  int width, height, row, col;
};
struct kitty_percent_t {
  int width, height, row, col;
};
struct kitty_term_t {
  int row, col;
  int height, width;
};
union kitty_size_t {
  struct kitty_percent_t        percent;
  struct kitty_term_t           term;
  struct kitty_position_pixel_t *pixel;
};
struct kitty_render_title_t {
  char *text;
  char *bg_color, *fg_color;
  bool bold, italic, underline;
};
struct kitty_style_t {
  struct kitty_render_title *title;
};
union kitty_position_t {
  struct kitty_percent_t       percent;
  struct kitty_term_t          term;
  enum kitty_corner_t          corner;
  enum kitty_msg_resize_type_t type;
};
struct kitty_write_t {
  union kitty_position_t *pos;
  union kitty_size_t     *size;
  struct kitty_render_t  *render;
};

char *kitty_msg_delete_images(void);
char *kitty_msg_delete_image_id(size_t id);

bool kitty_display_image_path(char *image_path);
bool kitty_display_image_path_resized_height(char *image_path, size_t height);
bool kitty_display_image_path_resized_width(char *image_path, size_t width);
bool kitty_display_image_buffer(unsigned char *buf, size_t len);
bool kitty_display_image_buffer_resized_width(unsigned char *buf, size_t len, size_t width);
bool kitty_display_image_buffer_resized_height(unsigned char *buf, size_t len, size_t height);

bool kitty_display_image_path_at_row_col(char *image_path, int row, int col);
bool kitty_display_image_path_resized_height_at_row_col(char *image_path, size_t height, int row, int col);
bool kitty_display_image_path_resized_width_at_row_col(char *image_path, size_t width, int row, int col);
bool kitty_display_image_buffer_at_row_col(unsigned char *buf, size_t len, int row, int col);
bool kitty_display_image_buffer_resized_width_at_row_col(unsigned char *buf, size_t len, size_t width, int row, int col);
bool kitty_display_image_buffer_resized_height_at_row_col(unsigned char *buf, size_t len, size_t height, int row, int col);

char *kitty_msg_display_image_path(char *image_path);
char *kitty_msg_display_image_path_resized_height(char *image_path, size_t height);
char *kitty_msg_display_image_path_resized_width(char *image_path, size_t width);
char *kitty_msg_display_image_buffer(unsigned char *buf, size_t len);
char *kitty_msg_display_image_buffer_resized_width(unsigned char *buf, size_t len, size_t width);
char *kitty_msg_display_image_buffer_resized_height(unsigned char *buf, size_t len, size_t height);

char *kitty_msg_display_image_path_at_row_col(char *image_path, int row, int col);
char *kitty_msg_display_image_path_resized_height_at_row_col(char *image_path, size_t height, int row, int col);
char *kitty_msg_display_image_path_resized_width_at_row_col(char *image_path, size_t width, int row, int col);
char *kitty_msg_display_image_buffer_at_row_col(unsigned char *buf, size_t len, int row, int col);
char *kitty_msg_display_image_buffer_resized_width_at_row_col(unsigned char *buf, size_t len, size_t width, int row, int col);
char *kitty_msg_display_image_buffer_resized_height_at_row_col(unsigned char *buf, size_t len, size_t height, int row, int col);

bool kitty_write_terminal_corner(int corner, char *msg);
#endif
