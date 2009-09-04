//============================================================================================
/**
 * @file	b_bag_bmp.c
 * @brief	�퓬�p�o�b�O��� BMP�֘A
 * @author	Hiroyuki Nakamura
 * @date	09.08.25
 */
//============================================================================================
#include <gflib.h>

#include "gamesystem/msgspeed.h"
#include "system/bmp_winframe.h"
/*
#include "system/window.h"
#include "system/fontproc.h"
#include "system/msgdata.h"
#include "system/numfont.h"
#include "system/wordset.h"
#include "system/pmfprint.h"
#include "system/buflen.h"
*/
//#include "system/clact_tool.h"
/*��[GS_CONVERT_TAG]*/
/*
#include "battle/battle_common.h"
#include "battle/fight_tool.h"
#include "msg/msg.naix"
*/
#include "msg/msg_b_bag.h"
/*
#include "itemtool/item.h"
*/
#include "b_app_tool.h"

#include "b_bag.h"
#include "b_bag_main.h"
#include "b_bag_bmp.h"
#include "b_bag_bmp_def.h"


//============================================================================================
//	�萔��`
//============================================================================================
/*
#define	PCOL_N_BLACK	( PRINTSYS_LSB_MAKE( 1, 2, 0 ) )		// �t�H���g�J���[�F��
#define	PCOL_N_WHITE	( PCOL_N_BLACK )						// �t�H���g�J���[�F��
#define	PCOL_N_BLUE		( PRINTSYS_LSB_MAKE( 7, 8, 0 ) )		// �t�H���g�J���[�F��
#define	PCOL_N_RED		( PRINTSYS_LSB_MAKE( 3, 4, 0 ) )		// �t�H���g�J���[�F��
#define	PCOL_BTN		( PRINTSYS_LSB_MAKE( 3, 2, 1 ) )		// �{�^���t�H���g
*/

#define	FCOL_S12W		( PRINTSYS_LSB_Make(15,2,0) )	// �t�H���g�J���[�F��
//#define	FCOL_T12B		( PRINTSYS_LSB_Make(1,2,15) )	// �t�H���g�J���[�F��


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void BBAG_Page1BmpWrite( BBAG_WORK * wk );
static void BBAG_Page2BmpWrite( BBAG_WORK * wk );
static void BBAG_Page3BmpWrite( BBAG_WORK * wk );


//============================================================================================
//	�O���[�o���ϐ�
//============================================================================================
/*
extern GFL_BMPWIN* GFL_BMPWIN_Create( u8 frmnum, u8 posx, u8 posy, u8 sizx, u8 sizy, u8 palnum, u8 dir );
GFL_BMP_CHRAREA_GET_B
*/

// ��b�E�B���h�E�f�[�^
static const u8 CommBmpData[] = {
	WIN_TALK_FRM, WIN_TALK_PX, WIN_TALK_PY, WIN_TALK_SX, WIN_TALK_SY, WIN_TALK_PAL
};

