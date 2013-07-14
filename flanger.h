//---------------------------------------------------------------------------------------------
//
// Filename     : flanger.h
// Created by   : Michael Murphy
// Description  : Stereo Flanger
//
//---------------------------------------------------------------------------------------------

#ifndef __again__
#define __again__
#define NUMCHANS 2

#include "public.sdk/source/vst2.x/audioeffectx.h"

//---------------------------------------------------------------------------------------------
class Flanger : public AudioEffectX
{
public:
	Flanger (audioMasterCallback audioMaster);
	~Flanger ();

	// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	// virtual void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames);

	// Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);

	// Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();

protected:
	void initVST();	
	float *delayline[NUMCHANS];
	float rate[NUMCHANS];
	float delaysize[NUMCHANS];
	float fwdhop[NUMCHANS];
	float readpos[NUMCHANS];
	int writepos[NUMCHANS];
	float gain[NUMCHANS];
	float depth[NUMCHANS];
	char programName[kVstMaxProgNameLen + 1];
};

#endif
