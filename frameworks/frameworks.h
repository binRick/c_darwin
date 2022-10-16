#pragma once
#ifndef FRAMEWORKS_H
#define FRAMEWORKS_H
//////////////////////////////////////
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
//////////////////////////////////////
#define g_connection                              CGSMainConnectionID()
#define ASCII_ENCODING                            kCFStringEncodingASCII
#define UTF8_ENCODING                             kCFStringEncodingUTF8
#define MAX_DISPLAYS                              8
#define DEBUG_MODE                                false
#define LAYER_BELOW                               kCGBackstopMenuLevelKey
#define LAYER_NORMAL                              kCGNormalWindowLevelKey
#define LAYER_ABOVE                               kCGFloatingWindowLevelKey
#define kCPSUserGenerated                         0x200
#define kCGSFloatingWindowTagBit                  (1 << 1)
#define kCGSFollowsUserTagBit                     (1 << 2)
#define kCGSHiddenTagBit                          (1 << 8)
#define kCGSStickyTagBit                          (1 << 13)
#define kCGSWindowOwnerFollowsForegroundTagBit    (1 << 27)
#define kCGSOnAllWorkspacesTagBit                 (1 << 11)
#define kCGSSuperStickyTagBit                     (1 << 45)
#define CGSConnectionID                           CGSConnection
#define CGSWindowID                               CGSWindow
#define CGSDefaultConnection                      _CGSDefaultConnection()
#define CGSDefaultConnection                      _CGSDefaultConnection()
static CGPoint g_nirvana = { -9999, -9999 };
static const char *bool_str[]  = { "off", "on" };
static const char *layer_str[] = {
  [0]            = "",
  [LAYER_BELOW]  = "below",
  [LAYER_NORMAL] = "normal",
  [LAYER_ABOVE]  = "above"
};
typedef int                   CGSSessionID;
typedef size_t                CGSTileID;
typedef CFTypeRef             CGSAnimationRef;
typedef CFTypeRef             CGSWindowBackdropRef;
typedef struct CGSWarpPoint   CGSWarpPoint;
typedef unsigned int          CGSWorkspaceID;
typedef int                   CGSSurfaceID;
typedef int                   CGSConnectionID;
typedef unsigned long         CGSByteCount;
typedef unsigned short        CGSEventRecordVersion;
typedef unsigned long long    CGSEventRecordTime; /* nanosecond timer */
typedef unsigned long         CGSEventFlag;
typedef unsigned long         CGSError;
typedef void (*CGSConnectionDeathNotificationProc)(CGSConnectionID cid);
typedef enum {
  CGSCursorArrow        = 0,
  CGSCursorIBeam        = 1,
  CGSCursorIBeamXOR     = 2,
  CGSCursorAlias        = 3,
  CGSCursorCopy         = 4,
  CGSCursorMove         = 5,
  CGSCursorArrowContext = 6,
  CGSCursorWait         = 7,
  CGSCursorEmpty        = 8,
} CGSCursorID;
typedef enum {
  // full keyboard access hotkeys
  kCGSHotKeyToggleFullKeyboardAccess    = 12,
  kCGSHotKeyFocusMenubar = 7,
  kCGSHotKeyFocusDock = 8,
  kCGSHotKeyFocusNextGlobalWindow       = 9,
  kCGSHotKeyFocusToolbar = 10,
  kCGSHotKeyFocusFloatingWindow         = 11,
  kCGSHotKeyFocusApplicationWindow      = 27,
  kCGSHotKeyFocusNextControl            = 13,
  kCGSHotKeyFocusDrawer = 51,
  kCGSHotKeyFocusStatusItems            = 57,

  // screenshot hotkeys
  kCGSHotKeyScreenshot = 28,
  kCGSHotKeyScreenshotToClipboard       = 29,
  kCGSHotKeyScreenshotRegion            = 30,
  kCGSHotKeyScreenshotRegionToClipboard = 31,

  // universal access
  kCGSHotKeyToggleZoom                   = 15,
  kCGSHotKeyZoomOut                      = 19,
  kCGSHotKeyZoomIn                       = 17,
  kCGSHotKeyZoomToggleSmoothing          = 23,
  kCGSHotKeyIncreaseContrast             = 25,
  kCGSHotKeyDecreaseContrast             = 26,
  kCGSHotKeyInvertScreen                 = 21,
  kCGSHotKeyToggleVoiceOver              = 59,

  // Dock
  kCGSHotKeyToggleDockAutohide           = 52,
  kCGSHotKeyExposeAllWindows             = 32,
  kCGSHotKeyExposeAllWindowsSlow         = 34,
  kCGSHotKeyExposeApplicationWindows     = 33,
  kCGSHotKeyExposeApplicationWindowsSlow = 35,
  kCGSHotKeyExposeDesktop                = 36,
  kCGSHotKeyExposeDesktopsSlow           = 37,
  kCGSHotKeyDashboard                    = 62,
  kCGSHotKeyDashboardSlow                = 63,

  // spaces (Leopard and later)
  kCGSHotKeySpaces                       = 75,
  kCGSHotKeySpacesSlow                   = 76,
  // 77 - fn F7 (disabled)
  // 78 - ⇧fn F7 (disabled)
  kCGSHotKeySpaceLeft                    = 79,
  kCGSHotKeySpaceLeftSlow                = 80,
  kCGSHotKeySpaceRight                   = 81,
  kCGSHotKeySpaceRightSlow               = 82,
  kCGSHotKeySpaceDown                    = 83,
  kCGSHotKeySpaceDownSlow                = 84,
  kCGSHotKeySpaceUp                      = 85,
  kCGSHotKeySpaceUpSlow                  = 86,

  // input
  kCGSHotKeyToggleCharacterPallette      = 50,
  kCGSHotKeySelectPreviousInputSource    = 60,
  kCGSHotKeySelectNextInputSource        = 61,

  // Spotlight
  kCGSHotKeySpotlightSearchField         = 64,
  kCGSHotKeySpotlightWindow              = 65,

  kCGSHotKeyToggleFrontRow               = 73,
  kCGSHotKeyLookUpWordInDictionary       = 70,
  kCGSHotKeyHelp = 98,

  // displays - not verified
  kCGSHotKeyDecreaseDisplayBrightness    = 53,
  kCGSHotKeyIncreaseDisplayBrightness    = 54,
} CGSSymbolicHotKey;

/// The possible operating modes of a hot key.
typedef enum {
  /// All hot keys are enabled app-wide.
  kCGSGlobalHotKeyEnable  = 0,
  /// All hot keys are disabled app-wide.
  kCGSGlobalHotKeyDisable = 1,
  /// Hot keys are disabled app-wide, but exceptions are made for Accessibility.
  kCGSGlobalHotKeyDisableAllButUniversalAccess = 2,
} CGSGlobalHotKeyOperatingMode;

