#pragma once
#include <ApplicationServices/ApplicationServices.h>
extern int CGSMainConnectionID(void);
extern CGError CGSGetConnectionPSN(int cid, ProcessSerialNumber *psn);
extern CGError CGSSetWindowAlpha(int cid, uint32_t wid, float alpha);
extern CGError CGSSetWindowListAlpha(int cid, const uint32_t *window_list, int window_count, float alpha, float duration);
extern CGError CGSSetWindowLevelForGroup(int cid, uint32_t wid, int level);
extern OSStatus CGSMoveWindowWithGroup(const int cid, const uint32_t wid, CGPoint *point);
extern CGError CGSReassociateWindowsSpacesByGeometry(int cid, CFArrayRef window_list);
extern CGError CGSGetWindowOwner(int cid, uint32_t wid, int *window_cid);
extern CGError CGSSetWindowTags(int cid, uint32_t wid, const int tags[2], size_t maxTagSize);
extern CGError CGSClearWindowTags(int cid, uint32_t wid, const int tags[2], size_t maxTagSize);
extern CGError CGSGetWindowBounds(int cid, uint32_t wid, CGRect *frame);
extern CGError CGSGetWindowTransform(int cid, uint32_t wid, CGAffineTransform *t);
extern CGError CGSSetWindowTransform(int cid, uint32_t wid, CGAffineTransform t);
extern void CGSManagedDisplaySetCurrentSpace(int cid, CFStringRef display_ref, uint64_t spid);
extern uint64_t CGSManagedDisplayGetCurrentSpace(int cid, CFStringRef display_ref);
extern CFArrayRef CGSCopyManagedDisplaySpaces(const int cid);
extern CFStringRef CGSCopyManagedDisplayForSpace(const int cid, uint64_t spid);
extern void CGSShowSpaces(int cid, CFArrayRef spaces);
extern void CGSHideSpaces(int cid, CFArrayRef spaces);
///////////////////////////////////
