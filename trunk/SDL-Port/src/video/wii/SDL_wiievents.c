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

#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"
#include <wiiuse/wpad.h>

#include "SDL_wiivideo.h"
#include "SDL_wiievents_c.h"

int lastX = 0;
int lastY = 0;
Uint8 lastButtonStateA = SDL_RELEASED;
Uint8 lastButtonStateB = SDL_RELEASED;
Uint8 lastButtonStateHome = SDL_RELEASED;

static SDLKey keymap[512];

static s32 stat;

typedef struct
{
	u32 message;
	u32 id; // direction
	u8 modifiers;
	u8 unknown;
	u8 keys[6];
	u8 pad[16];
} key_data_t;


static key_data_t key_data ATTRIBUTE_ALIGN(32);

static u8 prev_keys[6];

static key_data_t key_data1,key_data2;

static int keyboard_kb=-1;

static int keyboard_stop = 1;

typedef enum
{
	KEYBOARD_PRESSED = 0,
	KEYBOARD_RELEASED,
	KEYBOARD_DISCONNECTED,
	KEYBOARD_CONNECTED
}keyboard_eventType;


typedef struct _KeyboardEvent{
	int type;
	int modifiers;
	int scancode;
} keyboardEvent;


static keyboardEvent ke;

typedef struct _node
{
	lwp_node node;
	keyboardEvent event;
}node;


lwp_queue *queue;

//Add an event to the event queue
s32 KEYBOARD_addEvent(int type, int scancode, int modifiers)
{
	node *n = (node *)malloc(sizeof(node));
	n->event.type = type;
	n->event.scancode = scancode;
	n->event.modifiers= modifiers;
	__lwp_queue_append(queue,(lwp_node*)n);
	return 1;
}

//Get the first event of the event queue
s32 KEYBOARD_getEvent(keyboardEvent* event)
{
	node *n = (node*) __lwp_queue_get(queue);
	if (!n)
		return 0;
	*event = n->event;
	return 1;
}


SDLMod to_SDL_Modifiers(int km)
{
	SDLMod m = SDL_GetModState() & (KMOD_CAPS|KMOD_NUM);
	if (km & 1) m |= KMOD_LCTRL;
	if (km & 2) m |= KMOD_LSHIFT;
	if (km & 4) m |= KMOD_LALT;
	if (km & 8) m |= KMOD_LMETA;
	if (km & 0x10) m |= KMOD_RCTRL;
	if (km & 0x20) m |= KMOD_RSHIFT;	
	if (km & 0x40) m |= KMOD_RALT;	
	if (km & 0x80) m |= KMOD_RMETA;	
	return m;
}


s32 keyboard_callback(int ret,void * none)
{ 
	int i, j;
	if(keyboard_kb>=0) {
		if(key_data.message!=0x7fffffff) {		
			if(key_data.message==2)  {				
				for (i =0; i<6; i++) {
					if (key_data.keys[i]) {
						int found = false;
						for (j =0; j<6; j++) {
							if (prev_keys[j]==key_data.keys[i]) {
								found = true;
								break;
							}
						}
						if (!found) KEYBOARD_addEvent(KEYBOARD_PRESSED, keymap[key_data.keys[i]], key_data.modifiers);
					}
				}
				
				for (i =0; i<6; i++) {
					if (prev_keys[i]) {
						int found = false;
						for (j =0; j<6; j++) {
							if (prev_keys[i]==key_data.keys[j]) {
								found = true;
								break;
							}
						}
						if (!found) KEYBOARD_addEvent(KEYBOARD_RELEASED, keymap[prev_keys[i]], key_data.modifiers);
					}
				}
				memcpy(prev_keys, key_data.keys, 6);
			} 


			key_data.message=0x7fffffff;
			if (!keyboard_stop) 
				IOS_IoctlAsync(keyboard_kb,1,(void *) &key_data, 16,(void *) &key_data, 16, keyboard_callback, NULL);
		}
	}

	return 0;
}

