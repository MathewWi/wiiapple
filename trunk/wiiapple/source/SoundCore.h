#pragma once

// Define max 1 of these:
//#define RIFF_SPKR
//#define RIFF_MB

enum {FADE_OUT, FADE_IN };

bool DSInit();		// init SDL_Auidio
void DSUninit();	// uninit SDL_Auidio
void DSSndPlay(void * mydata, Uint8 *stream, int len); // callback func for playing sound

void SoundCore_SetFade(int how);	//

double DSUploadBuffer(short* buffer, unsigned len);
void   DSUploadMockBuffer(short* buffer, unsigned len);	// Upload Mockingboard data
//LONG NewVolume(DWORD dwVolume, DWORD dwVolumeMax);

extern bool g_bDSAvailable;

void SysClk_WaitTimer();
bool SysClk_InitTimer();
void SysClk_UninitTimer();
void SysClk_StartTimerUsec(DWORD dwUsecPeriod);
void SysClk_StopTimer();

