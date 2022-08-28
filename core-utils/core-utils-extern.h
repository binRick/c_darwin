#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>

typedef enum {
  kCoreDockOrientationIgnore = 0,
  kCoreDockOrientationTop = 1,
  kCoreDockOrientationBottom = 2,
  kCoreDockOrientationLeft = 3,
  kCoreDockOrientationRight = 4
} CoreDockOrientation;

typedef enum {
  kCoreDockPinningIgnore = 0,
  kCoreDockPinningStart = 1,
  kCoreDockPinningMiddle = 2,
  kCoreDockPinningEnd = 3
} CoreDockPinning;

typedef enum {
  kCoreDockEffectGenie = 1,
  kCoreDockEffectScale = 2,
  kCoreDockEffectSuck = 3
} CoreDockEffect;

// Tile size ranges from 0.0 to 1.0.
extern float CoreDockGetTileSize(void);
extern void CoreDockSetTileSize(float tileSize);

// If you only want to set one, use 0 for the other.
extern void CoreDockSetOrientationAndPinning(CoreDockOrientation orientation, CoreDockPinning pinning);

extern void CoreDockGetEffect(CoreDockEffect *outEffect);
extern void CoreDockSetEffect(CoreDockEffect effect);

extern Boolean CoreDockGetAutoHideEnabled(void);
extern void CoreDockSetAutoHideEnabled(Boolean flag);

extern Boolean CoreDockIsMagnificationEnabled(void);
extern void CoreDockSetMagnificationEnabled(Boolean flag);

// Magnification ranges from 0.0 to 1.0.
extern float CoreDockGetMagnificationSize(void);
extern void CoreDockSetMagnificationSize(float newSize);

extern Boolean CoreDockGetWorkspacesEnabled(void);
extern void CoreDockSetWorkspacesEnabled(Boolean); // This works, but wipes out all of the other spaces prefs. An alternative is to use the ScriptingBridge which works just fine.

