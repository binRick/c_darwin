#pragma once
#define DEBUG_STDOUT    true
#include "assertf/assertf.h"
#include "dbg.h"
//#include "debug-memory/debug_memory.h"
#include "capture/capture.h"
#include "which/src/which.h"
#include <libproc.h>
#include <sys/sysctl.h>
#define STDOUT_READ_BUFFER_SIZE    1024 * 2
#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreServices/CoreServices.h>
#include <ImageIO/ImageIO.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


struct _MMBitmap {
  uint8_t *imageBuffer;        /* Pixels stored in Quad I format; */
  int32_t width;               /* Never 0, unless image is NULL. */
  int32_t height;              /* Never 0, unless image is NULL. */

  int32_t bytewidth;           /* The aligned width (width + padding). */
  uint8_t bitsPerPixel;        /* Should be either 24 or 32. */
  uint8_t bytesPerPixel;       /* For convenience; should be bitsPerPixel / 8. */
};

typedef struct _MMBitmap   MMBitmap;
typedef MMBitmap           *MMBitmapRef;


MMBitmapRef createMMBitmap(uint8_t *buffer,
                           size_t  width,
                           size_t  height,
                           size_t  bytewidth,
                           uint8_t bitsPerPixel,
                           uint8_t bytesPerPixel) {
  MMBitmapRef bitmap = malloc(sizeof(MMBitmap));

  if (bitmap == NULL) {
    return(NULL);
  }

  bitmap->imageBuffer   = buffer;
  bitmap->width         = width;
  bitmap->height        = height;
  bitmap->bytewidth     = bytewidth;
  bitmap->bitsPerPixel  = bitsPerPixel;
  bitmap->bytesPerPixel = bytesPerPixel;

  return(bitmap);
}


CFDictionaryRef window_id_to_window(const int WINDOW_ID){
  CFArrayRef      windowList;
  CFDictionaryRef window;

  windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );
  int count = 0;

  for (int i = 0; i < CFArrayGetCount(windowList); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);
    int windowId = CFDictionaryGetInt(window, kCGWindowNumber);
    if (windowId != WINDOW_ID) {
      continue;
    }
    return(window);
  }
  return(NULL);
}


bool capture_to_file_image(const int WINDOW_ID, const char *FILE_NAME){
  CFDictionaryRef W   = window_id_to_window(WINDOW_ID);
  int             WID = (int)CFDictionaryGetInt(W, kCGWindowNumber);

  assert(WID == WINDOW_ID);
  CGSize     W_SIZE = CGWindowGetSize(W);
  CGImageRef img    = CGWindowListCreateImage(
    CGRectMake(0, 0, (int)W_SIZE.height, (int)W_SIZE.width),
    kCGWindowListOptionIncludingWindow,
    WID,
    kCGWindowImageBestResolution
    );
  CFStringRef           type   = CFSTR("public.png");
  CFStringRef           file   = CFStringCreateWithCString(NULL, FILE_NAME, kCFStringEncodingMacRoman);
  CFURLRef              urlRef = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, file, kCFURLPOSIXPathStyle, false);
  CGImageDestinationRef idst   = CGImageDestinationCreateWithURL(urlRef, type, 1, NULL);

  CGImageDestinationAddImage(idst, img, NULL);
  CGImageDestinationFinalize(idst);
  return(fsio_file_exists(FILE_NAME));
}


bool capture_to_file_screencapture(const int WINDOW_ID, const char *FILE_NAME){
  char *window_id_s = malloc(32);

  sprintf(window_id_s, "%d", WINDOW_ID);
  const char           *command_line[] = {
    (char *)which("screencapture"),
    "-x",
    "-l",                          window_id_s,
    "-o",
    "-m",                          FILE_NAME,
    NULL
  };
  char                 *READ_STDOUT;
  int                  exited, result;
  struct subprocess_s  subprocess;
  char                 stdout_buffer[STDOUT_READ_BUFFER_SIZE] = { 0 };
  struct  StringBuffer *SB                                    = stringbuffer_new_with_options(STDOUT_READ_BUFFER_SIZE, true);
  size_t               bytes_read                             = 0,
                       index                                  = 0;

  result = subprocess_create(command_line, 0, &subprocess);
  assert_eq(result, 0, %d);
  do {
    bytes_read = subprocess_read_stdout(&subprocess, stdout_buffer, STDOUT_READ_BUFFER_SIZE - 1);
    stringbuffer_append_string(SB, stdout_buffer);
    index += bytes_read;
  } while (bytes_read != 0);

  result = subprocess_join(&subprocess, &exited);
  assert_eq(result, 0, %d);
  assert_eq(exited, 0, %d);

  READ_STDOUT = stringbuffer_to_string(SB);
  stringbuffer_release(SB);

  if (DEBUG_STDOUT) {
    dbg(exited, %d);
    dbg(READ_STDOUT, %s);
    dbg(FILE_NAME, %s);
    dbg(fsio_file_size(FILE_NAME), %lu);
  }

  free(window_id_s);

  return(fsio_file_exists(FILE_NAME));
} /* capture_to_file_screencapture */

