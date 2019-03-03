/* ------------------------------------------------------------
author: "Oleg Kapitonov"
license: "GPLv3"
name: "kpp_tubeamp"
version: "1.0"
Code generated with Faust 2.5.23 (https://faust.grame.fr)
Compilation options: cpp, -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

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
	float fVec0[2];
	float fRec9[2];
	float fVec1[2];
	float fRec8[2];
	float fVec2[2];
	float fRec7[2];
	float fConst6;
	float fConst7;
	float fConst8;
	float fRec6[3];
	float fRec5[3];
	float fRec4[3];
	float fVec3[2];
	float fRec3[2];
	float fVec4[2];
	float fRec10[2];
	float fVec5[2];
	float fRec2[2];
	float fRec1[2];
	float fVec6[2];
	float fRec0[2];
	
 public:

  stPorts ports;

	void metadata(Meta* m) { 
		m->declare("author", "Oleg Kapitonov");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("filename", "kpp_tubeamp");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("license", "GPLv3");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "kpp_tubeamp");
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
		fConst1 = (1.0f / tanf((34557.5195f / fConst0)));
		fConst2 = (fConst1 + 1.0f);
		fConst3 = (0.0f - ((1.0f - fConst1) / fConst2));
		fConst4 = (1.0f / fConst2);
		fConst5 = (3.14159274f / fConst0);
		fConst6 = (1.0f / fConst0);
		fConst7 = (3.14159274f * fConst6);
		fConst8 = (6.28318548f * fConst6);
		
	}
	
	virtual void instanceResetUserInterface() {
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fVec0[l0] = 0.0f;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec9[l1] = 0.0f;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fVec1[l2] = 0.0f;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec8[l3] = 0.0f;
			
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fVec2[l4] = 0.0f;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec7[l5] = 0.0f;
			
		}
		for (int l6 = 0; (l6 < 3); l6 = (l6 + 1)) {
			fRec6[l6] = 0.0f;
			
		}
		for (int l7 = 0; (l7 < 3); l7 = (l7 + 1)) {
			fRec5[l7] = 0.0f;
			
		}
		for (int l8 = 0; (l8 < 3); l8 = (l8 + 1)) {
			fRec4[l8] = 0.0f;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fVec3[l9] = 0.0f;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec3[l10] = 0.0f;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fVec4[l11] = 0.0f;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec10[l12] = 0.0f;
			
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fVec5[l13] = 0.0f;
			
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec2[l14] = 0.0f;
			
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec1[l15] = 0.0f;
			
		}
		for (int l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			fVec6[l16] = 0.0f;
			
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec0[l17] = 0.0f;
			
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
		ui_interface->openVerticalBox("kpp_tubeamp");
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = (VOLUME_CTRL * OUTPUT_LEVEL);
		float fSlow1 = tanf((fConst5 * HIGH_FREQ_CTRL));
		float fSlow2 = (2.0f * (1.0f - (1.0f / mydsp_faustpower2_f(fSlow1))));
		float fSlow3 = tanf((fConst5 * MIDDLE_FREQ_CTRL));
		float fSlow4 = (2.0f * (1.0f - (1.0f / mydsp_faustpower2_f(fSlow3))));
		float fSlow5 = tanf((fConst5 * LOW_FREQ_CTRL));
		float fSlow6 = (2.0f * (1.0f - (1.0f / mydsp_faustpower2_f(fSlow5))));
		float fSlow7 = ((powf(10.0f, (0.0199999996f * MASTERGAIN_CTRL)) + -1.0f) * AMP_LEVEL);
		float fSlow8 = (0.0f - PREAMP_UPOR_CTRL);
		float fSlow9 = ((powf(10.0f, (0.0199999996f * DRIVE_CTRL)) + -1.0f) * PREAMP_LEVEL);
		float fSlow10 = (1.0f / fSlow5);
		int iSlow11 = (LOW_CTRL > 0.0f);
		float fSlow12 = sinf((fConst8 * LOW_FREQ_CTRL));
		float fSlow13 = (fConst7 * ((LOW_BAND_CTRL * powf(10.0f, (0.0500000007f * fabsf(LOW_CTRL)))) / fSlow12));
		float fSlow14 = (fConst7 * (LOW_BAND_CTRL / fSlow12));
		float fSlow15 = (iSlow11?fSlow14:fSlow13);
		float fSlow16 = ((fSlow10 * (fSlow10 - fSlow15)) + 1.0f);
		float fSlow17 = ((fSlow10 * (fSlow10 + fSlow15)) + 1.0f);
		float fSlow18 = (iSlow11?fSlow13:fSlow14);
		float fSlow19 = ((fSlow10 * (fSlow10 + fSlow18)) + 1.0f);
		float fSlow20 = (1.0f - (fSlow10 * (fSlow18 - fSlow10)));
		float fSlow21 = (1.0f / fSlow3);
		int iSlow22 = (MIDDLE_CTRL > 0.0f);
		float fSlow23 = sinf((fConst8 * MIDDLE_FREQ_CTRL));
		float fSlow24 = (fConst7 * ((MIDDLE_BAND_CTRL * powf(10.0f, (0.0500000007f * fabsf(MIDDLE_CTRL)))) / fSlow23));
		float fSlow25 = (fConst7 * (MIDDLE_BAND_CTRL / fSlow23));
		float fSlow26 = (iSlow22?fSlow25:fSlow24);
		float fSlow27 = ((fSlow21 * (fSlow21 - fSlow26)) + 1.0f);
		float fSlow28 = ((fSlow21 * (fSlow21 + fSlow26)) + 1.0f);
		float fSlow29 = (iSlow22?fSlow24:fSlow25);
		float fSlow30 = ((fSlow21 * (fSlow21 + fSlow29)) + 1.0f);
		float fSlow31 = (1.0f - (fSlow21 * (fSlow29 - fSlow21)));
		float fSlow32 = (1.0f / fSlow1);
		int iSlow33 = (HIGH_CTRL > 0.0f);
		float fSlow34 = sinf((fConst8 * HIGH_FREQ_CTRL));
		float fSlow35 = (fConst7 * ((HIGH_BAND_CTRL * powf(10.0f, (0.0500000007f * fabsf(HIGH_CTRL)))) / fSlow34));
		float fSlow36 = (fConst7 * (HIGH_BAND_CTRL / fSlow34));
		float fSlow37 = (iSlow33?fSlow36:fSlow35);
		float fSlow38 = ((fSlow32 * (fSlow32 - fSlow37)) + 1.0f);
		float fSlow39 = ((fSlow32 * (fSlow32 + fSlow37)) + 1.0f);
		float fSlow40 = (iSlow33?fSlow35:fSlow36);
		float fSlow41 = ((fSlow32 * (fSlow32 + fSlow40)) + 1.0f);
		float fSlow42 = (1.0f - (fSlow32 * (fSlow40 - fSlow32)));
		float fSlow43 = (1.0f / tanf((fConst5 * SAG_TIME)));
		float fSlow44 = (fSlow43 + 1.0f);
		float fSlow45 = (0.0f - ((1.0f - fSlow43) / fSlow44));
		float fSlow46 = (1.0f / fSlow44);
		for (int i = 0; (i < count); i = (i + 1)) {
			float fTemp0 = float(input0[i]);
			float fTemp1 = float(input1[i]);
			fVec0[0] = (fTemp0 + fTemp1);
			fRec9[0] = ((((0.995000005f * fRec9[1]) + fTemp0) + fTemp1) - fVec0[1]);
			float fTemp2 = (fSlow9 * fRec9[0]);
			fVec1[0] = fTemp2;
			fRec8[0] = ((fConst3 * fRec8[1]) + (fConst4 * (fTemp2 + fVec1[1])));
			float fTemp3 = (max((PREAMP_KREG_CTRL * (fRec8[0] - PREAMP_UPOR_CTRL)), 0.0f) + 1.0f);
			float fTemp4 = max(fSlow8, (PREAMP_BIAS_CTRL + ((fRec8[0] / fTemp3) - (PREAMP_UPOR_CTRL * ((1.0f / fTemp3) + -1.0f)))));
			fVec2[0] = fTemp4;
			fRec7[0] = (((0.995000005f * fRec7[1]) + fTemp4) - fVec2[1]);
			float fTemp5 = (fSlow6 * fRec6[1]);
			fRec6[0] = ((fSlow7 * fRec7[0]) - (((fRec6[2] * fSlow16) + fTemp5) / fSlow17));
			float fTemp6 = (fSlow4 * fRec5[1]);
			fRec5[0] = ((((fTemp5 + (fRec6[0] * fSlow19)) + (fRec6[2] * fSlow20)) / fSlow17) - (((fRec5[2] * fSlow27) + fTemp6) / fSlow28));
			float fTemp7 = (fSlow2 * fRec4[1]);
			fRec4[0] = ((((fTemp6 + (fRec5[0] * fSlow30)) + (fRec5[2] * fSlow31)) / fSlow28) - (((fRec4[2] * fSlow38) + fTemp7) / fSlow39));
			float fTemp8 = (((fTemp7 + (fRec4[0] * fSlow41)) + (fRec4[2] * fSlow42)) / fSlow39);
			fVec3[0] = fTemp8;
			fRec3[0] = ((fConst3 * fRec3[1]) + (fConst4 * (fTemp8 + fVec3[1])));
			float fTemp9 = mydsp_faustpower2_f(fRec1[1]);
			fVec4[0] = fTemp9;
			fRec10[0] = ((fSlow45 * fRec10[1]) + (fSlow46 * (fTemp9 + fVec4[1])));
			float fTemp10 = min(2.5f, max(1.0f, (SAG_COEFF * fRec10[0])));
			float fTemp11 = (fRec3[0] * fTemp10);
			float fTemp12 = (max((AMP_KREG_CTRL * (fTemp11 - AMP_UPOR_CTRL)), 0.0f) + 1.0f);
			float fTemp13 = (max((AMP_KREG_CTRL * (0.0f - (AMP_UPOR_CTRL + fTemp11))), 0.0f) + 1.0f);
			float fTemp14 = (max(0.0f, (AMP_BIAS_CTRL + ((fTemp11 / fTemp12) - (AMP_UPOR_CTRL * ((1.0f / fTemp12) + -1.0f))))) - max(0.0f, (AMP_BIAS_CTRL + (((0.0f - fTemp11) / fTemp13) - (AMP_UPOR_CTRL * ((1.0f / fTemp13) + -1.0f))))));
			fVec5[0] = fTemp14;
			fRec2[0] = ((fConst3 * fRec2[1]) + (fConst4 * (fTemp14 + fVec5[1])));
			fRec1[0] = (fRec2[0] / fTemp10);
			float fTemp15 = (fSlow0 * fRec1[0]);
			fVec6[0] = fTemp15;
			fRec0[0] = (((0.995000005f * fRec0[1]) + fTemp15) - fVec6[1]);
			output0[i] = FAUSTFLOAT(fRec0[0]);
			output1[i] = FAUSTFLOAT(fRec0[0]);
			fVec0[1] = fVec0[0];
			fRec9[1] = fRec9[0];
			fVec1[1] = fVec1[0];
			fRec8[1] = fRec8[0];
			fVec2[1] = fVec2[0];
			fRec7[1] = fRec7[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			fVec3[1] = fVec3[0];
			fRec3[1] = fRec3[0];
			fVec4[1] = fVec4[0];
			fRec10[1] = fRec10[0];
			fVec5[1] = fVec5[0];
			fRec2[1] = fRec2[0];
			fRec1[1] = fRec1[0];
			fVec6[1] = fVec6[0];
			fRec0[1] = fRec0[0];
			
		}
		
	}

	
};

#endif
