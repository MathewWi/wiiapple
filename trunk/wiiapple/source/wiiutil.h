#ifndef WII_UTIL
#define WII_UTIL

int usleep(int t);
inline unsigned short efix(unsigned short x)
{
    return (x>>8) | 
        (x<<8);
}

inline unsigned int lfix(unsigned int x)
{
   return (x>>24) | 
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
}

#define DEBUGFILE "fat3:/debug.txt"

void logmsg(const char *s, int n);

#define TRACE do { logmsg (__FILE__, __LINE__); } while (0)

#endif
