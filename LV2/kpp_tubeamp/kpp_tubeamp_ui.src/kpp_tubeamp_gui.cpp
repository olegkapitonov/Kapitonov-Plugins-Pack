/*
 * Copyright (C) 2018-2020 Oleg Kapitonov
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

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include "lv2/lv2plug.in/ns/ext/atom/forge.h"
#include "lv2/lv2plug.in/ns/ext/patch/patch.h"

#include <stdio.h>
#include <stdlib.h>

#include <cairo.h>
#include <cairo-xcb.h>

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>

#include "kpp_file_picker.h"

// Port numbers for communication with the main LV2 plugin module
enum {PORT_BASS, PORT_MIDDLE, PORT_TREBLE, PORT_DRIVE, PORT_MASTERGAIN, PORT_VOLUME,
  PORT_CABINET, PORT_IN0, PORT_IN1, PORT_OUT0, PORT_OUT1, PORT_CONTROL, PORT_NOTIFY};

// Dial GUI element structure
typedef struct
{
  int value; // 0 - 100
  int start_value; // Holds value at the start of dialing
  int base_x; // Coordinates of Dial in main window
  int base_y; //
} st_dial;

// Slider GUI element structure
typedef struct
{
  int value; // 0 - 100
  int start_value; // Holds value at the start of sliding
  int base_x; // Coordinates of Slider in main window
  int base_y; //
} st_slider;

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

typedef struct {
  LV2_URID atom_eventTransfer;
  LV2_URID patch_Get;
  LV2_URID patch_Set;
  LV2_URID profile;
  LV2_URID patch_property;
  LV2_URID patch_value;
  LV2_URID atom_Path;
} tubeAmpURIs;

// GUI window structure
typedef struct
{
  xcb_connection_t *connection;

  xcb_window_t win;

  int width, height;

  void *controller;

  LV2UI_Write_Function write_function;
  LV2UI_Resize* resize;
  LV2_URID_Map* map;

  LV2_Atom_Forge forge;  // Forge for writing atoms in run thread

  uint8_t forge_buf[4096];

  tubeAmpURIs uris;

  // Main window
  //win_t win;

  // Dials
  st_dial driveDial;

  st_dial bassDial;
  st_dial middleDial;
  st_dial trebleDial;

  st_dial mastergainDial;

  // Sliders

  st_slider levelSlider;
  st_slider cabinetSlider;

  // Hold mouse coordinates at the start of dialing
  int pos_x;
  int pos_y;

  // Cairo objects
  cairo_t *cr;
  cairo_surface_t *surface;
  xcb_visualtype_t *visual;

  cairo_surface_t *image, *image2;
  cairo_device_t *device;

  // Port number (from enum) of the Dial, which is now
  // adjusted by the user
  // -1 means that no dial is adjusted
  int active_dial;

  int profile_select_fd;

  char profile_path[PATH_MAX];
  char new_profile_path[PATH_MAX];
  size_t new_profile_path_len;

  KPPFilePicker::Win *fp_win;

} win_t;

// Create main window
static void win_init(win_t *win, xcb_screen_t *screen,
    xcb_window_t parentXwindow)
{
  win->fp_win = NULL;
  win->width = 1000;
  win->height = 376;

  win->win = xcb_generate_id(win->connection);

  uint32_t mask = XCB_CW_EVENT_MASK;
  uint32_t mask_values[1] = {
    XCB_EVENT_MASK_STRUCTURE_NOTIFY |
    XCB_EVENT_MASK_EXPOSURE |
    XCB_EVENT_MASK_BUTTON_PRESS |
    XCB_EVENT_MASK_BUTTON_1_MOTION };

  xcb_create_window(win->connection, XCB_COPY_FROM_PARENT,
                    win->win, parentXwindow,
                    0, 0, win->width, win->height, 0, XCB_WINDOW_CLASS_COPY_FROM_PARENT,
                    XCB_COPY_FROM_PARENT, mask, mask_values);


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

  win_t *win = (win_t *) malloc(sizeof(win_t));

  win->active_dial = -1;

  win->profile_select_fd = -1;

  win->driveDial.value = 0;
  win->driveDial.start_value = 0;
  win->driveDial.base_x = 215;
  win->driveDial.base_y = 235;

  win->bassDial.value = 0;
  win->bassDial.start_value = 0;
  win->bassDial.base_x = 377;
  win->bassDial.base_y = 235;

  win->middleDial.value = 0;
  win->middleDial.start_value = 0;
  win->middleDial.base_x = 479;
  win->middleDial.base_y = 235;

  win->trebleDial.value = 0;
  win->trebleDial.start_value = 0;
  win->trebleDial.base_x = 583;
  win->trebleDial.base_y = 235;

  win->mastergainDial.value = 0;
  win->mastergainDial.start_value = 0;
  win->mastergainDial.base_x = 808;
  win->mastergainDial.base_y = 235;

  win->levelSlider.value = 0;
  win->levelSlider.start_value = 0;
  win->levelSlider.base_x = 511;
  win->levelSlider.base_y = 354;

  win->cabinetSlider.value = 0;
  win->cabinetSlider.start_value = 0;
  win->cabinetSlider.base_x = 812;
  win->cabinetSlider.base_y = 354;

  win->profile_path[0] = '\0';

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
    if (!strcmp(features[i]->URI, LV2_URID__map))
    {
      win->map = (LV2_URID_Map*)features[i]->data;
    }
  }

  win->uris.atom_eventTransfer = win->map->map(win->map->handle, LV2_ATOM__eventTransfer);
  win->uris.patch_Get = win->map->map(win->map->handle, LV2_PATCH__Get);
  win->uris.patch_Set = win->map->map(win->map->handle, LV2_PATCH__Set);
  win->uris.profile = win->map->map(win->map->handle, "https://faustlv2.bitbucket.io/kpp_tubeamp#profile");
  win->uris.patch_property = win->map->map(win->map->handle, LV2_PATCH__property);
  win->uris.patch_value = win->map->map(win->map->handle, LV2_PATCH__value);
  win->uris.atom_Path = win->map->map(win->map->handle, LV2_ATOM__Path);

  lv2_atom_forge_init(&win->forge, win->map);

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

  win->device = cairo_device_reference(cairo_surface_get_device(win->surface));

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
  //*widget = (void*) (size_t) win->win;
  *(uintptr_t *)widget = (uintptr_t)win->win;

  if (resize)
  {
    win->resize = resize;
    // Ask the host to properly resize the plugin host window
    resize->ui_resize(resize->handle, win->width, win->height);
  }

  win->controller = controller;
  win->write_function = write_function;


  lv2_atom_forge_set_buffer(&win->forge, win->forge_buf, sizeof(win->forge_buf));
  LV2_Atom_Forge_Frame frame;
  LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&win->forge,
                                                   &frame,
                                                   0,
                                                   win->uris.patch_Get);
  lv2_atom_forge_pop(&win->forge, &frame);

  win->write_function(win->controller, PORT_CONTROL, lv2_atom_total_size(msg),
                      win->uris.atom_eventTransfer,
                      msg);

  return (LV2UI_Handle)win;
}

static void win_deinit(win_t *win)
{
  if (win->fp_win != NULL)
  {
    delete win->fp_win;
  }
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
  cairo_device_finish(win->device);
  cairo_device_destroy(win->device);
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
  p.x = 25 + 15*sin(pointerAngle/180.0*M_PI);
  p.y = 25 - 15*cos(pointerAngle/180.0*M_PI);
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

void draw_slider(cairo_t *cr, int value, int base_x, int base_y)
{
  cairo_set_source_rgb(cr, 0, 0.35, 0);

  int length = (int)(1.83 * value);

  cairo_rectangle(cr, base_x, base_y, length, 19);
  cairo_fill(cr);
}

static void win_draw(win_t *win)
{
  cairo_push_group (win->cr);

  // File select dialog is pending
  if (win->profile_select_fd != -1)
  {
    // Draw profile name element
    cairo_set_source_rgb(win->cr, 1, 0, 0);
    cairo_rectangle(win->cr, 75, 354, 439 - 75, 373 - 354);
    cairo_fill(win->cr);

    // Draw profile name
    cairo_set_source_rgb(win->cr, 0, 0, 0);
    cairo_select_font_face(win->cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(win->cr, 18);
    cairo_move_to(win->cr, 77, 370);
  }
  // Draw profile name element in normal state
  else
  {
    // Draw profile name element
    cairo_set_source_rgb(win->cr, 0, 0, 0);
    cairo_rectangle(win->cr, 75, 354, 439 - 75, 373 - 354);
    cairo_fill(win->cr);

    // Draw profile name
    cairo_set_source_rgb(win->cr, 1, 1, 1);
    cairo_select_font_face(win->cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(win->cr, 18);
    cairo_move_to(win->cr, 77, 370);
  }

  char profile_name[PATH_MAX];

  strncpy(profile_name, win->profile_path, sizeof(profile_name));
  size_t profile_name_len = strlen(profile_name);
  if (profile_name_len >= 5)
  {
    profile_name[strlen(profile_name) - 5] = '\0';
    char *base = basename(profile_name);
    cairo_show_text(win->cr, base);
  }

  // Draw background image
  cairo_set_source_surface (win->cr, win->image, 0, 0);
  cairo_paint (win->cr);

  draw_slider(win->cr,
              win->levelSlider.value,
              win->levelSlider.base_x,
              win->levelSlider.base_y);

  draw_slider(win->cr,
              win->cabinetSlider.value,
              win->cabinetSlider.base_x,
              win->cabinetSlider.base_y);

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
            win->mastergainDial.value,
            win->mastergainDial.base_x,
            win->mastergainDial.base_y);

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

  // Atom event received
  if (format == win->uris.atom_eventTransfer)
  {
    const LV2_Atom* atom = (const LV2_Atom*)buffer;

    if (lv2_atom_forge_is_object_type(&win->forge, atom->type))
    {
      const LV2_Atom_Object* obj = (const LV2_Atom_Object*)atom;
      if (obj->body.otype == win->uris.patch_Set)
      {
        // Received patch_Set message
        const LV2_Atom* property = NULL;
        lv2_atom_object_get(obj, win->uris.patch_property, &property, 0);

        if (((const LV2_Atom_URID*)property)->body == win->uris.profile)
        {
          const LV2_Atom* value = NULL;

          lv2_atom_object_get(obj, win->uris.patch_value, &value, 0);

          if (value->type == win->uris.atom_Path)
          {
            // Received new profile path
            strncpy(win->profile_path, (const char*)LV2_ATOM_BODY_CONST(value),
                sizeof(win->profile_path));
            win->profile_path[sizeof(win->profile_path) - 1] = '\0';
          }
        }
      }
    }
  }
  // Port event received
  else if (format == 0)
  {
    switch (port_index)
    {
      case PORT_BASS:
        win->bassDial.value = db_to_value(*(float*)buffer, 10.0);
      break;
      case PORT_DRIVE:
        win->driveDial.value = clamp((int)(*(float*)buffer));
      break;
      case PORT_MASTERGAIN:
        win->mastergainDial.value = clamp((int)(*(float*)buffer));
      break;
      case PORT_MIDDLE:
        win->middleDial.value = db_to_value(*(float*)buffer, 10.0);
      break;
      case PORT_TREBLE:
        win->trebleDial.value = db_to_value(*(float*)buffer, 10.0);
      break;
      case PORT_VOLUME:
        win->levelSlider.value = clamp((int)((*(float*)buffer)*100.0));
      break;
      case PORT_CABINET:
        win->cabinetSlider.value = clamp((int)((*(float*)buffer)*100.0));
      break;
    }
  }
  // Redraw window due to value change
  win_draw(win);
}

// Determine if the point with given coordinates
// belongs to the given Dial
bool is_point_in_dial_area(int x, int y, st_dial *dial)
{
  if ((x >= dial->base_x + 5) && (x <= dial->base_x + 65)
      && (y >= dial->base_y + 5) && (y <= dial->base_y + 65))
  {
    return true;
  }
  return false;
}

// Determine if the point with given coordinates
// belongs to the given Slider
bool is_point_in_slider_area(int x, int y, st_slider *slider)
{
  if ((x >= slider->base_x) && (x <= slider->base_x + 183)
      && (y >= slider->base_y) && (y <= slider->base_y + 19))
  {
    return true;
  }
  return false;
}

// Determine if the point with given coordinates
// belongs to the Profile choose control
bool is_point_in_profile_area(int x, int y)
{
  if ((x >= 75) && (x <= 439)
      && (y >= 354) && (y <= 373))
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

// Shows file select dialog and sends Atom message
// to change profile.
static int select_new_profile_file(win_t *win)
{
  if (win->fp_win != NULL)
  {
    delete win->fp_win;
    win->fp_win = NULL;
  }
  win->fp_win = new KPPFilePicker::Win();
  win->fp_win->init();
  win->fp_win->list.filters.push_back("tapf");

  std::string currentDir = win->profile_path;
  currentDir = currentDir.substr(0, currentDir.find_last_of('/'));
  win->fp_win->setTitle("Open *.tapf Profile File");
  //win->fp_win->list.showHidden = true;
  win->fp_win->list.readDir(currentDir);

  return 0;
}

// Process X11 events
static void
win_handle_events(win_t *win)
{
  if (win->fp_win != NULL)
  {
    if (win->fp_win->isInited)
    {
      xcb_flush(win->fp_win->connection);
      xcb_generic_event_t *event;
      while ((event = xcb_poll_for_event(win->fp_win->connection)) != NULL)
      {
        win->fp_win->handle_events(event);
        free(event);
      }
      xcb_flush(win->fp_win->connection);
    }
  }

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
          else if (is_point_in_dial_area(win->pos_x, win->pos_y, &(win->mastergainDial)))
          {
            win->mastergainDial.start_value = win->mastergainDial.value;
            win->active_dial = PORT_MASTERGAIN;
          }
          else if (is_point_in_slider_area(win->pos_x, win->pos_y, &(win->levelSlider)))
          {
            win->levelSlider.start_value = win->levelSlider.value;
            win->active_dial = PORT_VOLUME;
          }
          else if (is_point_in_slider_area(win->pos_x, win->pos_y, &(win->cabinetSlider)))
          {
            win->cabinetSlider.start_value = win->cabinetSlider.value;
            win->active_dial = PORT_CABINET;
          }
          else if (is_point_in_profile_area(win->pos_x, win->pos_y))
          {
            select_new_profile_file(win);
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
              value = value_to_db(win->bassDial.value, 10.0);
              win->write_function(win->controller,PORT_BASS,sizeof(float),0,&value);
            break;
            case PORT_MIDDLE:
              win->middleDial.value = clamp(win->middleDial.start_value + win->pos_y - mev->event_y);
              value = value_to_db(win->middleDial.value, 10.0);
              win->write_function(win->controller,PORT_MIDDLE,sizeof(float),0,&value);
            break;
            case PORT_TREBLE:
              win->trebleDial.value = clamp(win->trebleDial.start_value + win->pos_y - mev->event_y);
              value = value_to_db(win->trebleDial.value, 10.0);
              win->write_function(win->controller,PORT_TREBLE,sizeof(float),0,&value);
            break;
            case PORT_MASTERGAIN:
              win->mastergainDial.value = clamp(win->mastergainDial.start_value + win->pos_y - mev->event_y);
              value = win->mastergainDial.value;
              win->write_function(win->controller,PORT_MASTERGAIN,sizeof(float),0,&value);
            break;
            case PORT_VOLUME:
              win->levelSlider.value = clamp(win->levelSlider.start_value - win->pos_x + mev->event_x);
              value = win->levelSlider.value / 100.0;
              win->write_function(win->controller,PORT_VOLUME,sizeof(float),0,&value);
            break;
            case PORT_CABINET:
              win->cabinetSlider.value = clamp(win->cabinetSlider.start_value - win->pos_x + mev->event_x);
              value = win->cabinetSlider.value / 100.0;
              win->write_function(win->controller,PORT_CABINET,sizeof(float),0,&value);
          }
          win_draw(win);
        }
      }
      break;
    }
    free(event);
  }
  xcb_flush(win->connection);

  if (win->fp_win != NULL)
  {
    if (win->fp_win->isInited)
    {
      if (win->fp_win->isFilenameUpdated)
      {
        strncpy(win->profile_path, win->fp_win->newFileName.c_str(), win->fp_win->newFileName.size());
        win->profile_path[win->fp_win->newFileName.size()] = '\0';

        lv2_atom_forge_set_buffer(&win->forge, win->forge_buf, sizeof(win->forge_buf));
        LV2_Atom_Forge_Frame frame;
        LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&win->forge,
                                                         &frame,
                                                         0,
                                                         win->uris.patch_Set);

        lv2_atom_forge_key(&win->forge, win->uris.patch_property);
        lv2_atom_forge_urid(&win->forge, win->uris.profile);
        lv2_atom_forge_key(&win->forge, win->uris.patch_value);
        lv2_atom_forge_path(&win->forge, win->profile_path, win->fp_win->newFileName.size());

        lv2_atom_forge_pop(&win->forge, &frame);

        win->write_function(win->controller, PORT_CONTROL, lv2_atom_total_size(msg),
                            win->uris.atom_eventTransfer,
                            msg);
        win_draw(win);
        win->fp_win->isFilenameUpdated = false;
      }
    }
  }
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
  (void* (*)(const LV2UI_Descriptor*, const char*, const char*, void (*)(void*, unsigned int, unsigned int, unsigned int, const void*), void*, void**, const LV2_Feature* const*))instantiate,
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
