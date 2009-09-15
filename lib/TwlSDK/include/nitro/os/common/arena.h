/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     arena.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_ARENA_H_
#define NITRO_OS_ARENA_H_

#include <nitro/misc.h>
#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------------
//---- arena id
typedef enum
{
    OS_ARENA_MAIN = 0,                 // Main RAM, Main-Processor private
    OS_ARENA_MAIN_SUBPRIV = 1,         // Main RAM, Sub-Processor private
    OS_ARENA_MAINEX = 2,               // Extended Main RAM
    OS_ARENA_ITCM = 3,                 // ITCM
    OS_ARENA_DTCM = 4,                 // DTCM
    OS_ARENA_SHARED = 5,               // Shared Memory
    OS_ARENA_WRAM_MAIN = 6,            // common Work RAM, Main-Processor private
    OS_ARENA_WRAM_SUB = 7,             // common Work RAM, Sub-Processor private
    OS_ARENA_WRAM_SUBPRIV = 8,         // Sub-Processor private Work RAM

    OS_ARENA_MAX = 9
}
OSArenaId;

//---- struct of Arena infomation ( 0x4C byte )
typedef struct
{
    void   *lo[OS_ARENA_MAX];
    void   *hi[OS_ARENA_MAX];
    //u16     initialized;
    //u8      padding[2];
}
OSArenaInfo;

//---- Arena resource
typedef struct OSArenaResource
{
	OSArenaInfo	info;
}
OSArenaResource;

//================================================================================
//        INIT
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitArena

  Description:  Initialize Arena

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_InitArena(void);

/*---------------------------------------------------------------------------*
  Name:         OS_InitArenaEx

  Description:  Initialize extended MainRam Arena 

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef SDK_ARM9
void    OS_InitArenaEx(void);
#endif


//================================================================================
//        GET ARENA INFO
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_GetArenaInfo

  Description:  Get the pointer of Arena structure

  Arguments:    None.

  Returns:      the pointer of Arena structure
 *---------------------------------------------------------------------------*/
void   *OS_GetArenaInfo(void);

/*---------------------------------------------------------------------------*
  Name:         OS_GetArenaHi / OS_GetArenaLo

  Description:  Get the high/low boundary of the arena

  Arguments:    id    : arena ID

  Returns:      the high/low boundary of the arena
 *---------------------------------------------------------------------------*/
void   *OS_GetArenaHi(OSArenaId id);
void   *OS_GetArenaLo(OSArenaId id);

/*---------------------------------------------------------------------------*
  Name:         OS_GetInitArenaHi / OS_GetInitArenaLo

  Description:  Get the initial high/low boundary of the arena

  Arguments:    id    : arena ID

  Returns:      the initial high/low boundary of the arena
 *---------------------------------------------------------------------------*/
void   *OS_GetInitArenaHi(OSArenaId id);
void   *OS_GetInitArenaLo(OSArenaId id);


//================================================================================
//        SET ARENA INFO
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SetArenaHi / OS_SetArenaLo

  Description:  Set the high/low boundary of the arena

  Arguments:    id          : arena ID
                newHi/newLo : New high/low boundary of the arena.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_SetArenaHi(OSArenaId id, void *newHi);
void    OS_SetArenaLo(OSArenaId id, void *newLo);


//================================================================================
//        ALLOC
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_AllocFromArenaLo

  Description:  Allocates /size/ bytes from the low boundary of the arena.
                Some additional memory will also be consumed from arena for
                the alignment.

  Arguments:    id    : arena ID
                size  : size of object to be allocated
                align : alignment of object and new arena boundary to
                        be set (must be power of 2).

  Returns:      None.
 *---------------------------------------------------------------------------*/
void   *OS_AllocFromArenaLo(OSArenaId id, u32 size, u32 align);

/*---------------------------------------------------------------------------*
  Name:         OS_AllocFromArenaHi

  Description:  Allocates /size/ bytes from the high boundary of the arena.
                Some additional memory will also be consumed from arena for
                the alignment.

  Arguments:    id    : arena ID
                size  : Size of object to be allocated
                align : Alignment of object and new arena boundary to
                        be set (must be power of 2).

  Returns:      a pointer to the allocated space aligned with /align/ bytes
                boundaries. The new high boundary of the arena is also
                aligned with /align/ bytes boundaries.
 *---------------------------------------------------------------------------*/
void   *OS_AllocFromArenaHi(OSArenaId id, u32 size, u32 align);


//================================================================================
//         convenient functions
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_[Set/Get/AllocFrom]MainArena[Hi/Lo]

  Description:  convenience functions for access main memory, 
                Main-Processor private
 *---------------------------------------------------------------------------*/
