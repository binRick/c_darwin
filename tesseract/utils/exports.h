#pragma once
#ifndef TESSERACT_UTILS_EXPORTS_H
#define TESSERACT_UTILS_EXPORTS_H
#include "tesseract/utils/utils.h"
struct Vector *tesseract_extract_items(struct Vector *ids_v, size_t concurrency);
struct tesseract_extract_result_t *tesseract_find_item_matching_word_locations(size_t id, struct Vector *words);
struct Vector *tesseract_extract_text(size_t id);
void tesseract_extract_symbols(size_t id);
struct Vector *tesseract_extract_memory(unsigned char *img_data, size_t img_data_len, unsigned long MODE);
struct Vector *get_security_words_v();
bool parse_tesseract_extraction_results(struct tesseract_extract_result_t *r);
void report_tesseract_extraction_results(struct tesseract_extract_result_t *r);
bool tesseract_security_preferences_logic(int space_id);

#endif
