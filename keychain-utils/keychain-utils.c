#pragma once
#ifndef KEYCHAIN_UTILS_C
#define KEYCHAIN_UTILS_C
////////////////////////////////////////////
#include "keychain-utils/keychain-utils.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "bytes/bytes.h"
#include "c_fsio/include/fsio.h"
#include "c_string_buffer/include/stringbuffer.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/vector/vector.h"
#include "log/log.h"
#include "ms/ms.h"
#include "timestamp/timestamp.h"

////////////////////////////////////////////
static bool KEYCHAIN_UTILS_DEBUG_MODE = false;
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__keychain_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_keychain_utils") != NULL) {
    log_debug("Enabling keychain-utils Debug Mode");
    KEYCHAIN_UTILS_DEBUG_MODE = true;
  }
}

OSStatus CreateAccessWithUid(uid_t uid, SecAccessRef *ret_access) {
  // make the "uid/gid" ACL subject
  // this is a CSSM_LIST_ELEMENT chain
  CSSM_ACL_PROCESS_SUBJECT_SELECTOR selector = {
    CSSM_ACL_PROCESS_SELECTOR_CURRENT_VERSION, // selector version
    CSSM_ACL_MATCH_UID,                        // set mask: match uids (only)
    uid,                                       // uid to match
    0                                          // gid (not matched here)
  };
  CSSM_LIST_ELEMENT                 subject2 = { NULL, 0, 0, { { 0, 0, 0 } } };

  subject2.Element.Word.Data   = (UInt8 *)&selector;
  subject2.Element.Word.Length = sizeof(selector);
  CSSM_LIST_ELEMENT subject1 = { &subject2, CSSM_ACL_SUBJECT_TYPE_PROCESS, CSSM_LIST_ELEMENT_WORDID, { { 0, 0, 0 } } };

  // rights granted (replace with individual list if desired)
  CSSM_ACL_AUTHORIZATION_TAG rights[] = {
    CSSM_ACL_AUTHORIZATION_ANY          // everything
  };
  // owner component (right to change ACL)
  CSSM_ACL_OWNER_PROTOTYPE   owner = {
    // TypedSubject
    { CSSM_LIST_TYPE_UNKNOWN, &subject1, &subject2 },
    // Delegate
    false
  };
  // ACL entries (any number, just one here)
  CSSM_ACL_ENTRY_INFO        acls = {
    // CSSM_ACL_ENTRY_PROTOTYPE
    {
      { CSSM_LIST_TYPE_UNKNOWN,             &subject1, &subject2 }, // TypedSubject
      false,                                                        // Delegate
      { sizeof(rights) / sizeof(rights[0]), rights },               // Authorization rights for this entry
      { { 0,                                0 },       { 0, 0    }}, // CSSM_ACL_VALIDITY_PERIOD
      ""                                                            // CSSM_STRING EntryTag
    },
    // CSSM_ACL_HANDLE
    0
  };

  return(SecAccessCreateFromOwnerAndACL(&owner, 1, &acls, ret_access));
} /* CreateAccessWithUid */

OSStatus OpenKeychain(SecKeychainRef keychain) {
  OSStatus status = SecKeychainSetPreferenceDomain(kSecPreferencesDomainSystem);

  if (status == noErr) {
    status = SecKeychainCopyDomainDefault(kSecPreferencesDomainSystem, &keychain);
  }
  return(status);
}

OSStatus UpdateKeychainItem(SecKeychainRef keychain, const char *name, const char *key, const void *data, size_t len) {
  void               *pwd    = NULL;
  UInt32             pwd_len = 0;
  SecKeychainItemRef item    = NULL;

  OSStatus           status = SecKeychainFindGenericPassword(keychain,
                                                             (UInt32)strlen(key), key,
                                                             (UInt32)strlen(name), name,
                                                             &pwd_len, &pwd,
                                                             &item);

  if (status == noErr) {    // item is found, update the value
    if ((len != pwd_len) || (bcmp(data, pwd, pwd_len) != 0)) {
      status = SecKeychainItemModifyAttributesAndData(item, NULL, len, data);
    }
  }

  if (pwd != NULL) {
    SecKeychainItemFreeContent(NULL, pwd);
    pwd = NULL;
  }
  return(status);
}

