#ifndef MYFSD_H
#define MYFSD_H

#if defined(_DEBUG) && !defined(DEBUG)
#define DEBUG
#endif



#include <windows.h>
#include <tchar.h>
#include <types.h>
#include <excpt.h>
#include <memory.h>
#include <diskio.h>
#include <fsdmgr.h>


#ifdef UNDER_CE
#define GetSystemTimeAsFileTime(pft) { \
        SYSTEMTIME st; \
        GetSystemTime(&st); \
        SystemTimeToFileTime(&st, pft); \
}
#endif


#define ENTER_BREAK_SCOPE switch(TRUE) { case TRUE:
#define LEAVE_BREAK_SCOPE }

#define INVALID_PTR             ((PVOID)0xFFFFFFFF)

#ifdef DEBUG
#define DEBUGONLY(s)            s
#define RETAILONLY(s)           
#define VERIFYTRUE(c)           DEBUGCHK(c)
#define VERIFYNULL(c)           DEBUGCHK(!(c))
#else
#define DEBUGONLY(s)
#define RETAILONLY(s)           s
#define VERIFYTRUE(c)           c
#define VERIFYNULL(c)           c
#endif

#ifndef ERRFALSE
#define ERRFALSE(exp)           extern char __ERRXX[(exp)!=0]
#endif


/*  Debug-zone stuff
 */

#ifdef DEBUG

#define DEBUGBREAK(cond)         if (cond) DebugBreak(); else
#define DEBUGMSGBREAK(cond,msg)  if (cond) {DEBUGMSG(TRUE,msg); DebugBreak();} else
#define DEBUGMSGWBREAK(cond,msg) if (cond) {DEBUGMSGW(TRUE,msg); DebugBreak();} else

/* debug zones */
#define ZONEID_APIS  0

/* zone masks */
#define ZONEMASK_APIS (1 << ZONEID_APIS)

/* these are used as the first arg to DEBUGMSG */
#define ZONE_APIS   DEBUGZONE(ZONEID_APIS)
#else   // !DEBUG

#define DEBUGBREAK(cond)
#define DEBUGMSGBREAK(cond,msg)
#define DEBUGMSGWBREAK(cond,msg)

#define ZONE_APIS   0

#endif


#define BLOCK_SIZE              512     
#define DEFAULT_VOLUME_NAME  TEXT("Fake FSD")


typedef struct
{
    HVOL hVolume;
} VolumeState;


#endif /* MYFSD_H */
