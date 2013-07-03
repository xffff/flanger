//--------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// Version 2.4		$Date: 2006/11/13 09:08:27 $
//
// Category     : VST 2.x SDK Samples
// Filename     : flanger.cpp
// Created by   : Steinberg Media Technologies
// Description  : Stereo plugin which applies flange, algorithm taken from Dan Stowell's
//                example in the SuperCollider book. (ch25)
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
  : AudioEffectX (audioMaster, 1, 6)	// 1 program, 3 parameters only
{
  numchans = 2;
  setNumInputs (numchans);		  // stereo in
  setNumOutputs (numchans);		  // stereo out
  setUniqueID ('Flanger');        // identify
  canProcessReplacing ();	  // supports replacing output
  canDoubleReplacing ();	  // supports double precision processing
  float* delta = new float[2];
  gain = new float[2];
  fwdhop = new float[2];
  delaysize = new float[2];
  rate = new float[2];
  depth = new float[2];
  writepos = new int[2];
  readpos = new float[2];
  delayline = new float*[2];
  
  for(int i=0; i<numchans; ++i) {
    delta[i] = (delaysize[i] * rate[i]) / sampleRate;
    gain[i] = 1.f;		  // default to 0 dB
    fwdhop[i] = delta[i] + 1.0f;
    delaysize[i] = sampleRate * 0.02f;
    rate[i] = 1.0f;
    depth[i] = 0.75f;
    writepos[i] = 0;
    readpos[i] = 0;  
    delayline[i] = new float[(int)delaysize];

    // start with 0s
    memset(delayline[i], 0, delaysize[i] * sizeof(float));
  }
  delete [] delta;
  
  vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
}

//--------------------------------------------------------------------------------------------
Flanger::~Flanger ()
{
  for(int i=0; i<numchans; i++) {
    delete [] delayline[i];
    delete [] gain;
    delete [] fwdhop;
    delete [] rate;
    delete [] depth;
    delete [] writepos;
    delete [] readpos;
  }
  
  delete [] delayline;
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
    gain[0] = value;
    break;
  case 1:
    depth[0] = value;
    break;
  case 2:
    rate[0] = value;
    break;
  case 3:
    gain[1] = value;
    break;
  case 4:
    depth[1] = value;
    break;
  case 5:
    rate[1] = value;
    break;
  }
}

//-----------------------------------------------------------------------------------------
float Flanger::getParameter (VstInt32 index)
{
  switch(index) {
  case 0:
    return gain[0];
    break;
  case 1:
    return depth[0];
    break;
  case 2:
    return rate[0];
    break;
  case 3:
    return gain[1];
    break;
  case 4:
    return depth[1];
    break;
  case 5:
    return rate[1];
    break;    
  }
}

//-----------------------------------------------------------------------------------------
void Flanger::getParameterName (VstInt32 index, char* label)
{
  switch(index) {
  case 0:
    vst_strncpy (label, "Gain1", kVstMaxParamStrLen);
    break;
  case 1:
    vst_strncpy (label, "Depth1", kVstMaxParamStrLen);
    break;
  case 2:
    vst_strncpy (label, "Rate1", kVstMaxParamStrLen);
    break;
  case 3:
    vst_strncpy (label, "Gain2", kVstMaxParamStrLen);
    break;
  case 4:
    vst_strncpy (label, "Depth2", kVstMaxParamStrLen);
    break;
  case 5:
    vst_strncpy (label, "Rate2", kVstMaxParamStrLen);
    break;    
  }
}

//-----------------------------------------------------------------------------------------
void Flanger::getParameterDisplay (VstInt32 index, char* text)
{
  switch(index) {
  case 0:
    dB2string (gain[0], text, kVstMaxParamStrLen);
    break;
  case 1:
    float2string (depth[0], text, kVstMaxParamStrLen);
    break;
  case 2:
    float2string (rate[0], text, kVstMaxParamStrLen);
    break;
  case 3:
    dB2string (gain[1], text, kVstMaxParamStrLen);
    break;
  case 4:
    float2string (depth[1], text, kVstMaxParamStrLen);
    break;
  case 5:
    float2string (rate[1], text, kVstMaxParamStrLen);
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
  case 1:
    vst_strncpy (label, " ", kVstMaxParamStrLen);
    break;
  case 2:
    vst_strncpy (label, "Hz", kVstMaxParamStrLen);
    break;
  case 3:
    vst_strncpy (label, "dB", kVstMaxParamStrLen);
    break;
  case 4:
    vst_strncpy (label, " ", kVstMaxParamStrLen);
    break;
  case 5:
    vst_strncpy (label, "Hz", kVstMaxParamStrLen);
    break;
  }
}

//------------------------------------------------------------------------
bool Flanger::getEffectName (char* name)
{
  vst_strncpy (name, "Flanger", kVstMaxEffectNameLen);
  return true;
}

//------------------------------------------------------------------------
bool Flanger::getProductString (char* text)
{
  vst_strncpy (text, "mFlange", kVstMaxProductStrLen);
  return true;
}

//------------------------------------------------------------------------
bool Flanger::getVendorString (char* text)
{
  vst_strncpy (text, "xffff", kVstMaxVendorStrLen);
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
  float val[2];
  float delayed[2];

  for(int j=0; j<numchans; ++j)
    { fwdhop[j] = ((delaysize[j]*rate[j]*2)/sampleRate) + 1.0f; }

  for(int i=0; i<sampleFrames; ++i) {
    for(int j=0; j<numchans; ++j) {
    val[j] = inputs[j][i] * gain[j];    
    
    // write to delay line
    delayline[j][writepos[j]++] = val[j];
    if(writepos[j]==delaysize[j]) { writepos[j] = 0; }

    // read from delay line
    delayed[j] = delayline[j][(int)readpos];
    readpos[j] += fwdhop[j];

    // update pos, could be going forward or backward
    while((int)readpos[j] >= delaysize[j]) { readpos[j] -= delaysize[j]; }
    while((int)readpos[j] < 0) { readpos[j] += delaysize[j]; }

    // mix
    outputs[j][i] = val[j] + (delayed[j] * depth[j]);
    }
  }
}

//-----------------------------------------------------------------------------------------
void Flanger::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{
  double* in1  =  inputs[0];
  double* in2  =  inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];
  double dGain = gain[0];

  for(int i=0;i<sampleFrames;++i) {
    out1[i] = in1[i] * dGain;
    out2[i] = in2[i] * dGain;
  } 
}
