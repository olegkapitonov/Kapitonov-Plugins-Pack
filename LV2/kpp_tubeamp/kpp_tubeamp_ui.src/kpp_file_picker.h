#ifndef KPP_FILE_PICKER_H
#define KPP_FILE_PICKER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#include <cairo.h>
#include <cairo-xcb.h>

#include <cmath>

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>

#include <string>
#include <cstring>
#include <vector>
#include <stack>

namespace KPPFilePicker {

enum FileItemType {FIT_FILE, FIT_DIR};

class Color
{
public:

  Color(uint32_t code)
  {
    setColor(code);
  }

  void setColor(uint32_t code)
  {
    _b = (code & 0x000000FF) / 255.0;
    _g = ((code & 0x0000FF00) >> 8) / 255.0;
    _r = ((code & 0x00FF0000) >> 16) / 255.0;
  }

  float r()
  {
    return _r;
  }

  float g()
  {
    return _g;
  }

  float b()
  {
    return _b;
  }

private:
  float _r;
  float _g;
  float _b;
};

struct FileItem
{
  std::string fileName;
  FileItemType type;
};

typedef std::vector<FileItem> FileList;

struct ListItem
{
  int top;
  int left;
  int width;
  int height;
  float r, g, b;
  std::string text;
  FileItemType type;

  bool checkArea(int x, int y)
  {
    if ((x >= left) && (x <= (left + width)) && (y <= (top + height)) && (y >= top))
    {
      return true;
    }
    return false;
  }
};

class List
{

public:

  Color itemColor1;
  Color itemColor2;
  Color cursorColor;
  Color backgroundColor;
  Color textColor;

  std::vector<ListItem> listItems;

  bool showHidden = false;

  int cursor_pos = 0;

  int absoluteMaxItems = 20;
  int marginLeft = 10;
  int marginRight = 10;
  int marginTop = 10;
  int marginBottom = 10;

  int width = 800;
  int height = 480;

  int fontSize = 16;

  List() : itemColor1(0x000000),
           itemColor2(0x191919),
           cursorColor(0x007F33),
           backgroundColor(0x000000),
           textColor(0xFFFFFF)
  {
  }

  void savePosition()
  {
    savedPosition = position;
  }

  void cursorUp()
  {
    if (cursor_pos != 0)
    {
      cursor_pos--;
    }
    else
    {
      if (position != 0)
      {
        position--;
      }
    }
  }

  void cursorDown()
  {
    if (cursor_pos < maxItems - 1)
    {
      cursor_pos++;
    }
    else
    {
      if (position < (int)sortedFileList.size() - maxItems)
      {
        position++;
      }
    }
  }

  void cursorBegin()
  {
    position = 0;
    cursor_pos = 0;
  }

  void cursorEnd()
  {
    position = sortedFileList.size() - maxItems;
    cursor_pos = maxItems - 1;
  }

  void setPositionDelta(int delta)
  {
    position = savedPosition + delta;
    if (position > (int)(sortedFileList.size() - maxItems))
    {
      position = sortedFileList.size() - maxItems;
    }

    if (position < 0)
    {
      position = 0;
    }
  }

  void readDir(std::string path)
  {
    if (path.substr(path.find_last_of('/') +1) == "..")
    {
      path = path.substr(0, path.find_last_of('/'));
      path = path.substr(0, path.find_last_of('/'));

      if (positionStack.size() != 0)
      {
        PositionStackItem item = positionStack.top();
        position = item.position;
        cursor_pos = item.cursor_pos;
        positionStack.pop();
      }
      else
      {
        position = 0;
        cursor_pos = 0;
      }
    }
    else
    {
      PositionStackItem item;
      item.position = position;
      item.cursor_pos = cursor_pos;
      positionStack.push(item);

      position = 0;
      cursor_pos = 0;
    }

    if (path == "")
    {
      path = "/";
    }

    currentDir = path;
    sortedFileList.resize(0);

    struct dirent **namelist;
    int n;

    n = scandir(path.c_str(), &namelist, NULL, alphasort);

    std::vector<FileItem> fileList;

    if (n < 0)
    {
      perror("scandir");
      return;
    }

    for (int i = 0; i < n; i++)
    {
      if ((!((namelist[i]->d_name[0] == '.') && !(namelist[i]->d_name[1] == '.'))) ||
        ((showHidden) && (strlen(namelist[i]->d_name) != 1))
      )
      {
        FileItem item;
        item.fileName = namelist[i]->d_name;

        if((item.fileName.substr(item.fileName.find_last_of(".") + 1) == "tapf") ||
          (namelist[i]->d_type == DT_DIR))
        {
          if (namelist[i]->d_type == DT_DIR)
          {
            item.type = FIT_DIR;
          }
          else
          {
            item.type = FIT_FILE;
          }
          fileList.push_back(item);
        }
      }
      free(namelist[i]);
    }
    free(namelist);

    for (auto it = fileList.begin(); it != fileList.end(); it++)
    {
      if (it->type == FIT_DIR)
      {
        sortedFileList.push_back(*it);
      }
    }

    for (auto it = fileList.begin(); it != fileList.end(); it++)
    {
      if (it->type == FIT_FILE)
      {
        sortedFileList.push_back(*it);
      }
    }

    maxItems = sortedFileList.size();
    if (maxItems > absoluteMaxItems)
    {
      maxItems = absoluteMaxItems;
    }
  }

