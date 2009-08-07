//==============================================================================
/**
 * @file	trade_main.c
 * @brief	�������PROC����
 * @author	matsuda
 * @date	2008.12.09(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system\main.h"
#include "print\printsys.h"
#include "print\gf_font.h"
#include "arc_def.h"
#include "message.naix"
#include "msg\msg_d_matsu.h"
#include "test/performance.h"
#include "font/font.naix"
#include "pm_define.h"
#include "gamesystem/game_data.h"
#include "trade.h"
#include "trade/trade_main.h"
#include "poke_tool/pokeparty.h"


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct {
	u16		seq;
	POKEPARTY *party;
	
//	EASY_POKELIST_PARENT epp;		///<�|�P�������X�g�Ăяo���悤���[�N
	TRADE_DISP_PARENT trade_disp;	///<������ʌĂяo���悤���[�N
}TRADE_MAIN_WORK;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT TradeMainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT TradeMainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT TradeMainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


//==============================================================================
//	�f�[�^
//==============================================================================
///�^�C�g����ʌĂяo���悤��PROC�f�[�^
const GFL_PROC_DATA TradeMainProcData = {
	TradeMainProcInit,
	TradeMainProcMain,
	TradeMainProcEnd,
};


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   proc		
 * @param   seq			
 * @param   pwk			
 * @param   mywk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT TradeMainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	TRADE_MAIN_WORK *tmw;
	TRADE_PARENT_WORK *parent = pwk;
	
	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TRADE, 0x40000 );
	tmw = GFL_PROC_AllocWork( proc, sizeof(TRADE_MAIN_WORK), HEAPID_TRADE );
	GFL_STD_MemClear(tmw, sizeof(TRADE_MAIN_WORK));

	if(pwk == NULL){	//NULL�̎��̓f�o�b�O�ł̎g�p�Ȃ̂ł����ō쐬���Ă��܂�
		POKEPARTY *party;
		POKEMON_PARAM *pp;
		int i, monsno;
		u8 mac_address[6];
		
		party = PokeParty_AllocPartyWork(HEAPID_TRADE);
		
		//�K���Ƀ|�P�������쐬
		OS_GetMacAddress(mac_address);
		pp = PP_Create(1, 50, 123456, HEAPID_TRADE);
		for(i = 0; i < TEMOTI_POKEMAX; i++){
			OS_TPrintf("monsno = %d\n", mac_address[i]);
			monsno = (mac_address[i] == 0) ? 1 : mac_address[i];
			PP_Setup(pp, monsno, 50, 123456);
			PokeParty_Add(party, pp);
		}
		GFL_HEAP_FreeMemory(pp);

		tmw->party = party;
	}
	else{
		tmw->party = parent->party;
	}
	
	//�|�P�������X�g�Ăяo���悤�̍\���̃f�[�^�쐬
//	tmw->epp.party = tmw->party;
	
	//������ʌĂяo���悤�̍\���̃f�[�^�쐬
	tmw->trade_disp.party = tmw->party;
	tmw->trade_disp.sv = SaveControl_GetPointer();

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT TradeMainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	TRADE_MAIN_WORK* tmw = mywk;
	enum{
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_LIST_RETURN,
	};
	
	switch(tmw->seq){
	case SEQ_INIT:
		GFL_PROC_SysCallProc(NO_OVERLAY_ID, &TradeProcData, &tmw->trade_disp);
		tmw->seq++;
		break;
	case SEQ_MAIN:
		switch(tmw->trade_disp.select_menu){
		case TRADE_MENU_CANCEL:
			return GFL_PROC_RES_FINISH;
		default:
//			GFL_PROC_SysCallProc(FS_OVERLAY_ID(pokelist), &EasyPokeListData, &tmw->epp);
			tmw->seq++;
			break;
		}
		break;
	case SEQ_LIST_RETURN:
#if 0
		if(tmw->epp.select_pos == -1){
			tmw->trade_disp.select_menu = TRADE_MENU_NULL;
			tmw->trade_disp.first_seq = TRADE_STARTUP_SEQ_SELECT;
		}
		else{
			tmw->trade_disp.first_seq = TRADE_STARTUP_SEQ_CHANGE;
			tmw->trade_disp.sel_mons = tmw->epp.select_pos;
		}
#endif
		tmw->seq = SEQ_INIT;
		break;
	}

	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT TradeMainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	TRADE_MAIN_WORK* tmw = mywk;
	TRADE_PARENT_WORK *parent = pwk;
	
	if(pwk == NULL){
		GFL_HEAP_FreeMemory(tmw->party);
	}
	
	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_TRADE);
	
	return GFL_PROC_RES_FINISH;
}

