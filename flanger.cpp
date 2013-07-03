//--------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// Version 2.4		$Date: 2006/11/13 09:08:27 $
//
// Category     : VST 2.x SDK Samples
// Filename     : flanger.cpp
// Created by   : Steinberg Media Technologies
// Description  : Stereo plugin which applies Gain [-oo, 0dB]
//
// © 2006, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------

#include "flanger.h"

//-------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
  return new Flanger (audioMaster);
}

//-------------------------------------------------------------------------------------------
Flanger::Flanger (audioMasterCallback audioMaster)
  : AudioEffectX (audioMaster, 1, 3)	// 1 program, 2 parameters only
{
  setNumInputs (2);		  // stereo in
  setNumOutputs (2);		  // stereo out
  setUniqueID ('Flanger');        // identify
  canProcessReplacing ();	  // supports replacing output
  canDoubleReplacing ();	  // supports double precision processing
  gain = 1.f;			  // default to 0 dB
  float delta = (delaysize * rate) / sampleRate;
  fwdhop = delta + 1.0f;
  delaysize = sampleRate * 0.02f;
  rate = 1.0f;
  writepos = 0;
  readpos = 0;  
  delayline = new float[(int)delaysize];
  memset(delayline, 0, delaysize * sizeof(float));
  
  vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
}

//--------------------------------------------------------------------------------------------
Flanger::~Flanger ()
{
  // nothing to do here
}

//-------------------------------------------------------------------------------------------
void Flanger::setProgramName (char* name)
{
  vst_strncpy (programName, name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void Flanger::getProgramName (char* name)
{
  vst_strncpy (name, programName, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void Flanger::setParameter (VstInt32 index, float value)
{
  switch(index) {
  case 0:
    gain = value;
    break;
  case 1:
    depth = value;
    break;
  case 2:
    rate = value;
    break;
  }
}

//-----------------------------------------------------------------------------------------
float Flanger::getParameter (VstInt32 index)
{
  switch(index) {
  case 0:
    return gain;
    break;
  case 1:
    return depth;
    break;
  case 2:
    return rate;
    break;
  }
}

//-----------------------------------------------------------------------------------------
void Flanger::getParameterName (VstInt32 index, char* label)
{
  switch(index) {
  case 0:
    vst_strncpy (label, "Gain", kVstMaxParamStrLen);
    break;
  }
}

//-----------------------------------------------------------------------------------------
void Flanger::getParameterDisplay (VstInt32 index, char* text)
{
  switch(index) {
  case 0:
    dB2string (gain, text, kVstMaxParamStrLen);
    break;
  }
}

//-----------------------------------------------------------------------------------------
void Flanger::getParameterLabel (VstInt32 index, char* label)
{
  switch(index) {
  case 0:
    vst_strncpy (label, "dB", kVstMaxParamStrLen);
    break;
  }
}

//------------------------------------------------------------------------
bool Flanger::getEffectName (char* name)
{
  vst_strncpy (name, "Gain", kVstMaxEffectNameLen);
  return true;
}

//------------------------------------------------------------------------
bool Flanger::getProductString (char* text)
{
  vst_strncpy (text, "Gain", kVstMaxProductStrLen);
  return true;
}

//------------------------------------------------------------------------
bool Flanger::getVendorString (char* text)
{
  vst_strncpy (text, "Steinberg Media Technologies", kVstMaxVendorStrLen);
  return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 Flanger::getVendorVersion ()
{ 
  return 1000; 
}

//-----------------------------------------------------------------------------------------
void Flanger::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
  float* in1  =  inputs[0];
  float* in2  =  inputs[1];
  float* out1 = outputs[0];
  float* out2 = outputs[1];
  float val, delayed;
  fwdhop = ((delaysize*rate*2)/sampleRate) + 1.0f;

  for(int i=0;i<sampleFrames;++i) {
    val = in1[i] * gain;

    // write to delay line
    delayline[writepos++] = val;
    if(writepos==delaysize) { writepos = 0; }

    // read from delay line
    delayed = delayline[(int)readpos];
    readpos += fwdhop;

    // update pos, could be going forward or backward
    while((int)readpos >= delaysize) { readpos -= delaysize; }
    while((int)readpos < 0) { readpos += delaysize; }

    // mix
    out1[i] = val + (delayed * depth);
  }
}

//-----------------------------------------------------------------------------------------
void Flanger::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{
  double* in1  =  inputs[0];
  double* in2  =  inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];
  double dGain = gain;

  for(int i=0;i<sampleFrames;++i) {
    out1[i] = in1[i] * dGain;
    out2[i] = in2[i] * dGain;
  } 
}
