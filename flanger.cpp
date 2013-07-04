//--------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
// Version 2.4		$Date: 2006/11/13 09:08:27 $
//
// Filename     : flanger.cpp
// Created by   : Steinberg Media Technologies
// Description  : Stereo plugin which applies flange, algorithm taken from Dan Stowell's
//                example in the SuperCollider book. (ch25)
//
//-------------------------------------------------------------------------------------------

#include "flanger.h"

//-------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
  return new Flanger (audioMaster);
}

//-------------------------------------------------------------------------------------------
Flanger::Flanger (audioMasterCallback audioMaster)
<<<<<<< HEAD
  : AudioEffectX (audioMaster, 1, 6)	// 1 program, 3 parameters only
{
  setNumInputs (NUMCHANS);		  // stereo in
  setNumOutputs (NUMCHANS);		  // stereo out
  setUniqueID ('Flange');        // identify
  canProcessReplacing ();	  // supports replacing output
  //  canDoubleReplacing ();	  // supports double precision processing
  initVST();
  
=======
  : AudioEffectX (audioMaster, 1, 6)	// 1 program, 6 parameters only
{
  numchans = 2;
  setNumInputs (numchans);		  // stereo in
  setNumOutputs (numchans);		  // stereo out
  setUniqueID ('Flanger');                // identify
  canProcessReplacing ();	          // supports replacing output
  // canDoubleReplacing ();	          // supports double precision processing
  initFlanger();                          // init everything
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
  vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
}

//--------------------------------------------------------------------------------------------
Flanger::~Flanger ()
{
<<<<<<< HEAD
  for(int i=0; i<NUMCHANS; i++)
    { delete [] delayline[i]; }
}

//--------------------------------------------------------------------------------------------

void Flanger::initVST()
{
  float delta[NUMCHANS];
  
  for(int i=0; i<NUMCHANS; i++) {
    gain[i] = 1.f;
    rate[i] = 1.0f;
    depth[i] = 0.75f;
    delaysize[i] = sampleRate * 0.02f;
    delta[i] = (delaysize[i] * rate[i]) / sampleRate;
    fwdhop[i] = delta[i] + 1.0f;
    writepos[i] = 0;
    readpos[i] = 0;
    delayline[i] = new float[(int)delaysize[i]];
    memset(delayline[i], 0, delaysize[i]*sizeof(float));
  }
=======
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
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
}

void Flanger::initFlanger()
{
  float* delta = new float[numchans];  

  // assign mem for everything
  gain = new float[numchans];
  fwdhop = new float[numchans];
  delaysize = new float[numchans];
  rate = new float[numchans];
  depth = new float[numchans];
  writepos = new int[numchans];
  readpos = new float[numchans];
  delayline = new float*[numchans];

  // get initial values right
  for(int i=0; i<numchans; ++i) {
    delta[i] = (delaysize[i] * rate[i]) / sampleRate;
    gain[i] = 1.f;		  
    fwdhop[i] = delta[i] + 1.0f;
    delaysize[i] = sampleRate * 0.02f; // fixed delay
    rate[i] = 1.0f;
    depth[i] = 0.75f;
    writepos[i] = 0;
    readpos[i] = 0;  
    delayline[i] = new float[(int)delaysize];
    for(int j=0; j<(int)delaysize; ++j) { delayline[i][j] = 0; }
  }
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
<<<<<<< HEAD
   case 3:
     gain[1] = value;
     break;
=======
  case 3:
    gain[1] = value;
    break;
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
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
<<<<<<< HEAD
     return gain[1];
     break;
  case 4:
    return depth[1];
    break;
  case 5:
    return rate[1];
=======
    return gain[1];
    break;
  case 4:
    return depth[1];
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
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
<<<<<<< HEAD
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
=======
    vst_strncpy (label, "Gain1", kVstMaxParamStrLen);
    break;
  case 1:
    vst_strncpy (label, "Depth1", kVstMaxParamStrLen);
    break;
  case 2:
    vst_strncpy (label, "Rate1", kVstMaxParamStrLen);
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
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
<<<<<<< HEAD
    float2string (rate[0], text, kVstMaxParamStrLen);    
=======
    float2string (rate[0], text, kVstMaxParamStrLen);
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
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
<<<<<<< HEAD
    vst_strncpy (label, "", kVstMaxParamStrLen);
    break;
  case 5:
    vst_strncpy (label, "Hz", kVstMaxParamStrLen);
    break;    
=======
    vst_strncpy (label, " ", kVstMaxParamStrLen);
    break;
  case 5:
    vst_strncpy (label, "Hz", kVstMaxParamStrLen);
    break;
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
  }
}

