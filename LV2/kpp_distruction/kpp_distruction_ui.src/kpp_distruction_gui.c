/*
 * Copyright (C) 2018 Oleg Kapitonov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

#include <stdio.h>
#include <stdlib.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include "X11/Xutil.h"

// Port numbers for communication with the main LV2 plugin module
enum {PORT_BYPASS, PORT_BASS, PORT_DRIVE, PORT_MIDDLE, PORT_TREBLE, PORT_VOICE, PORT_LEVEL};

// GUI window structure
typedef struct 
{
  Display *dpy;
  int scr;

  Window win;

  int width, height;
  long event_mask;
    
  void *controller;
    
  LV2UI_Write_Function write_function;
  LV2UI_Resize* resize;
    
} win_t;

// Dial GUI element structure
typedef struct 
{
  int value; // 0 - 100
  int start_value; // Holds value at the start of dialing
  int base_x; // Coordinates of Dial in main window
  int base_y; // 
} st_dial;

// Point
typedef struct 
{
  int x;
  int y;
}st_point;

// Color
typedef struct 
{
  double r;
  double g;
  double b;
}st_rgb;

// Main window
win_t win;

// Dials
st_dial driveDial = {0, 0, 62, 86};
st_dial bassDial = {0, 0, 21, 450};
st_dial middleDial = {0, 0, 57, 304};
st_dial trebleDial = {0, 0, 174, 220};
st_dial voiceDial = {0, 0, 314, 215};
st_dial volumeDial = {0, 0, 247, 68};

// Holds state of bypass button
int bypass_flag;

// Hold mouse coordinates at the start of dialing 
int pos_x;
int pos_y;

// Cairo objects
cairo_t *cr;
cairo_surface_t *surface;
Visual *visual;

cairo_surface_t *image,*image2;

// Port number (from enum) of the Dial, which is now
// adjusted by the user
// -1 means that no dial is adjusted
int active_dial = -1;

// Buffer for holding full path to image files
char name_and_path[10000];

// Create main window
static void win_init(win_t *win, Window parentXwindow)
{
  win->width = 442;
  win->height = 600;

  win->scr = DefaultScreen(win->dpy);
  
  win->win = XCreateSimpleWindow(win->dpy, parentXwindow, 0, 0,
                                win->width, win->height, 0,
                                0, 0);

  win->event_mask = StructureNotifyMask
                    |ExposureMask
                    |ButtonPressMask
                    |Button1MotionMask;
                      
  XSizeHints sizeHints;
  memset(&sizeHints, 0, sizeof(sizeHints));
    
  sizeHints.flags      = PMinSize|PMaxSize|PSize;
  sizeHints.min_width  = win->width;
  sizeHints.min_height = win->height;
  sizeHints.max_width  = win->width;;
  sizeHints.max_height = win->height;
    
  XSetWMNormalHints(win->dpy, win->win, &sizeHints);
                      
  XSelectInput(win->dpy, win->win, win->event_mask);

  XMapWindow(win->dpy, win->win);
}

// LV2 initialization function
static LV2UI_Handle
instantiate(const struct _LV2UI_Descriptor * descriptor,
            const char * plugin_uri,
            const char * bundle_path,
            LV2UI_Write_Function write_function,
            LV2UI_Controller controller,
            LV2UI_Widget * widget,
            const LV2_Feature * const * features)
{
  // Quit if GUI module loaded with wrong main module
  if (strcmp(plugin_uri, PLUGIN_URI)) 
  {
    fprintf(stderr,
      "%s: ERROR: this GUI does not support plugin with URI %s\n",
      PLUGIN_URI, plugin_uri);
    return NULL;
  }
  
  // X11 handle of host window, in which plugin GUI
  // window must be embedded
  void * parentXwindow = 0;
    
  // Resize function provided by LV2 host
  LV2UI_Resize* resize = NULL;
    
  // Obtain this pointers
  for (int i = 0; features[i]; ++i) 
  {
    if (!strcmp(features[i]->URI, LV2_UI__parent)) 
    {
      parentXwindow = features[i]->data;
    }
    else if (!strcmp(features[i]->URI, LV2_UI__resize)) 
    {
      resize = (LV2UI_Resize*)features[i]->data;
    }
  }

  win.dpy = XOpenDisplay(0);

  if (win.dpy == NULL) 
  {
    fprintf(stderr, "Failed to open display\n");
    return NULL;
  }

  win_init(&win,(Window)parentXwindow);
    
  visual = DefaultVisual(win.dpy, DefaultScreen (win.dpy));
  XClearWindow(win.dpy, win.win);
  surface = cairo_xlib_surface_create (win.dpy, win.win, visual,
                                        win.width, win.height);
  cr = cairo_create(surface);
  
  strcpy(name_and_path,bundle_path);
  strcat(name_and_path,"/base_scale.png");
    
  // Load background image
  image = cairo_image_surface_create_from_png (name_and_path);
  
  strcpy(name_and_path,bundle_path);
  strcat(name_and_path,"/light.png");
  
  // Load dial point image
  image2 = cairo_image_surface_create_from_png (name_and_path);
  
  // Return to host the X11 handle of created window
  *widget = (void*)win.win;
  
  if (resize)
  {
    win.resize = resize;
    // Ask the host to properly resize the plugin host window
    resize->ui_resize(resize->handle, win.width, win.height);
  }
  
  win.controller = controller;
  win.write_function = write_function;

  return (LV2UI_Handle)&win;
}

static void win_deinit(win_t *win)
{
    XDestroyWindow(win->dpy, win->win);
}

static void
cleanup(LV2UI_Handle ui)
{
  win_deinit(&win);
  XCloseDisplay(win.dpy);
}

// Limit value by range 0 - 100
int clamp(int value)
{
  if (value < 0) return 0;
  if (value > 100) return 100;
  return value;
}

// Convert a value in dB to the linear range 0 - 100
// 0 corresponds to `-range` dB, 100 corresponds to `range` dB
int db_to_value(float db, float range)
{
  int value = clamp((int)((db + range)/2.0/range*100.0));
  return value;
}

// Calculate RGB color of Dial's LED for given value
st_rgb value_to_color(int value)
{
  st_rgb color;
  color.r = 0.01*pow(value,1.0/3.0)*100.0/pow(100,1.0/3.0);
  color.g = 0.7874*(1.0-pow(value,3)/1000000.0);
  color.b = 0;
  return color;
}

// Calculate coordinates of Dial's LED for given value
st_point value_to_xy(int value)
{
  int pointerAngle = value/100.0*(360.0-80.0-80.0) - 105.0;
  st_point p;
  p.x = 43 + 27*sin(pointerAngle/180.0*M_PI);
  p.y = 43 - 27*cos(pointerAngle/180.0*M_PI);
  return p;
}

void draw_dial(cairo_t *cr, cairo_surface_t *image, int value, int base_x, int base_y)
{
  st_rgb color = value_to_color(value);
  st_point p = value_to_xy(value);
    
  cairo_set_source_rgb(cr, color.r, color.g, color.b);
  cairo_arc(cr, base_x+p.x+10.5, base_y+p.y+10, 5.5, 0, 2 * M_PI);
  cairo_fill(cr);
  cairo_set_source_surface (cr, image, base_x+p.x, base_y+p.y);
  cairo_paint (cr);
}

static void win_draw(win_t *win)
{
  cairo_push_group (cr);
    
  if (bypass_flag == 0)
  {
    cairo_set_source_rgb(cr, 1.0, 0.63, 0.0);
    cairo_arc(cr, 213, 428, 12, 0, 2 * M_PI);
    cairo_fill(cr);
  }
  else
  {
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_arc(cr, 213, 428, 12, 0, 2 * M_PI);
    cairo_fill(cr);
  }
    
    
  cairo_set_source_surface (cr, image, 0, 0);
  cairo_paint (cr);
    
  draw_dial(cr,image2,driveDial.value,driveDial.base_x,driveDial.base_y);
  draw_dial(cr,image2,bassDial.value,bassDial.base_x,bassDial.base_y);
  draw_dial(cr,image2,middleDial.value,middleDial.base_x,middleDial.base_y);
  draw_dial(cr,image2,trebleDial.value,trebleDial.base_x,trebleDial.base_y);
  draw_dial(cr,image2,voiceDial.value,voiceDial.base_x,voiceDial.base_y);
  draw_dial(cr,image2,volumeDial.value,volumeDial.base_x,volumeDial.base_y);
        
  cairo_pop_group_to_source (cr);
  cairo_paint (cr);
}

// Callback from host to change Dial values
static void port_event(LV2UI_Handle ui,
                        uint32_t port_index,
                        uint32_t buffer_size,
                        uint32_t format,
                        const void * buffer)
{
  switch (port_index)
  {
    case PORT_BYPASS:
    {
      float bypass_value = *(float*)buffer;
      if (bypass_value > 0.5) bypass_flag = 1;
      else bypass_flag = 0;
    }
    break;
    case PORT_BASS:
      bassDial.value = db_to_value(*(float*)buffer, 15.0);
    break;
    case PORT_DRIVE:
      driveDial.value = clamp((int)(*(float*)buffer));
    break;
    case PORT_LEVEL:
      volumeDial.value = clamp((int)((*(float*)buffer)*100.0));
    break;
    case PORT_MIDDLE:
      middleDial.value = db_to_value(*(float*)buffer, 15.0);
    break;
    case PORT_TREBLE:
      trebleDial.value = db_to_value(*(float*)buffer, 15.0);
    break;
    case PORT_VOICE:
      voiceDial.value = clamp((int)((*(float*)buffer)*100.0));
  }
  // Redraw window due to value change
  win_draw(&win);
}

// Determine if the point with given coordinates
// belongs to the given Dial
bool is_point_in_dial_area(int x, int y, st_dial *dial)
{
  if ((x >= dial->base_x + 5) && (x <= dial->base_x + 100)
      && (y >= dial->base_y + 5) && (y <= dial->base_y + 105))
  {
    return true;
  }
  return false;
}

// Determine if the point with given coordinates
// belongs to the given area
bool is_point_in_area(int x, int y, int topleft_x, int topleft_y,
                      int bottomright_x, int bottomright_y)
{
  if ((x >= topleft_x) && (x <= bottomright_x)
    && (y >= topleft_y) && (y <= bottomright_y))
  {
    return true;
  }
  return false;
}

// Convert the linear value 0 - 100 to the dB value,
// 0 corresponds to `-range` dB, 100 corresponds to `range` dB
float value_to_db(int value, float range)
{
  float db = value / 100.0 * 2.0 * range - range;
  return db;
}

// Process X11 events
static void
win_handle_events(win_t *win)
{
  XEvent xev;

  while (XPending(win->dpy) > 0) {
    
    XNextEvent(win->dpy, &xev);

    switch(xev.type) {

      case Expose: // the window must be redrawn
      {
        XExposeEvent *eev = &xev.xexpose;

        if (eev->count == 0)
        {
          win_draw(win);
        }
      }
      break;
      case ButtonPress: // Left mouse button pressed
      {
        XButtonPressedEvent *bev = &xev.xbutton;
        if (bev->button == Button1)
        {
          // Save position
          pos_x = bev->x;
          pos_y = bev->y;
                  
          // Find on which dial the button was pressed
          if (is_point_in_dial_area(pos_x, pos_y, &driveDial))
          {
            driveDial.start_value = driveDial.value;
            active_dial = PORT_DRIVE;
          }
          else if (is_point_in_dial_area(pos_x, pos_y, &bassDial))
          {
            bassDial.start_value = bassDial.value;
            active_dial = PORT_BASS;
          }
          else if (is_point_in_dial_area(pos_x, pos_y, &middleDial))
          {
            middleDial.start_value = middleDial.value;
            active_dial = PORT_MIDDLE;
          }
          else if (is_point_in_dial_area(pos_x, pos_y, &trebleDial))
          {
            trebleDial.start_value = trebleDial.value;
            active_dial = PORT_TREBLE;
          }
          else if (is_point_in_dial_area(pos_x, pos_y, &voiceDial))
          {
            voiceDial.start_value = voiceDial.value;
            active_dial = PORT_VOICE;
          }
          else if (is_point_in_dial_area(pos_x, pos_y, &volumeDial))
          {
            volumeDial.start_value = volumeDial.value;
            active_dial = PORT_LEVEL;
          }
          // Click on bypass button
          else if (is_point_in_area(pos_x, pos_y, 275, 459, 326, 511))
          {
            // Toggle bypass flag
            bypass_flag ^= 1;
            float value = bypass_flag;
            win->write_function(win->controller,PORT_BYPASS,sizeof(float),0,&value);
            active_dial = -1;
          }
          else
          {
            active_dial = -1;
          }
        }
      }
      break;
      case MotionNotify: // Mouse move while left button is pressed
      {
        XMotionEvent *mev = &xev.xmotion;
                
        if (active_dial != -1)
        {
          // Recalculate active Dial value and send new value to the
          // main LV2 plugin module
          switch (active_dial)
          {
            float value;
            case PORT_DRIVE:
              driveDial.value = clamp(driveDial.start_value + pos_y - mev->y);
              value = driveDial.value;
              win->write_function(win->controller,PORT_DRIVE,sizeof(float),0,&value);
            break;
            case PORT_BASS:
              bassDial.value = clamp(bassDial.start_value + pos_y - mev->y);
              value = value_to_db(bassDial.value, 15.0);
              win->write_function(win->controller,PORT_BASS,sizeof(float),0,&value);
            break;
            case PORT_MIDDLE:
              middleDial.value = clamp(middleDial.start_value + pos_y - mev->y);
              value = value_to_db(middleDial.value, 15.0);
              win->write_function(win->controller,PORT_MIDDLE,sizeof(float),0,&value);
            break;
            case PORT_TREBLE:
              trebleDial.value = clamp(trebleDial.start_value + pos_y - mev->y);
              value = value_to_db(trebleDial.value, 15.0);
              win->write_function(win->controller,PORT_TREBLE,sizeof(float),0,&value);
            break;
            case PORT_VOICE:
              voiceDial.value = clamp(voiceDial.start_value + pos_y - mev->y);
              value = voiceDial.value / 100.0;
              win->write_function(win->controller,PORT_VOICE,sizeof(float),0,&value);
            break;
            case PORT_LEVEL:
              volumeDial.value = clamp(volumeDial.start_value + pos_y - mev->y);
              value = volumeDial.value / 100.0;
              win->write_function(win->controller,PORT_LEVEL,sizeof(float),0,&value);
          }
          win_draw(win);
        }
      }
      break;
    }
  }
}

// LV2 callback function to organize event handling
static int
idle(LV2UI_Handle handle)
{
  win_handle_events(&win);
  
  return 0;
}


// LV2 interfaces and descriptors
static const LV2UI_Idle_Interface idle_iface = { idle };

static const void*
extension_data(const char* uri)
{
  if (!strcmp(uri, LV2_UI__idleInterface)) 
  {
    return &idle_iface;
  }
  return NULL;
}

static const LV2UI_Descriptor descriptor = 
{
  PLUGIN_URI "ui",
  instantiate,
  cleanup,
  port_event,
  extension_data
};

extern "C"
LV2_SYMBOL_EXPORT
const LV2UI_Descriptor*
lv2ui_descriptor(uint32_t index)
{
  switch (index) 
  {
    case 0:
      return &descriptor;
    default:
      return NULL;
  }
}





