#ifndef DLS_META_C
#define DLS_META_C
#include "core/core.h"
#include "core/core.h"
#include "dls/meta.h"
#include "frameworks/frameworks.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define __LAYOUT_QTY(QTY)      { do {                      \
                                   self->layout.qty = QTY; \
                                 } while (0); }

#define __LAYOUT_SIZE(SIZE)    { do {                        \
                                   self->layout.size = SIZE; \
                                 } while (0); }

char *Vertical_string(const VSelf) {
  char *s;

  asprintf(&s, "%s", "v");
  return(s);
}

bool Vertical_qty(VSelf, int qty){
  VSELF(Vertical);
  __LAYOUT_QTY(qty);
  return(true);
}

bool Horizontal_qty(VSelf, int qty){
  VSELF(Horizontal);
  __LAYOUT_QTY(qty);
  return(true);
}

bool Horizontal_render(VSelf){
  VSELF(Horizontal);
  self->layout.res = layout_request(self->layout.req);
  layout_print_result(self->layout.res);
  return(true);
}

void Horizontal_print(VSelf){
  VSELF(Horizontal);
  layout_print_result(self->layout.res);
}

void Vertical_print(VSelf){
  VSELF(Vertical);
  layout_print_result(self->layout.res);
}

bool Vertical_render(VSelf){
  VSELF(Vertical);
  self->layout.res = layout_request(self->layout.req);
  return(true);
}

bool Horizontal_size(VSelf, float size){
  VSELF(Horizontal);
  __LAYOUT_SIZE(size);
  self->layout.req->master_width  = (int)((float)(self->layout.req->max_width) * self->layout.size);
  self->layout.req->master_height = (int)((float)(self->layout.req->max_height) * 1);
  return(true);
}

bool Vertical_size(VSelf, float size){
  VSELF(Vertical);
  __LAYOUT_SIZE(size);
  self->layout.req->master_height = (int)((float)(self->layout.req->max_width) * self->layout.size);
  self->layout.req->master_width  = (int)((float)(self->layout.req->max_width) * 1);
  return(true);
}

void Vertical_init(VSelf, LAYOUT_INIT_ARGS) {
  VSELF(Vertical);
  log_info("vertical init %f", size);
  self->layout.size            = size;
  self->layout.qty             = qty;
  self->layout.req             = layout_init_request();
  self->layout.req->debug      = false;
  self->layout.req->mode       = LAYOUT_MODE_VERTICAL;
  self->layout.req->max_width  = width;
  self->layout.req->max_height = height;
  self->layout.req->qty        = self->layout.qty;
}

char *Horizontal_string(const VSelf) {
  char *s;

  asprintf(&s, "%s", "h");
  return(s);
}

void Horizontal_init(VSelf, LAYOUT_INIT_ARGS) {
  VSELF(Horizontal);
  log_info("horizontal init %f", size);
  self->layout.width     = width;
  self->layout.height    = height;
  self->layout.size      = size;
  self->layout.qty       = qty;
  self->layout.req       = layout_init_request();
  self->layout.req->mode = LAYOUT_MODE_HORIZONTAL;
  self->layout.req->qty  = self->layout.qty;
  self->layout.res       = layout_request(self->layout.req);
  layout_print_result(self->layout.res);
}
implExtern(Layout, Horizontal);
implExtern(Layout, Vertical);

char * ByID_name(const VSelf) {
  VSELF(ByID);
  return(self->window.name);
}

char * ByName_name(const VSelf) {
  VSELF(ByName);
  return(self->window.name);
}

int ByID_id(const VSelf) {
  VSELF(ByID);
  return(self->window.id);
}

int ByName_id(const VSelf) {
  VSELF(ByName);
  return(self->window.id);
}
#define GET_WINDOW_INDEX_PID(INDEX) \
  (CFDictionaryGetInt(CFArrayGetValueAtIndex(self->windows.list, INDEX), kCGWindowPID))
#define GET_WINDOW_INDEX_ID(INDEX) \
  (CFDictionaryGetInt(CFArrayGetValueAtIndex(self->windows.list, INDEX), kCGWindowNumber))
#define GET_WINDOW_INDEX_OWNER_NAME(INDEX) \
  (CFDictionaryCopyCString(CFArrayGetValueAtIndex(self->windows.list, INDEX), kCGWindowOwnerName))
#define INIT_WINDOWS()    { do {                                                                                                           \
                              self->windows.list = CGWindowListCopyWindowInfo((kCGWindowListExcludeDesktopElements), kCGNullWindowID);     \
                              self->windows.qty  = CFArrayGetCount(self->windows.list);                                                    \
                              self->windows.ids  = hash_new();                                                                             \
                              for (int i = 0; i < self->windows.qty; i++)                                                                  \
                              hash_set(self->windows.ids,                                                                                  \
                                       size_to_string(CFDictionaryGetInt(CFArrayGetValueAtIndex(self->windows.list, i), kCGWindowNumber)), \
                                       (void *)0                                                                                           \
                                       );                                                                                                  \
                              self->windows.props = get_window_properties_map_for_window_ids(self->windows.ids);                           \
                            }while (0); }

struct Window *iterate_window(struct Windows *windows, int i){
  return(&(struct Window){
    .name = (CFDictionaryCopyCString(CFArrayGetValueAtIndex(windows->list, i), kCGWindowName)),
    .id = (CFDictionaryGetInt(CFArrayGetValueAtIndex(windows->list, i), kCGWindowNumber)),
  });
}

struct Window *ByID_find(const VSelf, void *ID) {
  VSELF(ByID);
  INIT_WINDOWS();
  int id = (int)(size_t)ID;
  self->window.id = id;
  log_info("searching by id %d", self->window.id);
  return(&(self->window));
}

