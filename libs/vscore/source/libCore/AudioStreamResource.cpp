//============================================================================
// Name        : AudioStreamResource.cpp
// Author      : Enrique Oriol
// Version     :
// Copyright   : Narada Robotics
// Description : Hello World in C++, Ansi-style
//============================================================================
#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <stdlib.h>
#include "AudioStreamResource.hpp"

using namespace std;

namespace VisageSDK
{

	AudioStreamResource::AudioStreamResource()
	{
		audioFile = NULL;
		fullCPUusage = false;
		context = new AudioContext(true);
		bufferSize = STREAMING_BUFFER_SIZE;
		init();
		genBuffersAndSources();

	}


	AudioStreamResource::~AudioStreamResource()
	{

		std::cout << "deleting stream resource" << std::endl;
		release();

		if(context)
		{
			std::cout << "deleting context inside stream resource" << std::endl;
			delete context;
		}
	}




	AudioStreamResource::AudioStreamResource(AudioContext *ctx)
	{
		context = ctx;
		bufferSize = STREAMING_BUFFER_SIZE;
		init();
		genBuffersAndSources();

	}


	AudioStreamResource::AudioStreamResource(char* file)
	{
		context = new AudioContext(true);
		filename = file;
		bufferSize = STREAMING_BUFFER_SIZE;
		init();
		initStreamFromWAV();
		genBuffersAndSources();
	}


	AudioStreamResource::AudioStreamResource(AudioContext *ctx, char* file)
	{
		context = ctx;
		filename = file;
		bufferSize = STREAMING_BUFFER_SIZE;
		init();
		initStreamFromWAV();
		genBuffersAndSources();
	}


	void AudioStreamResource::init()
	{
		//Init audio source and dest.
	//TODO IDEA: bind sourcePos and orientation to 3D model mouth
		// Position of the source sound.
		for(int i=0; i<3; i++)
		{
			SourcePos[i] = 0.0;
			SourceVel[i] = 0.0;
			ListenerPos[i] = 0.0;
			ListenerVel[i] = 0.0;
			ListenerOri[i] = 0.0;
			ListenerOri[i+3] = 0.0;
		}
		ListenerOri[2] = -1.0;
		ListenerOri[4] = 1.0;
		loop = AL_FALSE;
		mute = false;

	}



	//setters

	void AudioStreamResource::setFormat(ALenum form)
	{
		format = form;
	}

	void AudioStreamResource::setSize(ALsizei dim)
	{
		audioSize = dim;
	}

	void AudioStreamResource::setFrequency(ALsizei f)
	{
		freq = f;
	}

	ALuint AudioStreamResource::getBytesPerFrame()
	{
		switch(format)
		{
		case AL_FORMAT_MONO8:
			return 1;

		case AL_FORMAT_STEREO8:
			return 2;

		case AL_FORMAT_MONO16:
			return 2;

		case AL_FORMAT_STEREO16:
			return 4;

		default: return 0;
		}
	}

	void AudioStreamResource::printFormat()
	{
		switch(format)
		{
		case AL_FORMAT_MONO8:
			std::cout << "format MONO 8 bits" << std::endl;
			break;

		case AL_FORMAT_STEREO8:
			std::cout << "format STEREO 8 bits" << std::endl;
			break;

		case AL_FORMAT_MONO16:
			std::cout << "format MONO 16 bits" << std::endl;
			break;

		case AL_FORMAT_STEREO16:
			std::cout << "format STEREO 16 bits" << std::endl;
			break;

		default:
			std::cout << "no format defined!!!!" << std::endl;
			break;
		}
	}




	//getters

	ALuint* AudioStreamResource::getSource()
	{
		return &alSource;
	}

	ALenum AudioStreamResource::getFormat()
	{
		return format;
	}

	ALsizei AudioStreamResource::getSize()
	{
		return audioSize;
	}

	ALsizei AudioStreamResource::getFrequency()
	{
		return freq;
	}

	char* AudioStreamResource::getFilename()
	{
		return filename;
	}


	AudioContext* AudioStreamResource::getContext()
	{
		return context;
	}




	void AudioStreamResource::bindListener()
	{
		alListenerfv(AL_POSITION,    ListenerPos);
		alListenerfv(AL_VELOCITY,    ListenerVel);
		alListenerfv(AL_ORIENTATION, ListenerOri);
	}



	void AudioStreamResource::release()
	{
		alSourceStop(alSource);
		emptyBuffers();
		alDeleteSources(1, &alSource);
		alDeleteBuffers(N_BUFFERS, alBuffersID);

	}