  void getFileItems()
  {
    itemHeight = floor(height / absoluteMaxItems);
    listItems.resize(maxItems);

    auto fileListIterator = sortedFileList.begin() + position;

    int counter = 0;

    for (auto it = listItems.begin(); it != listItems.end(); it++)
    {
      it->type = fileListIterator->type;
      it->top = counter * itemHeight + marginTop;
      it->left = marginLeft;
      it->width = width;
      it->height = itemHeight;

      it->text = fileListIterator->fileName;

      if (counter % 2 == 0)
      {
        it->r = itemColor2.r();
        it->g = itemColor2.g();
        it->b = itemColor2.b();
      }
      else
      {
        it->r = itemColor1.r();
        it->g = itemColor1.g();
        it->b = itemColor1.b();
      }

      if (counter == cursor_pos)
      {
        it->r = cursorColor.r();
        it->g = cursorColor.g();
        it->b = cursorColor.b();
      }

      fileListIterator++;
      counter++;
    }
  }

  bool selectedItemIsDir()
  {
    if (listItems[cursor_pos].type == FIT_DIR)
    {
      return true;
    }
    return false;
  }

  std::string getFileNameOnCursor()
  {
    return currentDir + "/" + listItems[cursor_pos].text;
  }

  int getItemHeight()
  {
    return itemHeight;
  }

private:

  FileList fileItems;
  std::vector<FileItem> sortedFileList;

  std::string currentDir;

  int position = 0;
  int savedPosition = 0;

  int maxItems;
  int itemHeight;

  struct PositionStackItem
  {
    int position;
    int cursor_pos;
  };

  std::stack<PositionStackItem> positionStack;
};

// GUI window structure
class Win
{
public:

  Color buttonOpenColor;
  Color buttonCloseColor;

  bool isInited = false;
  bool isFilenameUpdated = false;

  xcb_connection_t *connection;

  std::string newFileName;

  std::string title;

  List list;

  Win() : buttonOpenColor(0xFF0010),
          buttonCloseColor(0x006010)
  {
  }

