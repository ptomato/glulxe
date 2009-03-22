/* main.c: Glulxe top-level code.
    Designed by Andrew Plotkin <erkyrath@netcom.com>
    http://www.eblong.com/zarf/glulx/index.html
*/

#include "glk.h"
#include "glulxe.h"

strid_t gamefile = NULL;

/* glk_main():
   The top-level routine. This does everything, and consequently is
   very simple. 
*/
void glk_main()
{
  if (!is_gamefile_valid()) {
    return;
  }

  glulx_setrandom(0);
  if (!init_dispatch()) {
    return;
  }

  setup_vm();
  execute_loop();
  finalize_vm();

  glk_exit();
}

/* get_error_win():
   Return a window in which to display errors. The first time this is called,
   it creates a new window; after that it returns the window it first
   created.
*/
static winid_t get_error_win()
{
  static winid_t errorwin = NULL;

  if (!errorwin) {
    winid_t rootwin = glk_window_get_root();
    if (!rootwin) {
      errorwin = glk_window_open(0, 0, 0, wintype_TextBuffer, 1);
    }
    else {
      errorwin = glk_window_open(rootwin, winmethod_Below | winmethod_Fixed, 
        3, wintype_TextBuffer, 0);
    }
    if (!errorwin)
      errorwin = rootwin;
  }

  return errorwin;
}

/* fatal_error_handler():
   Display an error in the error window, and then exit.
*/
void fatal_error_handler(char *str, char *arg, int useval, glsi32 val)
{
  winid_t win = get_error_win();
  if (win) {
    glk_set_window(win);
    glk_put_string("Glulxe fatal error: ");
    glk_put_string(str);
    if (arg || useval) {
      glk_put_string(" (");
      if (arg)
        glk_put_string(arg);
      if (arg && useval)
        glk_put_string(" ");
      if (useval)
        stream_hexnum(val);
      glk_put_string(")");
    }
    glk_put_string("\n");
  }
  glk_exit();
}

/* nonfatal_warning_handler():
   Display a warning in the error window, and then continue.
*/
void nonfatal_warning_handler(char *str, char *arg, int useval, glsi32 val)
{
  winid_t win = get_error_win();
  if (win) {
    strid_t oldstr = glk_stream_get_current();
    glk_set_window(win);
    glk_put_string("Glulxe warning: ");
    glk_put_string(str);
    if (arg || useval) {
      glk_put_string(" (");
      if (arg)
        glk_put_string(arg);
      if (arg && useval)
        glk_put_string(" ");
      if (useval)
        stream_hexnum(val);
      glk_put_string(")");
    }
    glk_put_string("\n");
    glk_stream_set_current(oldstr);
  }
}