/// Options representing device-independent bits found in event modifier flags:
typedef enum : unsigned int {
  /// Set if Caps Lock key is pressed.
  kCGSAlphaShiftKeyMask = 1 << 16,
  /// Set if Shift key is pressed.
  kCGSShiftKeyMask = 1 << 17,
  /// Set if Control key is pressed.
  kCGSControlKeyMask = 1 << 18,
  /// Set if Option or Alternate key is pressed.
  kCGSAlternateKeyMask = 1 << 19,
  /// Set if Command key is pressed.
  kCGSCommandKeyMask = 1 << 20,
  /// Set if any key in the numeric keypad is pressed.
  kCGSNumericPadKeyMask = 1 << 21,
  /// Set if the Help key is pressed.
  kCGSHelpKeyMask = 1 << 22,
  /// Set if any function key is pressed.
  kCGSFunctionKeyMask = 1 << 23,
  /// Used to retrieve only the device-independent modifier flags, allowing applications to mask
  /// off the device-dependent modifier flags, including event coalescing information.
  kCGSDeviceIndependentModifierFlagsMask = 0xffff0000U
} CGSModifierFlags;
typedef enum : unsigned int {
  kCGSDisplayWillReconfigure                   = 100,
  kCGSDisplayDidReconfigure                    = 101,
  kCGSDisplayWillSleep                         = 102,
  kCGSDisplayDidWake                           = 103,
  kCGSDisplayIsCaptured                        = 106,
  kCGSDisplayIsReleased                        = 107,
  kCGSDisplayAllDisplaysReleased               = 108,
  kCGSDisplayHardwareChanged                   = 111,
  kCGSDisplayDidReconfigure2                   = 115,
  kCGSDisplayFullScreenAppRunning              = 116,
  kCGSDisplayFullScreenAppDone                 = 117,
  kCGSDisplayReconfigureHappened               = 118,
  kCGSDisplayColorProfileChanged               = 119,
  kCGSDisplayZoomStateChanged                  = 120,
  kCGSDisplayAcceleratorChanged                = 121,
  kCGSDebugOptionsChangedNotification          = 200,
  kCGSDebugPrintResourcesNotification          = 203,
  kCGSDebugPrintResourcesMemoryNotification    = 205,
  kCGSDebugPrintResourcesContextNotification   = 206,
  kCGSDebugPrintResourcesImageNotification     = 208,
  kCGSServerConnDirtyScreenNotification        = 300,
  kCGSServerLoginNotification                  = 301,
  kCGSServerShutdownNotification               = 302,
  kCGSServerUserPreferencesLoadedNotification  = 303,
  kCGSServerUpdateDisplayNotification          = 304,
  kCGSServerCAContextDidCommitNotification     = 305,
  kCGSServerUpdateDisplayCompletedNotification = 306,

  kCPXForegroundProcessSwitched                    = 400,
  kCPXSpecialKeyPressed                            = 401,
  kCPXForegroundProcessSwitchRequestedButRedundant = 402,

  kCGSSpecialKeyEventNotification = 700,

  kCGSEventNotificationNullEvent         = 710,
  kCGSEventNotificationLeftMouseDown     = 711,
  kCGSEventNotificationLeftMouseUp       = 712,
  kCGSEventNotificationRightMouseDown    = 713,
  kCGSEventNotificationRightMouseUp      = 714,
  kCGSEventNotificationMouseMoved        = 715,
  kCGSEventNotificationLeftMouseDragged  = 716,
  kCGSEventNotificationRightMouseDragged = 717,
  kCGSEventNotificationMouseEntered      = 718,
  kCGSEventNotificationMouseExited       = 719,

  kCGSEventNotificationKeyDown                   = 720,
  kCGSEventNotificationKeyUp                     = 721,
  kCGSEventNotificationFlagsChanged              = 722,
  kCGSEventNotificationKitDefined                = 723,
  kCGSEventNotificationSystemDefined             = 724,
  kCGSEventNotificationApplicationDefined        = 725,
  kCGSEventNotificationTimer                     = 726,
  kCGSEventNotificationCursorUpdate              = 727,
  kCGSEventNotificationSuspend                   = 729,
  kCGSEventNotificationResume                    = 730,
  kCGSEventNotificationNotification              = 731,
  kCGSEventNotificationScrollWheel               = 732,
  kCGSEventNotificationTabletPointer             = 733,
  kCGSEventNotificationTabletProximity           = 734,
  kCGSEventNotificationOtherMouseDown            = 735,
  kCGSEventNotificationOtherMouseUp              = 736,
  kCGSEventNotificationOtherMouseDragged         = 737,
  kCGSEventNotificationZoom                      = 738,
  kCGSEventNotificationAppIsUnresponsive         = 750,
  kCGSEventNotificationAppIsNoLongerUnresponsive = 751,

  kCGSEventSecureTextInputIsActive = 752,
  kCGSEventSecureTextInputIsOff    = 753,

  kCGSEventNotificationSymbolicHotKeyChanged                        = 760,
  kCGSEventNotificationSymbolicHotKeyDisabled                       = 761,
  kCGSEventNotificationSymbolicHotKeyEnabled                        = 762,
  kCGSEventNotificationHotKeysGloballyDisabled                      = 763,
  kCGSEventNotificationHotKeysGloballyEnabled                       = 764,
  kCGSEventNotificationHotKeysExceptUniversalAccessGloballyDisabled = 765,
  kCGSEventNotificationHotKeysExceptUniversalAccessGloballyEnabled  = 766,

  kCGSWindowIsObscured           = 800,
  kCGSWindowIsUnobscured         = 801,
  kCGSWindowIsOrderedIn          = 802,
  kCGSWindowIsOrderedOut         = 803,
  kCGSWindowIsTerminated         = 804,
  kCGSWindowIsChangingScreens    = 805,
  kCGSWindowDidMove              = 806,
  kCGSWindowDidResize            = 807,
  kCGSWindowDidChangeOrder       = 808,
  kCGSWindowGeometryDidChange    = 809,
  kCGSWindowMonitorDataPending   = 810,
  kCGSWindowDidCreate            = 811,
  kCGSWindowRightsGrantOffered   = 812,
  kCGSWindowRightsGrantCompleted = 813,
  kCGSWindowRecordForTermination = 814,
  kCGSWindowIsVisible            = 815,
  kCGSWindowIsInvisible          = 816,

  kCGSLikelyUnbalancedDisableUpdateNotification = 902,

  kCGSConnectionWindowsBecameVisible       = 904,
  kCGSConnectionWindowsBecameOccluded      = 905,
  kCGSConnectionWindowModificationsStarted = 906,
  kCGSConnectionWindowModificationsStopped = 907,

  kCGSWindowBecameVisible  = 912,
  kCGSWindowBecameOccluded = 913,

  kCGSServerWindowDidCreate      = 1000,
  kCGSServerWindowWillTerminate  = 1001,
  kCGSServerWindowOrderDidChange = 1002,
  kCGSServerWindowDidTerminate   = 1003,

  kCGSWindowWasMovedByDockEvent         = 1205,
  kCGSWindowWasResizedByDockEvent       = 1207,
  kCGSWindowDidBecomeManagedByDockEvent = 1208,

  kCGSServerMenuBarCreated             = 1300,
  kCGSServerHidBackstopMenuBar         = 1301,
  kCGSServerShowBackstopMenuBar        = 1302,
  kCGSServerMenuBarDrawingStyleChanged = 1303,
  kCGSServerPersistentAppsRegistered   = 1304,
  kCGSServerPersistentCheckinComplete  = 1305,

  kCGSPackagesWorkspacesDisabled    = 1306,
  kCGSPackagesWorkspacesEnabled     = 1307,
  kCGSPackagesStatusBarSpaceChanged = 1308,

  kCGSWorkspaceWillChange                                     = 1400,
  kCGSWorkspaceDidChange                                      = 1401,
  kCGSWorkspaceWindowIsViewable                               = 1402,
  kCGSWorkspaceWindowIsNotViewable                            = 1403,
  kCGSWorkspaceWindowDidMove                                  = 1404,
  kCGSWorkspacePrefsDidChange                                 = 1405,
  kCGSWorkspacesWindowDragDidStart                            = 1411,
  kCGSWorkspacesWindowDragDidEnd                              = 1412,
  kCGSWorkspacesWindowDragWillEnd                             = 1413,
  kCGSWorkspacesShowSpaceForProcess                           = 1414,
  kCGSWorkspacesWindowDidOrderInOnNonCurrentManagedSpacesOnly = 1415,
  kCGSWorkspacesWindowDidOrderOutOnNonCurrentManagedSpaces    = 1416,

  kCGSessionConsoleConnect        = 1500,
  kCGSessionConsoleDisconnect     = 1501,
  kCGSessionRemoteConnect         = 1502,
  kCGSessionRemoteDisconnect      = 1503,
  kCGSessionLoggedOn              = 1504,
  kCGSessionLoggedOff             = 1505,
  kCGSessionConsoleWillDisconnect = 1506,
  kCGXWillCreateSession           = 1550,
  kCGXDidCreateSession            = 1551,
  kCGXWillDestroySession          = 1552,
  kCGXDidDestroySession           = 1553,
  kCGXWorkspaceConnected          = 1554,
  kCGXSessionReleased             = 1555,

  kCGSTransitionDidFinish = 1700,

  kCGXServerDisplayHardwareWillReset     = 1800,
  kCGXServerDesktopShapeChanged          = 1801,
  kCGXServerDisplayConfigurationChanged  = 1802,
  kCGXServerDisplayAcceleratorOffline    = 1803,
  kCGXServerDisplayAcceleratorDeactivate = 1804,
} CGSEventType;
typedef struct _CGSEventRecord {
  CGSEventRecordVersion major;          /*0x0*/
  CGSEventRecordVersion minor;          /*0x2*/
  CGSByteCount          length;         /*0x4*/ /* Length of complete event record */
  CGSEventType          type;           /*0x8*/ /* An event type from above */
  CGPoint               location;       /*0x10*/ /* Base coordinates (global), from upper-left */
  CGPoint               windowLocation; /*0x20*/ /* Coordinates relative to window */
  CGSEventRecordTime    time;           /*0x30*/ /* nanoseconds since startup */
  CGSEventFlag          flags;          /* key state flags */
  CGWindowID            window;         /* window number of assigned window */
  CGSConnectionID       connection;     /* connection the event came from */
  struct __CGEventSourceData {
    int                source;
    unsigned int       sourceUID;
    unsigned int       sourceGID;
    unsigned int       flags;
    unsigned long long userData;
    unsigned int       sourceState;
    unsigned short     localEventSuppressionInterval;
    unsigned char      suppressionIntervalFlags;
    unsigned char      remoteMouseDragFlags;
    unsigned long long serviceID;
  } eventSource;
  struct _CGEventProcess {
    int          pid;
    unsigned int psnHi;
    unsigned int psnLo;
    unsigned int targetID;
    unsigned int flags;
  }              eventProcess;
  NXEventData    eventData;
  SInt32         _padding[4];
  void           *ioEventData;
  unsigned short _field16;
  unsigned short _field17;
  struct _CGSEventAppendix {
    unsigned short windowHeight;
    unsigned short mainDisplayHeight;
    unsigned short *unicodePayload;
    unsigned int   eventOwner;
    unsigned char  passedThrough;
  }            *appendix;
  unsigned int _field18;
  bool         passedThrough;
  CFDataRef    data;
} CGSEventRecord;
typedef void (*CGSNotifyProcPtr)(CGSEventType type, void *data, unsigned int dataLength, void *userData);
typedef CFTypeRef CGSRegionEnumeratorRef;
typedef enum {
  CGSDisplayQueryMirrorStatus = 9,
} CGSDisplayQuery;

