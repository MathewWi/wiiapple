#pragma once

// configuration file
#define REGISTRY	"fat3:/wiiapple/linapple.conf"
extern FILE * registry;	// our opened file


BOOL    RegLoadString (LPCTSTR,LPCTSTR,BOOL,char**,DWORD);
BOOL    RegLoadValue (LPCTSTR,LPCTSTR,BOOL,DWORD *);
BOOL	RegLoadBool(LPCTSTR,LPCTSTR,BOOL,BOOL *);

void    RegSaveString (LPCTSTR,LPCTSTR,BOOL,LPCTSTR);
void    RegSaveValue (LPCTSTR,LPCTSTR,BOOL,DWORD);
void    RegSaveBool (LPCTSTR,LPCTSTR,BOOL,BOOL);
