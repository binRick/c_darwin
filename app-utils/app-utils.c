#pragma once
#include "app-utils/app-utils.h"
///////////////////////////////////////////////////////////////////////
static bool              APP_UTILS_DEBUG_MODE = false;
static struct db_cache_t *db                  = NULL;
static struct db_cache_t *init_system_profiler_cache();
static int save_system_profiler_cache(char *SYSTEM_PROFILER_JSON_CONTENT);
static char *load_system_profiler_cache();
///////////////////////////////////////////////////////////////////////
static void __attribute__((constructor)) __constructor__app_utils(void){
  if (getenv("DEBUG") != NULL || getenv("DEBUG_APP_UTILS") != NULL) {
    log_debug("Enabling App Utils Debug Mode");
    APP_UTILS_DEBUG_MODE = true;
  }
  db = init_system_profiler_cache();
}

///////////////////////////////////////////////////////////////////////
char *run_system_profiler_subprocess(){
  char *cached = load_system_profiler_cache();

  if (cached) {
    return(cached);
  }

  char *READ_STDOUT, *READ_STDERR, *timeout;

  asprintf(&timeout, "%d", APP_UTILS_SYSTEM_PROFILER_TIMEOUT);
  int                  exited, result;
  struct subprocess_s  subprocess;
  const char           *command_line[] = {
    (char *)which_path("system_profiler",             getenv("PATH")),
    "-detailLevel",
    APP_UTILS_SYSTEM_PROFILER_DETAIL_LEVEL,
    "-timeout",
    timeout,
    "-json",
    APP_UTILS_SYSTEM_PROFILER_APPLICATIONS_DATA_TYPE,
    NULL,
  };
  char                 stdout_buffer[APP_UTILS_STDOUT_READ_BUFFER_SIZE + 1] = { 0 };
  char                 stderr_buffer[APP_UTILS_STDOUT_READ_BUFFER_SIZE + 1] = { 0 };
  struct  StringBuffer *SB                                                  = stringbuffer_new();
  struct  StringBuffer *SB_err                                              = stringbuffer_new();
  size_t               bytes_read                                           = 0;

  result = subprocess_create(command_line, 0, &subprocess);
  assert(result == 0);
  do {
    bytes_read = subprocess_read_stdout(&subprocess, stdout_buffer, APP_UTILS_STDOUT_READ_BUFFER_SIZE - 1);
    stringbuffer_append_string(SB, stdout_buffer);
  } while (bytes_read != 0);
  bytes_read = 0;
  do {
    bytes_read = subprocess_read_stderr(&subprocess, stderr_buffer, APP_UTILS_STDOUT_READ_BUFFER_SIZE - 1);
    stringbuffer_append_string(SB_err, stderr_buffer);
  } while (bytes_read != 0);

  result = subprocess_join(&subprocess, &exited);
  assert(result == 0);
  READ_STDOUT = stringbuffer_to_string(SB);
  READ_STDERR = stringbuffer_to_string(SB_err);
  assert(strlen(READ_STDOUT) > 0);
  stringbuffer_release(SB);
  stringbuffer_release(SB_err);

  log_info("Read %s stdout", bytes_to_string(strlen(READ_STDOUT)));
  log_info("Read %s stderr", bytes_to_string(strlen(READ_STDERR)));

  save_system_profiler_cache(READ_STDOUT);
  return(READ_STDOUT);
} /* run_system_profiler_subprocess */

static struct db_cache_t *init_system_profiler_cache(){
  struct db_cache_t *db_cache = calloc(1, sizeof(IWKV));
  char              *path;

  asprintf(&path, "%s", APP_UTILS_CACHE_FILE);
  db_cache->opts = (IWKV_OPTS){
    .path   = path,
    .oflags = APP_UTILS_SYSTEM_PROFILER_DB_LOCK_MODE,
  };
  db_cache->rc = iwkv_open(&(db_cache->opts), &(db_cache->iwkv));

  if (db_cache->rc) {
    iwlog_ecode_error3(db_cache->rc);
    log_error("Failed to open %s", db_cache->opts.path);
    return(NULL);
  }
  db_cache->rc = iwkv_db(db_cache->iwkv, APP_UTILS_SYSTEM_PROFILER_DATABASE_ID, 0, &(db_cache->iwdb));
  if (db_cache->rc) {
    iwlog_ecode_error2(db_cache->rc, "Failed to open mydb");
    return(NULL);
  }
  return(db_cache);
}

static char *load_system_profiler_cache(){
  char     *cached_system_profiler_json = NULL;
  IWKV_val key, val;

  key.data = strdup(APP_UTILS_SYSTEM_PROFILER_JSON_CACHE_KEY);
  key.size = strlen(key.data);
  val.data = 0;
  val.size = 0;
  db->rc   = iwkv_get(db->iwdb, &key, &val);
  if (db->rc) {
    //iwlog_ecode_error3(db->rc);
    return(NULL);
  }

  fprintf(stdout, "get: %.*s => %.*s\n",
          (int)key.size, (char *)key.data,
          (int)val.size, (char *)val.data);
  if ((val.size > 0) && (val.data != NULL) && strlen(val.data) > 1024) {
    cached_system_profiler_json = strdup(val.data);
  }

//  iwkv_val_dispose(&val);
  iwkv_close(db->iwkv);
  return(cached_system_profiler_json);
}

static int save_system_profiler_cache(char *SYSTEM_PROFILER_JSON_CONTENT) {
  IWKV_val key, val;

  key.data = strdup(APP_UTILS_SYSTEM_PROFILER_JSON_CACHE_KEY);
  key.size = strlen(key.data);
  val.data = strdup(SYSTEM_PROFILER_JSON_CONTENT);
  val.size = strlen(val.data);

  fprintf(stdout, "put: %.*s => %.*s\n",
          (int)key.size, (char *)key.data,
          (int)val.size, (char *)val.data);

  db->rc = iwkv_put(db->iwdb, &key, &val, 0);
  if (db->rc) {
    iwlog_ecode_error3(db->rc);
    return(db->rc);
  }

  return(EXIT_SUCCESS);
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
struct Vector *get_installed_apps_v(){
  struct Vector *a   = vector_new();
  char          *out = run_system_profiler_subprocess();

  //log_debug("%s",out);
  return(a);
}

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