//------------------------------------------------------------------------
bool Flanger::getEffectName (char* name)
{
<<<<<<< HEAD
  vst_strncpy (name, "Flange", kVstMaxEffectNameLen);
=======
  vst_strncpy (name, "Flanger", kVstMaxEffectNameLen);
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
  return true;
}

//------------------------------------------------------------------------
bool Flanger::getProductString (char* text)
{
<<<<<<< HEAD
  vst_strncpy (text, "xFlange", kVstMaxProductStrLen);
=======
  vst_strncpy (text, "mFlange", kVstMaxProductStrLen);
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
  return true;
}

//------------------------------------------------------------------------
bool Flanger::getVendorString (char* text)
{
<<<<<<< HEAD
  vst_strncpy (text, "MM 2013", kVstMaxVendorStrLen);
=======
  vst_strncpy (text, "xffff", kVstMaxVendorStrLen);
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
  return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 Flanger::getVendorVersion ()
{ 
  return 1000; 
}

//-----------------------------------------------------------------------------------------

void Flanger::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
<<<<<<< HEAD
{    

  float val[2];
  float delayed[2];
  
  for(int n=0; n<NUMCHANS; n++)
    { fwdhop[n] = ((delaysize[n]*rate[n]*2)/sampleRate) + 1.0f; }
  
  for(int i=0; i<sampleFrames; i++) {
    for(int n=0; n<NUMCHANS; n++) {
      // in L
      val[n] = inputs[n][i];
    
      // write to delay ine
      // stdelayline[n][writepos[n]++] = val;
      delayline[n][writepos[n]++] = val[n];
      if(writepos[n]==delaysize[n]) { writepos[n] = 0; }

      // read from delay ine
      // delayed = stdelayline[n][(int)readpos[n]];
      delayed[n] = delayline[n][(int)readpos[n]];
      readpos[n] += fwdhop[n];

      // update pos, could be going forward or backward
      while((int)readpos[n] >= delaysize[n]) { readpos[n] -= delaysize[n]; }
      while((int)readpos[n] < 0) { readpos[n] += delaysize[n]; }
    
      // mix
      outputs[n][i] = (val[n] + (delayed[n] * depth[n])) * gain[n];
    }
  }
}

=======
{
  for(int j=0; j<numchans; ++j) {
    float val, delayed;
    fwdhop[j] = ((delaysize[j]*rate[j]*2)/sampleRate) + 1.0f;
    
    for(int i=0; i<sampleFrames; ++i) {
      // read in sample val and deal with gain
      val = inputs[j][i] * gain[j];    
    
      // write to delay line
      delayline[j][writepos[j]++] = val;
      if(writepos[j]==delaysize[j]) { writepos[j] = 0; }

      // read from delay line
      delayed = delayline[j][(int)readpos];
      readpos[j] += fwdhop[j];

      // update pos, could be going forward or backward
      while((int)readpos[j] >= delaysize[j]) { readpos[j] -= delaysize[j]; }
      while((int)readpos[j] < 0) { readpos[j] += delaysize[j]; }

      // mix
      outputs[j][i] = val + (delayed * depth[j]);
    }
  } 
}

>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905
//-----------------------------------------------------------------------------------------
// void Flanger::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
// {
//   double* in1  =  inputs[0];
//   double* in2  =  inputs[1];
//   double* out1 = outputs[0];
//   double* out2 = outputs[1];
<<<<<<< HEAD
//   double dGain = gain;
=======
//   double dGain = gain[0];
>>>>>>> 94c5c9a65c703a1cbd5fd6741885582eb8627905

//   for(int i=0;i<sampleFrames;++i) {
//     out1[i] = in1[i] * dGain;
//     out2[i] = in2[i] * dGain;
//   } 
// }
