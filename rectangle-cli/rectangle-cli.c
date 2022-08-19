////////////////////////////////////////////
#include "active-app/active-app.h"
#include "app-utils/app-utils.h"
#include "generic-print/print.h"
#include "rectangle-cli/rectangle-cli.h"
#include "timestamp/timestamp.h"
#include "window-utils/window-utils.h"
////////////////////////////////////////////

static struct ctx_t      ctx = {
  .verbose     = false,
  .app         = NULL,
  .width       = -1,
  .todo_switch = false,
};

static struct cag_option options[] = {
  { .identifier     = 'w',
    .access_letters = "w",
    .access_name    = "width",
    .value_name     = "TODO_WIDTH",
    .description    = "Todo App Width", },
  { .identifier     = 'a',
    .access_letters = "a",
    .access_name    = "app",
    .value_name     = "TODO_APP",
    .description    = "Todo App", },
  { .identifier     = 's',
    .access_letters = "s",
    .access_name    = "switch",
    .value_name     = NULL,
    .description    = "Switch to Todo App", },
  { .identifier     = 'V',
    .access_letters = "V",
    .access_name    = "verbose",
    .value_name     = NULL,
    .description    = "Verbose Mode", },
  { .identifier     = 'h',
    .access_letters = "h",
    .access_name    = "help",
    .value_name     = NULL,
    .description    = "Help Mode", },
};

const size_t             sleep_recheck_pid_ms = 500;

int main(const int argc, const char **argv) {
  int orig_focused_pid = get_focused_pid();

  parse_args(argc, argv);

  if (ctx.app) {
    assert(rectangle_set_todo_app(ctx.app) == true);
  }
  if (ctx.width > 0) {
    assert(rectangle_set_todo_width(ctx.width) == true);
  }
  if ((ctx.width > 0) || ctx.app) {
    assert(rectangle_kill() == true);
    usleep(1000 * 10);
    assert(rectangle_run() == true);
    size_t started_ms = (size_t)timestamp();
    usleep(1000 * 10);
    int    cur_focused_pid = get_focused_pid();
    if (ctx.todo_switch == false) {
      is_authorized_for_accessibility();
      size_t dur_ms = ((size_t)timestamp()) - started_ms;
      if (dur_ms < sleep_recheck_pid_ms) {
        usleep(1000 * (sleep_recheck_pid_ms - dur_ms));
      }
      set_focused_pid(orig_focused_pid);
      usleep(1000 * 10);
      cur_focused_pid = get_focused_pid();
      assert((cur_focused_pid == orig_focused_pid));
      if (ctx.verbose) {
        printf("orig focused pid:\t%d\n", orig_focused_pid);
        printf("cur focused pid:\t%d\n", cur_focused_pid);
      }
    }
  }
}

static int parse_args(int argc, char *argv[]){
  cag_option_context context;

  cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&context)) {
    char identifier = cag_option_get(&context);
    switch (identifier) {
    case 'v': ctx.verbose     = true; break;
    case 'w': ctx.width       = atoi(cag_option_get_value(&context)); break;
    case 'a': ctx.app         = cag_option_get_value(&context); break;
    case 's': ctx.todo_switch = true; break;
    case 'h':
      fprintf(stderr, AC_RESETALL AC_YELLOW AC_BOLD "Usage: rec [OPTION]\n" AC_RESETALL);
      cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
      exit(EXIT_SUCCESS);
    }
  }
  return(EXIT_SUCCESS);
} /* parse_args */