// �y�[�W�P�̃E�B���h�E�f�[�^
static const u8 Page1_BmpData[][6] =
{
	// �y�[�W�P�̃E�B���h�E�f�[�^
	{	// �uHP/PP�����ӂ��v
		WIN_P1_HP_FRM, WIN_P1_HP_PX, WIN_P1_HP_PY, WIN_P1_HP_SX, WIN_P1_HP_SY, WIN_P1_HP_PAL
	},
	{	// �u���傤���������ӂ��v
		WIN_P1_ZYOUTAI_FRM, WIN_P1_ZYOUTAI_PX, WIN_P1_ZYOUTAI_PY, WIN_P1_ZYOUTAI_SX, WIN_P1_ZYOUTAI_SY, WIN_P1_ZYOUTAI_PAL
	},
	{	// �u�{�[���v
		WIN_P1_BALL_FRM, WIN_P1_BALL_PX, WIN_P1_BALL_PY, WIN_P1_BALL_SX, WIN_P1_BALL_SY, WIN_P1_BALL_PAL
	},
	{	// �u����Ƃ��悤�v
		WIN_P1_BATTLE_FRM, WIN_P1_BATTLE_PX, WIN_P1_BATTLE_PY, WIN_P1_BATTLE_SX, WIN_P1_BATTLE_SY, WIN_P1_BATTLE_PAL
	},
	{	// �u�������ɂ������ǂ����v
		WIN_P1_LASTITEM_FRM, WIN_P1_LASTITEM_PX, WIN_P1_LASTITEM_PY, WIN_P1_LASTITEM_SX, WIN_P1_LASTITEM_SY, WIN_P1_LASTITEM_PAL
	},

	// �y�[�W�Q�̃E�B���h�E�f�[�^
	{	// ����P
		WIN_P2_NAME1_FRM, WIN_P2_NAME1_PX, WIN_P2_NAME1_PY, WIN_P2_NAME1_SX, WIN_P2_NAME1_SY, WIN_P2_NAME1_PAL
	},
	{	// ����P
		WIN_P2_NUM1_FRM, WIN_P2_NUM1_PX, WIN_P2_NUM1_PY, WIN_P2_NUM1_SX, WIN_P2_NUM1_SY, WIN_P2_NUM1_PAL
	},
	{	// ����Q
		WIN_P2_NAME2_FRM, WIN_P2_NAME2_PX, WIN_P2_NAME2_PY, WIN_P2_NAME2_SX, WIN_P2_NAME2_SY, WIN_P2_NAME2_PAL
	},
	{	// ����Q
		WIN_P2_NUM2_FRM, WIN_P2_NUM2_PX, WIN_P2_NUM2_PY, WIN_P2_NUM2_SX, WIN_P2_NUM2_SY, WIN_P2_NUM2_PAL
	},
	{	// ����R
		WIN_P2_NAME3_FRM, WIN_P2_NAME3_PX, WIN_P2_NAME3_PY, WIN_P2_NAME3_SX, WIN_P2_NAME3_SY, WIN_P2_NAME3_PAL
	},
	{	// ����R
		WIN_P2_NUM3_FRM, WIN_P2_NUM3_PX, WIN_P2_NUM3_PY, WIN_P2_NUM3_SX, WIN_P2_NUM3_SY, WIN_P2_NUM3_PAL
	},
	{	// ����S
		WIN_P2_NAME4_FRM, WIN_P2_NAME4_PX, WIN_P2_NAME4_PY, WIN_P2_NAME4_SX, WIN_P2_NAME4_SY, WIN_P2_NAME4_PAL
	},
	{	// ����S
		WIN_P2_NUM4_FRM, WIN_P2_NUM4_PX, WIN_P2_NUM4_PY, WIN_P2_NUM4_SX, WIN_P2_NUM4_SY, WIN_P2_NUM4_PAL
	},
	{	// ����T
		WIN_P2_NAME5_FRM, WIN_P2_NAME5_PX, WIN_P2_NAME5_PY, WIN_P2_NAME5_SX, WIN_P2_NAME5_SY, WIN_P2_NAME5_PAL
	},
	{	// ����T
		WIN_P2_NUM5_FRM, WIN_P2_NUM5_PX, WIN_P2_NUM5_PY, WIN_P2_NUM5_SX, WIN_P2_NUM5_SY, WIN_P2_NUM5_PAL
	},
	{	// ����U
		WIN_P2_NAME6_FRM, WIN_P2_NAME6_PX, WIN_P2_NAME6_PY, WIN_P2_NAME6_SX, WIN_P2_NAME6_SY, WIN_P2_NAME6_PAL
	},
	{	// ����U
		WIN_P2_NUM6_FRM, WIN_P2_NUM6_PX, WIN_P2_NUM6_PY, WIN_P2_NUM6_SX, WIN_P2_NUM6_SY, WIN_P2_NUM6_PAL
	},
	{	// ����P�i�X���b�v�p�j
		WIN_P2_NAME1_FRM, WIN_P2_NAME1_PX, WIN_P2_NAME1_PY, WIN_P2_NAME1_SX, WIN_P2_NAME1_SY, WIN_P2_NAME1_PAL
	},
	{	// ����P�i�X���b�v�p�j
		WIN_P2_NUM1_FRM, WIN_P2_NUM1_PX, WIN_P2_NUM1_PY, WIN_P2_NUM1_SX, WIN_P2_NUM1_SY, WIN_P2_NUM1_PAL
	},
	{	// ����Q�i�X���b�v�p�j
		WIN_P2_NAME2_FRM, WIN_P2_NAME2_PX, WIN_P2_NAME2_PY, WIN_P2_NAME2_SX, WIN_P2_NAME2_SY, WIN_P2_NAME2_PAL
	},
	{	// ����Q�i�X���b�v�p�j
		WIN_P2_NUM2_FRM, WIN_P2_NUM2_PX, WIN_P2_NUM2_PY, WIN_P2_NUM2_SX, WIN_P2_NUM2_SY, WIN_P2_NUM2_PAL
	},
	{	// ����R�i�X���b�v�p�j
		WIN_P2_NAME3_FRM, WIN_P2_NAME3_PX, WIN_P2_NAME3_PY, WIN_P2_NAME3_SX, WIN_P2_NAME3_SY, WIN_P2_NAME3_PAL
	},
	{	// ����R�i�X���b�v�p�j
		WIN_P2_NUM3_FRM, WIN_P2_NUM3_PX, WIN_P2_NUM3_PY, WIN_P2_NUM3_SX, WIN_P2_NUM3_SY, WIN_P2_NUM3_PAL
	},
	{	// ����S�i�X���b�v�p�j
		WIN_P2_NAME4_FRM, WIN_P2_NAME4_PX, WIN_P2_NAME4_PY, WIN_P2_NAME4_SX, WIN_P2_NAME4_SY, WIN_P2_NAME4_PAL
	},
	{	// ����S�i�X���b�v�p�j
		WIN_P2_NUM4_FRM, WIN_P2_NUM4_PX, WIN_P2_NUM4_PY, WIN_P2_NUM4_SX, WIN_P2_NUM4_SY, WIN_P2_NUM4_PAL
	},
	{	// ����T�i�X���b�v�p�j
		WIN_P2_NAME5_FRM, WIN_P2_NAME5_PX, WIN_P2_NAME5_PY, WIN_P2_NAME5_SX, WIN_P2_NAME5_SY, WIN_P2_NAME5_PAL
	},
	{	// ����T�i�X���b�v�p�j
		WIN_P2_NUM5_FRM, WIN_P2_NUM5_PX, WIN_P2_NUM5_PY, WIN_P2_NUM5_SX, WIN_P2_NUM5_SY, WIN_P2_NUM5_PAL
	},
	{	// ����U�i�X���b�v�p�j
		WIN_P2_NAME6_FRM, WIN_P2_NAME6_PX, WIN_P2_NAME6_PY, WIN_P2_NAME6_SX, WIN_P2_NAME6_SY, WIN_P2_NAME6_PAL
	},
	{	// ����U�i�X���b�v�p�j
		WIN_P2_NUM6_FRM, WIN_P2_NUM6_PX, WIN_P2_NUM6_PY, WIN_P2_NUM6_SX, WIN_P2_NUM6_SY, WIN_P2_NUM6_PAL
	},
	{	// �|�P�b�g��
		WIN_P2_POCKET_FRM, WIN_P2_POCKET_PX, WIN_P2_POCKET_PY, WIN_P2_POCKET_SX, WIN_P2_POCKET_SY, WIN_P2_POCKET_PAL
	},
	{	// �y�[�W���i�A�C�e�����j
		WIN_P2_PAGENUM_FRM, WIN_P2_PAGENUM_PX, WIN_P2_PAGENUM_PY, WIN_P2_PAGENUM_SX, WIN_P2_PAGENUM_SY, WIN_P2_PAGENUM_PAL
	},

	// �y�[�W�R�̃E�B���h�E�f�[�^
	{	// ���
		WIN_P3_NAME_FRM, WIN_P3_NAME_PX, WIN_P3_NAME_PY, WIN_P3_NAME_SX, WIN_P3_NAME_SY, WIN_P3_NAME_PAL
	},
	{	// ��
		WIN_P3_NUM_FRM, WIN_P3_NUM_PX, WIN_P3_NUM_PY, WIN_P3_NUM_SX, WIN_P3_NUM_SY, WIN_P3_NUM_PAL
	},
	{	// ����
		WIN_P3_INFO_FRM, WIN_P3_INFO_PX, WIN_P3_INFO_PY, WIN_P3_INFO_SX, WIN_P3_INFO_SY, WIN_P3_INFO_PAL
	},
	{	// �u�����v
		WIN_P3_USE_FRM, WIN_P3_USE_PX, WIN_P3_USE_PY, WIN_P3_USE_SX, WIN_P3_USE_SY, WIN_P3_USE_PAL
	}
};



