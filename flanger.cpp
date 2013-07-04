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
// � 2006, Steinberg Media Technologies, All Rights Reserved
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
  setNumInputs (2);		  // stereo in
  setNumOutputs (2);		  // stereo out
  setUniqueID ('Flanger');        // identify
  canProcessReplacing ();	  // supports replacing output
  //  canDoubleReplacing ();	  // supports double precision processing
  initVST();
  
  vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
}

//--------------------------------------------------------------------------------------------
Flanger::~Flanger ()
{
  for(int i=0; i<numchans; i++)
     { delete [] stdelayline[i]; }
  delete [] stdelayline;

  delete [] delayline;
  delete [] gain;
  delete [] rate;
  delete [] depth;
  delete [] fwdhop;
  delete [] writepos;
  delete [] readpos;
}

//--------------------------------------------------------------------------------------------

void Flanger::initVST()
{
  float *delta;
  
  gain = new float[numchans];
  rate = new float[numchans];
  depth = new float[numchans];
  delta = new float[numchans];
  fwdhop = new float[numchans];
  writepos = new int[numchans];
  readpos = new float[numchans];
  stdelayline = new float*[numchans];  
  
  for(int i=0; i<numchans; i++) {
    gain[i] = 1.f;
    rate[i] = 1.0f;
    depth[i] = 0.75f;
    delaysize[i] = sampleRate * 0.02f;
    delta[i] = (delaysize[i] * rate[i]) / sampleRate;
    fwdhop[i] = delta[i] + 1.0f;
    writepos[i] = 0;
    readpos[i] = 0;
    
    stdelayline[i] = new float[(int)delaysize[i]];
    for(int j=0; j<(int)delaysize[j]; j++)
      { stdelayline[i][j] = 0; }
  }

  delayline = new float[(int)delaysize[0]];
  for(int j=0; j<(int)delaysize[0]; j++)
    { delayline[j] = 0; }
  
  delete [] delta;
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
    vst_strncpy (label, "L Gain", kVstMaxParamStrLen);
    break;
  case 1:
    vst_strncpy (label, "L Depth", kVstMaxParamStrLen);
    break;
  case 2:
    vst_strncpy (label, "L Rate", kVstMaxParamStrLen);
    break;
  case 3:
    vst_strncpy (label, "R Gain", kVstMaxParamStrLen);
    break;
  case 4:
    vst_strncpy (label, "R Depth", kVstMaxParamStrLen);
    break;
  case 5:
    vst_strncpy (label, "R Rate", kVstMaxParamStrLen);
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
    vst_strncpy (label, "", kVstMaxParamStrLen);
    break;
  case 2:
    vst_strncpy (label, "Hz", kVstMaxParamStrLen);
    break;
  case 3:
    vst_strncpy (label, "dB", kVstMaxParamStrLen);
    break;
  case 4:
    vst_strncpy (label, "", kVstMaxParamStrLen);
    break;
  case 5:
    vst_strncpy (label, "Hz", kVstMaxParamStrLen);
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
  float val, delayed;
  fwdhop[0] = ((delaysize[0]*rate[0]*2)/sampleRate) + 1.0f;
  
  for(int i=0; i<sampleFrames; i++) {
    val = inputs[0][i];
    
    // write to delay line
    // stdelayline[0][writepos[0]++] = val;
    delayline[writepos[0]++] = val;
    if(writepos[0]==delaysize[0]) { writepos[0] = 0; }

    // read from delay ine
    // delayed = stdelayline[0][(int)readpos[0]];
    delayed = delayline[(int)readpos[0]];
    readpos[0] += fwdhop[0];

    // update pos, could be going forward or backward
    while((int)readpos[0] >= delaysize[0]) { readpos[0] -= delaysize[0]; }
    while((int)readpos[0] < 0) { readpos[0] += delaysize[0]; }

    // mix
    outputs[0][i] = (val + (delayed * depth[0])) * gain[0];
    outputs[1][i] = (val + (delayed * depth[1])) * gain[1];
  }
}

//-----------------------------------------------------------------------------------------
// void Flanger::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
// {
//   double* in1  =  inputs[0];
//   double* in2  =  inputs[1];
//   double* out1 = outputs[0];
//   double* out2 = outputs[1];
//   double dGain = gain;

//   for(int i=0;i<sampleFrames;++i) {
//     out1[i] = in1[i] * dGain;
//     out2[i] = in2[i] * dGain;
//   } 
// }
