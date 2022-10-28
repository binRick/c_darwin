#pragma once
#ifndef KEYBOARD_UTILS_C
#define KEYBOARD_UTILS_C
////////////////////////////////////////////
#include <Carbon/Carbon.h>
////////////////////////////////////////////
#include "keyboard/utils/utils.h"
#include "osx-keys/osx-keys.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "frameworks/frameworks.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"
#include <Carbon/Carbon.h>
#include <stdio.h>
#include <string.h>
#define TYPOMATIC_RATE    100000
////////////////////////////////////////////
static bool keyboard_utils_DEBUG_MODE = false;

///////////////////////////////////////////////////////////////////////
short AsciiToKeyCode(Ascii2KeyCodeTable *ttable, short asciiCode) {
  if (asciiCode >= 0 && asciiCode <= 255)
    return(ttable->transtable[asciiCode]);
  else
    return(false);
}

/*
 * char KeyCodeToAscii(short virtualKeyCode) {
 * unsigned long state;
 * long keyTrans;
 * char charCode;
 * Ptr kchr;
 * state = 0;
 * kchr = (Ptr)GetScriptVariable(smCurrentScript, smKCHRCache);
 * keyTrans = KeyTranslate(kchr, virtualKeyCode, &state);
 * charCode = keyTrans;
 * if (!charCode) {
 *  charCode = (keyTrans >> 16);
 * }
 * return charCode;
 * }
 * OSStatus InitAscii2KeyCodeTable(Ascii2KeyCodeTable* ttable) {
 * unsigned char* theCurrentKCHR, *ithKeyTable;
 * short count, i, j, resID;
 * Handle theKCHRRsrc;
 * ResType rType;
 *
 * // set up our table to all minus ones
 * for (i = 0; i < 256; i++)
 *        ttable->transtable[i] = -1;
 *
 * // find the current kchr resource ID
 * ttable->kchrID = (short)GetScriptVariable(smCurrentScript, smScriptKeys);
 *
 * // get the current KCHR resource
 * theKCHRRsrc = GetResource('KCHR', ttable->kchrID);
 * if (theKCHRRsrc == NULL)
 *        return resNotFound;
 * GetResInfo(theKCHRRsrc, &resID, &rType, ttable->KCHRname);
 *
 * // dereference the resource
 * theCurrentKCHR = (unsigned char *)(*theKCHRRsrc);
 *
 * // get the count from the resource
 * count = *(short*)(theCurrentKCHR + kTableCountOffset);
 *
 * // build inverse table by merging all key tables
 * for (i = 0; i < count; i++) {
 *        ithKeyTable = theCurrentKCHR + kFirstTableOffset + (i * kTableSize);
 *        for (j = 0; j < kTableSize; j++) {
 *                if (ttable->transtable[ ithKeyTable[j] ] == -1)
 *                        ttable->transtable[ ithKeyTable[j] ] = j;
 *        }
 * }
 *
 * return noErr;
 * }
 * void typeString(char *str) {
 * Ascii2KeyCodeTable ttable;
 * InitAscii2KeyCodeTable(&ttable);
 *
 * int i;
 * char chr;
 * for (i = 0; i < strlen(str); i++) {
 *  chr = str[i];
 *  UInt32 kc = AsciiToKeyCode(&ttable, (int)chr);
 *
 *  // if the kc doesn't match the char when we convert it back, assume Shift
 *  CGEventFlags flags = NULL;
 *  if (KeyCodeToAscii(kc) != chr) {
 *    flags = kCGEventFlagMaskShift;
 *  }
 *  keyHit((CGKeyCode)kc, flags);
 * }
 * }
 */
void keyHit(CGKeyCode kc, CGEventFlags flags) {
  keyPress(kc, flags);
  usleep(TYPOMATIC_RATE);
  keyRelease(kc, flags);
}

void keyPress(CGKeyCode kc, CGEventFlags flags) {
  toKey(kc, flags, true);
}

void keyRelease(CGKeyCode kc, CGEventFlags flags) {
  toKey(kc, flags, false);
}

static void __attribute__((constructor)) __constructor__keyboard_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_keyboard_utils") != NULL) {
    log_debug("Enabling keyboard-utils Debug Mode");
    keyboard_utils_DEBUG_MODE = true;
  }
}
////////////////////////////////////////////
#endif