//--------------------------------------------------------------------------------------------
/**
 * BMP�E�B���h�E������
 *
 * @param	wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_BmpInit( BBAG_WORK * wk )
{
//	OS_Printf( "BBAG2 : %d\n", WIN_P2_POCKET_CGX+WIN_P2_POCKET_SX*WIN_P2_POCKET_SY );
//	OS_Printf( "BBAG3 : %d\n", WIN_P3_USE_CGX+WIN_P3_USE_SX*WIN_P3_USE_SY );
	wk->talk_win = GFL_BMPWIN_Create(
									CommBmpData[0],
									CommBmpData[1], CommBmpData[2],
									CommBmpData[3], CommBmpData[4],
									CommBmpData[5],
									GFL_BMP_CHRAREA_GET_B );
	BattleBag_BmpAdd( wk, wk->page );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W���Ƃ�BMP�E�B���h�E�ǉ�
 *
 * @param	wk		���[�N
 * @param	page	�y�[�W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_BmpAdd( BBAG_WORK * wk, u32 page )
{
	const u8 * dat;
	u32	i;

	// �|�P�b�g�I���y�[�W
	dat = Page1_BmpData[0];
	for( i=0; i<WIN_MAX; i++ ){
		wk->add_win[i] = GFL_BMPWIN_Create(
												dat[0],
												dat[1], dat[2],
												dat[3], dat[4],
												dat[5],
												GFL_BMP_CHRAREA_GET_B );
		dat += 6;
	}

#if 0
	const u8 * dat;
	u32	i;

	switch( page ){
	case BBAG_PAGE_POCKET:	// �|�P�b�g�I���y�[�W
		dat = Page1_BmpData[0];
		wk->bmp_add_max = WIN_P1_MAX;
		break;
	case BBAG_PAGE_MAIN:	// �A�C�e���I���y�[�W
		dat = Page2_BmpData[0];
		wk->bmp_add_max = WIN_P2_MAX;
		break;
	case BBAG_PAGE_ITEM:	// �A�C�e���g�p�y�[�W
		dat = Page3_BmpData[0];
		wk->bmp_add_max = WIN_P3_MAX;
		break;
	}

//	wk->add_win = GFL_BMPWIN_Init( wk->dat->heap, wk->bmp_add_max );

	for( i=0; i<wk->bmp_add_max; i++ ){
//		GF_BGL_BmpWinAddEx( wk->bgl, &wk->add_win[i], &dat[i] );
		wk->add_win[i] = GFL_BMPWIN_Create(
												dat[0],
												dat[1], dat[2],
												dat[3], dat[4],
												dat[5],
												GFL_BMP_CHRAREA_GET_B );
		dat += 6;
	}
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �ǉ�BMP�E�B���h�E�폜
 *
 * @param	wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_BmpFree( BBAG_WORK * wk )
{
	u32	i;

	for( i=0; i<WIN_MAX; i++ ){
		GFL_BMPWIN_Delete( wk->add_win[i] );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�E�B���h�E�S�폜
 *
 * @param	wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_BmpFreeAll( BBAG_WORK * wk )
{
	u32	i;

	BattleBag_BmpFree( wk );
	GFL_BMPWIN_Delete( wk->talk_win );
}

//--------------------------------------------------------------------------------------------
/**
 * BMP��������
 *
 * @param	wk		���[�N
 * @param	page	�y�[�W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_BmpWrite( BBAG_WORK * wk, u32 page )
{
	switch( page ){
	case BBAG_PAGE_POCKET:	// �|�P�b�g�I���y�[�W
		BBAG_Page1BmpWrite( wk );
		break;
	case BBAG_PAGE_MAIN:	// �A�C�e���I���y�[�W
		BBAG_Page2BmpWrite( wk );
		break;
	case BBAG_PAGE_ITEM:	// �A�C�e���g�p�y�[�W
		BBAG_Page3BmpWrite( wk );
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ������\��
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 * @param	widx	�E�B���h�E�C���f�b�N�X
 * @param	midx	���b�Z�[�W�C���f�b�N�X
 * @param	py		�\��Y���W
 * @param	col		�J���[
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_StrPut( BBAG_WORK * wk, u32 widx, u32 midx, u32 py, u16 col )
{
	GFL_BMPWIN * win;
	STRBUF * str;
	u32	siz;
	u32	px;

	win = wk->add_win[widx];
	str = GFL_MSG_CreateString( wk->mman, midx );
	siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	px = ( GFL_BMPWIN_GetSizeX(win) * 8 - siz ) / 2;
	PRINTSYS_PrintQueColor( wk->que, GFL_BMPWIN_GetBmp(win), px, py, str, wk->dat->font, col );
	GFL_STR_DeleteBuffer( str );

  GFL_BMPWIN_MakeTransWindow_VBlank( win );
}

// �|�P�b�g���\�����W
#define	P1_ZYOUTAI_PY	( 0 )
#define	P1_KAIFUKU2_PY	( 16 )
#define	P1_HP_PY		( 0 )
#define	P1_KAIFUKU3_PY	( 16 )
#define	P1_BATTLE_PY	( 0 )
#define	P1_BALL_PY		( 0 )
#define	P1_LASTITEM_PY	( 0 )
/*
	BBAG_StrPut( wk, WIN_P1_HP, mes_b_bag_01_000, P1_HP_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_HP, mes_b_bag_01_001, P1_KAIFUKU3_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_ZYOUTAI, mes_b_bag_01_100, P1_ZYOUTAI_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_ZYOUTAI, mes_b_bag_01_101, P1_KAIFUKU2_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_BALL, mes_b_bag_01_500, P1_BALL_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_BATTLE, mes_b_bag_01_400, P1_BATTLE_PY, FCOL_S12W );
*/