extern void CoreDockGetWorkspacesCount(int *rows, int *cols);
extern void CoreDockSetWorkspacesCount(int rows, int cols);
extern AXError _AXUIElementGetWindow(AXUIElementRef ref, uint32_t *wid);
extern Boolean CoreDockGetAutoHideEnabled(void);
extern CFArrayRef CGSCopyManagedDisplaySpaces(const int cid);
extern CFArrayRef SLSCopyAssociatedWindows(int cid, uint32_t wid);
extern CFArrayRef SLSCopyManagedDisplaySpaces(int cid);
extern CFArrayRef SLSCopyManagedDisplays(int cid);
extern CFArrayRef SLSCopySpacesForWindows(int cid, int selector, CFArrayRef window_list);
extern CFArrayRef SLSCopyWindowsWithOptionsAndTags(int cid, uint32_t owner, CFArrayRef spaces, uint32_t options, uint64_t *set_tags, uint64_t *clear_tags);
extern CFArrayRef _LSCopyApplicationArrayInFrontToBackOrder(int negative_one, int one);
extern CFStringRef CGSCopyManagedDisplayForSpace(const int cid, uint64_t spid);
extern CFStringRef SLSCopyActiveMenuBarDisplayIdentifier(int cid);
extern CFStringRef SLSCopyBestManagedDisplayForPoint(int cid, CGPoint point);
extern CFStringRef SLSCopyBestManagedDisplayForRect(int cid, CGRect rect);
extern CFStringRef SLSCopyManagedDisplayForSpace(int cid, uint64_t sid);
extern CFStringRef SLSCopyManagedDisplayForWindow(int cid, uint32_t wid);
extern CFStringRef SLSSpaceCopyName(int cid, uint64_t sid);
extern CFTypeID _LSASNGetTypeID(void);
extern CFTypeRef SLSWindowQueryResultCopyWindows(CFTypeRef window_query);
extern CFTypeRef SLSWindowQueryWindows(int cid, CFArrayRef windows, int count);
extern CFUUIDRef CGDisplayCreateUUIDFromDisplayID(uint32_t did);
extern CGContextRef SLWindowContextCreate(int cid, uint32_t wid, CFDictionaryRef options);
extern CGError CGSClearWindowTags(int cid, uint32_t wid, const int tags[2], size_t maxTagSize);
extern CGError CGSGetConnectionPSN(int cid, ProcessSerialNumber *psn);
extern CGError CGSGetWindowBounds(int cid, uint32_t wid, CGRect *frame);
extern CGError CGSGetWindowOwner(int cid, uint32_t wid, int *window_cid);
extern CGError CGSGetWindowTransform(int cid, uint32_t wid, CGAffineTransform *t);
extern CGError CGSNewRegionWithRect(CGRect *rect, CFTypeRef *outRegion);
extern CGError CGSNewRegionWithRect(CGRect *rect, CFTypeRef *region);
extern CGError CGSReassociateWindowsSpacesByGeometry(int cid, CFArrayRef window_list);
extern CGError CGSSetWindowAlpha(int cid, uint32_t wid, float alpha);
extern CGError CGSSetWindowLevelForGroup(int cid, uint32_t wid, int level);
extern CGError CGSSetWindowListAlpha(int cid, const uint32_t *window_list, int window_count, float alpha, float duration);
extern CGError CGSSetWindowTags(int cid, uint32_t wid, const int tags[2], size_t maxTagSize);
extern CGError CGSSetWindowTransform(int cid, uint32_t wid, CGAffineTransform t);
extern CGError CGSWindowSetShadowProperties(int wid, CFDictionaryRef properties);
extern CGError CoreDockSendNotification(CFStringRef notification, int unknown);
extern CGError SLPSPostEventRecordTo(ProcessSerialNumber *psn, uint8_t *bytes);
extern CGError SLSClearWindowTags(int cid, uint32_t wid, uint64_t *tags, int tag_size);
extern CGError SLSConnectionGetPID(int cid, pid_t *pid);
extern CGError SLSCopyWindowProperty(int cid, uint32_t wid, CFStringRef property, CFTypeRef *value);
extern CGError SLSDisableUpdate(int cid);
extern CGError SLSGetConnectionIDForPSN(int cid, ProcessSerialNumber *psn, int *psn_cid);
extern CGError SLSGetConnectionPSN(int cid, ProcessSerialNumber *psn);
extern CGError SLSGetCurrentCursorLocation(int cid, CGPoint *point);
extern CGError SLSGetDockRectWithReason(int cid, CGRect *rect, int *reason);
extern CGError SLSGetMenuBarAutohideEnabled(int cid, int *enabled);
extern CGError SLSGetRevealedMenuBarBounds(CGRect *rect, int cid, uint64_t sid);
extern CGError SLSGetWindowAlpha(int cid, uint32_t wid, float *alpha);
extern CGError SLSGetWindowBounds(int cid, uint32_t wid, CGRect *frame);
extern CGError SLSGetWindowLevel(int cid, uint32_t wid, int *level);
extern CGError SLSGetWindowOwner(int cid, uint32_t wid, int *wcid);
extern CGError SLSMoveWindow(int cid, uint32_t wid, CGPoint *point);
extern CGError SLSNewWindow(int cid, int type, float x, float y, CFTypeRef region, uint32_t *wid);
extern CGError SLSOrderWindow(int cid, uint32_t wid, int mode, uint32_t rel_wid);
extern CGError SLSOrderWindow(int cid, uint32_t wid, int mode, uint32_t relativeToWID);
extern CGError SLSProcessAssignToAllSpaces(int cid, pid_t pid);
extern CGError SLSProcessAssignToSpace(int cid, pid_t pid, uint64_t sid);
extern CGError SLSReenableUpdate(int cid);
extern CGError SLSReleaseWindow(int cid, uint32_t wid);
extern CGError SLSRequestNotificationsForWindows(int cid, uint32_t *window_list, int window_count);
extern CGError SLSSetMouseEventEnableFlags(int cid, uint32_t wid, bool shouldEnable);
extern CGError SLSSetWindowLevel(int cid, uint32_t wid, int level);
extern CGError SLSSetWindowOpacity(int cid, uint32_t wid, bool isOpaque);
extern CGError SLSSetWindowOpacity(int cid, uint32_t wid, bool opaque);
extern CGError SLSSetWindowResolution(int cid, uint32_t wid, double res);
extern CGError SLSSetWindowResolution(int cid, uint32_t wid, double resolution);
extern CGError SLSSetWindowShape(int cid, uint32_t wid, float x_offset, float y_offset, CFTypeRef shape);
extern CGError SLSSetWindowTags(int cid, uint32_t wid, uint64_t *tags, int tag_size);
extern CGError SLSWindowIteratorAdvance(CFTypeRef iterator);
extern CGError SLSWindowSetShadowProperties(uint32_t wid, CFDictionaryRef options);
extern CGError _SLPSSetFrontProcessWithOptions(ProcessSerialNumber *psn, uint32_t wid, uint32_t mode);
extern OSStatus CGSMoveWindowWithGroup(const int cid, const uint32_t wid, CGPoint *point);
extern OSStatus SLSFindWindowByGeometry(int cid, int zero, int one, int zero_again, CGPoint *screen_point, CGPoint *window_point, uint32_t *wid, int *wcid);
extern OSStatus _SLPSGetFrontProcess(ProcessSerialNumber *psn);
extern bool SLSManagedDisplayIsAnimating(int cid, CFStringRef uuid);
extern bool g_mission_control_active;
extern bool g_verbose;
extern int CGSMainConnectionID(void);
extern int SLSGetSpaceManagementMode(int cid);
extern int SLSMainConnectionID(void);
extern int SLSSpaceGetType(int cid, uint64_t sid);
extern int g_connection;
extern uint32_t SLSWindowIteratorGetParentID(CFTypeRef iterator);
extern uint32_t SLSWindowIteratorGetWindowID(CFTypeRef iterator);
extern uint64_t CGSManagedDisplayGetCurrentSpace(int cid, CFStringRef display_ref);
extern uint64_t SLSManagedDisplayGetCurrentSpace(int cid, CFStringRef uuid);
extern uint64_t SLSWindowIteratorGetAttributes(CFTypeRef iterator);
extern uint64_t SLSWindowIteratorGetTags(CFTypeRef iterator);
extern void CGSHideSpaces(int cid, CFArrayRef spaces);
extern void CGSManagedDisplaySetCurrentSpace(int cid, CFStringRef display_ref, uint64_t spid);
extern void CGSShowSpaces(int cid, CFArrayRef spaces);
extern void CoreDockGetOrientationAndPinning(int *orientation, int *pinning);
extern void SLSMoveWindowsToManagedSpace(int cid, CFArrayRef window_list, uint64_t sid);
extern void _LSASNExtractHighAndLowParts(const void *asn, uint32_t *high, uint32_t *low);


