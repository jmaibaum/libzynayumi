/****************************************************************************
    
    VST wrapper for Zynayumi

    vstzynayumi.hpp

    Copyleft (c) 2016 Nil Geisweiller
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#ifndef __ZYNAYUMI_VSTZYNAYUMI_HPP
#define __ZYNAYUMI_VSTZYNAYUMI_HPP

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "zynayumi/zynayumi.hpp"

namespace zynayumi {

class VSTZynayumi : public AudioEffectX
{
public:
	Zynayumi zynayumi;

	VstEvents* events;			// midi events

	VSTZynayumi(audioMasterCallback audioMaster);
	~VSTZynayumi();

	// Process audio and events
	VstInt32 processEvents(VstEvents* events);
	void midi(unsigned char status, unsigned char byte1, unsigned char byte2);
	void process(float **inputs, float **outputs, VstInt32 sampleFrames);
	void processReplacing(float **inputs, float **outputs,
	                      VstInt32 sampleFrames);
	VstIntPtr dispatcher(VstInt32 opCode, VstInt32 index, VstIntPtr value,
	                     void *ptr, float opt);

	// // Access/modify parameter
	void setParameter(VstInt32 index, float value);
	float getParameter(VstInt32 index);
	void getParameterLabel(VstInt32 index, char *label);
	void getParameterName(VstInt32 index, char *text);
	void getParameterDisplay(VstInt32 index, char *text);

	// Plugin name, vendor and version
	bool getEffectName(char* name);
	bool getVendorString(char* text);
	bool getProductString(char* text);
	VstInt32 getVendorVersion();

	VstInt32 canDo(char* text);

	bool getOutputProperties(VstInt32 index, VstPinProperties* properties);

	/** TODO possibly implement the following methods

		void setProgram(VstInt32 program);
		void setProgramName(char *name);
		void getProgramName(char *name);
		void setSampleRate(float sampleRate);
		void setBlockSize(VstInt32 blockSize);
		void suspend();
		void resume();

		bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);
		bool copyProgram (VstInt32 destination);
	*/

private:
	// Midi status codes
	static const unsigned char NOTE_ON = 0x90;
	static const unsigned char NOTE_OFF = 0x80;
	static const unsigned char CONTROL = 0xb0;

	// Control change codes
	static const unsigned char CTL_PORTAMENTO_TIME = 0x05;
	static const unsigned char CTL_ALL_NOTES_OFF = 0x7b;

   void processEvent(VstEvent* event);

	// Intermediary representations between user parameters and patch
	float _tone_detune;
	int _tone_transpose;
	float _ringmode_detune;
	int _ringmode_transpose;
};

} // ~namespace zynayumi

#endif