typedef struct {
  uint32_t mode;
  uint32_t flags;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t dc2[42];
  uint16_t dc3;
  uint16_t freq;
  uint8_t  dc4[16];
  CGFloat  scale;
} CGSDisplayModeDescription;
typedef int      CGSDisplayMode;

typedef void     *CGSNotificationData;
typedef void     *CGSNotificationArg;
typedef int      CGSTransitionID;
typedef uint64_t CGSSpace;
typedef enum _CGSSpaceSelector {
  kCGSSpaceCurrent = 5,
  kCGSSpaceOther,
  kCGSSpaceAll
} CGSSpaceSelector;
typedef size_t CGSSpaceID;
/// Representations of the possible types of spaces the system can create.
typedef enum {
  /// User-created desktop spaces.
  CGSSpaceTypeUser       = 0,
  /// Fullscreen spaces.
  CGSSpaceTypeFullscreen = 1,
  /// System spaces e.g. Dashboard.
  CGSSpaceTypeSystem     = 2,
} CGSSpaceType;

/// Flags that can be applied to queries for spaces.
typedef enum {
  CGSSpaceIncludesCurrent  = 1 << 0,
  CGSSpaceIncludesOthers   = 1 << 1,
  CGSSpaceIncludesUser     = 1 << 2,

  CGSSpaceVisible          = 1 << 16,

  kCGSCurrentSpaceMask     = CGSSpaceIncludesUser | CGSSpaceIncludesCurrent,
  kCGSOtherSpacesMask      = CGSSpaceIncludesOthers | CGSSpaceIncludesCurrent,
  kCGSAllSpacesMask        = CGSSpaceIncludesUser | CGSSpaceIncludesOthers | CGSSpaceIncludesCurrent,
  KCGSAllVisibleSpacesMask = CGSSpaceVisible | kCGSAllSpacesMask,
} CGSSpaceMask;

typedef enum {
  /// Each display manages a single contiguous space.
  kCGSPackagesSpaceManagementModeNone       = 0,
  /// Each display manages a separate stack of spaces.
  kCGSPackagesSpaceManagementModePerDesktop = 1,
} CGSSpaceManagementMode;
typedef void          *CGSWindowFilterRef;
typedef int           CGSWindow;
typedef int           CGSWorkspace;
typedef int           CGSValue;
typedef void (*CGConnectionNotifyProc)(int data1, int data2, int data3, void *userParameter);
typedef void          *CGSRegionRef;
typedef unsigned long CGSDebugOptions;
enum dock_orientation_t {
  DOCK_ORIENTATION_TOP = 1,
  DOCK_ORIENTATION_BOTTOM,
  DOCK_ORIENTATION_LEFT,
  DOCK_ORIENTATION_RIGHT,
  DOCK_ORIENTATIONS_QTY,
};
typedef enum {
  kCoreDockOrientationIgnore = 0,
  kCoreDockOrientationTop    = 1,
  kCoreDockOrientationBottom = 2,
  kCoreDockOrientationLeft   = 3,
  kCoreDockOrientationRight  = 4
} CoreDockOrientation;

typedef enum {
  kCoreDockPinningIgnore = 0,
  kCoreDockPinningStart  = 1,
  kCoreDockPinningMiddle = 2,
  kCoreDockPinningEnd    = 3
} CoreDockPinning;

typedef enum {
  kCoreDockEffectGenie = 1,
  kCoreDockEffectScale = 2,
  kCoreDockEffectSuck  = 3
} CoreDockEffect;
typedef enum {
  CGSTagNone        = 0,            // No tags
  CGSTagExposeFade  = 0x0002,       // Fade out when Expose activates.
  CGSTagNoShadow    = 0x0008,       // No window shadow.
  CGSTagTransparent = 0x0200,       // Transparent to mouse clicks.
  CGSTagSticky      = 0x0800,       // Appears on all workspaces.
} CGSWindowTag;
typedef enum _CGSWindowOrderingMode {
  kCGSOrderAbove = 1,                   // Window is ordered above target.
  kCGSOrderBelow = -1,                  // Window is ordered below target.
  kCGSOrderOut   = 0                    // Window is removed from the on-screen window list.
} CGSWindowOrderingMode;

typedef struct {
  CGPoint local;
  CGPoint global;
} CGPointWarp;

typedef enum {
  CGSNone = 0,                            // No transition effect.
  CGSFade,                                // Cross-fade.
  CGSZoom,                                // Zoom/fade towards us.
  CGSReveal,                              // Reveal new desktop under old.
  CGSSlide,                               // Slide old out and new in.
  CGSWarpFade,                            // Warp old and fade out revealing new.
  CGSSwap,                                // Swap desktops over graphically.
  CGSCube,                                // The well-known cube effect.
  CGSWarpSwitch,                          // Warp old, switch and un-warp.
  CGSFlip,                                // Flip over
  CGSTransparentBackgroundMask = (1 << 7) // OR this with any other type to get a transparent background
} CGSTransitionType;

typedef enum {
  CGSDown,                // Old desktop moves down.
  CGSLeft,                // Old desktop moves left.
  CGSRight,               // Old desktop moves right.
  CGSInRight,             // CGSSwap: Old desktop moves into screen, new comes from right.
  CGSBottomLeft = 5,      // CGSSwap: Old desktop moves to bl, new comes from tr.
  CGSBottomRight,         // CGSSwap: Old desktop to br, New from tl.
  CGSDownTopRight,        // CGSSwap: Old desktop moves down, new from tr.
  CGSUp,                  // Old desktop moves up.
  CGSTopLeft,             // Old desktop moves tl.
  CGSTopRight,            // CGSSwap: old to tr. new from bl.
  CGSUpBottomRight,       // CGSSwap: old desktop up, new from br.
  CGSInBottom,            // CGSSwap: old in, new from bottom.
  CGSLeftBottomRight,     // CGSSwap: old one moves left, new from br.
  CGSRightBottomLeft,     // CGSSwap: old one moves right, new from bl.
  CGSInBottomRight,       // CGSSwap: onl one in, new from br.
  CGSInOut                // CGSSwap: old in, new out.
} CGSTransitionOption;

typedef struct {
  uint32_t            unknown1;
  CGSTransitionType   type;
  CGSTransitionOption option;
  CGSWindow           wid;         /* Can be 0 for full-screen */
  float               *backColour; /* Null for black otherwise pointer to 3 float array with RGB value */
} CGSTransitionSpec;
typedef enum {
  kCGSSharingNone,
  kCGSSharingReadOnly,
  kCGSSharingReadWrite
} CGSSharingState;

typedef enum {
  kCGSBackingNonRetianed,
  kCGSBackingRetained,
  kCGSBackingBuffered,
} CGSBackingType;

