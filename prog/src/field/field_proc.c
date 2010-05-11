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
#include "field_data.h"   //FIELDDATA_GetFieldHeapSize

//======================================================================
//	define
//======================================================================
///HEAPID_FIELD_SUBSCREEN�̃T�C�Y
#define HEAP_FIELD_SUBSCREEN_SIZE   (0xd000)



//--------------------------------------------------------------
///	�q�[�v�p�v���O�����̈�
//--------------------------------------------------------------
#define FIELD_PROG_AREA_HEAP_SIZE  (0xf800)
static u8 FIELD_PROG_AREA_HEAP_BUF[ FIELD_PROG_AREA_HEAP_SIZE ] ATTRIBUTE_ALIGN(4);  //<-4byte�A���C�����g
#define FIELD_PROG_AREA_WEATHER_HEAP_SIZE (0x6400)  // �V�C�Ɋ��蓖�Ă郁�����T�C�Y
#define FIELD_PROG_AREA_PLACENAME_HEAP_SIZE (0x7400)  // �n���\���Ɋ��蓖�Ă郁�����T�C�Y

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
      heap_size = FIELDDATA_GetFieldHeapSize( zone_id );
    }
    heap_size -= HEAP_FIELD_SUBSCREEN_SIZE;
  	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_FIELDMAP, heap_size );
  	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_FIELD_SUBSCREEN, HEAP_FIELD_SUBSCREEN_SIZE );

    // �v���O�����G���A���g�p�����q�[�v�쐬
    // ���̃��������g�p���āA
    // �V�C�Ȃǂ𓮂����B
    GFL_HEAP_CreateHeapInBuffer( FIELD_PROG_AREA_HEAP_BUF, FIELD_PROG_AREA_HEAP_SIZE, HEAPID_FIELD_PRBUF );
    // �V�C�A�n���\���́AZONE�؂�ւ����ɕ����ǂݍ��݂��s������
    // �f�Љ����₷���B��Ƀ��������m�ۂ��A�f�Љ����������B
    GFL_HEAP_CreateHeap( HEAPID_FIELD_PRBUF, HEAPID_WEATHER, FIELD_PROG_AREA_WEATHER_HEAP_SIZE );
    GFL_HEAP_CreateHeap( HEAPID_FIELD_PRBUF, HEAPID_PLACE_NAME, FIELD_PROG_AREA_PLACENAME_HEAP_SIZE );


  	fpwk = GFL_PROC_AllocWork(proc, sizeof(FIELDPROC_WORK), HEAPID_FIELDMAP);
  	fpwk->fieldWork = FIELDMAP_Create(gsys, HEAPID_FIELDMAP );
  	GAMESYSTEM_SetFieldMapWork(gsys, fpwk->fieldWork);
  	(*seq)++;
  	break;
  
  case 1:
    //�펞�ʐM���[�h
    {
      if(GAMESYSTEM_GetAlwaysNetFlag(gsys) == FALSE){
        //�펞�ʐM��OFF���[�h
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
  
	FIELDMAP_Delete(fpwk->fieldWork);
	GAMESYSTEM_SetFieldMapWork(gsys, NULL);
	GFL_PROC_FreeWork(proc);

  GFL_HEAP_DeleteHeap( HEAPID_WEATHER );
  GFL_HEAP_DeleteHeap( HEAPID_PLACE_NAME );
  GFL_HEAP_DeleteHeap( HEAPID_FIELD_PRBUF );
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