void initkeymap()
{
	int i;


	for ( i=0; i<SDL_arraysize(keymap); ++i )
		keymap[i] = SDLK_UNKNOWN;
	//a-z
	for (i = 0; i<27; i++) {
		keymap[4+i] = SDLK_a + i;
	}
	
	//numbers
	for (i = 1; i<10; i++) {
		keymap[30 + i - 1] = SDLK_1 + i - 1;
	}
	keymap[39] = SDLK_0;
	keymap[40] = SDLK_RETURN;
	keymap[41] = SDLK_ESCAPE;
	keymap[42] = SDLK_BACKSPACE;
	keymap[43] = SDLK_TAB;
	keymap[44] = SDLK_SPACE;
	keymap[45] = SDLK_MINUS;
	keymap[46] = SDLK_EQUALS;
	keymap[47] = SDLK_LEFTBRACKET;
	keymap[47] = SDLK_RIGHTBRACKET;	
	keymap[49] = SDLK_BACKSLASH;
	keymap[51] = SDLK_SEMICOLON;
	keymap[52] = SDLK_QUOTE;
	keymap[53] = SDLK_BACKQUOTE;
	keymap[54] = SDLK_COMMA;
	keymap[55] = SDLK_PERIOD;
	keymap[56] = SDLK_SLASH;
	keymap[57] = SDLK_CAPSLOCK;


	//F1 to F12
	for (i = 1; i<13; i++) {
		keymap[58 + i - 1] = SDLK_F1 + i - 1;
	}	      

	keymap[72] = SDLK_PAUSE;
	keymap[73] = SDLK_INSERT;
	keymap[74] = SDLK_HOME;
	keymap[75] = SDLK_PAGEUP;	
	keymap[76] = SDLK_DELETE;
	keymap[77] = SDLK_END;
	keymap[78] = SDLK_PAGEDOWN;	

	keymap[79] = SDLK_RIGHT;
	keymap[80] = SDLK_LEFT;
	keymap[81] = SDLK_DOWN;
	keymap[82] = SDLK_UP;
	keymap[83] = SDLK_NUMLOCK;

	keymap[84] = SDLK_KP_DIVIDE;
	keymap[85] = SDLK_KP_MULTIPLY;
	keymap[86] = SDLK_KP_MINUS;
	keymap[87] = SDLK_KP_PLUS;

	keymap[87] = SDLK_KP_ENTER;
	
	
	//keypad numbers
	for (i = 1; i<10; i++) {
		keymap[89 + i - 1] = SDLK_KP1 + i - 1;
	}
	keymap[98] = SDLK_KP0;

}




static int keyboard_initialized =0;

void wii_keyboard_init()
{
	if (!keyboard_initialized) {
		//printf("init keyboard");
		initkeymap();
		queue = (lwp_queue*)malloc(sizeof(lwp_queue));	
		__lwp_queue_initialize(queue,0,0,0);
		keyboard_kb=IOS_Open("/dev/usb/kbd", 1);
		/*
		if (keyboard_kb>=0) {
			printf("keyboard kb ok\n");
		} else{
			printf("keyboard kb not ok\n");
			}*/
		sleep(2);
		key_data.message=0x0;
		key_data1.id=0;
		key_data2.id=0;
		keyboard_stop = 0;
		if(keyboard_kb>=0) 
			IOS_IoctlAsync(keyboard_kb,1,(void *) &key_data, 16,(void *) &key_data, 16, keyboard_callback, NULL);
		keyboard_initialized  = 1;
	} else {
		if(keyboard_kb>=0) 
			IOS_IoctlAsync(keyboard_kb,1,(void *) &key_data, 16,(void *) &key_data, 16, keyboard_callback, NULL);

	}
}


void WII_PumpEvents(_THIS)
{       
	WPAD_ScanPads();
/*
	if (!keyboard_initialized) {
		wii_keyboard_init();
		keyboard_initialized = 1;
	}
*/
	WPADData *wd = WPAD_Data(0);	


	stat =  KEYBOARD_getEvent(&ke);

	if(wd->ir.valid)
	{
		int x = wd->ir.x;
		int y = wd->ir.y;

		if(lastX != x || lastY != y)
		{
			SDL_PrivateMouseMotion(0, 0, x, y);
			lastX = x;
			lastY = y;
		}
		
		Uint8 stateA = SDL_RELEASED;
		Uint8 stateB = SDL_RELEASED;
		if(wd->btns_h & WPAD_BUTTON_A)
		{
			stateA = SDL_PRESSED;
		}
		if(wd->btns_h & WPAD_BUTTON_B)
		{
			stateB = SDL_PRESSED;
		}
		
		if(stateA != lastButtonStateA)
		{
			lastButtonStateA = stateA;
			SDL_PrivateMouseButton(stateA, SDL_BUTTON_LEFT, x, y);
		}
		if(stateB != lastButtonStateB)
		{
			lastButtonStateB = stateB;
			SDL_PrivateMouseButton(stateB, SDL_BUTTON_RIGHT, x, y);
		}
	}

	Uint8 stateHome = SDL_RELEASED;
	if(wd->btns_h & WPAD_BUTTON_HOME)
	{
		stateHome = SDL_PRESSED;
	}

	if(stateHome != lastButtonStateHome)
	{
		lastButtonStateHome = stateHome;
		/*
		SDL_keysym keysym;
		SDL_memset(&keysym, 0, (sizeof keysym));
		keysym.sym = SDLK_ESCAPE;
		SDL_PrivateKeyboard(stateHome, &keysym);*/

		SDL_Event event;
		event.type = SDL_QUIT;
		SDL_PushEvent(&event);


	}

	if (stat && (ke.type==KEYBOARD_RELEASED || ke.type==KEYBOARD_PRESSED) ) {
		SDL_keysym keysym;
		memset(&keysym, 0, sizeof(keysym));
		Uint8 keystate =  (ke.type==KEYBOARD_PRESSED)?SDL_PRESSED:SDL_RELEASED;
		keysym.sym = ke.scancode;
		SDL_SetModState(to_SDL_Modifiers(ke.modifiers));
		SDL_PrivateKeyboard(keystate, &keysym);       	
	}
}



void WII_InitOSKeymap(_THIS)
{
//	int i;

}

/* end of SDL_nullevents.c ... */