#define CGSConnectionID CGSConnection
#define CGSWindowID CGSWindow
#define CGSDefaultConnection _CGSDefaultConnection()


  typedef int CGSConnection;
  typedef int CGSWindow;
  typedef int CGSWorkspace;
  typedef int CGSValue;

#pragma mark Listing Windows
  /* Get the default connection for the current process. */
  extern CGSConnection _CGSDefaultConnection(void);

  // Disable/Enable Screen Updates
  extern CGError CGSDisableUpdate(CGSConnection cid);
  extern CGError CGSReenableUpdate(CGSConnection cid);

#pragma mark Listing Windows

  // Get window counts and lists.
  extern CGError CGSGetWindowCount(const CGSConnection cid, CGSConnection targetCID, int* outCount);
  extern CGError CGSGetWindowList(const CGSConnection cid, CGSConnection targetCID, int count, int* list, int* outCount);

  // Get on-screen window counts and lists.
  extern CGError CGSGetOnScreenWindowCount(const CGSConnection cid, CGSConnection targetCID, int* outCount);
  extern CGError CGSGetOnScreenWindowList(const CGSConnection cid, CGSConnection targetCID, int count, int* list, int* outCount);

  // Per-workspace window counts and lists.
  extern CGError CGSGetWorkspaceWindowCount(const CGSConnection cid, CGSWorkspace workspaceNumber, int *outCount);
  extern CGError CGSGetWorkspaceWindowList(const CGSConnection cid, CGSWorkspace workspaceNumber, int count, int* list, int* outCount);