//---- set Arena Hi/Lo address
static inline void OS_SetMainArenaHi(void *newHi)
{
    OS_SetArenaHi(OS_ARENA_MAIN, newHi);
}
static inline void OS_SetMainArenaLo(void *newLo)
{
    OS_SetArenaLo(OS_ARENA_MAIN, newLo);
}

//---- get Arena Hi/Lo address
static inline void *OS_GetMainArenaHi(void)
{
    return OS_GetArenaHi(OS_ARENA_MAIN);
}
static inline void *OS_GetMainArenaLo(void)
{
    return OS_GetArenaLo(OS_ARENA_MAIN);
}

//---- allocate memory from Arena
static inline void *OS_AllocFromMainArenaLo(u32 size, u32 align)
{
    return OS_AllocFromArenaLo(OS_ARENA_MAIN, size, align);
}
static inline void *OS_AllocFromMainArenaHi(u32 size, u32 align)
{
    return OS_AllocFromArenaHi(OS_ARENA_MAIN, size, align);
}

/*---------------------------------------------------------------------------*
  Name:         OS_[Set/Get/AllocFrom]SubPrivArena[Hi/Lo]

  Description:  convenience functions for access main memory, 
                Sub-Processor private
 *---------------------------------------------------------------------------*/
//---- set Arena Hi/Lo address
static inline void OS_SetSubPrivArenaHi(void *newHi)
{
    OS_SetArenaHi(OS_ARENA_MAIN_SUBPRIV, newHi);
}
static inline void OS_SetSubPrivArenaLo(void *newLo)
{
    OS_SetArenaLo(OS_ARENA_MAIN_SUBPRIV, newLo);
}

//---- get Arena Hi/Lo address
static inline void *OS_GetSubPrivArenaHi(void)
{
    return OS_GetArenaHi(OS_ARENA_MAIN_SUBPRIV);
}
static inline void *OS_GetSubPrivArenaLo(void)
{
    return OS_GetArenaLo(OS_ARENA_MAIN_SUBPRIV);
}

//---- allocate memory from Arena
static inline void *OS_AllocFromSubPrivArenaLo(u32 size, u32 align)
{
    return OS_AllocFromArenaLo(OS_ARENA_MAIN_SUBPRIV, size, align);
}
static inline void *OS_AllocFromSubPrivArenaHi(u32 size, u32 align)
{
    return OS_AllocFromArenaHi(OS_ARENA_MAIN_SUBPRIV, size, align);
}

/*---------------------------------------------------------------------------*
  Name:         OS_[Set/Get/AllocFrom]MainExArena[Hi/Lo]

  Description:  convenience functions for extended main memory
 *---------------------------------------------------------------------------*/
//---- set Arena Hi/Lo address
static inline void OS_SetMainExArenaHi(void *newHi)
{
    OS_SetArenaHi(OS_ARENA_MAINEX, newHi);
}
static inline void OS_SetMainExArenaLo(void *newLo)
{
    OS_SetArenaLo(OS_ARENA_MAINEX, newLo);
}

//---- get Arena Hi/Lo address
static inline void *OS_GetMainExArenaHi(void)
{
    return OS_GetArenaHi(OS_ARENA_MAINEX);
}
static inline void *OS_GetMainExArenaLo(void)
{
    return OS_GetArenaLo(OS_ARENA_MAINEX);
}

//---- allocate memory from Arena
static inline void *OS_AllocFromMainExArenaLo(u32 size, u32 align)
{
    return OS_AllocFromArenaLo(OS_ARENA_MAINEX, size, align);
}
static inline void *OS_AllocFromMainExArenaHi(u32 size, u32 align)
{
    return OS_AllocFromArenaHi(OS_ARENA_MAINEX, size, align);
}

/*---------------------------------------------------------------------------*
  Name:         OS_[Set/Get/AllocFrom]ITCMArena[Hi/Lo]

  Description:  convenience functions for ITCM
 *---------------------------------------------------------------------------*/
//---- set Arena Hi/Lo address
static inline void OS_SetITCMArenaHi(void *newHi)
{
    OS_SetArenaHi(OS_ARENA_ITCM, newHi);
}
static inline void OS_SetITCMArenaLo(void *newLo)
{
    OS_SetArenaLo(OS_ARENA_ITCM, newLo);
}

//---- get Arena Hi/Lo address
static inline void *OS_GetITCMArenaHi(void)
{
    return OS_GetArenaHi(OS_ARENA_ITCM);
}
static inline void *OS_GetITCMArenaLo(void)
{
    return OS_GetArenaLo(OS_ARENA_ITCM);
}

