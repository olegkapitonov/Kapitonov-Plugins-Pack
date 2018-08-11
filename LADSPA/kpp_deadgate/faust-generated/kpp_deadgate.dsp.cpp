/* ------------------------------------------------------------
author: "Oleg Kapitonov"
license: "GPLv3"
name: "kpp_distruction"
version: "0.1b"
Code generated with Faust 2.5.23 (https://faust.grame.fr)
Compilation options: cpp, -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

/************************************************************************

	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
    Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; If not, see <http://www.gnu.org/licenses/>.

    EXCEPTION : As a special exception, you may create a larger work
    that contains this FAUST architecture section and distribute
    that work under terms of your choice, so long as this FAUST
    architecture section is not modified.

 ************************************************************************
 ************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <stack>
#include <string>
#include <iostream>
#include <map>

#include "ladspa.h"
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/
 
#ifndef __GUI_H__
#define __GUI_H__

#include <list>
#include <map>
#include <vector>
#include <iostream>

/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __UI_H__
#define __UI_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

struct Soundfile;

class UI
{

    public:

        UI() {}

        virtual ~UI() {}

        // -- widget's layouts

        virtual void openTabBox(const char* label) = 0;
        virtual void openHorizontalBox(const char* label) = 0;
        virtual void openVerticalBox(const char* label) = 0;
        virtual void closeBox() = 0;

        // -- active widgets

        virtual void addButton(const char* label, FAUSTFLOAT* zone) = 0;
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) = 0;
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;

        // -- passive widgets

        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;
    
        // -- soundfiles
    
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) = 0;

        // -- metadata declarations

        virtual void declare(FAUSTFLOAT*, const char*, const char*) {}
};

#endif
/*
  Copyright (C) 2000 Paul Davis
  Copyright (C) 2003 Rohan Drape
  Copyright (C) 2016 GRAME (renaming for internal use)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

  ISO/POSIX C version of Paul Davis's lock free ringbuffer C++ code.
  This is safe for the case of one read thread and one write thread.
*/

#ifndef __ring_buffer__
#define __ring_buffer__

#include <stdlib.h>
#include <string.h>

typedef struct {
    char *buf;
    size_t len;
}
ringbuffer_data_t;

typedef struct {
    char *buf;
    volatile size_t write_ptr;
    volatile size_t read_ptr;
    size_t	size;
    size_t	size_mask;
    int	mlocked;
}
ringbuffer_t;

ringbuffer_t *ringbuffer_create(size_t sz);
void ringbuffer_free(ringbuffer_t *rb);
void ringbuffer_get_read_vector(const ringbuffer_t *rb,
                                         ringbuffer_data_t *vec);
void ringbuffer_get_write_vector(const ringbuffer_t *rb,
                                          ringbuffer_data_t *vec);
size_t ringbuffer_read(ringbuffer_t *rb, char *dest, size_t cnt);
size_t ringbuffer_peek(ringbuffer_t *rb, char *dest, size_t cnt);
void ringbuffer_read_advance(ringbuffer_t *rb, size_t cnt);
size_t ringbuffer_read_space(const ringbuffer_t *rb);
int ringbuffer_mlock(ringbuffer_t *rb);
void ringbuffer_reset(ringbuffer_t *rb);
void ringbuffer_reset_size (ringbuffer_t * rb, size_t sz);
size_t ringbuffer_write(ringbuffer_t *rb, const char *src,
                                 size_t cnt);
void ringbuffer_write_advance(ringbuffer_t *rb, size_t cnt);
size_t ringbuffer_write_space(const ringbuffer_t *rb);

/* Create a new ringbuffer to hold at least `sz' bytes of data. The
   actual buffer size is rounded up to the next power of two.  */

inline ringbuffer_t *
ringbuffer_create (size_t sz)
{
	size_t power_of_two;
	ringbuffer_t *rb;

	if ((rb = (ringbuffer_t *) malloc (sizeof (ringbuffer_t))) == NULL) {
		return NULL;
	}

	for (power_of_two = 1u; 1u << power_of_two < sz; power_of_two++);

	rb->size = 1u << power_of_two;
	rb->size_mask = rb->size;
	rb->size_mask -= 1;
	rb->write_ptr = 0;
	rb->read_ptr = 0;
	if ((rb->buf = (char *) malloc (rb->size)) == NULL) {
		free (rb);
		return NULL;
	}
	rb->mlocked = 0;

	return rb;
}

/* Free all data associated with the ringbuffer `rb'. */

inline void
ringbuffer_free (ringbuffer_t * rb)
{
#ifdef USE_MLOCK
	if (rb->mlocked) {
		munlock (rb->buf, rb->size);
	}
#endif /* USE_MLOCK */
	free (rb->buf);
	free (rb);
}

/* Lock the data block of `rb' using the system call 'mlock'.  */

inline int
ringbuffer_mlock (ringbuffer_t * rb)
{
#ifdef USE_MLOCK
	if (mlock (rb->buf, rb->size)) {
		return -1;
	}
#endif /* USE_MLOCK */
	rb->mlocked = 1;
	return 0;
}

/* Reset the read and write pointers to zero. This is not thread
   safe. */

inline void
ringbuffer_reset (ringbuffer_t * rb)
{
	rb->read_ptr = 0;
	rb->write_ptr = 0;
    memset(rb->buf, 0, rb->size);
}

/* Reset the read and write pointers to zero. This is not thread
   safe. */

inline void
ringbuffer_reset_size (ringbuffer_t * rb, size_t sz)
{
    rb->size = sz;
    rb->size_mask = rb->size;
    rb->size_mask -= 1;
    rb->read_ptr = 0;
    rb->write_ptr = 0;
}

/* Return the number of bytes available for reading.  This is the
   number of bytes in front of the read pointer and behind the write
   pointer.  */

inline size_t
ringbuffer_read_space (const ringbuffer_t * rb)
{
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r) {
		return w - r;
	} else {
		return (w - r + rb->size) & rb->size_mask;
	}
}

/* Return the number of bytes available for writing.  This is the
   number of bytes in front of the write pointer and behind the read
   pointer.  */

inline size_t
ringbuffer_write_space (const ringbuffer_t * rb)
{
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r) {
		return ((r - w + rb->size) & rb->size_mask) - 1;
	} else if (w < r) {
		return (r - w) - 1;
	} else {
		return rb->size - 1;
	}
}

/* The copying data reader.  Copy at most `cnt' bytes from `rb' to
   `dest'.  Returns the actual number of bytes copied. */

inline size_t
ringbuffer_read (ringbuffer_t * rb, char *dest, size_t cnt)
{
	size_t free_cnt;
	size_t cnt2;
	size_t to_read;
	size_t n1, n2;

	if ((free_cnt = ringbuffer_read_space (rb)) == 0) {
		return 0;
	}

	to_read = cnt > free_cnt ? free_cnt : cnt;

	cnt2 = rb->read_ptr + to_read;

	if (cnt2 > rb->size) {
		n1 = rb->size - rb->read_ptr;
		n2 = cnt2 & rb->size_mask;
	} else {
		n1 = to_read;
		n2 = 0;
	}

	memcpy (dest, &(rb->buf[rb->read_ptr]), n1);
	rb->read_ptr = (rb->read_ptr + n1) & rb->size_mask;

	if (n2) {
		memcpy (dest + n1, &(rb->buf[rb->read_ptr]), n2);
		rb->read_ptr = (rb->read_ptr + n2) & rb->size_mask;
	}

	return to_read;
}

/* The copying data reader w/o read pointer advance.  Copy at most
   `cnt' bytes from `rb' to `dest'.  Returns the actual number of bytes
   copied. */

inline size_t
ringbuffer_peek (ringbuffer_t * rb, char *dest, size_t cnt)
{
	size_t free_cnt;
	size_t cnt2;
	size_t to_read;
	size_t n1, n2;
	size_t tmp_read_ptr;

	tmp_read_ptr = rb->read_ptr;

	if ((free_cnt = ringbuffer_read_space (rb)) == 0) {
		return 0;
	}

	to_read = cnt > free_cnt ? free_cnt : cnt;

	cnt2 = tmp_read_ptr + to_read;

	if (cnt2 > rb->size) {
		n1 = rb->size - tmp_read_ptr;
		n2 = cnt2 & rb->size_mask;
	} else {
		n1 = to_read;
		n2 = 0;
	}

	memcpy (dest, &(rb->buf[tmp_read_ptr]), n1);
	tmp_read_ptr = (tmp_read_ptr + n1) & rb->size_mask;

	if (n2) {
		memcpy (dest + n1, &(rb->buf[tmp_read_ptr]), n2);
	}

	return to_read;
}

/* The copying data writer.  Copy at most `cnt' bytes to `rb' from
   `src'.  Returns the actual number of bytes copied. */

inline size_t
ringbuffer_write (ringbuffer_t * rb, const char *src, size_t cnt)
{
	size_t free_cnt;
	size_t cnt2;
	size_t to_write;
	size_t n1, n2;

	if ((free_cnt = ringbuffer_write_space (rb)) == 0) {
		return 0;
	}

	to_write = cnt > free_cnt ? free_cnt : cnt;

	cnt2 = rb->write_ptr + to_write;

	if (cnt2 > rb->size) {
		n1 = rb->size - rb->write_ptr;
		n2 = cnt2 & rb->size_mask;
	} else {
		n1 = to_write;
		n2 = 0;
	}

	memcpy (&(rb->buf[rb->write_ptr]), src, n1);
	rb->write_ptr = (rb->write_ptr + n1) & rb->size_mask;

	if (n2) {
		memcpy (&(rb->buf[rb->write_ptr]), src + n1, n2);
		rb->write_ptr = (rb->write_ptr + n2) & rb->size_mask;
	}

	return to_write;
}

