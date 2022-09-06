#pragma once
#include <assert.h>
#include <assert.h>
#include <ctype.h>
#include <Carbon/Carbon.h>
#include <inttypes.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
typedef struct {
  short  kchrID;
  Str255 KCHRname;
  short  transtable[256];
} Ascii2KeyCodeTable;
enum {
  kTableCountOffset = 256 + 2,
  kFirstTableOffset = 256 + 4,
  kTableSize        = 128
};
short AsciiToKeyCode(Ascii2KeyCodeTable *ttable, short asciiCode);
const char *convertKeyboardCode(const int keyCode);
void toKey(CGKeyCode kc, CGEventFlags flags, bool upOrDown);