typedef enum {
  CGSWindowSaveWeightingDontReuse,
  CGSWindowSaveWeightingTopLeft,
  CGSWindowSaveWeightingTopRight,
  CGSWindowSaveWeightingBottomLeft,
  CGSWindowSaveWeightingBottomRight,
  CGSWindowSaveWeightingClip,
} CGSWindowSaveWeighting;
typedef enum {
  CGSScreenResolutionChangedEvent        = 100,
  CGSConnectionNotifyEventUnknown2       = 101,
  CGSConnectionNotifyEventUnknown3       = 102,
  CGSConnectionNotifyEventUnknown4       = 103,
  CGSClientEnterFullscreen               = 106,
  CGSClientExitFullscreen = 107,
  CGSConnectionNotifyEventUnknown7       = 750,
  CGSConnectionNotifyEventUnknown8       = 751,
  CGSWorkspaceConfigurationDisabledEvent = 761,   // Seems to occur when objects are removed (rows/columns), or disabled
  CGSWorkspaceConfigurationEnabledEvent  = 762,   // Seems to occur when objects are added (rows/columns), or enabled
  CGSConnectionNotifyEventUnknown9       = 763,
  CGSConnectionNotifyEventUnknown10      = 764,
  CGSConnectionNotifyEventUnknown11      = 806,
  CGSConnectionNotifyEventUnknown12      = 807,
  CGSConnectionNotifyEventUnknown13      = 1201, // Seems to occur when applications are launched/quit. Is this a connection being created/destroyed by the application to the window server?
  CGSWorkspaceChangedEvent               = 1401,
  CGSConnectionNotifyEventUnknown14      = 1409,
  CGSConnectionNotifyEventUnknown15      = 1410,
  CGSConnectionNotifyEventUnknown16      = 1411,
  CGSConnectionNotifyEventUnknown17      = 1412,
  CGSConnectionNotifyEventUnknown18      = 1500,
  CGSConnectionNotifyEventUnknown19      = 1501,
  CGSConnectionNotifyEventUnknown20      = 1700
} CGSConnectionNotifyEvent;