#pragma mark Window Manipulation

  // Window Level
  extern CGError CGSGetWindowLevel(const CGSConnection cid, CGSWindow wid, CGWindowLevel *level);
  extern CGError CGSSetWindowLevel(const CGSConnection cid, CGSWindow wid, CGWindowLevel level);

  // Window ordering
  typedef enum _CGSWindowOrderingMode {
    kCGSOrderAbove                =  1, // Window is ordered above target.
    kCGSOrderBelow                = -1, // Window is ordered below target.
    kCGSOrderOut                  =  0  // Window is removed from the on-screen window list.
  } CGSWindowOrderingMode;

  extern CGError CGSOrderWindow(const CGSConnection cid, const CGSWindow wid, CGSWindowOrderingMode place, CGSWindow relativeToWindowID /* can be NULL */);
  extern CGError CGSWindowIsOrderedIn(const CGSConnection cid, const CGSWindow wid, Boolean *result);

  extern CGError CGSUncoverWindow(const CGSConnection cid, const CGSWindow wid);
  extern CGError CGSFlushWindow(const CGSConnection cid, const CGSWindow wid, int unknown /* 0 works */ );

  // Position
  extern CGError CGSGetScreenRectForWindow(const CGSConnection cid, CGSWindow wid, CGRect *outRect);
  extern CGError CGSMoveWindow(const CGSConnection cid, const CGSWindow wid, CGPoint *point);
  extern CGError CGSSetWindowTransforms(const CGSConnection cid, CGSWindow *wids, CGAffineTransform *transform, int n);

  // Alpha
  extern CGError CGSGetWindowAlpha(const CGSConnection cid, const CGSWindow wid, float* alpha);

  // Brightness
  extern CGError CGSSetWindowListBrightness(const CGSConnection cid, CGSWindow *wids, float *brightness, int count);

  // Workspace
  extern CGError CGSMoveWorkspaceWindows(const CGSConnection connection, CGSWorkspace toWorkspace, CGSWorkspace fromWorkspace);
  extern CGError CGSMoveWorkspaceWindowList(const CGSConnection connection, CGSWindow *wids, int count, CGSWorkspace toWorkspace);

  // Shadow
  extern CGError CGSSetWindowShadowAndRimParameters(const CGSConnection cid, CGSWindow wid, float standardDeviation, float density, int offsetX, int offsetY, unsigned int flags);
  extern CGError CGSGetWindowShadowAndRimParameters(const CGSConnection cid, CGSWindow wid, float* standardDeviation, float* density, int *offsetX, int *offsetY, unsigned int *flags);

  // Properties
  extern CGError CGSGetWindowProperty(const CGSConnection cid, CGSWindow wid, CGSValue key, CGSValue *outValue);
  extern CGError CGSSetWindowProperty(const CGSConnection cid, CGSWindow wid, CGSValue key, CGSValue *outValue);

  // Owner
  extern CGError CGSConnectionGetPID(const CGSConnection cid, pid_t *pid, const CGSConnection ownerCid);

#pragma mark Window Tags

  typedef enum {
    CGSTagNone          = 0,        // No tags
    CGSTagExposeFade    = 0x0002,    // Fade out when Expose activates.
    CGSTagNoShadow      = 0x0008,    // No window shadow.
    CGSTagTransparent   = 0x0200,   // Transparent to mouse clicks.
    CGSTagSticky        = 0x0800,    // Appears on all workspaces.
  } CGSWindowTag;

  // thirtyTwo must = 32 for some reason.
  // tags is a pointer to an array of ints (size 2?). First entry holds window tags.
  extern CGError CGSGetWindowTags(const CGSConnection cid, const CGSWindow wid, CGSWindowTag *tags, int thirtyTwo);
  extern CGError CGSGetWindowEventMask(const CGSConnection cid, const CGSWindow wid, uint32_t *mask);
  extern CGError CGSSetWindowEventMask(const CGSConnection cid, const CGSWindow wid, uint32_t mask);

# pragma mark Window Warping

  typedef struct {
    CGPoint local;
    CGPoint global;
  } CGPointWarp;

  extern CGError CGSSetWindowWarp(const CGSConnection cid, const CGSWindow wid, int w, int h, CGPointWarp mesh[h][w]);

# pragma mark Window Core Image Filters

  typedef void *CGSWindowFilterRef;
  extern CGError CGSNewCIFilterByName(CGSConnection cid, CFStringRef filterName, CGSWindowFilterRef *outFilter);
  extern CGError CGSAddWindowFilter(CGSConnection cid, CGSWindowID wid, CGSWindowFilterRef filter, int flags);
  extern CGError CGSRemoveWindowFilter(CGSConnection cid, CGSWindowID wid, CGSWindowFilterRef filter);
  extern CGError CGSReleaseCIFilter(CGSConnection cid, CGSWindowFilterRef filter);
  extern CGError CGSSetCIFilterValuesFromDictionary(CGSConnection cid, CGSWindowFilterRef filter, CFDictionaryRef filterValues);

