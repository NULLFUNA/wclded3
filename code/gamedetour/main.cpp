/*
===========================================================================

   File: main.cpp
   Author: Andrey Lysenko (dpq8@yandex.ru)
   Purpose: 
   Creation date: 31.07.2023
   
   https://github.com/Waclendy
   copyright (c) 2023 Andrey Lysenko
   
===========================================================================
*/

#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"

/*
=============
Com_Printf

Both client and server can use this, and it will output
to the appropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void QDECL Com_Printf( const char *fmt, ... ) {

}


/*
================
Com_DPrintf

A Com_Printf that only shows up if the "developer" cvar is set
================
*/
void QDECL Com_DPrintf( const char *fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

#ifdef NDEBUG
	return;
#endif 

	va_start( argptr,fmt );
	Q_vsnprintf( msg, sizeof( msg ), fmt, argptr );
	va_end( argptr );

	Com_Printf( S_COLOR_CYAN "%s", msg );
}


/*
=============
Com_Error

Both client and server can use this, and it will
do the appropriate things.
=============
*/
void QDECL Com_Error( errorParm_t code, const char *fmt, ... ) {

}