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
#include "savedata/musical_save.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
//�m�ۂ���T�C�Y
#define MUSICAL_DIST_SAVE_SIZE (127*1024)
//Alloc����T�C�Y
#define MUSICAL_DIST_SAVE_WORK_SIZE (128*1024)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct
{
  u32 size;
  u32 pad[3];
}DIST_DATA_HEADER;

struct _MUSICAL_DIST_SAVE
{
  HEAPID heapId;
  BOOL isEnableData;
  int  saveSeq;
  DIST_DATA_HEADER dataHeader;
  void *saveData;
  
  DIST_DATA_HEADER *saveDataHeader;
  void *saveDataArc;
  SAVE_CONTROL_WORK *sv;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

int MUSICAL_DIST_SAVE_GetWorkSize(void)
{
  //�w�b�_�����炵�Ă���
  //�a������ł���Ɍ��邩��
	return MUSICAL_DIST_SAVE_SIZE;
}

void MUSICAL_DIST_SAVE_InitWork(MUSICAL_DIST_SAVE *musDistSave)
{
  //�����Ȃ�
}

MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_LoadData( SAVE_CONTROL_WORK *sv , HEAPID heapId )
{
  MUSICAL_DIST_SAVE *distSave = GFL_HEAP_AllocClearMemory( heapId , sizeof(distSave) );
  LOAD_RESULT ret;
  distSave->saveData = GFL_HEAP_AllocClearMemory( heapId , MUSICAL_DIST_SAVE_WORK_SIZE );
  ret = SaveControl_Extra_LoadWork( sv , SAVE_EXTRA_ID_MUSICAL_DIST , heapId , distSave->saveData , MUSICAL_DIST_SAVE_WORK_SIZE );
  
  distSave->heapId = heapId;
  distSave->sv = sv;
  distSave->saveSeq = 0;
  if( ret == LOAD_RESULT_OK )
  {
    //�f�[�^������
    distSave->isEnableData = TRUE;
    distSave->saveData = SaveControl_Extra_DataPtrGet( sv , SAVE_EXTRA_ID_MUSICAL_DIST , EXGMDATA_ID_MUSICAL_DIST );
    distSave->saveDataHeader = distSave->saveData;
    distSave->saveDataArc = (void*)((u32)distSave->saveData + sizeof(DIST_DATA_HEADER));
  }
  else
  {
    //�f�[�^������
    distSave->isEnableData = FALSE;
    distSave->saveData = SaveControl_Extra_DataPtrGet( sv , SAVE_EXTRA_ID_MUSICAL_DIST , EXGMDATA_ID_MUSICAL_DIST );
    distSave->saveDataHeader = distSave->saveData;
    distSave->saveDataArc = (void*)((u32)distSave->saveData + sizeof(DIST_DATA_HEADER));
  }
  return distSave;
}

void MUSICAL_DIST_SAVE_UnloadData( MUSICAL_DIST_SAVE *distSave )
{
  SaveControl_Extra_Unload( distSave->sv , SAVE_EXTRA_ID_MUSICAL_DIST );
  if( distSave->saveData != NULL )
  {
    GFL_HEAP_FreeMemory( distSave->saveData );
  }
  GFL_HEAP_FreeMemory( distSave );
}

//Init��Main�ŃZ�[�u�̊m�ہE�J���A�f�[�^�L���t���O�̃Z�b�g�Ȃǂ��s���܂��B
MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_SaveMusicalArchive_Init( SAVE_CONTROL_WORK *sv , void *arcData , const u32 size , const HEAPID heapId )
{
  MUSICAL_DIST_SAVE *distSave = MUSICAL_DIST_SAVE_LoadData( sv , heapId );
  if( distSave->saveData != NULL )
  {
    distSave->dataHeader.size = size;
    GFL_STD_MemCopy( &distSave->dataHeader , distSave->saveData , sizeof( DIST_DATA_HEADER ) );
    GFL_STD_MemCopy( arcData , distSave->saveDataArc , size );
  }
  return distSave;
}

//�������I����MUSICAL_DIST_SAVE�͖����ɂȂ��Ă��܂��B
const BOOL MUSICAL_DIST_SAVE_SaveMusicalArchive_Main( MUSICAL_DIST_SAVE *distSave )
{
  switch( distSave->saveSeq )
  {
  case 0:
    if( distSave->saveData == NULL )
    {
      //�f�[�^�����Ă��ăZ�[�u�ł��Ȃ��B
      MUSICAL_DIST_SAVE_UnloadData( distSave );
      return TRUE;
    }
    else
    {
      MUSICAL_SAVE* musSave = MUSICAL_SAVE_GetMusicalSave( distSave->sv );
      //�L���t���O�̐ݒ�
      MUSICAL_SAVE_SetEnableDistributData( musSave , TRUE );
      //���ڃ^�C�g���̑ޔ�
      {
        u32 msgDataSize;
        ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandleByMemory( distSave->saveDataArc , distSave->dataHeader.size , GFL_HEAP_LOWID(distSave->heapId) );
        void *msgData = GFL_ARCHDL_UTIL_LoadEx( arcHandle , MUSICAL_ARCDATAID_GMMDATA , FALSE , GFL_HEAP_LOWID(distSave->heapId) , &msgDataSize );
        GFL_MSGDATA *msgHandle = GFL_MSG_Construct( msgData, GFL_HEAP_LOWID(distSave->heapId) );
        STRBUF *tmpBuf = GFL_MSG_CreateString( msgHandle , 0 );
        STRCODE *saveBuf = MUSICAL_SAVE_GetDistributTitle( musSave );

        GFL_STR_GetStringCode( tmpBuf , saveBuf , MUSICAL_PROGRAM_NAME_MAX );

        GFL_STR_DeleteBuffer( tmpBuf );
        GFL_MSG_Delete( msgHandle );
        GFL_HEAP_FreeMemory( msgData );
        GFL_ARC_CloseDataHandle( arcHandle );
      }
      SaveControl_Extra_SaveAsyncInit( distSave->sv , SAVE_EXTRA_ID_MUSICAL_DIST );
      distSave->saveSeq++;
    }
    break;
  case 1:
    {
      const SAVE_RESULT ret = SaveControl_Extra_SaveAsyncMain( distSave->sv , SAVE_EXTRA_ID_MUSICAL_DIST );
      if( ret == SAVE_RESULT_OK )
      {
        MUSICAL_DIST_SAVE_UnloadData( distSave );
        return TRUE;
      }
    }
    break;
  }
  return FALSE;
}



#pragma mark [> proto

void* MUSICAL_DIST_SAVE_GetMusicalArc( MUSICAL_DIST_SAVE *distSave )
{
  return distSave->saveDataArc;
}
const u32 MUSICAL_DIST_SAVE_GetMusicalArcSize( MUSICAL_DIST_SAVE *distSave )
{
  return distSave->saveDataHeader->size;
}