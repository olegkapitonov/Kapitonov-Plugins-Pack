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

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

#include <stdio.h>
#include <stdlib.h>

#include <cairo.h>
#include <cairo-xcb.h>

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>

// Port numbers for communication with the main LV2 plugin module
enum {PORT_BYPASS, PORT_BASS, PORT_DRIVE, PORT_MIDDLE, PORT_TREBLE, PORT_VOICE, PORT_LEVEL};

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

// GUI window structure
typedef struct
{
  xcb_connection_t *connection;

  xcb_window_t win;

  int width, height;
  long event_mask;

  void *controller;

  LV2UI_Write_Function write_function;
  LV2UI_Resize* resize;


  // Main window
  //win_t win;

  // Dials
  st_dial driveDial;
  st_dial bassDial;
  st_dial middleDial;
  st_dial trebleDial;
  st_dial voiceDial;
  st_dial volumeDial;

  // Holds state of bypass button
  int bypass_flag;

  // Hold mouse coordinates at the start of dialing
  int pos_x;
  int pos_y;

  // Cairo objects
  cairo_t *cr;
  cairo_surface_t *surface;
  xcb_visualtype_t *visual;

  cairo_surface_t *image,*image2;

  // Port number (from enum) of the Dial, which is now
  // adjusted by the user
  // -1 means that no dial is adjusted
  int active_dial;

} win_t;
// Create main window
static void win_init(win_t *win, xcb_screen_t *screen,
    xcb_window_t parentXwindow)
{
  win->width = 442;
  win->height = 600;

  win->win = xcb_generate_id(win->connection);

  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t mask_values[1] = {
    XCB_EVENT_MASK_STRUCTURE_NOTIFY |
    XCB_EVENT_MASK_EXPOSURE |
    XCB_EVENT_MASK_BUTTON_PRESS |
    XCB_EVENT_MASK_BUTTON_1_MOTION };

  xcb_create_window(win->connection, XCB_COPY_FROM_PARENT,
      win->win, parentXwindow,
      0, 0, win->width, win->height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
      screen->root_visual, mask, mask_values);

  xcb_size_hints_t size_hints;
  memset(&size_hints, 0, sizeof(size_hints));
  xcb_icccm_size_hints_set_size(&size_hints, 1, win->width, win->height);
  xcb_icccm_size_hints_set_min_size(&size_hints, win->width, win->height);
  xcb_icccm_size_hints_set_max_size(&size_hints, win->width, win->height);

  xcb_icccm_set_wm_normal_hints(win->connection, win->win, &size_hints);

  xcb_map_window(win->connection, win->win);
  xcb_flush(win->connection);
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

  win_t *win = (win_t *)malloc(sizeof(win_t));

  win->active_dial = -1;

  win->driveDial.value = 0;
  win->driveDial.start_value = 0;
  win->driveDial.base_x = 62;
  win->driveDial.base_y = 86;

  win->bassDial.value = 0;
  win->bassDial.start_value = 0;
  win->bassDial.base_x = 21;
  win->bassDial.base_y = 450;

  win->middleDial.value = 0;
  win->middleDial.start_value = 0;
  win->middleDial.base_x = 57;
  win->middleDial.base_y = 304;

  win->trebleDial.value = 0;
  win->trebleDial.start_value = 0;
  win->trebleDial.base_x = 174;
  win->trebleDial.base_y = 220;

  win->voiceDial.value = 0;
  win->voiceDial.start_value = 0;
  win->voiceDial.base_x = 314;
  win->voiceDial.base_y = 215;

  win->volumeDial.value = 0;
  win->volumeDial.start_value = 0;
  win->volumeDial.base_x = 247;
  win->volumeDial.base_y = 68;

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

  win->connection = xcb_connect(NULL, NULL);

  if (win->connection == NULL)
  {
    fprintf(stderr, "Failed to open display\n");
    return NULL;
  }

  xcb_screen_t *screen =
    xcb_setup_roots_iterator(xcb_get_setup(win->connection)).data;

  win_init(win, screen, (xcb_window_t) (size_t) parentXwindow);

  win->visual = xcb_aux_find_visual_by_id(screen, screen->root_visual);
  xcb_clear_area(win->connection, 0, win->win, 0, 0, 0, 0);
  win->surface = cairo_xcb_surface_create(win->connection, win->win, win->visual,
                                        win->width, win->height);
  win->cr = cairo_create(win->surface);

  char image_path[PATH_MAX];

  // Load background image
  snprintf(image_path, sizeof(image_path), "%s/base_scale.png", bundle_path);
  image_path[sizeof(image_path) - 1] = '\0';
  win->image = cairo_image_surface_create_from_png (image_path);

  // Load dial point image
  snprintf(image_path, sizeof(image_path), "%s/light.png", bundle_path);
  image_path[sizeof(image_path) - 1] = '\0';
  win->image2 = cairo_image_surface_create_from_png (image_path);

  // Return to host the X11 handle of created window
  *widget = (void*) (size_t) win->win;

  if (resize)
  {
    win->resize = resize;
    // Ask the host to properly resize the plugin host window
    resize->ui_resize(resize->handle, win->width, win->height);
  }

  win->controller = controller;
  win->write_function = write_function;

  return (LV2UI_Handle)win;
}

