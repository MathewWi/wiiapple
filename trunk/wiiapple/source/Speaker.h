#pragma once
//
extern DWORD      soundtype;
extern double     g_fClksPerSpkrSample;

// needed for DSPlaySnd callback function
extern short 	* pDSSpkrBuf;	// speaker data buffer (in size of g_dwDSSpkrBufferSize samples?
extern DWORD g_dwDSSpkrBufferSize;	// size of Speakers audio buffer
extern UINT	nDSSpkrWCur;	// write cursor
extern UINT	nDSSpkrRCur;	// read cursor


void    SpkrDestroy ();
void    SpkrInitialize ();
void    SpkrReinitialize ();
void    SpkrReset();
BOOL    SpkrSetEmulationType (/*HWND,*/DWORD);
void    SpkrUpdate (DWORD);
//void    SpkrUpdate_Timer();
DWORD   SpkrGetVolume();
void    SpkrSetVolume(DWORD dwVolume, DWORD dwVolumeMax);
void    Spkr_Mute();
void    Spkr_Demute();
bool    Spkr_IsActive();
bool    Spkr_DSInit();
void    Spkr_DSUninit();
DWORD   SpkrGetSnapshot(SS_IO_Speaker* pSS);
DWORD   SpkrSetSnapshot(SS_IO_Speaker* pSS);

BYTE SpkrToggle (WORD pc, WORD addr, BYTE bWrite, BYTE d, ULONG nCyclesLeft);
