/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken at libsdl.or
*/
#include "SDL_config.h"

// Public includes.
#include "SDL_timer.h"

// Audio internal includes.
#include "SDL_audio.h"
#include "../SDL_audiomem.h"
#include "../SDL_sysaudio.h"
#include "../SDL_audio_c.h"

// GameCube audio internal includes.
#include <ogc/audio.h>
#include <ogc/cache.h>
#include "SDL_gamecubeaudio.h"

#define ALIGNED(x) __attribute__((aligned(x)));
//#define SAMPLES_PER_DMA_BUFFER 8192
#define SAMPLES_PER_DMA_BUFFER 2048

typedef Uint32 Sample;
typedef Sample DMABuffer[SAMPLES_PER_DMA_BUFFER];

static const char	GAMECUBEAUD_DRIVER_NAME[] = "gamecube";
static DMABuffer	dma_buffers[2] ALIGNED(32);
static size_t		current_dma_buffer = 0;
static Sample		silence[SAMPLES_PER_DMA_BUFFER] ALIGNED(32);

// Called whenever more audio data is required.
static void StartDMA(void)
{
	// Is the device ready?
	if (current_audio && (!current_audio->paused))
	{
		DMABuffer* const dma_buffer = &dma_buffers[current_dma_buffer];

		// Is conversion required?
		if (current_audio->convert.needed)
		{
			// Dump out the conversion info.
			// printf("----\n");
			// printf("conversion is needed\n");
			// printf("\tsrc_format = 0x%x\n", current_audio->convert.src_format);
			// printf("\tdst_format = 0x%x\n", current_audio->convert.dst_format);
			// printf("\trate_incr  = %f\n", (float) current_audio->convert.rate_incr);
			// printf("\tbuf        = 0x%08x\n", current_audio->convert.buf);
			// printf("\tlen        = %d\n", current_audio->convert.len);
			// printf("\tlen_cvt    = %d\n", current_audio->convert.len_cvt);
			// printf("\tlen_mult   = %d\n", current_audio->convert.len_mult);
			// printf("\tlen_ratio  = %f\n", (float) current_audio->convert.len_ratio);

			//SDL_mutexP(this->mixer_lock);
			// Get the client to produce audio.
			current_audio->spec.callback(
				current_audio->spec.userdata,
				current_audio->convert.buf,
				current_audio->convert.len);
			//SDL_mutexV(this->mixer_lock);

			// Convert the audio.
			SDL_ConvertAudio(&current_audio->convert);

			// Sanity check.
			if (sizeof(DMABuffer) != current_audio->convert.len_cvt)
			{
				// printf("The size of the DMA buffer (%u) doesn't match the converted buffer (%u)\n",
				//	sizeof(DMABuffer), current_audio->convert.len_cvt);
			}

			// Copy from SDL buffer to DMA buffer.
			memcpy(&(*dma_buffer)[0], current_audio->convert.buf, current_audio->convert.len_cvt);
		}
		else
		{
			// printf("conversion is not needed\n");
			current_audio->spec.callback(
				current_audio->spec.userdata,
				&(*dma_buffer)[0], sizeof(DMABuffer) );

			//memset(&(*dma_buffer)[0], 0, sizeof(DMABuffer));
		}


		AUDIO_StopDMA();
		// Set up the DMA.
		AUDIO_InitDMA((Uint32) &(*dma_buffer)[0], sizeof(DMABuffer));

		// Flush the data cache.
		DCFlushRange(&(*dma_buffer)[0], sizeof(DMABuffer));

		// Use the other DMA buffer next time.
		current_dma_buffer = 1 - current_dma_buffer;
	}
	else
	{
		AUDIO_StopDMA();
		// Set up the DMA.
		AUDIO_InitDMA((Uint32) silence, sizeof(silence));
		DCFlushRange(silence, sizeof(silence));
	}

	// Start the DMA.
	AUDIO_StartDMA();
}

static int GAMECUBEAUD_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	// Set up actual spec.
	spec->freq		= 32000;
	spec->format	= AUDIO_S16MSB;
	spec->channels	= 2;
	spec->samples	= SAMPLES_PER_DMA_BUFFER;
	spec->padding	= 0;
	SDL_CalculateAudioSpec(spec);

	// Initialise the GameCube side of the audio system.
	AUDIO_Init(0);
	AUDIO_SetDSPSampleRate(AI_SAMPLERATE_32KHZ);
	AUDIO_RegisterDMACallback(StartDMA);

	// Start the first chunk of audio playing.
	StartDMA();

	// Magic number which indicates that an audio thread is not required.
	//return 2;

	return 1;
}

static void GAMECUBEAUD_CloseAudio(_THIS)
{
	// Forget the DMA callback.
	AUDIO_RegisterDMACallback(0);

	// Stop any DMA going on.
	AUDIO_StopDMA();
}

static void GAMECUBEAUD_DeleteDevice(SDL_AudioDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_AudioDevice *GAMECUBEAUD_CreateDevice(int devindex)
{
	SDL_AudioDevice *this;

	/* Initialize all variables that we clean on shutdown */
	this = (SDL_AudioDevice *)SDL_malloc(sizeof(SDL_AudioDevice));
	if ( this ) {
		SDL_memset(this, 0, (sizeof *this));
		this->hidden = (struct SDL_PrivateAudioData *)
				SDL_malloc((sizeof *this->hidden));
	}
	if ( (this == NULL) || (this->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( this ) {
			SDL_free(this);
		}
		return(0);
	}
	SDL_memset(this->hidden, 0, (sizeof *this->hidden));

	/* Set the function pointers */
	this->OpenAudio = GAMECUBEAUD_OpenAudio;
	this->CloseAudio = GAMECUBEAUD_CloseAudio;
	this->free = GAMECUBEAUD_DeleteDevice;

	return this;
}

static int GAMECUBEAUD_Available(void)
{
	return 1;
}

AudioBootStrap GAMECUBEAUD_bootstrap = {
	GAMECUBEAUD_DRIVER_NAME, "SDL GameCube audio driver",
	GAMECUBEAUD_Available, GAMECUBEAUD_CreateDevice
};