	void AudioStreamResource::bindSourceAtt()
	{

		alSourcef (alSource, AL_PITCH,    1.0f     );

		alSourcef (alSource, AL_GAIN,     1000.0f     );

		alSourcefv(alSource, AL_POSITION, SourcePos);

		alSourcefv(alSource, AL_VELOCITY, SourceVel);

		alSourcef (alSource, AL_ROLLOFF_FACTOR, 0.0);

		//streams should not be looping
		alSourcei (alSource, AL_LOOPING, AL_FALSE);

	}




	ALboolean AudioStreamResource::readHeader()
	{
		//TAKE CARE: asume we are working on little-endian sistem
		long fileSize = 0;
		long byteCounter = 0;
		unsigned char buffer[12];
		int channels, bits;
		ALuint frequency;


		fseek(audioFile, 0, SEEK_END);
		fileSize = ftell(audioFile);
		std::cout << "total file size: " << fileSize << std::endl;
		std::rewind(audioFile);

		//Check header

		//pass through first sub-chunk
		fread(buffer, 1, 12, audioFile);


		//check if we are reading fmt sub-chunk or not
		fread(buffer, 1, 8, audioFile);
		if(buffer[0] != 'f' || buffer[1] != 'm' || buffer[2] != 't' || buffer[3] != ' ')
		{
			fprintf(stderr, "Not 'fmt ' \n");
			closeFile();
			return AL_FALSE;
		}


		//check if WAV has PCM format (default)
		fread(buffer, 1, 2, audioFile);
		if(buffer[1] != 0 || buffer[0] != 1)
		{
			fprintf(stderr, "Not PCM \n");
			closeFile();
			return AL_FALSE;
		}


		//get channel count
		fread(buffer, 1, 2, audioFile);
		channels  = buffer[1]<<8;
		channels |= buffer[0];

		//get sample frequency
		fread(buffer, 1, 4, audioFile);
		frequency  = buffer[3]<<24;
		frequency |= buffer[2]<<16;
		frequency |= buffer[1]<<8;
		frequency |= buffer[0];

		//passing throught bytes per second and block size
		fread(buffer, 1, 6, audioFile);

		//get bit depth. 8 or 16
		fread(buffer, 1, 2, audioFile);
		bits = buffer[1]<<8;
		bits |= buffer[0];

		//define format
		setFormat(0);
		if(bits == 8)
		{
			if(channels == 1)
				setFormat(AL_FORMAT_MONO8);
			else if(channels == 2)
				setFormat(AL_FORMAT_STEREO8);
		}
		else if(bits == 16)
		{
			if(channels == 1)
				setFormat(AL_FORMAT_MONO16);
			else if(channels == 2)
				setFormat(AL_FORMAT_STEREO16);
		}
		else
		{
			fprintf(stderr, "Incompatible format (%d, %d) :(\n", channels, bits);
			closeFile();
			return AL_FALSE;
		}


		//check that next sub-chunk is data
		fread(buffer, 1, 8, audioFile);
		if(buffer[0] != 'd' || buffer[1] != 'a' || buffer[2] != 't' || buffer[3] != 'a')
		{
			//IF it's not, there are more unexpected chunks, we'll find directly the "data" tag.
			std::cout << "not data chunk. let's search directly the data tag" << std::endl;

			fseek(audioFile, -7, SEEK_CUR);
			bool isData = false;
			byteCounter = ftell(audioFile);
			while(!isData && byteCounter < fileSize)
			{
				fread(buffer, 1, 1, audioFile);
				if(buffer[0]=='d')
				{
					fread(buffer,1,3, audioFile);
					if(buffer[0]=='a' && buffer[1]=='t' && buffer[2]=='a')
						isData = true;
					else
						fseek(audioFile, -3, SEEK_CUR);
				}
				byteCounter++;
			}
			if(isData)
				//pass through non-interesting bytes
				fread(buffer,1,4,audioFile);
			else
			{
				closeFile();
				fprintf(stderr, "Not 'data' \n");
				return AL_FALSE;
			}
		}


		headerSize = ftell(audioFile);
		std::cout << "header file size: " << headerSize << std::endl;
		
		audioSize = fileSize - headerSize;
		
		std::cout << "audio file size: " << audioSize << std::endl;

		setFrequency(frequency);


		return AL_TRUE;
	}


	ALboolean AudioStreamResource::initStreamFromWAV()
	{

		context->setContextCurrent();
		bufferIndex = 0;

		if(!openFile())
			//return AL_FALSE; commented by Jordi
			return AL_FALSE;

		readHeader();

		//time to upload a buffer
		update_time_us = 0.5*(1000000/*micros.*/ * bufferSize) / (freq * 2/*bytes Per Sample*/);

		return AL_TRUE;

	}


