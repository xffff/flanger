//---------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// Version 2.4		$Date: 2006/11/13 09:08:27 $
//
// Category     : VST 2.x SDK Samples
// Filename     : flanger.h
// Created by   : Steinberg Media Technologies
// Description  : Stereo plugin which applies Gain [-oo, 0dB]
//
// � 2006, Steinberg Media Technologies, All Rights Reserved
//---------------------------------------------------------------------------------------------

#ifndef __again__
#define __again__

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
	int numchans;
	float *delayline;
	float rate;
	float delaysize;
	float fwdhop;
	float readpos;
	int writepos;
	float *gain;
	float depth;
	char programName[kVstMaxProgNameLen + 1];
};

#endif
