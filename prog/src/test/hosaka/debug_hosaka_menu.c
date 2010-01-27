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
//static BOOL TESTMODE_ITEM_SelectIntro( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStart( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStartMulti( TESTMODE_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectPmsSelect( TESTMODE_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMicTest( TESTMODE_WORK *work , const int idx );

// ���j���[���X�g
static TESTMODE_MENU_LIST menuHosaka[] = 
{
//	{L"�C���g���f��",TESTMODE_ITEM_SelectIntro },
  {L"�ʐM�o�g���O�f��",TESTMODE_ITEM_SelectCmmBtlDemoStart},
  {L"�ʐM�o�g���O�}���`",TESTMODE_ITEM_SelectCmmBtlDemoStartMulti},
	{L"���񂢉�b�I��",TESTMODE_ITEM_SelectPmsSelect },
	{L"�}�C�N�e�X�g",TESTMODE_ITEM_SelectMicTest },
	
	{L"���ǂ�"				,TESTMODE_ITEM_BackTopMenu },
};

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

#if 0
// �C���g���f��
#include "demo/intro.h"
static BOOL TESTMODE_ITEM_SelectIntro( TESTMODE_WORK *work , const int idx )
{
  INTRO_PARAM* initParam = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP, sizeof(INTRO_PARAM) );

  initParam->save_ctrl = SaveControl_GetPointer();

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(intro), &ProcData_Intro, initParam);

  return TRUE;
}
#endif
  
#include "demo/comm_btl_demo.h"
static COMM_BTL_DEMO_PARAM g_CommBTlDemoParam = {0};

static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStart( TESTMODE_WORK* work, const int idx )
{ 
  g_CommBTlDemoParam.type = COMM_BTL_DEMO_TYPE_NORMAL_START;

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, &g_CommBTlDemoParam);

  return TRUE;
}

static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStartMulti( TESTMODE_WORK* work, const int idx )
{
  g_CommBTlDemoParam.type = COMM_BTL_DEMO_TYPE_MULTI_START;

	TESTMODE_COMMAND_ChangeProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, &g_CommBTlDemoParam);

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

