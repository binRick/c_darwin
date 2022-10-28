#pragma once
#ifndef DLS_META_H
#define DLS_META_H
#include "interface99/interface99.h"
#include "layout/utils/utils.h"
#include "metalang99/include/metalang99.h"

struct Window;
#define FindWindow_IFACE                            \
  vfunc(struct Window *, find, const VSelf, void *) \
  vfunc(int, id, const VSelf)                       \
  vfunc(char *, name, const VSelf)
interface(FindWindow);
struct Windows {
  CFArrayRef list;
  int        qty;
  hash_t     *ids;
  hash_t     *props;
};
struct Window {
  const char *name;
  int        id;
};
#define FIND_WINDOW_STRUCT() \
  struct Window  window;     \
  struct Windows windows;
typedef struct {
  FIND_WINDOW_STRUCT()
} ByID;
typedef struct {
  FIND_WINDOW_STRUCT()
} ByName;
struct Window *ByName_find(const VSelf, void *);
struct Window *ByID_find(const VSelf, void *);
declImplExtern(FindWindow, ByID);
declImplExtern(FindWindow, ByName);

#define LAYOUT_INIT_ARGS    int width, int height, float size, int qty
#define Layout_IFACE                         \
  vfunc(char *, string, const VSelf)         \
  vfunc(bool, size, const VSelf, float size) \
  vfunc(bool, qty, const VSelf, int qty)     \
  vfunc(bool, render, const VSelf)           \
  vfunc(void, print, const VSelf)            \
  vfunc(void, init, VSelf, LAYOUT_INIT_ARGS)
interface(Layout);
typedef struct {
  struct layout_request_t *req;
  struct layout_result_t  *res;
  float                   size;
  int                     qty;
  int                     width, height;
} CommonLayout;
typedef struct {
  CommonLayout layout;
} Vertical;
typedef struct {
  CommonLayout layout;
} Horizontal;
void Horizontal_init(VSelf, LAYOUT_INIT_ARGS);
bool Horizontal_size(VSelf, float size);
bool Horizontal_render(VSelf);
void Horizontal_print(VSelf);
void Vertical_print(VSelf);
bool Vertical_render(VSelf);
char *Horizontal_string(const VSelf);
void Vertical_init(VSelf, LAYOUT_INIT_ARGS);
bool Vertical_size(VSelf, float size);
char *Vertical_string(const VSelf);
declImplExtern(Layout, Horizontal);
declImplExtern(Layout, Vertical);

#define DLS_IFACE                \
  vfunc(int, perim, const VSelf) \
  vfunc(void, scale, VSelf, int factor)
interface(DLS);
typedef struct {
  int a, b;
} Rectangle;
typedef struct {
  int a, b, c;
} Triangle;
void Triangle_scale(VSelf, int factor);
int Triangle_perim(const VSelf);
void Rectangle_scale(VSelf, int factor);
int Rectangle_perim(const VSelf);
declImplExtern(DLS, Triangle);
declImplExtern(DLS, Rectangle);

#define FileReader_IFACE    vfunc(void, read, VSelf)
interface(IO);
typedef struct FileReader FileReader;
FileReader *FileReader_new(const char *path);
char *FileReader_tokens(FileReader *self);
size_t FileReader_qty(FileReader *self);
size_t FileReader_tokens_qty(FileReader *self);
void _command_test_file_reader();
struct StringFNStrings FileReader_lines(FileReader *self);
typedef bool (^fr_iterator_b)(char *line);
static FileReader *__fr_path(char *path);
static void __fr_iterate(char *line);
size_t FileReader_size(FileReader *self);
void FileReader_free(FileReader *self);
declImplExtern(IO, FileReader);

#define Croak_IFACE    vfunc(void, croak, VSelf)
interface(Croak);
typedef struct Frog Frog;
Frog *Frog_new(const char *name);
void Frog_free(Frog *self);
declImplExtern(Croak, Frog);

#define Droid_IFACE               \
  vfunc(const char *, name, void) \
  vfuncDefault(void, turn_on, Droid droid)
interface(Droid);
typedef struct {
  char dummy;
} C_3PO;
typedef struct {
  char dummy;
} R2_D2;
Droid *turn_on(Droid droid);
void R2_D2_turn_on(Droid droid);
const char *R2_D2_name(void);
const char *C_3PO_name(void);
void Droid_turn_on(Droid droid);
declImplExtern(Droid, R2_D2);
declImplExtern(Droid, C_3PO);

#endif
