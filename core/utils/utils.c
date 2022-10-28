#pragma once
#ifndef CORE_UTILS_C
#define CORE_UTILS_C
////////////////////////////////////////////
#include "core/utils/utils.h"
////////////////////////////////////////////

////////////////////////////////////////////
static bool CORE_UTILS_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
struct whereami_report_t *core_utils_whereami_report(){
  struct whereami_report_t *r = calloc(1, sizeof(struct whereami_report_t));
  char                     *path = NULL;
  int                      length, dirname_length;

  length = wai_getExecutablePath(NULL, 0, &dirname_length);
  if (length > 0) {
    path = (char *)malloc(length + 1);
    wai_getExecutablePath(path, length, &dirname_length);
    path[length] = '\0';
    asprintf(&(r->executable), "%s", path);
    path[dirname_length] = '\0';
    asprintf(&(r->executable_directory), "%s", path);
    asprintf(&(r->executable_basename), "%s", path + dirname_length + 1);
    free(path);
    return(r);
  }
  return(NULL);
}
static void __attribute__((constructor)) __constructor__core_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_core_utils") != NULL) {
    log_debug("Enabling core_utils Debug Mode");
    CORE_UTILS_DEBUG_MODE = true;
  }
}
#pragma once
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "djbhash/src/djbhash.h"
#include "frameworks/frameworks.h"
#include "hash/hash.h"
#include "iokit-utils/iokit-utils.h"
#include "log/log.h"
#include "ms/ms.h"
#include "osx-keys/osx-keys.h"
#include "parson/parson.h"
#include "space/utils/utils.h"
#include "string-utils/string-utils.h"
#include "timestamp/timestamp.h"
#include "window/utils/utils.h"
//#include <ApplicationServices/ApplicationServices.h>
#include <bsm/libbsm.h>
#include <Carbon/Carbon.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CoreFoundation.h>
#include <errno.h>
#include <libgen.h>
#include <libproc.h>
#include <pthread.h>
#include <pwd.h>
#include <security/audit/audit_ioctl.h>
#include <signal.h>
#include <signal.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <sys/syslimits.h>
#include <time.h>
#include <unistd.h>

bool window_id_is_minimized(size_t window_id){
  return(false);

  struct window_info_t *W = get_window_id_info(window_id);
  AXUIElementRef       app = AXWindowFromCGWindow(W->window);    CFBooleanRef boolRef;
  if (AXUIElementCopyAttributeValue(app, kAXMinimizedAttribute, &boolRef) != kAXErrorSuccess) {
    log_error("Failed to copy attrs");
    return(false);
  }
  return(CFBooleanGetValue(boolRef));
}

char *get_encoding_type_name(int encoding) {
  if (encoding == kCFStringEncodingMacRoman)
    return((char *)&"macintosh");

  if (encoding == kCFStringEncodingUTF8)
    return((char *)&"UTF-8");

  if (encoding == kCFStringEncodingShiftJIS)
    return((char *)&"ShiftJIS");

  if (encoding == kCFStringEncodingISOLatin1)
    return((char *)&"Latin1");

  return((char *)&"macintosh");
}

int run_applescript(const void *text, long textLength, AEDesc *resultData) {
  ComponentInstance theComponent;
  AEDesc            scriptTextDesc;
  OSStatus          err;
  OSAID             scriptID, resultID;

  /* set up locals to a known state */
  theComponent = NULL;
  AECreateDesc(typeNull, NULL, 0, &scriptTextDesc);
  scriptID = kOSANullScript;
  resultID = kOSANullScript;

  /* open the scripting component */
  theComponent = OpenDefaultComponent(kOSAComponentType,
                                      typeAppleScript);
  if (theComponent == NULL) {
    err = paramErr; goto bail;
  }

  /* put the script text into an aedesc */
  err = AECreateDesc(typeChar, text, textLength, &scriptTextDesc);
  if (err != noErr)
    goto bail;

  /* compile the script */
  err = OSACompile(theComponent, &scriptTextDesc,
                   kOSAModeNull, &scriptID);
  if (err != noErr)
    goto bail;

  /* run the script */
  err = OSAExecute(theComponent, scriptID, kOSANullScript,
                   kOSAModeNull, &resultID);

  /* collect the results - if any */
  if (resultData != NULL) {
    AECreateDesc(typeNull, NULL, 0, resultData);
    if (err == errOSAScriptError)
      OSAScriptError(theComponent, kOSAErrorMessage,
                     typeChar, resultData);
    else if (err == noErr && resultID != kOSANullScript)
      OSADisplay(theComponent, resultID, typeChar,
                 kOSAModeNull, resultData);
  }
bail:
  AEDisposeDesc(&scriptTextDesc);
  if (scriptID != kOSANullScript)
    OSADispose(theComponent, scriptID);
  if (resultID != kOSANullScript)
    OSADispose(theComponent, resultID);
  if (theComponent != NULL)
    CloseComponent(theComponent);
  return(err);
} /* run_applescript */

////////////////////////////////////////////
#endif
