#pragma once
#include "app-utils/app-utils.h"
#include "app/app.h"

#include "date.c/date.h"
#include "submodules/reproc/reproc/include/reproc/export.h"
#include "submodules/reproc/reproc/include/reproc/reproc.h"
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
static int save_system_profiler_cache(char *SYSTEM_PROFILER_JSON_CONTENT);
static char *load_system_profiler_cache();
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__app_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_APP_UTILS") != NULL) {
    log_debug("Enabling App Utils Debug Mode");
    APP_UTILS_DEBUG_MODE = true;
  }
}

///////////////////////////////////////////////////////////////////////
char *run_system_profiler_subprocess(){
  char *cached = load_system_profiler_cache();

  if ((cached != NULL) && (strlen(cached) > APP_UTILS_SYSTEM_PROFILER_JSON_CACHE_MINIMUM_SIZE_BYTES) && (cached[0] == '{') && (cached[strlen(cached) - 1] == '}')) {
    log_debug("returning cache!");
    return(cached);
  }
  enum { NUM_CHILDREN = 1 };

  reproc_event_source  children[NUM_CHILDREN] = { { 0 } };
  int                  r                      = -1;
  struct  StringBuffer *SB                    = stringbuffer_new();

  for (int i = 0; i < NUM_CHILDREN; i++) {
    reproc_t   *process = reproc_new();
    const char *args[]  = { (char *)which_path("system_profiler", getenv("PATH")), "-detailLevel", "mini", "-json", "SPApplicationsDataType", NULL };

    r = reproc_start(process, args, (reproc_options){ .nonblocking = true });
    if (r < 0) {
      goto finish;
    }

    children[i].process   = process;
    children[i].interests = REPROC_EVENT_OUT;
  }

  for ( ;;) {
    r = reproc_poll(children, NUM_CHILDREN, REPROC_INFINITE);
    if (r < 0) {
      r = r == REPROC_EPIPE ? 0 : r;
      goto finish;
    }

    for (int i = 0; i < NUM_CHILDREN; i++) {
      if (children[i].process == NULL || !children[i].events) {
        continue;
      }
      uint8_t output[1024 * 32];
      r = reproc_read(children[i].process, REPROC_STREAM_OUT, output,
                      sizeof(output));
      if (r == REPROC_EPIPE) {
        children[i].process = reproc_destroy(children[i].process);
        continue;
      }

      if (r < 0) {
        goto finish;
      }

      output[r] = '\0';
      stringbuffer_append_string(SB, output);
    }
  }

finish:
  for (int i = 0; i < NUM_CHILDREN; i++) {
    reproc_destroy(children[i].process);
  }

  if (r < 0) {
    fprintf(stderr, "%s\n", reproc_strerror(r));
  }
  char *output = stringbuffer_to_string(SB);

  stringbuffer_release(SB);
  save_system_profiler_cache(output);
  return(output);
} /* run_system_profiler_subprocess */

#define INIT_SYSTEM_PROFILER_CACHE()                        \
  IWKV_OPTS opts = {                                        \
    .path   = APP_UTILS_CACHE_FILE,                         \
    .oflags = APP_UTILS_SYSTEM_PROFILER_DB_LOCK_MODE,       \
  };                                                        \
  IWKV iwkv;                                                \
  IWDB mydb;                                                \
  iwrc rc = iwkv_open(&opts, &iwkv);                        \
  if (rc) {                                                 \
    iwlog_ecode_error3(rc);                                 \
    goto fail;                                              \
  }                                                         \
  rc = iwkv_db(iwkv, 1, 0, &mydb);                          \
  if (rc) {                                                 \
    iwlog_ecode_error2(rc, "Failed to open mydb");          \
    goto fail;                                              \
  }                                                         \
  IWKV_val key, val;                                        \
  IWKV_val tskey, tsval; size_t ts = 0;                     \
  tskey.data = APP_UTILS_SYSTEM_PROFILER_JSON_TS_CACHE_KEY; \
  tskey.size = strlen(tskey.data);                          \
  key.data   = APP_UTILS_SYSTEM_PROFILER_JSON_CACHE_KEY;    \
  key.size   = strlen(key.data);

