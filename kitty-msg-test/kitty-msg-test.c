#define INCBIN_SILENCE_BITCODE_WARNING
#include "ansi-codes/ansi-codes.h"
#include "ansi-utils/ansi-utils.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "clamp/clamp.h"
#include "incbin/incbin.h"
#include "kitty-msg-test/kitty-msg-test.h"
#include "kitty/msg/msg.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
INCBIN(animation_1, "assets/animation_1.png");
INCBIN(communist_goals_qoi, "assets/communist_goals.qoi");
INCBIN(communist_goals_qoir, "assets/communist_goals.qoir");
INCBIN(communist_goals_jpeg, "assets/communist_goals.jpeg");
INCBIN(communist_goals_png, "assets/communist_goals.png");
INCBIN(communist_goals_tiff, "assets/communist_goals.tiff");
INCBIN(communist_goals_bmp, "assets/communist_goals.bmp");
INCBIN(communist_goals_gif, "assets/communist_goals.gif");
INCBIN(kitty_icon_qoi, "assets/kitty_icon.qoi");
INCBIN(kitty_icon_qoir, "assets/kitty_icon.qoir");
INCBIN(kitty_icon_jpeg, "assets/kitty_icon.jpeg");
INCBIN(kitty_icon_png, "assets/kitty_icon.png");
INCBIN(kitty_icon_tiff, "assets/kitty_icon.tiff");
INCBIN(kitty_icon_bmp, "assets/kitty_icon.bmp");
INCBIN(kitty_icon_gif, "assets/kitty_icon.gif");
static int    term_width = 0, term_height = 0;
#define QTY(X)    (sizeof(X) / sizeof(X[0]))
static char   *files[] = {
  "/tmp/kitty_icon.png",
  "/tmp/kitty_icon.tiff",
  "/tmp/kitty_icon.bmp",
  "/tmp/kitty_icon.jpeg",
  "/tmp/kitty_icon.gif",
  "/tmp/communist_goals.tiff",
  "/tmp/communist_goals.bmp",
  "/tmp/communist_goals.jpeg",
  "/tmp/communist_goals.gif",
  "/tmp/communist_goals.png",
  "/tmp/animation_1.png",
  "/tmp/communist_goals.qoir",
  "/tmp/communist_goals.qoi",
  "/tmp/kitty_icon.qoi",
  "/tmp/kitty_icon.qoir",
};
static size_t sizes[] = { 25, 50, 100, 250, }, files_qty = QTY(files), sizes_qty = QTY(sizes);

void __init(){
  au_term_size(&term_width, &term_height);
  term_width  = term_width == 0 ? 120 : term_width;
  term_height = term_height == 0 ? 40 : term_height;
  fsio_write_binary_file(files[0], gkitty_icon_tiffData, gkitty_icon_tiffSize);
  printf("\n");
  fsio_write_binary_file(files[1], gkitty_icon_bmpData, gkitty_icon_bmpSize);
  printf("\n");
  fsio_write_binary_file(files[2], gkitty_icon_jpegData, gkitty_icon_jpegSize);
  printf("\n");
  fsio_write_binary_file(files[3], gkitty_icon_gifData, gkitty_icon_gifSize);
  printf("\n");
  fsio_write_binary_file(files[4], gkitty_icon_pngData, gkitty_icon_pngSize);
  printf("\n");
  fsio_write_binary_file(files[5], gcommunist_goals_tiffData, gcommunist_goals_tiffSize);
  printf("\n");
  fsio_write_binary_file(files[6], gcommunist_goals_bmpData, gcommunist_goals_bmpSize);
  printf("\n");
  fsio_write_binary_file(files[7], gcommunist_goals_jpegData, gcommunist_goals_jpegSize);
  printf("\n");
  fsio_write_binary_file(files[8], gcommunist_goals_gifData, gcommunist_goals_gifSize);
  printf("\n");
  fsio_write_binary_file(files[9], gcommunist_goals_pngData, gcommunist_goals_pngSize);
  printf("\n");
  fsio_write_binary_file(files[10], ganimation_1Data, ganimation_1Size);
  printf("\n");
  fsio_write_binary_file(files[11], gcommunist_goals_qoirData, gcommunist_goals_qoirSize);
  printf("\n");
  fsio_write_binary_file(files[12], gcommunist_goals_qoiData, gcommunist_goals_qoiSize);
  printf("\n");
  fsio_write_binary_file(files[13], gkitty_icon_qoiData, gkitty_icon_qoiSize);
  printf("\n");
  fsio_write_binary_file(files[14], gkitty_icon_qoirData, gkitty_icon_qoirSize);
  printf("\n");
}

#define MSGS_QTY    2

TEST t_kitty_msg_image_buffers_resized(){
  char *msgs[MSGS_QTY];

  for (size_t i = 0; i < files_qty; i++)
    for (size_t s = 0; s < sizes_qty; s++) {
      Info(">Buffer Size:%s|Resized to:%lupx Width & Height|", bytes_to_string(fsio_file_size(files[i])), sizes[s]);
      msgs[0] = kitty_msg_display_image_buffer_resized_width(fsio_read_binary_file(files[i]), fsio_file_size(files[i]), sizes[s]);
      printf("\n");
      msgs[1] = kitty_msg_display_image_buffer_resized_height(fsio_read_binary_file(files[i]), fsio_file_size(files[i]), sizes[s]);
      printf("\n");
      for (size_t m = 0; m < MSGS_QTY; m++) {
        ASSERT_GT(strlen(msgs[m]), 0);
        fprintf(stdout, "%s\n", msgs[m]);
      }
      fflush(stdout);
    }
  PASS();
}