//--------------------------------------------------------------------------------------------
/**
 * �|�P�b�g�I���y�[�W�\��
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page1BmpWrite( BBAG_WORK * wk )
{
	GFL_BMP_DATA * bmp;
	u32	i;

	for( i=WIN_P1_HP; i<=WIN_P1_LASTITEM; i++ ){
		bmp = GFL_BMPWIN_GetBmp( wk->add_win[i] );
		GFL_BMP_Clear( bmp, 0 );
	}

	BBAG_StrPut( wk, WIN_P1_HP, mes_b_bag_01_000, P1_HP_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_HP, mes_b_bag_01_001, P1_KAIFUKU3_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_ZYOUTAI, mes_b_bag_01_100, P1_ZYOUTAI_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_ZYOUTAI, mes_b_bag_01_101, P1_KAIFUKU2_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_BALL, mes_b_bag_01_500, P1_BALL_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_BATTLE, mes_b_bag_01_400, P1_BATTLE_PY, FCOL_S12W );
	if( wk->dat->used_item != ITEM_DUMMY_DATA ){
		STRBUF * str = GFL_MSG_CreateString( wk->mman, mes_b_bag_01_600 );
/*
		GF_STR_PrintColor(
			&wk->add_win[WIN_P1_LASTITEM], FONT_TOUCH,
			str, 0, P1_LASTITEM_PY, MSG_NO_PUT, PCOL_BTN, NULL );
*/
		PRINTSYS_PrintQueColor(
			wk->que, GFL_BMPWIN_GetBmp( wk->add_win[WIN_P1_LASTITEM] ),
			0, P1_LASTITEM_PY, str, wk->dat->font, FCOL_S12W );

		GFL_STR_DeleteBuffer( str );
	  GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[WIN_P1_LASTITEM] );
	}
}