static char *load_system_profiler_cache(){
  INIT_SYSTEM_PROFILER_CACHE()
  val.data   = 0;
  val.size   = 0;
  tsval.data = 0;
  tsval.size = 0;
  rc         = iwkv_get(mydb, &key, &val);
  if (rc) {
    iwlog_ecode_error3(rc);
    goto fail;
  }
  rc = iwkv_get(mydb, &tskey, &tsval);
  if (rc) {
    iwlog_ecode_error3(rc);
    goto fail;
  }
  char **ep;
  ts = (size_t)strtoimax(tsval.data, &ep, 10);
  char *s = strdup(val.data);
  iwkv_val_dispose(&val);
  iwkv_close(&iwkv);
  bool expired = ((((size_t)timestamp() - ts) / 1000) > APP_UTILS_SYSTEM_PROFILER_TTL_SECONDS) ? true : false;
  log_debug("%s cache with ts %lu|age: %s|expired:%d|", bytes_to_string(strlen(s)), ts, milliseconds_to_string(((size_t)timestamp()) - ts), expired);
  if (expired == true) {
    return(NULL);
  }

  return(s);

fail:
  return(NULL);
}

static int save_system_profiler_cache(char *SYSTEM_PROFILER_JSON_CONTENT) {
  INIT_SYSTEM_PROFILER_CACHE()
  val.data = SYSTEM_PROFILER_JSON_CONTENT;
  val.size = strlen(val.data);
  rc       = iwkv_put(mydb, &key, &val, 0);
  if (rc) {
    iwlog_ecode_error3(rc);
    goto fail;
  }
  asprintf(&tsval.data, "%lu", (size_t)timestamp());
  tsval.size = strlen(tsval.data);
  rc         = iwkv_put(mydb, &tskey, &tsval, 0);
  if (rc) {
    iwlog_ecode_error3(rc);
    goto fail;
  }
  iwkv_close(&iwkv);
  return(EXIT_SUCCESS);

fail:
  return(EXIT_FAILURE);
}

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
  char          *out = run_system_profiler_subprocess();

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
        log_info("App #%lu"
                 "\n\tName             :   %s"
                 "\n\tVersion          :   %s"
                 "\n\tLast Modified    :   %s (%ld) %s"
                 "\n\tInfo             :   %s"
                 "\n\tPath             :   %s (%s" AC_RESETALL ")"
                 "\n\tArch             :   %s"
                 "\n\tObtained from    :   %s"
                 "%s",
                 i + 1,
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
        JSON_Object *app_object = json_array_get_object(apps_array, i);
        size_t      app_keys_qty = json_object_get_count(app_object);
        char        *app_name = NULL, *app_ver = NULL, *app_path = NULL, *app_last_modified = NULL;
        if (json_object_has_value_of_type(app_object, "_name", JSONString)) {
          app_name = json_object_get_string(app_object, "_name");
        }
        if (json_object_has_value_of_type(app_object, "path", JSONString)) {
          app_path = json_object_get_string(app_object, "path");
        }
        if (json_object_has_value_of_type(app_object, "lastModified", JSONString)) {
          app_last_modified = json_object_get_string(app_object, "lastModified");
          timelib_time *t = timelib_strtotime(app_last_modified, strlen(app_last_modified), NULL, timelib_builtin_db(), timelib_parse_tzfile);
          timelib_update_ts(t, NULL);
          if (APP_UTILS_DEBUG_MODE == true) {
            log_info("%s|"
                     "|y:%lld|"
                     "|m:%lld|"
                     "|d:%lld|"
                     "|h:%lld|"
                     "|i:%lld|"
                     "|s:%lld|"
                     "|sse:%lld|"
                     "%s",
                     app_last_modified,
                     t->y,
                     t->m,
                     t->d,
                     t->h,
                     t->i,
                     t->s,
                     t->sse,
                     ""
                     );
          }
        }
        if (json_object_has_value_of_type(app_object, "version", JSONString)) {
          app_ver = json_object_get_string(app_object, "version");
        }
        if (app_path != NULL && (
              stringfn_starts_with(app_path, "/Library/")
              || stringfn_starts_with(app_path, "/System/")
              || stringfn_starts_with(app_path, "/usr/local/Cellar/python")
              || (fsio_path_exists(app_path) == false)
              )) {
          continue;
        }

        if (APP_UTILS_DEBUG_MODE == true) {
          log_info(
            "app #%lu/%lu>  [" AC_BLUE "%s" AC_RESETALL "]"
            "\n\t|ver:" AC_RESETALL AC_YELLOW "%s" AC_RESETALL "|"
            "\n\t|path:" AC_RESETALL AC_GREEN "%s" AC_RESETALL "|"
            "\n\t|Last Modified:" AC_RESETALL AC_CYAN "%s" AC_RESETALL "|"
            "\n\t%lu keys|"
            "\n\t|ts:%lld|"
            "%s",
            i, apps_qty,
            app_name,
            app_ver,
            app_path,
            app_last_modified,
            app_keys_qty,
            date_now(),
            ""
            );
        }
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