	ALboolean AudioStreamResource::genBuffersAndSources()
	{
		context->setContextCurrent();

		alGenBuffers(N_BUFFERS, alBuffersID);
		if (alGetError() != AL_NO_ERROR)
		{
			std::cout << "error generating buffers" << std::endl;
		   return AL_FALSE;
		}

		alGenSources(1, &alSource);
        if (alGetError() != AL_NO_ERROR)
		{
			std::cout << "error generating alSource" << std::endl;
            return AL_FALSE;
		}
		bindSourceAtt();
		if (alGetError() != AL_NO_ERROR)
		{
			std::cout << "error binding source att." << std::endl;
		   return AL_FALSE;
		}
	}

    
    int count = 0;

	ALboolean AudioStreamResource::play()
	{
		context->setContextCurrent();
		int err = 0;
        
//        mute = false;
		
		if(mute)
			muteBufferID = 0;
		
		std::cout << "going to play stream" << std::endl;


		if(isPlaying())
		{
			std::cout << "returning because it's still playing" << std::endl;
			return AL_TRUE;
		}

		bufferIndex = 0;
		ALboolean notFinishReading = true;

		//queue up the first N_BUFFERS buffers for first time
		for(int i=0; i< N_BUFFERS; i++)
		{
			std::cout << "n buffer reading: " << i << std::endl;
			if(notFinishReading)
			{
				notFinishReading = loadNextStreamInBuffer(alBuffersID[i]);
				if(!mute)
				{
//					std::cerr << "enqueue buff: " << alBuffersID[i] << std::endl;
					alSourceQueueBuffers(alSource, 1, &alBuffersID[i]);
				}
			}
		}

		
		if(!mute)
		{

			if (alGetError() != AL_NO_ERROR)
			{
				std::cout << "error when loading init streams" << std::endl;
			   return AL_FALSE;
			}

			alSourcePlay(alSource);

			if (alGetError() != AL_NO_ERROR)
			{
				std::cout << "cannot start playing stream" << std::endl;
			   //return AL_FALSE;
				
			}
		}

        
        cerr << "Going to sleep" << endl;
		while(update())
		{
			//std::cout << "update" << std::endl;
//			if(!isPlaying() && !mute) Commented by Jordi
				//cout << "not playing after update" << endl;

		}
        cerr << "Wake from sleep" << endl;
		count = 0;
		
		while(isPlaying() && !mute){
			//cout << "inside playing while" << endl;
            if(update_time_us>10000)//10ms clock resolution (1Hz)
#ifdef WIN32
            	Sleep(update_time_us/1000);
#else
                usleep(update_time_us);
#endif
		}
		
		
		std::cout << "stream played" << std::endl;

		emptyBuffers();

		closeFile();

		return AL_TRUE;
	}


	ALboolean AudioStreamResource::update()
	{
		ALboolean active = AL_TRUE;
		
		if(mute) {
            
            cerr << "count: " << count << endl;
            count++;
            
            active = loadNextStreamInBuffer(muteBufferID);
			
        }
			
		else {
			int err = 0;
			
			int processed = 0;
			
			alGetSourcei(alSource, AL_BUFFERS_PROCESSED, &processed);
			
			//cout << "processed buffers: " << processed << endl;
			
			if(processed==0)
			{
				//	alSourcePlay(alSource);
				//	alGetError();i
                if((update_time_us)>10000){//10ms clock resolution (1Hz)
					#ifdef WIN32
						Sleep(update_time_us/1000);
					#else
						usleep(update_time_us);
					#endif
                }
			}
			
			
			
			while(processed)
			{
				
				//cout << "processed buffers in while: " << processed << endl;
				ALuint buffer = 0;
				
				alSourceUnqueueBuffers(alSource, 1, &buffer);
//				std::cerr << "UNQUEUE buff: " << buffer << std::endl;

				if((err = alGetError()) != AL_NO_ERROR)
				{
					std::cout << "cannot unqueue bufferID: " << buffer << ", error: "<< alGetString(err) << std::endl;
					continue;
				}


				if(!isPlaying())
				{
					std::cout << "audio is not being played inside update, trying to resolve" << std::endl;
					alSourcePlay(alSource);
					if((err = alGetError()) != AL_NO_ERROR)
						std::cout << "cannot play audio " << buffer << ", error: "<< alGetString(err) << std::endl;
					else
					{
						#ifdef WIN32
							Sleep(update_time_us/2000);
						#else
							usleep(update_time_us/2);
						#endif
					}
				}
				
				
				active = loadNextStreamInBuffer(buffer);
				
				
				if(active)
				{
//					std::cerr << "enqueue buff: " << buffer << std::endl;
					alSourceQueueBuffers(alSource, 1, &buffer);
				
					if((err = alGetError()) != AL_NO_ERROR)
					{
						std::cout << "cannot queue buffer, error: "<< alGetString(err) << std::endl;
						return AL_FALSE;
					}
				}
				
				alGetSourcei(alSource, AL_BUFFERS_PROCESSED, &processed);
				
			}
			
		}
		if(!fullCPUusage)
		{
			//std::cout << "sleeping 0.1ms" << std::endl;
			//usleep(100);
		}

//        count = 0;

		return active;
	}



