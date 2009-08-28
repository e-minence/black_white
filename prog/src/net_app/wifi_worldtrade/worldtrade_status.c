//============================================================================================
/**
 * @file	worldtrade_status.c
 * @brief	���E�����|�P�����X�e�[�^�X��ʌĂяo���E���A
 * @author	Akito Mori
 * @date	06.05.10
 */
//============================================================================================
#include <gflib.h>
#include <dwc.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include "libdpw/dpw_tr.h"
#include "print/wordset.h"
#include "message.naix"
#include "system/wipe.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "savedata/wifilist.h"
#include "savedata/worldtrade_data.h"
#include "savedata/sp_ribbon_save.h"
#include "poke_tool/pokeparty.h"
#include "savedata/box_savedata.h"
#include "item/itemsym.h"

#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "libdpw/dpw_tr.h"

#include "app/p_status.h"

#include "savedata/mystatus.h"


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
/*** �֐��v���g�^�C�v ***/
//static void InitWork( WORLDTRADE_WORK *wk );
//static void FreeWork( WORLDTRADE_WORK *wk );



// �X�e�[�^�X��ʂŕ\������t���O�̗�i�r�b�g�e�[�u���ɂ���΂����̂Ɂc�j
static const u8 StatusPageTable[]={
	PST_PAGE_INFO_MEMO,		// �u�|�P�������傤�ق��v�u�g���[�i�[�����v
	PST_PAGE_PARAM_B_SKILL,	// �u�|�P�����̂���傭�v�u�킴�����v
	PST_PAGE_RIBBON,		// �u���˂񃊃{���v
	PST_PAGE_RET,			// �u���ǂ�v
	PST_PAGE_MAX

};

//============================================================================================
//	�v���Z�X�֐�
//============================================================================================
FS_EXTERN_OVERLAY( poke_status );

//==============================================================================
/**
 * @brief   ���E�����������ʏ�����
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTrade_Status_Init(WORLDTRADE_WORK *wk, int seq)
{
	// ���[�N������
//	InitWork( wk );
	
	MORI_PRINT( "heap remain RAM = %d\n", GFL_HEAP_GetHeapFreeSize( GFL_HEAPID_APP ));
	
	GFL_STD_MemClear( &wk->statusParam, sizeof(PSTATUS_DATA) );

	wk->statusParam.ppd  = WorldTrade_GetPokePtr(wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos);
	wk->statusParam.cfg	 = wk->param->config;

	wk->statusParam.player_name	= MyStatus_GetMyName( wk->param->mystatus );
	wk->statusParam.player_id		= MyStatus_GetID( wk->param->mystatus );
	wk->statusParam.player_sex	=  MyStatus_GetMySex( wk->param->mystatus );

	wk->statusParam.ppt  = PST_PP_TYPE_POKEPASO;
	wk->statusParam.max  = 1;
	wk->statusParam.pos  = 0;
	wk->statusParam.mode = PST_MODE_NO_WAZACHG;	// �Z����ւ��֎~�ɂ���
	wk->statusParam.waza = 0;
//	wk->statusParam.ev_contest = PokeStatus_ContestFlagGet(wk->param->savedata);
	wk->statusParam.ev_contest = FALSE;
	//TODO
	wk->statusParam.zukan_mode = wk->param->zukanmode;
	wk->statusParam.ribbon     = SP_RIBBON_SAVE_GetSaveData(wk->param->savedata);
//	wk->statusParam.pokethlon  = PokeStatus_PerformanceFlagGet(wk->param->savedata);
	wk->statusParam.pokethlon  = FALSE;
	//TODO
//	PokeStatus_PageSet( &wk->statusParam, StatusPageTable );
//	PokeStatus_PlayerSet( &wk->statusParam, wk->param->mystatus );
//	TODO
	
//	wk->proc = GFL_PROC_Create( &PokeStatusProcData, &wk->statusParam, HEAPID_WORLDTRADE );
//	TODO

	GFL_PROC_SysCallProc( FS_OVERLAY_ID(poke_status), &PokeStatus_ProcData, &wk->statusParam );

	wk->subprocflag = 1;

	return SEQ_FADEIN;
}
//==============================================================================
/**
 * @brief   ���E�����������ʃ��C��
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTrade_Status_Main(WORLDTRADE_WORK *wk, int seq)
{
	int ret=SEQ_MAIN;

//		if( ProcMain( wk->proc ) ){
		if(1){	
			//GFL_PROC_Delete( wk->proc );

			WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, wk->sub_process_mode );
			
			ret = SEQ_FADEOUT;
		}

	return ret;
}


//==============================================================================
/**
 * @brief   ���E�����������ʏI��
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTrade_Status_End(WORLDTRADE_WORK *wk, int seq)
{
//	FreeWork( wk );

	// �{�b�N�X��ʂɖ߂�
	WorldTrade_SubProcessUpdate( wk );

	return SEQ_INIT;
}
