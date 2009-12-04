//=============================================================================
/**
 *
 *	@file		debug_hosaka_menu.c
 *	@brief  �ۍ�f�o�b�O���j���[
 *	@author		hosaka genya
 *	@data		2009.10.21
 *
 */
//=============================================================================

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static BOOL TESTMODE_ITEM_SelectDemo3D( TESTMODE_WORK *work, const int idx );
static BOOL TESTMODE_ITEM_SelectPmsSelect( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMicTest( TESTMODE_WORK *work , const int idx );

// ���j���[���X�g
static TESTMODE_MENU_LIST menuHosaka[] = 
{
	{L"3D�f����������",TESTMODE_ITEM_SelectDemo3D },
	{L"���񂢉�b�I��",TESTMODE_ITEM_SelectPmsSelect },
	{L"�}�C�N�e�X�g",TESTMODE_ITEM_SelectMicTest },
	
	{L"���ǂ�"				,TESTMODE_ITEM_BackTopMenu },
};

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

// 3D�f���Đ��A�v��
#include "demo/demo3d.h"
static BOOL TESTMODE_ITEM_SelectDemo3D( TESTMODE_WORK *work, const int idx )
{
  DEMO3D_PARAM* initParam;
  
  initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(DEMO3D_PARAM) );
  initParam->demo_id = DEMO3D_ID_C_CRUISER;
  initParam->start_frame = 1000; //@TODO
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(demo3d), &Demo3DProcData, initParam );
  return TRUE;
}

// �ȈՉ�b �I��
#include "app/pms_select.h"
FS_EXTERN_OVERLAY(pmsinput);

static BOOL TESTMODE_ITEM_SelectPmsSelect( TESTMODE_WORK *work , const int idx )
{
  PMS_SELECT_PARAM* initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(PMS_SELECT_PARAM) );

  initParam->save_ctrl = SaveControl_GetPointer();

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(pmsinput), &ProcData_PMSSelect, initParam);
	return TRUE;
}

// �}�C�N�e�X�g
FS_EXTERN_OVERLAY(mictest);
extern const GFL_PROC_DATA TitleMicTestProcData;
static BOOL TESTMODE_ITEM_SelectMicTest( TESTMODE_WORK *work , const int idx )
{
	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(mictest), &TitleMicTestProcData, NULL );
	return TRUE;
}

