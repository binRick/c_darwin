#pragma once
#ifndef LAYOUT_UTILS_C
#define LAYOUT_UTILS_C
#define LOCAL_DEBUG_MODE    LAYOUT_DEBUG_MODE
////////////////////////////////////////////
#define LAY_IMPLEMENTATION
#define MAX_ITEMS           1024
#include "layout/utils/utils.h"
static struct layout_render_result_t *layout_init_render_result();
static struct layout_result_t *layout_init_result(int qty);
static void print_rect(char *NAME, lay_vec4 RECT);
static bool LAYOUT_DEBUG_MODE = false;

////////////////////////////////////////////
static void print_rect(char *NAME, lay_vec4 RECT){
  char *s;

  asprintf(&s,
           AC_YELLOW "%s" AC_RESETALL
           "\n\ttop left: %dx%d"
           "\n\twidth   : %d"
           "\n\theight  : %d"
           "%s",
           NAME,
           RECT[0],
           RECT[1],
           RECT[2],
           RECT[3],
           ""
           );
  fprintf(stderr, "%s\n", s);
  free(s);
}

struct Vector *get_layouts_v(void){
  struct Vector *v = vector_new();
  return(v);
}
struct layout_request_t *layout_init_request(){
  struct layout_request_t *req = calloc(1, sizeof(struct layout_request_t));

  req->mode          = LAYOUT_MODE_HORIZONTAL;
  req->qty           = 0;
  req->debug         = true;
  req->max_width     = 100;
  req->max_width     = 100;
  req->master_width  = (int)((float)(req->max_width) * 2.0 / 3.0);
  req->master_height = (int)((float)(req->max_height) * 1.0);
  return(req);
}

static struct layout_render_result_t *layout_init_render_result(){
  struct layout_request_t *res = calloc(1, sizeof(struct layout_render_result_t));

  return(res);
}

struct layout_render_request_t *layout_init_render_request(){
  struct layout_request_t *req = calloc(1, sizeof(struct layout_render_request_t));

  return(req);
}

static struct layout_result_t *layout_init_result(int qty){
  struct layout_result_t *res = calloc(1, sizeof(struct layout_result_t));

  res->started     = timestamp();
  res->master      = calloc(1, sizeof(struct layout_content_result_t));
  res->items       = calloc(qty, sizeof(struct layout_content_result_t));
  res->layout      = calloc(1, sizeof(struct layout_t));
  res->layout->ctx = calloc(1, sizeof(struct lay_context));
  lay_init_context(res->layout->ctx);
  lay_reserve_items_capacity(res->layout->ctx, MAX_ITEMS);
  res->layout->root     = lay_item(res->layout->ctx);
  res->layout->master   = lay_item(res->layout->ctx);
  res->layout->content  = lay_item(res->layout->ctx);
  res->layout->contents = calloc(qty, sizeof(lay_id));
  return(res);
}

struct layout_render_result_t *layout_render(struct layout_render_request_t *req){
  struct layout_render_result_t *res = layout_init_render_result();

  printf(" - Size: %dx%d\n - Master\n\tpos:%dx%d|size:%dx%d\n",
         req->result->width,
         req->result->height,
         req->result->master->width,
         req->result->master->height,
         req->result->master->x,
         req->result->master->y);
  for (size_t i = 0; i < req->result->qty; i++) {
    continue;
    printf(" - Content #%lu\n\tpos:%dx%d|size:%dx%d\n",
           i,
           req->result->items[i]->x,
           req->result->items[i]->y,
           req->result->items[i]->width,
           req->result->items[i]->height);
  }

  return(res);
}

bool layout_print_result(struct layout_result_t *res){
  printf(" - Size: %dx%d\n - Master\n\tpos:%dx%d|size:%dx%d\n",
         res->width, res->height,
         res->master->x,
         res->master->y,
         res->master->width,
         res->master->height);
  for (size_t i = 0; i < res->qty; i++) {
    printf(" - Content #%lu\n\tpos:%dx%d|size:%dx%d\n",
           i,
           res->items[i]->x,
           res->items[i]->y,
           res->items[i]->width,
           res->items[i]->height);
  }
  return(true);
}

struct layout_result_t *layout_request(struct layout_request_t *req){
  struct layout_result_t *res = layout_init_result(req->qty);