  void run()
  {
    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event (connection)))
    {
      handle_events(e);
    }
  }

  void init()
  {
    isFilenameUpdated = false;
    connection = xcb_connect(NULL, NULL);

    if (connection == NULL)
    {
      fprintf(stderr, "Failed to open display\n");
      return;
    }

    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    width = 700;
    height = 550;

    list.width = width;

    win = xcb_generate_id(connection);

    uint32_t mask = XCB_CW_EVENT_MASK;
    uint32_t mask_values[1] = {
      XCB_EVENT_MASK_STRUCTURE_NOTIFY |
      XCB_EVENT_MASK_EXPOSURE |
      XCB_EVENT_MASK_BUTTON_PRESS |
      XCB_EVENT_MASK_BUTTON_1_MOTION |
      XCB_EVENT_MASK_KEY_PRESS
    };

    xcb_create_window(connection, XCB_COPY_FROM_PARENT,
                      win, screen->root,
                      0, 0, width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual, mask, mask_values);

    /*title = "Open *.tapf Profile File";
    xcb_change_property (connection,
                         XCB_PROP_MODE_REPLACE,
                         win,
                         XCB_ATOM_WM_NAME,
                         XCB_ATOM_STRING,
                         8,
                         title.size(),
                         title.c_str());*/

    xcb_size_hints_t size_hints;
    memset(&size_hints, 0, sizeof(size_hints));
    xcb_icccm_size_hints_set_size(&size_hints, 1, width, height);
    xcb_icccm_size_hints_set_min_size(&size_hints, width, height);
    xcb_icccm_size_hints_set_max_size(&size_hints, width, height);

    xcb_icccm_set_wm_size_hints(connection, win, XCB_ATOM_WM_NORMAL_HINTS, &size_hints);

    xcb_map_window(connection, win);

    const static int values[] = { (screen->width_in_pixels - width) / 2, (screen->height_in_pixels - height) / 2 };

    xcb_configure_window (connection, win, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);

    xcb_flush(connection);

    visual = xcb_aux_find_visual_by_id(screen, screen->root_visual);
    xcb_clear_area(connection, 0, win, 0, 0, 0, 0);
    surface = cairo_xcb_surface_create(connection, win, visual,
                                       width, height);
    device = cairo_device_reference(cairo_surface_get_device(surface));

    cr = cairo_create(surface);

    isInited = true;
  }

  void setTitle(std::string _title)
  {
    title = _title;

    xcb_change_property (connection,
                         XCB_PROP_MODE_REPLACE,
                         win,
                         XCB_ATOM_WM_NAME,
                         XCB_ATOM_STRING,
                         8,
                         title.size(),
                         title.c_str());
  }

  void handle_events(xcb_generic_event_t *event)
  {
    xcb_flush(connection);
    switch(event->response_type & ~0x80)
    {
      case XCB_EXPOSE: // the window must be redrawn
      {
        xcb_expose_event_t *eev = (xcb_expose_event_t *) event;

        if (eev->count == 0)
        {
          draw();
        }
      }
      break;
      case XCB_BUTTON_PRESS: // Left mouse button pressed
      {
        xcb_button_press_event_t *bev = (xcb_button_press_event_t *) event;
        switch (bev->detail)
        {
          case XCB_BUTTON_INDEX_1: // Left mouse
          {
            // Open button
            if ((bev->event_x >= width - buttonOpenLeft) &&
              (bev->event_x <= width - buttonOpenLeft + buttonWidth) &&
              (bev->event_y >= height - buttonHeight - list.marginBottom) &&
              (bev->event_y <= height - buttonHeight - list.marginBottom + buttonHeight))
            {
              finished();
            }
            // Cancel button
            else if ((bev->event_x >=  width - buttonCloseLeft) &&
              (bev->event_x <=  width - buttonCloseLeft + buttonWidth) &&
              (bev->event_y >= height - buttonHeight - list.marginBottom) &&
              (bev->event_y <= height - buttonHeight - list.marginBottom + buttonHeight))
            {
              xcb_destroy_window(connection, win);
              cairo_destroy(cr);
              cairo_surface_destroy(surface);
              cairo_device_finish(device);
              cairo_device_destroy(device);
              xcb_disconnect(connection);
            }
            else if ((bev->time - lastclick_time) < 400)
            {
              finished();
            }
            else
            {
              for (int i = 0; i < (int)list.listItems.size(); i++)
              {
                if (list.listItems[i].checkArea(bev->event_x, bev->event_y))
                {
                  list.cursor_pos = i;
                  draw();
                  break;
                }
              }
            }
            list.savePosition();
            lastclick_time = bev->time;
            last_y = bev->event_y;
          }
          break;
          case XCB_BUTTON_INDEX_4:
            list.cursorUp();
            draw();
            break;
          case XCB_BUTTON_INDEX_5:
            list.cursorDown();
            draw();
            break;
        }
      }
      break;
          case XCB_MOTION_NOTIFY: // Mouse move while left button is pressed
          {
            xcb_motion_notify_event_t *mev = (xcb_motion_notify_event_t *) event;
            int delta = (last_y - mev->event_y) / list.getItemHeight();
            list.setPositionDelta(delta);
            lastclick_time = mev->time;
            draw();
          }
          break;
          case XCB_KEY_PRESS:
          {
            xcb_key_press_event_t *ev = (xcb_key_press_event_t *)event;
            switch (ev->detail)
            {
              case 116: // Arrow down
                list.cursorDown();
                draw();
                break;
              case 111: // Arrow up
                list.cursorUp();
                draw();
                break;
              case 110: // Home
                list.cursorBegin();
                draw();
                break;
              case 115: // End
                list.cursorEnd();
                draw();
                break;
              case 36: //Enter
                finished();
                break;
              case 22: // Backspace
              {
                std::string parentDir = list.getFileNameOnCursor();
                parentDir = parentDir.substr(0, parentDir.find_last_of('/'));
                parentDir += "/..";
                //parentDir = parentDir.substr(0, parentDir.find_last_of('/'));
                list.readDir(parentDir);
                draw();
              }
                break;
            }
          }
          break;
          free(event);
    }
    xcb_flush(connection);
  }

