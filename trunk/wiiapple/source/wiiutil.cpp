#include "wiiutil.h"

#include <stdio.h>
#include <ogcsys.h>

int usleep(int t)
{
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = t*1000;
	nanosleep(&ts);
	return 0;
}



void logmsg(const char *s, int n)

	{
		FILE *debug = fopen(DEBUGFILE, "a+");
		if (debug) {
			fprintf(debug, "%s: %d\n", s, n);
			fclose(debug);
		}
	}




