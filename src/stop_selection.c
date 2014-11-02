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

#include "stop_selection.h"

#include "log.h"

#define DIGITS_LENGTH 4
struct stop_selection
{
   StopSelectionCompleteCallback success_callback;
   StopSelectionCancelledCallback failure_callback;
   void *context;
   
   Window *window;
   
   char *digits[DIGITS_LENGTH];
   TextLayer *digit_layers[DIGITS_LENGTH];
   int active_digit;
} __attribute__((aligned(1)));

static int ipow(int a, int b)
{
   return (b == 0) ? 1 : a * ipow(a, b - 1);
}// End of ipow method

static int stop_selection_get_stop_id(StopSelection ss)
{
   debug("Getting selected stop_id from digits");
   
   int stop_id = 0;
   
   for (int x = 0; x < DIGITS_LENGTH; ++x)
   {
      int digit = (int)(text_layer_get_text(ss->digit_layers[x])[0] - '0');
      stop_id += digit * ipow(10, DIGITS_LENGTH - 1 - x);
   }// End of for
   
   return stop_id;
}// End of stop_selection_get_stop_id method

static void stop_selection_activate_digit(StopSelection ss, int digit)
{
   if (digit < 0 || digit > DIGITS_LENGTH)
   {
      warn("Invalid digit index: %d", digit);
      return;
   }// End of if
   
   info("Changing active digit to: %d", digit);
   
   // deactivate the previous digit
   text_layer_set_background_color(ss->digit_layers[ss->active_digit], GColorWhite);
   text_layer_set_text_color(ss->digit_layers[ss->active_digit], GColorBlack);
   
   // activate the new digit
   ss->active_digit = digit;
   
   text_layer_set_background_color(ss->digit_layers[ss->active_digit], GColorBlack);
   text_layer_set_text_color(ss->digit_layers[ss->active_digit], GColorWhite);
}// End of stop_selection_activate_digit method

static TextLayer *create_digit_layer(GRect frame, const char *text)
{
   debug("Creating 'digit_layer'");
   
   TextLayer *layer = text_layer_create(frame);
   if (!layer) error("Unable to allocate memory for 'digit_layer'");
   
   text_layer_set_text(layer, text);
   text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
   text_layer_set_text_alignment(layer, GTextAlignmentCenter);
   
   return layer;
}// End of create_digit_layer method

static void stop_selection_handle_window_load(Window* window)
{
   StopSelection ss = (StopSelection)window_get_user_data(window);
   
   // Init GUI components
   info("Initializing 'stop_selection' GUI components");
   
   Layer *window_layer = window_get_root_layer(window);
   GRect bounds = layer_get_frame(window_layer);
   
   int digit_width = bounds.size.w / DIGITS_LENGTH;
   for (int x = 0; x < DIGITS_LENGTH; ++x)
   {
      ss->digits[x] = calloc(sizeof(char), 2);
      snprintf(ss->digits[x], sizeof(ss->digits[x]), "0");
      
      TextLayer *digit_layer = create_digit_layer((GRect) {
         .origin = { digit_width * x, bounds.size.h / 2 - 21},
         .size = { digit_width, 42 }
      }, ss->digits[x]);
      layer_add_child(window_get_root_layer(window), text_layer_get_layer(digit_layer));
      
      ss->digit_layers[x] = digit_layer;
   }// End of for
   
   // Set selection on the first digit
   stop_selection_activate_digit(ss, 0);
}// End of stop_selection_handle_window_load method

static void stop_selection_handle_window_unload(Window* window)
{
   StopSelection ss = (StopSelection)window_get_user_data(window);
   
   // Unload GUI components
   info("Destroying 'stop_selection' GUI components");
   
   for (int x = 0; x < DIGITS_LENGTH; ++x)
   {
      free(ss->digits[x]);
      text_layer_destroy(ss->digit_layers[x]);
      
      ss->digits[x] = NULL;
      ss->digit_layers[x] = NULL;
   }// End of for
}// End of stop_selection_handle_window_unload method

static void stop_selection_back_click_handler(ClickRecognizerRef recognizer, void *context)
{
   StopSelection ss = (StopSelection)context;
   
   info("Back button clicked on 'stop_selection' window");
   
   if (ss->active_digit > 0)
   {
      stop_selection_activate_digit(ss, ss->active_digit - 1);
   }// End of if
   else
   {
      stop_selection_hide(ss);
      if (ss->failure_callback) ss->failure_callback(ss->context);
   }// End of else
}// End of stop_selection_select_click_handler method

