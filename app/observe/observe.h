#pragma once
#ifndef OBSERVE_H
#define OBSERVE_H
//////////////////////////////////////
#include "frameworks/frameworks.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////////////////////////
void app_observe_start(pid_t pid);

#define AX_APPLICATION_WINDOW_CREATED_INDEX          0
#define AX_APPLICATION_WINDOW_FOCUSED_INDEX          1
#define AX_APPLICATION_WINDOW_MOVED_INDEX            2
#define AX_APPLICATION_WINDOW_RESIZED_INDEX          3
#define AX_APPLICATION_WINDOW_TITLE_CHANGED_INDEX    4
#define AX_APPLICATION_WINDOW_MENU_OPENED_INDEX      5
#define AX_APPLICATION_WINDOW_MENU_CLOSED_INDEX      6

#define AX_APPLICATION_WINDOW_CREATED                (1 << AX_APPLICATION_WINDOW_CREATED_INDEX)
#define AX_APPLICATION_WINDOW_FOCUSED                (1 << AX_APPLICATION_WINDOW_FOCUSED_INDEX)
#define AX_APPLICATION_WINDOW_MOVED                  (1 << AX_APPLICATION_WINDOW_MOVED_INDEX)
#define AX_APPLICATION_WINDOW_RESIZED                (1 << AX_APPLICATION_WINDOW_RESIZED_INDEX)
#define AX_APPLICATION_WINDOW_TITLE_CHANGED          (1 << AX_APPLICATION_WINDOW_TITLE_CHANGED_INDEX)
#define AX_APPLICATION_ALL                           (AX_APPLICATION_WINDOW_CREATED | \
                                                      AX_APPLICATION_WINDOW_FOCUSED | \
                                                      AX_APPLICATION_WINDOW_MOVED |   \
                                                      AX_APPLICATION_WINDOW_RESIZED | \

static const char *ax_error_str[] =
{
  [-kAXErrorSuccess] = "kAXErrorSuccess",
  [-kAXErrorFailure] = "kAXErrorFailure",
  [-kAXErrorIllegalArgument] = "kAXErrorIllegalArgument",
  [-kAXErrorInvalidUIElement] = "kAXErrorInvalidUIElement",
  [-kAXErrorInvalidUIElementObserver] = "kAXErrorInvalidUIElementObserver",
  [-kAXErrorCannotComplete] = "kAXErrorCannotComplete",
  [-kAXErrorAttributeUnsupported] = "kAXErrorAttributeUnsupported",
  [-kAXErrorActionUnsupported] = "kAXErrorActionUnsupported",
  [-kAXErrorNotificationUnsupported] = "kAXErrorNotificationUnsupported",
  [-kAXErrorNotImplemented] = "kAXErrorNotImplemented",
  [-kAXErrorNotificationAlreadyRegistered] = "kAXErrorNotificationAlreadyRegistered",
  [-kAXErrorNotificationNotRegistered] = "kAXErrorNotificationNotRegistered",
  [-kAXErrorAPIDisabled] = "kAXErrorAPIDisabled",
  [-kAXErrorNoValue] = "kAXErrorNoValue",
  [-kAXErrorParameterizedAttributeUnsupported] = "kAXErrorParameterizedAttributeUnsupported",
  [-kAXErrorNotEnoughPrecision] = "kAXErrorNotEnoughPrecision"
};
static const char *ax_application_notification_str[] =
{
  [AX_APPLICATION_WINDOW_CREATED_INDEX] = "kAXCreatedNotification",
  [AX_APPLICATION_WINDOW_FOCUSED_INDEX] = "kAXFocusedWindowChangedNotification",
  [AX_APPLICATION_WINDOW_MOVED_INDEX] = "kAXWindowMovedNotification",
  [AX_APPLICATION_WINDOW_RESIZED_INDEX] = "kAXWindowResizedNotification",
  [AX_APPLICATION_WINDOW_TITLE_CHANGED_INDEX] = "kAXTitleChangedNotification",
  [AX_APPLICATION_WINDOW_MENU_OPENED_INDEX] = "kAXMenuOpenedNotification",
  [AX_APPLICATION_WINDOW_MENU_CLOSED_INDEX] = "kAXMenuClosedNotification"
};

static CFStringRef ax_application_notification[] =
{
  [AX_APPLICATION_WINDOW_CREATED_INDEX] = kAXCreatedNotification,
  [AX_APPLICATION_WINDOW_FOCUSED_INDEX] = kAXFocusedWindowChangedNotification,
  [AX_APPLICATION_WINDOW_MOVED_INDEX] = kAXWindowMovedNotification,
  [AX_APPLICATION_WINDOW_RESIZED_INDEX] = kAXWindowResizedNotification,
  [AX_APPLICATION_WINDOW_TITLE_CHANGED_INDEX] = kAXTitleChangedNotification,
  [AX_APPLICATION_WINDOW_MENU_OPENED_INDEX] = kAXMenuOpenedNotification,
  [AX_APPLICATION_WINDOW_MENU_CLOSED_INDEX] = kAXMenuClosedNotification
};
#endif
