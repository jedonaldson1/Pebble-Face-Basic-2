#include <pebble.h>

/*
  This code was written by Jordan Donaldson. It was written in CloudPebble using the Pebble SDK, 
  C, and JavaScript. It has the following functionalities: time, date, weather, battery level, 
  Pebble Health, and vibration on disconnection.
*/

//Variable declarations
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GFont s_time_font;
static GFont s_date_font;
static TextLayer *s_weather_layer;
static GFont s_weather_font;
static int s_battery_level;

//Updates the time
static void update_time()
{
  //Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  //Write the current hours and minutes into a bufer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  //Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  //Writes the current date into a buffer
  static char s_buffer_2[12];
  strftime(s_buffer_2, sizeof(s_buffer_2), "%a %b %e", tick_time);
  text_layer_set_text(s_date_layer, s_buffer_2);
}

//Manages the updating of the time
static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  update_time();
}

//Manages construction of Window's sub-elements
static void main_window_load(Window *window)
{
  //Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //Create the TextLayer for the time with specific bounds
  s_time_layer = text_layer_create(GRect(0, 52, bounds.size.w, 50));
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GEOSANS_LIGHT_44));
  //Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  //Add as a child to Window root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  //Create the TextLayer for the date with specific bounds
  s_date_layer = text_layer_create(GRect(0, 105, bounds.size.w, 50));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GEOSANS_LIGHT_20));
  //Improve layout
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  //Add as a child to Window root layer
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  
  //Create the TextLayer for the weather with specific bounds
  s_weather_layer = text_layer_create(GRect(0, 0, bounds.size.w, 25));
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GEOSANS_LIGHT_18));
  //Improve layout
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentLeft);
  //text_layer_set_text(s_weather_layer, "Weather");
  text_layer_set_font(s_weather_layer, s_weather_font);
  //Add as a child to the Window root laye
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
}

//Manages deconstruction of Window's sub-elements
static void main_window_unload(Window *window)
{
  //Destroy time layer
  text_layer_destroy(s_time_layer);
  //Destroy date layer
  text_layer_destroy(s_date_layer);
  //Destroy weather layer
  text_layer_destroy(s_weather_layer);
  
  //Destroy time font
  fonts_unload_custom_font(s_time_font);
  //Destroy date font
  fonts_unload_custom_font(s_date_font);
  //Destroy weather font
  fonts_unload_custom_font(s_weather_font);
}

//AppMessage stuff
static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
  //Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  //Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
  
  //If all data is available, use it
  if(temp_tuple && conditions_tuple)
  {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°F", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
    //Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
  }
}
static void inbox_dropped_callback(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}
static void outbox_sent_callback(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
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
  //Sets background color of window
  window_set_background_color(s_main_window,GColorJazzberryJam);
  
  //Make sure the time is displayed from the start
  update_time();
  //Register with TickTimer service
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  //Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  //Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
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