static void stop_selection_select_click_handler(ClickRecognizerRef recognizer, void *context)
{
   StopSelection ss = (StopSelection)context;
   
   info("Select button clicked on 'stop_selection' window");
   
   if (ss->active_digit < DIGITS_LENGTH - 1)
   {
      stop_selection_activate_digit(ss, ss->active_digit + 1);
   }// End of if
   else
   {
      int stop_id = stop_selection_get_stop_id(ss);
      stop_selection_hide(ss);
      if (ss->success_callback) ss->success_callback(stop_id, ss->context);
   }// End of else
}// End of stop_selection_select_click_handler method

static void stop_selection_up_click_handler(ClickRecognizerRef recognizer, void *context)
{
   StopSelection ss = (StopSelection)context;
   
   info("Up button clicked on 'stop_selection' window");
   
   int current_digit = (int)(text_layer_get_text(ss->digit_layers[ss->active_digit])[0] - '0');
   ++current_digit;
   
   if (current_digit > 9) current_digit = 0;
   
   snprintf(ss->digits[ss->active_digit], sizeof(ss->digits[ss->active_digit]), "%d", current_digit);
   text_layer_set_text(ss->digit_layers[ss->active_digit], ss->digits[ss->active_digit]);
}// End of stop_selection_up_click_handler method

static void stop_selection_down_click_handler(ClickRecognizerRef recognizer, void *context)
{
   StopSelection ss = (StopSelection)context;
   
   info("Down button clicked on 'stop_selection' window");
   
   int current_digit = (int)(text_layer_get_text(ss->digit_layers[ss->active_digit])[0] - '0');
   --current_digit;
   
   if (current_digit < 0) current_digit = 9;
   
   snprintf(ss->digits[ss->active_digit], sizeof(ss->digits[ss->active_digit]), "%d", current_digit);
   text_layer_set_text(ss->digit_layers[ss->active_digit], ss->digits[ss->active_digit]);
}// End of stop_selection_down_click_handler method

static void stop_selection_window_click_config_provider(void *context)
{
   window_single_click_subscribe(BUTTON_ID_BACK, stop_selection_back_click_handler);
   window_single_click_subscribe(BUTTON_ID_SELECT, stop_selection_select_click_handler);
   window_single_click_subscribe(BUTTON_ID_UP, stop_selection_up_click_handler);
   window_single_click_subscribe(BUTTON_ID_DOWN, stop_selection_down_click_handler);
}// End of stop_selection_window_click_config_provider method

StopSelection stop_selection_create(StopSelectionCompleteCallback complete_callback, StopSelectionCancelledCallback cancelled_callback, void *context)
{
   info("Creating 'stop_selection' object");
   StopSelection ss = (StopSelection)malloc(sizeof(struct stop_selection));
   
   if (!ss) error("Unable to allocate memory for 'stop_selection' object");
   
   // Configure Stop Selection
   ss->success_callback = complete_callback;
   ss->failure_callback = cancelled_callback;
   ss->context = context;
   
   ss->active_digit = 0;
   
   // Configure window
   ss->window = window_create();
   
   if (!(ss->window)) error("Unable to allocate memory for 'window' object");
   
   window_set_fullscreen(ss->window, false);
   window_set_window_handlers(ss->window, (WindowHandlers) {
      .load = stop_selection_handle_window_load,
      .unload = stop_selection_handle_window_unload
   });
   window_set_click_config_provider_with_context(ss->window, stop_selection_window_click_config_provider, ss);
   window_set_user_data(ss->window, ss);
      
   return ss;
}// End of stop_selection_create method

void stop_selection_destroy(StopSelection ss)
{
   info("Destroying 'stop_selection' object");
   
   window_destroy(ss->window);
   free(ss);
}// End of stop_selection_destory method

void stop_selection_show(StopSelection ss)
{
   info("Showing 'stop_selection' window");
   window_stack_push(ss->window, true);
}// End of stop_selection_show method

void stop_selection_hide(StopSelection ss)
{
   info("Hiding 'stop_selection' window");
   window_stack_remove(ss->window, true);
}// End of stop_selection_hide method