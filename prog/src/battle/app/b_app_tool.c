//============================================================================================
/**
 * @file	b_app_tool.c
 * @brief	�퓬�p�A�v�����ʏ���
 * @author	Hiroyuki Nakamura
 * @date	09.08.25
 */
//============================================================================================
#include <gflib.h>

//#include "system/clact_tool.h"
/*��[GS_CONVERT_TAG]*/
//#include "battle_snd_def.h"


#include "b_app_tool.h"


//============================================================================================
//	�萔��`
//============================================================================================

#define	BAPP_CURSOR_MAX		( 4 )

// �I���J�[�\���ړ����[�N
struct _BAPP_CURSOR_MVWK {
//	BCURSOR_PTR	cursor;			// �J�[�\���f�[�^
//	const POINTSEL_WORK * cp_wk;	// �J�[�\���ʒu���[�N
	u8	cur_flg;				// �J�[�\��ON/OFF
	u8	cur_pos;				// �J�[�\���ʒu
	u8	old_pos;				// �O��̃J�[�\���ʒu
	u32	mv_tbl;					// �ړ��e�[�u��
};

struct _BAPP_CURSOR_PUT_WORK {
	GFL_CLWK * clwk[BAPP_CURSOR_MAX];
	const BAPP_CURSOR_PUT * put;
	HEAPID	heapID;
	BOOL	vanish;
};


// �L�[�S��
#define	PAD_KEY_ALL	( PAD_KEY_UP | PAD_KEY_DOWN | PAD_KEY_LEFT | PAD_KEY_RIGHT )


