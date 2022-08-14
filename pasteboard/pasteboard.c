#include "pasteboard/pasteboard.h"
#define CLIPBOARD_DEBUG_MODE      false
#define CLIPBOARD_MAX_SIZE_KB     32
#define CLIPBOARD_CONTENT_TYPE    "public.utf8-plain-text"
static size_t MAX_PASTEBOARD_CONTENT_LENGTH = CLIPBOARD_MAX_SIZE_KB * 1024 + 1;
static PasteboardRef pasteboard_reference;

int clipwrite(char *dat) {
  CFDataRef           cfdata;
  PasteboardSyncFlags flags;

  if (pasteboard_reference == NULL) {
    return(0);
  }
//  runeseprint(rsnarf, rsnarf + nelem(rsnarf), "%s", snarf);
  if (PasteboardClear(pasteboard_reference) != noErr) {
    fprintf(stderr, "apple pasteboard clear failed\n");
    return(0);
  }
  flags = PasteboardSynchronize(pasteboard_reference);
  if ((flags & kPasteboardModified) || !(flags & kPasteboardClientIsOwner)) {
    fprintf(stderr, "apple pasteboard cannot assert ownership\n");
    return(0);
  }
/*  cfdata = CFDataCreate(kCFAllocatorDefault, (uchar *)rsnarf, runestrlen(rsnarf) * 2);
 * if (cfdata == nil) {
 *  fprintf(stderr, "apple pasteboard cfdatacreate failed\n");
 *  return(0);
 * }
 * if (PasteboardPutItemFlavor(pasteboard_reference, (PasteboardItemID)1,
 *                            CFSTR("public.utf16-plain-text"), cfdata, 0) != noErr) {
 *  fprintf(stderr, "apple pasteboard putitem failed\n");
 *  CFRelease(cfdata);
 *  return(0);
 * }
 * CFRelease(cfdata);
 */
  return(1);
}

/*
 * void Clipboard_SetText(const cc_string *value) {
 * PasteboardRef pbRef;
 * CFDataRef     cfData;
 * UInt8         str[800];
 * int           len;
 * OSStatus      err;
 *
 * pbRef = Window_GetPasteboard();
 * err   = PasteboardClear(pbRef);
 * if (err) {
 *  Logger_Abort2(err, "Clearing Pasteboard");
 * }
 * PasteboardSynchronize(pbRef);
 *
 * len    = Platform_EncodeUtf8(str, value);
 * cfData = CFDataCreate(NULL, str, len);
 * if (!cfData) {
 *  Logger_Abort("CFDataCreate() returned null pointer");
 * }
 *
 * PasteboardPutItemFlavor(pbRef, 1, FMT_UTF8, cfData, 0);
 * }
 */

char *read_clipboard(void) {
  if (CLIPBOARD_DEBUG_MODE) {
    fprintf(stderr, "clipread>\n");
  }
  CFDataRef cfdata;
  OSStatus  err = noErr;
  ItemCount nitems;
  int       i;
  char      *s;

  if (pasteboard_reference == NULL) {
    if (PasteboardCreate(kPasteboardClipboard, &pasteboard_reference) != noErr) {
      fprintf(stderr, "pasteboard pasteboard_reference create failed\n");
      return(NULL);
    }
  }

  PasteboardSynchronize(pasteboard_reference);
  if ((err = PasteboardGetItemCount(pasteboard_reference, &nitems)) != noErr) {
    fprintf(stderr, "pasteboard_reference GetItemCount failed - Error %d\n", err);
    return(NULL);
  }
  if (CLIPBOARD_DEBUG_MODE) {
    fprintf(stderr, "clipread> %lu pasteboard items\n", nitems);
  }
  for (i = 1; i <= nitems; i++) {
    PasteboardItemID itemID;
    CFArrayRef       flavorTypeArray;
    CFIndex          flavorCount;

    if ((err = PasteboardGetItemIdentifier(pasteboard_reference, i, &itemID)) != noErr) {
      fprintf(stderr, "Can't get pasteboard item identifier: %d\n", err);
      return(NULL);
    }

    if ((err = PasteboardCopyItemFlavors(pasteboard_reference, itemID, &flavorTypeArray)) != noErr) {
      fprintf(stderr, "Can't copy pasteboard flavors: %d\n", err);
      return(NULL);
    }

    flavorCount = CFArrayGetCount(flavorTypeArray);
    CFIndex flavorIndex;
    for (flavorIndex = 0; flavorIndex < flavorCount; ++flavorIndex) {
      CFStringRef flavorType;
      flavorType = (CFStringRef)CFArrayGetValueAtIndex(flavorTypeArray, flavorIndex);

      if (UTTypeConformsTo(flavorType, CFSTR(CLIPBOARD_CONTENT_TYPE))) {
        if ((err = PasteboardCopyItemFlavorData(pasteboard_reference, itemID, CFSTR(CLIPBOARD_CONTENT_TYPE), &cfdata)) != noErr) {
          fprintf(stderr, "pasteboard_reference CopyItem failed - Error %d\n", err);
          return(NULL);
        }
        CFIndex length = CFDataGetLength(cfdata);
        if ((size_t)(length) > MAX_PASTEBOARD_CONTENT_LENGTH - 1) {
          fprintf(stderr, "clipread> pasteboard contents too large. (%lu > %lu)\n", (size_t)length, MAX_PASTEBOARD_CONTENT_LENGTH - 1);
          return(NULL);
        }
        char tmp_pasteboard_content[length + 1];
        if (CLIPBOARD_DEBUG_MODE) {
          fprintf(stderr, "clipread> pasteboard item #%lu> %lub\n", flavorIndex, (size_t)length);
        }
        CFDataGetBytes(cfdata, CFRangeMake(0, length), &tmp_pasteboard_content);

        if (CLIPBOARD_DEBUG_MODE) {
          fprintf(stderr, "clipread> got bytes.....\n");
        }
        char   *dat = calloc(length + 1, sizeof(char));
        s = tmp_pasteboard_content;
        size_t copied_len = 0;
        for (int i = 0; ((i < length) && (copied_len < MAX_PASTEBOARD_CONTENT_LENGTH)); (i++, s += 1)) {
          if (*s == '\0') {
            continue;
          }
          switch (*s) {
          case '\r':
            *s = '\n';
            break;
          }
          memcpy(dat + i, s, sizeof(char));
          copied_len++;
        }
        dat[copied_len] = '\0';
        if (CLIPBOARD_DEBUG_MODE) {
          fprintf(stderr, "clipread> clipboard item #%lu> %lub|copied_len:%lu|%.50s...\n", flavorIndex, strlen(dat), copied_len, dat);
        }
        CFRelease(cfdata);
        return(dat);
      }
    }
  }
  return(0);
} /* clipread */
