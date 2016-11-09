#include <pebble.h>

//Variable declarations
static Window *s_main_window;

//Manages construction of Window's sub-elements
static void main_window_load(Window *window)
{
  //code
}

//Manages deconstruction of Window's sub-elements
static void main_window_unload(Window *window)
{
  //code
}

//Handles main construction of watchface
static void init()
{
  //Create main Window element and assign to pointer
  s_main_window = window_create();
  //Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers)
  {
    .load = main_window_load,
    .unload = main_window_unload
  });
  //Show the window on the watch, with animated = true
  window_stack_push(s_main_window, true);
}

//Handles main deconstruction of watchface
static void deinit()
{
  //Destroy Window
  window_destroy(s_main_window);
}

//Just calls init and deinit to build face, wait for action, and
//  destroy face
int main(void)
{
  init();
  app_event_loop();
  deinit();
}