// �A�C�e�����̃��b�Z�[�WID
static const u32 ItemStrGmm[][2] =
{	// ���O�Ax???
	{ mes_b_bag_02_000, mes_b_bag_02_001 },
	{ mes_b_bag_02_100, mes_b_bag_02_101 },
	{ mes_b_bag_02_200, mes_b_bag_02_201 },
	{ mes_b_bag_02_300, mes_b_bag_02_301 },
	{ mes_b_bag_02_400, mes_b_bag_02_401 },
	{ mes_b_bag_02_500, mes_b_bag_02_501 }
};

#define	P2_ITEMNAME_PY	( 8-1 )		// �A�C�e���I���y�[�W�̃A�C�e�����\��Y���W

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�����\��
 *
 * @param	wk			�퓬�o�b�O�̃��[�N
 * @param	dat_pos		�f�[�^�ʒu�i�|�P�b�g���̃A�C�e���ʒu�j
 * @param	put_pos		�\���ʒu
 * @param	widx		�E�B���h�E�C���f�b�N�X
 * @param	fidx		�t�H���g�C���f�b�N�X
 * @param	col			�J���[
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ItemNamePut(
				BBAG_WORK * wk, u32 dat_pos, u32 put_pos, u32 widx, u32 fidx, u32 col )
{
	GFL_BMPWIN * win;
	GFL_BMP_DATA * bmp;
	STRBUF * str;
	u32	siz;
	u32	px;

	win = wk->add_win[widx];
	bmp = GFL_BMPWIN_GetBmp( win );
	GFL_BMP_Clear( bmp, 0 );

	if( wk->pocket[wk->poke_id][dat_pos].id != 0 ){
		str = GFL_MSG_CreateString( wk->mman, ItemStrGmm[put_pos][0] );
		WORDSET_RegisterItemName( wk->wset, 0, wk->pocket[wk->poke_id][dat_pos].id );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		siz = PRINTSYS_GetStrWidth( wk->msg_buf, wk->dat->font, 0 );
		px  = ( GFL_BMPWIN_GetSizeX(win) * 8 - siz ) / 2;
//		GF_STR_PrintColor( win, fidx, wk->msg_buf, px, P2_ITEMNAME_PY, MSG_NO_PUT, col, NULL );
		PRINTSYS_PrintQueColor(
			wk->que, bmp, px, P2_ITEMNAME_PY, wk->msg_buf, wk->dat->font, FCOL_S12W );
		GFL_STR_DeleteBuffer( str );
	}

	GFL_BMPWIN_MakeTransWindow_VBlank( win );
}

//--------------------------------------------------------------------------------------------
/**
 * ���\��
 *
 * @param	wk			�퓬�o�b�O�̃��[�N
 * @param	dat_pos		�f�[�^�ʒu�i�|�P�b�g���̃A�C�e���ʒu�j
 * @param	put_pos		�\���ʒu
 * @param	widx		�E�B���h�E�C���f�b�N�X
 * @param	fidx		�t�H���g�C���f�b�N�X
 * @param	py			�\��Y���W
 * @param	col			�J���[
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ItemNumPut(
				BBAG_WORK * wk, u32 dat_pos, u32 put_pos, u32 widx, u32 fidx, u32 py, u32 col )
{
	STRBUF * str;
	GFL_BMPWIN * win;
	GFL_BMP_DATA * bmp;

	win = wk->add_win[widx];
	bmp = GFL_BMPWIN_GetBmp( win );
	GFL_BMP_Clear( bmp, 0 );

	if( wk->pocket[wk->poke_id][dat_pos].no != 0 ){
		str = GFL_MSG_CreateString( wk->mman, ItemStrGmm[put_pos][1] );

		WORDSET_RegisterNumber(
			wk->wset, 0, wk->pocket[wk->poke_id][dat_pos].no,
			3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );

//		GF_STR_PrintColor( win, fidx, wk->msg_buf, 0, py, MSG_NO_PUT, col, NULL );
		PRINTSYS_PrintQueColor(
			wk->que, bmp, 0, py, wk->msg_buf, wk->dat->font, FCOL_S12W );

		GFL_STR_DeleteBuffer( str );
	}

	GFL_BMPWIN_MakeTransWindow_VBlank( win );
}

#define	P2_ITEMNUM_PY	( 4 )	// �A�C�e���I���y�[�W�̌��\��Y���W

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�����ƌ��\���i�ʁj
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 * @param	pos		�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_P2_ItemPut( BBAG_WORK * wk, u32 pos )
{
	u32	win_pos;
	u32	dat_pos;

	dat_pos = wk->dat->item_scr[wk->poke_id]*6+pos;

	if( wk->p2_swap == 0 ){
		win_pos = WIN_P2_NAME1;
	}else{
		win_pos = WIN_P2_NAME1_S;
	}

	BBAG_ItemNamePut( wk, dat_pos, pos, win_pos+pos*2, 0, 0 );
	if( wk->dat->mode != BBAG_MODE_SHOOTER ){
		BBAG_ItemNumPut(
			wk, dat_pos, pos, win_pos+1+pos*2, 0, P2_ITEMNUM_PY, 0 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�����ƌ��\���i�S�́j
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_Page2_StrItemPut( BBAG_WORK * wk )
{
	u16	i;

	GFL_BG_FillScreen( GFL_BG_FRAME1_S, 0, 0, 0, 32, 19, GFL_BG_SCRWRT_PALIN );

	for( i=0; i<6; i++ ){
		BBAG_P2_ItemPut( wk, i );
	}

	wk->p2_swap ^= 1;
}


#define	P2_PAGE_NUM_PY	( 4 )	// �A�C�e���I���y�[�W�̃y�[�W���\��Y���W

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W���\��
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_Page2_StrPageNumPut( BBAG_WORK * wk )
{
	GFL_BMPWIN * win;
	GFL_BMP_DATA * bmp;
	STRBUF * str;
	u32	siz;
	u32	px;

	win = wk->add_win[WIN_P2_PAGENUM];
	bmp = GFL_BMPWIN_GetBmp( win );
	GFL_BMP_Clear( bmp, 0 );

	str = GFL_MSG_CreateString( wk->mman, mes_b_bag_02_800 );
	siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	px  = ( GFL_BMPWIN_GetSizeX(win) * 8 - siz ) / 2;
/*
	GF_STR_PrintColor(
		win, FONT_SYSTEM, str, px, P2_PAGE_NUM_PY, MSG_NO_PUT, PCOL_N_WHITE, NULL );
*/
	PRINTSYS_PrintQueColor(
			wk->que, bmp, px, P2_PAGE_NUM_PY, str, wk->dat->font, FCOL_S12W );
	GFL_STR_DeleteBuffer( str );

	str = GFL_MSG_CreateString( wk->mman, mes_b_bag_02_801 );
	WORDSET_RegisterNumber(
		wk->wset, 0, wk->scr_max[wk->poke_id]+1,
		2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
//	GF_STR_PrintColor(
//		win, FONT_SYSTEM, wk->msg_buf, px+siz, P2_PAGE_NUM_PY, MSG_NO_PUT, PCOL_N_WHITE, NULL );
	PRINTSYS_PrintQueColor(
			wk->que, bmp, px+siz, P2_PAGE_NUM_PY, wk->msg_buf, wk->dat->font, FCOL_S12W );
	GFL_STR_DeleteBuffer( str );

	str = GFL_MSG_CreateString( wk->mman, mes_b_bag_02_802 );
	WORDSET_RegisterNumber(
		wk->wset, 0, wk->dat->item_scr[wk->poke_id]+1,
		2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );

	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	siz = PRINTSYS_GetStrWidth( wk->msg_buf, wk->dat->font, 0 );
//	GF_STR_PrintColor(
//		win, FONT_SYSTEM, wk->msg_buf, px-siz, P2_PAGE_NUM_PY, MSG_NO_PUT, PCOL_N_WHITE, NULL );
	PRINTSYS_PrintQueColor(
			wk->que, bmp, px-siz, P2_PAGE_NUM_PY, wk->msg_buf, wk->dat->font, FCOL_S12W );
	GFL_STR_DeleteBuffer( str );

	GFL_BMPWIN_MakeTransWindow_VBlank( win );
}

// �A�C�e���I���y�[�W�̃|�P�b�g���\�����W
#define	P2_NEXT_PY			( 8 )
#define	P2_POCKET_HP1_PY	( 4 )
#define	P2_POCKET_HP2_PY	( P2_POCKET_HP1_PY+16 )
#define	P2_POCKET_ST1_PY	( 4 )
#define	P2_POCKET_ST2_PY	( P2_POCKET_ST1_PY+16 )
#define	P2_POCKET_BALL_PY	( 12 )
#define	P2_POCKET_BATL_PY	( 12 )

//--------------------------------------------------------------------------------------------
/**
 * �|�P�b�g���\��
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page2PocketNamePut( BBAG_WORK * wk )
{
	GFL_BMP_DATA * bmp;

	bmp = GFL_BMPWIN_GetBmp( wk->add_win[WIN_P2_POCKET] );
	GFL_BMP_Clear( bmp, 0 );

	switch( wk->poke_id ){
	case BBAG_POKE_HPRCV:	// HP�񕜃|�P�b�g
		BBAG_StrPut(
			wk, WIN_P2_POCKET, mes_b_bag_02_700, P2_POCKET_HP1_PY, FCOL_S12W );
		BBAG_StrPut(
			wk, WIN_P2_POCKET, mes_b_bag_02_701, P2_POCKET_HP2_PY, FCOL_S12W );
		break;
	case BBAG_POKE_STRCV:	// ��ԉ񕜃|�P�b�g
		BBAG_StrPut(
			wk, WIN_P2_POCKET, mes_b_bag_02_702, P2_POCKET_ST1_PY, FCOL_S12W );
		BBAG_StrPut(
			wk, WIN_P2_POCKET, mes_b_bag_02_703, P2_POCKET_ST2_PY, FCOL_S12W );
		break;
	case BBAG_POKE_BALL:	// �{�[���|�P�b�g
		BBAG_StrPut(
			wk, WIN_P2_POCKET, mes_b_bag_02_704, P2_POCKET_BALL_PY, FCOL_S12W );
		break;
	case BBAG_POKE_BATTLE:	// �퓬�p�|�P�b�g
		BBAG_StrPut(
			wk, WIN_P2_POCKET, mes_b_bag_02_705, P2_POCKET_BATL_PY, FCOL_S12W );
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���I���y�[�W
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page2BmpWrite( BBAG_WORK * wk )
{
	BattleBag_Page2_StrItemPut( wk );
	if( wk->dat->mode != BBAG_MODE_SHOOTER ){
		BBAG_Page2PocketNamePut( wk );
	}
	BattleBag_Page2_StrPageNumPut( wk );
}

#define	P3_ITEMNAME_PY	( 0 )	// �A�C�e���g�p�y�[�W�̃A�C�e�����\��Y���W
#define	P3_ITEMINFO_PX	( 4 )	// �A�C�e���g�p�y�[�W�̃A�C�e�����\��X���W
#define	P3_ITEMINFO_PY	( 0 )	// �A�C�e���g�p�y�[�W�̃A�C�e�����\��Y���W
#define	P3_ITEMNUM_PY	( 0 )	// �A�C�e���g�p�y�[�W�̃A�C�e�����\��Y���W

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�����\��
 *
 * @param	wk			�퓬�o�b�O�̃��[�N
 * @param	dat_pos		�f�[�^�ʒu�i�|�P�b�g���̃A�C�e���ʒu�j
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_P3_ItemNamePut( BBAG_WORK * wk, u32 dat_pos )
{
	GFL_BMPWIN * win;
	GFL_BMP_DATA * bmp;
	STRBUF * str;

	win = wk->add_win[WIN_P3_NAME];
	str = GFL_MSG_CreateString( wk->mman, ItemStrGmm[0][0] );
	WORDSET_RegisterItemName( wk->wset, 0, wk->pocket[wk->poke_id][dat_pos].id );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );

	bmp = GFL_BMPWIN_GetBmp( win );
	GFL_BMP_Clear( bmp, 0 );
//	GF_STR_PrintColor(
//		win, FONT_SYSTEM, wk->msg_buf, 0, P3_ITEMNAME_PY, MSG_NO_PUT, PCOL_N_WHITE, NULL );
	PRINTSYS_PrintQueColor(
			wk->que, bmp, 0, P3_ITEMNAME_PY, wk->msg_buf, wk->dat->font, FCOL_S12W );
	GFL_STR_DeleteBuffer( str );

	GFL_BMPWIN_MakeTransWindow_VBlank( win );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�����\��
 *
 * @param	wk			�퓬�o�b�O�̃��[�N
 * @param	dat_pos		�f�[�^�ʒu�i�|�P�b�g���̃A�C�e���ʒu�j
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_P3_ItemInfoPut( BBAG_WORK * wk, u32 dat_pos )
{
	GFL_BMPWIN * win;
	STRBUF * buf;
	
	win = wk->add_win[WIN_P3_INFO];
	buf = GFL_STR_CreateBuffer( BUFLEN_ITEM_INFO, wk->dat->heap );
	ITEM_GetInfo( buf, wk->pocket[wk->poke_id][dat_pos].id, wk->dat->heap );
//	GF_STR_PrintColor(
//		win, FONT_SYSTEM, buf, P3_ITEMINFO_PX, P3_ITEMINFO_PY, MSG_NO_PUT, PCOL_N_BLACK, NULL );
	PRINTSYS_PrintQueColor(
			wk->que, GFL_BMPWIN_GetBmp(win), P3_ITEMINFO_PX, P3_ITEMINFO_PY, buf, wk->dat->font, FCOL_S12W );
	GFL_STR_DeleteBuffer( buf );

	GFL_BMPWIN_MakeTransWindow_VBlank( win );
}

#define	P3_USE_PY	( 6-1 )		// �u�����v�\��Y���W

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p�y�[�W
 *
 * @param	wk		�퓬�o�b�O�̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page3BmpWrite( BBAG_WORK * wk )
{
	GFL_BMP_DATA * bmp;
	u32	i;
	u32	dat_pos;

	for( i=WIN_P3_NAME; i<=WIN_P3_USE; i++ ){
		bmp = GFL_BMPWIN_GetBmp( wk->add_win[i] );
		GFL_BMP_Clear( bmp, 0 );
	}

	dat_pos = wk->dat->item_scr[wk->poke_id]*6+wk->dat->item_pos[wk->poke_id];

	BBAG_P3_ItemNamePut( wk, dat_pos );
	if( wk->dat->mode != BBAG_MODE_SHOOTER ){
		BBAG_ItemNumPut( wk, dat_pos, 0, WIN_P3_NUM, 0, P3_ITEMNUM_PY, 0 );
	}
	BBAG_P3_ItemInfoPut( wk, dat_pos );
	BBAG_StrPut( wk, WIN_P3_USE, mes_b_bag_03_000, P3_USE_PY, FCOL_S12W );
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�\��
 *
 * @param	wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_TalkMsgSet( BBAG_WORK * wk )
{
	GFL_BMP_DATA * bmp;

//	BmpWinFrame_Write( &wk->talk_win, WINDOW_TRANS_OFF, TALK_WIN_CGX_POS, BBAG_PAL_TALK_WIN );
	BmpWinFrame_Write( wk->talk_win, WINDOW_TRANS_OFF ,1, BBAG_PAL_TALK_WIN );
//	GFL_BMPWIN_MakeFrameScreen( wk->talk_win, 1, BBAG_PAL_TALK_WIN );

	bmp = GFL_BMPWIN_GetBmp( wk->talk_win );
//	GFL_BMP_Clear( bmp, FBMP_COL_WHITE );
	GFL_BMP_Clear( bmp, 15 );
	BattleBag_TalkMsgStart( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�\���J�n
 *
 * @param	wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_TalkMsgStart( BBAG_WORK * wk )
{
//	MsgPrintSkipFlagSet( MSG_SKIP_ON );
	wk->stream = PRINTSYS_PrintStream(
									wk->talk_win,
									0, 0, wk->msg_buf, wk->dat->font,
									MSGSPEED_GetWait(),
									wk->tcbl,
									10,		// tcbl pri
									wk->dat->heap,
									15 );	// clear color

	GFL_BMPWIN_MakeTransWindow_VBlank( wk->talk_win );

/*
	MsgPrintSkipFlagSet( MSG_SKIP_ON );
	wk->midx = GF_STR_PrintSimple(
				&wk->talk_win, FONT_TALK, wk->msg_buf,
				0, 0, BattleWorkConfigMsgSpeedGet(wk->dat->bw), NULL );
*/
}


//--------------------------------------------------------------------------------------------
/**
 * �o�b�O���g�p�A�C�e���̃��b�Z�[�W�Z�b�g
 *
 * @param	wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_ItemUseMsgSet( BBAG_WORK * wk )
{
/*
	POKEMON_PARAM * pp;
	void * item;
	STRBUF * str;
	int	smn;
	
	item = GetItemArcData( wk->dat->ret_item, ITEM_GET_DATA, wk->dat->heap );
	smn  = BattleBag_SelMonsNoGet( wk );
	pp   = BattleWorkPokemonParamGet( wk->dat->bw, wk->dat->client_no, smn );

	// �\�̓K�[�h
	if( ItemBufParamGet( item, ITEM_PRM_ABILITY_GUARD ) != 0 ){
		MSGMAN_GetString( wk->mman, mes_b_bag_m02, wk->msg_buf );
	// �U���̓A�b�v
	}else if( ItemBufParamGet( item, ITEM_PRM_ATTACK_UP ) != 0 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_bag_m04 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// �h��̓A�b�v
	}else if( ItemBufParamGet( item, ITEM_PRM_DEFENCE_UP ) != 0 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_bag_m08 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// ���U�A�b�v
	}else if( ItemBufParamGet( item, ITEM_PRM_SP_ATTACK_UP ) != 0 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_bag_m05 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// ���h�A�b�v
	}else if( ItemBufParamGet( item, ITEM_PRM_SP_DEFENCE_UP ) != 0 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_bag_m07 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// �f�����A�b�v
	}else if( ItemBufParamGet( item, ITEM_PRM_AGILITY_UP ) != 0 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_bag_m06 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// �������A�b�v
	}else if( ItemBufParamGet( item, ITEM_PRM_HIT_UP ) != 0 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_bag_m09 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	// �N���e�B�J�����A�b�v
	}else if( ItemBufParamGet( item, ITEM_PRM_CRITICAL_UP ) != 0 ){
		str = GFL_MSG_CreateString( wk->mman, mes_b_bag_m03 );
		WORDSET_RegisterPokeNickName( wk->wset, 0, PPPPointerGet(pp) );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	}else{
		str = GFL_MSG_CreateString( wk->mman, mes_b_bag_m10 );
		WORDSET_RegisterItemName( wk->wset, 0, wk->dat->ret_item );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		GFL_STR_DeleteBuffer( str );
	}

	GFL_HEAP_FreeMemory( item );
*/
/*��[GS_CONVERT_TAG]*/
}
