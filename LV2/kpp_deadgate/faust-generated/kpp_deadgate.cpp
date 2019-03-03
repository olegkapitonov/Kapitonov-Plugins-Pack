/* ------------------------------------------------------------
author: "Oleg Kapitonov"
license: "GPLv3"
name: "kpp_deadgate"
version: "1.0"
Code generated with Faust 2.5.23 (https://faust.grame.fr)
Compilation options: cpp, -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __kpp_deadgate_H__
#define  __kpp_deadgate_H__

/************************************************************************
 ************************************************************************
    FAUST Architecture File
    Copyright (C) 2009-2016 Albert Graef <aggraef@gmail.com>
    ---------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation; either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with the GNU C Library; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA.
 ************************************************************************
 ************************************************************************/

/* LV2 architecture for Faust synths. */

/* NOTE: This requires one of the Boost headers (boost/circular_buffer.hpp),
   so to compile Faust programs created with this architecture you need to
   have at least the Boost headers installed somewhere on your include path
   (the Boost libraries aren't needed). */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <list>
#include <map>
#include <set>

// generic Faust dsp and UI classes
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

using namespace std;

typedef pair<const char*,const char*> strpair;

struct Meta : std::map<const char*, const char*>
{
  void declare(const char *key, const char *value)
  {
    (*this)[key] = value;
  }
  const char* get(const char *key, const char *def)
  {
    if (this->find(key) != this->end())
      return (*this)[key];
    else
      return def;
  }
};

/******************************************************************************
*******************************************************************************

		       VECTOR INTRINSICS

*******************************************************************************
*******************************************************************************/


/***************************************************************************
   LV2 UI interface
 ***************************************************************************/

enum ui_elem_type_t {
  UI_BUTTON, UI_CHECK_BUTTON,
  UI_V_SLIDER, UI_H_SLIDER, UI_NUM_ENTRY,
  UI_V_BARGRAPH, UI_H_BARGRAPH,
  UI_END_GROUP, UI_V_GROUP, UI_H_GROUP, UI_T_GROUP
};

struct ui_elem_t {
  ui_elem_type_t type;
  const char *label;
  int port;
  float *zone;
  void *ref;
  float init, min, max, step;
};

class LV2UI : public UI
{
public:
  bool is_instr;
  int nelems, nports;
  ui_elem_t *elems;
  map< int, list<strpair> > metadata;

  LV2UI(int maxvoices = 0);
  virtual ~LV2UI();

protected:
  void add_elem(ui_elem_type_t type, const char *label = NULL);
  void add_elem(ui_elem_type_t type, const char *label, float *zone);
  void add_elem(ui_elem_type_t type, const char *label, float *zone,
		float init, float min, float max, float step);
  void add_elem(ui_elem_type_t type, const char *label, float *zone,
		float min, float max);

  bool have_freq, have_gain, have_gate;
  bool is_voice_ctrl(const char *label);

public:
  virtual void addButton(const char* label, float* zone);
  virtual void addCheckButton(const char* label, float* zone);
  virtual void addVerticalSlider(const char* label, float* zone, float init, float min, float max, float step);
  virtual void addHorizontalSlider(const char* label, float* zone, float init, float min, float max, float step);
  virtual void addNumEntry(const char* label, float* zone, float init, float min, float max, float step);

  virtual void addHorizontalBargraph(const char* label, float* zone, float min, float max);
  virtual void addVerticalBargraph(const char* label, float* zone, float min, float max);
    
  virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}

  virtual void openTabBox(const char* label);
  virtual void openHorizontalBox(const char* label);
  virtual void openVerticalBox(const char* label);
  virtual void closeBox();

  virtual void run();

  virtual void declare(float* zone, const char* key, const char* value);
};

LV2UI::LV2UI(int maxvoices)
{
  is_instr = maxvoices>0;
  have_freq = have_gain = have_gate = false;
  nelems = nports = 0;
  elems = NULL;
}

LV2UI::~LV2UI()
{
  if (elems) free(elems);
}

void LV2UI::declare(float* zone, const char* key, const char* value)
{
  map< int, list<strpair> >::iterator it = metadata.find(nelems);
  if (it != metadata.end())
    it->second.push_back(strpair(key, value));
  else
    metadata[nelems] = list<strpair>(1, strpair(key, value));
}

inline void LV2UI::add_elem(ui_elem_type_t type, const char *label)
{
  ui_elem_t *elems1 = (ui_elem_t*)realloc(elems, (nelems+1)*sizeof(ui_elem_t));
  if (elems1)
    elems = elems1;
  else
    return;
  elems[nelems].type = type;
  elems[nelems].label = label;
  elems[nelems].port = -1;
  elems[nelems].zone = NULL;
  elems[nelems].ref = NULL;
  elems[nelems].init = 0.0;
  elems[nelems].min = 0.0;
  elems[nelems].max = 0.0;
  elems[nelems].step = 0.0;
  nelems++;
}

#define portno(label) (is_voice_ctrl(label)?-1:nports++)

inline void LV2UI::add_elem(ui_elem_type_t type, const char *label, float *zone)
{
  ui_elem_t *elems1 = (ui_elem_t*)realloc(elems, (nelems+1)*sizeof(ui_elem_t));
  if (elems1)
    elems = elems1;
  else
    return;
  elems[nelems].type = type;
  elems[nelems].label = label;
  elems[nelems].port = portno(label);
  elems[nelems].zone = zone;
  elems[nelems].ref = NULL;
  elems[nelems].init = 0.0;
  elems[nelems].min = 0.0;
  elems[nelems].max = 0.0;
  elems[nelems].step = 0.0;
  nelems++;
}

inline void LV2UI::add_elem(ui_elem_type_t type, const char *label, float *zone,
			     float init, float min, float max, float step)
{
  ui_elem_t *elems1 = (ui_elem_t*)realloc(elems, (nelems+1)*sizeof(ui_elem_t));
  if (elems1)
    elems = elems1;
  else
    return;
  elems[nelems].type = type;
  elems[nelems].label = label;
  elems[nelems].port = portno(label);
  elems[nelems].zone = zone;
  elems[nelems].ref = NULL;
  elems[nelems].init = init;
  elems[nelems].min = min;
  elems[nelems].max = max;
  elems[nelems].step = step;
  nelems++;
}

inline void LV2UI::add_elem(ui_elem_type_t type, const char *label, float *zone,
			     float min, float max)
{
  ui_elem_t *elems1 = (ui_elem_t*)realloc(elems, (nelems+1)*sizeof(ui_elem_t));
  if (elems1)
    elems = elems1;
  else
    return;
  elems[nelems].type = type;
  elems[nelems].label = label;
  elems[nelems].port = portno(label);
  elems[nelems].zone = zone;
  elems[nelems].ref = NULL;
  elems[nelems].init = 0.0;
  elems[nelems].min = min;
  elems[nelems].max = max;
  elems[nelems].step = 0.0;
  nelems++;
}

inline bool LV2UI::is_voice_ctrl(const char *label)
{
  if (!is_instr)
    return false;
  else if (!have_freq && !strcmp(label, "freq"))
    return (have_freq = true);
  else if (!have_gain && !strcmp(label, "gain"))
    return (have_gain = true);
  else if (!have_gate && !strcmp(label, "gate"))
    return (have_gate = true);
  else
    return false;
}

void LV2UI::addButton(const char* label, float* zone)
{ add_elem(UI_BUTTON, label, zone); }
void LV2UI::addCheckButton(const char* label, float* zone)
{ add_elem(UI_CHECK_BUTTON, label, zone); }
void LV2UI::addVerticalSlider(const char* label, float* zone, float init, float min, float max, float step)
{ add_elem(UI_V_SLIDER, label, zone, init, min, max, step); }
void LV2UI::addHorizontalSlider(const char* label, float* zone, float init, float min, float max, float step)
{ add_elem(UI_H_SLIDER, label, zone, init, min, max, step); }
void LV2UI::addNumEntry(const char* label, float* zone, float init, float min, float max, float step)
{ add_elem(UI_NUM_ENTRY, label, zone, init, min, max, step); }

void LV2UI::addHorizontalBargraph(const char* label, float* zone, float min, float max)
{ add_elem(UI_H_BARGRAPH, label, zone, min, max); }
void LV2UI::addVerticalBargraph(const char* label, float* zone, float min, float max)
{ add_elem(UI_V_BARGRAPH, label, zone, min, max); }

void LV2UI::openTabBox(const char* label)
{ add_elem(UI_T_GROUP, label); }
void LV2UI::openHorizontalBox(const char* label)
{ add_elem(UI_H_GROUP, label); }
void LV2UI::openVerticalBox(const char* label)
{ add_elem(UI_V_GROUP, label); }
void LV2UI::closeBox()
{ add_elem(UI_END_GROUP); }

void LV2UI::run() {}

//----------------------------------------------------------------------------
//  FAUST generated signal processor
//----------------------------------------------------------------------------

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <cmath>
#include <math.h>

