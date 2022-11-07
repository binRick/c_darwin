
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "kitty/msg/msg.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "vips-basics-test/vips-basics-test.h"
#include "vips/vips.h"
#define UNUSED    __attribute__((unused))
#define KITTY_PRINT_PATH(PATH)    { do {                                                                                     \
                                      if (greatest_get_verbosity() > 1) {                                                    \
                                        kitty_display_image_buffer(fsio_read_binary_file(outfile), fsio_file_size(outfile)); \
                                        printf("\n");                                                                        \
                                      }                                                                                      \
                                    } while (0); }
#include "incbin/incbin.h"
INCBIN(communist_goals, "assets/communist-goals.png");
INCBIN(kitty_icon, "assets/kitty_icon.png");
INCBIN(spinner, "assets/spinner.gif");
INCBIN(kitty_icon_gif, "assets/kitty_icon.gif");
#define PROGRESS    false
#define QTY(X)    (sizeof(X) / sizeof(X[0]))
static const char
*gifs[] = {
  "/tmp/spinner.gif",
  "/tmp/kitty_icon.gif",
},
*frame_exts[] = { "qoi", "png", "gif", "jpg", "webp", },
*files[]      = {
  "/tmp/communist-goals.png",
  "/tmp/kitty_icon.png",
},
*exts[] = { "qoi", "png", "gif", "jpg", "tif", "webp", };
static size_t
  gifs_qty  = QTY(gifs),
  files_qty = QTY(files),
  exts_qty  = QTY(exts);

static int annotate_image(VipsObject *context, VipsImage *image, VipsImage **out) {
  int       page_height = vips_image_get_page_height(image);
  int       n_pages     = image->Ysize / page_height;
  VipsImage **overlay   = (VipsImage **)
                          vips_object_local_array(context, n_pages);
  VipsImage **page = (VipsImage **)
                     vips_object_local_array(context, n_pages);
  VipsImage **annotated = (VipsImage **)
                          vips_object_local_array(context, n_pages);
  double    red[]         = { 255, 0, 0, 255 };
  double    transparent[] = { 0, 0, 0, 0 };

  int       i;

  for ( i = 0; i < n_pages; i++ )
    if (vips_crop(image, &page[i], 0, page_height * i, image->Xsize, page_height, NULL))
      return(-1);

  if (!(overlay[0] = vips_image_new_from_image(page[0], red, VIPS_NUMBER(red)))
      || vips_draw_rect(
        overlay[0], transparent, VIPS_NUMBER(transparent),
        10, 10, overlay[0]->Xsize - 20, overlay[0]->Ysize - 20, "fill", true, NULL
        )
      )
    return(-1);

  for ( i = 0; i < n_pages; i++ )
    if (vips_composite2(page[i], overlay[0], &annotated[i],
                        VIPS_BLEND_MODE_OVER, NULL))
      return(-1);

  if (vips_arrayjoin(annotated, out, n_pages, "across", 1, NULL))
    return(-1);

  return(0);
} /* annotate_image */

