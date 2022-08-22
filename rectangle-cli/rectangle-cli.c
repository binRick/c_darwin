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
  .mode        = "configure",
};

static struct cag_option options[] = {
  { .identifier     = 'm',
    .access_letters = "m",
    .access_name    = "mode",
    .value_name     = "MODE",
    .description    = "Mode", },
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
  { .identifier     = 'r',
    .access_letters = "r",
    .access_name    = "resize-non-todo-windows",
    .value_name     = NULL,
    .description    = "Resize Non Todo Windows", },
  { .identifier     = 's',
    .access_letters = "s",
    .access_name    = "switch",
    .value_name     = NULL,
    .description    = "Switch to Todo App", },
  { .identifier     = 'v',
    .access_letters = "v",
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
int                      orig_focused_pid;

int resize_non_todo_windows(){
  int           display_width = get_display_width();
  int           todo_width    = rectangle_get_todo_width();
  int           todo_pid      = rectangle_get_pid();
  char          *todo_app     = rectangle_get_todo_app();
  int           new_width     = display_width - todo_width;
  int           new_x         = 0;
  int           new_y         = 25;
  struct Vector *windows      = get_windows();

  for (int i = 0; i < vector_size(windows); i++) {
    struct window_t *w = vector_get(windows, i);
    if (strcmp(w->app_name, todo_app) == 0) {
      continue;
    }
    char *todo_app_app;
    asprintf(&todo_app_app, "%s.app", todo_app);
    if (strcmp(w->app_name, todo_app_app) == 0) {
      continue;
    }
    if (w->pid == todo_pid) {
      continue;
    }
    if ((w->width == new_width) && ((int)w->position.x == new_x) && ((int)w->position.y) == new_y) {
      continue;
    }
    if (w->layer != 0) {
      continue;
    }
    if (w->is_visible != true) {
      continue;
    }
    if (ctx.verbose == true) {
      printf("[todo:%s] Resizing %s> %d|%d from %dx%d to %dx%d | moving from %dx%d to %dx%d\n",
             todo_app,
             w->app_name,
             w->pid, w->window_id,
             w->width, w->height,
             new_width, w->height,
             (int)w->position.x, (int)w->position.y,
             new_x, new_y
             );
    }
    move_window(w->window, new_x, new_y);
    usleep(1000 * 1);
    resize_window(w->window, new_width, w->height);
    usleep(1000 * 10);
  }
  return(EXIT_SUCCESS);
} /* resize_non_todo_windows */

int info_todo(){
  int             pid = get_focused_pid();
  struct window_t *W  = get_pid_window(pid);

  fprintf(stdout, "=========================================\n");
  fprintf(stdout, "Rectangle PID              :         %d\n", rectangle_get_pid());
  fprintf(stdout, "Current Todo App           :         %s\n", rectangle_get_todo_app());
  fprintf(stdout, "Current Todo Width         :         %d\n", rectangle_get_todo_width());
  fprintf(stdout, "Todo Mode Enabled          :         %s\n", rectangle_get_todo_mode_enabled() ? "Yes" : "No");
  fprintf(stdout, "=========================================\n");
  fprintf(stdout, "Focused Window PID         :         %d\n", W->pid);
  fprintf(stdout, "Focused Window ID          :         %d\n", W->window_id);
  fprintf(stdout, "Focused Window Name        :         %s\n", W->window_name);
  fprintf(stdout, "Focused App Name           :         %s\n", W->app_name);
  fprintf(stdout, "Focused Window Position    :         %dx%d\n", (int)W->position.x, (int)W->position.y);
  fprintf(stdout, "Focused Window Layer       :         %d\n", W->layer);
  fprintf(stdout, "Focused Window Size        :         %dx%d\n", W->width, W->height);
  fprintf(stdout, "Focused Window Focused     :         %s\n", W->is_focused ? "Yes" : "No");
  fprintf(stdout, "Focused Window Visible     :         %s\n", W->is_visible ? "Yes" : "No");
  fprintf(stdout, "=========================================\n");
  fprintf(stdout, "Display Width              :         %d\n", get_display_width());
  fprintf(stdout, "=========================================\n");
  return(EXIT_SUCCESS);
}

int configure_todo(){
  int orig_focused_pid = get_focused_pid();

  if (ctx.app) {
    assert(rectangle_set_todo_app(ctx.app) == true);
  }
  if (ctx.width > 0) {
    assert(rectangle_set_todo_width(ctx.width) == true);
  }
  if ((ctx.width > 0) || ctx.app) {
    assert(rectangle_kill() == true);
    usleep(1000 * 50);
    assert(rectangle_run() == true);
    size_t started_ms = (size_t)timestamp();
    usleep(1000 * 50);
    int    cur_focused_pid = get_focused_pid();
    if (ctx.todo_switch == false) {
      size_t dur_ms = ((size_t)timestamp()) - started_ms;
      if (dur_ms < sleep_recheck_pid_ms) {
        usleep(1000 * (sleep_recheck_pid_ms - dur_ms));
      }
      set_focused_pid(orig_focused_pid);
      usleep(1000 * 50);
      cur_focused_pid = get_focused_pid();
      assert((cur_focused_pid == orig_focused_pid));
      if (ctx.verbose == true) {
        printf("orig focused pid:\t%d\n", orig_focused_pid);
        printf("cur focused pid:\t%d\n", cur_focused_pid);
      }
    }
  }
  if (ctx.resize_non_todo_windows == true) {
    resize_non_todo_windows();
  }
  return(EXIT_SUCCESS);
}

int main(const int argc, const char **argv) {
  parse_args(argc, argv);
  usleep(1000 * 100);
  is_authorized_for_accessibility();
  usleep(1000 * 100);
  if (strcmp(ctx.mode, "configure") == 0) {
    return(configure_todo());
  }else if (strcmp(ctx.mode, "info") == 0) {
    return(info_todo());
  }else if (strcmp(ctx.mode, "resize") == 0) {
    return(resize_non_todo_windows());
  }
  return(1);
}

static int parse_args(int argc, char *argv[]){
  cag_option_context context;

  cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&context)) {
    char identifier = cag_option_get(&context);
    switch (identifier) {
    case 'v': ctx.verbose                 = true; break;
    case 'w': ctx.width                   = atoi(cag_option_get_value(&context)); break;
    case 'r': ctx.resize_non_todo_windows = true; break;
    case 'a': ctx.app                     = cag_option_get_value(&context); break;
    case 'm': ctx.mode                    = cag_option_get_value(&context); break;
    case 's': ctx.todo_switch             = true; break;
    case 'h':
      fprintf(stderr, AC_RESETALL AC_YELLOW AC_BOLD "Usage: rec [OPTION]\n" AC_RESETALL);
      cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
      exit(EXIT_SUCCESS);
    }
  }
  return(EXIT_SUCCESS);
} /* parse_args */