/* Advance the read pointer `cnt' places. */

inline void
ringbuffer_read_advance (ringbuffer_t * rb, size_t cnt)
{
	size_t tmp = (rb->read_ptr + cnt) & rb->size_mask;
	rb->read_ptr = tmp;
}

/* Advance the write pointer `cnt' places. */

inline void
ringbuffer_write_advance (ringbuffer_t * rb, size_t cnt)
{
	size_t tmp = (rb->write_ptr + cnt) & rb->size_mask;
	rb->write_ptr = tmp;
}

/* The non-copying data reader.  `vec' is an array of two places.  Set
   the values at `vec' to hold the current readable data at `rb'.  If
   the readable data is in one segment the second segment has zero
   length.  */

inline void
ringbuffer_get_read_vector (const ringbuffer_t * rb,
				 ringbuffer_data_t * vec)
{
	size_t free_cnt;
	size_t cnt2;
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r) {
		free_cnt = w - r;
	} else {
		free_cnt = (w - r + rb->size) & rb->size_mask;
	}

	cnt2 = r + free_cnt;

	if (cnt2 > rb->size) {

		/* Two part vector: the rest of the buffer after the current write
		   ptr, plus some from the start of the buffer. */

		vec[0].buf = &(rb->buf[r]);
		vec[0].len = rb->size - r;
		vec[1].buf = rb->buf;
		vec[1].len = cnt2 & rb->size_mask;

	} else {

		/* Single part vector: just the rest of the buffer */

		vec[0].buf = &(rb->buf[r]);
		vec[0].len = free_cnt;
		vec[1].len = 0;
	}
}

/* The non-copying data writer.  `vec' is an array of two places.  Set
   the values at `vec' to hold the current writeable data at `rb'.  If
   the writeable data is in one segment the second segment has zero
   length.  */

inline void
ringbuffer_get_write_vector (const ringbuffer_t * rb,
				  ringbuffer_data_t * vec)
{
	size_t free_cnt;
	size_t cnt2;
	size_t w, r;

	w = rb->write_ptr;
	r = rb->read_ptr;

	if (w > r) {
		free_cnt = ((r - w + rb->size) & rb->size_mask) - 1;
	} else if (w < r) {
		free_cnt = (r - w) - 1;
	} else {
		free_cnt = rb->size - 1;
	}

	cnt2 = w + free_cnt;

	if (cnt2 > rb->size) {

		/* Two part vector: the rest of the buffer after the current write
		   ptr, plus some from the start of the buffer. */

		vec[0].buf = &(rb->buf[w]);
		vec[0].len = rb->size - w;
		vec[1].buf = rb->buf;
		vec[1].len = cnt2 & rb->size_mask;
	} else {
		vec[0].buf = &(rb->buf[w]);
		vec[0].len = free_cnt;
		vec[1].len = 0;
	}
}

#endif // __ring_buffer__

/*******************************************************************************
 * GUI : Abstract Graphic User Interface
 * Provides additional mechanisms to synchronize widgets and zones. Widgets
 * should both reflect the value of a zone and allow to change this value.
 ******************************************************************************/

class uiItem;
typedef void (*uiCallback)(FAUSTFLOAT val, void* data);

class clist : public std::list<uiItem*>
{
    
    public:
    
        virtual ~clist();
        
};

typedef std::map<FAUSTFLOAT*, clist*> zmap;

typedef std::map<FAUSTFLOAT*, ringbuffer_t*> ztimedmap;

class GUI : public UI
{
		
    private:
     
        static std::list<GUI*> fGuiList;
        zmap fZoneMap;
        bool fStopped;
        
     public:
            
        GUI():fStopped(false)
        {	
            fGuiList.push_back(this);
        }
        
        virtual ~GUI() 
        {   
            // delete all items
            zmap::iterator it;
            for (it = fZoneMap.begin(); it != fZoneMap.end(); it++) {
                delete (*it).second;
            }
            // suppress 'this' in static fGuiList
            fGuiList.remove(this);
        }

        // -- registerZone(z,c) : zone management
        
        void registerZone(FAUSTFLOAT* z, uiItem* c)
        {
            if (fZoneMap.find(z) == fZoneMap.end()) fZoneMap[z] = new clist();
            fZoneMap[z]->push_back(c);
        }

        void updateAllZones();
        
        void updateZone(FAUSTFLOAT* z);
        
        static void updateAllGuis()
        {
            std::list<GUI*>::iterator g;
            for (g = fGuiList.begin(); g != fGuiList.end(); g++) {
                (*g)->updateAllZones();
            }
        }
        void addCallback(FAUSTFLOAT* zone, uiCallback foo, void* data);
        virtual void show() {};	
        virtual bool run() { return false; };
    
        virtual void stop() { fStopped = true; }
        bool stopped() { return fStopped; }
    
        // -- widget's layouts
        
        virtual void openTabBox(const char* label) {};
        virtual void openHorizontalBox(const char* label) {}
        virtual void openVerticalBox(const char* label) {}
        virtual void closeBox() {}
        
        // -- active widgets
        
        virtual void addButton(const char* label, FAUSTFLOAT* zone) {}
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) {}
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {}
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {}
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {}
    
        // -- passive widgets
        
        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
    
        // -- soundfiles
    
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
    
        // -- metadata declarations

        virtual void declare(FAUSTFLOAT* , const char* , const char*) {}
    
        // Static global for timed zones, shared between all UI that will set timed values
        static ztimedmap gTimedZoneMap;

};

/**
 * User Interface Item: abstract definition
 */

class uiItem
{
    protected:
          
        GUI* fGUI;
        FAUSTFLOAT* fZone;
        FAUSTFLOAT fCache;

        uiItem(GUI* ui, FAUSTFLOAT* zone):fGUI(ui), fZone(zone), fCache(FAUSTFLOAT(-123456.654321))
        { 
            ui->registerZone(zone, this); 
        }

    public:

        virtual ~uiItem() 
        {}

        void modifyZone(FAUSTFLOAT v) 	
        { 
            fCache = v;
            if (*fZone != v) {
                *fZone = v;
                fGUI->updateZone(fZone);
            }
        }
                
        FAUSTFLOAT cache() { return fCache; }
        virtual void reflectZone() = 0;
};

/**
 * User Interface item owned (and so deleted) by external code
 */

class uiOwnedItem : public uiItem {
    
    protected:
    
        uiOwnedItem(GUI* ui, FAUSTFLOAT* zone):uiItem(ui, zone)
        {}
    
     public:
    
        virtual ~uiOwnedItem()
        {}
    
        virtual void reflectZone() {}
};

/**
 * Callback Item
 */

struct uiCallbackItem : public uiItem {
    
	uiCallback fCallback;
	void* fData;
	
	uiCallbackItem(GUI* ui, FAUSTFLOAT* zone, uiCallback foo, void* data) 
			: uiItem(ui, zone), fCallback(foo), fData(data) {}
	
	virtual void reflectZone() 
    {		
		FAUSTFLOAT 	v = *fZone;
		fCache = v; 
		fCallback(v, fData);	
	}
};

/**
 * Base class for timed items
 */

// For precise timestamped control
struct DatedControl {
    
    double fDate;
    FAUSTFLOAT fValue;
    
    DatedControl(double d = 0., FAUSTFLOAT v = FAUSTFLOAT(0)):fDate(d), fValue(v) {}
    
};

class uiTimedItem : public uiItem
{
    
    protected:
        
        bool fDelete;
        
    public:
        
        uiTimedItem(GUI* ui, FAUSTFLOAT* zone):uiItem(ui, zone)
        {
            if (GUI::gTimedZoneMap.find(fZone) == GUI::gTimedZoneMap.end()) {
                GUI::gTimedZoneMap[fZone] = ringbuffer_create(8192);
                fDelete = true;
            } else {
                fDelete = false;
            }
        }
        
        virtual ~uiTimedItem()
        {
            ztimedmap::iterator it;
            if (fDelete && ((it = GUI::gTimedZoneMap.find(fZone)) != GUI::gTimedZoneMap.end())) {
                ringbuffer_free((*it).second);
                GUI::gTimedZoneMap.erase(it);
            }
        }
        
        virtual void modifyZone(double date, FAUSTFLOAT v)
        {
            size_t res;
            DatedControl dated_val(date, v);
            if ((res = ringbuffer_write(GUI::gTimedZoneMap[fZone], (const char*)&dated_val, sizeof(DatedControl))) != sizeof(DatedControl)) {
                std::cerr << "ringbuffer_write error DatedControl" << std::endl;
            }
        }
    
};

/**
 * Allows to group a set of zones
 */

class uiGroupItem : public uiItem
{
    protected:
    
        std::vector<FAUSTFLOAT*> fZoneMap;

    public:
    
        uiGroupItem(GUI* ui, FAUSTFLOAT* zone):uiItem(ui, zone)
        {}
        virtual ~uiGroupItem() 
        {}
        
        virtual void reflectZone() 
        {
            FAUSTFLOAT v = *fZone;
            fCache = v;
            
            // Update all zones of the same group
            std::vector<FAUSTFLOAT*>::iterator it;
            for (it = fZoneMap.begin(); it != fZoneMap.end(); it++) {
                (*(*it)) = v;
            }
        }
        
        void addZone(FAUSTFLOAT* zone) { fZoneMap.push_back(zone); }

};

/**
 * Update all user items reflecting zone z
 */

inline void GUI::updateZone(FAUSTFLOAT* z)
{
	FAUSTFLOAT v = *z;
	clist* l = fZoneMap[z];
	for (clist::iterator c = l->begin(); c != l->end(); c++) {
		if ((*c)->cache() != v) (*c)->reflectZone();
	}
}

