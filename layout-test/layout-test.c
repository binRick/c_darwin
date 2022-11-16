#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "layout-test/layout-test.h"
#include "layout/layout.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include "match/match.h"
#include "vips/vips.h"

TEST t_layout_test_load_dir(){
  int qty=5;
  lay_id t[10];
  lay_vec4 v[10];
  int WIDTH = 1*3400, HEIGHT = 1*1400;
  lay_context ctx;
  lay_init_context(&ctx);
  lay_reserve_items_capacity(&ctx, 1024);
  lay_id root = lay_item(&ctx);
  lay_set_size_xy(&ctx, root, WIDTH, HEIGHT);
  lay_set_behave(&ctx, root, LAY_FILL);
  int files_qty=0;
  VipsImage *vi;
  char *format="qoir",*path,*glob,**files;
  asprintf(&path,"/tmp/%s",format);
  asprintf(&glob,"*.%s",format);
  files = match_files(path,glob,&files_qty);
  char *fp[files_qty];
  unsigned char *resized_rgb_bufs[files_qty];
  unsigned char *qoir_bufs[files_qty];
  size_t resized_rgb_buf_lens[files_qty];
  size_t *qoir_buf_lens = calloc(files_qty,sizeof(size_t));
  VipsImage *joined, *right;

  VipsImage *overlay[10];/*
    if(!(overlay[0] = vips_image_new_from_image(resized,red, VIPS_NUMBER(red)))
    || vips_draw_rect(overlay[0],
                      transparent, VIPS_NUMBER(transparent),
                      10, 10, overlay[0]->Xsize - 20, overlay[0]->Ysize - 20,
                      "fill", TRUE,
                      NULL))
*/
  for(int i=0;i<files_qty;i++){
   asprintf(&fp[i],"%s/%s",path,files[i]);
   if(!fsio_file_exists(fp[i]))continue;
   if(!(vi=vips_image_new_from_file(fp[i],"access",VIPS_ACCESS_SEQUENTIAL,NULL))){
        log_error("Failed opening %s",fp[i]);
        FAIL();
   }
   int w=vips_image_get_width(vi),h=vips_image_get_height(vi);
   log_info("%dx%d %s %s",w,h, bytes_to_string(fsio_file_size(fp[i])),fp[i]);
   VipsImage *resized;
   if(vips_resize(vi,&resized,0.25,NULL)){
        log_error("Failed resizing %s",fp[i]);
        FAIL();
   }
   char *tf;
   asprintf(&tf,"%s%lld-%d.qoir",gettempdir(),timestamp(),i);
   if(vips_image_write_to_file(resized,tf,NULL)){
        log_error("Failed writing buffer %s",fp[i]);
        FAIL();
   }
   qoir_buf_lens[i]=fsio_file_size(tf);
   qoir_bufs[i]=fsio_read_binary_file(tf);
   if(!right){
    right = vips_image_copy_memory(resized);
   }else{
     if(vips_join(resized, right, &joined, VIPS_DIRECTION_HORIZONTAL,NULL)){
        log_error("Failed joining buffer %s",fp[i]);
        FAIL();
     }
     right = vips_image_copy_memory(joined);
   }
   fsio_remove(tf);

   log_info("%dx%d resized to %s",vips_image_get_width(resized),vips_image_get_height(resized),bytes_to_string(qoir_buf_lens[i]));
   continue;
   t[i] = lay_item(&ctx);
  lay_set_size_xy(&ctx, t[i], vips_image_get_width(resized), vips_image_get_height(resized));
   lay_insert(&ctx, root, t[i]);

  }
  char *tf;
   asprintf(&tf,"%sjoined-%lld-%d.qoir",gettempdir(),timestamp(),getpid());
   if(vips_image_write_to_file(joined,tf,NULL)){
        log_error("Failed writing buffer from joined");
        FAIL();
   }
   Ds(tf);
PASS();
  lay_run_context(&ctx);
  char *msg;
  for(int i=0;i<files_qty;i++){
    v[i] = lay_get_rect(&ctx, t[i]);
    for(int x=0;x<4;x++){
      asprintf(&msg,"%s :: %dx%d @ %dx%d", fp[i],v[i][0],v[i][1],v[i][2],v[i][3]);
      log_info("%s",msg);
      free(msg);
    }
  }

  PASS();
}
TEST t_layout_test2(){
  PASS();
}

TEST t_layout_test1(){
  lay_context ctx;
  lay_init_context(&ctx);
  lay_reserve_items_capacity(&ctx, 1024);
  lay_id root = lay_item(&ctx);

#define WIDTH        1800
#define TERMWIDTH    (WIDTH * .60)
  lay_set_size_xy(&ctx, root, WIDTH, 1500);
  lay_id terminals = lay_item(&ctx);

  lay_insert(&ctx, root, terminals);
  lay_set_size_xy(&ctx, terminals, TERMWIDTH, 0);
  lay_set_behave(&ctx, terminals, LAY_RIGHT | LAY_VFILL);
  lay_set_contain(&ctx, terminals, LAY_ROW);
  lay_id t1 = lay_item(&ctx);
  lay_set_behave(&ctx, t1, LAY_FILL);
  lay_insert(&ctx, terminals, t1);
  lay_id browsers = lay_item(&ctx);

  lay_insert(&ctx, root, browsers);
  lay_set_behave(&ctx, browsers, LAY_LEFT | LAY_VFILL);
  lay_set_contain(&ctx, browsers, LAY_ROW);
  lay_id b1 = lay_item(&ctx);
  lay_id b2 = lay_item(&ctx);
  lay_set_behave(&ctx, b1, LAY_TOP | LAY_HFILL);
  lay_set_behave(&ctx, b2, LAY_BOTTOM | LAY_HFILL);
  lay_insert(&ctx, browsers, b1);
  lay_insert(&ctx, browsers, b2);

  lay_run_context(&ctx);
  lay_vec4 b1r = lay_get_rect(&ctx, b1);
  lay_vec4 b2r = lay_get_rect(&ctx, b2);
  lay_vec4 t1r = lay_get_rect(&ctx, t1);
  lay_vec4 br  = lay_get_rect(&ctx, browsers);

  for (int i = 0; i < 4; i++) {
    log_debug("t1                   #%d:   %d", i, t1r[i]);
    log_debug("browsers_rect        #%d:   %d", i, br[i]);
    log_debug("b1                   #%d:   %d", i, b1r[i]);
    log_debug("b2                   #%d:   %d", i, b2r[i]);
  }
  /*my_ui_library_draw_box_x_y_width_height(
   *  terminal_rect[0],
   *  terminal_rect[1],
   *  terminal_rect[2],
   *  terminal_rect[3]);*/
  lay_reset_context(&ctx);
  lay_destroy_context(&ctx);
  PASS();
} /* t_layout_test1 */

SUITE(s_layout_test) {
  RUN_TEST(t_layout_test1);
  RUN_TEST(t_layout_test_load_dir);
  if (isatty(STDOUT_FILENO))
    RUN_TEST(t_layout_test2);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(s_layout_test);
  GREATEST_MAIN_END();
}