//---- allocate memory from Arena
static inline void *OS_AllocFromITCMArenaLo(u32 size, u32 align)
{
    return OS_AllocFromArenaLo(OS_ARENA_ITCM, size, align);
}
static inline void *OS_AllocFromITCMArenaHi(u32 size, u32 align)
{
    return OS_AllocFromArenaHi(OS_ARENA_ITCM, size, align);
}

/*---------------------------------------------------------------------------*
  Name:         OS_[Set/Get/AllocFrom]DTCMArena[Hi/Lo]

  Description:  convenience functions for DTCM
 *---------------------------------------------------------------------------*/
//---- set Arena Hi/Lo address
static inline void OS_SetDTCMArenaHi(void *newHi)
{
    OS_SetArenaHi(OS_ARENA_DTCM, newHi);
}
static inline void OS_SetDTCMArenaLo(void *newLo)
{
    OS_SetArenaLo(OS_ARENA_DTCM, newLo);
}

//---- get Arena Hi/Lo address
static inline void *OS_GetDTCMArenaHi(void)
{
    return OS_GetArenaHi(OS_ARENA_DTCM);
}
static inline void *OS_GetDTCMArenaLo(void)
{
    return OS_GetArenaLo(OS_ARENA_DTCM);
}

//---- allocate memory from Arena
static inline void *OS_AllocFromDTCMArenaLo(u32 size, u32 align)
{
    return OS_AllocFromArenaLo(OS_ARENA_DTCM, size, align);
}
static inline void *OS_AllocFromDTCMArenaHi(u32 size, u32 align)
{
    return OS_AllocFromArenaHi(OS_ARENA_DTCM, size, align);
}

/*---------------------------------------------------------------------------*
  Name:         OS_[Set/Get/AllocFrom]SharedArena[Hi/Lo]

  Description:  convenience functions for shared memory
 *---------------------------------------------------------------------------*/
//---- set Arena Hi/Lo address
static inline void OS_SetSharedArenaHi(void *newHi)
{
    OS_SetArenaHi(OS_ARENA_SHARED, newHi);
}
static inline void OS_SetSharedArenaLo(void *newLo)
{
    OS_SetArenaLo(OS_ARENA_SHARED, newLo);
}

//---- get Arena Hi/Lo address
static inline void *OS_GetSharedArenaHi(void)
{
    return OS_GetArenaHi(OS_ARENA_SHARED);
}
static inline void *OS_GetSharedArenaLo(void)
{
    return OS_GetArenaLo(OS_ARENA_SHARED);
}

//---- allocate memory from Arena
static inline void *OS_AllocFromSharedArenaLo(u32 size, u32 align)
{
    return OS_AllocFromArenaLo(OS_ARENA_SHARED, size, align);
}
static inline void *OS_AllocFromSharedArenaHi(u32 size, u32 align)
{
    return OS_AllocFromArenaHi(OS_ARENA_SHARED, size, align);
}

/*---------------------------------------------------------------------------*
  Name:         OS_[Set/Get/AllocFrom]WramMainArena[Hi/Lo]

  Description:  convenience functions for common Work RAM,
                Main-Processor private
 *---------------------------------------------------------------------------*/
//---- set Arena Hi/Lo address
static inline void OS_SetWramMainArenaHi(void *newHi)
{
    OS_SetArenaHi(OS_ARENA_WRAM_MAIN, newHi);
}
static inline void OS_SetWramMainArenaLo(void *newLo)
{
    OS_SetArenaLo(OS_ARENA_WRAM_MAIN, newLo);
}

//---- get Arena Hi/Lo address
static inline void *OS_GetWramMainArenaHi(void)
{
    return OS_GetArenaHi(OS_ARENA_WRAM_MAIN);
}
static inline void *OS_GetWramMainArenaLo(void)
{
    return OS_GetArenaLo(OS_ARENA_WRAM_MAIN);
}

//---- allocate memory from Arena
static inline void *OS_AllocFromWramMainArenaLo(u32 size, u32 align)
{
    return OS_AllocFromArenaLo(OS_ARENA_WRAM_MAIN, size, align);
}
static inline void *OS_AllocFromWramMainArenaHi(u32 size, u32 align)
{
    return OS_AllocFromArenaHi(OS_ARENA_WRAM_MAIN, size, align);
}

/*---------------------------------------------------------------------------*
  Name:         OS_[Set/Get/AllocFrom]WramSubArena[Hi/Lo]

  Description:  convenience functions for common Work RAM,
                Sub-Processor private
 *---------------------------------------------------------------------------*/
//---- set Arena Hi/Lo address
static inline void OS_SetWramSubArenaHi(void *newHi)
{
    OS_SetArenaHi(OS_ARENA_WRAM_SUB, newHi);
}
static inline void OS_SetWramSubArenaLo(void *newLo)
{
    OS_SetArenaLo(OS_ARENA_WRAM_SUB, newLo);
}

