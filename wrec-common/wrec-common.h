#pragma once
#include "app-utils/app-utils.h"
#include "string-utils/string-utils.h"
#include "window-utils/window-utils.h"
///////////////////////////////////////
#include "wrec-common/def.h"
///////////////////////////////////////
#include "wrec-common/qoi.h"
///////////////////////////////////////
#include "wrec-common/capture.h"
///////////////////////////////////////
#include "wrec-common/args.h"
///////////////////////////////////////
#include "wrec-common/image.h"
///////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_stringfn/include/stringfn.h"
#include "log.h/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
////////////////////////////////////////////////////////////////////////////////////////
int compare_file_time_items(struct file_time_t *e1, struct file_time_t *e2);
int compare_file_time_t(struct file_time_t *e1, struct file_time_t *e2);
//////////////////////////////////////////////////////////////////////////////////////////
void debug_resize(int WINDOW_ID, char *FILE_NAME, int RESIZE_TYPE, int RESIZE_VALUE, int ORIGINAL_WIDTH, int ORIGINAL_HEIGHT, long unsigned CAPTURE_DURATION_MS, long unsigned SAVE_DURATION_MS);
//////////////////////////////////////////////////////////////////////////////////////////
int load_pngs_create_animated_gif(const char *PATH);
char *resize_type_name(const int RESIZE_TYPE);
////////////////////////////////////////////