OSStatus SetKeychainItem(SecKeychainRef keychain, const char *name, const char *key, const void *data, size_t len) {
  SecKeychainItemRef item = NULL;

  OSStatus           status = UpdateKeychainItem(keychain, name, key, data, len);

  if (status == errSecItemNotFound) {
    SecAccessRef access = NULL;

    status = CreateAccessWithUid(0, &access);      // 0 for root uid
    if (status == noErr) {
      size_t                   sz      = strlen(name);
      SecKeychainAttribute     attrs[] = {
        { kSecLabelItemAttr,   (UInt32)sz,          (char *)name },
        { kSecAccountItemAttr, (UInt32)sz,          (char *)name },
        { kSecServiceItemAttr, (UInt32)strlen(key), (char *)key  }
      };
      SecKeychainAttributeList attributes = { sizeof(attrs) / sizeof(attrs[0]),
                                              attrs };

      status = SecKeychainItemCreateFromContent(
        kSecGenericPasswordItemClass,
        &attributes,
        (UInt32)len,
        data,
        keychain,
        access,
        &item);
      if (status == errSecDuplicateItem) {
        status = UpdateKeychainItem(keychain, name, key, data, len);
      }
    }

    if (access != NULL) {
      CFRelease(access);
    }
  }

  if (item != NULL) {
    CFRelease(item);
    item = NULL;
  }

  return(status);
} /* SetKeychainItem */

OSStatus GetKeychainItem(SecKeychainRef keychain, const char *name, const char *key, void **data, size_t *len) {
  void               *pwd    = NULL;
  UInt32             pwd_len = 0;
  SecKeychainItemRef item    = NULL;

  OSStatus           status = SecKeychainFindGenericPassword(keychain,
                                                             (UInt32)strlen(key), key,
                                                             (UInt32)strlen(name), name,
                                                             &pwd_len, &pwd,
                                                             &item);

  if (status == noErr) {
    *data = malloc(pwd_len);
    memcpy(*data, pwd, pwd_len);
    *len = pwd_len;
  }

  if (pwd != NULL) {
    SecKeychainItemFreeContent(NULL, pwd);
    pwd = NULL;
  }

  if (item != NULL) {
    CFRelease(item);
    item = NULL;
  }

  return(status);
}

OSStatus ExistsKeychainItem(SecKeychainRef keychain, const char *name, const char *key) {
  SecKeychainItemRef item = NULL;

  OSStatus           status = SecKeychainFindGenericPassword(keychain,
                                                             (UInt32)strlen(key), key,
                                                             (UInt32)strlen(name), name,
                                                             NULL, NULL,
                                                             &item);

  if (item != NULL) {
    CFRelease(item);
    item = NULL;
  }

  return(status);
}

OSStatus RemoveKeychainItem(SecKeychainRef keychain, const char *name, const char *key) {
  SecKeychainItemRef item = NULL;

  OSStatus           status = SecKeychainFindGenericPassword(keychain,
                                                             (UInt32)strlen(key), key,
                                                             (UInt32)strlen(name), name,
                                                             NULL, NULL,
                                                             &item);

  if (status == noErr) {
    status = SecKeychainItemDelete(item);
  }

  if (item != NULL) {
    CFRelease(item);
    item = NULL;
  }

  return(status);
}

char * GetOSStatusMessage(OSStatus status) {
  CFStringRef s = SecCopyErrorMessageString(status, NULL);
  char        *p;
  int         n;

  n = CFStringGetLength(s) * 8;
  p = malloc(n);
  CFStringGetCString(s, p, n, kCFStringEncodingUTF8);
  CFRelease(s);
  return(p);
}

