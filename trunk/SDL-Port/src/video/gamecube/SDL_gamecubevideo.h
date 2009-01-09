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
    slouken@libsdl.org
*/
#include "SDL_config.h"

#ifndef _SDL_gamecubevideo_h
#define _SDL_gamecubevideo_h

/* SDL internal includes */
#include "../SDL_sysvideo.h"

/* OGC includes */
#include <ogc/gx_struct.h>

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_VideoDevice *this

/* The GameCube uses this frame buffer format */
typedef Uint32 GameCube_Y1CBY2CR;

/* A single palette entry */
typedef struct GameCube_YCBCR
{
	Uint8 y;
	Uint8 cb;
	Uint8 cr;
} GameCube_YCBCR;

/* Types */
typedef GameCube_YCBCR GameCube_Palette[256];
typedef struct { GameCube_Y1CBY2CR entries[256][256]; } GameCube_PackedPalette;
typedef void (GAMECUBE_UpdateRowFn)(const void*, const void*, GameCube_Y1CBY2CR*, const GameCube_PackedPalette*);

/* Private display data */
struct SDL_PrivateVideoData
{
	Uint8*					back_buffer;
	GameCube_Palette		palette;
	GameCube_PackedPalette	packed_palette;
	GAMECUBE_UpdateRowFn*	update_row;
	unsigned int			magnification;
};

/* Globals */
extern GXRModeObj*			display_mode;
extern GameCube_Y1CBY2CR	(*frame_buffer)[][320];

#endif /* _SDL_gamecubevideo_h */