static void win_deinit(win_t *win)
{
  xcb_destroy_window(win->connection, win->win);
}

static void
cleanup(LV2UI_Handle ui)
{
  win_t *win = (win_t *)ui;
  win_deinit(win);
  cairo_destroy(win->cr);
  cairo_surface_destroy(win->surface);
  cairo_surface_destroy(win->image);
  cairo_surface_destroy(win->image2);
  xcb_disconnect(win->connection);
  free(win);
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
  cairo_push_group (win->cr);

  if (win->bypass_flag == 0)
  {
    cairo_set_source_rgb(win->cr, 1.0, 0.63, 0.0);
    cairo_arc(win->cr, 213, 428, 12, 0, 2 * M_PI);
    cairo_fill(win->cr);
  }
  else
  {
    cairo_set_source_rgb(win->cr, 0.0, 0.0, 0.0);
    cairo_arc(win->cr, 213, 428, 12, 0, 2 * M_PI);
    cairo_fill(win->cr);
  }


  cairo_set_source_surface (win->cr, win->image, 0, 0);
  cairo_paint (win->cr);

  draw_dial(win->cr,
            win->image2,
            win->driveDial.value,
            win->driveDial.base_x,
            win->driveDial.base_y);
  draw_dial(win->cr,
            win->image2,
            win->bassDial.value,
            win->bassDial.base_x,
            win->bassDial.base_y);
  draw_dial(win->cr,
            win->image2,
            win->middleDial.value,
            win->middleDial.base_x,
            win->middleDial.base_y);
  draw_dial(win->cr,
            win->image2,
            win->trebleDial.value,
            win->trebleDial.base_x,
            win->trebleDial.base_y);
  draw_dial(win->cr,
            win->image2,
            win->voiceDial.value,
            win->voiceDial.base_x,
            win->voiceDial.base_y);
  draw_dial(win->cr,
            win->image2,
            win->volumeDial.value,
            win->volumeDial.base_x,
            win->volumeDial.base_y);

  cairo_pop_group_to_source (win->cr);
  cairo_paint (win->cr);
}

