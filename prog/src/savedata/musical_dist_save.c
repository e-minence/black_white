//============================================================================================
/**
 * @file	musical_dist_save.c
 * @brief	�~���[�W�J���z�M�p�Z�[�u�f�[�^
 * @author	Nobuhiko Ariizumi
 * @date	2009/04/08
 */
//============================================================================================
#include <gflib.h>

#include "savedata/save_tbl.h"
#include "savedata/musical_dist_save.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _MUSICAL_DIST_SAVE
{
  u32 programDataSize;
  u32 messageDataSize;
  u32 scriptDataSize;
  u32 streamingDataSize;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
int MUSICAL_DIST_SAVE_GetWorkSize(void)
{
  //��
	return sizeof(MUSICAL_DIST_SAVE);
}

void MUSICAL_DIST_SAVE_InitWork(MUSICAL_DIST_SAVE *musDistSave)
{
  //��
  musDistSave->programDataSize = 0;
  musDistSave->messageDataSize = 0;
  musDistSave->scriptDataSize = 0;
  musDistSave->streamingDataSize = 0;
}

MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_LoadData( SAVE_CONTROL_WORK *sv , HEAPID heapId )
{
  //SaveControl_Extra_Load( sv , EXGMDATA_ID_STREAMING , heapId );
  return NULL;
}

void MUSICAL_DIST_SAVE_UnloadData( SAVE_CONTROL_WORK *sv )
{
  //SaveControl_Extra_Unload( sv , EXGMDATA_ID_STREAMING );
}

#pragma mark [> proto
#include "arc_def.h"
void* MUSICAL_DIST_SAVE_GetProgramData( MUSICAL_DIST_SAVE *distSave , const HEAPID heapId , u32 *dataSize )
{
  //��
  return GFL_ARC_UTIL_LoadEx( ARCID_MUSICAL_PROGRAM_01 , 0 , FALSE , heapId , dataSize);
}
void* MUSICAL_DIST_SAVE_GetMessageData( MUSICAL_DIST_SAVE *distSave , const HEAPID heapId , u32 *dataSize )
{
  //��
  return GFL_ARC_UTIL_LoadEx( ARCID_MUSICAL_PROGRAM_01 , 1 , FALSE , heapId , dataSize);
}
void* MUSICAL_DIST_SAVE_GetScriptData( MUSICAL_DIST_SAVE *distSave , const HEAPID heapId , u32 *dataSize )
{
  //��
  return GFL_ARC_UTIL_LoadEx( ARCID_MUSICAL_PROGRAM_01 , 2 , FALSE , heapId , dataSize);
}
void* MUSICAL_DIST_SAVE_GetStreamingData( MUSICAL_DIST_SAVE *distSave , const HEAPID heapId , u32 *dataSize )
{
  //��
  return GFL_ARC_UTIL_LoadEx( ARCID_MUSICAL_PROGRAM_01 , 3 , FALSE , heapId , dataSize);
}