private:


  xcb_window_t win;

  int width, height;
  long event_mask;

  // Cairo objects
  cairo_t *cr;
  cairo_surface_t *surface;
  xcb_visualtype_t *visual;

  cairo_device_t *device;

  xcb_timestamp_t lastclick_time = 0;
  int last_y;
  int buttonWidth = 100;
  int buttonHeight = 30;

  int buttonOpenLeft = 300;
  int buttonCloseLeft = 150;

  void draw()
  {
    cairo_push_group (cr);

    cairo_set_source_rgb(cr, list.backgroundColor.r(), list.backgroundColor.g(), list.backgroundColor.b());
    cairo_fill(cr);

    cairo_paint(cr);

    cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, list.fontSize);

    list.getFileItems();

    cairo_text_extents_t cte;
    cairo_text_extents_t cte_h;

    for (auto it = list.listItems.begin(); it != list.listItems.end(); it++)
    {
      cairo_set_source_rgb(cr, it->r, it->g, it->b);
      cairo_rectangle(cr, it->left, it->top, it->width, it->height);
      cairo_fill(cr);

      cairo_set_source_rgb(cr, list.textColor.r(), list.textColor.g(), list.textColor.b());
      cairo_text_extents(cr, "A", &cte);
      cairo_move_to(cr, it->left, it->top + list.getItemHeight() - (list.getItemHeight() - (int)cte.height) / 2);
      if (it->type == FIT_DIR)
      {
        cairo_show_text(cr, std::string("[" + it->text + "]").c_str());
      }
      else
      {
        cairo_show_text(cr, it->text.c_str());
      }
    }

    cairo_set_source_rgb(cr, list.backgroundColor.r(), list.backgroundColor.g(), list.backgroundColor.b());
    cairo_rectangle(cr, width - list.marginRight, 0, list.marginRight, height);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, buttonOpenColor.r(), buttonOpenColor.g(), buttonOpenColor.b());
    cairo_rectangle(cr, width - buttonOpenLeft, height - buttonHeight - list.marginBottom,
                    buttonWidth, buttonHeight);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, list.textColor.r(), list.textColor.g(), list.textColor.b());
    cairo_text_extents(cr, "Open", &cte);
    cairo_text_extents(cr, "a", &cte_h);
    cairo_move_to(cr, width - buttonOpenLeft + (buttonWidth - (int)cte.width) / 2,
                  height - buttonHeight - list.marginBottom +
                  (buttonHeight + (int)cte_h.height) / 2);
    cairo_show_text(cr, "Open");

    cairo_set_source_rgb(cr, buttonCloseColor.r(), buttonCloseColor.g(), buttonCloseColor.b());
    cairo_rectangle(cr, width - buttonCloseLeft, height - buttonHeight - list.marginBottom,
                    buttonWidth, buttonHeight);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, list.textColor.r(), list.textColor.g(), list.textColor.b());
    cairo_text_extents(cr, "Cancel", &cte);
    cairo_move_to(cr, width - buttonCloseLeft + (buttonWidth - (int)cte.width) / 2,
                  height - buttonHeight - list.marginBottom +
                  (buttonHeight + (int)cte_h.height) / 2);
    cairo_show_text(cr, "Cancel");

    cairo_pop_group_to_source (cr);
    cairo_paint (cr);
  }

  void finished()
  {
    if (list.selectedItemIsDir())
    {
      list.readDir(list.getFileNameOnCursor());
      draw();
    }
    else
    {
      printf((list.getFileNameOnCursor() + "\n").c_str());
      isFilenameUpdated = true;
      newFileName = list.getFileNameOnCursor();
      xcb_destroy_window(connection, win);
      cairo_destroy(cr);
      cairo_surface_destroy(surface);
      cairo_device_finish(device);
      cairo_device_destroy(device);
      xcb_disconnect(connection);
    }
  }
};

} // End of namespace

#endif
