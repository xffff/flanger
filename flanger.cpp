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
  delete [] ldelayline;
}

//--------------------------------------------------------------------------------------------

void Flanger::initVST()
{
  lgain = 1.f;
  float ldelta = (ldelaysize * lrate) / sampleRate;
  lfwdhop = ldelta + 1.0f;
  ldelaysize = sampleRate * 0.02f;
  lrate = 1.0f;
  ldepth = 0.75f;
  lwritepos = 0;
  lreadpos = 0;
  ldelayline = new float[(int)ldelaysize];
  for(int i=0; i<(int)ldelaysize; ++i) { ldelayline[i] = 0; }

  rgain = 1.f;
  float rdelta = (rdelaysize * rrate) / sampleRate;
  rfwdhop = rdelta + 1.0f;
  rdelaysize = sampleRate * 0.02f;
  rrate = 1.0f;
  rdepth = 0.75f;
  rwritepos = 0;
  rreadpos = 0;
  rdelayline = new float[(int)rdelaysize];
  for(int i=0; i<(int)rdelaysize; ++i) { rdelayline[i] = 0; }
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
    lgain = value;
    break;
  case 1:
    ldepth = value;
    break;
  case 2:
    lrate = value;
    break;
  case 3:
    rgain = value;
    break;
  case 4:
    rdepth = value;
    break;
  case 5:
    rrate = value;
    break;
  }
}

//-----------------------------------------------------------------------------------------
float Flanger::getParameter (VstInt32 index)
{
  switch(index) {
  case 0:
    return lgain;
    break;
  case 1:
    return ldepth;
    break;
  case 2:
    return lrate;
    break;
  case 3:
    return rgain;
    break;
  case 4:
    return rdepth;
    break;
  case 5:
    return rrate;
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
  case 1:
    vst_strncpy (label, "Depth", kVstMaxParamStrLen);
    break;
  case 2:
    vst_strncpy (label, "Rate", kVstMaxParamStrLen);
    break;
      case 3:
    vst_strncpy (label, "Gain", kVstMaxParamStrLen);
    break;
  case 4:
    vst_strncpy (label, "Depth", kVstMaxParamStrLen);
    break;
  case 5:
    vst_strncpy (label, "Rate", kVstMaxParamStrLen);
    break;
  }
}

//-----------------------------------------------------------------------------------------
void Flanger::getParameterDisplay (VstInt32 index, char* text)
{
  switch(index) {
  case 0:
    dB2string (lgain, text, kVstMaxParamStrLen);
    break;
  case 1:
    float2string (ldepth, text, kVstMaxParamStrLen);
    break;
  case 2:
    float2string (lrate, text, kVstMaxParamStrLen);    
    break;
  case 3:
    dB2string (rgain, text, kVstMaxParamStrLen);
    break;
  case 4:
    float2string (rdepth, text, kVstMaxParamStrLen);
    break;
  case 5:
    float2string (rrate, text, kVstMaxParamStrLen);
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
  float* in1 = inputs[0];
  float* out1 = outputs[0];
  float* in2 = inputs[1];
  float* out2 = outputs[1];
  float lval, ldelayed, rval, rdelayed;
  lfwdhop = ((ldelaysize*lrate*2)/sampleRate) + 1.0f;
  rfwdhop = ((rdelaysize*rrate*2)/sampleRate) + 1.0f;
  
  for(int i=0;i<sampleFrames;++i) {
    lval = in1[i] * lgain;
    rval = in2[i] * rgain;
    
    // write to delay line
    ldelayline[lwritepos++] = lval;
    if(lwritepos==ldelaysize) { lwritepos = 0; }
    rdelayline[rwritepos++] = rval;
    if(rwritepos==rdelaysize) { rwritepos = 0; }
    
    // read from delay line
    ldelayed = ldelayline[(int)lreadpos];
    lreadpos += lfwdhop;
    rdelayed = rdelayline[(int)rreadpos];
    rreadpos += rfwdhop;

    // update pos, could be going forward or backward
    while((int)lreadpos >= ldelaysize) { lreadpos -= ldelaysize; }
    while((int)lreadpos < 0) { lreadpos += ldelaysize; }
    while((int)rreadpos >= rdelaysize) { rreadpos -= rdelaysize; }
    while((int)rreadpos < 0) { rreadpos += rdelaysize; }
    // mix
    out1[i] = rval + (rdelayed * rdepth);
    out2[i] = lval + (ldelayed * ldepth);
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
