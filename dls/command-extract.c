#pragma once
#ifndef DLS_EXTRACT_COMMANDS_C
#define DLS_EXTRACT_COMMANDS_C
#include "core/core.h"
#include "dls/dls.h"
#include "tesseract/utils/utils.h"
#ifndef DEP_LIBSPINNER
#define DEP_LIBSPINNER
#include "libspinner/spinner.h"
#endif

//extern struct args_t *args;
void _command_extract_image(void){
  initialize_args(args);
  debug_dls_arguments();
  if (!fsio_file_exists(args->input_file)) {
    log_error("Invalid Input Image File");
    exit(EXIT_FAILURE);
  }
  VipsImage                         *v;
  unsigned long                     started = timestamp();
  bool                              do_print, match, print_all;
  struct StringFNStrings            words, split;
  struct Vector                     *items;
  struct tesseract_extract_result_t *r;
  char                              *match_src = NULL, *s = NULL;
  size_t                            matched_qty = 0;
  char                              *extension;
  {
    split = stringfn_split(fsio_file_extension(args->input_file), '.');
    if (split.count < 2) {
      log_error("File %s has no extension", args->input_file);
      exit(EXIT_FAILURE);
    }
    extension = stringfn_to_uppercase(split.strings[split.count - 1]);
    if (!(v = vips_image_new_from_file(args->input_file, "access", VIPS_ACCESS_SEQUENTIAL, NULL))) {
      log_error("Failed to decode file %s", args->input_file);
      exit(EXIT_FAILURE);
    }
    spinner_t *spinner;
    spinner        = spinner_new(1);
    spinner->delay = 100000;
    asprintf(&s,
             AC_RESETALL AC_YELLOW "Analyzing" AC_RESETALL
             " "
             AC_CYAN "%s" AC_RESETALL
             " "
             AC_RED "%s" AC_RESETALL
             " "
             AC_YELLOW "File" AC_RESETALL
             " ",
             bytes_to_string(fsio_file_size(args->input_file)),
             extension
             );
    spinner->prefix = s;
    spinner->suffix = AC_RESETALL "" AC_RESETALL;
    spinner_start(spinner);
    items = tesseract_extract_memory(
      fsio_read_binary_file(args->input_file), fsio_file_size(args->input_file),
      RIL_TEXTLINE
      );
    spinner_stop(spinner);
    spinner_free(spinner);
    free(s);
  }
  for (size_t i = 0; i < vector_size(items); i++) {
    match     = false;
    r         = (struct tesseract_extract_result_t *)vector_get(items, i);
    words     = stringfn_split_words(stringfn_trim(r->text));
    print_all = (args->search_words_qty == 0 && args->globs_qty == 0 && !args->quiet_mode);
    for (int q = 0; !match && q < words.count; q++)
      for (int qq = 0; !match && qq < args->search_words_qty; qq++)
        match = (
          (strcmp(stringfn_to_lowercase(args->search_words[qq]), words.strings[q]) == 0 && (match_src = args->search_words[qq]))
          || (strcmp(stringfn_to_lowercase(args->search_words), strip_non_alpha(words.strings[q])) == 0 && (match_src = args->search_words[qq]))
          );
    for (int qq = 0; !match && qq < args->globs_qty; qq++)
      match = (
        (wildcardcmp(args->globs[qq], r->text) && (match_src = args->globs[qq]))
        ||
        (wildcardcmp(args->globs[qq], strip_non_alpha(r->text)) && (match_src = args->globs[qq]))
        );
    if (match)
      matched_qty++;
    if (match || print_all)
      fprintf(stdout,
              "%lu"
              "/"
              "%lu"
              ">"
              " "
              "["
              AC_RESETALL AC_GREEN AC_BOLD AC_REVERSED "%s"AC_RESETALL
              "]"
              " "
              "@"
              AC_MAGENTA "%d"AC_RESETALL
              "x"
              AC_MAGENTA "%d"AC_RESETALL
              " "
              "["
              AC_YELLOW "%d"AC_RESETALL
              "x"
              AC_YELLOW "%d"AC_RESETALL
              "]"
              "\n"AC_RESETALL AC_YELLOW "%s" AC_RESETALL "\n"
              ,
              i + 1, vector_size(items),
              match_src,
              r->x, r->y,
              r->width, r->height,
              stringfn_trim(r->text)
              );
  }
  fprintf(stderr, "Matched %lu/%lu Results from %s %dx%d %s File in %s\n",
          matched_qty,
          vector_size(items),
          bytes_to_string(fsio_file_size(args->input_file)),
          vips_image_get_width(v), vips_image_get_height(v),
          extension,
          milliseconds_to_string(timestamp() - started)
          );
  stringfn_release_strings_struct(split);
  stringfn_release_strings_struct(words);
  vector_release(items);
  free(r);
  g_object_unref(v);
  exit(EXIT_SUCCESS);
} /* _command_extract_image */

void _command_extract_dev(void){
  log_info("dev");
  exit(EXIT_SUCCESS);
}
#endif
