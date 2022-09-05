#pragma once
#include "app-utils/app-utils.h"
#include "app/app.h"
#include "date.c/date.h"
#include "systemprofiler/systemprofiler.h"
#include "timelib/timelib.h"
static struct app_parser_t app_parsers[APP_PARSER_TYPES_QTY] = {
  [APP_PARSER_TYPE_NAME] =                                                     { .enabled = true,                                      .parser = ^ void (struct app_t *app, JSON_Object *app_object){
                                                                                   app->name                                                  = (json_object_has_value_of_type(app_object, "_name", JSONString))
                    ? json_object_get_string(app_object, "_name")
                    : NULL;
                                                                                 }, },
  [APP_PARSER_TYPE_VERSION] =                                                  { .enabled = true,                                      .parser = ^ void (struct app_t *app, JSON_Object *app_object){
                                                                                   app->version                                            = (json_object_has_value_of_type(app_object, "version", JSONString))
                    ? json_object_get_string(app_object, "version")
                    : NULL;
                                                                                 }, },
  [APP_PARSER_TYPE_LAST_MODIFIED] =                                            { .enabled = true,                                      .parser = ^ void (struct app_t *app, JSON_Object *app_object){
                                                                                   app->last_modified_s                              = (json_object_has_value_of_type(app_object, "lastModified", JSONString))
                    ? json_object_get_string(app_object, "lastModified")
                    : NULL;
                                                                                 }, },
  [APP_PARSER_TYPE_LAST_MODIFIED_TIME] =                                       { .enabled = true, .parser = ^ void (struct app_t *app, JSON_Object *app_object){
                                                                                   app->last_modified_time                      = (app->last_modified_s)
                    ? timelib_strtotime(app->last_modified_s, strlen(app->last_modified_s), NULL, timelib_builtin_db(), timelib_parse_tzfile)
                    : NULL;
                                                                                 }, },
  [APP_PARSER_TYPE_LAST_MODIFIED_TIMESTAMP] =                                  { .enabled = true, .parser = ^ void (struct app_t *app, JSON_Object *app_object){
                                                                                   if (app->last_modified_time) {
                                                                                     timelib_update_ts(app->last_modified_time, NULL);
                                                                                   }
                                                                                   app->last_modified_timestamp            = (app->last_modified_time)
                    ? app->last_modified_time->sse
                    : 0;
                                                                                 }, },
  [APP_PARSER_TYPE_PATH] =                                                     { .enabled = true,                                      .parser = ^ void (struct app_t *app, JSON_Object *app_object){
                                                                                   app->path                                                  = (json_object_has_value_of_type(app_object, "path", JSONString))
                    ? json_object_get_string(app_object, "path")
                    : NULL;
                                                                                   app->path_exists                                           = (app->path)
                    ? fsio_path_exists(app->path)
                    : false;
                                                                                 }, },
  [APP_PARSER_TYPE_OBTAINED_FROM] =                                            { .enabled = true,                                      .parser = ^ void (struct app_t *app, JSON_Object *app_object){
                                                                                   app->obtained_from                                = (json_object_has_value_of_type(app_object, "obtained_from", JSONString))
                    ? json_object_get_string(app_object, "obtained_from")
                    : NULL;
                                                                                 }, },
  [APP_PARSER_TYPE_INFO] =                                                     { .enabled = true,                                      .parser = ^ void (struct app_t *app, JSON_Object *app_object){
                                                                                   app->info                                                  = (json_object_has_value_of_type(app_object, "info", JSONString))
                    ? json_object_get_string(app_object, "info")
                    : NULL;
                                                                                 }, },
  [APP_PARSER_TYPE_ARCH] =                                                     { .enabled = true,                                      .parser = ^ void (struct app_t *app, JSON_Object *app_object){
                                                                                   app->arch                                                  = (json_object_has_value_of_type(app_object, "arch_kind", JSONString))
                    ? json_object_get_string(app_object, "arch_kind")
                    : NULL;
                                                                                 }, },
};
///////////////////////////////////////////////////////////////////////
static void parse_app(struct app_t *app, JSON_Object *app_object);
static bool APP_UTILS_DEBUG_MODE = false;
static void debug_app(struct app_t *app);
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__app_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_APP_UTILS") != NULL) {
    log_debug("Enabling App Utils Debug Mode");
    APP_UTILS_DEBUG_MODE = true;
  }
}

static void debug_app(struct app_t *app){
  fprintf(stdout,
          "\n\tName             :   %s"
          "\n\tVersion          :   %s"
          "\n\tLast Modified    :   %s (%ld) %s"
          "\n\tInfo             :   %s"
          "\n\tPath             :   %s (%s" AC_RESETALL ")"
          "\n\tArch             :   %s"
          "\n\tObtained from    :   %s"
          "%s",
          app->name,
          app->version,
          app->last_modified_s,
          app->last_modified_timestamp,
          (app->last_modified_timestamp > 0)
                  ? milliseconds_to_string(timestamp() - (app->last_modified_timestamp * 1000))
                  : "",
          app->info,
          app->path,
          app->path_exists ? AC_GREEN "Existant" : AC_RED "Absent",
          app->arch,
          app->obtained_from,
          "\n"
          );
}

///////////////////////////////////////////////////////////////////////