	ALboolean AudioStreamResource::emptyBuffers()
	{
		int queued;
		alGetSourcei(alSource, AL_BUFFERS_QUEUED, &queued);
		while(queued--)
		{
			ALuint buffer;
			alSourceUnqueueBuffers(alSource, 1, &buffer);
//			std::cerr << "UNQUEUE buff: " << buffer << std::endl;

			if (alGetError() != AL_NO_ERROR)
			{
				std::cout << "cannot empty buffer" << std::endl;
			   return AL_FALSE;
			}
		}
		return AL_TRUE;
	}


	int AudioStreamResource::getBufferIdentifier(ALuint buff)
	{
		int pos=0;
		for (int i=0; i< N_BUFFERS; i++)
		{
			if(alBuffersID[i]!=buff)
				pos++;
		}
		if(pos==N_BUFFERS)
		{
			std::cerr << "wrong buffer identifier" << std::endl;
			pos = 0;
		}

		return pos;
	}


	ALboolean AudioStreamResource::loadNextStreamInBuffer(ALuint buffID)
	{

		int totalChunks = audioSize / STREAMING_BUFFER_SIZE;
		if(bufferIndex > totalChunks)
			return AL_FALSE;

		long startOffset = bufferIndex*STREAMING_BUFFER_SIZE;

		if(bufferIndex == totalChunks) //the audioFile does not have to be an exact multiple of bufferIndex
			bufferSize = audioSize - (bufferSize*totalChunks);


		unsigned char *outData = (unsigned char*)malloc(bufferSize*sizeof(unsigned char*));

		if(!readAudioStream((ALCvoid*)outData, bufferSize, startOffset))
		{
			std::cerr << "cannot read audio stream" << std::endl;
		   return AL_FALSE;
		}

		std::cerr << "new buffer loaded!!" << std::endl;

		if(!mute)
		{
			alGetError();
			alBufferData(buffID, format, outData, bufferSize, freq);


			if (alGetError() != AL_NO_ERROR)
			{
				std::cout << "cannot buffer data stream" << std::endl;
			   return AL_FALSE;
			}
		}

		if(outData)
		{
			free(outData);
			outData = NULL;
		}

		bufferIndex++;
		
		if(mute)
		{
			muteBufferID++;
			if(muteBufferID == N_BUFFERS)
				muteBufferID = 0;
		}

		if(bufferIndex > totalChunks)
			return AL_FALSE;//there are no more data to enqueue. IF we want to loop, here check loop var and do bufferIndex = 0 again, insted of return false.

		return AL_TRUE;

	}


	bool AudioStreamResource::readAudioStream(ALCvoid* buff, int bufferSize, int startOffset)
	{

		int byteCounter = 0;
		int offset= headerSize + startOffset;


		fseek(audioFile, offset, SEEK_SET);
		byteCounter = fread(buff, 1, bufferSize, audioFile);


		if(byteCounter != bufferSize)
		{
			std::cout << "unexpected size of stream audio data read" << std::endl;
			return false;
		}

		return true;

	}


//	bool AudioStreamResource::readAudioStream(short* buff, int bufferSize, int startOffset)
//	{
//		std::cout << "read audio stream with shorts" << std::endl;
//
//		int byteCounter = 0;
//		int offset= headerSize + startOffset;
//
//
//		fseek(audioFile, offset, SEEK_SET);
//		byteCounter = fread(buff, 2, bufferSize/2, audioFile);
//
//		if(byteCounter != bufferSize/2)
//		{
//			std::cout << "unexpected size of stream audio data read" << std::endl;
//			std::cout << "read "<< byteCounter << " instead of the expected " << bufferSize/2 << " bytes." << std::endl;
//
//		}
//
//		return true;
//
//	}



	bool AudioStreamResource::isPlaying()
	{
		ALint play;
		alGetSourcei(alSource, AL_SOURCE_STATE, &play);

		return (play == AL_PLAYING);
	}


	void AudioStreamResource::stop()
	{
		alSourceStop(alSource);
	}


	void AudioStreamResource::rewind()
	{
		alSourceRewind(alSource);
	}




	bool AudioStreamResource::openFile()
	{

		audioFile = fopen(filename, "r");
		if(audioFile==NULL)
		{
			std::cout << "cannot open file" << std::endl;
			return false;
		}
		return true;

	}

	void AudioStreamResource::closeFile()
	{
		if(audioFile)
			fclose(audioFile);
	}

}





