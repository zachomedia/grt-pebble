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

#ifndef _stop_selection_h
#define _stop_selection_h

struct stop_selection;
typedef struct stop_selection * StopSelection;

typedef void (*StopSelectionCompleteCallback)(int stop_selection, void *context);
typedef void (*StopSelectionCancelledCallback)(void *context);

StopSelection stop_selection_create(StopSelectionCompleteCallback complete_callback, StopSelectionCancelledCallback cancelled_callback, void *context);
void stop_selection_destroy(StopSelection ss);

void stop_selection_show(StopSelection ss);
void stop_selection_hide(StopSelection ss);

#endif