
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

#include "vips-basics-test/vips-basics-test.h"
#include "vips/vips.h"
#define UNUSED    __attribute__((unused))
#include "incbin/incbin.h"
INCBIN(communist_goals, "assets/communist-goals.png");
INCBIN(kitty_icon, "assets/kitty_icon.png");
#define PROGRESS    false
#define QTY(X)    (sizeof(X) / sizeof(X[0]))
static char *files[] = {
  "/tmp/communist-goals.png",
  "/tmp/kitty_icon.png",
}, *exts[] = { "png", "gif", "jpg", "tif", "raw", "webp", "vips" };
static size_t files_qty = QTY(files), exts_qty = QTY(exts);

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
        10, 10, overlay[0]->Xsize - 20, overlay[0]->Ysize - 20, "fill", TRUE, NULL
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

void preeval_callback(VipsImage __attribute__((unused)) *image, VipsProgress __attribute__((unused)) *progress, void __attribute__((unused)) *pdata) {
  log_debug("preeval_callback:");
}

void eval_callback(VipsImage UNUSED *image, VipsProgress *progress, void UNUSED *pdata) {
  log_debug("eval_callback: percent = %d", progress->percent);
}

void posteval_callback(VipsImage UNUSED *image, VipsProgress *progress, void UNUSED *pdata) {
  log_debug("posteval_callback: finished in %.3gs", g_timer_elapsed(progress->start, NULL));
}

TEST t_vips_basics_test3(){
  size_t TEST_INDEX = 3;

  for (size_t i = 0; i < files_qty; i++)
    for (size_t o = 0; o < exts_qty; o++) {
      VipsImage *image;
      char      *outfile;
      asprintf(&outfile, "/tmp/output-%lu-%lu-%lu.%s", i, o, TEST_INDEX, exts[o]);

      if (VIPS_INIT(files[i]))
        FAIL();

      if (!(image = vips_image_new_from_file(files[i], "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
        FAIL();

      VipsImage *out;
      float     resize = (float)1 / ((float)1 / (float)(o + (float)1) * 10);

      if (vips_resize(image, &out, resize, NULL))
        FAIL();
      log_debug(AC_BLUE "Resizing by %f %s %s [%dx%d] (%d)" AC_RESETALL,
                resize,
                bytes_to_string(fsio_file_size(files[i])),
                files[i],
                vips_image_get_width(image),
                vips_image_get_height(image),
                vips_image_get_format(image)
                );

      vips_image_set_progress(out, PROGRESS);
      g_signal_connect(out, "preeval", G_CALLBACK(preeval_callback), NULL);
      g_signal_connect(out, "eval", G_CALLBACK(eval_callback), NULL);
      g_signal_connect(out, "posteval", G_CALLBACK(posteval_callback), NULL);

      if (vips_image_write_to_file(out, outfile, NULL))
        FAIL();
      timg_utils_image(outfile);
      log_debug(AC_MAGENTA "%s %s [%dx%d] (%d)" AC_RESETALL,
                bytes_to_string(fsio_file_size(outfile)),
                outfile,
                vips_image_get_width(out),
                vips_image_get_height(out),
                vips_image_get_format(out)
                );
      g_object_unref(out);

      g_object_unref(image);
    }
  PASS();
} /* t_vips_basics_test3 */

TEST t_vips_basics_test2(){
  size_t TEST_INDEX = 2;

  for (size_t i = 0; i < files_qty; i++)
    for (size_t o = 0; o < exts_qty; o++) {
      char *outfile, *infile;
      asprintf(&outfile, "/tmp/output-%lu-%lu-%lu.%s", i, o, TEST_INDEX, exts[o]);
      asprintf(&infile, "%s", files[i]);

      VipsImage  *image;
      VipsObject *context;
      VipsImage  *x;

      if (VIPS_INIT(infile))
        FAIL();

      if (!(image = vips_image_new_from_file(infile, "access", VIPS_ACCESS_SEQUENTIAL, NULL)))
        FAIL();

      vips_object_print_summary(VIPS_OBJECT(image));

      context = VIPS_OBJECT(vips_image_new());
      if (annotate_image(context, image, &x)) {
        g_object_unref(image);
        g_object_unref(context);
        FAIL();
      }
      g_object_unref(image);
      g_object_unref(context);
      image = x;

      if (vips_image_write_to_file(image, outfile, NULL)) {
        g_object_unref(image);
        FAIL();
        g_object_unref(image);
      }
      timg_utils_image(outfile);
      log_debug(AC_CYAN "Annotated Image: %s [%dx%d] (%d)" AC_RESETALL,
                outfile,
                vips_image_get_width(image),
                vips_image_get_height(image),
                vips_image_get_format(image)
                );
    }

  PASS();
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

SUITE(s_vips_basics_test1) {
  RUN_TEST(t_vips_basics_test1);
}
SUITE(s_vips_basics_test2) {
  RUN_TEST(t_vips_basics_test2);
}
SUITE(s_vips_basics_test3) {
  RUN_TEST(t_vips_basics_test3);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  VIPS_INIT(argv);
  fsio_write_binary_file(files[0], gcommunist_goalsData, gcommunist_goalsSize);
  timg_utils_image(files[0]);
  fsio_write_binary_file(files[1], gkitty_iconData, gkitty_iconSize);
  timg_utils_image(files[1]);
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_vips_basics_test1);
  RUN_SUITE(s_vips_basics_test2);
  RUN_SUITE(s_vips_basics_test3);
  GREATEST_MAIN_END();
  vips_shutdown();
}