extern AXError _AXUIElementGetWindow(AXUIElementRef ref, uint32_t *wid);
extern Boolean CoreDockGetAutoHideEnabled(void);
extern Boolean CoreDockGetWorkspacesEnabled(void);
extern Boolean CoreDockIsMagnificationEnabled(void);
extern Boolean HIWindowIsOnActiveSpace(WindowRef inWindow);
extern CFArrayRef CGSCopyManagedDisplaySpaces(const int cid);
extern CFArrayRef SLSCopyAssociatedWindows(int cid, uint32_t wid);
extern CFArrayRef SLSCopyManagedDisplaySpaces(int cid);
extern CFArrayRef SLSCopyManagedDisplays(int cid);
extern CFArrayRef SLSCopySpacesForWindows(int cid, int selector, CFArrayRef window_list);
extern CFArrayRef SLSCopyWindowsWithOptionsAndTags(int cid, uint32_t owner, CFArrayRef spaces, uint32_t options, uint64_t *set_tags, uint64_t *clear_tags);
extern CFArrayRef SLSHWCaptureSpace(int64_t cid, int64_t sid, int64_t flags);
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
extern CGContextRef CGWindowContextCreate(CGSConnection cid, CGSWindowID wid, void *unknown);
extern CGContextRef SLWindowContextCreate(int cid, uint32_t wid, CFDictionaryRef options);
extern CGError CGSAddWindowFilter(CGSConnection cid, CGSWindowID wid, CGSWindowFilterRef filter, int flags);
extern CGError CGSClearWindowTags(int cid, uint32_t wid, const int tags[2], size_t maxTagSize);
extern CGError CGSConnectionGetPID(const CGSConnection cid, pid_t *pid, const CGSConnection ownerCid);
extern CGError CGSDisableUpdate(CGSConnection cid);
extern CGError CGSFlushWindow(const CGSConnection cid, const CGSWindow wid, int unknown /* 0 works */);
extern CGError CGSGetConnectionPSN(int cid, ProcessSerialNumber *psn);
extern CGError CGSGetOnScreenWindowCount(const CGSConnection cid, CGSConnection targetCID, int *outCount);
extern CGError CGSGetOnScreenWindowList(const CGSConnection cid, CGSConnection targetCID, int count, int *list, int *outCount);
extern CGError CGSGetScreenRectForWindow(const CGSConnection cid, CGSWindow wid, CGRect *outRect);
extern CGError CGSGetWindowAlpha(const CGSConnection cid, const CGSWindow wid, float *alpha);
extern CGError CGSGetWindowBounds(int cid, uint32_t wid, CGRect *frame);
extern CGError CGSGetWindowCount(const CGSConnection cid, CGSConnection targetCID, int *outCount);
extern CGError CGSGetWindowEventMask(const CGSConnection cid, const CGSWindow wid, uint32_t *mask);
extern CGError CGSGetWindowLevel(const CGSConnection cid, CGSWindow wid, CGWindowLevel *level);
extern CGError CGSGetWindowList(const CGSConnection cid, CGSConnection targetCID, int count, int *list, int *outCount);
extern CGError CGSGetWindowOwner(int cid, uint32_t wid, int *window_cid);
extern CGError CGSGetWindowProperty(const CGSConnection cid, CGSWindow wid, CGSValue key, CGSValue *outValue);
extern CGError CGSGetWindowShadowAndRimParameters(const CGSConnection cid, CGSWindow wid, float *standardDeviation, float *density, int *offsetX, int *offsetY, unsigned int *flags);
extern CGError CGSGetWindowTags(const CGSConnection cid, const CGSWindow wid, CGSWindowTag *tags, int thirtyTwo);
extern CGError CGSGetWindowTransform(int cid, uint32_t wid, CGAffineTransform *t);
extern CGError CGSGetWindowWorkspace(const CGSConnection cid, const CGSWindow wid, CGSWorkspace *workspace);
extern CGError CGSGetWorkspaceWindowCount(const CGSConnection cid, CGSWorkspace workspaceNumber, int *outCount);
extern CGError CGSGetWorkspaceWindowList(const CGSConnection cid, CGSWorkspace workspaceNumber, int count, int *list, int *outCount);
extern CGError CGSInvokeTransition(const CGSConnection cid, int transitionHandle, float duration);
extern CGError CGSMoveWindow(const CGSConnection cid, const CGSWindow wid, CGPoint *point);
extern CGError CGSMoveWorkspaceWindowList(const CGSConnection connection, CGSWindow *wids, int count, CGSWorkspace toWorkspace);
extern CGError CGSMoveWorkspaceWindows(const CGSConnection connection, CGSWorkspace toWorkspace, CGSWorkspace fromWorkspace);
extern CGError CGSNewCIFilterByName(CGSConnection cid, CFStringRef filterName, CGSWindowFilterRef *outFilter);
extern CGError CGSNewEmptyRegion(CGSRegionRef *outRegion);
extern CGError CGSNewRegionWithRect(CGRect *rect, CFTypeRef *outRegion);
extern CGError CGSNewRegionWithRect(CGRect *rect, CFTypeRef *region);
extern CGError CGSNewTransition(const CGSConnection cid, const CGSTransitionSpec *spec, int *pTransitionHandle);
extern CGError CGSNewWindowWithOpaqueShape(CGSConnection cid, int always2, float x, float y, CGSRegionRef shape, CGSRegionRef opaqueShape, int unknown1, void *unknownPtr, int always32, CGSWindowID *outWID);
extern CGError CGSOrderWindow(const CGSConnection cid, const CGSWindow wid, CGSWindowOrderingMode place, CGSWindow relativeToWindowID /* can be NULL */);
extern CGError CGSReassociateWindowsSpacesByGeometry(int cid, CFArrayRef window_list);
extern CGError CGSReenableUpdate(CGSConnection cid);
extern CGError CGSRegisterConnectionNotifyProc(const CGSConnection cid, CGConnectionNotifyProc function, CGSConnectionNotifyEvent event, void *userParameter);
extern CGError CGSReleaseCIFilter(CGSConnection cid, CGSWindowFilterRef filter);
extern CGError CGSReleaseRegion(CGSRegionRef region);
extern CGError CGSReleaseTransition(const CGSConnection cid, int transitionHandle);
extern CGError CGSReleaseWindow(CGSConnection cid, CGSWindowID wid);
extern CGError CGSRemoveConnectionNotifyProc(const CGSConnection cid, CGConnectionNotifyProc function, CGSConnectionNotifyEvent event, void *userParameter);
extern CGError CGSRemoveWindowFilter(CGSConnection cid, CGSWindowID wid, CGSWindowFilterRef filter);
extern CGError CGSSetCIFilterValuesFromDictionary(CGSConnection cid, CGSWindowFilterRef filter, CFDictionaryRef filterValues);
extern CGError CGSSetWindowAlpha(int cid, uint32_t wid, float alpha);
extern CGError CGSSetWindowEventMask(const CGSConnection cid, const CGSWindow wid, uint32_t mask);
extern CGError CGSSetWindowLevel(const CGSConnection cid, CGSWindow wid, CGWindowLevel level);
extern CGError CGSSetWindowLevelForGroup(int cid, uint32_t wid, int level);
extern CGError CGSSetWindowListAlpha(int cid, const uint32_t *window_list, int window_count, float alpha, float duration);
extern CGError CGSSetWindowListBrightness(const CGSConnection cid, CGSWindow *wids, float *brightness, int count);
extern CGError CGSSetWindowProperty(const CGSConnection cid, CGSWindow wid, CGSValue key, CGSValue *outValue);
extern CGError CGSSetWindowShadowAndRimParameters(const CGSConnection cid, CGSWindow wid, float standardDeviation, float density, int offsetX, int offsetY, unsigned int flags);
extern CGError CGSSetWindowTags(int cid, uint32_t wid, const int tags[2], size_t maxTagSize);
extern CGError CGSSetWindowTransform(int cid, uint32_t wid, CGAffineTransform t);
extern CGError CGSSetWindowTransforms(const CGSConnection cid, CGSWindow *wids, CGAffineTransform *transform, int n);
extern CGError CGSSetWindowWarp(const CGSConnection cid, const CGSWindow wid, int w, int h, CGPointWarp mesh[h][w]);
extern CGError CGSSetWorkspace(const CGSConnection cid, CGSWorkspace workspace);
extern CGError CGSSetWorkspaceWithTransition(const CGSConnection cid, CGSWorkspace workspace, CGSTransitionType transition, CGSTransitionOption subtype, float time);
extern CGError CGSUncoverWindow(const CGSConnection cid, const CGSWindow wid);
extern CGError CGSWindowIsOrderedIn(const CGSConnection cid, const CGSWindow wid, Boolean *result);
extern CGError CGSWindowSetShadowProperties(int wid, CFDictionaryRef properties);
extern CGError CoreDockSendNotification(CFStringRef notification, int unknown);
extern CGError SLPSPostEventRecordTo(ProcessSerialNumber *psn, uint8_t *bytes);
extern CGError SLSAddActivationRegion(uint32_t cid, uint32_t wid, CFTypeRef region);
extern CGError SLSAddSurface(int cid, uint32_t wid, uint32_t *outSID);
extern CGError SLSAddTrackingRect(uint32_t cid, uint32_t wid, CGRect rect);
extern CGError SLSAddWindowToWindowOrderingGroup(int cid, uint32_t parent_wid, uint32_t child_wid, int order);
extern CGError SLSBindSurface(int cid, uint32_t wid, uint32_t sid, int x, int y, CGContextRef ctx);
extern CGError SLSClearActivationRegion(uint32_t cid, uint32_t wid);
extern CGError SLSClearWindowTags(int cid, uint32_t wid, uint64_t *tags, int tag_size);
extern CGError SLSConnectionGetPID(int cid, pid_t *pid);
extern CGError SLSCopyWindowProperty(int cid, uint32_t wid, CFStringRef property, CFTypeRef *value);
extern CGError SLSDisableUpdate(int cid);
extern CGError SLSFlushSurface(int cid, uint32_t wid, uint32_t surface, int param);
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
extern CGError SLSOrderSurface(int cid, uint32_t wid, uint32_t surface, uint32_t other_surface, int order);
extern CGError SLSOrderWindow(int cid, uint32_t wid, int mode, uint32_t rel_wid);
extern CGError SLSOrderWindow(int cid, uint32_t wid, int mode, uint32_t relativeToWID);
extern CGError SLSProcessAssignToAllSpaces(int cid, pid_t pid);
extern CGError SLSProcessAssignToSpace(int cid, pid_t pid, uint64_t sid);
extern CGError SLSReassociateWindowsSpacesByGeometry(int cid, CFArrayRef wids);
extern CGError SLSReenableUpdate(int cid);
extern CGError SLSReleaseWindow(int cid, uint32_t wid);
extern CGError SLSRemoveAllTrackingAreas(uint32_t cid, uint32_t wid);
extern CGError SLSRemoveFromOrderingGroup(int cid, uint32_t wid);
extern CGError SLSRemoveSurface(int cid, uint32_t wid, uint32_t sid);
extern CGError SLSRequestNotificationsForWindows(int cid, uint32_t *window_list, int window_count);
extern CGError SLSSetMouseEventEnableFlags(int cid, uint32_t wid, bool shouldEnable);
extern CGError SLSSetSurfaceBounds(int cid, uint32_t wid, uint32_t sid, CGRect bounds);
extern CGError SLSSetSurfaceOpacity(int cid, uint32_t wid, uint32_t sid, bool opaque);
extern CGError SLSSetSurfaceResolution(int cid, uint32_t wid, uint32_t sid, CGFloat scale);
extern CGError SLSSetWindowAlpha(int cid, uint32_t wid, float alpha);
extern CGError SLSSetWindowBackgroundBlurRadius(int cid, uint32_t wid, uint32_t radius);
extern CGError SLSSetWindowLevel(int cid, uint32_t wid, int level);
extern CGError SLSSetWindowOpacity(int cid, uint32_t wid, bool isOpaque);
extern CGError SLSSetWindowOpacity(int cid, uint32_t wid, bool opaque);
extern CGError SLSSetWindowResolution(int cid, uint32_t wid, double res);
extern CGError SLSSetWindowResolution(int cid, uint32_t wid, double resolution);
extern CGError SLSSetWindowShape(int cid, uint32_t wid, float x_offset, float y_offset, CFTypeRef shape);
extern CGError SLSSetWindowTags(int cid, uint32_t wid, uint64_t *tags, int tag_size);
extern CGError SLSWindowIteratorAdvance(CFTypeRef iterator);
extern CGError SLSWindowSetShadowProperties(uint32_t wid, CFDictionaryRef options);
extern CGError SLSWindowSetShadowProperties(uint32_t wid, CFDictionaryRef properties);
extern CGError _SLPSSetFrontProcessWithOptions(ProcessSerialNumber *psn, uint32_t wid, uint32_t mode);
extern CGSConnection _CGSDefaultConnection(void);
extern CGSValue CGSCreateCString(const char *str);
extern OSStatus CGSMoveWindowWithGroup(const int cid, const uint32_t wid, CGPoint *point);
extern OSStatus SLSFindWindowByGeometry(int cid, int zero, int one, int zero_again, CGPoint *screen_point, CGPoint *window_point, uint32_t *wid, int *wcid);
extern OSStatus _SLPSGetFrontProcess(ProcessSerialNumber *psn);
extern bool SLSManagedDisplayIsAnimating(int cid, CFStringRef uuid);
extern char * CGSCStringValue(CGSValue string);
extern float CoreDockGetMagnificationSize(void);
extern float CoreDockGetTileSize(void);
extern int CGSIntegerValue(CGSValue intVal);
extern int CGSMainConnectionID(void);
extern int SLSGetScreenRectForWindow(uint32_t cid, uint32_t wid, CGRect *out);
extern int SLSGetSpaceManagementMode(int cid);
extern int SLSMainConnectionID(void);
extern int SLSSpaceGetType(int cid, uint64_t sid);
extern uint32_t SLSGetActiveSpace(int cid);
extern uint32_t SLSWindowIteratorGetParentID(CFTypeRef iterator);
extern uint32_t SLSWindowIteratorGetWindowID(CFTypeRef iterator);
extern uint64_t CGSManagedDisplayGetCurrentSpace(int cid, CFStringRef display_ref);
extern uint64_t SLSManagedDisplayGetCurrentSpace(int cid, CFStringRef uuid);
extern uint64_t SLSWindowIteratorGetAttributes(CFTypeRef iterator);
extern uint64_t SLSWindowIteratorGetTags(CFTypeRef iterator);
extern void *CGSReleaseGenericObj(void *);
extern void CGSHideSpaces(int cid, CFArrayRef spaces);
extern void CGSManagedDisplaySetCurrentSpace(int cid, CFStringRef display_ref, uint64_t spid);
extern void CGSSetDebugOptions(CGSDebugOptions options);
extern void CGSShowSpaces(int cid, CFArrayRef spaces);
extern void CoreDockGetEffect(CoreDockEffect *outEffect);
extern void CoreDockGetOrientationAndPinning(int *orientation, int *pinning);
extern void CoreDockGetWorkspacesCount(int *rows, int *cols);
extern void CoreDockSetAutoHideEnabled(Boolean flag);
extern void CoreDockSetEffect(CoreDockEffect effect);
extern void CoreDockSetMagnificationEnabled(Boolean flag);
extern void CoreDockSetMagnificationSize(float newSize);
extern void CoreDockSetOrientationAndPinning(CoreDockOrientation orientation, CoreDockPinning pinning);
extern void CoreDockSetTileSize(float tileSize);
extern void CoreDockSetWorkspacesCount(int rows, int cols);
extern void SLSCaptureWindowsContentsToRectWithOptions(uint32_t cid, uint64_t *wid, bool meh, CGRect bounds, uint32_t flags, CGImageRef *image);
extern void SLSMoveWindowsToManagedSpace(int cid, CFArrayRef window_list, uint64_t sid);
extern void _LSASNExtractHighAndLowParts(const void *asn, uint32_t *high, uint32_t *low);
extern OSStatus HIWindowGetAvailablePositioningBounds(CGDirectDisplayID inDisplay, HICoordinateSpace inSpace, HIRect *outAvailableRect);
extern CGError CGSGetDisplayBounds(CGDirectDisplayID display, CGRect *outRect);
extern CGSSpaceManagementMode CGSGetSpaceManagementMode(CGSConnectionID cid) AVAILABLE_MAC_OS_X_VERSION_10_9_AND_LATER;
extern int CGSSpaceGetCompatID(const CGSConnectionID cid, CGSSpace space);
extern CGSSpaceType CGSSpaceGetType(const CGSConnectionID cid, CGSSpace space);
extern CGSSpaceID CGSSpaceCreate(CGSConnectionID cid, void *null, CFDictionaryRef options);
extern CFArrayRef CGSSpaceCopyOwners(const CGSConnectionID cid, CGSSpace space);
extern CFDictionaryRef CGSSpaceCopyValues(CGSConnectionID cid, CGSSpaceID space);
extern CFArrayRef CGSCopySpacesForWindows(CGSConnectionID cid, CGSSpaceMask mask, CFArrayRef windowIDs);
extern CFArrayRef CGSCopySpaces(const CGSConnectionID cid, CGSSpaceSelector type);
extern CFArrayRef CGSCopySpacesForWindows(CGSConnectionID cid, CGSSpaceMask mask, CFArrayRef windowIDs);
extern void CGSSpaceDestroy(CGSConnectionID cid, CGSSpaceID sid);
extern CFStringRef CGSSpaceCopyName(const CGSConnectionID cid, CGSSpace space);
extern void CGSAddWindowsToSpaces(CGSConnectionID cid, CFArrayRef windows, CFArrayRef spaces);
extern void CGSRemoveWindowsFromSpaces(CGSConnectionID cid, CFArrayRef windows, CFArrayRef spaces);
extern CGSSpaceID CGSGetActiveSpace(CGSConnectionID cid);
extern CGError CGSGetDisplayBounds(CGDirectDisplayID display, CGRect *outRect);
extern CGSConnectionID _CGSDefaultConnection(void);
extern CGError CGSSetUniversalOwner(CGSConnectionID cid);
extern CGError CGSSetOtherUniversalConnection(CGSConnectionID cid, CGSConnectionID otherConnection);
extern CGError CGSNewConnection(int unused, CGSConnectionID *outConnection);
typedef void (*CGSNewConnectionNotificationProc)(CGSConnectionID cid);
extern CGError CGSRegisterForNewConnectionNotification(CGSNewConnectionNotificationProc proc);
extern CGError CGSRemoveNewConnectionNotification(CGSNewConnectionNotificationProc proc);
extern CGError CGSGetConnectionIDForPSN(CGSConnectionID cid, const ProcessSerialNumber *psn, CGSConnectionID *outOwnerCID);
extern CGError CGSInvalidateWindowShadow(CGSConnectionID cid, CGWindowID wid);
extern CGError CGSSetWindowOriginRelativeToWindow(CGSConnectionID cid, CGWindowID wid, CGWindowID relativeToWID, CGFloat offsetX, CGFloat offsetY);
extern CGError CGSRegisterNotifyProc(CGSNotifyProcPtr proc, CGSEventType type, void *userData);
extern CGError CGSRemoveNotifyProc(CGSNotifyProcPtr proc, CGSEventType type, void *userData);
extern CGError CGSSetConnectionProperty(CGSConnectionID cid, CGSConnectionID targetCID, CFStringRef key, CFTypeRef value);
extern CGError CGSGetCurrentCursorLocation(CGSConnectionID cid, CGPoint *outPos);
extern CGError CGSGetSurfaceCount(CGSConnectionID cid, CGWindowID wid, int *outCount);
extern bool CGSEventIsAppUnresponsive(CGSConnectionID cid, const ProcessSerialNumber *psn);
extern void CGSWindowSetBackdropBackgroundBleed(CGWindowID wid, CGFloat bleedAmount) AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER;
extern CGError CGEventGetEventRecord(CGEventRef event, CGSEventRecord *outRecord, size_t recSize);