  lay_set_size_xy(res->layout->ctx, res->layout->root, req->max_width, req->max_height);
  lay_set_contain(res->layout->ctx, res->layout->root, LAY_ROW);
  lay_insert(res->layout->ctx, res->layout->root, res->layout->master);
  lay_insert(res->layout->ctx, res->layout->root, res->layout->content);
  lay_set_size_xy(res->layout->ctx, res->layout->master, req->master_width, req->master_height);
  lay_set_behave(res->layout->ctx, res->layout->master, LAY_VFILL);
  lay_set_contain(res->layout->ctx, res->layout->master, LAY_ROW);
  lay_set_margins_ltrb(res->layout->ctx, res->layout->master,
                       req->master_margins[0],
                       req->master_margins[1],
                       req->master_margins[2],
                       req->master_margins[3]
                       );
  lay_set_margins_ltrb(res->layout->ctx, res->layout->content,
                       req->content_margins[0],
                       req->content_margins[1],
                       req->content_margins[2],
                       req->content_margins[3]
                       );
  lay_set_behave(res->layout->ctx, res->layout->content, LAY_VFILL | LAY_HFILL);
  lay_set_contain(res->layout->ctx, res->layout->content, req->mode);
  for (size_t i = 0; i < req->qty; i++) {
    res->layout->contents[i] = lay_item(res->layout->ctx);
    lay_insert(res->layout->ctx, res->layout->content, res->layout->contents[i]);
    lay_set_behave(res->layout->ctx, res->layout->contents[i], LAY_FILL);
  }
  lay_run_context(res->layout->ctx);
  lay_vec4 root_rect = lay_get_rect(res->layout->ctx, res->layout->root);

  res->width  = root_rect[2];
  res->height = root_rect[3];
  lay_get_rect_xywh(res->layout->ctx, res->layout->master,
                    &(res->master->x),
                    &(res->master->y),
                    &(res->master->width),
                    &(res->master->height)
                    );
  for (size_t i = 0; i < req->qty; i++) {
    res->qty++;
    res->items[i] = calloc(1, sizeof(struct layout_content_result_t));
    lay_get_rect_xywh(res->layout->ctx, res->layout->contents[i],
                      &(res->items[i]->x),
                      &(res->items[i]->y),
                      &(res->items[i]->width),
                      &(res->items[i]->height)
                      );
  }
  res->dur = timestamp() - res->started;
  if (req->debug) {
    layout_print_result(res);
  }
  return(res);
} /* layout_request */

#define MAX_WIDTH                1280
#define MAX_HEIGHT               720
#define MASTER_WIDTH             600
#define MASTER_MARGIN_LEFT       5
#define MASTER_MARGIN_RIGHT      5
#define MASTER_MARGIN_TOP        5
#define MASTER_MARGIN_BOTTOM     5
#define CONTENT_MARGIN_LEFT      2
#define CONTENT_MARGIN_RIGHT     2
#define CONTENT_MARGIN_TOP       2
#define CONTENT_MARGIN_BOTTOM    2
#define CONTENTS                 4
#define CONTENT_MODE             LAY_COLUMN
#define LAYOUT_INIT()                                                                                                             \
  lay_context ctx;                                                                                                                \
  lay_init_context(&ctx);                                                                                                         \
  lay_reserve_items_capacity(&ctx, MAX_ITEMS);                                                                                    \
  lay_id root         = lay_item(&ctx);                                                                                           \
  lay_id master_list  = lay_item(&ctx);                                                                                           \
  lay_id content_view = lay_item(&ctx);                                                                                           \
  lay_id contents[CONTENTS];                                                                                                      \
  lay_set_size_xy(&ctx, root, MAX_HEIGHT, MAX_HEIGHT);                                                                            \
  lay_set_contain(&ctx, root, LAY_ROW);                                                                                           \
  lay_insert(&ctx, root, master_list);                                                                                            \
  lay_insert(&ctx, root, content_view);                                                                                           \
  lay_set_margins_ltrb(&ctx, master_list, MASTER_MARGIN_LEFT, MASTER_MARGIN_RIGHT, MASTER_MARGIN_TOP, MASTER_MARGIN_BOTTOM);      \
  lay_set_margins_ltrb(&ctx, content_view, CONTENT_MARGIN_LEFT, CONTENT_MARGIN_RIGHT, CONTENT_MARGIN_TOP, CONTENT_MARGIN_BOTTOM); \
////////////////
#define MASTER_INIT()                                  \
  lay_set_size_xy(&ctx, master_list, MASTER_WIDTH, 0); \
  lay_set_behave(&ctx, master_list, LAY_VFILL);        \
  lay_set_contain(&ctx, master_list, LAY_ROW);         \
////////////////
void layout_test(){
  LAYOUT_INIT()
  MASTER_INIT()

  lay_set_behave(&ctx, content_view, LAY_VFILL | LAY_HFILL);
  lay_set_contain(&ctx, content_view, CONTENT_MODE);
  for (int i = 0; i < CONTENTS; i++) {
    contents[i] = lay_item(&ctx);
    if (i == 0) {
    }
    lay_insert(&ctx, content_view, contents[i]);
    lay_set_behave(&ctx, contents[i], LAY_FILL);
  }
  lay_run_context(&ctx);

  print_rect("Master", lay_get_rect(&ctx, master_list));
  char *n;
  for (int i = 0; i < CONTENTS; i++) {
    asprintf(&n, "Content #%d", i);
    print_rect(n, lay_get_rect(&ctx, contents[i]));
    free(n);
  }
  lay_reset_context(&ctx);
  lay_destroy_context(&ctx);
}
static void __attribute__((constructor)) __constructor__layout(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_layout") != NULL) {
    log_debug("Enabling layout Debug Mode");
    LAYOUT_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#undef LOCAL_DEBUG_MODE
#endif