#pragma mark Transitions

  typedef enum {
    CGSNone = 0,          // No transition effect.
    CGSFade,              // Cross-fade.
    CGSZoom,              // Zoom/fade towards us.
    CGSReveal,            // Reveal new desktop under old.
    CGSSlide,              // Slide old out and new in.
    CGSWarpFade,          // Warp old and fade out revealing new.
    CGSSwap,              // Swap desktops over graphically.
    CGSCube,              // The well-known cube effect.
    CGSWarpSwitch,        // Warp old, switch and un-warp.
    CGSFlip,              // Flip over
    CGSTransparentBackgroundMask = (1<<7) // OR this with any other type to get a transparent background
  } CGSTransitionType;

  typedef enum {
    CGSDown,              // Old desktop moves down.
    CGSLeft,              // Old desktop moves left.
    CGSRight,              // Old desktop moves right.
    CGSInRight,            // CGSSwap: Old desktop moves into screen, new comes from right.
    CGSBottomLeft = 5,    // CGSSwap: Old desktop moves to bl, new comes from tr.
    CGSBottomRight,        // CGSSwap: Old desktop to br, New from tl.
    CGSDownTopRight,      // CGSSwap: Old desktop moves down, new from tr.
    CGSUp,                // Old desktop moves up.
    CGSTopLeft,            // Old desktop moves tl.
    CGSTopRight,          // CGSSwap: old to tr. new from bl.
    CGSUpBottomRight,      // CGSSwap: old desktop up, new from br.
    CGSInBottom,          // CGSSwap: old in, new from bottom.
    CGSLeftBottomRight,    // CGSSwap: old one moves left, new from br.
    CGSRightBottomLeft,    // CGSSwap: old one moves right, new from bl.
    CGSInBottomRight,      // CGSSwap: onl one in, new from br.
    CGSInOut              // CGSSwap: old in, new out.
  } CGSTransitionOption;

  typedef struct {
    uint32_t unknown1;
    CGSTransitionType type;
    CGSTransitionOption option;
    CGSWindow wid;      /* Can be 0 for full-screen */
    float *backColour;  /* Null for black otherwise pointer to 3 float array with RGB value */
  } CGSTransitionSpec;

  extern CGError CGSNewTransition(const CGSConnection cid, const CGSTransitionSpec* spec, int *pTransitionHandle);
  extern CGError CGSInvokeTransition(const CGSConnection cid, int transitionHandle, float duration);
  extern CGError CGSReleaseTransition(const CGSConnection cid, int transitionHandle);

#pragma mark Workspaces

  extern CGError CGSGetWorkspace(const CGSConnection cid, CGSWorkspace *workspace);
  extern CGError CGSGetWindowWorkspace(const CGSConnection cid, const CGSWindow wid, CGSWorkspace *workspace);
  extern CGError CGSSetWorkspace(const CGSConnection cid, CGSWorkspace workspace);
  extern CGError CGSSetWorkspaceWithTransition(const CGSConnection cid, CGSWorkspace workspace, CGSTransitionType transition, CGSTransitionOption subtype, float time);

  typedef enum {
    CGSScreenResolutionChangedEvent = 100,
    CGSConnectionNotifyEventUnknown2 = 101,
    CGSConnectionNotifyEventUnknown3 = 102,
    CGSConnectionNotifyEventUnknown4 = 103,
    CGSClientEnterFullscreen = 106,
    CGSClientExitFullscreen = 107,
    CGSConnectionNotifyEventUnknown7 = 750,
    CGSConnectionNotifyEventUnknown8 = 751,
    CGSWorkspaceConfigurationDisabledEvent = 761, // Seems to occur when objects are removed (rows/columns), or disabled
    CGSWorkspaceConfigurationEnabledEvent = 762,  // Seems to occur when objects are added (rows/columns), or enabled
    CGSConnectionNotifyEventUnknown9 = 763,
    CGSConnectionNotifyEventUnknown10 = 764,
    CGSConnectionNotifyEventUnknown11 = 806,
    CGSConnectionNotifyEventUnknown12 = 807,
    CGSConnectionNotifyEventUnknown13 = 1201,  // Seems to occur when applications are launched/quit. Is this a connection being created/destroyed by the application to the window server?
    CGSWorkspaceChangedEvent = 1401,
    CGSConnectionNotifyEventUnknown14 = 1409,
    CGSConnectionNotifyEventUnknown15 = 1410,
    CGSConnectionNotifyEventUnknown16 = 1411,
    CGSConnectionNotifyEventUnknown17 = 1412,
    CGSConnectionNotifyEventUnknown18 = 1500,
    CGSConnectionNotifyEventUnknown19 = 1501,
    CGSConnectionNotifyEventUnknown20 = 1700
  } CGSConnectionNotifyEvent;

  /* Prototype for the Spaces change notification callback.
   *
   * data1 -- returns whatever value is passed to data1 parameter in CGSRegisterConnectionNotifyProc
   * data2 -- indeterminate (always a large negative integer; seems to be limited to a small set of values)
   * data3 -- indeterminate (always returns the number '4' for me)
   * userParameter -- returns whatever value is passed to userParameter in CGSRegisterConnectionNotifyProc
   */

  typedef void (*CGConnectionNotifyProc)(int data1, int data2, int data3, void* userParameter);

  /* Register a callback function to receive notifications about when
   the current Space is changing.
   *
   * cid -- Current connection
   * function -- A pointer to the intended callback function (must be in C; cannot be an Objective-C selector)
   * event -- indeterminate (this is hard-coded to 0x579 in Spaces.menu...perhpas some kind of event filter code?) -- use CGSWorkspaceChangedEvent in this for now
   * userParameter -- pointer to user-defined auxiliary information structure; passed directly to callback proc
   */

  // For spaces notifications: CGSRegisterConnectionNotifyProc(_CGSDefaultConnection(), spacesCallback, 1401, (void*)userInfo);

  extern CGError CGSRegisterConnectionNotifyProc(const CGSConnection cid, CGConnectionNotifyProc function, CGSConnectionNotifyEvent event, void* userParameter);

  extern CGError CGSRemoveConnectionNotifyProc(const CGSConnection cid, CGConnectionNotifyProc function, CGSConnectionNotifyEvent event, void* userParameter);