SecAccessRef SCSecAccessCreateForUID(uid_t uid){
  SecAccessRef access = NULL;
  OSStatus     status;

  // make the "uid/gid" ACL subject
  // this is a CSSM_LIST_ELEMENT chain

  CSSM_ACL_PROCESS_SUBJECT_SELECTOR selector = {
    CSSM_ACL_PROCESS_SELECTOR_CURRENT_VERSION,                                  // version
    CSSM_ACL_MATCH_UID,                                                         // active fields mask: match uids (only)
    uid,                                                                        // effective user id to match
    0                                                                           // effective group id to match
  };

  CSSM_LIST_ELEMENT                 subject2 = {
    NULL,                                                                       // NextElement
    0                                                                           // WordID
    // rest is defaulted
  };

  subject2.Element.Word.Data   = (UInt8 *)&selector;
  subject2.Element.Word.Length = sizeof(selector);

  CSSM_LIST_ELEMENT subject1 = {
    &subject2,                                                                  // NextElement
    CSSM_ACL_SUBJECT_TYPE_PROCESS,                                              // WordID
    CSSM_LIST_ELEMENT_WORDID                                                    // ElementType
    // rest is defaulted
  };

  // rights granted (replace with individual list if desired)
  CSSM_ACL_AUTHORIZATION_TAG rights[] = {
    CSSM_ACL_AUTHORIZATION_ANY                                                  // everything
  };

  // owner component (right to change ACL)
  CSSM_ACL_OWNER_PROTOTYPE owner = {
    {                                                                           // TypedSubject
      CSSM_LIST_TYPE_UNKNOWN,                                                   // type of this list
      &subject1,                                                                // head of the list
      &subject2                                                                 // tail of the list
    },
    FALSE                                                                       // Delegate
  };

  // ACL entries (any number, just one here)
  CSSM_ACL_ENTRY_INFO acls[] = {
    {
      {                                                                         // EntryPublicInfo
        {                                                                       // TypedSubject
          CSSM_LIST_TYPE_UNKNOWN,                                               // type of this list
          &subject1,                                                            // head of the list
          &subject2                                                             // tail of the list
        },
        FALSE,                                                                  // Delegate
        {                                                                       // Authorization
          sizeof(rights) / sizeof(rights[0]),                                   // NumberOfAuthTags
          rights                                                                // AuthTags
        },
        {                                                                       // TimeRange
        },
        {                                                                       // EntryTag
        }
      },
      0                                                                         // EntryHandle
    }
  };

  status = SecAccessCreateFromOwnerAndACL(&owner,
                                          sizeof(acls) / sizeof(acls[0]),
                                          acls,
                                          &access);
  if (status != noErr) {
    return(NULL);
  }

  return(access);
} /* SCSecAccessCreateForUID */
/*
 * CFDataRef copyMyExecutablePath(void){
 * char      fspath[MAXPATHLEN];
 * Boolean   isBundle = FALSE;
 * Boolean   ok;
 * CFDataRef path = NULL;
 * CFURLRef  url;
 *
 * url = _CFBundleCopyMainBundleExecutableURL(&isBundle);
 * if (url == NULL) {
 *  return(NULL);
 * }
 *
 * ok = CFURLGetFileSystemRepresentation(url, TRUE, (UInt8 *)fspath, sizeof(fspath));
 * CFRelease(url);
 * if (!ok) {
 *  return(NULL);
 * }
 * fspath[sizeof(fspath) - 1] = '\0';
 *
 * if (isBundle) {
 *  const char *slash;
 *
 *  slash = strrchr(fspath, '/');
 *  if (slash != NULL) {
 *    const char *contents;
 *
 *    contents = strstr(fspath, "/Contents/MacOS/");
 *    if ((contents != NULL)
 *        && ((contents + sizeof("/Contents/MacOS/") - 1) == slash)) {
 *      path = CFDataCreate(NULL, (UInt8 *)fspath, contents - fspath);
 *      goto done;
 *    }
 *  }
 * }
 *
 * path = CFDataCreate(NULL, (UInt8 *)fspath, strlen(fspath));
 *
 * done:
 *
 * return(path);
 * }
 */

#endif
