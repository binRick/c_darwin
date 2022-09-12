#pragma once
#ifndef KEYCHAIN_UTILS_H
#define KEYCHAIN_UTILS_H
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <CoreFoundation/CoreFoundation.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Security/Security.h>
#include <unistd.h>
//////////////////////////////////////
char *GetOSStatusMessage(OSStatus status);
OSStatus RemoveKeychainItem(SecKeychainRef keychain, const char *name, const char *key);
OSStatus ExistsKeychainItem(SecKeychainRef keychain, const char *name, const char *key);
OSStatus GetKeychainItem(SecKeychainRef keychain, const char *name, const char *key, void **data, size_t *len);
OSStatus SetKeychainItem(SecKeychainRef keychain, const char *name, const char *key, const void *data, size_t len);
OSStatus UpdateKeychainItem(SecKeychainRef keychain, const char *name, const char *key, const void *data, size_t len);
OSStatus OpenKeychain(SecKeychainRef keychain);
OSStatus CreateAccessWithUid(uid_t uid, SecAccessRef *ret_access);
#endif