TEST t_kitty_msg_image_files_resized(){
  char *msgs[MSGS_QTY];

  for (size_t i = 0; i < files_qty; i++)
    for (size_t s = 0; s < sizes_qty; s++) {
      Info(">File:%s|Resized to:%lupx Width & Height|", files[i], sizes[s]);
      msgs[0] = kitty_msg_display_image_path_resized_width(files[i], sizes[s]);
      printf("\n");
      msgs[1] = kitty_msg_display_image_path_resized_height(files[i], sizes[s]);
      printf("\n");
      for (size_t m = 0; m < MSGS_QTY; m++) {
        ASSERT_GT(strlen(msgs[m]), 0);
        fprintf(stdout, "%s\n", msgs[m]);
      }
      fflush(stdout);
    }
  PASS();
}

TEST t_kitty_msg_image_buffers(){
  char *msg;

  for (size_t i = 0; i < files_qty; i++) {
    msg = kitty_msg_display_image_buffer(fsio_read_binary_file(files[i]), fsio_file_size(files[i]));
    printf("\n");
    ASSERT_GT(strlen(msg), 0);
    log_info("%s", files[i]);
    fprintf(stdout, "%s\n", msg);
    fflush(stdout);
  }

  PASS();
}

TEST t_kitty_msg_delete_images(){
  char *msg;

  msg = kitty_msg_delete_images();
  ASSERT_GT(strlen(msg), 0);
  fprintf(stdout, AC_RED "Deleting Images\n" AC_RESETALL);
  fprintf(stdout, "%s\n", msg);
  fflush(stdout);
}

TEST t_kitty_msg_cursor(){
  char *msg;
  int  row = 0, col = 0, nrow = 0, ncol = 0;

  for (int i = 0; i < (int)files_qty; i++)
    for (size_t s = 0; s < sizes_qty; s++) {
      nrow = row + (i * 5);
      ncol = col + (i * 5);
      Info(">File:%s|Resized to:%lupx Width & Height|Row:%d/%d|Col:%d/%d|", files[i], sizes[s], nrow, term_height, ncol, term_width);
      msg = kitty_msg_display_image_buffer_resized_width_at_row_col(fsio_read_binary_file(files[i]), fsio_file_size(files[i]), sizes[s], nrow, ncol);
      ASSERT_GT(strlen(msg), 0);
      fprintf(stdout, "%s\n", msg);
      fflush(stdout);
    }
  PASS();
}

TEST t_kitty_image_buffers(){
  for (size_t i = 0; i < files_qty; i++) {
    bool ok = kitty_display_image_buffer(fsio_read_binary_file(files[i]), fsio_file_size(files[i]));
    printf("\n");
    ASSERT_EQ(ok, true);
  }
  PASS();
}

TEST t_kitty_image_files(){
  for (size_t i = 0; i < files_qty; i++) {
    bool ok = kitty_display_image_path(files[i]);
    printf("\n");
    ASSERT_EQ(ok, true);
  }
  PASS();
}

TEST t_kitty_msg_image_files(){
  char *msg;

  for (size_t i = 0; i < files_qty; i++) {
    msg = kitty_msg_display_image_path(files[i]);
    printf("\n");
    ASSERT_GT(strlen(msg), 0);
    fprintf(stdout, AC_YELLOW "%s (%s)\n" AC_RESETALL, files[i], bytes_to_string(fsio_file_size(files[i])));
    fprintf(stdout, "%s\n", msg);
    fflush(stdout);
  }

  PASS();
}

SUITE(s_kitty_msg_test_delete_images) {
  RUN_TEST(t_kitty_msg_delete_images);
}
SUITE(s_kitty_msg_test_buffers) {
  RUN_TEST(t_kitty_msg_image_buffers);
}
SUITE(s_kitty_test_buffers) {
  RUN_TEST(t_kitty_image_buffers);
}
SUITE(s_kitty_test_files) {
  RUN_TEST(t_kitty_image_files);
}
SUITE(s_kitty_msg_test_files) {
  RUN_TEST(t_kitty_msg_image_files);
}
SUITE(s_kitty_msg_test_buffers_resized) {
  RUN_TEST(t_kitty_msg_image_buffers_resized);
}
SUITE(s_kitty_msg_test_files_resized) {
  RUN_TEST(t_kitty_msg_image_files_resized);
}
SUITE(s_kitty_msg_test_cursor) {
  RUN_TEST(t_kitty_msg_cursor);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  __init();
  GREATEST_MAIN_BEGIN();
  if (isatty(STDOUT_FILENO)) {
    RUN_SUITE(s_kitty_test_files);
    RUN_SUITE(s_kitty_test_buffers);
    RUN_SUITE(s_kitty_msg_test_files);
    RUN_SUITE(s_kitty_msg_test_delete_images);
    RUN_SUITE(s_kitty_msg_test_buffers);
    RUN_SUITE(s_kitty_msg_test_files_resized);
    RUN_SUITE(s_kitty_msg_test_buffers_resized);
    RUN_SUITE(s_kitty_msg_test_cursor);
  }
  GREATEST_MAIN_END();
}