struct Window *ByName_find(const VSelf, void *NAME) {
  VSELF(ByName);
  INIT_WINDOWS();
  char *name = (char *)NAME;
  self->window.name = name;
  log_info("searching %d windows by name %s", self->windows.qty, self->window.name);
  hash_t *p;
  hash_each(self->windows.props, {
    p = (hash_t *)val;
    log_info("Window ID: %s> %d props", key, hash_size(p));
  });
  return(&(self->window));
}
implExtern(FindWindow, ByID);
implExtern(FindWindow, ByName);

int Rectangle_perim(const VSelf) {
  VSELF(const Rectangle);
  return((self->a + self->b) * 2);
}

void Rectangle_scale(VSelf, int factor) {
  VSELF(Rectangle);
  self->a *= factor;
  self->b *= factor;
}

int Triangle_perim(const VSelf) {
  VSELF(const Triangle);
  return(self->a + self->b + self->c);
}

void Triangle_scale(VSelf, int factor) {
  VSELF(Triangle);
  self->a *= factor;
  self->b *= factor;
  self->c *= factor;
}

implExtern(DLS, Rectangle);
implExtern(DLS, Triangle);

struct FileReader {
  const char             *path;
  struct StringFNStrings lines, *tokens;
  struct StringBuffer    *sb;
  hash_t                 *items;
  int                    ncroaks;
};

FileReader *FileReader_new(const char *path) {
  FileReader *self = malloc(sizeof *self);

  assert(self);
  self->tokens = malloc(sizeof *self->tokens);
  assert(self->tokens);
  self->items   = hash_new();
  self->sb      = stringbuffer_new();
  self->path    = path;
  self->ncroaks = 0;
  Dbg(self->path, %s);
  return(self);
}

void FileReader_free(FileReader *self) {
  assert(self);
  stringfn_release_strings_struct(self->lines);
  //stringbuffer_release(self->sb);
//  stringfn_release_strings_struct(self->tokens);
  free(self);
}

size_t FileReader_tokens_qty(FileReader *self){
//  return((size_t)(hash_size(FileReader_tokens(self))));
}

char *FileReader_tokens(FileReader *self){
  char *s;

  for (size_t i = 0; i < self->lines.count; i++) {
    Dbg(i, %u);
    struct StringFNStrings tokens = stringfn_split_words(stringfn_trim(self->lines.strings[i]));
    for (size_t q = 0; q < tokens.count; q++)
      if (stringfn_is_ascii(tokens.strings[q])) {
        s = strdup(tokens.strings[q]);
        stringbuffer_append_string(self->sb, s);
        stringbuffer_append_string(self->sb, " ");
      }
  }
  s = stringbuffer_to_string(self->sb);
  return(s);
}

size_t FileReader_qty(){
}
struct StringFNStrings FileReader_lines(FileReader *self){
  return((self->lines = stringfn_split_lines_and_trim(fsio_read_text_file(self->path))));
}

size_t FileReader_size(FileReader *self){
  return(fsio_file_size(self->path));
}
void FileReader_free(FileReader *self);

static FileReader *__fr_path(char *path){
  FileReader *fr = FileReader_new(path);

  return(fr);
}

void _command_test_file_reader(){
#define FR_PATH(PATH, OP, TYPE)    { do {                                                  \
                                       unsigned long ts  = timestamp();                    \
                                       FileReader    *fr = FileReader_new(PATH);           \
                                       Dbg(FileReader_ ## OP(fr), TYPE);                   \
                                       Dbg(milliseconds_to_string(timestamp() - ts), %s); \
                                     } while (0); }
  FR_PATH("/Users/rick/repos/c_darwin/dls/dls.c", size, % lu)
  FR_PATH("/Users/rick/repos/c_darwin/dls/dls.c", tokens, % s)
  FR_PATH("/etc/passwd", size, %u);
  FR_PATH("/etc/passwd", tokens, %s);
//Dbg(bytes_to_string(IO_size("/etc/passwd")), %s);
//FR_PATH("/etc/passwd",(lines),%d);
//Dbg(  FileReader_lines(io).tokens,%d);
//  VCALL(DYN(FileReader,IO,io),read);
}

static void __fr_iterate(char *line){
}

static void FileReader_IO(VSelf) {
  VSELF(FileReader);
  self->ncroaks++;
  printf("path! %s\n", self->path);
}

implExtern(IO, FileReader);

struct Frog {
  const char *name;
  int        ncroaks;
};

Frog *Frog_new(const char *name) {
  Frog *self = malloc(sizeof *self);

  assert(self);

  self->name    = name;
  self->ncroaks = 0;

  return(self);
}

void Frog_free(Frog *self) {
  assert(self);
  free(self);
}

static void Frog_croak(VSelf) {
  VSELF(Frog);
  self->ncroaks++;
  struct vn_init vn;
  vn.width  = 60;
  vn.height = 5;
  vn.pos_x  = 2;
  vn.pos_y  = 2;

  printf("%s: croak! #%d\n", self->name, self->ncroaks);
}

implExtern(Croak, Frog);

void Droid_turn_on(Droid droid) {
  printf("Turning on %s...\n", droid.vptr->name());
}

const char *C_3PO_name(void) {
  return("C-3PO");
}
implExtern(Droid, C_3PO);

const char *R2_D2_name(void) {
  return("R2-D2");
}

void R2_D2_turn_on(Droid droid) {
  Droid_turn_on(droid);
  puts("Waaaaoow!");
}

#define R2_D2_turn_on_CUSTOM    ()
implExtern(Droid, R2_D2);

#endif