// Callback from host to change Dial values
static void port_event(LV2UI_Handle ui,
                        uint32_t port_index,
                        uint32_t buffer_size,
                        uint32_t format,
                        const void * buffer)
{
  win_t *win = (win_t *)ui;
  switch (port_index)
  {
    case PORT_BYPASS:
    {
      float bypass_value = *(float*)buffer;
      if (bypass_value > 0.5) win->bypass_flag = 1;
      else win->bypass_flag = 0;
    }
    break;
    case PORT_BASS:
      win->bassDial.value = db_to_value(*(float*)buffer, 15.0);
    break;
    case PORT_DRIVE:
      win->driveDial.value = clamp((int)(*(float*)buffer));
    break;
    case PORT_LEVEL:
      win->volumeDial.value = clamp((int)((*(float*)buffer)*100.0));
    break;
    case PORT_MIDDLE:
      win->middleDial.value = db_to_value(*(float*)buffer, 15.0);
    break;
    case PORT_TREBLE:
      win->trebleDial.value = db_to_value(*(float*)buffer, 15.0);
    break;
    case PORT_VOICE:
      win->voiceDial.value = clamp((int)((*(float*)buffer)*100.0));
  }
  // Redraw window due to value change
  win_draw(win);
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
  xcb_flush(win->connection);
  xcb_generic_event_t *event;
  while ((event = xcb_poll_for_event(win->connection)) != NULL)
  {
    switch(event->response_type & ~0x80)
    {
      case XCB_EXPOSE: // the window must be redrawn
      {
        xcb_expose_event_t *eev = (xcb_expose_event_t *) event;

        if (eev->count == 0)
        {
          win_draw(win);
        }
      }
      break;
      case XCB_BUTTON_PRESS: // Left mouse button pressed
      {
        xcb_button_press_event_t *bev = (xcb_button_press_event_t *) event;
        if (bev->detail == XCB_BUTTON_INDEX_1)
        {
          // Save position
          win->pos_x = bev->event_x;
          win->pos_y = bev->event_y;

          // Find on which dial the button was pressed
          if (is_point_in_dial_area(win->pos_x, win->pos_y, &(win->driveDial)))
          {
            win->driveDial.start_value = win->driveDial.value;
            win->active_dial = PORT_DRIVE;
          }
          else if (is_point_in_dial_area(win->pos_x, win->pos_y, &(win->bassDial)))
          {
            win->bassDial.start_value = win->bassDial.value;
            win->active_dial = PORT_BASS;
          }
          else if (is_point_in_dial_area(win->pos_x, win->pos_y, &(win->middleDial)))
          {
            win->middleDial.start_value = win->middleDial.value;
            win->active_dial = PORT_MIDDLE;
          }
          else if (is_point_in_dial_area(win->pos_x, win->pos_y, &(win->trebleDial)))
          {
            win->trebleDial.start_value = win->trebleDial.value;
            win->active_dial = PORT_TREBLE;
          }
          else if (is_point_in_dial_area(win->pos_x, win->pos_y, &(win->voiceDial)))
          {
            win->voiceDial.start_value = win->voiceDial.value;
            win->active_dial = PORT_VOICE;
          }
          else if (is_point_in_dial_area(win->pos_x, win->pos_y, &(win->volumeDial)))
          {
            win->volumeDial.start_value = win->volumeDial.value;
            win->active_dial = PORT_LEVEL;
          }
          // Click on bypass button
          else if (is_point_in_area(win->pos_x, win->pos_y, 275, 459, 326, 511))
          {
            // Toggle bypass flag
            win->bypass_flag ^= 1;
            float value = win->bypass_flag;
            win->write_function(win->controller,PORT_BYPASS,sizeof(float),0,&value);
            win->active_dial = -1;
            win_draw(win);
          }
          else
          {
            win->active_dial = -1;
          }
        }
      }
      break;
      case XCB_MOTION_NOTIFY: // Mouse move while left button is pressed
      {
        xcb_motion_notify_event_t *mev = (xcb_motion_notify_event_t *) event;

        if (win->active_dial != -1)
        {
          // Recalculate active Dial value and send new value to the
          // main LV2 plugin module
          switch (win->active_dial)
          {
            float value;
            case PORT_DRIVE:
              win->driveDial.value = clamp(win->driveDial.start_value + win->pos_y - mev->event_y);
              value = win->driveDial.value;
              win->write_function(win->controller,PORT_DRIVE,sizeof(float),0,&value);
            break;
            case PORT_BASS:
              win->bassDial.value = clamp(win->bassDial.start_value + win->pos_y - mev->event_y);
              value = value_to_db(win->bassDial.value, 15.0);
              win->write_function(win->controller,PORT_BASS,sizeof(float),0,&value);
            break;
            case PORT_MIDDLE:
              win->middleDial.value = clamp(win->middleDial.start_value + win->pos_y - mev->event_y);
              value = value_to_db(win->middleDial.value, 15.0);
              win->write_function(win->controller,PORT_MIDDLE,sizeof(float),0,&value);
            break;
            case PORT_TREBLE:
              win->trebleDial.value = clamp(win->trebleDial.start_value + win->pos_y - mev->event_y);
              value = value_to_db(win->trebleDial.value, 15.0);
              win->write_function(win->controller,PORT_TREBLE,sizeof(float),0,&value);
            break;
            case PORT_VOICE:
              win->voiceDial.value = clamp(win->voiceDial.start_value + win->pos_y - mev->event_y);
              value = win->voiceDial.value / 100.0;
              win->write_function(win->controller,PORT_VOICE,sizeof(float),0,&value);
            break;
            case PORT_LEVEL:
              win->volumeDial.value = clamp(win->volumeDial.start_value + win->pos_y - mev->event_y);
              value = win->volumeDial.value / 100.0;
              win->write_function(win->controller,PORT_LEVEL,sizeof(float),0,&value);
          }
          win_draw(win);
        }
      }
      break;
    }
    free(event);
  }
  xcb_flush(win->connection);
}

// LV2 callback function to organize event handling
static int
idle(LV2UI_Handle handle)
{
  win_t *win = (win_t *)handle;
  win_handle_events(win);

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








