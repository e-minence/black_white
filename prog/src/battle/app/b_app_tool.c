//============================================================================================
/**
 * @file		b_app_tool.c
 * @brief		�퓬�p�A�v�����ʏ���
 * @author	Hiroyuki Nakamura
 * @date		09.08.25
 */
//============================================================================================
#include <gflib.h>

#include "b_app_tool.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �J�[�\���\�����[�N
struct _BAPP_CURSOR_PUT_WORK {
	GFL_CLWK * clwk[BAPPTOOL_CURSOR_MAX];
	HEAPID	heapID;
	BOOL	vanish;
};

#define	BATTLE_CURSOR_ANIME		( 0 )		// �J�[�\���n�a�i�̃A�j���ԍ�


//============================================================================================
//	�J�[�\���֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���\�����[�N�쐬
 *
 * @param		heapID		�q�[�v�h�c
 *
 * @return	�J�[�\���\�����[�N
 */
//--------------------------------------------------------------------------------------------
BAPP_CURSOR_PUT_WORK * BAPPTOOL_CreateCursor( HEAPID heapID )
{
	BAPP_CURSOR_PUT_WORK * wk = GFL_HEAP_AllocMemory( heapID, sizeof(BAPP_CURSOR_PUT_WORK) );
	wk->heapID = heapID;
	wk->vanish = TRUE;
	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���\�����[�N�J��
 *
 * @param		wk		�J�[�\���\�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPPTOOL_FreeCursor( BAPP_CURSOR_PUT_WORK * wk )
{
	GFL_HEAP_FreeMemory( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ǉ�
 *
 * @param		wk			�J�[�\���\�����[�N
 * @param		clunit	�Z���A�N�^�[���j�b�g
 * @param		chrRes	�L�������\�[�X
 * @param		palRes	�p���b�g���\�[�X
 * @param		celRes	�Z�����\�[�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPPTOOL_AddCursor( BAPP_CURSOR_PUT_WORK * wk, GFL_CLUNIT * clunit, u32 chrRes, u32 palRes, u32 celRes )
{
	GFL_CLWK_DATA	dat;
	u32	i;

	dat.pos_x   = 0;
	dat.pos_y   = 0;
	dat.softpri = 0;
	dat.bgpri   = 1;

	for( i=0; i<BAPPTOOL_CURSOR_MAX; i++ ){
		dat.anmseq = BATTLE_CURSOR_ANIME + i;
		wk->clwk[i] = GFL_CLACT_WK_Create( clunit, chrRes, palRes, celRes, &dat, CLSYS_DRAW_SUB, wk->heapID );
		GFL_CLACT_WK_SetPlttOffs( wk->clwk[i], 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );
		GFL_CLACT_WK_SetAutoAnmFlag( wk->clwk[i], TRUE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���폜
 *
 * @param		wk			�J�[�\���\�����[�N
 * @param		clunit	�Z���A�N�^�[���j�b�g
 * @param		chrRes	�L�������\�[�X
 * @param		palRes	�p���b�g���\�[�X
 * @param		celRes	�Z�����\�[�X
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPPTOOL_DelCursor( BAPP_CURSOR_PUT_WORK * wk )
{
	u32	i;

	for( i=0; i<BAPPTOOL_CURSOR_MAX; i++ ){
		GFL_CLACT_WK_Remove( wk->clwk[i] );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���\���؂�ւ�
 *
 * @param		wk			�J�[�\���\�����[�N
 * @param		flg			TRUE = �\��, FALSE = ��\��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPPTOOL_VanishCursor( BAPP_CURSOR_PUT_WORK * wk, BOOL flg )
{
	if( wk->vanish != flg ){
		u32	i;

		for( i=0; i<BAPPTOOL_CURSOR_MAX; i++ ){
			GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], flg );
		}
		wk->vanish = flg;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ�
 *
 * @param		wk			�J�[�\���\�����[�N
 * @param		dat			���W�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPPTOOL_MoveCursorPoint( BAPP_CURSOR_PUT_WORK * wk, const CURSORMOVE_DATA * dat )
{
	GFL_CLACTPOS	pos;
	u8	sx, sy;

	sx = dat->sx >> 1;
	sy = dat->sy >> 1;

	// ����
	pos.x = dat->px - sx;
	pos.y = dat->py - sy;
	GFL_CLACT_WK_SetPos( wk->clwk[0], &pos, CLSYS_DRAW_SUB );
	// ����
	pos.x = dat->px - sx;
	pos.y = dat->py + sy;
	GFL_CLACT_WK_SetPos( wk->clwk[1], &pos, CLSYS_DRAW_SUB );
	// �E��
	pos.x = dat->px + sx;
	pos.y = dat->py - sy;
	GFL_CLACT_WK_SetPos( wk->clwk[2], &pos, CLSYS_DRAW_SUB );
	// �E��
	pos.x = dat->px + sx;
	pos.y = dat->py + sy;
	GFL_CLACT_WK_SetPos( wk->clwk[3], &pos, CLSYS_DRAW_SUB );
}


//============================================================================================
//	�����\���֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		BMP�E�B���h�E�X�N���[���]���i�P���j
 *
 * @param		util		PRINT_UTIL
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPPTOOL_PrintScreenTrans( PRINT_UTIL * util )
{
	GFL_BMPWIN_MakeScreen( util->win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(util->win) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BMP�E�B���h�E�X�N���[���]���i�e�[�u���Q�Ɓj
 *
 * @param		util		PRINT_UTIL
 * @param		tbl			�f�[�^�ԍ��e�[�u��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPPTOOL_SetStrScrn( PRINT_UTIL * util, const u8 * tbl )
{
	u32	i = 0;

	while(1){
		if( tbl[i] == BAPPTOOL_SET_STR_SCRN_END ){
			break;
		}
		BAPPTOOL_PrintScreenTrans( &util[tbl[i]] );
		i++;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		������]��
 *
 * @param		util		PRINT_UTIL
 * @param		que			PRINT_QUE
 * @param		max			PRINT_UTIL�̐�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPPTOOL_PrintUtilTrans( PRINT_UTIL * util, PRINT_QUE * que, u32 max )
{
	u32	i;

	PRINTSYS_QUE_Main( que );
	for( i=0; i<max; i++ ){
		PRINT_UTIL_Trans( &util[i], que );
	}
}