bool request_accessibility_permissions() {
  bool                       result          = false;
  CFDictionaryKeyCallBacks   key_callbacks   = kCFTypeDictionaryKeyCallBacks;
  CFDictionaryValueCallBacks value_callbacks = kCFTypeDictionaryValueCallBacks;
  CFStringRef                key             = kAXTrustedCheckOptionPrompt;
  CFBooleanRef               value           = kCFBooleanTrue;
  CFDictionaryRef            options         = CFDictionaryCreate(
    kCFAllocatorDefault, (CFTypeRef *)&key, (CFTypeRef *)&value, 1,
    &key_callbacks, &value_callbacks);

  result = AXIsProcessTrustedWithOptions(options);
  CFRelease(key);
  CFRelease(value);
  CFRelease(options);
  return(result);
}

authorized_tests_t authorized_tests = {
  .tests             = {
    [AUTHORIZED_ACCESSIBILITY] =     {
      .id            = AUTHORIZED_ACCESSIBILITY,
      .name          = "accessibility",
      .test_function = (*is_authorized_for_accessibility),
      .authorized    = false,                             .ts= 0, .dur_ms = 0,
    },
    [AUTHORIZED_SCREEN_RECORDING] =  {
      .id            = AUTHORIZED_SCREEN_RECORDING,
      .name          = "screen_recording",
      .test_function = (*is_authorized_for_screen_recording),
      .authorized    = false,                                .ts= 0, .dur_ms = 0,
    },
    [AUTHORIZED_TEST_TYPE_IDS_QTY] = { 0 },
  },
};

///////////////////////////////////////////////////////////////////////
static void parse_app(struct app_t *app, JSON_Object *app_object){
  for (size_t i = 0; i < APP_PARSER_TYPES_QTY; i++) {
    if (app_parsers[i].enabled == true) {
      app_parsers[i].parser(app, app_object);
    }
  }
}
struct Vector *get_installed_apps_v(){
  struct Vector *a   = vector_new();
  char          *out = run_system_profiler_item_subprocess("SPApplicationsDataType", 60 * 60);

  log_debug("%s", bytes_to_string(strlen(out)));
  JSON_Value *root_value = json_parse_string(out);

  if (json_value_get_type(root_value) == JSONObject) {
    JSON_Object *root_object = json_value_get_object(root_value);
    if (json_object_has_value_of_type(root_object, "SPApplicationsDataType", JSONArray)) {
      JSON_Array *apps_array = json_object_dotget_array(root_object, "SPApplicationsDataType");
      size_t     apps_qty    = json_array_get_count(apps_array);
      for (size_t i = 0; i < apps_qty; i++) {
        struct app_t *app = calloc(1, sizeof(struct app_t));
        parse_app(app, json_array_get_object(apps_array, i));
        debug_app(app);
      }
    }
  }

  return(a);
} /* get_installed_apps_v */

struct Vector *get_running_apps_v(){
  struct Vector *a = vector_new();

  return(a);
}

///////////////////////////////////////////////////////////////////////
AXUIElementRef get_frontmost_app(){
  pid_t pid; ProcessSerialNumber psn;

  GetFrontProcess(&psn); GetProcessPID(&psn, &pid);
  return(AXUIElementCreateApplication(pid));
}

authorized_test_t execute_authorization_test(int authorized_test_type_id){
  authorized_test_t authorized_test = authorized_tests.tests[authorized_test_type_id];

  authorized_test.ts            = timestamp();
  authorized_test.authorized    = authorized_test.test_function();
  authorized_test.test_function = NULL;
  return(authorized_test);
}

authorized_test_t *execute_authorization_tests(){
  authorized_test_t *authorized_test_results = calloc(AUTHORIZED_TEST_TYPE_IDS_QTY, sizeof(authorized_test_t));

  for (int i = 0; i < AUTHORIZED_TEST_TYPE_IDS_QTY; i++) {
    unsigned long started = timestamp();
    authorized_test_results[i]        = (execute_authorization_test(i));
    authorized_test_results[i].dur_ms = timestamp() - started;
  }
  return(authorized_test_results);
}

bool request_screen_recording_permissions() {
  CGDisplayStreamRef stream = NULL;

  stream = CGDisplayStreamCreate(CGMainDisplayID(), 1, 1, kCVPixelFormatType_32BGRA, nil,
                                 ^ (CGDisplayStreamFrameStatus status, uint64_t displayTime, IOSurfaceRef frameSurface,
                                    CGDisplayStreamUpdateRef updateRef){});
  if (stream == NULL) {
    return(false);
  }
  CFRelease(stream);
  return(true);
}

bool is_authorized_for_screen_recording() {
  CGDisplayStreamFrameAvailableHandler handler = ^ (CGDisplayStreamFrameStatus status, uint64_t display_time, IOSurfaceRef frame_surface, CGDisplayStreamUpdateRef updateRef){ return; };
  CGDisplayStreamRef                   stream = CGDisplayStreamCreate(CGMainDisplayID(), 1, 1, 'BGRA', NULL, handler);

  if (stream == NULL) {
    return(false);
  }
  CFRelease(stream);
  return(true);
}

bool is_authorized_for_accessibility() {
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1090
  return(AXAPIEnabled() || AXIsProcessTrusted());
#else
  return(AXIsProcessTrusted());
#endif
}