//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�쐬
 *
 * @param	heap	�q�[�vID
 *
 * @return	�J�[�\���ړ����[�N
 */
//--------------------------------------------------------------------------------------------
BAPP_CURSOR_MVWK * BAPP_CursorMoveWorkAlloc( u32 heap )
{
	BAPP_CURSOR_MVWK * wk = GFL_HEAP_AllocClearMemory( heap, sizeof(BAPP_CURSOR_MVWK) );
	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N���
 *
 * @param	heap	�q�[�vID
 *
 * @return	�J�[�\���ړ����[�N
 */
//--------------------------------------------------------------------------------------------
void BAPP_CursorMoveWorkFree( BAPP_CURSOR_MVWK * wk )
{
	GFL_HEAP_FreeMemory( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�擾�F�J�[�\���f�[�^
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @return	�J�[�\���f�[�^
 */
//--------------------------------------------------------------------------------------------
/*
BCURSOR_PTR BAPP_CursorMvWkGetBCURSOR_PTR( BAPP_CURSOR_MVWK * wk )
{
	return wk->cursor;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�擾�F�J�[�\���ʒu
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @return	�J�[�\���ʒu
 */
//--------------------------------------------------------------------------------------------
u8 BAPP_CursorMvWkGetPos( BAPP_CURSOR_MVWK * wk )
{
	return wk->cur_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�擾�F�J�[�\��ON/OFF
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @return	�J�[�\��ON/OFF
 */
//--------------------------------------------------------------------------------------------
u8 BAPP_CursorMvWkGetFlag( BAPP_CURSOR_MVWK * wk )
{
	return wk->cur_flg;
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�Z�b�g�F�J�[�\��ON/OFF
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	flg		ON/OFF�t���O
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPP_CursorMvWkSetFlag( BAPP_CURSOR_MVWK * wk, u8 flg )
{
	wk->cur_flg = flg;
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�Z�b�g�F�J�[�\���f�[�^
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	flg		�J�[�\���f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BAPP_CursorMvWkSetBCURSOR_PTR( BAPP_CURSOR_MVWK * wk, BCURSOR_PTR cur )
{
	wk->cursor = cur;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�Z�b�g�F�J�[�\���ʒu
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	pos		�J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPP_CursorMvWkSetPos( BAPP_CURSOR_MVWK * wk, u8 pos )
{
	wk->cur_pos = pos;

	if( wk->cur_flg == 1 ){
/*
		BCURSOR_PosSetON(
			wk->cursor,
			wk->cp_wk[wk->cur_pos].px, wk->cp_wk[wk->cur_pos].sx,
			wk->cp_wk[wk->cur_pos].py, wk->cp_wk[wk->cur_pos].sy );
*/
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ʒu������
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPP_CursorMvWkPosInit( BAPP_CURSOR_MVWK * wk )
{
	wk->cur_pos = 0;
	wk->old_pos = 0xff;
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�Z�b�g�F�J�[�\���ʒu���[�N
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	pwk		�J�[�\���ʒu���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BAPP_CursorMvWkSetPoint( BAPP_CURSOR_MVWK * wk, const POINTSEL_WORK * pwk )
{
	BAPP_CursorMvWkPosInit( wk );
	wk->cp_wk  = pwk;
	wk->mv_tbl = 0xffffffff;

	if( wk->cur_flg == 1 ){
		BCURSOR_PosSetON(
			wk->cursor, wk->cp_wk[0].px, wk->cp_wk[0].sx, wk->cp_wk[0].py, wk->cp_wk[0].sy );
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�Z�b�g�F�ړ��e�[�u��
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	mv_tbl	�ړ��e�[�u��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BAPP_CursorMvWkSetMvTbl( BAPP_CURSOR_MVWK * wk, u32 mv_tbl )
{
	wk->mv_tbl = mv_tbl;
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���\��
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @retval	"TRUE = �\��"
 * @retval	"FALSE = ��\��"
 */
//--------------------------------------------------------------------------------------------
static u8 BAPP_CursorOnOffCheck( BAPP_CURSOR_MVWK * wk )
{
	if( wk->cur_flg == 1 ){
		return TRUE;
	}

	if( GFL_UI_KEY_GetTrg() & ( PAD_KEY_ALL | PAD_BUTTON_CANCEL | PAD_BUTTON_DECIDE ) ){
/*��[GS_CONVERT_TAG]*/
		wk->cur_flg = 1;
/*
		BCURSOR_PosSetON(
			wk->cursor,
			wk->cp_wk[ wk->cur_pos ].px, wk->cp_wk[ wk->cur_pos ].sx,
			wk->cp_wk[ wk->cur_pos ].py, wk->cp_wk[ wk->cur_pos ].sy );
*/
//	Snd_SePlay( BCURSOR_MOVE_SE );
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �O�̃J�[�\���ʒu�Ɉړ����邩
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	mv		�ړ�����
 *
 * @retval	"TRUE = �ړ�����"
 * @retval	"FALSE = �ړ����Ȃ�"
 */
//--------------------------------------------------------------------------------------------
/*
static u8 BAPP_OldCursorSetCheck( const POINTSEL_WORK * pw, u8 mv )
{
	switch( mv ){
	case POINTSEL_MV_UP:
		if( pw->down & BAPP_CMV_RETBIT ){
			return TRUE;
		}
		break;
	case POINTSEL_MV_DOWN:
		if( pw->up & BAPP_CMV_RETBIT ){
			return TRUE;
		}
		break;
	case POINTSEL_MV_LEFT:
		if( pw->right & BAPP_CMV_RETBIT ){
			return TRUE;
		}
		break;
	case POINTSEL_MV_RIGHT:
		if( pw->left & BAPP_CMV_RETBIT ){
			return TRUE;
		}
		break;
	}
	return FALSE;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ�
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 BAPP_CursorMove( BAPP_CURSOR_MVWK * wk )
{
/*
	u8	lx, ly, rx, ry;
	u8	pm_ret;
	u8	mv;

	if( BAPP_CursorOnOffCheck( wk ) == FALSE ){ return BAPP_CMV_NONE; }

	if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
		pm_ret = POINTSEL_MoveVec( wk->cp_wk,NULL,NULL,NULL,NULL,wk->cur_pos,POINTSEL_MV_UP );
		mv     = POINTSEL_MV_UP;
	}else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){
		pm_ret = POINTSEL_MoveVec( wk->cp_wk,NULL,NULL,NULL,NULL,wk->cur_pos,POINTSEL_MV_DOWN );
		mv     = POINTSEL_MV_DOWN;
	}else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT ){
		pm_ret = POINTSEL_MoveVec( wk->cp_wk,NULL,NULL,NULL,NULL,wk->cur_pos,POINTSEL_MV_LEFT );
		mv     = POINTSEL_MV_LEFT;
	}else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ){
		pm_ret = POINTSEL_MoveVec( wk->cp_wk,NULL,NULL,NULL,NULL,wk->cur_pos,POINTSEL_MV_RIGHT );
		mv     = POINTSEL_MV_RIGHT;
	}else{
		pm_ret = BAPP_CMV_NONE;
	}

	if( pm_ret != POINTSEL_MOVE_NONE ){
		u8	old_save = 1;

		if( pm_ret & BAPP_CMV_RETBIT ){
			if( wk->old_pos != 0xff ){
				pm_ret = wk->old_pos;
			}else{
				pm_ret ^= BAPP_CMV_RETBIT;
			}
		}

		while(1){
			u8	tmp;
			if( wk->mv_tbl & (1<<pm_ret) ){
				break;
			}
			old_save = 0;
			tmp = POINTSEL_MoveVec( wk->cp_wk,NULL,NULL,NULL,NULL,pm_ret,mv )&(0xff^BAPP_CMV_RETBIT);
			if( tmp == pm_ret || tmp == wk->cur_pos ){
				pm_ret = wk->cur_pos;
				break;
			}
			pm_ret = tmp;
		}

		if( wk->cur_pos != pm_ret ){
			POINTSEL_GetPos( &wk->cp_wk[pm_ret], &lx, &ly );
			POINTSEL_GetSize( &wk->cp_wk[pm_ret], &rx, &ry );

			if( BAPP_OldCursorSetCheck(&wk->cp_wk[pm_ret],mv) == TRUE && old_save != 0 ){
				wk->old_pos = wk->cur_pos;
			}else{
				wk->old_pos = 0xff;
			}

			wk->cur_pos = pm_ret;
			BCURSOR_PosSetON( wk->cursor, lx, rx, ly, ry );
//		Snd_SePlay( BCURSOR_MOVE_SE );
		}
		return BAPP_CMV_NONE;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
		return wk->cur_pos;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
//	Snd_SePlay( SEQ_SE_DP_DECIDE );
		return BAPP_CMV_CANCEL;
	}
*/
	return BAPP_CMV_NONE;
}



#define	BATTLE_CURSOR_ANIME		( 11 )

BAPP_CURSOR_PUT_WORK * BAPPTOOL_CreateCursor( HEAPID heapID )
{
	BAPP_CURSOR_PUT_WORK * wk = GFL_HEAP_AllocMemory( heapID, sizeof(BAPP_CURSOR_PUT_WORK) );
	wk->heapID = heapID;
	wk->vanish = TRUE;
	return wk;
}

void BAPPTOOL_FreeCursor( BAPP_CURSOR_PUT_WORK * wk )
{
	GFL_HEAP_FreeMemory( wk );
}

void BAPPTOOL_SetCursorPutData( BAPP_CURSOR_PUT_WORK * wk, const BAPP_CURSOR_PUT * put )
{
	wk->put = put;
}

void BAPPTOOL_AddCursor( BAPP_CURSOR_PUT_WORK * wk, GFL_CLUNIT * clunit, u32 chrRes, u32 palRes, u32 celRes )
{
	GFL_CLWK_DATA	dat;
	u32	i;

	dat.pos_x   = 0;
	dat.pos_y   = 0;
	dat.softpri = 0;
	dat.bgpri   = 1;

	for( i=0; i<BAPP_CURSOR_MAX; i++ ){
		dat.anmseq = BATTLE_CURSOR_ANIME + i;
		wk->clwk[i] = GFL_CLACT_WK_Create( clunit, chrRes, palRes, celRes, &dat, CLSYS_DRAW_SUB, wk->heapID );
		GFL_CLACT_WK_SetPlttOffs( wk->clwk[i], 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );
		GFL_CLACT_WK_SetAutoAnmFlag( wk->clwk[i], TRUE );
	}
}

void BAPPTOOL_DelCursor( BAPP_CURSOR_PUT_WORK * wk )
{
	u32	i;

	for( i=0; i<BAPP_CURSOR_MAX; i++ ){
		GFL_CLACT_WK_Remove( wk->clwk[i] );
	}
}

void BAPPTOOL_VanishCursor( BAPP_CURSOR_PUT_WORK * wk, BOOL flg )
{
	if( wk->vanish != flg ){
		u32	i;

		for( i=0; i<BAPP_CURSOR_MAX; i++ ){
			GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], flg );
		}
		wk->vanish = flg;
	}
}

void BAPPTOOL_MoveCursor( BAPP_CURSOR_PUT_WORK * wk, u32 point )
{
	GFL_CLACTPOS	pos;
	u8	sx, sy;

	sx = wk->put[point].sx >> 1;
	sy = wk->put[point].sy >> 1;

	// ����
	pos.x = wk->put[point].px - sx;
	pos.y = wk->put[point].py - sy;
	GFL_CLACT_WK_SetPos( wk->clwk[0], &pos, CLSYS_DRAW_SUB );
	// ����
	pos.x = wk->put[point].px - sx;
	pos.y = wk->put[point].py + sy;
	GFL_CLACT_WK_SetPos( wk->clwk[1], &pos, CLSYS_DRAW_SUB );
	// �E��
	pos.x = wk->put[point].px + sx;
	pos.y = wk->put[point].py - sy;
	GFL_CLACT_WK_SetPos( wk->clwk[2], &pos, CLSYS_DRAW_SUB );
	// �E��
	pos.x = wk->put[point].px + sx;
	pos.y = wk->put[point].py + sy;
	GFL_CLACT_WK_SetPos( wk->clwk[3], &pos, CLSYS_DRAW_SUB );
}

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










void BAPPTOOL_PrintScreenTrans( PRINT_UTIL * util )
{
	GFL_BMPWIN_MakeScreen( util->win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(util->win) );
}

void BAPPTOOL_PrintUtilTrans( PRINT_UTIL * util, PRINT_QUE * que, u32 max )
{
	u32	i;

	for( i=0; i<max; i++ ){
		PRINT_UTIL_Trans( &util[i], que );
	}
}
