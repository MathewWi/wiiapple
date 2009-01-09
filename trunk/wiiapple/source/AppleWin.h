#pragma once

#ifdef _WIN32
#define FILE_SEPARATOR	TEXT('\\')
#else
#define FILE_SEPARATOR	TEXT('/')
#endif


// let it be out first version!
#define LINAPPLE_VERSION	1
extern char VERSIONSTRING[];	// Contructed in WinMain()

extern const TCHAR     *g_pAppTitle;

extern eApple2Type	g_Apple2Type;

extern BOOL       behind;
extern DWORD      cumulativecycles;
extern DWORD      cyclenum;
extern DWORD      emulmsec;
extern bool       g_bFullSpeed;

// Win32
//extern HINSTANCE  g_hInstance;


extern AppMode_e g_nAppMode;

extern DWORD      needsprecision;
//extern TCHAR      g_sProgramDir[MAX_PATH];
extern TCHAR      g_sCurrentDir[MAX_PATH];
extern TCHAR      g_sHDDDir[MAX_PATH];
extern TCHAR      g_sSaveStateDir[MAX_PATH];
extern TCHAR      g_sParallelPrinterFile[MAX_PATH];


extern bool       g_bResetTiming;
extern BOOL       restart;

extern DWORD      g_dwSpeed;
extern double     g_fCurrentCLK6502;

extern int        g_nCpuCyclesFeedback;
extern DWORD      g_dwCyclesThisFrame;

extern FILE*      g_fh;				// Filehandle for log file
extern bool       g_bDisableDirectSound;	// Cmd line switch: don't init DS (so no MB support)

extern UINT		g_Slot4;	// Mockingboard or Mouse in slot4

void    SetCurrentCLK6502();
