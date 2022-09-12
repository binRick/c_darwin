/* This file was automatically generated.  Do not edit! */
#if !defined (KEYCHAIN_UTILS_C)
char *GetOSStatusMessage(OSStatus status);
OSStatus RemoveKeychainItem(SecKeychainRef keychain, const char *name, const char *key);
OSStatus ExistsKeychainItem(SecKeychainRef keychain, const char *name, const char *key);
OSStatus GetKeychainItem(SecKeychainRef keychain, const char *name, const char *key, void **data, size_t *len);
OSStatus SetKeychainItem(SecKeychainRef keychain, const char *name, const char *key, const void *data, size_t len);
OSStatus UpdateKeychainItem(SecKeychainRef keychain, const char *name, const char *key, const void *data, size_t len);
OSStatus OpenKeychain(SecKeychainRef keychain);
OSStatus CreateAccessWithUid(uid_t uid, SecAccessRef *ret_access);
#endif
