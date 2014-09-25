/*
   The MIT License (MIT)

   Copyright (c) 2014 Zachary Seguin

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <pebble.h>

#include "stop_details.h"

#include "log.h"

#define MENU_SECTIONS 2
#define MENU_ITEMS_SECTION_1 1
#define MENU_ITEMS_SECTION_2 5

struct stop_details
{
   int stop_id;
   
   Window *window;
   
   SimpleMenuLayer *simple_menu_layer;
   
   SimpleMenuSection sections[MENU_SECTIONS];
   SimpleMenuItem items1[MENU_ITEMS_SECTION_1];
   SimpleMenuItem items2[MENU_ITEMS_SECTION_2];
} __attribute__((aligned(1)));

static void stop_details_handle_window_load(Window *window)
{
   StopDetails sd = (StopDetails)window_get_user_data(window);
   
   // Init GUI components
   info("Initializing 'stop_details' GUI components");
   
   Layer *window_layer = window_get_root_layer(window);
   GRect bounds = layer_get_frame(window_layer);
   
   // Create menu sections
   int section = 0;
   int item = 0;
   
   sd->sections[section++] = (SimpleMenuSection) {
     .title = "Stop Details",
     .items = sd->items1,
     .num_items = MENU_ITEMS_SECTION_1
   };
   
   static char *stop_id = "####";
   snprintf(stop_id, sizeof(stop_id) + 1, "%d", sd->stop_id);
   
   sd->items1[item++] = (SimpleMenuItem) {
      .title = "Stop ID",
      .subtitle = stop_id
   };
   
   item = 0;
   sd->sections[section++] = (SimpleMenuSection) {
     .title = "Next Buses",
     .items = sd->items2,
     .num_items = MENU_ITEMS_SECTION_2
   };
   sd->items2[item++] = (SimpleMenuItem) {
      .title = "--",
      .subtitle = "--" 
   };
   sd->items2[item++] = (SimpleMenuItem) {
      .title = "--",
      .subtitle = "--" 
   };
   sd->items2[item++] = (SimpleMenuItem) {
      .title = "--",
      .subtitle = "--" 
   };
   sd->items2[item++] = (SimpleMenuItem) {
      .title = "--",
      .subtitle = "--" 
   };
   sd->items2[item++] = (SimpleMenuItem) {
      .title = "--",
      .subtitle = "--" 
   };
   
   sd->simple_menu_layer = simple_menu_layer_create(bounds, window, sd->sections, MENU_SECTIONS, sd);
   layer_add_child(window_layer, simple_menu_layer_get_layer(sd->simple_menu_layer));
}// End of stop_details_handle_window_load method

static void stop_details_handle_window_unload(Window *window)
{
   StopDetails sd = (StopDetails)window_get_user_data(window);
   
   // Unload GUI components
   info("Destroying 'stop_details' GUI components");
   
   simple_menu_layer_destroy(sd->simple_menu_layer);
}// End of stop_details_handle_window_unload method

StopDetails stop_details_create(int stop_id)
{
   info("Creating 'stop_details' object");
   
   StopDetails sd = (StopDetails)malloc(sizeof(struct stop_details));
   
   sd->stop_id = stop_id;
   
   // Configure window
   sd->window = window_create();
   
   if (!(sd->window)) error("Unable to allocate memory for 'window' object");
   
   window_set_fullscreen(sd->window, false);
   window_set_window_handlers(sd->window, (WindowHandlers) {
      .load = stop_details_handle_window_load,
      .unload = stop_details_handle_window_unload
   });
   window_set_user_data(sd->window, sd);
   
   return sd;
}// End of stop_details_create method

void stop_details_destroy(StopDetails sd)
{
   info("Destroying 'main_menu' object");
   
   window_destroy(sd->window);
   free(sd);
}// End of stop_details_destroy method

void stop_details_show(StopDetails sd)
{
   info("Showing 'stop_details' window");
   window_stack_push(sd->window, true);
}// End of stop_details_show method

void stop_details_hide(StopDetails sd)
{
   info("Hiding 'stop_details' window");
   window_stack_remove(sd->window, true);
}// End of stop_details_hide method