extern bool CGSIsHotKeyEnabled(CGSConnectionID cid, int uid);
extern CFArrayRef CGSCopySessionList(void);
extern CFArrayRef CGSSpaceCopyTileSpaces(CGSConnectionID cid, CGSSpaceID sid) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CFDictionaryRef CGSCopyCurrentSessionDictionary(void);
extern CFStringRef kCGSPackagesMainDisplayIdentifier;
extern CGAffineTransform CGSSpaceGetTransform(CGSConnectionID cid, CGSSpaceID space);
extern CGDirectDisplayID CGSMainDisplayID(void);
extern CGDisplayErr CGSBeginDisplayConfiguration(CGDisplayConfigRef *config);
extern CGDisplayErr CGSCancelDisplayConfiguration(CGDisplayConfigRef config);
extern CGDisplayErr CGSCompleteDisplayConfiguration(CGDisplayConfigRef config);
extern CGDisplayErr CGSConfigureDisplayOrigin(CGDisplayConfigRef config, CGDirectDisplayID display, int32_t x, int32_t y);
extern CGDisplayErr CGSGetActiveDisplayList(CGDisplayCount maxDisplays, CGDirectDisplayID *displays, CGDisplayCount *outDisplayCount);
extern CGDisplayErr CGSGetOnlineDisplayList(CGDisplayCount maxDisplays, CGDirectDisplayID *displays, CGDisplayCount *outDisplayCount);
extern CGError CGSActuateDeviceWithPattern(CGSConnectionID cid, int deviceID, int pattern, int strength) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGError CGSAddActivationRegion(CGSConnectionID cid, CGWindowID wid, CGSRegionRef region);
extern CGError CGSAddDragRegion(CGSConnectionID cid, CGWindowID wid, CGSRegionRef region);
extern CGError CGSAddSurface(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID *outSID);
extern CGError CGSAdjustSystemStatusBarWindows(CGSConnectionID cid);
extern CGError CGSBindSurface(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID sid, int x, int y, unsigned int ctx);
extern CGError CGSClearDragRegion(CGSConnectionID cid, CGWindowID wid);
extern CGError CGSConfigureDisplayMode(CGDisplayConfigRef config, CGDirectDisplayID display, int modeNum);
extern CGError CGSCopyConnectionProperty(CGSConnectionID cid, CGSConnectionID targetCID, CFStringRef key, CFTypeRef *outValue);
extern CGError CGSCopyRegion(CGSRegionRef region, CGSRegionRef *outRegion);
extern CGError CGSCopyRegisteredCursorImages(CGSConnectionID cid, const char *cursorName, CGSize *imageSize, CGPoint *hotSpot, int *frameCount, CGFloat *frameDuration, CFArrayRef *imageArray);
extern CGError CGSCopyWindowClipShape(CGSConnectionID cid, CGWindowID wid, CGSRegionRef *outRegion);
extern CGError CGSCopyWindowColorSpace(CGSConnectionID cid, CGWindowID wid, CGColorSpaceRef *outColorSpace);
extern CGError CGSCreateGenieWindowAnimation(CGSConnectionID cid, CGWindowID wid, CGWindowID destinationWID, CGSAnimationRef *outAnimation);
extern CGError CGSCreateLoginSession(CGSSessionID *outSession);
extern CGError CGSCreateMetalSheetWindowAnimationWithParent(CGSConnectionID cid, CGWindowID wid, CGWindowID parentWID, CGSAnimationRef *outAnimation);
extern CGError CGSDiffRegion(CGSRegionRef region1, CGSRegionRef region2, CGSRegionRef *outRegion);
extern CGError CGSDisplayStatusQuery(CGDirectDisplayID display, CGSDisplayQuery query);
extern CGError CGSDragWindowRelativeToMouse(CGSConnectionID cid, CGWindowID wid, CGPoint point);
extern CGError CGSEventSetAppIsUnresponsiveNotificationTimeout(CGSConnectionID cid, double theTime);
extern CGError CGSFetchDirtyScreenRegion(CGSConnectionID cid, CGSRegionRef *outDirtyRegion);
extern CGError CGSFlushSurface(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID surface, int param);
extern CGError CGSForceWaitCursorActive(CGSConnectionID cid, bool showWaitCursor);
extern CGError CGSGetCurrentDisplayMode(CGDirectDisplayID display, int *modeNum);
extern CGError CGSGetCursorData(CGSConnectionID cid, void *outData);
extern CGError CGSGetCursorDataSize(CGSConnectionID cid, size_t *outDataSize);
extern CGError CGSGetCursorScale(CGSConnectionID cid, CGFloat *outScale);
extern CGError CGSGetDebugOptions(int *outCurrentOptions);
extern CGError CGSGetDisplayDepth(CGDirectDisplayID display, int *outDepth);
extern CGError CGSGetDisplayModeDescriptionOfLength(CGDirectDisplayID display, int idx, CGSDisplayModeDescription *desc, int length);
extern CGError CGSGetDisplayRegion(CGDirectDisplayID display, CGSRegionRef *outRegion);
extern CGError CGSGetDisplayRowBytes(CGDirectDisplayID display, int *outRowBytes);
extern CGError CGSGetDisplaysWithPoint(const CGPoint *point, int maxDisplayCount, CGDirectDisplayID *outDisplays, int *outDisplayCount);
extern CGError CGSGetDisplaysWithRect(const CGRect *point, int maxDisplayCount, CGDirectDisplayID *outDisplays, int *outDisplayCount);
extern CGError CGSGetGlobalCursorData(CGSConnectionID cid, void *outData, int *outDataSize, int *outRowBytes, CGRect *outRect, CGPoint *outHotSpot, int *outDepth, int *outComponents, int *outBitsPerComponent);
extern CGError CGSGetGlobalCursorDataSize(CGSConnectionID cid, size_t *outDataSize);
extern CGError CGSGetGlobalHotKeyOperatingMode(CGSConnectionID cid, CGSGlobalHotKeyOperatingMode *outMode);
extern CGError CGSGetNumberOfDisplayModes(CGDirectDisplayID display, int *nModes);
extern CGError CGSGetPerformanceData(CGSConnectionID cid, CGFloat *outFPS, CGFloat *unk, CGFloat *unk2, CGFloat *unk3);
extern CGError CGSGetRegionBounds(CGSRegionRef region, CGRect *outRect);
extern CGError CGSGetRegisteredCursorDataSize(CGSConnectionID cid, const char *cursorName, size_t *outDataSize);
extern CGError CGSGetSurfaceBounds(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID sid, CGFloat *bounds);
extern CGError CGSGetSurfaceList(CGSConnectionID cid, CGWindowID wid, int countIds, CGSSurfaceID *ids, int *outCount);
extern CGError CGSGetSymbolicHotKeyValue(CGSSymbolicHotKey hotKey, u_char *outKeyEquivalent, u_char *outVirtualKeyCode, CGSModifierFlags *outModifiers);
extern CGError CGSGetSystemDefinedCursorData(CGSConnectionID cid, CGSCursorID cursor, void *outData, int *outRowBytes, CGRect *outRect, CGPoint *outHotSpot, int *outDepth, int *outComponents, int *outBitsPerComponent);
extern CGError CGSGetSystemDefinedCursorDataSize(CGSConnectionID cid, CGSCursorID cursor, size_t *outDataSize);
extern CGError CGSGetWindowAutofill(CGSConnectionID cid, CGWindowID wid, bool *outShouldAutoFill);
extern CGError CGSGetWindowMouseLocation(CGSConnectionID cid, CGWindowID wid, CGPoint *outPos);
extern CGError CGSGetWindowOpacity(CGSConnectionID cid, CGWindowID wid, bool *outIsOpaque);
extern CGError CGSGetWindowSharingState(CGSConnectionID cid, CGWindowID wid, CGSSharingState *outState);
extern CGError CGSGetWindowTransformAtPlacement(CGSConnectionID cid, CGWindowID wid, const CGAffineTransform *outTransform);
extern CGError CGSGlobalError(CGError err, const char *msg);
extern CGError CGSGlobalErrorv(CGError err, const char *msg, ...);
extern CGError CGSHideCursor(CGSConnectionID cid);
extern CGError CGSIsZoomed(CGSConnectionID cid, bool *outIsZoomed);
extern CGError CGSNewRegionWithQDRgn(RgnHandle region, CGSRegionRef *outRegion);
extern CGError CGSNewRegionWithRectList(const CGRect *rects, int rectCount, CGSRegionRef *outRegion);
extern CGError CGSObscureCursor(CGSConnectionID cid);
extern CGError CGSOffsetRegion(CGSRegionRef region, CGFloat offsetLeft, CGFloat offsetTop, CGSRegionRef *outRegion);
extern CGError CGSOrderFrontConditionally(CGSConnectionID cid, CGWindowID wid, bool force);
extern CGError CGSOrderSurface(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID surface, CGSSurfaceID otherSurface, int place);

