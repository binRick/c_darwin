#pragma once
#ifndef FONT_UTILS_C
#define FONT_UTILS_C
////////////////////////////////////////////
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "font-utils/font-utils.h"
#include "font/font.h"
#include "log/log.h"
#include "ms/ms.h"
#include "parson/parson.h"
#include "systemprofiler/systemprofiler.h"
#include "timestamp/timestamp.h"
static bool FONT_UTILS_DEBUG_MODE = false;
typedef void (^font_parser_b)(struct font_t *font, JSON_Object *font_object);
const char  *font_file_type_extensions[FONT_FILE_TYPES_QTY + 1] = {
  [FONT_FILE_TYPE_TRUETYPE]   = "ttf",
  [FONT_FILE_TYPE_OPENTYPE]   = "otf",
  [FONT_FILE_TYPE_POSTSCRIPT] = "otf",
  [FONT_FILE_TYPE_BITMAP]     = "ttf",
};
const char  *font_file_type_names[FONT_FILE_TYPES_QTY + 1] = {
  [FONT_FILE_TYPE_TRUETYPE]   = "truetype",
  [FONT_FILE_TYPE_OPENTYPE]   = "opentype",
  [FONT_FILE_TYPE_POSTSCRIPT] = "postscript",
  [FONT_FILE_TYPE_BITMAP]     = "bitmap",
};
enum font_parser_type_t {
  FONT_PARSER_TYPE_TYPEFACES_O = 1,
  FONT_PARSER_TYPE_NAME,
  FONT_PARSER_TYPE_FILE_NAME,
  FONT_PARSER_TYPE_PATH,
  FONT_PARSER_TYPE_ENABLED,
  FONT_PARSER_TYPE_TYPE,
  FONT_PARSER_TYPE_SIZE,
  FONT_PARSER_TYPE_TYPEFACES_QTY,
  FONT_PARSER_TYPE_FAMILY,
  FONT_PARSER_TYPE_STYLE,
  FONT_PARSER_TYPE_FULLNAME,
  FONT_PARSER_TYPE_VERSION,
  FONT_PARSER_TYPE_COPY_PROTECTED,
  FONT_PARSER_TYPE_EMBEDDABLE,
  FONT_PARSER_TYPE_UNIQUE,
  FONT_PARSER_TYPE_VALID,
  FONT_PARSER_TYPE_DUPLICATE,
  FONT_PARSER_TYPE_OUTLINE,
  FONT_PARSER_TYPES_QTY,
};
struct font_parser_t {
  bool enabled;
  void (^parser)(struct font_t *font, JSON_Object *font_object);
};
static void __attribute__((constructor)) __constructor__font_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_FONT_UTILS") != NULL) {
    log_debug("Enabling font-utils Debug Mode");
    FONT_UTILS_DEBUG_MODE = true;
  }
}
static struct font_parser_t font_parsers[FONT_PARSER_TYPES_QTY] = {
  [FONT_PARSER_TYPE_TYPEFACES_O] =    { .enabled = true,
                                        .parser  = ^ void (struct font_t *font,               JSON_Object *font_object){
                                          font->typefaces_o = (json_object_has_value_of_type(font_object, "typefaces", JSONArray))
       ? json_array_get_object(json_object_dotget_array(font_object, "typefaces"), 0)
       : NULL;
                                        }, },
  [FONT_PARSER_TYPE_TYPEFACES_QTY] =  { .enabled = true,
                                        .parser  = ^ void (struct font_t *font, __attribute__((unused)) JSON_Object *font_object){
                                          font->typefaces_qty = (font->typefaces_o)
         ? json_object_get_count(font->typefaces_o)
         : 0;
                                        }, },
  [FONT_PARSER_TYPE_FILE_NAME] =      { .enabled = true,
                                        .parser  = ^ void (struct font_t *font,               JSON_Object *font_object){
                                          font->file_name = (json_object_has_value_of_type(font_object, "_name", JSONString))
       ? json_object_get_string(font_object, "_name")
       : NULL;
                                        }, },
  [FONT_PARSER_TYPE_PATH] =           { .enabled = true,
                                        .parser  = ^ void (struct font_t *font,               JSON_Object *font_object){
                                          font->path       = (json_object_has_value_of_type(font_object,"path", JSONString))
       ? json_object_get_string(font_object, "path")
       : NULL;
                                        }, },
  [FONT_PARSER_TYPE_ENABLED] =        { .enabled = true,
                                        .parser  = ^ void (struct font_t *font,               JSON_Object *font_object){
                                          font->enabled = (json_object_has_value_of_type(font_object, "enabled", JSONString))
       ? (strcmp(json_object_get_string(font_object, "enabled"), "yes") == 0) ? true : false
       : false;
                                        }, },
  [FONT_PARSER_TYPE_TYPE] =           { .enabled = true,
                                        .parser  = ^ void (struct font_t *font,               JSON_Object *font_object){
                                          font->type       = (json_object_has_value_of_type(font_object,"type", JSONString))
       ? json_object_get_string(font_object, "type")
       : NULL;
                                        }, },
  [FONT_PARSER_TYPE_SIZE] =           { .enabled = true,
                                        .parser  = ^ void (struct font_t *font, __attribute__((unused)) JSON_Object *font_object){
                                          font->size       = (font->path != NULL) ? fsio_file_size(font->path) : 0;
                                        }, },
  [FONT_PARSER_TYPE_FAMILY] =         { .enabled = true,
                                        .parser  = ^ void (struct font_t *font, __attribute__((unused)) JSON_Object *font_object){
                                          font->family   = (font->typefaces_o)
         ? (json_object_has_value_of_type(font->typefaces_o, "family", JSONString))
            ? json_object_get_string(font->typefaces_o, "family")
            : NULL
         : NULL;
                                        }, },
  [FONT_PARSER_TYPE_UNIQUE] =         { .enabled = true,
                                        .parser  = ^ void (struct font_t *font, __attribute__((unused)) JSON_Object *font_object){
                                          font->unique   = (font->typefaces_o)
         ? (json_object_has_value_of_type(font->typefaces_o, "unique", JSONString))
            ? json_object_get_string(font->typefaces_o, "unique")
            : NULL
         : NULL;
                                        }, },
  [FONT_PARSER_TYPE_NAME] =           { .enabled = true,
                                        .parser  = ^ void (struct font_t *font,                                           __attribute__((unused)) JSON_Object *font_object){
                                          font->name       = (font->typefaces_o) ? (json_object_has_value_of_type(font->typefaces_o,"_name", JSONString)) ? json_object_get_string(font->typefaces_o, "_name") : NULL : NULL;
                                        }, },
  [FONT_PARSER_TYPE_FULLNAME] =       { .enabled = true,
                                        .parser  = ^ void (struct font_t *font, __attribute__((unused)) JSON_Object *font_object){
                                          font->fullname = (font->typefaces_o)
         ? (json_object_has_value_of_type(font->typefaces_o, "fullname", JSONString))
            ? json_object_get_string(font->typefaces_o, "fullname")
            : NULL
         : NULL;
                                        }, },
  [FONT_PARSER_TYPE_STYLE] =          { .enabled = true,
                                        .parser  = ^ void (struct font_t *font,                                           __attribute__((unused)) JSON_Object *font_object){
                                          font->style     = (font->typefaces_o) ? (json_object_has_value_of_type(font->typefaces_o,"style", JSONString)) ? json_object_get_string(font->typefaces_o, "style") : NULL : NULL;
                                        }, },
  [FONT_PARSER_TYPE_VERSION] =        { .enabled = true,
                                        .parser  = ^ void (struct font_t *font,                                           __attribute__((unused)) JSON_Object *font_object){
                                          font->version = (font->typefaces_o) ? (json_object_has_value_of_type(font->typefaces_o, "version", JSONString)) ? json_object_get_string(font->typefaces_o, "version") : NULL : NULL;
                                        }, },
  [FONT_PARSER_TYPE_COPY_PROTECTED] = { .enabled = true,
                                        .parser  = ^ void (struct font_t *font, __attribute__((unused)) JSON_Object *font_object){
                                          font->copy_protected = (font->typefaces_o)
         ? (json_object_has_value_of_type(font->typefaces_o, "copy_protected", JSONString))
              ? (strcmp(json_object_get_string(font->typefaces_o, "copy_protected"), "yes"))
                  ? false : true
              : false
         : false;
                                        }, },
  [FONT_PARSER_TYPE_EMBEDDABLE] =     { .enabled = true,
                                        .parser  = ^ void (struct font_t *font, __attribute__((unused)) JSON_Object *font_object){
                                          font->embeddable = (font->typefaces_o)
         ? (json_object_has_value_of_type(font->typefaces_o, "embeddable", JSONString))
              ? (strcmp(json_object_get_string(font->typefaces_o, "embeddable"), "yes"))
                  ? false : true
              : false
         : false;
                                        }, },
  [FONT_PARSER_TYPE_DUPLICATE] =      { .enabled = true,
                                        .parser  = ^ void (struct font_t *font, __attribute__((unused)) JSON_Object *font_object){
                                          font->duplicate = (font->typefaces_o)
         ? (json_object_has_value_of_type(font->typefaces_o, "duplicate", JSONString))
              ? (strcmp(json_object_get_string(font->typefaces_o, "duplicate"), "yes"))
                  ? false : true
              : false
         : false;
                                        }, },
  [FONT_PARSER_TYPE_OUTLINE] =        { .enabled = true,
                                        .parser  = ^ void (struct font_t *font, __attribute__((unused)) JSON_Object *font_object){
                                          font->outline = (font->typefaces_o)
         ? (json_object_has_value_of_type(font->typefaces_o, "outline", JSONString))
              ? (strcmp(json_object_get_string(font->typefaces_o, "outline"), "yes"))
                  ? false : true
              : false
         : false;
                                        }, },
  [FONT_PARSER_TYPE_VALID] =          { .enabled = true,
                                        .parser  = ^ void (struct font_t *font, __attribute__((unused)) JSON_Object *font_object){
                                          font->valid     = (font->typefaces_o)
         ? (json_object_has_value_of_type(font->typefaces_o, "valid", JSONString))
              ? (strcmp(json_object_get_string(font->typefaces_o, "valid"), "yes"))
                  ? false : true
              : false
         : false;
                                        }, },
};