# pragma mark Miscellaneous

  // Regions
  typedef void *CGSRegionRef;
  extern CGError CGSNewEmptyRegion(CGSRegionRef *outRegion);
  extern CGError CGSReleaseRegion(CGSRegionRef region);

  // Creating Windows
  extern CGError CGSNewWindowWithOpaqueShape(CGSConnection cid, int always2, float x, float y, CGSRegionRef shape, CGSRegionRef opaqueShape, int unknown1, void *unknownPtr, int always32, CGSWindowID *outWID);
  extern CGError CGSReleaseWindow(CGSConnection cid, CGSWindowID wid);
  extern CGContextRef CGWindowContextCreate(CGSConnection cid, CGSWindowID wid, void *unknown);

  // Values
  extern int CGSIntegerValue(CGSValue intVal);
  extern void *CGSReleaseGenericObj(void*);

  // Deprecated in 10.5
  extern CGSValue CGSCreateCStringNoCopy(const char *str); //Normal CFStrings will work
  extern CGSValue CGSCreateCString(const char* str);
  extern char* CGSCStringValue(CGSValue string);

#pragma mark Debugging

  // These all create files called /tmp/WindowServer + suffix
  #define kCGSDumpWindowInfo 0x80000001 // .winfo.out
  #define kCGSDumpConnectionInfo 0x80000002 // .cinfo.out
  #define kCGSDumpKeyInfo 0x8000000e // .keyinfo.out
  #define kCGSDumpSurfaceInfo 0x80000010 // .sinfo.out
  #define kCGSDumpGLInfo 0x80000013 // .glinfo.out
  #define kCGSDumpShadowInfo 0x80000014 //.shinfo.out
  #define kCGSDumpStoragesAndCachesInfo 0x80000015 // .scinfo.out
  #define kCGSDumpWindowPlistInfo 0x80000017 // .winfo.plist
  // Other flags:
  #define kCGSDebugOptionNormal 0 // Reset everything
  #define kCGSFlashScreenUpdates 4 // This is probably what the checkbox in Quartz Debug calls internally
  typedef unsigned long CGSDebugOptions;

  extern void CGSSetDebugOptions(CGSDebugOptions options);

  // Missing functions

  //CGSIntersectRegionWithRect
  //CGSSetWindowTransformsAtPlacement
  //CGSSetWindowListGlobalClipShape
  //extern CGError CGSWindowAddRectToDirtyShape(const CGSConnection cid, const CGSWindow wid, CGRect *rect);

#ifdef __cplusplus
}
#endif