TEST t_vips_gif_frames(){
  size_t        TEST_INDEX = 4;
  VipsImage     *image, *annotated, *resized, *frames;
  VipsObject    *context = NULL;
  char          *outfile, *infile, *frameoutfile;
  float         resize = 0.10;
  int           n_pages;
  size_t        qty = 0, total_bytes = 0;
  unsigned long ts = timestamp();

  for (size_t i = 0; i < gifs_qty; i++)
    for (size_t o = 0; o < QTY(frame_exts); o++) {
      infile  = gifs[i];
      context = VIPS_OBJECT(vips_image_new());
      if (!(image = vips_image_new_from_file(infile, "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
        FAILm("Failed to load file");
      if (vips_image_get_n_pages(image) > 1)
        for (int f = 0; f < vips_image_get_n_pages(image); f++) {
          unsigned long ts = timestamp();
          if (!(frames = vips_image_new_from_file(infile, "access", VIPS_ACCESS_SEQUENTIAL, "page", f, NULL)))
            FAIL();
          asprintf(&outfile, "/tmp/gif-%s-%lu-%lu-frame-%d.%s", basename(stringfn_substring(infile, 0, strlen(infile) - 4)), i, TEST_INDEX, f, frame_exts[o]);
          if (vips_image_write_to_file(frames, outfile, NULL))
            FAIL();
          if (greatest_get_verbosity() > 0)
            log_debug(AC_BLUE "Wrote %s Frame #%d/%d to %s in %s",
                      bytes_to_string(fsio_file_size(outfile)),
                      f + 1, vips_image_get_n_pages(image),
                      outfile, milliseconds_to_string(timestamp() - ts)
                      );
          total_bytes += fsio_file_size(outfile);
          qty++;
          KITTY_PRINT_PATH(outfile);
        }

      g_object_unref(image);
      g_object_unref(context);
    }
  log_debug(AC_BLUE "Wrote %lu %s Images in %s", qty,
            bytes_to_string(total_bytes),
            milliseconds_to_string(timestamp() - ts)
            );

  PASS();
} /* t_vips_gif_frames */

TEST t_vips_resize(){
  unsigned long ts = timestamp();
  size_t        total_len = 0, qty = 0;
  size_t        TEST_INDEX = 3;
  VipsImage     *image;

  for (size_t i = 0; i < files_qty; i++)
    for (size_t o = 0; o < exts_qty; o++) {
      char  *outfile;
      float resize = 0.10;
      asprintf(&outfile, "/tmp/output-resized-%.0f-percent-%lu-%lu-%lu.%s", resize * 100, i, o, TEST_INDEX, exts[o]);

      if (!(image = vips_image_new_from_file(files[i], "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
        FAILm("Failed to load file");

      VipsImage *out;

      if (vips_resize(image, &out, resize, NULL))
        FAILm("Failed to resize image");
      if (greatest_get_verbosity() > 1)
        log_debug(AC_BLUE "Resizing by %f %s %s [%dx%d] (%d) %s" AC_RESETALL,
                  resize,
                  bytes_to_string(fsio_file_size(files[i])),
                  files[i],
                  vips_image_get_width(image),
                  vips_image_get_height(image),
                  vips_image_get_format(image), outfile
                  );

      if (vips_image_write_to_file(out, outfile, NULL))
        FAILm("Failed to write outfile");
      if (greatest_get_verbosity() > 1)
        log_debug(AC_MAGENTA "%s %s [%dx%d] (%d)" AC_RESETALL,
                  bytes_to_string(fsio_file_size(outfile)),
                  outfile,
                  vips_image_get_width(out),
                  vips_image_get_height(out),
                  vips_image_get_format(out)
                  );
      KITTY_PRINT_PATH(outfile);

      unsigned long ts = timestamp();
      if (!(out = vips_image_new_from_file(outfile, "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
        FAILm("Failed to Read outfile");
      if (greatest_get_verbosity() > 0)
        log_debug(AC_GREEN "%s %s [%dx%d] (%d) %s" AC_RESETALL,
                  bytes_to_string(fsio_file_size(outfile)),
                  outfile,
                  vips_image_get_width(out),
                  vips_image_get_height(out),
                  vips_image_get_format(out), milliseconds_to_string(timestamp() - ts)
                  );

      qty++;
      total_len += fsio_file_size(outfile);
      g_object_unref(out);
      g_object_unref(image);
    }
  char *msg;

  asprintf(&msg, "Resized %s %lu Images in %s", bytes_to_string(total_len), qty, milliseconds_to_string(timestamp() - ts));
  PASSm(msg);
} /* t_vips_resize */

TEST t_vips_annotate(){
  size_t        TEST_INDEX = 2;
  unsigned long ts = timestamp();
  size_t        total_len = 0, qty = 0;
  VipsImage     *image, *annotated;
  VipsObject    *context = NULL;

  for (size_t i = 0; i < files_qty; i++)
    for (size_t o = 0; o < exts_qty; o++) {
      char *outfile, *infile;
      asprintf(&outfile, "/tmp/output-%lu-%lu-%lu.%s", i, o, TEST_INDEX, exts[o]);
      asprintf(&infile, "%s", files[i]);

      unsigned long ts = timestamp();
      context = VIPS_OBJECT(vips_image_new());
      if (!(image = vips_image_new_from_file(infile, "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
        FAIL();
      if (annotate_image(context, image, &annotated))
        FAIL();
      if (vips_image_write_to_file(annotated, outfile, NULL))
        FAIL();
      unsigned long dur = timestamp() - ts;

      if (greatest_get_verbosity() > 0)
        log_debug(AC_CYAN "Annotated %s Image %s in %s : [%dx%d]" AC_RESETALL,
                  outfile,
                  bytes_to_string(fsio_file_size(outfile)),
                  milliseconds_to_string(dur),
                  vips_image_get_width(annotated),
                  vips_image_get_height(annotated)
                  );
      qty++;
      total_len += fsio_file_size(outfile);
      KITTY_PRINT_PATH(outfile);

      g_object_unref(image);
      g_object_unref(context);
    }

  char *msg;

  asprintf(&msg, "Annotated %s %lu Images in %s", bytes_to_string(total_len), qty, milliseconds_to_string(timestamp() - ts));
  PASSm(msg);
} /* t_vips_basics_test2 */

TEST t_vips_basics_test1(){
  size_t UNUSED TEST_INDEX = 1;

  for (size_t i = 0; i < files_qty; i++) {
    char      *infile = files[i];
    VipsImage *image;
    void      *new_buf;
    size_t    new_len;
    gchar     *buf;
    gsize     len;

    if (!g_file_get_contents(infile, &buf, &len, NULL))
      FAIL();
    if (!(image = vips_image_new_from_buffer(buf, len, "", "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
      FAIL();

    if (vips_image_write_to_buffer(image, ".jpg", &new_buf, &new_len, "Q", 95, NULL))
      FAIL();
    log_debug(AC_YELLOW "%s [%dx%d] (%d) : %s -> %s" AC_RESETALL,
              infile,
              vips_image_get_width(image),
              vips_image_get_height(image),
              vips_image_get_format(image),
              bytes_to_string(len), bytes_to_string(new_len)
              );

    g_object_unref(image);
    g_free(new_buf);
    g_free(buf);
  }

  PASS();
} /* t_vips_basics_test1 */

SUITE(s_vips_annotate) {
  RUN_TEST(t_vips_annotate);
}
SUITE(s_vips_gif_frames) {
  RUN_TEST(t_vips_gif_frames);
}
SUITE(s_vips_resize) {
  RUN_TEST(t_vips_resize);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  VIPS_INIT(argv);
  fsio_write_binary_file(files[0], gcommunist_goalsData, gcommunist_goalsSize);
  fsio_write_binary_file(files[1], gkitty_iconData, gkitty_iconSize);
  fsio_write_binary_file(gifs[0], gspinnerData, gspinnerSize);
  fsio_write_binary_file(gifs[1], gkitty_icon_gifData, gkitty_icon_gifSize);
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_vips_annotate);
  RUN_SUITE(s_vips_resize);
  RUN_SUITE(s_vips_gif_frames);
  GREATEST_MAIN_END();
  vips_shutdown();
}
