////////////////////////////////////////////

#ifdef DEBUG_MEMORY
#include "debug-memory/debug_memory.h"
#endif
////////////////////////////////////////////
#include "mac-load-test/mac-load-test.h"
////////////////////////////////////////////
#include "ansi-codes/ansi-codes.h"
#include "c_fsio/include/fsio.h"
#include "c_greatest/greatest/greatest.h"
#include "c_stringfn/include/stringfn.h"
#include "c_vector/include/vector.h"
#include "mac_load/mac_load.h"

void MAC_Handler(OnRect, NSRect rect) {
  set(keyboardFocusIndicatorColor(_(NSColor)));
  const int n = 12;
  for (double f = 0; f < 2 * M_PI; f += 2 * M_PI / n) {
    for (double g = 0; g < 2 * M_PI; g += 2 * M_PI / n) {
      NSPoint begin = { (sin(f) + 1) * rect.size.width * 0.5,
                        (cos(f) + 1) * rect.size.height * 0.5 };
      NSPoint end = { (sin(g) + 1) * rect.size.width * 0.5,
                      (cos(g) + 1) * rect.size.height * 0.5 };
      strokeLineFromPoint_toPoint_(_(NSBezierPath), begin, end);
    }
  }
}

void MAC_Handler(OnClose, __attribute__((unused)) NSNotification *notification) {
  stop_(sharedApplication(_(NSApplication)), self);
}

int test_mac_load() {
  NSApplication *thrd = sharedApplication(_(NSApplication));

  setActivationPolicy_(thrd, NSApplicationActivationPolicyAccessory);
  //setActivationPolicy_(thrd, NSApplicationActivationPolicyRegular);
  // use MAC_LoadClass("nAmE") if unable to pass MyView to where it is needed
  Class MyView = MAC_MakeClass("nAmE", _(NSView), /* extra fields = */ nil,
                               MAC_TempArray(_(drawRect_), OnRect,
                                             _(windowShouldClose_), OnClose));
  NSAutoreleasePool *myPool = init(alloc(_(NSAutoreleasePool)));

  NSRect            rect = { { 100.0, 350.0 }, { 400.0, 400.0 } };
//    NSInteger ap = NSApplicationActivationPolicyRegular;
  NSInteger         mask = NSTitledWindowMask | NSClosableWindowMask
                           | NSMiniaturizableWindowMask;
  NSWindow          *myWnd = autorelease(initWithContentRect_styleMask_backing_defer_(
                                           alloc(_(NSWindow)), rect, mask, kCGBackingStoreBuffered, NO));

//setHidden_(myWnd,true);
  setTitle_(myWnd, MAC_ConstString("Pure C Application Window"));

  NSView *myView = autorelease(init(alloc(MyView)));

  setContentView_(myWnd, myView);
  setDelegate_(myWnd, (id)myView);
  orderFront_(myWnd, nil);
  makeKeyWindow(myWnd);

  run(thrd);

  release(myPool);

  MAC_FreeClass(MyView);   // segfaults if called before all instances get freed
  return(EXIT_SUCCESS);
}
////////////////////////////////////////////
static char EXECUTABLE_PATH[PATH_MAX + 1] = { 0 };
static char *EXECUTABLE;
static char *EXECUTABLE_PATH_DIRNAME;
static bool DEBUG_MODE_ENABLED = false;
void __attribute__((constructor)) __constructor__mac_load_test();
void __attribute__((destructor)) __destructor__mac_load_test();
void __mac_load_test__setup_executable_path(const char **argv);

TEST t_mac_load_test(void *PARAMETER){
  char *msg         = NULL;
  int  tested_value = -1;

  test_mac_load();
  tested_value = 1;
  ASSERT_EQm("Test Value should equal One", tested_value, 1);
  asprintf(&msg, "Test mac-load-test OK\n\t(%s) @ (%s)\n\tValidated tested_value: %d",
           EXECUTABLE,
           EXECUTABLE_PATH_DIRNAME,
           tested_value
           );
  PASSm(msg);
}

SUITE(s_mac_load_test) {
  void *TEST_PARAM = 0;

  RUN_TESTp(t_mac_load_test, (void *)TEST_PARAM);
}

GREATEST_MAIN_DEFS();

int main(const int argc, const char **argv) {
  __mac_load_test__setup_executable_path(argv);
  GREATEST_MAIN_BEGIN();
  if (isatty(STDOUT_FILENO)) {
  }
  RUN_SUITE(s_mac_load_test);
  GREATEST_MAIN_END();
}

void __mac_load_test__setup_executable_path(const char **argv){
  realpath(argv[0], EXECUTABLE_PATH);
  EXECUTABLE              = basename(EXECUTABLE_PATH);
  EXECUTABLE_PATH_DIRNAME = dirname(EXECUTABLE_PATH);
  if (DEBUG_MODE_ENABLED) {
    fprintf(stderr, "mac-load-test Initialized @ %s/%s\n", EXECUTABLE_PATH_DIRNAME, EXECUTABLE_PATH);
  }
}

void __attribute__((constructor)) __constructor__mac_load_test(){
  DEBUG_MODE_ENABLED = (getenv("DEBUG")) ? true
      : (getenv("DEBUG_MODE")) ? true
        : (getenv("DEBUGMODE")) ? true
          : false;
}

void __attribute__((destructor)) __destructor__mac_load_test(){
#ifdef DEBUG_MEMORY
  fprintf(stderr, "\nChecking mac-load-test memory leaks\n");
  print_allocated_memory();
#endif
}
