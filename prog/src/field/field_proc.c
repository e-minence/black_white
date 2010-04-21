//======================================================================
/**
 * @file	field_proc.c
 * @author	tamada
 * @date	2008.11.18
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"	//HEAPID�Q�Ƃ̂���

#include "gamesystem/gamesystem.h"
#include "field/fieldmap_proc.h"
#include "field/fieldmap_call.h"  //FIELDMAP_Create/FIELDMAP_Main/FIELDMAP_Delete

#include "field/zonedata.h" //ZONEDATA_IsUnionRoom
#include "system/net_err.h"

//======================================================================
//	define
//======================================================================
///HEAPID_FIELD_SUBSCREEN�̃T�C�Y
#define HEAP_FIELD_SUBSCREEN_SIZE   (0xd000)

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FIELDPROC_WORK
//--------------------------------------------------------------
typedef struct {
	FIELDMAP_WORK *fieldWork;
}FIELDPROC_WORK;

//======================================================================
//	proto
//======================================================================
const GFL_PROC_DATA FieldProcData;

//======================================================================
//	
//======================================================================
#ifdef PM_DEBUG
extern BOOL DebugScanOnly;
#endif


//======================================================================
//	�t�B�[���h�v���Z�X
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̏�����
 * �����Ńq�[�v�̐�����e�평�����������s���B
 * �����i�K�ł�mywk��NULL�����AGFL_PROC_AllocWork���g�p�����
 * �ȍ~�͊m�ۂ������[�N�̃A�h���X�ƂȂ�B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FieldMapProcInit
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	FIELDMAP_WORK * fieldWork;
	FIELDPROC_WORK * fpwk;
	GAMESYS_WORK * gsys = pwk;
  u32 heap_size;

  switch(*seq){
  case 0:
    {
      u16 zone_id = PLAYERWORK_getZoneID( GAMESYSTEM_GetMyPlayerWork(gsys) );
      if (ZONEDATA_IsUnionRoom(zone_id) || ZONEDATA_IsColosseum(zone_id) )
      {
        heap_size = 0x140000 - 0x18000;
      }
      else if ( ZONEDATA_IsMusicalWaitingRoom(zone_id) )
      {
        heap_size = 0xC0000 - 0x18000;
      }
      else
      {
        heap_size = 0x148000 - 0x18000;
      }
    }
    heap_size -= HEAP_FIELD_SUBSCREEN_SIZE;
  	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_FIELDMAP, heap_size );
  	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_FIELD_SUBSCREEN, HEAP_FIELD_SUBSCREEN_SIZE );
  	fpwk = GFL_PROC_AllocWork(proc, sizeof(FIELDPROC_WORK), HEAPID_FIELDMAP);
  	fpwk->fieldWork = FIELDMAP_Create(gsys, HEAPID_FIELDMAP );
  	GAMESYSTEM_SetFieldMapWork(gsys, fpwk->fieldWork);
  	(*seq)++;
  	break;
  
  case 1:
    //�펞�ʐM���[�h
    {
      if(GAMESYSTEM_GetAlwaysNetFlag(gsys) == TRUE){
        GAMESYSTEM_CommBootAlways(gsys);
      }
      else{ //�펞�ʐM��OFF���[�h
        GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
        GAME_COMM_NO comm_no = GameCommSys_BootCheck(game_comm);
        switch(comm_no){
        case GAME_COMM_NO_FIELD_BEACON_SEARCH:
        case GAME_COMM_NO_INVASION:
          OS_TPrintf("��ʐM���[�h�ׁ̈A�ʐM�I��\n");
          GameCommSys_ExitReq(game_comm);
          break;
        }
      }
    }
  	return GFL_PROC_RES_FINISH;
  }
  
 	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̃��C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FieldMapProcMain
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESYS_WORK * gsys = pwk;
	FIELDPROC_WORK * fpwk = mywk;
	if( FIELDMAP_Main(gsys, fpwk->fieldWork) == TRUE ){
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̏I������
 *
 * �P�ɏI�������ꍇ�A�e�v���Z�X�ɏ������Ԃ�B
 * GFL_PROC_SysSetNextProc���Ă�ł����ƁA�I���セ�̃v���Z�X��
 * �������J�ڂ���B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT FieldMapProcEnd
				( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GAMESYS_WORK * gsys = pwk;
	FIELDPROC_WORK * fpwk = mywk;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  
  //��check�@�t�B�[���h�������Ԃ̓r�[�R���T�[�`���Ȃ�
  if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_FIELD_BEACON_SEARCH
      || GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_DEBUG_SCANONLY){
  //  GameCommSys_ExitReq(game_comm);
  }

	FIELDMAP_Delete(fpwk->fieldWork);
	GAMESYSTEM_SetFieldMapWork(gsys, NULL);
	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap( HEAPID_FIELD_SUBSCREEN );
	GFL_HEAP_DeleteHeap( HEAPID_FIELDMAP );

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
///	�t�B�[���h�v���Z�X�@�f�[�^
//------------------------------------------------------------------
const GFL_PROC_DATA FieldProcData = {
	FieldMapProcInit,
	FieldMapProcMain,
	FieldMapProcEnd,
};