float kpp_deadgate_faustpower2_f(float value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS kpp_deadgate
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class kpp_deadgate : public dsp {
	
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
		m->declare("name", "kpp_deadgate");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 2;
		
	}
	virtual int getNumOutputs() {
		return 2;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			case 1: {
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
			case 1: {
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
		fConst7 = (1.0f / kpp_deadgate_faustpower2_f(fConst3));
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
		fConst24 = (1.0f / kpp_deadgate_faustpower2_f(fConst20));
		fConst25 = (2.0f * (1.0f - fConst24));
		fConst26 = tanf((1884.95557f / fConst0));
		fConst27 = (1.0f / fConst26);
		fConst28 = (1.0f / (((fConst27 + 1.0f) / fConst26) + 1.0f));
		fConst29 = (1.0f - ((1.0f - fConst27) / fConst26));
		fConst30 = (1.0f / kpp_deadgate_faustpower2_f(fConst26));
		fConst31 = (2.0f * (1.0f - fConst30));
		fConst32 = tanf((942.477783f / fConst0));
		fConst33 = (1.0f / fConst32);
		fConst34 = (1.0f / (((fConst33 + 1.0f) / fConst32) + 1.0f));
		fConst35 = (1.0f - ((1.0f - fConst33) / fConst32));
		fConst36 = (1.0f / kpp_deadgate_faustpower2_f(fConst32));
		fConst37 = (2.0f * (1.0f - fConst36));
		fConst38 = tanf((471.238892f / fConst0));
		fConst39 = (1.0f / fConst38);
		fConst40 = (1.0f / (((fConst39 + 1.0f) / fConst38) + 1.0f));
		fConst41 = (1.0f - ((1.0f - fConst39) / fConst38));
		fConst42 = (1.0f / kpp_deadgate_faustpower2_f(fConst38));
		fConst43 = (2.0f * (1.0f - fConst42));
		fConst44 = tanf((204.203522f / fConst0));
		fConst45 = (1.0f / fConst44);
		fConst46 = (1.0f / (((fConst45 + 1.0f) / fConst44) + 1.0f));
		fConst47 = (1.0f - ((1.0f - fConst45) / fConst44));
		fConst48 = (1.0f / kpp_deadgate_faustpower2_f(fConst44));
		fConst49 = (2.0f * (1.0f - fConst48));
		iConst50 = int((0.0199999996f * fConst0));
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
	
	virtual kpp_deadgate* clone() {
		return new kpp_deadgate();
	}
	virtual int getSampleRate() {
		return fSamplingFreq;
		
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("kpp_deadgate");
		ui_interface->addVerticalSlider("Dead Zone", &fVslider0, -120.0f, -120.0f, 0.0f, 0.00100000005f);
		ui_interface->addVerticalSlider("Noise Gate", &fVslider1, -120.0f, -120.0f, 0.0f, 0.00100000005f);
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = powf(10.0f, (0.0500000007f * float(fVslider0)));
		float fSlow1 = (0.0f - fSlow0);
		float fSlow2 = powf(10.0f, (0.0500000007f * float(fVslider1)));
		for (int i = 0; (i < count); i = (i + 1)) {
			float fTemp0 = (float(input0[i]) + float(input1[i]));
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
			float fTemp50 = (0.501187205f * ((((((fRec0[0] * fTemp7) + (fRec13[0] * fTemp16)) + (fRec26[0] * fTemp24)) + (fRec38[0] * fTemp31)) + (fRec49[0] * fTemp37)) + (fConst84 * ((fRec59[0] * fTemp42) + (fRec68[0] * fTemp46)))));
			output0[i] = FAUSTFLOAT(fTemp50);
			output1[i] = FAUSTFLOAT(fTemp50);
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

//----------------------------------------------------------------------------
//  LV2 interface
//----------------------------------------------------------------------------

#line 286 "lv2.cpp"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <bitset>
#include <boost/circular_buffer.hpp>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/dynmanifest/dynmanifest.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

// Set this to the proper shared library extension for your system
#ifndef DLLEXT
#define DLLEXT ".so"
#endif

#ifndef URI_PREFIX
#define URI_PREFIX "https://faustlv2.bitbucket.io"
#endif

#ifndef PLUGIN_URI
#define PLUGIN_URI URI_PREFIX "/kpp_deadgate"
#endif

#define MIDI_EVENT_URI "http://lv2plug.in/ns/ext/midi#MidiEvent"

/* Setting NVOICES at compile time overrides meta data in the Faust source. If
   set, this must be an integer value >= 0. A nonzero value indicates an
   instrument (VSTi) plugin with the given maximum number of voices. Use 1 for
   a monophonic synthesizer, and 0 for a simple effect plugin. If NVOICES
   isn't defined at compile time then the number of voices of an instrument
   plugin can also be set with the global "nvoices" meta data key in the Faust
   source. This setting also adds a special "polyphony" control to the plugin
   which can be used to dynamically adjust the actual number of voices in the
   range 1..NVOICES. */
//#define NVOICES 16

/* This enables a special "tuning" control for instruments which lets you
   select the MTS tuning to be used for the synth. In order to use this, you
   just drop some sysex (.syx) files with MTS octave-based tunings in 1- or
   2-byte format into the ~/.fautvst/tuning directory (these can be generated
   with the author's sclsyx program, https://bitbucket.org/agraef/sclsyx).
   The control will only be shown if any .syx files were found at startup. 0
   selects the default tuning (standard 12-tone equal temperament), i>0 the
   tuning in the ith sysex file (in alphabetic order). */
#ifndef FAUST_MTS
#define FAUST_MTS 1
#endif

/* This allows various manifest data to be generated from the corresponding
   metadata (author, name, description, license) in the Faust source. */
#ifndef FAUST_META
#define FAUST_META 1
#endif

/* This enables automatic MIDI controller mapping based on the midi:ctrl
   attributes in the Faust source. We have this enabled by default, but you
   may have to disable it if the custom controller mapping gets in the way of
   the automation facilities that the host provides. (But then again if the
   host wants to do its own controller mapping then it probably won't, or at
   least shouldn't, send us the MIDI controllers in the first place.) */
#ifndef FAUST_MIDICC
#define FAUST_MIDICC 1
#endif

/* This enables or disables the plugin's custom Qt GUI (cf. lv2ui.cpp). Note
   that this only affects the plugin manifest, the GUI code itself is in a
   separate module created with the lv2ui.cpp architecture. Also, you'll have
   to use the alternative lv2ui manifest templates to tell the LV2 host about
   the GUI. */
#ifndef FAUST_UI
#define FAUST_UI 0
#endif

// You can define these for various debugging output items.
//#define DEBUG_META 1 // recognized MIDI controller metadata
//#define DEBUG_VOICES 1 // triggering of synth voices
//#define DEBUG_VOICE_ALLOC 1 // voice allocation
//#define DEBUG_MIDI 1 // incoming MIDI messages
//#define DEBUG_NOTES 1 // note messages
//#define DEBUG_MIDICC 1 // controller messages
//#define DEBUG_RPN 1 // RPN messages (pitch bend range, master tuning)
//#define DEBUG_MTS 1 // MTS messages (octave/scale tuning)

// Note and voice data structures.

struct NoteInfo {
  uint8_t ch;
  int8_t note;
};

struct VoiceData {
  // Octave tunings (offsets in semitones) per MIDI channel.
  float tuning[16][12];
  // Allocated voices per MIDI channel and note.
  int8_t notes[16][128];
  // Free and used voices.
  int n_free, n_used;
  boost::circular_buffer<int> free_voices;
  boost::circular_buffer<int> used_voices;
  NoteInfo *note_info;
  // Voices queued for note-offs (zero-length notes).
  set<int> queued;
  // Last gate value during run() for each voice. We need to keep track of
  // these so that we can force the Faust synth to retrigger a note when
  // needed.
  float *lastgate;
  // Current pitch bend and pitch bend range on each MIDI channel, in semitones.
  float bend[16], range[16];
  // Current coarse, fine and total master tuning on each MIDI channel (tuning
  // offset relative to A4 = 440 Hz, in semitones).
  float coarse[16], fine[16], tune[16];
  VoiceData(int n) : free_voices(n), used_voices(n) { }
};

#if FAUST_MTS

// Helper classes to read and store MTS tunings.

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <string>
#include <vector>

struct MTSTuning {
  char *name; // name of the tuning
  int len; // length of sysex data in bytes
  unsigned char *data; // sysex data
  MTSTuning() : name(0), len(0), data(0) {}
  MTSTuning& operator=(const MTSTuning &t)
  {
    if (this == &t) return *this;
    if (name) free(name); if (data) free(data);
    name = 0; data = 0; len = t.len;
    if (t.name) {
      name = strdup(t.name); assert(name);
    }
    if (t.data) {
      data = (unsigned char*)malloc(len); assert(data);
      memcpy(data, t.data, len);
    }
    return *this;
  }
  MTSTuning(const MTSTuning& t) : name(0), len(0), data(0)
  { *this = t; }
  MTSTuning(const char *filename);
  ~MTSTuning()
  { if (name) free(name); if (data) free(data); }
};

MTSTuning::MTSTuning(const char *filename)
{
  FILE *fp = fopen(filename, "rb");
  name = 0; len = 0; data = 0;
  if (!fp) return;
  struct stat st;
  if (fstat(fileno(fp), &st)) return;
  len = st.st_size;
  data = (unsigned char*)calloc(len, 1);
  if (!data) {
    len = 0; fclose(fp);
    return;
  }
  assert(len > 0);
  if (fread(data, 1, len, fp) < len) {
    free(data); len = 0; data = 0; fclose(fp);
    return;
  }
  fclose(fp);
  // Do some basic sanity checks.
  if (data[0] != 0xf0 || data[len-1] != 0xf7 || // not a sysex message
      (data[1] != 0x7e && data[1] != 0x7f) || data[3] != 8 || // not MTS
      !((len == 21 && data[4] == 8) ||
	(len == 33 && data[4] == 9))) { // no 1- or 2-byte tuning
    free(data); len = 0; data = 0;
    return;
  }
  // Name of the tuning is the basename of the file, without the trailing .syx
  // suffix.
  string nm = filename;
  size_t p = nm.rfind(".syx");
  if (p != string::npos) nm.erase(p);
  p = nm.rfind('/');
  if (p != string::npos) nm.erase(0, p+1);
  name = strdup(nm.c_str());
  assert(name);
}

struct MTSTunings {
  vector<MTSTuning> tuning;
  MTSTunings() {}
  MTSTunings(const char *path);
};

static bool compareByName(const MTSTuning &a, const MTSTuning &b)
{
  return strcmp(a.name, b.name) < 0;
}

MTSTunings::MTSTunings(const char *path)
{
  DIR *dp = opendir(path);
  if (!dp) return;
  struct dirent *d;
  while ((d = readdir(dp))) {
    string nm = d->d_name;
    if (nm.length() > 4 && nm.substr(nm.length()-4) == ".syx") {
      string pathname = path;
      pathname += "/";
      pathname += nm;
      MTSTuning t(pathname.c_str());
      if (t.data) tuning.push_back(t);
    }
  }
  closedir(dp);
  // sort found tunings by name
  sort(tuning.begin(), tuning.end(), compareByName);
}

#endif

#if FAUST_MIDICC
static float ctrlval(const ui_elem_t &el, uint8_t v)
{
  // Translate the given MIDI controller value to the range and stepsize
  // indicated by the Faust control.
  switch (el.type) {
  case UI_BUTTON: case UI_CHECK_BUTTON:
    return (float)(v>=64);
  default:
    /* Continuous controllers. The problem here is that the range 0..127 is
       not symmetric. We'd like to map 64 to the center of the range
       (max-min)/2 and at the same time retain the full control range
       min..max. So let's just pretend that there are 128 controller values
       and map value 127 to the max value anyway. */
    if (v==127)
      return el.max;
    else
      // XXXFIXME: We might want to add proper quantization according to
      // el.step here.
      return el.min+(el.max-el.min)*v/128;
  }
}
#endif

/***************************************************************************/

/* Polyphonic Faust plugin data structure. XXXTODO: At present this is just a
   big struct which exposes all requisite data. Some more work is needed to
   make the interface a bit more abstract and properly encapsulate the
   internal data structures, so that implementation details can be changed
   more easily. */

struct LV2Plugin {
  const int maxvoices;	// maximum number of voices (zero if not an instrument)
  const int ndsps;	// number of dsp instances (1 if maxvoices==0)
  bool active;		// activation status
  int rate;		// sampling rate
  int nvoices;		// current number of voices (<= maxvoices)
  int tuning_no;	// current tuning number (<= n_tunings)
  kpp_deadgate **dsp;		// the dsps
  LV2UI **ui;		// their Faust interface descriptions
  int n_in, n_out;	// number of input and output control ports
  int *ctrls;		// Faust ui elements (indices into ui->elems)
  float **ports;	// corresponding LV2 data
  float *portvals;	// cached port data from the last run
  float *midivals[16];	// per-midi channel data
  int *inctrls, *outctrls;	// indices for active and passive controls
  float **inputs, **outputs;	// audio buffers
  int freq, gain, gate;	// indices of voice controls
  unsigned n_samples;	// current block size
  float **outbuf;	// audio buffers for mixing down the voices
  float **inbuf;	// dummy input buffer
  LV2_Atom_Sequence* event_port; // midi input
  float *poly, *tuning;	// polyphony and tuning ports
  std::map<uint8_t,int> ctrlmap; // MIDI controller map
  // Needed host features.
  LV2_URID_Map* map;	// the urid extension
  LV2_URID midi_event;	// midi event uri
  // Current RPN MSB and LSB numbers, as set with controllers 101 and 100.
  uint8_t rpn_msb[16], rpn_lsb[16];
  // Current data entry MSB and LSB numbers, as set with controllers 6 and 38.
  uint8_t data_msb[16], data_lsb[16];
  // Synth voice data (instruments only).
  VoiceData *vd;

  // Static methods. These all use static data so they can be invoked before
  // instantiating a plugin.

  // Global meta data (dsp name, author, etc.).
  static Meta *meta;
  static void init_meta()
  {
    if (!meta && (meta = new Meta)) {
      // We allocate the temporary dsp object on the heap here, to prevent
      // large dsp objects from running out of stack in environments where
      // stack space is precious (e.g., Reaper). Note that if any of these
      // allocations fail then no meta data will be available, but at least we
      // won't make the host crash and burn.
      kpp_deadgate* tmp_dsp = new kpp_deadgate();
      if (tmp_dsp) {
	tmp_dsp->metadata(meta);
	delete tmp_dsp;
      }
    }
  }
  static const char *meta_get(const char *key, const char *deflt)
  {
    init_meta();
    return meta?meta->get(key, deflt):deflt;
  }

  static const char *pluginName()
  {
    return meta_get("name", "kpp_deadgate");
  }

  static const char *pluginAuthor()
  {
    return meta_get("author", "");
  }

  static const char *pluginDescription()
  {
    return meta_get("description", "");
  }

  static const char *pluginLicense()
  {
    return meta_get("license", "");
  }

  static const char *pluginVersion()
  {
    return meta_get("version", "");
  }

  // Load a collection of sysex files with MTS tunings in ~/.faust/tuning.
  static int n_tunings;
#if FAUST_MTS
  static MTSTunings *mts;

  static MTSTunings *load_sysex_data()
  {
    if (!mts) {
      string mts_path;
      // Look for FAUST_HOME. If that isn't set, try $HOME/.faust. If HOME
      // isn't set either, just assume a .faust subdir of the cwd.
      const char *home = getenv("FAUST_HOME");
      if (home)
	mts_path = home;
      else {
	home = getenv("HOME");
	if (home) {
	  mts_path = home;
	  mts_path += "/.faust";
	} else
	  mts_path = ".faust";
      }
      // MTS tunings are looked for in this subdir.
      mts_path += "/tuning";
      mts = new MTSTunings(mts_path.c_str());
#ifdef __APPLE__
      if (!mts || mts->tuning.size() == 0) {
	// Also check ~/Library/Faust/Tuning on the Mac.
	home = getenv("HOME");
	if (home) {
	  if (mts) delete mts;
	  mts_path = home;
	  mts_path += "/Library/Faust/Tuning";
	  mts = new MTSTunings(mts_path.c_str());
	}
      }
#endif
      n_tunings = mts->tuning.size();
    }
    return mts;
  }
#endif

  // The number of voices of an instrument plugin. We get this information
  // from the global meta data (nvoices key) of the dsp module if present, and
  // you can also override this setting at compile time by defining the
  // NVOICES macro. If neither is defined or the value is zero then the plugin
  // becomes a simple audio effect instead.
  static int numVoices()
  {
#ifdef NVOICES
    return NVOICES;
#else
    const char *numVoices = meta_get("nvoices", "0");
    int nvoices = atoi(numVoices);
    if (nvoices < 0 ) nvoices = 0;
    return nvoices;
#endif
  }

  // Instance methods.

  LV2Plugin(const int num_voices, const int sr)
    : maxvoices(num_voices), ndsps(num_voices<=0?1:num_voices),
      vd(num_voices>0?new VoiceData(num_voices):0)
  {
    // Initialize static data.
    init_meta();
#if FAUST_MTS
    // Synth: load tuning sysex data if present.
    if (num_voices>0) load_sysex_data();
#endif
    // Allocate data structures and set some reasonable defaults.
    dsp = (kpp_deadgate**)calloc(ndsps, sizeof(kpp_deadgate*));
    ui = (LV2UI**)calloc(ndsps, sizeof(LV2UI*));
    assert(dsp && ui);
    if (vd) {
      vd->note_info = (NoteInfo*)calloc(ndsps, sizeof(NoteInfo));
      vd->lastgate = (float*)calloc(ndsps, sizeof(float));
      assert(vd->note_info && vd->lastgate);
    }
    active = false;
    rate = sr;
    nvoices = maxvoices;
    tuning_no = 0;
    n_in = n_out = 0;
    map = NULL;
    midi_event = -1;
    event_port = NULL;
    poly = tuning = NULL;
    freq = gain = gate = -1;
    if (vd) {
      vd->n_free = maxvoices;
      for (int i = 0; i < maxvoices; i++) {
	vd->free_voices.push_back(i);
	vd->lastgate[i] = 0.0f;
      }
      for (int i = 0; i < 16; i++) {
	vd->bend[i] = 0.0f;
	vd->range[i] = 2.0f;
	vd->coarse[i] = vd->fine[i] = vd->tune[i] = 0.0f;
	for (int j = 0; j < 12; j++)
	  vd->tuning[i][j] = 0.0f;
      }
      vd->n_used = 0;
      memset(vd->notes, 0xff, sizeof(vd->notes));
    }
    n_samples = 0;
    ctrls = inctrls = outctrls = NULL;
    ports = inputs = outputs = inbuf = outbuf = NULL;
    portvals = NULL;
    memset(midivals, 0, sizeof(midivals));
    // Initialize the Faust DSPs.
    for (int i = 0; i < ndsps; i++) {
      dsp[i] = new kpp_deadgate();
      ui[i] = new LV2UI(num_voices);
      dsp[i]->init(rate);
      dsp[i]->buildUserInterface(ui[i]);
    }
    // The ports are numbered as follows: 0..k-1 are the control ports, then
    // come the n audio input ports, then the m audio output ports, and
    // finally the midi input port and the polyphony and tuning controls.
    int k = ui[0]->nports, p = 0, q = 0;
    int n = dsp[0]->getNumInputs(), m = dsp[0]->getNumOutputs();
    // Allocate tables for the built-in control elements and their ports.
    ctrls = (int*)calloc(k, sizeof(int));
    inctrls = (int*)calloc(k, sizeof(int));
    outctrls = (int*)calloc(k, sizeof(int));
    ports = (float**)calloc(k, sizeof(float*));
    portvals = (float*)calloc(k, sizeof(float));
    assert(k == 0 || (ctrls && inctrls && outctrls && ports && portvals));
    for (int ch = 0; ch < 16; ch++) {
      midivals[ch] = (float*)calloc(k, sizeof(float));
      assert(k == 0 || midivals[ch]);
    }
    // Scan the Faust UI for active and passive controls which become the
    // input and output control ports of the plugin, respectively.
    for (int i = 0, j = 0; i < ui[0]->nelems; i++) {
      switch (ui[0]->elems[i].type) {
      case UI_T_GROUP: case UI_H_GROUP: case UI_V_GROUP: case UI_END_GROUP:
	// control groups (ignored right now)
	break;
      case UI_H_BARGRAPH: case UI_V_BARGRAPH:
	// passive controls (output ports)
	ctrls[j++] = i;
	outctrls[q++] = i;
	break;
      default:
	// active controls (input ports)
	if (maxvoices == 0)
	  goto noinstr;
	else if (freq == -1 &&
		 !strcmp(ui[0]->elems[i].label, "freq"))
	  freq = i;
	else if (gain == -1 &&
		 !strcmp(ui[0]->elems[i].label, "gain"))
	  gain = i;
	else if (gate == -1 &&
		 !strcmp(ui[0]->elems[i].label, "gate"))
	  gate = i;
	else {
	noinstr:
#if FAUST_MIDICC
	  std::map< int, list<strpair> >::iterator it =
	    ui[0]->metadata.find(i);
	  if (it != ui[0]->metadata.end()) {
	    // Scan for controller mappings.
	    for (std::list<strpair>::iterator jt = it->second.begin();
		 jt != it->second.end(); jt++) {
	      const char *key = jt->first, *val = jt->second;
#if DEBUG_META
	      fprintf(stderr, "ctrl '%s' meta: '%s' -> '%s'\n",
		      ui[0]->elems[i].label, key, val);
#endif
	      if (strcmp(key, "midi") == 0) {
		unsigned num;
		if (sscanf(val, "ctrl %u", &num) < 1) continue;
#if 0 // enable this to get feedback about controller assignments
		fprintf(stderr, "%s: cc %d -> %s\n", PLUGIN_URI, num,
			ui[0]->elems[i].label);
#endif
		ctrlmap.insert(std::pair<uint8_t,int>(num, p));
	      }
	    }
	  }
#endif
	  ctrls[j++] = i;
	  inctrls[p++] = i;
	  int p = ui[0]->elems[i].port;
	  float val = ui[0]->elems[i].init;
	  assert(p>=0);
	  portvals[p] = val;
	  for (int ch = 0; ch < 16; ch++)
	    midivals[ch][p] = val;
	}
	break;
      }
    }
    // Realloc the inctrls and outctrls vectors to their appropriate sizes.
    inctrls = (int*)realloc(inctrls, p*sizeof(int));
    assert(p == 0 || inctrls);
    outctrls = (int*)realloc(outctrls, q*sizeof(int));
    assert(q == 0 || outctrls);
    n_in = p; n_out = q;
    // Allocate vectors for the audio input and output ports. Like
    // ports, these will be initialized in the connect_port callback.
    inputs = (float**)calloc(n, sizeof(float*));
    assert(n == 0 || inputs);
    outputs = (float**)calloc(m, sizeof(float*));
    assert(m == 0 || outputs);
    if (maxvoices > 0) {
      // Initialize the mixdown buffer.
      outbuf = (float**)calloc(m, sizeof(float*));
      assert(m == 0 || outbuf);
      // We start out with a blocksize of 512 samples here. Hopefully this is
      // enough for most realtime hosts so that we can avoid reallocations
      // later when we know what the actual blocksize is.
      n_samples = 512;
      for (int i = 0; i < m; i++) {
	outbuf[i] = (float*)malloc(n_samples*sizeof(float));
	assert(outbuf[i]);
      }
      // Initialize a 1-sample dummy input buffer used for retriggering notes.
      inbuf = (float**)calloc(n, sizeof(float*));
      assert(n == 0 || inbuf);
      for (int i = 0; i < m; i++) {
	inbuf[i] = (float*)malloc(sizeof(float));
	assert(inbuf[i]);
	*inbuf[i] = 0.0f;
      }
    }
  }

  ~LV2Plugin()
  {
    const int n = dsp[0]->getNumInputs();
    const int m = dsp[0]->getNumOutputs();
    for (int i = 0; i < ndsps; i++) {
      delete dsp[i];
      delete ui[i];
    }
    free(ctrls); free(inctrls); free(outctrls);
    free(ports); free(portvals);
    free(inputs); free(outputs);
    for (int ch = 0; ch < 16; ch++)
      free(midivals[ch]);
    if (inbuf) {
      for (int i = 0; i < n; i++)
	free(inbuf[i]);
      free(inbuf);
    }
    if (outbuf) {
      for (int i = 0; i < m; i++)
	free(outbuf[i]);
      free(outbuf);
    }
    free(dsp); free(ui);
    if (vd) {
      free(vd->note_info);
      free(vd->lastgate);
      delete vd;
    }
  }
  // Voice allocation.

#if DEBUG_VOICE_ALLOC
  void print_voices(const char *msg)
  {
    fprintf(stderr, "%s: notes =", msg);
    for (uint8_t ch = 0; ch < 16; ch++)
      for (int note = 0; note < 128; note++)
	if (vd->notes[ch][note] >= 0)
	  fprintf(stderr, " [%d] %d(#%d)", ch, note, vd->notes[ch][note]);
    fprintf(stderr, "\nqueued (%d):", vd->queued.size());
    for (int i = 0; i < nvoices; i++)
      if (vd->queued.find(i) != vd->queued.end()) fprintf(stderr, " #%d", i);
    fprintf(stderr, "\nused (%d):", vd->n_used);
    for (boost::circular_buffer<int>::iterator it = vd->used_voices.begin();
	 it != vd->used_voices.end(); it++)
      fprintf(stderr, " #%d->%d", *it, vd->note_info[*it].note);
    fprintf(stderr, "\nfree (%d):", vd->n_free);
    for (boost::circular_buffer<int>::iterator it = vd->free_voices.begin();
	 it != vd->free_voices.end(); it++)
      fprintf(stderr, " #%d", *it);
    fprintf(stderr, "\n");
  }
#endif

  int alloc_voice(uint8_t ch, int8_t note, int8_t vel)
  {
    int i = vd->notes[ch][note];
    if (i >= 0) {
      // note already playing on same channel, retrigger it
      voice_off(i);
      voice_on(i, note, vel, ch);
      // move this voice to the end of the used list
      for (boost::circular_buffer<int>::iterator it =
	     vd->used_voices.begin();
	   it != vd->used_voices.end(); it++) {
	if (*it == i) {
	  vd->used_voices.erase(it);
	  vd->used_voices.push_back(i);
	  break;
	}
      }
#if DEBUG_VOICE_ALLOC
      print_voices("retrigger");
#endif
      return i;
    } else if (vd->n_free > 0) {
      // take voice from free list
      int i = vd->free_voices.front();
      vd->free_voices.pop_front();
      vd->n_free--;
      vd->used_voices.push_back(i);
      vd->note_info[i].ch = ch;
      vd->note_info[i].note = note;
      vd->n_used++;
      voice_on(i, note, vel, ch);
      vd->notes[ch][note] = i;
#if DEBUG_VOICE_ALLOC
      print_voices("alloc");
#endif
      return i;
    } else {
      // steal a voice
      assert(vd->n_used > 0);
      // FIXME: Maybe we should look for the oldest note on the *current*
      // channel here, but this is faster.
      int i = vd->used_voices.front();
      int oldch = vd->note_info[i].ch;
      int oldnote = vd->note_info[i].note;
      voice_off(i);
      vd->notes[oldch][oldnote] = -1;
      vd->queued.erase(i);
      vd->used_voices.pop_front();
      vd->used_voices.push_back(i);
      vd->note_info[i].ch = ch;
      vd->note_info[i].note = note;
      voice_on(i, note, vel, ch);
      vd->notes[ch][note] = i;
#if DEBUG_VOICE_ALLOC
      print_voices("steal");
#endif
      return i;
    }
  }

  int dealloc_voice(uint8_t ch, int8_t note, int8_t vel)
  {
    int i = vd->notes[ch][note];
    if (i >= 0) {
      if (vd->lastgate[i] == 0.0f && gate >= 0) {
	// zero-length note, queued for later
	vd->queued.insert(i);
	vd->notes[ch][note] = -1;
#if DEBUG_VOICE_ALLOC
	print_voices("dealloc (queued)");
#endif
	return i;
      }
      assert(vd->n_free < nvoices);
      vd->free_voices.push_back(i);
      vd->n_free++;
      voice_off(i);
      vd->notes[ch][note] = -1;
      // erase this voice from the used list
      for (boost::circular_buffer<int>::iterator it =
	     vd->used_voices.begin();
	   it != vd->used_voices.end(); it++) {
	if (*it == i) {
	  vd->used_voices.erase(it);
	  vd->n_used--;
	  break;
	}
      }
#if DEBUG_VOICE_ALLOC
      print_voices("dealloc");
#endif
      return i;
    }
    return -1;
  }


  float midicps(int8_t note, uint8_t chan)
  {
    float pitch = note + vd->tune[chan] +
      vd->tuning[chan][note%12] + vd->bend[chan];
    return 440.0*pow(2, (pitch-69.0)/12.0);
  }

  void voice_on(int i, int8_t note, int8_t vel, uint8_t ch)
  {
    if (vd->lastgate[i] == 1.0f && gate >= 0) {
      // Make sure that the synth sees the 0.0f gate so that the voice is
      // properly retriggered.
      *ui[i]->elems[gate].zone = 0.0f;
      dsp[i]->compute(1, inbuf, outbuf);
    }
#if DEBUG_VOICES
    fprintf(stderr, "voice on: %d %d (%g Hz) %d (%g)\n", i,
	    note, midicps(note, ch), vel, vel/127.0);
#endif
    if (freq >= 0)
      *ui[i]->elems[freq].zone = midicps(note, ch);
    if (gate >= 0)
      *ui[i]->elems[gate].zone = 1.0f;
    if (gain >= 0)
      *ui[i]->elems[gain].zone = vel/127.0;
    // reinitialize the per-channel control data for this voice
    for (int idx = 0; idx < n_in; idx++) {
      int j = inctrls[idx], k = ui[0]->elems[j].port;
      *ui[i]->elems[j].zone = midivals[ch][k];
    }
  }

  void voice_off(int i)
  {
#if DEBUG_VOICES
    fprintf(stderr, "voice off: %d\n", i);
#endif
    if (gate >= 0)
      *ui[i]->elems[gate].zone = 0.0f;
  }

  void update_voices(uint8_t chan)
  {
    // update running voices on the given channel after tuning or pitch bend
    // changes
    for (boost::circular_buffer<int>::iterator it =
	   vd->used_voices.begin();
	 it != vd->used_voices.end(); it++) {
      int i = *it;
      if (vd->note_info[i].ch == chan && freq >= 0) {
	int note = vd->note_info[i].note;
	*ui[i]->elems[freq].zone = midicps(note, chan);
      }
    }
  }

  void all_notes_off()
  {
    for (int i = 0; i < nvoices; i++)
      voice_off(i);
    for (int i = 0; i < 16; i++)
      vd->bend[i] = 0.0f;
    memset(vd->notes, 0xff, sizeof(vd->notes));
    vd->free_voices.clear();
    vd->n_free = nvoices;
    for (int i = 0; i < nvoices; i++)
      vd->free_voices.push_back(i);
    vd->queued.clear();
    vd->used_voices.clear();
    vd->n_used = 0;
  }

  void all_notes_off(uint8_t chan)
  {
    for (boost::circular_buffer<int>::iterator it =
	   vd->used_voices.begin();
	 it != vd->used_voices.end(); ) {
      int i = *it;
      if (vd->note_info[i].ch == chan) {
	assert(vd->n_free < nvoices);
	vd->free_voices.push_back(i);
	vd->n_free++;
	voice_off(i);
	vd->notes[vd->note_info[i].ch][vd->note_info[i].note] = -1;
	vd->queued.erase(i);
	// erase this voice from the used list
	it = vd->used_voices.erase(it);
	vd->n_used--;
#if DEBUG_VOICE_ALLOC
	print_voices("dealloc (all-notes-off)");
#endif
      } else
	it++;
    }
    vd->bend[chan] = 0.0f;
  }

  void queued_notes_off()
  {
    if (vd->queued.empty()) return;
    for (int i = 0; i < nvoices; i++)
      if (vd->queued.find(i) != vd->queued.end()) {
	assert(vd->n_free < nvoices);
	vd->free_voices.push_back(i);
	vd->n_free++;
	voice_off(i);
	vd->notes[vd->note_info[i].ch][vd->note_info[i].note] = -1;
	vd->queued.erase(i);
	// erase this voice from the used list
	for (boost::circular_buffer<int>::iterator it =
	       vd->used_voices.begin();
	     it != vd->used_voices.end(); it++) {
	  if (*it == i) {
	    vd->used_voices.erase(it);
	    vd->n_used--;
	    break;
	  }
	}
#if DEBUG_VOICE_ALLOC
	print_voices("dealloc (unqueued)");
#endif
      }
  }

  // Plugin activation status. suspend() deactivates a plugin (disables audio
  // processing), resume() reactivates it. Also, set_rate() changes the sample
  // rate. Note that the audio and MIDI process functions (see below) can
  // still be called in deactivated state, but this is optional. The plugin
  // tries to do some reasonable processing in either case, no matter whether
  // the host plugin architecture actually executes callbacks in suspended
  // state or not.

  void suspend()
  {
    active = false;
    if (maxvoices > 0) all_notes_off();
  }

  void resume()
  {
    for (int i = 0; i < ndsps; i++)
      dsp[i]->init(rate);
    for (int i = 0, j = 0; i < ui[0]->nelems; i++) {
      int p = ui[0]->elems[i].port;
      if (p >= 0) {
	float val = ui[0]->elems[i].init;
	portvals[p] = val;
      }
    }
    active = true;
  }

  void set_rate(int sr)
  {
    rate = sr;
    for (int i = 0; i < ndsps; i++)
      dsp[i]->init(rate);
  }

  // Audio and MIDI process functions. The plugin should run these in the
  // appropriate real-time callbacks.

  void process_audio(int blocksz, float **inputs, float **outputs)
  {
    int n = dsp[0]->getNumInputs(), m = dsp[0]->getNumOutputs();
    AVOIDDENORMALS;
    if (maxvoices > 0) queued_notes_off();
    if (!active) {
      // Depending on the plugin architecture and host, this code might never
      // be invoked, since the plugin is deactivitated at this point. But
      // let's do something reasonable here anyway.
      if (n == m) {
	// copy inputs to outputs
	for (int i = 0; i < m; i++)
	  for (unsigned j = 0; j < blocksz; j++)
	    outputs[i][j] = inputs[i][j];
      } else {
	// silence
	for (int i = 0; i < m; i++)
	  for (unsigned j = 0; j < blocksz; j++)
	    outputs[i][j] = 0.0f;
      }
      return;
    }
    // Handle changes in the polyphony and tuning controls.
    bool is_instr = maxvoices > 0;
    if (is_instr) {
      if (!poly)
	; // this shouldn't happen but...
      else if (nvoices != (int)*poly &&
	       (int)*poly > 0 && (int)*poly <= maxvoices) {
	for (int i = 0; i < nvoices; i++)
	  voice_off(i);
        nvoices = (int)*poly;
	// Reset the voice allocation.
	memset(vd->notes, 0xff, sizeof(vd->notes));
	vd->free_voices.clear();
	vd->n_free = nvoices;
	for (int i = 0; i < nvoices; i++)
	  vd->free_voices.push_back(i);
	vd->used_voices.clear();
	vd->n_used = 0;
      } else
	*poly = nvoices;
#if FAUST_MTS
      if (tuning && tuning_no != (int)*tuning) change_tuning((int)*tuning);
#endif
    }
    // Only update the controls (of all voices simultaneously) if a port value
    // actually changed. This is necessary to allow MIDI controllers to modify
    // the values for individual MIDI channels (see processEvents below). Also
    // note that this will be done *after* processing the MIDI controller data
    // for the current audio block, so manual inputs can still override these.
    for (int i = 0; i < n_in; i++) {
      int j = inctrls[i], k = ui[0]->elems[j].port;
      float &oldval = portvals[k], newval = *ports[k];
      if (newval != oldval) {
	if (is_instr) {
	  // instrument: update running voices
	  for (boost::circular_buffer<int>::iterator it =
		 vd->used_voices.begin();
	       it != vd->used_voices.end(); it++) {
	    int i = *it;
	    *ui[i]->elems[j].zone = newval;
	  }
	} else {
	  // simple effect: here we only have a single dsp instance
	  *ui[0]->elems[j].zone = newval;
	}
	// also update the MIDI controller data for all channels (manual
	// control input is always omni)
	for (int ch = 0; ch < 16; ch++)
	  midivals[ch][k] = newval;
	// record the new value
	oldval = newval;
      }
    }
    // Initialize the output buffers.
    if (n_samples < blocksz) {
      // We need to enlarge the buffers. We're not officially allowed to do
      // this here (presumably in the realtime thread), but since we usually
      // don't know the hosts's block size beforehand, there's really nothing
      // else that we can do. Let's just hope that doing this once suffices,
      // then hopefully noone will notice.
      if (outbuf) {
	for (int i = 0; i < m; i++) {
	  outbuf[i] = (float*)realloc(outbuf[i],
				      blocksz*sizeof(float));
	  assert(outbuf[i]);
	}
      }
      n_samples = blocksz;
    }
    if (outbuf) {
      // Polyphonic instrument: Mix the voices down to one signal.
      for (int i = 0; i < m; i++)
	for (unsigned j = 0; j < blocksz; j++)
	  outputs[i][j] = 0.0f;
      for (int l = 0; l < nvoices; l++) {
	// Let Faust do all the hard work.
	dsp[l]->compute(blocksz, inputs, outbuf);
	for (int i = 0; i < m; i++)
	  for (unsigned j = 0; j < blocksz; j++)
	    outputs[i][j] += outbuf[i][j];
      }
    } else {
      // Simple effect: We can write directly to the output buffer.
      dsp[0]->compute(blocksz, inputs, outputs);
    }
    // Finally grab the passive controls and write them back to the
    // corresponding control ports. NOTE: Depending on the plugin
    // architecture, this might require a host call to get the control GUI
    // updated in real-time (if the host supports this at all).
    // FIXME: It's not clear how to aggregate the data of the different
    // voices. We compute the maximum of each control for now.
    for (int i = 0; i < n_out; i++) {
      int j = outctrls[i], k = ui[0]->elems[j].port;
      float *z = ui[0]->elems[j].zone;
      *ports[k] = *z;
      for (int l = 1; l < nvoices; l++) {
	float *z = ui[l]->elems[j].zone;
	if (*ports[k] < *z)
	  *ports[k] = *z;
      }
    }
    // Keep track of the last gates set for each voice, so that voices can be
    // forcibly retriggered if needed.
    if (gate >= 0)
      for (int i = 0; i < nvoices; i++)
	vd->lastgate[i] =
	  *ui[i]->elems[gate].zone;
  }

  // This processes just a single MIDI message, so to process an entire series
  // of MIDI events you'll have to loop over the event data in the plugin's
  // MIDI callback. XXXTODO: Sample-accurate processing of MIDI events.
  
  void process_midi(unsigned char *data, int sz)
  {
#if DEBUG_MIDI
    fprintf(stderr, "midi ev (%d bytes):", sz);
    for (int i = 0; i < sz; i++)
      fprintf(stderr, " 0x%0x", data[i]);
    fprintf(stderr, "\n");
#endif
    uint8_t status = data[0] & 0xf0, chan = data[0] & 0x0f;
    bool is_instr = maxvoices > 0;
    switch (status) {
    case 0x90: {
      if (!is_instr) break;
      // note on
#if DEBUG_NOTES
      fprintf(stderr, "note-on  chan %d, note %d, vel %d\n", chan+1,
	      data[1], data[2]);
#endif
      if (data[2] == 0) goto note_off;
      alloc_voice(chan, data[1], data[2]);
      break;
    }
    case 0x80: {
      if (!is_instr) break;
      // note off
#if DEBUG_NOTES
      fprintf(stderr, "note-off chan %d, note %d, vel %d\n", chan+1,
	      data[1], data[2]);
#endif
      note_off:
      dealloc_voice(chan, data[1], data[2]);
      break;
    }
    case 0xe0: {
      if (!is_instr) break;
      // pitch bend
      // data[1] is LSB, data[2] MSB, range is 0..0x3fff (which maps to
      // -2..+2 semitones by default), center point is 0x2000 = 8192
      int val = data[1] | (data[2]<<7);
      vd->bend[chan] =
	(val-0x2000)/8192.0f*vd->range[chan];
#if DEBUG_MIDICC
      fprintf(stderr, "pitch-bend (chan %d): %g cent\n", chan+1,
	      vd->bend[chan]*100.0);
#endif
      update_voices(chan);
      break;
    }
    case 0xb0: {
      // controller change
      switch (data[1]) {
      case 120: case 123:
	if (!is_instr) break;
	// all-sound-off and all-notes-off controllers (these are treated
	// the same in the current implementation)
	all_notes_off(chan);
#if DEBUG_MIDICC
	fprintf(stderr, "all-notes-off (chan %d)\n", chan+1);
#endif
	break;
      case 121:
	// all-controllers-off (in the current implementation, this just
	// resets the RPN-related controllers)
	data_msb[chan] = data_lsb[chan] = 0;
	rpn_msb[chan] = rpn_lsb[chan] = 0x7f;
#if DEBUG_MIDICC
	fprintf(stderr, "all-controllers-off (chan %d)\n", chan+1);
#endif
	break;
      case 101: case 100:
	// RPN MSB/LSB
	if (data[1] == 101)
	  rpn_msb[chan] = data[2];
	else
	  rpn_lsb[chan] = data[2];
	break;
      case 6: case 38:
	// data entry coarse/fine
	if (data[1] == 6)
	  data_msb[chan] = data[2];
	else
	  data_lsb[chan] = data[2];
	goto rpn;
      case 96: case 97:
	// data increment/decrement
	/* NOTE: The specification of these controllers is a complete
	   mess. Originally, the MIDI specification didn't have anything
	   to say about their exact behaviour at all. Nowadays, the
	   behaviour depends on which RPN or NRPN is being modified, which
	   is also rather confusing. Fortunately, as we only handle RPNs
	   0..2 here anyway, it's sufficient to assume the MSB for RPN #2
	   (channel coarse tuning) and the LSB otherwise. */
	if (rpn_msb[chan] == 0 && rpn_lsb[chan] == 2) {
	  // modify the MSB
	  if (data[1] == 96 && data_msb[chan] < 0x7f)
	    data_msb[chan]++;
	  else if (data[1] == 97 && data_msb[chan] > 0)
	    data_msb[chan]--;
	} else {
	  // modify the LSB
	  if (data[1] == 96 && data_lsb[chan] < 0x7f)
	    data_lsb[chan]++;
	  else if (data[1] == 97 && data_lsb[chan] > 0)
	    data_lsb[chan]--;
	}
      rpn:
	if (!is_instr) break;
	if (rpn_msb[chan] == 0) {
	  switch (rpn_lsb[chan]) {
	  case 0:
	    // pitch bend range, coarse value is in semitones, fine value
	    // in cents
	    vd->range[chan] = data_msb[chan]+
	      data_lsb[chan]/100.0;
#if DEBUG_RPN
	    fprintf(stderr, "pitch-bend-range (chan %d): %g cent\n", chan+1,
		    vd->range[chan]*100.0);
#endif
	    break;
	  case 1:
	    {
	      // channel fine tuning (14 bit value, range -100..+100 cents)
	      int value = (data_msb[chan]<<7) |
		data_lsb[chan];
	      vd->fine[chan] = (value-8192)/8192.0f;
	    }
	    goto master_tune;
	  case 2:
	    // channel coarse tuning (only msb is used, range -64..+63
	    // semitones)
	    vd->coarse[chan] = data_msb[chan]-64;
	  master_tune:
	    vd->tune[chan] = vd->coarse[chan]+
	      vd->fine[chan];
#if DEBUG_RPN
	    fprintf(stderr, "master-tuning (chan %d): %g cent\n", chan+1,
		    vd->tune[chan]*100.0);
#endif
	    update_voices(chan);
	    break;
	  default:
	    break;
	  }
	}
	break;
      default: {
#if FAUST_MIDICC
	// interpret all other controller changes according to the MIDI
	// controller map defined in the Faust plugin itself
	std::map<uint8_t,int>::iterator it = ctrlmap.find(data[1]);
	if (it != ctrlmap.end()) {
	  // defined MIDI controller
	  int j = inctrls[it->second],
	    k = ui[0]->elems[j].port;
	  float val = ctrlval(ui[0]->elems[j], data[2]);
	  midivals[chan][k] = val;
	  if (is_instr) {
	    // instrument: update running voices on this channel
	    for (boost::circular_buffer<int>::iterator it =
		   vd->used_voices.begin();
		 it != vd->used_voices.end(); it++) {
	      int i = *it;
	      if (vd->note_info[i].ch == chan)
		*ui[i]->elems[j].zone = val;
	    }
	  } else {
	    // simple effect: here we only have a single dsp instance and
	    // we're operating in omni mode, so we just update the control no
	    // matter what the midi channel is
	    *ui[0]->elems[j].zone = val;
	  }
#if DEBUG_MIDICC
	  fprintf(stderr, "ctrl-change chan %d, ctrl %d, val %d\n", chan+1,
		  data[1], data[2]);
#endif
	}
#endif
	break;
      }
      }
      break;
    }
    default:
      break;
    }
  }

  // Process an MTS sysex message and update the control values accordingly.

  void process_sysex(uint8_t *data, int sz)
  {
    if (!data || sz < 2) return;
#if DEBUG_MIDI
    fprintf(stderr, "midi sysex (%d bytes):", sz);
    for (int i = 0; i < sz; i++)
      fprintf(stderr, " 0x%0x", data[i]);
    fprintf(stderr, "\n");
#endif
    if (data[0] == 0xf0) {
      // Skip over the f0 and f7 status bytes in case they are included in the
      // dump.
      data++; sz--;
      if (data[sz-1] == 0xf7) sz--;
    }
    if ((data[0] == 0x7e || data[0] == 0x7f) && data[2] == 8) {
      // MIDI tuning standard
      bool realtime = data[0] == 0x7f;
      if ((sz == 19 && data[3] == 8) ||
	  (sz == 31 && data[3] == 9)) {
	// MTS scale/octave tuning 1- or 2-byte form
	bool onebyte = data[3] == 8;
	unsigned chanmsk = (data[4]<<14) | (data[5]<<7) | data[6];
	for (int i = 0; i < 12; i++) {
	  float t;
	  if (onebyte)
	    t = (data[i+7]-64)/100.0;
	  else
	    t = (((data[2*i+7]<<7)|data[2*i+8])-8192)/8192.0;
	  for (uint8_t ch = 0; ch < 16; ch++)
	    if (chanmsk & (1<<ch))
	      vd->tuning[ch][i] = t;
	}
	if (realtime) {
	  for (uint8_t ch = 0; ch < 16; ch++)
	    if (chanmsk & (1<<ch)) {
	      // update running voices on this channel
	      update_voices(ch);
	    }
	}
#if DEBUG_MTS
	fprintf(stderr, "octave-tuning-%s (chan ",
		realtime?"realtime":"non-realtime");
	bool first = true;
	for (uint8_t i = 0; i < 16; )
	  if (chanmsk & (1<<i)) {
	    uint8_t j;
	    for (j = i+1; j < 16 && (chanmsk&(1<<j)); )
	      j++;
	    if (first)
	      first = false;
	    else
	      fprintf(stderr, ",");
	    if (j > i+1)
	      fprintf(stderr, "%u-%u", i+1, j);
	    else
	      fprintf(stderr, "%u", i+1);
	    i = j;
	  } else
	    i++;
	fprintf(stderr, "):");
	if (onebyte) {
	  for (int i = 7; i < 19; i++) {
	    int val = data[i];
	    fprintf(stderr, " %d", val-64);
	  }
	} else {
	  for (int i = 7; i < 31; i++) {
	    int val = data[i++] << 7;
	    val |= data[i];
	    fprintf(stderr, " %g", ((double)val-8192.0)/8192.0*100.0);
	  }
	}
	fprintf(stderr, "\n");
#endif
      }
    }
  }

  // Change to a given preloaded tuning. The given tuning number may be in the
  // range 1..PFaustPlugin::n_tunings, zero denotes the default tuning (equal
  // temperament). This is only supported if FAUST_MTS is defined at compile
  // time.

  void change_tuning(int num)
  {
#if FAUST_MTS
    if (!mts || num == tuning_no) return;
    if (num < 0) num = 0;
    if (num > mts->tuning.size())
      num = mts->tuning.size();
    tuning_no = num;
    if (tuning_no > 0) {
      process_sysex(mts->tuning[tuning_no-1].data,
		    mts->tuning[tuning_no-1].len);
    } else {
      memset(vd->tuning, 0, sizeof(vd->tuning));
#if DEBUG_MTS
      fprintf(stderr,
	      "octave-tuning-default (chan 1-16): equal temperament\n");
#endif
    }
#endif
  }

};

Meta *LV2Plugin::meta = 0;
int LV2Plugin::n_tunings = 0;
#if FAUST_MTS
MTSTunings *LV2Plugin::mts = 0;
#endif

/* LV2-specific part starts here. ********************************************/

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
  LV2Plugin* plugin =
    new LV2Plugin(LV2Plugin::numVoices(), (int)rate);
  // Scan host features for URID map.
  for (int i = 0; features[i]; i++) {
    if (!strcmp(features[i]->URI, LV2_URID_URI "#map")) {
      plugin->map = (LV2_URID_Map*)features[i]->data;
      plugin->midi_event =
	plugin->map->map(plugin->map->handle, MIDI_EVENT_URI);
    }
  }
  if (!plugin->map) {
    fprintf
      (stderr, "%s: host doesn't support urid:map, giving up\n",
       PLUGIN_URI);
    delete plugin;
    return 0;
  }
  return (LV2_Handle)plugin;
}

static void
cleanup(LV2_Handle instance)
{
  LV2Plugin* plugin = (LV2Plugin*)instance;
  delete plugin;
}

static void
connect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
  LV2Plugin* plugin = (LV2Plugin*)instance;
  int i = port, k = plugin->ui[0]->nports;
  int n = plugin->dsp[0]->getNumInputs(), m = plugin->dsp[0]->getNumOutputs();
  if (i < k)
    plugin->ports[i] = (float*)data;
  else {
    i -= k;
    if (i < n)
      plugin->inputs[i] = (float*)data;
    else {
      i -= n;
      if (i < m)
	plugin->outputs[i] = (float*)data;
      else if (i == m)
	plugin->event_port = (LV2_Atom_Sequence*)data;
      else if (i == m+1)
	plugin->poly = (float*)data;
      else if (i == m+2)
	plugin->tuning = (float*)data;
      else
	fprintf(stderr, "%s: bad port number %u\n", PLUGIN_URI, port);
    }
  }
}

static void
run(LV2_Handle instance, uint32_t n_samples)
{
  LV2Plugin* plugin = (LV2Plugin*)instance;
  // Process incoming MIDI events.
  if (plugin->event_port) {
    LV2_ATOM_SEQUENCE_FOREACH(plugin->event_port, ev) {
      if (ev->body.type == plugin->midi_event) {
	uint8_t *data = (uint8_t*)(ev+1);
#if 0
	// FIXME: Consider doing sample-accurate note onsets here. LV2 keeps
	// track of the exact onset in the frames and subframes fields
	// (http://lv2plug.in/ns/doc/html/structLV2__Atom__Event.html), but we
	// can't use that information at present, since our gate parameter is
	// a control variable which can only change at block boundaries. In
	// the future, the gate could be implemented as an audio signal to get
	// sample-accurate note onsets.
	uint32_t frames = ev->body.frames;
#endif
	if (data[0] == 0xf0)
	  plugin->process_sysex(data, ev->body.size);
	else
	  plugin->process_midi(data, ev->body.size);
      }
    }
  }
  // Process audio.
  plugin->process_audio(n_samples, plugin->inputs, plugin->outputs);
}

static void
activate(LV2_Handle instance)
{
  LV2Plugin* plugin = (LV2Plugin*)instance;
  plugin->resume();
}

static void
deactivate(LV2_Handle instance)
{
  LV2Plugin* plugin = (LV2Plugin*)instance;
  plugin->suspend();
}

const void*
extension_data(const char* uri)
{
  return NULL;
}

static const LV2_Descriptor descriptor = {
  PLUGIN_URI,
  instantiate,
  connect_port,
  activate,
  run,
  deactivate,
  cleanup,
  extension_data
};

extern "C"
LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &descriptor;
  default:
    return NULL;
  }
}

//----------------------------------------------------------------------------
//  Dynamic manifest
//----------------------------------------------------------------------------

// NOTE: If your LV2 host doesn't offer this extension then you'll have to
// create a static ttl file with the descriptions of the ports. You can do
// this by compiling this code to a standalone executable. Running the
// executable then prints the manifest on stdout.

extern "C"
LV2_SYMBOL_EXPORT
int lv2_dyn_manifest_open(LV2_Dyn_Manifest_Handle *handle,
			  const LV2_Feature *const *features)
{
  LV2Plugin* plugin =
    new LV2Plugin(LV2Plugin::numVoices(), 48000);
  *handle = (LV2_Dyn_Manifest_Handle)plugin;
  return 0;
}

extern "C"
LV2_SYMBOL_EXPORT
int lv2_dyn_manifest_get_subjects(LV2_Dyn_Manifest_Handle handle,
				  FILE *fp)
{
  fprintf(fp, "@prefix lv2:  <http://lv2plug.in/ns/lv2core#> .\n\
<%s> a lv2:Plugin .\n", PLUGIN_URI);
  return 0;
}

#include <string>
#include <ctype.h>

static string mangle(const string &s)
{
  string t = s;
  size_t n = s.size();
  for (size_t i = 0; i < n; i++)
    if ((i == 0 && !isalpha(t[i]) && t[i] != '_') ||
	(!isalnum(t[i]) && t[i] != '_'))
      t[i] = '_';
  return t;
}

static unsigned steps(float min, float max, float step)
{
  if (step == 0.0) return 1;
  int n = (max-min)/step;
  if (n < 0) n = -n;
  if (n == 0) n = 1;
  return n;
}

#if FAUST_META
static bool is_xmlstring(const char *s)
{
  // This is just a basic sanity check. The string must not contain any
  // (unescaped) newlines, carriage returns or double quotes.
  return !strchr(s, '\n') && !strchr(s, '\r') && !strchr(s, '"');
}
#endif

extern "C"
LV2_SYMBOL_EXPORT
int lv2_dyn_manifest_get_data(LV2_Dyn_Manifest_Handle handle,
			      FILE *fp,
			      const char *uri)
{
  LV2Plugin* plugin = (LV2Plugin*)handle;
  int k = plugin->ui[0]->nports;
  int n = plugin->dsp[0]->getNumInputs(), m = plugin->dsp[0]->getNumOutputs();
  bool is_instr = plugin->maxvoices > 0, have_midi = is_instr;
  // Scan the global metadata for plugin name, description, license etc.
  const char *plugin_name = NULL, *plugin_author = NULL, *plugin_descr = NULL,
    *plugin_version = NULL, *plugin_license = NULL;
#if FAUST_META
  plugin_name = plugin->pluginName();
  plugin_descr = plugin->pluginDescription();
  plugin_author = plugin->pluginAuthor();
  plugin_version = plugin->pluginVersion();
  plugin_license = plugin->pluginLicense();
#endif
  if (!plugin_name || !*plugin_name) plugin_name = "kpp_deadgate";
  fprintf(fp, "@prefix doap:  <http://usefulinc.com/ns/doap#> .\n\
@prefix foaf:  <http://xmlns.com/foaf/0.1/> .\n\
@prefix lv2:   <http://lv2plug.in/ns/lv2core#> .\n\
@prefix ui:    <http://lv2plug.in/ns/extensions/ui#> .\n\
@prefix epp:   <http://lv2plug.in/ns/ext/port-props#> .\n\
@prefix atom:  <http://lv2plug.in/ns/ext/atom#> .\n\
@prefix rdf:   <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n\
@prefix rdfs:  <http://www.w3.org/2000/01/rdf-schema#> .\n\
@prefix units: <http://lv2plug.in/ns/extensions/units#> .\n\
<%s>\n\
       a lv2:Plugin%s ;\n\
       doap:name \"%s\" ;\n\
       lv2:binary <kpp_deadgate%s> ;\n\
       lv2:optionalFeature epp:supportsStrictBounds ;\n\
       lv2:optionalFeature lv2:hardRtCapable ;\n", PLUGIN_URI,
	  is_instr?", lv2:InstrumentPlugin":"",
	  plugin_name, DLLEXT);
  if (plugin_author && *plugin_author)
    fprintf(fp, "\
       doap:maintainer [ foaf:name \"%s\" ] ;\n", plugin_author);
  // doap:description just seems to be ignored by all LV2 hosts anyway, so we
  // rather use rdfs:comment now which works with Ardour at least.
  if (plugin_descr && *plugin_descr)
    fprintf(fp, "\
       rdfs:comment \"%s\" ;\n", plugin_descr);
  if (plugin_version && *plugin_version)
    fprintf(fp, "\
       doap:revision \"%s\" ;\n", plugin_version);
  if (plugin_license && *plugin_license)
    fprintf(fp, "\
       doap:license \"%s\" ;\n", plugin_license);
#if FAUST_UI
    fprintf(fp, "\
       ui:ui <%sui> ;\n", PLUGIN_URI);
#endif
  int idx = 0;
  // control ports
  for (int i = 0; i < k; i++, idx++) {
    int j = plugin->ctrls[i];
    assert(idx == plugin->ui[0]->elems[j].port);
    fprintf(fp, "%s [\n", idx==0?"    lv2:port":" ,");
    const char *label = plugin->ui[0]->elems[j].label;
    assert(label);
    string sym = mangle(plugin->ui[0]->elems[j].label);
    switch (plugin->ui[0]->elems[j].type) {
    // active controls (input ports)
    case UI_BUTTON: case UI_CHECK_BUTTON:
    fprintf(fp, "\
	a lv2:InputPort ;\n\
	a lv2:ControlPort ;\n\
	lv2:index %d ;\n\
	lv2:symbol \"%s_%d\" ;\n\
	lv2:name \"%s\" ;\n\
        lv2:portProperty epp:hasStrictBounds ;\n\
        lv2:portProperty lv2:toggled ;\n\
	lv2:default 0.00000 ;\n\
	lv2:minimum 0.00000 ;\n\
	lv2:maximum 1.00000 ;\n", idx, sym.c_str(), idx, label);
      break;
    case UI_NUM_ENTRY: case UI_H_SLIDER: case UI_V_SLIDER:
    fprintf(fp, "\
	a lv2:InputPort ;\n\
	a lv2:ControlPort ;\n\
	lv2:index %d ;\n\
	lv2:symbol \"%s_%d\" ;\n\
	lv2:name \"%s\" ;\n\
        lv2:portProperty epp:hasStrictBounds ;\n\
        epp:rangeSteps %u ;\n\
	lv2:default %g ;\n\
	lv2:minimum %g ;\n\
	lv2:maximum %g ;\n", idx, sym.c_str(), idx, label,
	    steps(plugin->ui[0]->elems[j].min,
		  plugin->ui[0]->elems[j].max,
		  plugin->ui[0]->elems[j].step),
	    plugin->ui[0]->elems[j].init,
	    plugin->ui[0]->elems[j].min,
	    plugin->ui[0]->elems[j].max);
      break;
    // passive controls (output ports)
    case UI_H_BARGRAPH: case UI_V_BARGRAPH:
    fprintf(fp, "\
	a lv2:OutputPort ;\n\
	a lv2:ControlPort ;\n\
	lv2:index %d ;\n\
	lv2:symbol \"%s_%d\" ;\n\
	lv2:name \"%s\" ;\n\
	lv2:default %g ;\n\
	lv2:minimum %g ;\n\
	lv2:maximum %g ;\n", idx, sym.c_str(), idx, label,
	    plugin->ui[0]->elems[j].min,
	    plugin->ui[0]->elems[j].min,
	    plugin->ui[0]->elems[j].max);
      break;
    default:
      assert(0 && "this can't happen");
      break;
    }
    // Scan for Faust control metadata we understand and add corresponding
    // hints to the LV2 description of the port.
    std::map< int, list<strpair> >::iterator it =
      plugin->ui[0]->metadata.find(j);
    if (it != plugin->ui[0]->metadata.end()) {
      for (std::list<strpair>::iterator jt = it->second.begin();
	   jt != it->second.end(); jt++) {
	const char *key = jt->first, *val = jt->second;
#if FAUST_MIDICC
	unsigned num;
	if (!strcmp(key, "midi") && sscanf(val, "ctrl %u", &num) == 1)
	  have_midi = true;
#endif
	if (!strcmp(key, "unit"))
	  fprintf(fp, "\
	units:unit [\n\
            a            units:Unit ;\n\
            units:name   \"%s\" ;\n\
            units:symbol \"%s\" ;\n\
            units:render \"%%f %s\"\n\
	] ;\n", val, val, val);
	if (strcmp(key, "lv2")) continue;
	if (!strcmp(val, "integer"))
	  fprintf(fp, "\
	lv2:portProperty lv2:integer ;\n");
	else if (!strcmp(val, "reportsLatency"))
	  fprintf(fp, "\
	lv2:portProperty lv2:reportsLatency ;\n\
	lv2:designation lv2:latency ;\n");
	else if (!strcmp(val, "hidden") || !strcmp(val, "notOnGUI"))
	  fprintf(fp, "\
	lv2:portProperty epp:notOnGUI ;\n");
	else if (!strncmp(val, "scalepoint", 10) ||
		 !strncmp(val, "scalePoint", 10)) {
	  val += 10;
	  if (!isspace(*val)) continue;
	  char *label = (char*)malloc(strlen(val)+1);
	  float point;
	  int pos;
	  while (sscanf(val, "%s %g%n", label, &point, &pos) == 2) {
	    fprintf(fp, "\
	lv2:scalePoint [ rdfs:label \"%s\"; rdf:value %g ] ;\n",
		    label, point);
	    val += pos;
	  }
	  free(label);
	} else
	  fprintf(stderr, "%s: bad port property '%s:%s'\n", PLUGIN_URI,
		  key, val);
      }
    }
    fprintf(fp, "    ]");
  }
  // audio inputs
  for (int i = 0; i < n; i++, idx++)
    fprintf(fp, "%s [\n\
	a lv2:InputPort ;\n\
	a lv2:AudioPort ;\n\
	lv2:index %d ;\n\
	lv2:symbol \"in%d\" ;\n\
	lv2:name \"in%d\" ;\n\
    ]", idx==0?"    lv2:port":" ,", idx, i, i);
  // audio outputs
  for (int i = 0; i < m; i++, idx++)
    fprintf(fp, "%s [\n\
	a lv2:OutputPort ;\n\
	a lv2:AudioPort ;\n\
	lv2:index %d ;\n\
	lv2:symbol \"out%d\" ;\n\
	lv2:name \"out%d\" ;\n\
    ]", idx==0?"    lv2:port":" ,", idx, i, i);
  if (have_midi) {
    // midi input
    fprintf(fp, "%s [\n\
	a lv2:InputPort ;\n\
	a atom:AtomPort ;\n\
	atom:bufferType atom:Sequence ;\n\
	atom:supports <http://lv2plug.in/ns/ext/midi#MidiEvent> ;\n\
	lv2:index %d ;\n\
	lv2:symbol \"midiin\" ;\n\
	lv2:name \"midiin\"\n\
    ]", idx==0?"    lv2:port":" ,", idx);
    idx++;
  }
  if (is_instr) {
    // polyphony control
    fprintf(fp, "%s [\n\
	a lv2:InputPort ;\n\
	a lv2:ControlPort ;\n\
	lv2:index %d ;\n\
	lv2:symbol \"polyphony\" ;\n\
	lv2:name \"polyphony\" ;\n\
        lv2:portProperty epp:hasStrictBounds ;\n\
#       lv2:portProperty epp:expensive ;\n\
        lv2:portProperty lv2:integer ;\n\
        epp:rangeSteps %d ;\n\
	lv2:default %d ;\n\
	lv2:minimum 1 ;\n\
	lv2:maximum %d ;\n\
    ]", idx==0?"    lv2:port":" ,", idx, plugin->maxvoices-1,
      plugin->maxvoices>1?plugin->maxvoices/2:1,
      plugin->maxvoices);
    idx++;
#if FAUST_MTS
    if (plugin->n_tunings > 0) {
      // tuning control
      fprintf(fp, "%s [\n\
	a lv2:InputPort ;\n\
	a lv2:ControlPort ;\n\
	lv2:index %d ;\n\
	lv2:symbol \"tuning\" ;\n\
	lv2:name \"tuning\" ;\n\
        lv2:portProperty epp:hasStrictBounds ;\n\
        lv2:portProperty lv2:integer ;\n\
        epp:rangeSteps %d ;\n\
	lv2:default 0 ;\n\
	lv2:minimum 0 ;\n\
	lv2:maximum %d ;\n",
	idx==0?"    lv2:port":" ,", idx, plugin->n_tunings, plugin->n_tunings);
      for (int i = 0; i <= plugin->n_tunings; i++)
	fprintf(fp, "\
	lv2:scalePoint [ rdfs:label \"%s\"; rdf:value %d ] ;\n",
		(i>0)?plugin->mts->tuning[i-1].name:"default", i);
      fprintf(fp, "    ]");
      idx++;
    }
#endif
  }
  fprintf(fp, "\n.\n");
  return 0;
}

extern "C"
LV2_SYMBOL_EXPORT
void lv2_dyn_manifest_close(LV2_Dyn_Manifest_Handle handle)
{
  LV2Plugin* plugin = (LV2Plugin*)handle;
  delete plugin;
}

int main()
{
  LV2_Dyn_Manifest_Handle handle;
  LV2_Feature **features = { NULL };
  int res = lv2_dyn_manifest_open(&handle, features);
  if (res) return res;
  res = lv2_dyn_manifest_get_data(handle, stdout, PLUGIN_URI);
  return res;
}

#endif
