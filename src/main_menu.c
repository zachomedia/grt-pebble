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

#include "main_menu.h"
#include "stop_selection.h"
#include "stop_details.h"

#include "log.h"

#define MENU_SECTIONS 2
#define MENU_ITEMS_SECTION_1 1
#define MENU_ITEMS_SECTION_2 2

struct main_menu
{
   Window *window;
   
   SimpleMenuLayer *simple_menu_layer;
   
   SimpleMenuSection sections[MENU_SECTIONS];
   SimpleMenuItem items1[MENU_ITEMS_SECTION_1];
   SimpleMenuItem items2[MENU_ITEMS_SECTION_2];
   
   StopSelection ss;
   StopDetails sd;
} __attribute__((aligned(1)));

static void show_stop_schedule(int stop_id, void *context)
{
   MainMenu mm = (MainMenu)context;
   
   info("Stop selected: %d", stop_id);
   
   stop_selection_destroy(mm->ss);
   mm->ss = NULL;
   
   if (mm->sd) stop_details_destroy(mm->sd);
   mm->sd = stop_details_create(stop_id);
   stop_details_show(mm->sd);
}// End of show_stop_schedule method

static void stop_schedule_selected(int index, void *context)
{
   MainMenu mm = (MainMenu)context;
   
   info("Showing 'stop_selection' to get a stop id");
   
   mm->ss = stop_selection_create(show_stop_schedule, NULL, mm);
   stop_selection_show(mm->ss);
}// End of show_stop_schedule method

static void main_menu_handle_window_load(Window *window)
{
   MainMenu mm = (MainMenu)window_get_user_data(window);
   
   // Init GUI components
   info("Initializing 'main_menu' GUI components");
   
   Layer *window_layer = window_get_root_layer(window);
   GRect bounds = layer_get_frame(window_layer);
   
   // Create menu sections
   int section = 0;
   int item = 0;
   
   mm->sections[section++] = (SimpleMenuSection) {
     .title = "GRT Schedule",
     .items = mm->items1,
     .num_items = MENU_ITEMS_SECTION_1
   };
   mm->items1[item++] = (SimpleMenuItem) {
      .title = "Stop Schedule...",
      .subtitle = "View schedule for a stop",
      .callback = stop_schedule_selected,
   };
   
   item = 0;
   mm->sections[section++] = (SimpleMenuSection) {
     .title = "About",
     .items = mm->items2,
     .num_items = MENU_ITEMS_SECTION_2
   };
   mm->items2[item++] = (SimpleMenuItem) {
      .title = "Author",
      .subtitle = "Zachary Seguin" 
   };
   mm->items2[item++] = (SimpleMenuItem) {
      .title = "Webiste",
      .subtitle = "zacharyseguin.ca" 
   };
   
   mm->simple_menu_layer = simple_menu_layer_create(bounds, window, mm->sections, MENU_SECTIONS, mm);
   layer_add_child(window_layer, simple_menu_layer_get_layer(mm->simple_menu_layer));
}// End of main_menu_handle_window_load method

static void main_menu_handle_window_unload(Window *window)
{
   MainMenu mm = (MainMenu)window_get_user_data(window);
   
   // Unload GUI components
   info("Destroying 'main_menu' GUI components");
   
   simple_menu_layer_destroy(mm->simple_menu_layer);
}// End of main_menu_handle_window_unload method

MainMenu main_menu_create(void)
{
   info("Creating 'main_menu' object");
   
   MainMenu mm = (MainMenu)malloc(sizeof(struct main_menu));
   
   // Configure window
   mm->window = window_create();
   
   if (!(mm->window)) error("Unable to allocate memory for 'window' object");
   
   window_set_fullscreen(mm->window, false);
   window_set_window_handlers(mm->window, (WindowHandlers) {
      .load = main_menu_handle_window_load,
      .unload = main_menu_handle_window_unload
   });
   window_set_user_data(mm->window, mm);
   
   return mm;
}// End of main_menu_create method

void main_menu_destroy(MainMenu mm)
{
   info("Destroying 'main_menu' object");
   
   window_destroy(mm->window);
   if (mm->sd) stop_details_destroy(mm->sd);
   
   free(mm);
}// End of main_menu_destroy method

void main_menu_show(MainMenu mm)
{
   info("Showing 'main_menu' window");
   window_stack_push(mm->window, true);
}// End of main_menu_show method

void main_menu_hide(MainMenu mm)
{
   info("Hiding 'main_menu' window");
   window_stack_remove(mm->window, true);
}// End of main_menu_hide method