CG_EXTERN CGError CGSRegisterCursorWithImages(CGSConnectionID cid, const char *cursorName, bool setGlobally, bool instantly, int frameCount, CFArrayRef imageArray, CGSize cursorSize, CGPoint hotspot, int *seed, CGRect bounds, CGFloat frameDuration, int repeatCount);
extern CGError CGSRegisterForConnectionDeathNotification(CGSConnectionDeathNotificationProc proc);
extern CGError CGSReleaseConnection(CGSConnectionID cid);
extern CGError CGSReleaseSession(CGSSessionID session);
extern CGError CGSReleaseWindowAnimation(CGSAnimationRef animation);
extern CGError CGSRemoveConnectionDeathNotification(CGSConnectionDeathNotificationProc proc);
extern CGError CGSRemoveSurface(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID sid);
extern CGError CGSRevealCursor(CGSConnectionID cid);
extern CGError CGSSetBackgroundEventMask(CGSConnectionID cid, int mask);
extern CGError CGSSetCursorScale(CGSConnectionID cid, CGFloat scale);
extern CGError CGSSetDisplayContrast(CGFloat contrast);
extern CGError CGSSetGlobalHotKeyOperatingMode(CGSConnectionID cid, CGSGlobalHotKeyOperatingMode mode);
extern CGError CGSSetIgnoresCycle(CGSConnectionID cid, CGWindowID wid, bool ignoresCycle);
extern CGError CGSSetLoginwindowConnection(CGSConnectionID cid) AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER;
extern CGError CGSSetMouseEventEnableFlags(CGSConnectionID cid, CGWindowID wid, bool shouldEnable);
extern CGError CGSSetMouseFocusWindow(CGSConnectionID cid, CGWindowID wid);
extern CGError CGSSetPressureConfigurationOverride(CGSConnectionID cid, int deviceID, void *config) AVAILABLE_MAC_OS_X_VERSION_10_10_3_AND_LATER;
extern CGError CGSSetRegisteredCursor(CGSConnectionID cid, const char *cursorName, int *cursorSeed);
extern CGError CGSSetSecureEventInput(CGSConnectionID cid, bool useSecureInput);
extern CGError CGSSetSpaceManagementMode(CGSConnectionID cid, CGSSpaceManagementMode mode) AVAILABLE_MAC_OS_X_VERSION_10_9_AND_LATER;
extern CGError CGSSetSurfaceBackgroundBlur(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID sid, CGFloat blur) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGError CGSSetSurfaceBounds(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID sid, CGRect bounds);
extern CGError CGSSetSurfaceColorSpace(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID surface, CGColorSpaceRef colorSpace);
extern CGError CGSSetSurfaceLayerBackingOptions(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID surface, CGFloat flattenDelay, CGFloat decelerationDelay, CGFloat discardDelay);
extern CGError CGSSetSurfaceOpacity(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID sid, bool isOpaque);
extern CGError CGSSetSurfaceResolution(CGSConnectionID cid, CGWindowID wid, CGSSurfaceID sid, CGFloat scale) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGError CGSSetSymbolicHotKeyEnabled(CGSSymbolicHotKey hotKey, bool isEnabled);
extern CGError CGSSetWindowActive(CGSConnectionID cid, CGWindowID wid, bool isActive);
extern CGError CGSSetWindowAnimationProgress(CGSAnimationRef animation, CGFloat progress);
extern CGError CGSSetWindowAutofill(CGSConnectionID cid, CGWindowID wid, bool shouldAutoFill);
extern CGError CGSSetWindowAutofillColor(CGSConnectionID cid, CGWindowID wid, CGFloat red, CGFloat green, CGFloat blue);
extern CGError CGSSetWindowClipShape(CGWindowID wid, CGSRegionRef shape);
extern CGError CGSSetWindowColorSpace(CGSConnectionID cid, CGWindowID wid, CGColorSpaceRef colorSpace);
extern CGError CGSSetWindowEventShape(CGSConnectionID cid, CGSBackingType backingType, CGSRegionRef *shape);
extern CGError CGSSetWindowHasKeyAppearance(CGSConnectionID cid, CGWindowID wid, bool hasKeyAppearance);
extern CGError CGSSetWindowOpacity(CGSConnectionID cid, CGWindowID wid, bool isOpaque);
extern CGError CGSSetWindowOwner(CGSConnectionID cid, CGWindowID wid, CGSConnectionID owner);
extern CGError CGSSetWindowShadowAndRimParametersWithStretch(CGSConnectionID cid, CGWindowID wid, CGFloat standardDeviation, CGFloat density, int offsetX, int offsetY, int stretch_x, int stretch_y, unsigned int flags);
extern CGError CGSSetWindowShadowParameters(CGSConnectionID cid, CGWindowID wid, CGFloat standardDeviation, CGFloat density, int offsetX, int offsetY);
extern CGError CGSSetWindowShape(CGSConnectionID cid, CGWindowID wid, CGFloat offsetX, CGFloat offsetY, CGSRegionRef shape);
extern CGError CGSSetWindowShapeWithWeighting(CGSConnectionID cid, CGWindowID wid, CGFloat offsetX, CGFloat offsetY, CGSRegionRef shape, CGSWindowSaveWeighting weight);
extern CGError CGSSetWindowSharingState(CGSConnectionID cid, CGWindowID wid, CGSSharingState state);
extern CGError CGSSetWindowTitle(CGSConnectionID cid, CGWindowID wid, CFStringRef title);
extern CGError CGSSetWindowTransformAtPlacement(CGSConnectionID cid, CGWindowID wid, CGAffineTransform transform);
extern CGError CGSSetWindowWorkspace(CGSConnectionID cid, CGWindowID wid, CGSWorkspaceID workspace);
extern CGError CGSShowCursor(CGSConnectionID cid);
extern CGError CGSShutdownServerConnections(void);
extern CGError CGSSpaceCreateTile(CGSConnectionID cid, CGSSpaceID sid, CGSTileID *outTID) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGError CGSSpaceGetSizeForProposedTile(CGSConnectionID cid, CGSSpaceID sid, CGSize *outSize) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGError CGSSpaceRemoveValuesForKeys(CGSConnectionID cid, CGSSpaceID sid, CFArrayRef values);
extern CGError CGSSpaceSetInterTileSpacing(CGSConnectionID cid, CGSSpaceID sid, CGFloat spacing) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGError CGSSpaceSetName(CGSConnectionID cid, CGSSpaceID sid, CFStringRef name);
extern CGError CGSSpaceSetValues(CGSConnectionID cid, CGSSpaceID sid, CFDictionaryRef values);
extern CGError CGSSystemStatusBarRegisterWindow(CGSConnectionID cid, CGWindowID wid, int priority);
extern CGError CGSUnionRegion(CGSRegionRef region1, CGSRegionRef region2, CGSRegionRef *outRegion);
extern CGError CGSUnionRegionWithRect(CGSRegionRef region, CGRect *rect, CGSRegionRef *outRegion);
extern CGError CGSUnregisterWindowWithSystemStatusBar(CGSConnectionID cid, CGWindowID wid);
extern CGError CGSWarpCursorPosition(CGSConnectionID cid, CGFloat x, CGFloat y);
extern CGError CGSWindowAnimationChangeLevel(CGSAnimationRef animation, CGWindowLevel level);
extern CGError CGSWindowAnimationSetParent(CGSAnimationRef animation, CGWindowID parent) AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER;
extern CGError CGSWindowCanAccelerate(CGSConnectionID cid, CGWindowID wid, bool *outCanAccelerate);
extern CGError CGSWindowIsAccelerated(CGSConnectionID cid, CGWindowID wid, bool *outIsAccelerated);
extern CGError CGSWindowSetCanAccelerate(CGSConnectionID cid, CGWindowID wid, bool canAccelerate);
extern CGError CGSXorRegion(CGSRegionRef region1, CGSRegionRef region2, CGSRegionRef *outRegion);
extern CGFloat CGSSpaceGetInterTileSpacing(CGSConnectionID cid, CGSSpaceID sid) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGRect *CGSNextRect(CGSRegionEnumeratorRef enumerator);
extern CGSConnectionID CGSDefaultConnectionForThread(void);
extern CGSRegionRef CGSSpaceCopyManagedShape(CGSConnectionID cid, CGSSpaceID sid);
extern CGSRegionRef CGSSpaceCopyShape(CGSConnectionID cid, CGSSpaceID space);
extern CGSSpaceID CGSTileEvictionRecordGetManagedSpaceID(CGSConnectionID ownerID) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGSSpaceID CGSTileOwnerChangeRecordGetManagedSpaceID(CGSConnectionID ownerID) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGSSpaceID CGSTileOwnerChangeRecordGetNewOwner(CGSConnectionID ownerID) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGSSpaceID CGSTileOwnerChangeRecordGetOldOwner(CGSConnectionID ownerID) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGSSpaceID CGSTileSpaceResizeRecordGetParentSpaceID(CGSSpaceID sid) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGSSpaceID CGSTileSpaceResizeRecordGetSpaceID(CGSSpaceID sid) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGSTileID CGSTileEvictionRecordGetTileID(CGSConnectionID ownerID) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGSTileID CGSTileOwnerChangeRecordGetTileID(CGSConnectionID ownerID) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern CGSWindowBackdropRef CGSWindowBackdropCreateWithLevel(CGWindowID wid, CFStringRef materialName, CGWindowLevel level, CGRect frame) AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER;
extern OSErr CGSGetEventPort(CGSConnectionID identifier, mach_port_t *port);
extern bool CGDisplayUsesForceToGray(void);
extern bool CGDisplayUsesInvertedPolarity(void);
extern bool CGSGetHotKey(CGSConnectionID cid, int uid, u_char *options, u_char *key, CGSModifierFlags *modifierFlags);
extern bool CGSIsScreenWatcherPresent(void);
extern bool CGSIsSecureEventInputSet(void);
extern bool CGSIsSymbolicHotKeyEnabled(CGSSymbolicHotKey hotKey);
extern bool CGSMenuBarExists(CGSConnectionID cid);
extern bool CGSPointInRegion(CGSRegionRef region, const CGPoint *point);
extern bool CGSRectInRegion(CGSRegionRef region, const CGRect *rect);
extern bool CGSRegionInRegion(CGSRegionRef region1, CGSRegionRef region2);
extern bool CGSRegionIntersectsRect(CGSRegionRef obj, const CGRect *rect);
extern bool CGSRegionIntersectsRegion(CGSRegionRef region1, CGSRegionRef region2);
extern bool CGSRegionIsEmpty(CGSRegionRef region);
extern bool CGSRegionIsRectangular(CGSRegionRef region);
extern bool CGSRegionsEqual(CGSRegionRef region1, CGSRegionRef region2);
extern bool CGSSpaceCanCreateTile(CGSConnectionID cid, CGSSpaceID sid) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern bool CGSTileSpaceResizeRecordIsLiveResizing(CGSSpaceID sid) AVAILABLE_MAC_OS_X_VERSION_10_11_AND_LATER;
extern char *CGSCursorNameForSystemCursor(CGSCursorID cursor);
extern char *CGSErrorString(CGError error);
extern int CGSCurrentCursorSeed(void);
extern uint32_t CGSGetNumberOfDisplays(void);
extern void CGDisplayForceToGray(bool forceToGray);
extern void CGDisplaySetInvertedPolarity(bool invertedPolarity);
extern void CGSGetBackgroundEventMask(CGSConnectionID cid, int *outMask);
extern void CGSReleaseRegionEnumerator(CGSRegionEnumeratorRef enumerator);
extern void CGSRemoveHotKey(CGSConnectionID cid, int uid);
extern void CGSSetHotKey(CGSConnectionID cid, int uid, u_char options, u_char key, CGSModifierFlags modifierFlags);
extern void CGSSetHotKeyEnabled(CGSConnectionID cid, int uid, bool enabled);
extern void CGSSetHotKeyWithExclusion(CGSConnectionID cid, int uid, u_char options, u_char key, CGSModifierFlags modifierFlags, int exclusion);
extern void CGSSetSystemDefinedCursorWithSeed(CGSConnectionID connection, CGSCursorID systemCursor, int *cursorSeed);
extern void CGSSpaceSetShape(CGSConnectionID cid, CGSSpaceID space, CGSRegionRef shape);
extern void CGSSpaceSetTransform(CGSConnectionID cid, CGSSpaceID space, CGAffineTransform transform);
extern void CGSWindowBackdropActivate(CGSWindowBackdropRef backdrop) AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER;
extern void CGSWindowBackdropDeactivate(CGSWindowBackdropRef backdrop) AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER;
extern void CGSWindowBackdropRelease(CGSWindowBackdropRef backdrop) AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER;
extern void CGSWindowBackdropSetSaturation(CGSWindowBackdropRef backdrop, CGFloat saturation) AVAILABLE_MAC_OS_X_VERSION_10_10_AND_LATER;
AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window);
#endif
