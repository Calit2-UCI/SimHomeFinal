//============================================================================
// Name        : AudioContext.hpp
// Author      : Enrique Oriol
// Version     :
// Copyright   : Narada Robotics
// Description : Hello World in C++, Ansi-style
//============================================================================


#ifndef __HPP_HEADER__AUDIOCONTEXT_HPP
#define __HPP_HEADER__AUDIOCONTEXT_HPP

	//OpenAL libraries
	//OpenAL libraries
	#ifdef ECLIPSE_MAC
		#include <al.h>
		#include <alc.h>
    #endif
    #if defined(MAC) || defined(IPHONE)
        #include <OpenAL/al.h>
        #include <OpenAL/alc.h>
    #endif
	#ifdef LINUX
		#include <AL/al.h>
		#include <AL/alc.h>
	#endif
	#ifdef WIN32
		#include <al.h>
		#include <alc.h>
	#endif

	//other libs


#include "AudioResource.hpp"





namespace VisageSDK
{
	class AudioResource;



	class AudioContext{


	public:

		AudioContext(bool stream);

		~AudioContext();

		void initAudioContext(bool stream);

		ALboolean loadWAVonContext(AudioResource *audio);

		ALboolean loadCaptureOnContext(AudioResource *audio);

		ALboolean bindAudioToBuffer(AudioResource *audio);

		void setContextCurrent();

		void deleteContext();



	private:

        static int numAudioResources;

		static ALCdevice *audioDevice;
		static ALCcontext *alContext;
		ALuint alBuffer;

	};
    
}


#endif

