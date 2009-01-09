/* Include the SDL main definition header */
#include "SDL_main.h"
#undef main

/* Standard includes */
#include <stdio.h>

/* SDL includes */
#include "../../video/gamecube/SDL_gamecubevideo.h"

/* OGC includes */
#include <ogcsys.h>
#include <wiiuse/wpad.h>

/* Globals */
GXRModeObj*			display_mode			= 0;
GameCube_Y1CBY2CR	(*frame_buffer)[][320]	= 0;

extern void gamecube_keyboard_init();

/* Do initialisation which has to be done first for the console to work */
static void GAMECUBE_Initialize(void)
{
	//printf("GAMECUBE_Initialize ENTER\n");

	/* Initialise the video system */
	VIDEO_Init();

	display_mode = VIDEO_GetPreferredMode(NULL);

	WPAD_Init();
	PAD_Init();

	WPAD_SetDataFormat(0, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(0, 640, 480);

	/* Allocate the frame buffer */
	frame_buffer = (GameCube_Y1CBY2CR (*)[][320])(MEM_K0_TO_K1(SYS_AllocateFramebuffer(display_mode)));

	/* Set up the video system with the chosen mode */
	VIDEO_Configure(display_mode);

	/* Set the frame buffer */
	VIDEO_SetNextFramebuffer(frame_buffer);

	// Show the screen.
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (display_mode->viTVMode & VI_NON_INTERLACE)
	{
		VIDEO_WaitVSync();
	}

	// Initialise the debug console.
	console_init(frame_buffer,20,20,display_mode->fbWidth,display_mode->xfbHeight,display_mode->fbWidth*VI_DISPLAY_PIX_SZ);

	gamecube_keyboard_init();
	
	//printf("GAMECUBE_Initialize EXIT\n");
}

/* Entry point */
int main(int argc, char *argv[])
{
	printf("main ENTER\n");

	/* Set up the screen mode */
	GAMECUBE_Initialize();

	/* Call the user's main function */
	return(SDL_main(argc, argv));
	
	printf("main EXIT\n");
}

/* This function isn't implemented */
/*int unlink(const char* file_name)
{
	return -1;
}
*/