//---- get Arena Hi/Lo address
static inline void *OS_GetWramSubArenaHi(void)
{
    return OS_GetArenaHi(OS_ARENA_WRAM_SUB);
}
static inline void *OS_GetWramSubArenaLo(void)
{
    return OS_GetArenaLo(OS_ARENA_WRAM_SUB);
}

//---- allocate memory from Arena
static inline void *OS_AllocFromWramSubArenaLo(u32 size, u32 align)
{
    return OS_AllocFromArenaLo(OS_ARENA_WRAM_SUB, size, align);
}
static inline void *OS_AllocFromWramSubArenaHi(u32 size, u32 align)
{
    return OS_AllocFromArenaHi(OS_ARENA_WRAM_SUB, size, align);
}

/*---------------------------------------------------------------------------*
  Name:         OS_[Set/Get/AllocFrom]WramSubPrivArena[Hi/Lo]

  Description:  convenience functions for Sub-Processor private Work RAM
 *---------------------------------------------------------------------------*/
//---- set Arena Hi/Lo address
static inline void OS_SetWramSubPrivArenaHi(void *newHi)
{
    OS_SetArenaHi(OS_ARENA_WRAM_SUBPRIV, newHi);
}
static inline void OS_SetWramSubPrivArenaLo(void *newLo)
{
    OS_SetArenaLo(OS_ARENA_WRAM_SUBPRIV, newLo);
}

//---- get Arena Hi/Lo address
static inline void *OS_GetWramSubPrivArenaHi(void)
{
    return OS_GetArenaHi(OS_ARENA_WRAM_SUBPRIV);
}
static inline void *OS_GetWramSubPrivArenaLo(void)
{
    return OS_GetArenaLo(OS_ARENA_WRAM_SUBPRIV);
}

//---- allocate memory from Arena
static inline void *OS_AllocFromWramSubPrivArenaLo(u32 size, u32 align)
{
    return OS_AllocFromArenaLo(OS_ARENA_WRAM_SUBPRIV, size, align);
}
static inline void *OS_AllocFromWramSubPrivArenaHi(u32 size, u32 align)
{
    return OS_AllocFromArenaHi(OS_ARENA_WRAM_SUBPRIV, size, align);
}


/*---------------------------------------------------------------------------*
  Name:         OS_InitArenaHi

  Description:  set ArenaHi as initial setting
 *---------------------------------------------------------------------------*/
static inline void OS_InitArenaHi(OSArenaId id)
{
    OS_SetArenaHi((id), OS_GetInitArenaHi((OSArenaId)id));
}

/*---------------------------------------------------------------------------*
  Name:         OS_InitArenaLo

  Description:  set ArenaLo as initial setting
 *---------------------------------------------------------------------------*/
static inline void OS_InitArenaLo(OSArenaId id)
{
    OS_SetArenaLo((id), OS_GetInitArenaLo(id));
}

/*---------------------------------------------------------------------------*
  Name:         OS_InitArenaHiAndLo

  Description:  set ArenaHi/Lo as initial setting
 *---------------------------------------------------------------------------*/
static inline void OS_InitArenaHiAndLo(OSArenaId id)
{
    OS_InitArenaHi(id);
    OS_InitArenaLo(id);
}

#ifdef SDK_ARM9
/*---------------------------------------------------------------------------*
  Name:         OS_EnableMainExArena

  Description:  Set extended main memory arena enable.
                This function should be called before OS_Init().

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_EnableMainExArena(void);

/*---------------------------------------------------------------------------*
  Name:         OS_DisableMainExArena

  Description:  Set extended main memory arena disable.
                This function should be called before OS_Init().

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_DisableMainExArena(void);
#endif

//================================================================================
//   DUMP ARENA INFO
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_DumpArenaInfo

  Description:  display arena info

  Arguments:    id         : arena id
                isInfoLine : whether display explanation line or not.
                              TRUE  ... display
                              FALSE ... not display.

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
void OS_DumpArenaInfo( OSArenaId id, BOOL isInfoLine );
#else
#define OS_DumpArenaInfo(id,isInfoLine)    ((void)0)
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_DumpAllArenaInfo

  Description:  display each arena info

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
void OS_DumpAllArenaInfo(void);
#else
#define OS_DumpAllArenaInfo()   ((void)0)
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_GetArenaResource

  Description:  store resources of arena to specified pointer

  Arguments:    resource       pointer to store arena resources

  Returns:      TRUE  ... success (always return this now)
                FALSE ... fail
 *---------------------------------------------------------------------------*/
BOOL OS_GetArenaResource(OSArenaResource *resource);

//--------------------------------------------------------------------------------

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_OS_ARENA_H_ */
#endif
