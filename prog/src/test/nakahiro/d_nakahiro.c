//============================================================================================
/**
 * @file	  d_nakahiro.c
 * @brief		�����p�f�o�b�O����
 * @author  Hiroyuki Nakamura
 * @date	  2009.10.02
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "system/wipe.h"
#include "system/bmp_menulist.h"
#include "gamesystem/game_data.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "font/font.naix"

#include "app/box2.h"
#include "app/zukan.h"
#include "demo/command_demo.h"

#include "arc_def.h"  //ARCID_MESSAGE
#include "message.naix"
#include "msg/msg_d_nakahiro.h"


//============================================================================================
//	�萔��`
//============================================================================================
#define	TOP_MENU_SIZ	( 9 )

typedef struct {
	u32	main_seq;
	u32	next_seq;
	HEAPID	heapID;

	GFL_FONT * font;					// �ʏ�t�H���g
	GFL_MSGDATA * mman;				// ���b�Z�[�W�f�[�^�}�l�[�W��
	PRINT_QUE * que;					// �v�����g�L���[
	GFL_BMPWIN * win;					// BMPWIN
	PRINT_UTIL	util;

	BMP_MENULIST_DATA * ld;
	BMPMENULIST_WORK * lw;

	GAMEDATA * gamedata;

	BOX2_GFL_PROC_PARAM	box_data;
	void * bb_party;

	COMMANDDEMO_DATA	cdemo_data;

	ZUKAN_PARAM	zkn_data;

}NAKAHIRO_MAIN_WORK;

enum {
	MAIN_SEQ_INIT = 0,
	MAIN_SEQ_MAIN,
	MAIN_SEQ_FADE_MAIN,

	MAIN_SEQ_BOX_CALL1,
	MAIN_SEQ_BOX_CALL2,
	MAIN_SEQ_BOX_CALL3,
	MAIN_SEQ_BOX_CALL4,
	MAIN_SEQ_BOX_CALL5,
	MAIN_SEQ_BOX_CALL6,

	MAIN_SEQ_MOVIE_CALL,

	MAIN_SEQ_ZUKAN_CALL,
	MAIN_SEQ_ZKNLIST_CALL,

	MAIN_SEQ_END,
};




//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT MainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT MainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT MainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static void FadeInSet( NAKAHIRO_MAIN_WORK * wk, u32 next );
static void FadeOutSet( NAKAHIRO_MAIN_WORK * wk, u32 next );

static void SetBoxPoke( NAKAHIRO_MAIN_WORK * wk );
static void SetPartyPoke( NAKAHIRO_MAIN_WORK * wk );

static void BgInit( NAKAHIRO_MAIN_WORK * wk );
static void BgExit(void);

static void TopMenuInit( NAKAHIRO_MAIN_WORK * wk );
static void TopMenuExit( NAKAHIRO_MAIN_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================
const GFL_PROC_DATA DebugNakahiroMainProcData = {
  MainProcInit,
  MainProcMain,
  MainProcEnd,
};

static const BMPMENULIST_HEADER TopMenuListH = {
	NULL, NULL, NULL, NULL,
	TOP_MENU_SIZ,		// ���X�g���ڐ�
	6,		// �\���ő區�ڐ�
	0,		// ���x���\���w���W
	12,		// ���ڕ\���w���W
	0,		// �J�[�\���\���w���W
	0,		// �\���x���W
	1,		// �\�������F
	15,		// �\���w�i�F
	2,		// �\�������e�F
	0,		// �����Ԋu�w
	16,		// �����Ԋu�x
	BMPMENULIST_NO_SKIP,		// �y�[�W�X�L�b�v�^�C�v
	0,		// �����w��(�{����u8�����ǂ���Ȃɍ��Ȃ��Ǝv���̂�)
	0,		// �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)

	NULL,
	
	12,			// �����T�C�YX(�h�b�g
	16,			// �����T�C�YY(�h�b�g
	NULL,		// �\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
	NULL,		// �\���Ɏg�p����v�����g���[�e�B���e�B
	NULL,		// �\���Ɏg�p����v�����g�L���[
	NULL,		// �\���Ɏg�p����t�H���g�n���h��
};




static GFL_PROC_RESULT MainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	NAKAHIRO_MAIN_WORK * wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAKAHIRO_DEBUG, 0x20000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(NAKAHIRO_MAIN_WORK), HEAPID_NAKAHIRO_DEBUG );

	wk->main_seq  = MAIN_SEQ_INIT;
	wk->heapID    = HEAPID_NAKAHIRO_DEBUG;
	wk->gamedata	= GAMEDATA_Create( wk->heapID );
	wk->bb_party  = NULL;

  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT MainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	NAKAHIRO_MAIN_WORK * wk = mywk;

	switch( wk->main_seq ){
	case MAIN_SEQ_INIT:
		BgInit( wk );
		TopMenuInit( wk );
		wk->main_seq = MAIN_SEQ_MAIN;
		break;

	case MAIN_SEQ_MAIN:
		{
			u32 ret = BmpMenuList_Main( wk->lw );

			switch( ret ){
			case BMPMENULIST_NULL:
			case BMPMENULIST_RABEL:
				break;

			case BMPMENULIST_CANCEL:		// �L�����Z��
				TopMenuExit( wk );
				BgExit();
				wk->main_seq = MAIN_SEQ_END;
				break;

			default:
				TopMenuExit( wk );
				BgExit();
				FadeOutSet( wk, ret );
				wk->main_seq = MAIN_SEQ_FADE_MAIN;
			}
		}
		break;

	case MAIN_SEQ_FADE_MAIN:
		if( WIPE_SYS_EndCheck() == TRUE ){
			wk->main_seq = wk->next_seq;
		}
		break;

	case MAIN_SEQ_END:
		if( wk->bb_party != NULL ){
			BATTLE_BOX_SAVE * sv;
			sv = BATTLE_BOX_SAVE_GetBattleBoxSave( GAMEDATA_GetSaveControlWork(wk->gamedata) );
			BATTLE_BOX_SAVE_SetPokeParty( sv, wk->bb_party );
			GFL_HEAP_FreeMemory( wk->bb_party );
		}
		OS_Printf( "nakahiro�f�o�b�O�����I�����܂���\n" );
	  return GFL_PROC_RES_FINISH;




	case MAIN_SEQ_BOX_CALL1:
		wk->box_data.gamedata  = wk->gamedata;
		wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
		wk->box_data.pokeparty = GAMEDATA_GetMyPokemon( wk->gamedata );
		wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
		wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
		wk->box_data.callMode  = BOX_MODE_SEIRI;
		SetBoxPoke( wk );
		SetPartyPoke( wk );
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
		wk->main_seq = MAIN_SEQ_END;
		break;

	case MAIN_SEQ_BOX_CALL2:
		wk->box_data.gamedata  = wk->gamedata;
		wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
		wk->box_data.pokeparty = GAMEDATA_GetMyPokemon( wk->gamedata );
		wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
		wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
		wk->box_data.callMode  = BOX_MODE_TURETEIKU;
		SetBoxPoke( wk );
		SetPartyPoke( wk );
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
		wk->main_seq = MAIN_SEQ_END;
		break;

	case MAIN_SEQ_BOX_CALL3:
		wk->box_data.gamedata  = wk->gamedata;
		wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
		wk->box_data.pokeparty = GAMEDATA_GetMyPokemon( wk->gamedata );
		wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
		wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
		wk->box_data.callMode  = BOX_MODE_AZUKERU;
		SetBoxPoke( wk );
		SetPartyPoke( wk );
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
		wk->main_seq = MAIN_SEQ_END;
		break;

	case MAIN_SEQ_BOX_CALL4:
		wk->box_data.gamedata  = wk->gamedata;
		wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
		wk->box_data.pokeparty = GAMEDATA_GetMyPokemon( wk->gamedata );
		wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
		wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
		wk->box_data.callMode  = BOX_MODE_ITEM;
		SetBoxPoke( wk );
		SetPartyPoke( wk );
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
		wk->main_seq = MAIN_SEQ_END;
		break;

	case MAIN_SEQ_BOX_CALL5:
		{
			BATTLE_BOX_SAVE * sv;
			sv = BATTLE_BOX_SAVE_GetBattleBoxSave( GAMEDATA_GetSaveControlWork(wk->gamedata) );
			wk->bb_party = BATTLE_BOX_SAVE_MakePokeParty( sv, wk->heapID );
		}
		wk->box_data.gamedata  = wk->gamedata;
		wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
		wk->box_data.pokeparty = wk->bb_party;
		wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
		wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
		wk->box_data.callMode  = BOX_MODE_BATTLE;
		SetBoxPoke( wk );
//		SetPartyPoke( wk );
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
		wk->main_seq = MAIN_SEQ_END;
		break;

	case MAIN_SEQ_BOX_CALL6:
		wk->box_data.gamedata  = wk->gamedata;
		wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
		wk->box_data.pokeparty = GAMEDATA_GetMyPokemon( wk->gamedata );
		wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
		wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
		wk->box_data.callMode  = BOX_MODE_SLEEP;
		SetBoxPoke( wk );
		SetPartyPoke( wk );
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
		wk->main_seq = MAIN_SEQ_END;
		break;

	case MAIN_SEQ_MOVIE_CALL:
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &wk->cdemo_data );
		wk->main_seq = MAIN_SEQ_END;
		break;


	case MAIN_SEQ_ZUKAN_CALL:
		wk->zkn_data.callMode = ZUKAN_MODE_LIST;
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(zukan), &ZUKAN_ProcData, &wk->zkn_data );
		wk->main_seq = MAIN_SEQ_END;
		break;

	case MAIN_SEQ_ZKNLIST_CALL:
		wk->zkn_data.callMode = ZUKAN_MODE_LIST;
		GFL_PROC_SysCallProc( FS_OVERLAY_ID(zukan), &ZUKAN_ProcData, &wk->zkn_data );
		wk->main_seq = MAIN_SEQ_END;
		break;
	}

  return GFL_PROC_RES_CONTINUE;
}

static GFL_PROC_RESULT MainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	NAKAHIRO_MAIN_WORK * wk = mywk;

	GAMEDATA_Delete( wk->gamedata );

	GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_NAKAHIRO_DEBUG );

  return GFL_PROC_RES_FINISH;
}

static void BgInit( NAKAHIRO_MAIN_WORK * wk )
{
	GFL_BG_Init( wk->heapID );
	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}
	{	// ���C����ʁF����
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0, wk->heapID );
	}
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );

	// �t�H���g�p���b�g
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0, 0x20, wk->heapID );
}

static void BgExit(void)
{
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
	GFL_BG_Exit();
}

// �t�F�[�h�C��
static void FadeInSet( NAKAHIRO_MAIN_WORK * wk, u32 next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapID );

	wk->next_seq = next;
}

// �t�F�[�h�A�E�g
static void FadeOutSet( NAKAHIRO_MAIN_WORK * wk, u32 next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapID );

	wk->next_seq = next;
}

static void TopMenuInit( NAKAHIRO_MAIN_WORK * wk )
{
	BMPMENULIST_HEADER	lh;
	u32	i;

	GFL_BMPWIN_Init( wk->heapID );

	wk->mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_nakahiro_dat, wk->heapID );
  wk->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );
	wk->que  = PRINTSYS_QUE_Create( wk->heapID );
	wk->win  = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 1, 1, 16, 12, 0, GFL_BMP_CHRAREA_GET_B );

	wk->ld = BmpMenuWork_ListCreate( TOP_MENU_SIZ, wk->heapID );
	for( i=0; i<TOP_MENU_SIZ; i++ ){
		STRBUF * str = GFL_MSG_CreateString( wk->mman, top_menu00+i );
		BmpMenuWork_ListAddString( &wk->ld[i], str, MAIN_SEQ_BOX_CALL1+i, wk->heapID );
		GFL_STR_DeleteBuffer( str );
	}

	lh = TopMenuListH;
	lh.list = wk->ld;
	lh.win  = wk->win;
	lh.msgdata = wk->mman;			//�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
	lh.print_util = &wk->util;	//�\���Ɏg�p����v�����g���[�e�B���e�B
	lh.print_que  = wk->que;		//�\���Ɏg�p����v�����g�L���[
	lh.font_handle = wk->font;	//�\���Ɏg�p����t�H���g�n���h��

	wk->lw = BmpMenuList_Set( &lh, 0, 0, wk->heapID );
	BmpMenuList_SetCursorBmp( wk->lw, wk->heapID );

	GFL_BMPWIN_TransVramCharacter( wk->win );
	GFL_BMPWIN_MakeScreen( wk->win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(wk->win) );
}

static void TopMenuExit( NAKAHIRO_MAIN_WORK * wk )
{
	BmpMenuList_Exit( wk->lw, NULL, NULL );
	BmpMenuWork_ListDelete( wk->ld );

	GFL_BMPWIN_Delete( wk->win );
	PRINTSYS_QUE_Delete( wk->que );
	GFL_FONT_Delete( wk->font );
	GFL_MSG_Delete( wk->mman );

	GFL_BMPWIN_Exit();
}


static void SetBoxPoke( NAKAHIRO_MAIN_WORK * wk )
{
	const POKEMON_PASO_PARAM * ppp;
	POKEMON_PARAM * pp;
	GFL_MSGDATA * man;
	STRBUF * str;
	u32	i;

	man = GFL_MSG_Create(
						GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_nakahiro_dat, wk->heapID );
	str = GFL_MSG_CreateString( man, pokename );

//	for( i=MONSNO_HUSIGIDANE; i<=MONSNO_END; i++ ){
	for( i=MONSNO_HUSIGIDANE; i<=40; i++ ){
//	for( i=MONSNO_HUSIGIDANE; i<=8; i++ ){
		pp  = PP_Create( i, 50, 0, wk->heapID );
    PP_Put( pp, ID_PARA_oyaname, (u32)str );
    PP_Put( pp, ID_PARA_oyasex, PTL_SEX_MALE );
		if( i == 40 ){
	    PP_Put( pp, ID_PARA_tamago_flag, 1 );
	    PP_Put( pp, ID_PARA_form_no, 0 );
		}
		if( ( i & 3 ) == 3 ){
	    PP_Put( pp, ID_PARA_item, 1+(i&1) );
		}
		ppp = PP_GetPPPPointerConst( pp );
		if( BOXDAT_PutPokemon( wk->box_data.sv_box, ppp ) == FALSE ){
			GFL_HEAP_FreeMemory( pp );
			break;
		}
		GFL_HEAP_FreeMemory( pp );
	}

  GFL_STR_DeleteBuffer( str );
	GFL_MSG_Delete( man );

/*
  ID_PARA_oyaname,              //�e�̖��O�iSTRBUF�g�p�j
  ID_PARA_oyaname_raw,            //�e�̖��O�iSTRCODE�z��g�p �������j
  ID_PARA_get_year,             //�߂܂����N
  ID_PARA_get_month,              //�߂܂�����
  ID_PARA_get_day,              //�߂܂�����
  ID_PARA_birth_year,             //���܂ꂽ�N
  ID_PARA_birth_month,            //���܂ꂽ��
  ID_PARA_birth_day,              //���܂ꂽ��
  ID_PARA_get_place,              //�߂܂����ꏊ
  ID_PARA_birth_place,            //���܂ꂽ�ꏊ
  ID_PARA_pokerus,              //�|�P���X
  ID_PARA_get_ball,             //�߂܂����{�[��
  ID_PARA_get_level,              //�߂܂������x��
  ID_PARA_oyasex,               //�e�̐���
*/

}

static void SetPartyPoke( NAKAHIRO_MAIN_WORK * wk )
{
	static const u32 mons[] = {
		MONSNO_HUSIGIDANE,
		MONSNO_HITOKAGE,
		MONSNO_ZENIGAME,
	};
	POKEMON_PARAM * pp;
	GFL_MSGDATA * man;
	STRBUF * str;
	u32	i;

	man = GFL_MSG_Create(
						GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_nakahiro_dat, wk->heapID );
	str = GFL_MSG_CreateString( man, pokename );

	for( i=0; i<3; i++ ){
		pp = PP_Create( mons[i], 50, 0, wk->heapID );
    PP_Put( pp, ID_PARA_oyaname, (u32)str );
    PP_Put( pp, ID_PARA_oyasex, PTL_SEX_MALE );
		if( i & 1 ){
//			PP_Put( pp, ID_PARA_item, 1 );
//#define ITEM_GURASUMEERU		( 137 )		// �O���X���[��
			PP_Put( pp, ID_PARA_item, 137 );
		}
		PokeParty_Add( wk->box_data.pokeparty, pp );
		GFL_HEAP_FreeMemory( pp );
	}

  GFL_STR_DeleteBuffer( str );
	GFL_MSG_Delete( man );
}