static void debug_font(struct font_t *font){
  fprintf(stdout,
          "\n  File Name        :   " AC_RESETALL AC_GREEN "%s" AC_RESETALL
          "\n\tName             :   %s"
          "\n\tPath             :   %s"
          "\n\tEnabled          :   %s"
          "\n\tType             :   %s"
          "\n\tSize             :   " AC_CYAN "%s" AC_RESETALL
          "\n\t# Typefaces      :   %lu"
          "\n\tFamily           :   " AC_MAGENTA "%s" AC_RESETALL
          "\n\tFullname         :   %s"
          "\n\tStyle            :   %s"
          "\n\tVersion          :   %s"
          "\n\tCopy Protected   :   %s"
          "\n\tEmbeddable       :   %s"
          "\n\tDuplicate        :   %s"
          "\n\tValid            :   %s"
          "\n\tOutline          :   %s"
          "\n\tUnique           :   %s"
          "%s",
          font->file_name,
          font->name,
          font->path,
          (font->enabled == true) ? "Yes" : "No",
          font->type,
          bytes_to_string(font->size),
          font->typefaces_qty,
          font->family,
          font->fullname,
          font->style,
          font->version,
          (font->copy_protected == true) ? "Yes" : "No",
          (font->embeddable == true) ? "Yes" : "No",
          (font->duplicate == true) ? "Yes" : "No",
          (font->valid == true) ? "Yes" : "No",
          (font->outline == true) ? "Yes" : "No",
          font->unique,
          "\n"
          );
}

static void parse_font(struct font_t *font, JSON_Object *font_object){
  for (size_t i = 0; i < FONT_PARSER_TYPES_QTY; i++) {
    if (font_parsers[i].enabled == true) {
      font_parsers[i].parser(font, font_object);
    }
  }
}
struct Vector *get_installed_fonts_v(){
  struct Vector *a   = vector_new();
  char          *out = run_system_profiler_item_subprocess("SPFontsDataType", 60 * 60);

  log_debug("%s", bytes_to_string(strlen(out)));
  JSON_Value *root_value = json_parse_string(out);

  if (json_value_get_type(root_value) == JSONObject) {
    JSON_Object *root_object = json_value_get_object(root_value);
    if (json_object_has_value_of_type(root_object, "SPFontsDataType", JSONArray)) {
      JSON_Array *fonts_array = json_object_dotget_array(root_object, "SPFontsDataType");
      size_t     fonts_qty    = json_array_get_count(fonts_array);
      for (size_t i = 0; i < fonts_qty; i++) {
        struct font_t *font = calloc(1, sizeof(struct font_t));
        parse_font(font, json_array_get_object(fonts_array, i));
        debug_font(font);
      }
    }
  }
  return(a);
}
#endif