/**
 * Update all user items not up to date
 */

inline void GUI::updateAllZones()
{
	for (zmap::iterator m = fZoneMap.begin(); m != fZoneMap.end(); m++) {
		FAUSTFLOAT* z = m->first;
		clist*	l = m->second;
        if (z) {
            FAUSTFLOAT	v = *z;
            for (clist::iterator c = l->begin(); c != l->end(); c++) {
                if ((*c)->cache() != v) (*c)->reflectZone();
            }
        }
	}
}

inline void GUI::addCallback(FAUSTFLOAT* zone, uiCallback foo, void* data) 
{ 
	new uiCallbackItem(this, zone, foo, data); 
};

inline clist::~clist() 
{
    std::list<uiItem*>::iterator it;
    for (it = begin(); it != end(); it++) {
        uiOwnedItem* owned = dynamic_cast<uiOwnedItem*>(*it);
        // owned items are deleted by external code
        if (!owned) {
            delete (*it);
        }
    }
}

#endif
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/
 
#ifndef __misc__
#define __misc__

#include <algorithm>
#include <map>
#include <string.h>
#include <stdlib.h>

/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __meta__
#define __meta__

struct Meta
{
    virtual void declare(const char* key, const char* value) = 0;
    virtual ~Meta() {};
};

#endif

using std::max;
using std::min;

struct XXXX_Meta : std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key]=value; }
};

struct MY_Meta : Meta, std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key]=value; }
};

inline int lsr(int x, int n)	{ return int(((unsigned int)x) >> n); }

inline int int2pow2(int x)		{ int r = 0; while ((1<<r) < x) r++; return r; }

inline long lopt(char* argv[], const char* name, long def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return atoi(argv[i+1]);
	return def;
}

inline bool isopt(char* argv[], const char* name)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return true;
	return false;
}

inline const char* lopts(char* argv[], const char* name, const char* def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return argv[i+1];
	return def;
}

#endif

/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class UI;
struct Meta;

/**
 * DSP memory manager.
 */

struct dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    virtual void* allocate(size_t size) = 0;
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class dsp {

    public:

        dsp() {}
        virtual ~dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'addBtton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Returns the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param samplingRate - the sampling rate in Hertz
         */
        virtual void init(int samplingRate) = 0;

        /**
         * Init instance state
         *
         * @param samplingRate - the sampling rate in Hertz
         */
        virtual void instanceInit(int samplingRate) = 0;

        /**
         * Init instance constant state
         *
         * @param samplingRate - the sampling rate in Hertz
         */
        virtual void instanceConstants(int samplingRate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (delay lines...) */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual dsp* clone() = 0;
    
        /**
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value) metadata.
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with successive in/out audio buffers.
         *
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = 0):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() { return fDSP->getSampleRate(); }
        virtual void init(int samplingRate) { fDSP->init(samplingRate); }
        virtual void instanceInit(int samplingRate) { fDSP->instanceInit(samplingRate); }
        virtual void instanceConstants(int samplingRate) { fDSP->instanceConstants(samplingRate); }
        virtual void instanceResetUserInterface() { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) { fDSP->metadata(m); }
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class.
 */

class dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() {}
    
    public:
    
        virtual std::string getName() = 0;
        virtual std::string getSHAKey() = 0;
        virtual std::string getDSPCode() = 0;
    
        virtual dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif

#define sym(name) xsym(name)
#define xsym(name) #name

/******************************************************************************
*******************************************************************************

							       VECTOR INTRINSICS

*******************************************************************************
*******************************************************************************/


/********************END ARCHITECTURE SECTION (part 1/2)****************/

/**************************BEGIN USER SECTION **************************/

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <cmath>
#include <math.h>

float mydsp_faustpower2_f(float value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	int fSamplingFreq;
	float fConst0;
	float fConst1;
	float fConst2;
	float fConst3;
	float fConst4;
	float fConst5;
	float fConst6;
	float fConst7;
	float fConst8;
	float fConst9;
	float fConst10;
	FAUSTFLOAT fVslider0;
	float fConst11;
	float fConst12;
	float fConst13;
	float fConst14;
	float fVec0[2];
	float fConst15;
	float fRec11[2];
	float fVec1[2];
	float fConst16;
	float fRec10[2];
	float fConst17;
	float fConst18;
	float fRec9[3];
	float fConst19;
	float fConst20;
	float fConst21;
	float fConst22;
	float fConst23;
	float fConst24;
	float fConst25;
	float fRec8[3];
	float fConst26;
	float fConst27;
	float fConst28;
	float fConst29;
	float fConst30;
	float fConst31;
	float fRec7[3];
	float fConst32;
	float fConst33;
	float fConst34;
	float fConst35;
	float fConst36;
	float fConst37;
	float fRec6[3];
	float fConst38;
	float fConst39;
	float fConst40;
	float fConst41;
	float fConst42;
	float fConst43;
	float fRec5[3];
	float fConst44;
	float fConst45;
	float fConst46;
	float fConst47;
	float fConst48;
	float fConst49;
	float fRec4[3];
	float fRec3[2];
	FAUSTFLOAT fVslider1;
	int iVec2[2];
	int iConst50;
	int iRec12[2];
	float fConst51;
	float fRec1[2];
	float fRec0[2];
	float fConst52;
	float fConst53;
	float fConst54;
	float fConst55;
	float fConst56;
	float fRec24[2];
	float fRec23[3];
	float fVec3[2];
	float fConst57;
	float fRec22[2];
	float fConst58;
	float fRec21[3];
	float fConst59;
	float fRec20[3];
	float fRec19[3];
	float fRec18[3];
	float fRec17[3];
	float fRec16[2];
	int iVec4[2];
	int iRec25[2];
	float fRec14[2];
	float fRec13[2];
	float fConst60;
	float fConst61;
	float fConst62;
	float fConst63;
	float fConst64;
	float fRec36[2];
	float fRec35[3];
	float fVec5[2];
	float fConst65;
	float fRec34[2];
	float fConst66;
	float fRec33[3];
	float fConst67;
	float fRec32[3];
	float fRec31[3];
	float fRec30[3];
	float fRec29[2];
	int iVec6[2];
	int iRec37[2];
	float fRec27[2];
	float fRec26[2];
	float fConst68;
	float fConst69;
	float fConst70;
	float fConst71;
	float fConst72;
	float fRec47[2];
	float fRec46[3];
	float fVec7[2];
	float fConst73;
	float fRec45[2];
	float fConst74;
	float fRec44[3];
	float fConst75;
	float fRec43[3];
	float fRec42[3];
	float fRec41[2];
	int iVec8[2];
	int iRec48[2];
	float fRec39[2];
	float fRec38[2];
	float fConst76;
	float fConst77;
	float fConst78;
	float fConst79;
	float fConst80;
	float fRec57[2];
	float fRec56[3];
	float fVec9[2];
	float fConst81;
	float fRec55[2];
	float fConst82;
	float fRec54[3];
	float fConst83;
	float fRec53[3];
	float fRec52[2];
	int iVec10[2];
	int iRec58[2];
	float fRec50[2];
	float fRec49[2];
	float fConst84;
	float fConst85;
	float fConst86;
	float fConst87;
	float fRec66[2];
	float fRec65[3];
	float fVec11[2];
	float fConst88;
	float fRec64[2];
	float fConst89;
	float fRec63[3];
	float fConst90;
	float fRec62[2];
	int iVec12[2];
	int iRec67[2];
	float fRec60[2];
	float fRec59[2];
	float fRec73[2];
	float fRec72[3];
	float fRec71[2];
	int iVec13[2];
	int iRec74[2];
	float fRec69[2];
	float fRec68[2];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "0.0");
		m->declare("author", "Oleg Kapitonov");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("filename", "kpp_deadgate");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("license", "GPLv3");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("misceffects.lib/name", "Faust Math Library");
		m->declare("misceffects.lib/version", "2.0");
		m->declare("name", "kpp_distruction");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "0.1b");
	}

	virtual int getNumInputs() {
		return 1;
		
	}
	virtual int getNumOutputs() {
		return 1;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	
	static void classInit(int samplingFreq) {
		
	}
	
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(192000.0f, max(1.0f, float(fSamplingFreq)));
		fConst1 = expf((0.0f - (100.0f / fConst0)));
		fConst2 = (1.0f - fConst1);
		fConst3 = tanf((7539.82227f / fConst0));
		fConst4 = (1.0f / fConst3);
		fConst5 = (((fConst4 + 1.0f) / fConst3) + 1.0f);
		fConst6 = (1.0f / fConst5);
		fConst7 = (1.0f / mydsp_faustpower2_f(fConst3));
		fConst8 = (fConst4 + 1.0f);
		fConst9 = (0.0f - ((1.0f - fConst4) / fConst8));
		fConst10 = (1.0f / fConst8);
		fConst11 = (1.0f / tanf((31.415926f / fConst0)));
		fConst12 = (fConst11 + 1.0f);
		fConst13 = (0.0f - ((1.0f - fConst11) / fConst12));
		fConst14 = (1.0f / fConst12);
		fConst15 = (0.0f - fConst11);
		fConst16 = (0.0f - fConst4);
		fConst17 = (((fConst4 + -1.0f) / fConst3) + 1.0f);
		fConst18 = (2.0f * (1.0f - fConst7));
		fConst19 = (2.0f * (0.0f - fConst7));
		fConst20 = tanf((3769.91113f / fConst0));
		fConst21 = (1.0f / fConst20);
		fConst22 = (1.0f / (((fConst21 + 1.0f) / fConst20) + 1.0f));
		fConst23 = (1.0f - ((1.0f - fConst21) / fConst20));
		fConst24 = (1.0f / mydsp_faustpower2_f(fConst20));
		fConst25 = (2.0f * (1.0f - fConst24));
		fConst26 = tanf((1884.95557f / fConst0));
		fConst27 = (1.0f / fConst26);
		fConst28 = (1.0f / (((fConst27 + 1.0f) / fConst26) + 1.0f));
		fConst29 = (1.0f - ((1.0f - fConst27) / fConst26));
		fConst30 = (1.0f / mydsp_faustpower2_f(fConst26));
		fConst31 = (2.0f * (1.0f - fConst30));
		fConst32 = tanf((942.477783f / fConst0));
		fConst33 = (1.0f / fConst32);
		fConst34 = (1.0f / (((fConst33 + 1.0f) / fConst32) + 1.0f));
		fConst35 = (1.0f - ((1.0f - fConst33) / fConst32));
		fConst36 = (1.0f / mydsp_faustpower2_f(fConst32));
		fConst37 = (2.0f * (1.0f - fConst36));
		fConst38 = tanf((471.238892f / fConst0));
		fConst39 = (1.0f / fConst38);
		fConst40 = (1.0f / (((fConst39 + 1.0f) / fConst38) + 1.0f));
		fConst41 = (1.0f - ((1.0f - fConst39) / fConst38));
		fConst42 = (1.0f / mydsp_faustpower2_f(fConst38));
		fConst43 = (2.0f * (1.0f - fConst42));
		fConst44 = tanf((204.203522f / fConst0));
		fConst45 = (1.0f / fConst44);
		fConst46 = (1.0f / (((fConst45 + 1.0f) / fConst44) + 1.0f));
		fConst47 = (1.0f - ((1.0f - fConst45) / fConst44));
		fConst48 = (1.0f / mydsp_faustpower2_f(fConst44));
		fConst49 = (2.0f * (1.0f - fConst48));
		iConst50 = int((0.100000001f * fConst0));
		fConst51 = expf((0.0f - (50.0f / fConst0)));
		fConst52 = (((fConst21 + 1.0f) / fConst20) + 1.0f);
		fConst53 = (1.0f / fConst52);
		fConst54 = (fConst21 + 1.0f);
		fConst55 = (0.0f - ((1.0f - fConst21) / fConst54));
		fConst56 = (1.0f / (fConst54 * fConst5));
		fConst57 = (0.0f - fConst21);
		fConst58 = (((fConst21 + -1.0f) / fConst20) + 1.0f);
		fConst59 = (2.0f * (0.0f - fConst24));
		fConst60 = (((fConst27 + 1.0f) / fConst26) + 1.0f);
		fConst61 = (1.0f / fConst60);
		fConst62 = (fConst27 + 1.0f);
		fConst63 = (0.0f - ((1.0f - fConst27) / fConst62));
		fConst64 = (1.0f / (fConst62 * fConst52));
		fConst65 = (0.0f - fConst27);
		fConst66 = (((fConst27 + -1.0f) / fConst26) + 1.0f);
		fConst67 = (2.0f * (0.0f - fConst30));
		fConst68 = (((fConst33 + 1.0f) / fConst32) + 1.0f);
		fConst69 = (1.0f / fConst68);
		fConst70 = (fConst33 + 1.0f);
		fConst71 = (0.0f - ((1.0f - fConst33) / fConst70));
		fConst72 = (1.0f / (fConst70 * fConst60));
		fConst73 = (0.0f - fConst33);
		fConst74 = (((fConst33 + -1.0f) / fConst32) + 1.0f);
		fConst75 = (2.0f * (0.0f - fConst36));
		fConst76 = (((fConst39 + 1.0f) / fConst38) + 1.0f);
		fConst77 = (1.0f / fConst76);
		fConst78 = (fConst39 + 1.0f);
		fConst79 = (0.0f - ((1.0f - fConst39) / fConst78));
		fConst80 = (1.0f / (fConst78 * fConst68));
		fConst81 = (0.0f - fConst39);
		fConst82 = (((fConst39 + -1.0f) / fConst38) + 1.0f);
		fConst83 = (2.0f * (0.0f - fConst42));
		fConst84 = (1.0f / (((fConst45 + 1.0f) / fConst44) + 1.0f));
		fConst85 = (fConst45 + 1.0f);
		fConst86 = (0.0f - ((1.0f - fConst45) / fConst85));
		fConst87 = (1.0f / (fConst85 * fConst76));
		fConst88 = (0.0f - fConst45);
		fConst89 = (((fConst45 + -1.0f) / fConst44) + 1.0f);
		fConst90 = (2.0f * (0.0f - fConst48));
		
	}
	
	virtual void instanceResetUserInterface() {
		fVslider0 = FAUSTFLOAT(-120.0f);
		fVslider1 = FAUSTFLOAT(-120.0f);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fVec0[l0] = 0.0f;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec11[l1] = 0.0f;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fVec1[l2] = 0.0f;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec10[l3] = 0.0f;
			
		}
		for (int l4 = 0; (l4 < 3); l4 = (l4 + 1)) {
			fRec9[l4] = 0.0f;
			
		}
		for (int l5 = 0; (l5 < 3); l5 = (l5 + 1)) {
			fRec8[l5] = 0.0f;
			
		}
		for (int l6 = 0; (l6 < 3); l6 = (l6 + 1)) {
			fRec7[l6] = 0.0f;
			
		}
		for (int l7 = 0; (l7 < 3); l7 = (l7 + 1)) {
			fRec6[l7] = 0.0f;
			
		}
		for (int l8 = 0; (l8 < 3); l8 = (l8 + 1)) {
			fRec5[l8] = 0.0f;
			
		}
		for (int l9 = 0; (l9 < 3); l9 = (l9 + 1)) {
			fRec4[l9] = 0.0f;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec3[l10] = 0.0f;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			iVec2[l11] = 0;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			iRec12[l12] = 0;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec1[l13] = 0.0f;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec0[l14] = 0.0f;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec24[l15] = 0.0f;
			
		}
		for (int l16 = 0; (l16 < 3); l16 = (l16 + 1)) {
			fRec23[l16] = 0.0f;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fVec3[l17] = 0.0f;
			
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec22[l18] = 0.0f;
			
		}
		for (int l19 = 0; (l19 < 3); l19 = (l19 + 1)) {
			fRec21[l19] = 0.0f;
			
		}
		for (int l20 = 0; (l20 < 3); l20 = (l20 + 1)) {
			fRec20[l20] = 0.0f;
			
		}
		for (int l21 = 0; (l21 < 3); l21 = (l21 + 1)) {
			fRec19[l21] = 0.0f;
			
		}
		for (int l22 = 0; (l22 < 3); l22 = (l22 + 1)) {
			fRec18[l22] = 0.0f;
			
		}
		for (int l23 = 0; (l23 < 3); l23 = (l23 + 1)) {
			fRec17[l23] = 0.0f;
			
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec16[l24] = 0.0f;
			
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			iVec4[l25] = 0;
			
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			iRec25[l26] = 0;
			
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec14[l27] = 0.0f;
			
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec13[l28] = 0.0f;
			
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec36[l29] = 0.0f;
			
		}
		for (int l30 = 0; (l30 < 3); l30 = (l30 + 1)) {
			fRec35[l30] = 0.0f;
			
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fVec5[l31] = 0.0f;
			
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec34[l32] = 0.0f;
			
		}
		for (int l33 = 0; (l33 < 3); l33 = (l33 + 1)) {
			fRec33[l33] = 0.0f;
			
		}
		for (int l34 = 0; (l34 < 3); l34 = (l34 + 1)) {
			fRec32[l34] = 0.0f;
			
		}
		for (int l35 = 0; (l35 < 3); l35 = (l35 + 1)) {
			fRec31[l35] = 0.0f;
			
		}
		for (int l36 = 0; (l36 < 3); l36 = (l36 + 1)) {
			fRec30[l36] = 0.0f;
			
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec29[l37] = 0.0f;
			
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			iVec6[l38] = 0;
			
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			iRec37[l39] = 0;
			
		}
		for (int l40 = 0; (l40 < 2); l40 = (l40 + 1)) {
			fRec27[l40] = 0.0f;
			
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec26[l41] = 0.0f;
			
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec47[l42] = 0.0f;
			
		}
		for (int l43 = 0; (l43 < 3); l43 = (l43 + 1)) {
			fRec46[l43] = 0.0f;
			
		}
		for (int l44 = 0; (l44 < 2); l44 = (l44 + 1)) {
			fVec7[l44] = 0.0f;
			
		}
		for (int l45 = 0; (l45 < 2); l45 = (l45 + 1)) {
			fRec45[l45] = 0.0f;
			
		}
		for (int l46 = 0; (l46 < 3); l46 = (l46 + 1)) {
			fRec44[l46] = 0.0f;
			
		}
		for (int l47 = 0; (l47 < 3); l47 = (l47 + 1)) {
			fRec43[l47] = 0.0f;
			
		}
		for (int l48 = 0; (l48 < 3); l48 = (l48 + 1)) {
			fRec42[l48] = 0.0f;
			
		}
		for (int l49 = 0; (l49 < 2); l49 = (l49 + 1)) {
			fRec41[l49] = 0.0f;
			
		}
		for (int l50 = 0; (l50 < 2); l50 = (l50 + 1)) {
			iVec8[l50] = 0;
			
		}
		for (int l51 = 0; (l51 < 2); l51 = (l51 + 1)) {
			iRec48[l51] = 0;
			
		}
		for (int l52 = 0; (l52 < 2); l52 = (l52 + 1)) {
			fRec39[l52] = 0.0f;
			
		}
		for (int l53 = 0; (l53 < 2); l53 = (l53 + 1)) {
			fRec38[l53] = 0.0f;
			
		}
		for (int l54 = 0; (l54 < 2); l54 = (l54 + 1)) {
			fRec57[l54] = 0.0f;
			
		}
		for (int l55 = 0; (l55 < 3); l55 = (l55 + 1)) {
			fRec56[l55] = 0.0f;
			
		}
		for (int l56 = 0; (l56 < 2); l56 = (l56 + 1)) {
			fVec9[l56] = 0.0f;
			
		}
		for (int l57 = 0; (l57 < 2); l57 = (l57 + 1)) {
			fRec55[l57] = 0.0f;
			
		}
		for (int l58 = 0; (l58 < 3); l58 = (l58 + 1)) {
			fRec54[l58] = 0.0f;
			
		}
		for (int l59 = 0; (l59 < 3); l59 = (l59 + 1)) {
			fRec53[l59] = 0.0f;
			
		}
		for (int l60 = 0; (l60 < 2); l60 = (l60 + 1)) {
			fRec52[l60] = 0.0f;
			
		}
		for (int l61 = 0; (l61 < 2); l61 = (l61 + 1)) {
			iVec10[l61] = 0;
			
		}
		for (int l62 = 0; (l62 < 2); l62 = (l62 + 1)) {
			iRec58[l62] = 0;
			
		}
		for (int l63 = 0; (l63 < 2); l63 = (l63 + 1)) {
			fRec50[l63] = 0.0f;
			
		}
		for (int l64 = 0; (l64 < 2); l64 = (l64 + 1)) {
			fRec49[l64] = 0.0f;
			
		}
		for (int l65 = 0; (l65 < 2); l65 = (l65 + 1)) {
			fRec66[l65] = 0.0f;
			
		}
		for (int l66 = 0; (l66 < 3); l66 = (l66 + 1)) {
			fRec65[l66] = 0.0f;
			
		}
		for (int l67 = 0; (l67 < 2); l67 = (l67 + 1)) {
			fVec11[l67] = 0.0f;
			
		}
		for (int l68 = 0; (l68 < 2); l68 = (l68 + 1)) {
			fRec64[l68] = 0.0f;
			
		}
		for (int l69 = 0; (l69 < 3); l69 = (l69 + 1)) {
			fRec63[l69] = 0.0f;
			
		}
		for (int l70 = 0; (l70 < 2); l70 = (l70 + 1)) {
			fRec62[l70] = 0.0f;
			
		}
		for (int l71 = 0; (l71 < 2); l71 = (l71 + 1)) {
			iVec12[l71] = 0;
			
		}
		for (int l72 = 0; (l72 < 2); l72 = (l72 + 1)) {
			iRec67[l72] = 0;
			
		}
		for (int l73 = 0; (l73 < 2); l73 = (l73 + 1)) {
			fRec60[l73] = 0.0f;
			
		}
		for (int l74 = 0; (l74 < 2); l74 = (l74 + 1)) {
			fRec59[l74] = 0.0f;
			
		}
		for (int l75 = 0; (l75 < 2); l75 = (l75 + 1)) {
			fRec73[l75] = 0.0f;
			
		}
		for (int l76 = 0; (l76 < 3); l76 = (l76 + 1)) {
			fRec72[l76] = 0.0f;
			
		}
		for (int l77 = 0; (l77 < 2); l77 = (l77 + 1)) {
			fRec71[l77] = 0.0f;
			
		}
		for (int l78 = 0; (l78 < 2); l78 = (l78 + 1)) {
			iVec13[l78] = 0;
			
		}
		for (int l79 = 0; (l79 < 2); l79 = (l79 + 1)) {
			iRec74[l79] = 0;
			
		}
		for (int l80 = 0; (l80 < 2); l80 = (l80 + 1)) {
			fRec69[l80] = 0.0f;
			
		}
		for (int l81 = 0; (l81 < 2); l81 = (l81 + 1)) {
			fRec68[l81] = 0.0f;
			
		}
		
	}
	
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void instanceInit(int samplingFreq) {
		instanceConstants(samplingFreq);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	virtual int getSampleRate() {
		return fSamplingFreq;
		
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("kpp_distruction");
		ui_interface->addVerticalSlider("Dead Zone", &fVslider0, -120.0f, -120.0f, 0.0f, 0.00100000005f);
		ui_interface->addVerticalSlider("Noise Gate", &fVslider1, -120.0f, -120.0f, 0.0f, 0.00100000005f);
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = powf(10.0f, (0.0500000007f * float(fVslider0)));
		float fSlow1 = (0.0f - fSlow0);
		float fSlow2 = powf(10.0f, (0.0500000007f * float(fVslider1)));
		for (int i = 0; (i < count); i = (i + 1)) {
			float fTemp0 = float(input0[i]);
			fVec0[0] = fTemp0;
			fRec11[0] = ((fConst13 * fRec11[1]) + (fConst14 * ((fConst11 * fTemp0) + (fConst15 * fVec0[1]))));
			float fTemp1 = (max(fSlow0, fRec11[0]) + min(fSlow1, fRec11[0]));
			fVec1[0] = fTemp1;
			fRec10[0] = ((fConst9 * fRec10[1]) + (fConst10 * ((fConst4 * fTemp1) + (fConst16 * fVec1[1]))));
			fRec9[0] = (fRec10[0] - (fConst6 * ((fConst17 * fRec9[2]) + (fConst18 * fRec9[1]))));
			float fTemp2 = (fConst25 * fRec8[1]);
			fRec8[0] = ((fConst6 * (((fConst7 * fRec9[0]) + (fConst19 * fRec9[1])) + (fConst7 * fRec9[2]))) - (fConst22 * ((fConst23 * fRec8[2]) + fTemp2)));
			float fTemp3 = (fConst31 * fRec7[1]);
			fRec7[0] = ((fRec8[2] + (fConst22 * (fTemp2 + (fConst23 * fRec8[0])))) - (fConst28 * ((fConst29 * fRec7[2]) + fTemp3)));
			float fTemp4 = (fConst37 * fRec6[1]);
			fRec6[0] = ((fRec7[2] + (fConst28 * (fTemp3 + (fConst29 * fRec7[0])))) - (fConst34 * ((fConst35 * fRec6[2]) + fTemp4)));
			float fTemp5 = (fConst43 * fRec5[1]);
			fRec5[0] = ((fRec6[2] + (fConst34 * (fTemp4 + (fConst35 * fRec6[0])))) - (fConst40 * ((fConst41 * fRec5[2]) + fTemp5)));
			float fTemp6 = (fConst49 * fRec4[1]);
			fRec4[0] = ((fRec5[2] + (fConst40 * (fTemp5 + (fConst41 * fRec5[0])))) - (fConst46 * ((fConst47 * fRec4[2]) + fTemp6)));
			float fTemp7 = (fRec4[2] + (fConst46 * (fTemp6 + (fConst47 * fRec4[0]))));
			fRec3[0] = ((fConst1 * fRec3[1]) + (fConst2 * fabsf(fTemp7)));
			float fRec2 = fRec3[0];
			int iTemp8 = (fRec2 > fSlow2);
			iVec2[0] = iTemp8;
			iRec12[0] = max(int((iConst50 * (iTemp8 < iVec2[1]))), int((iRec12[1] + -1)));
			float fTemp9 = fabsf(max(float(iTemp8), float((iRec12[0] > 0))));
			float fTemp10 = ((fRec0[1] > fTemp9)?fConst51:fConst1);
			fRec1[0] = ((fRec1[1] * fTemp10) + (fTemp9 * (1.0f - fTemp10)));
			fRec0[0] = fRec1[0];
			fRec24[0] = ((fConst9 * fRec24[1]) + (fConst10 * (fTemp1 + fVec1[1])));
			fRec23[0] = (fRec24[0] - (fConst6 * ((fConst17 * fRec23[2]) + (fConst18 * fRec23[1]))));
			float fTemp11 = (fRec23[2] + (fRec23[0] + (2.0f * fRec23[1])));
			fVec3[0] = fTemp11;
			fRec22[0] = ((fConst55 * fRec22[1]) + (fConst56 * ((fConst21 * fTemp11) + (fConst57 * fVec3[1]))));
			fRec21[0] = (fRec22[0] - (fConst53 * ((fConst58 * fRec21[2]) + (fConst25 * fRec21[1]))));
			float fTemp12 = (fConst31 * fRec20[1]);
			fRec20[0] = ((fConst53 * (((fConst24 * fRec21[0]) + (fConst59 * fRec21[1])) + (fConst24 * fRec21[2]))) - (fConst28 * ((fConst29 * fRec20[2]) + fTemp12)));
			float fTemp13 = (fConst37 * fRec19[1]);
			fRec19[0] = ((fRec20[2] + (fConst28 * (fTemp12 + (fConst29 * fRec20[0])))) - (fConst34 * ((fConst35 * fRec19[2]) + fTemp13)));
			float fTemp14 = (fConst43 * fRec18[1]);
			fRec18[0] = ((fRec19[2] + (fConst34 * (fTemp13 + (fConst35 * fRec19[0])))) - (fConst40 * ((fConst41 * fRec18[2]) + fTemp14)));
			float fTemp15 = (fConst49 * fRec17[1]);
			fRec17[0] = ((fRec18[2] + (fConst40 * (fTemp14 + (fConst41 * fRec18[0])))) - (fConst46 * ((fConst47 * fRec17[2]) + fTemp15)));
			float fTemp16 = (fRec17[2] + (fConst46 * (fTemp15 + (fConst47 * fRec17[0]))));
			fRec16[0] = ((fConst1 * fRec16[1]) + (fConst2 * fabsf(fTemp16)));
			float fRec15 = fRec16[0];
			int iTemp17 = (fRec15 > fSlow2);
			iVec4[0] = iTemp17;
			iRec25[0] = max(int((iConst50 * (iTemp17 < iVec4[1]))), int((iRec25[1] + -1)));
			float fTemp18 = fabsf(max(float(iTemp17), float((iRec25[0] > 0))));
			float fTemp19 = ((fRec13[1] > fTemp18)?fConst51:fConst1);
			fRec14[0] = ((fRec14[1] * fTemp19) + (fTemp18 * (1.0f - fTemp19)));
			fRec13[0] = fRec14[0];
			fRec36[0] = ((fConst55 * fRec36[1]) + (fConst56 * (fTemp11 + fVec3[1])));
			fRec35[0] = (fRec36[0] - (fConst53 * ((fConst58 * fRec35[2]) + (fConst25 * fRec35[1]))));
			float fTemp20 = (fRec35[2] + (fRec35[0] + (2.0f * fRec35[1])));
			fVec5[0] = fTemp20;
			fRec34[0] = ((fConst63 * fRec34[1]) + (fConst64 * ((fConst27 * fTemp20) + (fConst65 * fVec5[1]))));
			fRec33[0] = (fRec34[0] - (fConst61 * ((fConst66 * fRec33[2]) + (fConst31 * fRec33[1]))));
			float fTemp21 = (fConst37 * fRec32[1]);
			fRec32[0] = ((fConst61 * (((fConst30 * fRec33[0]) + (fConst67 * fRec33[1])) + (fConst30 * fRec33[2]))) - (fConst34 * ((fConst35 * fRec32[2]) + fTemp21)));
			float fTemp22 = (fConst43 * fRec31[1]);
			fRec31[0] = ((fRec32[2] + (fConst34 * (fTemp21 + (fConst35 * fRec32[0])))) - (fConst40 * ((fConst41 * fRec31[2]) + fTemp22)));
			float fTemp23 = (fConst49 * fRec30[1]);
			fRec30[0] = ((fRec31[2] + (fConst40 * (fTemp22 + (fConst41 * fRec31[0])))) - (fConst46 * ((fConst47 * fRec30[2]) + fTemp23)));
			float fTemp24 = (fRec30[2] + (fConst46 * (fTemp23 + (fConst47 * fRec30[0]))));
			fRec29[0] = ((fConst1 * fRec29[1]) + (fConst2 * fabsf(fTemp24)));
			float fRec28 = fRec29[0];
			int iTemp25 = (fRec28 > fSlow2);
			iVec6[0] = iTemp25;
			iRec37[0] = max(int((iConst50 * (iTemp25 < iVec6[1]))), int((iRec37[1] + -1)));
			float fTemp26 = fabsf(max(float(iTemp25), float((iRec37[0] > 0))));
			float fTemp27 = ((fRec26[1] > fTemp26)?fConst51:fConst1);
			fRec27[0] = ((fRec27[1] * fTemp27) + (fTemp26 * (1.0f - fTemp27)));
			fRec26[0] = fRec27[0];
			fRec47[0] = ((fConst63 * fRec47[1]) + (fConst64 * (fTemp20 + fVec5[1])));
			fRec46[0] = (fRec47[0] - (fConst61 * ((fConst66 * fRec46[2]) + (fConst31 * fRec46[1]))));
			float fTemp28 = (fRec46[2] + (fRec46[0] + (2.0f * fRec46[1])));
			fVec7[0] = fTemp28;
			fRec45[0] = ((fConst71 * fRec45[1]) + (fConst72 * ((fConst33 * fTemp28) + (fConst73 * fVec7[1]))));
			fRec44[0] = (fRec45[0] - (fConst69 * ((fConst74 * fRec44[2]) + (fConst37 * fRec44[1]))));
			float fTemp29 = (fConst43 * fRec43[1]);
			fRec43[0] = ((fConst69 * (((fConst36 * fRec44[0]) + (fConst75 * fRec44[1])) + (fConst36 * fRec44[2]))) - (fConst40 * ((fConst41 * fRec43[2]) + fTemp29)));
			float fTemp30 = (fConst49 * fRec42[1]);
			fRec42[0] = ((fRec43[2] + (fConst40 * (fTemp29 + (fConst41 * fRec43[0])))) - (fConst46 * ((fConst47 * fRec42[2]) + fTemp30)));
			float fTemp31 = (fRec42[2] + (fConst46 * (fTemp30 + (fConst47 * fRec42[0]))));
			fRec41[0] = ((fConst1 * fRec41[1]) + (fConst2 * fabsf(fTemp31)));
			float fRec40 = fRec41[0];
			int iTemp32 = (fRec40 > fSlow2);
			iVec8[0] = iTemp32;
			iRec48[0] = max(int((iConst50 * (iTemp32 < iVec8[1]))), int((iRec48[1] + -1)));
			float fTemp33 = fabsf(max(float(iTemp32), float((iRec48[0] > 0))));
			float fTemp34 = ((fRec38[1] > fTemp33)?fConst51:fConst1);
			fRec39[0] = ((fRec39[1] * fTemp34) + (fTemp33 * (1.0f - fTemp34)));
			fRec38[0] = fRec39[0];
			fRec57[0] = ((fConst71 * fRec57[1]) + (fConst72 * (fTemp28 + fVec7[1])));
			fRec56[0] = (fRec57[0] - (fConst69 * ((fConst74 * fRec56[2]) + (fConst37 * fRec56[1]))));
			float fTemp35 = (fRec56[2] + (fRec56[0] + (2.0f * fRec56[1])));
			fVec9[0] = fTemp35;
			fRec55[0] = ((fConst79 * fRec55[1]) + (fConst80 * ((fConst39 * fTemp35) + (fConst81 * fVec9[1]))));
			fRec54[0] = (fRec55[0] - (fConst77 * ((fConst82 * fRec54[2]) + (fConst43 * fRec54[1]))));
			float fTemp36 = (fConst49 * fRec53[1]);
			fRec53[0] = ((fConst77 * (((fConst42 * fRec54[0]) + (fConst83 * fRec54[1])) + (fConst42 * fRec54[2]))) - (fConst46 * ((fConst47 * fRec53[2]) + fTemp36)));
			float fTemp37 = (fRec53[2] + (fConst46 * (fTemp36 + (fConst47 * fRec53[0]))));
			fRec52[0] = ((fConst1 * fRec52[1]) + (fConst2 * fabsf(fTemp37)));
			float fRec51 = fRec52[0];
			int iTemp38 = (fRec51 > fSlow2);
			iVec10[0] = iTemp38;
			iRec58[0] = max(int((iConst50 * (iTemp38 < iVec10[1]))), int((iRec58[1] + -1)));
			float fTemp39 = fabsf(max(float(iTemp38), float((iRec58[0] > 0))));
			float fTemp40 = ((fRec49[1] > fTemp39)?fConst51:fConst1);
			fRec50[0] = ((fRec50[1] * fTemp40) + (fTemp39 * (1.0f - fTemp40)));
			fRec49[0] = fRec50[0];
			fRec66[0] = ((fConst79 * fRec66[1]) + (fConst80 * (fTemp35 + fVec9[1])));
			fRec65[0] = (fRec66[0] - (fConst77 * ((fConst82 * fRec65[2]) + (fConst43 * fRec65[1]))));
			float fTemp41 = (fRec65[2] + (fRec65[0] + (2.0f * fRec65[1])));
			fVec11[0] = fTemp41;
			fRec64[0] = ((fConst86 * fRec64[1]) + (fConst87 * ((fConst45 * fTemp41) + (fConst88 * fVec11[1]))));
			fRec63[0] = (fRec64[0] - (fConst84 * ((fConst89 * fRec63[2]) + (fConst49 * fRec63[1]))));
			float fTemp42 = (((fConst48 * fRec63[0]) + (fConst90 * fRec63[1])) + (fConst48 * fRec63[2]));
			fRec62[0] = ((fConst1 * fRec62[1]) + (fConst2 * fabsf((fConst84 * fTemp42))));
			float fRec61 = fRec62[0];
			int iTemp43 = (fRec61 > fSlow2);
			iVec12[0] = iTemp43;
			iRec67[0] = max(int((iConst50 * (iTemp43 < iVec12[1]))), int((iRec67[1] + -1)));
			float fTemp44 = fabsf(max(float(iTemp43), float((iRec67[0] > 0))));
			float fTemp45 = ((fRec59[1] > fTemp44)?fConst51:fConst1);
			fRec60[0] = ((fRec60[1] * fTemp45) + (fTemp44 * (1.0f - fTemp45)));
			fRec59[0] = fRec60[0];
			fRec73[0] = ((fConst86 * fRec73[1]) + (fConst87 * (fTemp41 + fVec11[1])));
			fRec72[0] = (fRec73[0] - (fConst84 * ((fConst89 * fRec72[2]) + (fConst49 * fRec72[1]))));
			float fTemp46 = (fRec72[2] + (fRec72[0] + (2.0f * fRec72[1])));
			fRec71[0] = ((fConst1 * fRec71[1]) + (fConst2 * fabsf((fConst84 * fTemp46))));
			float fRec70 = fRec71[0];
			int iTemp47 = (fRec70 > fSlow2);
			iVec13[0] = iTemp47;
			iRec74[0] = max(int((iConst50 * (iTemp47 < iVec13[1]))), int((iRec74[1] + -1)));
			float fTemp48 = fabsf(max(float(iTemp47), float((iRec74[0] > 0))));
			float fTemp49 = ((fRec68[1] > fTemp48)?fConst51:fConst1);
			fRec69[0] = ((fRec69[1] * fTemp49) + (fTemp48 * (1.0f - fTemp49)));
			fRec68[0] = fRec69[0];
			output0[i] = FAUSTFLOAT(((((((fRec0[0] * fTemp7) + (fRec13[0] * fTemp16)) + (fRec26[0] * fTemp24)) + (fRec38[0] * fTemp31)) + (fRec49[0] * fTemp37)) + (fConst84 * ((fRec59[0] * fTemp42) + (fRec68[0] * fTemp46)))));
			fVec0[1] = fVec0[0];
			fRec11[1] = fRec11[0];
			fVec1[1] = fVec1[0];
			fRec10[1] = fRec10[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec7[2] = fRec7[1];
			fRec7[1] = fRec7[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
			iVec2[1] = iVec2[0];
			iRec12[1] = iRec12[0];
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec24[1] = fRec24[0];
			fRec23[2] = fRec23[1];
			fRec23[1] = fRec23[0];
			fVec3[1] = fVec3[0];
			fRec22[1] = fRec22[0];
			fRec21[2] = fRec21[1];
			fRec21[1] = fRec21[0];
			fRec20[2] = fRec20[1];
			fRec20[1] = fRec20[0];
			fRec19[2] = fRec19[1];
			fRec19[1] = fRec19[0];
			fRec18[2] = fRec18[1];
			fRec18[1] = fRec18[0];
			fRec17[2] = fRec17[1];
			fRec17[1] = fRec17[0];
			fRec16[1] = fRec16[0];
			iVec4[1] = iVec4[0];
			iRec25[1] = iRec25[0];
			fRec14[1] = fRec14[0];
			fRec13[1] = fRec13[0];
			fRec36[1] = fRec36[0];
			fRec35[2] = fRec35[1];
			fRec35[1] = fRec35[0];
			fVec5[1] = fVec5[0];
			fRec34[1] = fRec34[0];
			fRec33[2] = fRec33[1];
			fRec33[1] = fRec33[0];
			fRec32[2] = fRec32[1];
			fRec32[1] = fRec32[0];
			fRec31[2] = fRec31[1];
			fRec31[1] = fRec31[0];
			fRec30[2] = fRec30[1];
			fRec30[1] = fRec30[0];
			fRec29[1] = fRec29[0];
			iVec6[1] = iVec6[0];
			iRec37[1] = iRec37[0];
			fRec27[1] = fRec27[0];
			fRec26[1] = fRec26[0];
			fRec47[1] = fRec47[0];
			fRec46[2] = fRec46[1];
			fRec46[1] = fRec46[0];
			fVec7[1] = fVec7[0];
			fRec45[1] = fRec45[0];
			fRec44[2] = fRec44[1];
			fRec44[1] = fRec44[0];
			fRec43[2] = fRec43[1];
			fRec43[1] = fRec43[0];
			fRec42[2] = fRec42[1];
			fRec42[1] = fRec42[0];
			fRec41[1] = fRec41[0];
			iVec8[1] = iVec8[0];
			iRec48[1] = iRec48[0];
			fRec39[1] = fRec39[0];
			fRec38[1] = fRec38[0];
			fRec57[1] = fRec57[0];
			fRec56[2] = fRec56[1];
			fRec56[1] = fRec56[0];
			fVec9[1] = fVec9[0];
			fRec55[1] = fRec55[0];
			fRec54[2] = fRec54[1];
			fRec54[1] = fRec54[0];
			fRec53[2] = fRec53[1];
			fRec53[1] = fRec53[0];
			fRec52[1] = fRec52[0];
			iVec10[1] = iVec10[0];
			iRec58[1] = iRec58[0];
			fRec50[1] = fRec50[0];
			fRec49[1] = fRec49[0];
			fRec66[1] = fRec66[0];
			fRec65[2] = fRec65[1];
			fRec65[1] = fRec65[0];
			fVec11[1] = fVec11[0];
			fRec64[1] = fRec64[0];
			fRec63[2] = fRec63[1];
			fRec63[1] = fRec63[0];
			fRec62[1] = fRec62[0];
			iVec12[1] = iVec12[0];
			iRec67[1] = iRec67[0];
			fRec60[1] = fRec60[0];
			fRec59[1] = fRec59[0];
			fRec73[1] = fRec73[0];
			fRec72[2] = fRec72[1];
			fRec72[1] = fRec72[0];
			fRec71[1] = fRec71[0];
			iVec13[1] = iVec13[0];
			iRec74[1] = iRec74[0];
			fRec69[1] = fRec69[0];
			fRec68[1] = fRec68[0];
			
		}
		
	}

	
};

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/

//-----------------------------------portCollector--------------------------------------
//
// portCollector is passed to the buildUserInterface method of a dsp object
// in order to build a description of its inputs, outputs and control ports.
// This description is used to fill a LADSPA_Descriptor
//
//--------------------------------------------------------------------------------------

//--------------------------------useful constants--------------------------------------

#define MAXPORT 1024
static const int ICONTROL 	= LADSPA_PORT_INPUT|LADSPA_PORT_CONTROL;
static const int OCONTROL 	= LADSPA_PORT_OUTPUT|LADSPA_PORT_CONTROL;
static const int RANGE 		= LADSPA_PORT_INPUT|LADSPA_PORT_CONTROL;

static const char* inames[] = {
					"input00", "input01", "input02", "input03", "input04",
					"input05", "input06", "input07", "input08", "input09",
					"input10", "input11", "input12", "input13", "input14",
					"input15", "input16", "input17", "input18", "input19",
					"input20", "input21", "input22", "input23", "input24",
					"input25", "input26", "input27", "input28", "input29",
					"input30", "input31", "input32", "input33", "input34",
					"input35", "input36", "input37", "input38", "input39"
};

static const char* onames[] = {
					"output00", "output01", "output02", "output03", "output04",
					"output05", "output06", "output07", "output08", "output09",
					"output10", "output11", "output12", "output13", "output14",
					"output15", "output16", "output17", "output18", "output19",
					"output20", "output21", "output22", "output23", "output24",
					"output25", "output26", "output27", "output28", "output29",
					"output30", "output31", "output32", "output33", "output34",
					"output35", "output36", "output37", "output38", "output39"
};

class portCollector : public UI
{
 private:

	//--------------------------------------------------------------------------------------

	const int				fInsCount;					// number of audio input ports
	const int				fOutsCount;					// number of audio output ports
	int						fCtrlCount;					// number of control ports

	LADSPA_PortDescriptor 	fPortDescs[MAXPORT];		// table of port descriptors to be used in a LADSPA_Descriptor
	const char* 			fPortNames[MAXPORT];		// table of port names to be used in a LADSPA_Descriptor
	LADSPA_PortRangeHint 	fPortHints[MAXPORT];		// table of port hints to be used in a LADSPA_Descriptor

    std::string					fPluginName;				// toplevel prefix used as plugin name
    std::stack<std::string>			fPrefix;					// current prefix for controls name


	//--------------------------------------------------------------------------------------
    std::string simplify(const std::string& src)
	{
		int		i=0;
		int		level=2;
        std::string	dst;

		while (src[i] ) {

			switch (level) {

				case 0 :
				case 1 :
				case 2 :
					// Skip the begin of the label "--foo-"
					// until 3 '-' have been read
					if (src[i]=='-') { level++; }
					break;

				case 3 :
					// copy the content, but skip non alphnum
					// and content in parenthesis
					switch (src[i]) {
						case '(' :
						case '[' :
							level++;
							break;

						case '-' :
							dst += '-';
							break;

						default :
							if (isalnum(src[i])) {
								dst+= tolower(src[i]);
							}

					}
					break;

				default :
					// here we are inside parenthesis and
					// we skip the content until we are back to
					// level 3
					switch (src[i]) {

						case '(' :
						case '[' :
							level++;
							break;

						case ')' :
						case ']' :
							level--;
							break;

						default :
							break;
					}

			}
			i++;
		}
		return (dst.size() > 0) ? dst :src;
	}

	void addPortDescr(int type, const char* label, int hint, float min=0.0, float max=0.0)
	{
        std::string fullname = simplify(fPrefix.top() + "-" + label);
		char * str = strdup(fullname.c_str());

		fPortDescs[fInsCount + fOutsCount + fCtrlCount] = type;
		fPortNames[fInsCount + fOutsCount + fCtrlCount] = str;
		fPortHints[fInsCount + fOutsCount + fCtrlCount].HintDescriptor = hint;
		fPortHints[fInsCount + fOutsCount + fCtrlCount].LowerBound = min;
		fPortHints[fInsCount + fOutsCount + fCtrlCount].UpperBound = max;
		fCtrlCount++;
	}

	void openAnyBox(const char* label)
	{
		if (fPrefix.size() == 0) {
			// top level label is used as plugin name
			fPluginName = label;
			fPrefix.push(label);

		} else {
            std::string s;
			if (label && label[0]) {
				s = fPrefix.top() + "-" + label;
			} else {
				s = fPrefix.top();
			}
			fPrefix.push(s);
		}
	}

 public:

	//--------------------------------Collect the audio ports-------------------------------

	portCollector(int ins, int outs) : UI(), fInsCount(ins), fOutsCount(outs), fCtrlCount(0)
	{
		for (int i = 0; i < ins; i++) {
			fPortDescs[i] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
			fPortNames[i] = inames[i];
			fPortHints[i].HintDescriptor = 0;
		}
		for (int j = 0; j < outs; j++) {
			fPortDescs[ins + j] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
			fPortNames[ins + j] = onames[j];
			fPortHints[ins + j].HintDescriptor = 0;
		}
	};

	virtual ~portCollector() {}

	//------------------------------Collect the control ports-------------------------------

	virtual void addButton(const char* label, float* zone) {
		addPortDescr(ICONTROL, label, LADSPA_HINT_TOGGLED);
	}

	virtual void addToggleButton(const char* label, float* zone) {
		addPortDescr(ICONTROL, label, LADSPA_HINT_TOGGLED);
	}

	virtual void addCheckButton(const char* label, float* zone) {
		addPortDescr(ICONTROL, label, LADSPA_HINT_TOGGLED);
	}

	virtual void addVerticalSlider(const char* label, float* zone, float init, float min, float max, float step) {
		addPortDescr(ICONTROL, label, LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, min, max);
	}

	virtual void addHorizontalSlider(const char* label, float* zone, float init, float min, float max, float step) {
		addPortDescr(ICONTROL, label, LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, min, max);
	}

	virtual void addNumEntry(const char* label, float* zone, float init, float min, float max, float step) {
		addPortDescr(ICONTROL, label, LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, min, max);
	}

	// -- passive widgets

	virtual void addNumDisplay(const char* label, float* zone, int precision) {
		addPortDescr(OCONTROL, label, 0, -10000, +10000);
	}
	virtual void addTextDisplay(const char* label, float* zone, const char* names[], float min, float max) {
		addPortDescr(OCONTROL, label, LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, min, max);
	}
    
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
    
	virtual void addHorizontalBargraph(const char* label, float* zone, float min, float max) {
		addPortDescr(OCONTROL, label, LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, min, max);
	}
	virtual void addVerticalBargraph(const char* label, float* zone, float min, float max){
		addPortDescr(OCONTROL, label, LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE, min, max);
	}
    
	virtual void openFrameBox(const char* label)		{ openAnyBox(label); }
	virtual void openTabBox(const char* label)		{ openAnyBox(label); }
	virtual void openHorizontalBox(const char* label)	{ openAnyBox(label); }
	virtual void openVerticalBox(const char* label)	{ openAnyBox(label); }

	virtual void closeBox() 					{ fPrefix.pop(); }

	virtual void show() {}
	virtual void run() 	{}

	//---------------------------------Fill the LADSPA descriptor---------------------------

	// generate an ID from a plugin name
	int makeID (const char* s) {
		int h = 0;
		for (int i = 0; s[i]; i++) {
			h = (h << 3) + (s[i] & 7);
		}
		return 1+h%1000;
	}

	// fill a ladspa descriptor with the information collected on ports
	void fillPortDescription (LADSPA_Descriptor * descriptor) {
		const char* name = sym(mydsp);
		descriptor->PortCount 			= fCtrlCount+fInsCount+fOutsCount;
		descriptor->PortDescriptors 	= fPortDescs;
		descriptor->PortNames 			= fPortNames;
		descriptor->PortRangeHints 		= fPortHints;

		descriptor->Label = strdup(name);
		descriptor->UniqueID = makeID(name);
//		descriptor->Label = strdup(fPluginName.c_str());
//		descriptor->UniqueID = makeID(fPluginName.c_str());
		descriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
		descriptor->Name = name;
//		descriptor->Name = strdup(fPluginName.c_str());
		descriptor->Maker = "Oleg Kapitonov";
		descriptor->Copyright = "GPLv3";
	}
};

//--------------------------------------portData----------------------------------------
//
// portData : a user interface used to associate the data buffers and the ports
//
//--------------------------------------------------------------------------------------

class portData : public UI
{

 private:

	//--------------------------------------------------------------------------------------

	const int				fInsCount;					// number of audio input ports
	const int				fOutsCount;					// number of audio output ports
	int						fCtrlCount;					// number of control ports

	float* 					fPortZone[MAXPORT];			//
	float* 					fPortData[MAXPORT];

	//--------------------------------------------------------------------------------------

	void addZone(float* zone)
	{
		fPortZone[fInsCount + fOutsCount + fCtrlCount] = zone;
		fCtrlCount++;
	}

 public:

	//--------------------------------Collect the audio ports-------------------------------

	portData(int ins, int outs) : UI(), fInsCount(ins), fOutsCount(outs), fCtrlCount(0) {};
	virtual ~portData() {}

	//------------------------------Collect the control zones-------------------------------

	virtual void addButton(const char* label, float* zone) 			{ addZone(zone); }
	virtual void addToggleButton(const char* label, float* zone)  	{ addZone(zone); }
	virtual void addCheckButton(const char* label, float* zone)  		{ addZone(zone); }

	virtual void addVerticalSlider(const char* label, float* zone, float init, float min, float max, float step) 		{ addZone(zone); }
	virtual void addHorizontalSlider(const char* label, float* zone, float init, float min, float max, float step) 	{ addZone(zone); }
	virtual void addNumEntry(const char* label, float* zone, float init, float min, float max, float step)  			{ addZone(zone); }

	// -- passive widgets

	virtual void addNumDisplay(const char* label, float* zone, int precision) 						{ addZone(zone); }
	virtual void addTextDisplay(const char* label, float* zone, const char* names[], float min, float max) 	{ addZone(zone); }
	virtual void addHorizontalBargraph(const char* label, float* zone, float min, float max) 			{ addZone(zone); }
	virtual void addVerticalBargraph(const char* label, float* zone, float min, float max)			{ addZone(zone); }

    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}

	virtual void openFrameBox(const char* label)		{ }
	virtual void openTabBox(const char* label)		{ }
	virtual void openHorizontalBox(const char* label)	{ }
	virtual void openVerticalBox(const char* label)	{ }
	virtual void closeBox() 					{ }

	virtual void show() {}
	virtual void run() 	{}

	//---------------------------------interaction with LADSPA------------------------------

	void setPortData (unsigned long port, LADSPA_Data* data) {
		fPortData[port] = data;
	}

	void updateCtrlZones() {
		for (int i = fInsCount+fOutsCount; i < fInsCount+fOutsCount+fCtrlCount; i++)	*fPortZone[i] = *fPortData[i];
	}

	float** getInputs() {
		return &fPortData[0];
	}

	float** getOutputs() {
		return &fPortData[fInsCount];
	}
};

//--------------------------------Faust-LADSPA plugin-----------------------------------
//
// Plugin structure, callbacks and LADSPA_descriptor(i) entry point
//
//--------------------------------------------------------------------------------------

LADSPA_Descriptor* 	gDescriptor = 0;

struct PLUGIN
{
	unsigned long	fSampleRate;
	portData*		fPortData;
	dsp*			fDsp;

	PLUGIN(unsigned long r, portData* d, dsp* p) : fSampleRate(r), fPortData(d), fDsp(p) {}
};

LADSPA_Handle instantiate_method (const struct _LADSPA_Descriptor * Descriptor, unsigned long SampleRate)
{
	dsp*		p = new mydsp();
	portData* 	d = new portData(p->getNumInputs(), p->getNumOutputs());

	p->buildUserInterface(d);
	return new PLUGIN (SampleRate, d, p);
}

void connect_method (LADSPA_Handle Instance, unsigned long Port, LADSPA_Data * DataLocation)
{
	PLUGIN* p = (PLUGIN*) Instance;
	p->fPortData->setPortData(Port, DataLocation);
}

void activate_method (LADSPA_Handle Instance)
{
	PLUGIN* p = (PLUGIN*) Instance;
	p->fDsp->init(p->fSampleRate);
}

void run_method (LADSPA_Handle Instance, unsigned long SampleCount)
{
	PLUGIN* p = (PLUGIN*) Instance;
	p->fPortData->updateCtrlZones();
	AVOIDDENORMALS;
	p->fDsp->compute(SampleCount, p->fPortData->getInputs(), p->fPortData->getOutputs());
}

void deactivate_method (LADSPA_Handle Instance)
{}

void cleanup_method (LADSPA_Handle Instance)
{
	PLUGIN* p = (PLUGIN*) Instance;
	delete p->fPortData;
	delete p->fDsp;
	delete p;
}

//--------------------------------------------------------------------------------------

void init_descriptor(LADSPA_Descriptor* descriptor)
{
	descriptor->UniqueID = 123456;
	descriptor->Label = "none";
	descriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
	descriptor->Name = "none";
	descriptor->Maker = "Yann Orlarey";
	descriptor->Copyright = "GPL";

	descriptor->ImplementationData = 0;

	// description des methods
	descriptor->instantiate = instantiate_method;
	descriptor->connect_port = connect_method;
	descriptor->activate = activate_method;
	descriptor->run = run_method;
	descriptor->run_adding = 0;
	descriptor->set_run_adding_gain = 0;
	descriptor->deactivate = deactivate_method;
	descriptor->cleanup = cleanup_method;
}

//--------------------------------------------------------------------------------------

const LADSPA_Descriptor * ladspa_descriptor(unsigned long Index)
{
    if (Index == 0) {
		if (gDescriptor == 0)
		{
			// allocate temporaries dsp and portCollector to build the plugin description
			mydsp* p = new mydsp();
			if (p) {
				portCollector*	c=new portCollector(p->getNumInputs(), p->getNumOutputs());
				p->buildUserInterface(c);
				gDescriptor = new LADSPA_Descriptor;
				init_descriptor(gDescriptor);
				c->fillPortDescription(gDescriptor);
				delete p;
			} else {
				printf("Memory Error : unable to allocate the dsp object\n");
			}
		}
		return gDescriptor;
	} else {
		return NULL;
	}
}

/********************END ARCHITECTURE SECTION (part 2/2)****************/



#endif
