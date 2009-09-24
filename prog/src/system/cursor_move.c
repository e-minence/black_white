//============================================================================================
/**
 * @file	cursor_move.c
 * @brief	�J�[�\���ړ�����
 * @author	Hiroyuki Nakamura
 * @date	08.08.06
 */
//============================================================================================
#include <gflib.h>

#include "sound/pm_sndsys.h"
#include "system/cursor_move.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �I���J�[�\���ړ����[�N
struct _CURSOR_MOVE_WORK {
	const GFL_UI_TP_HITTBL * rect;			// �^�b�`�e�[�u��
	const POINTER_WORK * pw;			// �J�[�\���ʒu���[�N
	BOOL cur_flg;						// �J�[�\��ON/OFF
	u8	cur_mode;						// �J�[�\���\�����[�h
	u8	cur_pos;						// �J�[�\���ʒu
	u8	old_pos;						// �O��̃J�[�\���ʒu
	u8	save_pos;						// �J�[�\���ʒu�L��
	u32	mv_tbl[2];						// �ړ��e�[�u��
	const CURSORMOVE_CALLBACK * func;	// �R�[���o�b�N�֐�
	void * work;						// �R�[���o�b�N�֐��ɓn�����[�N
};

// �J�[�\���\�����[�h
enum {
	CURSOR_MODE_NORMAL = 0,		// ��ɕ\��
	CURSOR_MODE_VANISH,			// �L�[����ɂ���ĕ\���ؑ�
};

// �L�[�S��
#define	PAD_KEY_ALL	( PAD_KEY_UP | PAD_KEY_DOWN | PAD_KEY_LEFT | PAD_KEY_RIGHT )

#define	DEF_OLDPOS		( 0xff )			// �O��̃J�[�\���ʒu�Ȃ�
#define	DEF_MVTBL		( 0xffffffff )		// �f�t�H���g�̈ړ��e�[�u��


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static u32 Main_TouchCheck( CURSOR_MOVE_WORK * wk, int hit );
static u32 Main_CursorOn( CURSOR_MOVE_WORK * wk );
static u32 Main_KeyMove( CURSOR_MOVE_WORK * wk, u8 mv, u8 ret );

static BOOL OldCursorSetCheck( const POINTER_WORK * pw, u8 mv );
static BOOL MoveTableBitCheck( u32 * tbl, u32 pos );



//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�쐬
 *
 * @param	rect		�^�b�`����e�[�u��
 * @param	pw			�J�[�\���ʒu���[�N
 * @param	func		�R�[���o�b�N�֐�
 * @param	work		�R�[���o�b�N�֐��ɓn����郏�[�N
 * @param	cur_flg		�J�[�\���\���t���O
 * @param	cur_pos		�J�[�\���ʒu
 * @param	heapID		�q�[�vID
 *
 * @return	�J�[�\���ړ����[�N
 *
 *	�f�t�H���g�̓J�[�\������ɕ\��
 */
//--------------------------------------------------------------------------------------------
CURSOR_MOVE_WORK * CURSORMOVE_Create(
					const GFL_UI_TP_HITTBL * rect,
					const POINTER_WORK * pw,
					const CURSORMOVE_CALLBACK * func,
					void * work,
					BOOL cur_flg,
					u8	cur_pos,
					u32 heapID )
{
	CURSOR_MOVE_WORK * wk = GFL_HEAP_AllocMemory( heapID, sizeof(CURSOR_MOVE_WORK) );

	wk->rect = rect;
	wk->pw   = pw;

	wk->func = func;
	wk->work = work;

	wk->cur_mode = CURSOR_MODE_NORMAL;
	wk->cur_flg  = cur_flg;
	wk->cur_pos  = cur_pos;
	wk->old_pos  = DEF_OLDPOS;
	wk->save_pos = DEF_OLDPOS;

	wk->mv_tbl[0] = DEF_MVTBL;
	wk->mv_tbl[1] = DEF_MVTBL;

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N���
 *
 * @param	wk		�J�[�\���ړ����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CURSORMOVE_Exit( CURSOR_MOVE_WORK * wk )
{
	GFI_HEAP_FreeMemory( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\�����[�h���\������ɐݒ�
 *
 * @param	wk		�J�[�\���ړ����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CURSORMOVE_VanishModeSet( CURSOR_MOVE_WORK * wk )
{
	wk->cur_mode = CURSOR_MODE_VANISH;
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ��i�g���K���́j
 *
 * @param	wk		�J�[�\���ړ����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 CURSORMOVE_Main( CURSOR_MOVE_WORK * wk )
{
	int	hit;
	u16	cnt, flg;
	u8	mv, ret, tmp, old;

/*
	// �^�b�`�`�F�b�N
	hit = GF_TP_RectHitTrg( wk->rect );

	if( hit != RECT_HIT_NONE ){
		return Main_TouchCheck( wk, hit );
	}
*/
	// �^�b�`�`�F�b�N
	cnt = 0;
	flg = 0;
	while( 1 ){
		if( wk->rect[cnt].rect.top == GFL_UI_TP_HIT_END ){
			break;
		}
		if( GFL_UI_TP_HitTrg( &wk->rect[cnt] ) == TRUE ){
			hit = Main_TouchCheck( wk, cnt );
			if( hit != CURSORMOVE_NONE ){
				return hit;
			}
			flg = 1;
		}
		cnt++;
	}
	if( flg != 0 ){ return CURSORMOVE_NONE; }

	// �J�[�\���\���`�F�b�N
	if( wk->cur_flg == FALSE && wk->cur_mode == CURSOR_MODE_VANISH ){
		if( GFL_UI_KEY_GetTrg() & ( PAD_KEY_ALL | PAD_BUTTON_CANCEL | PAD_BUTTON_DECIDE ) ){
			return Main_CursorOn( wk );
		}
		return CURSORMOVE_NONE;
	}

	// �ړ���擾
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
		ret = PointerWkMoveSel( wk->pw,NULL,NULL,NULL,NULL,wk->cur_pos,POINT_MV_UP );
		mv  = POINT_MV_UP;
	}else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){
		ret = PointerWkMoveSel( wk->pw,NULL,NULL,NULL,NULL,wk->cur_pos,POINT_MV_DOWN );
		mv  = POINT_MV_DOWN;
	}else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT ){
		ret = PointerWkMoveSel( wk->pw,NULL,NULL,NULL,NULL,wk->cur_pos,POINT_MV_LEFT );
		mv  = POINT_MV_LEFT;
	}else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ){
		ret = PointerWkMoveSel( wk->pw,NULL,NULL,NULL,NULL,wk->cur_pos,POINT_MV_RIGHT );
		mv  = POINT_MV_RIGHT;
	}else{
		ret = POINT_SEL_NOMOVE;
	}

	// �L�[�ړ�
	if( ret != POINT_SEL_NOMOVE ){
		return Main_KeyMove( wk, mv, ret );
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
//		Snd_SePlay( SEQ_SE_DP_DECIDE );		// �e�A�v����SE���ςɂȂ�\��������
		return wk->cur_pos;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
//		Snd_SePlay( SEQ_SE_DP_DECIDE );
		return CURSORMOVE_CANCEL;
	}

	return CURSORMOVE_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ��i�x�^���́j
 *
 * @param	wk		�J�[�\���ړ����[�N
 *
 * @return	���쌋��
 *
 *	�{�^���i�`�A�a�Ȃǁj�̓g���K����
 */
//--------------------------------------------------------------------------------------------
u32 CURSORMOVE_MainCont( CURSOR_MOVE_WORK * wk )
{
	int	hit;
	u16	cnt, flg;
	u8	mv, ret, tmp, old;

/*
	// �^�b�`�`�F�b�N
	hit = GF_TP_RectHitTrg( wk->rect );

	if( hit != RECT_HIT_NONE ){
		return Main_TouchCheck( wk, hit );
	}
*/
	// �^�b�`�`�F�b�N
	cnt = 0;
	flg = 0;
	while( 1 ){
		if( wk->rect[cnt].rect.top == GFL_UI_TP_HIT_END ){
			break;
		}
		if( GFL_UI_TP_HitTrg( &wk->rect[cnt] ) == TRUE ){
			hit = Main_TouchCheck( wk, cnt );
			if( hit != CURSORMOVE_NONE ){
				return hit;
			}
			flg = 1;
		}
		cnt++;
	}
	if( flg != 0 ){ return CURSORMOVE_NONE; }

	// �J�[�\���\���`�F�b�N
	if( wk->cur_flg == FALSE && wk->cur_mode == CURSOR_MODE_VANISH ){
		if( GFL_UI_KEY_GetTrg() & ( PAD_KEY_ALL | PAD_BUTTON_CANCEL | PAD_BUTTON_DECIDE ) ){
			return Main_CursorOn( wk );
		}
		return CURSORMOVE_NONE;
	}

	// �ړ���擾
	if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP  ){
		ret = PointerWkMoveSel( wk->pw,NULL,NULL,NULL,NULL,wk->cur_pos,POINT_MV_UP );
		mv  = POINT_MV_UP;
	}else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
		ret = PointerWkMoveSel( wk->pw,NULL,NULL,NULL,NULL,wk->cur_pos,POINT_MV_DOWN );
		mv  = POINT_MV_DOWN;
	}else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
		ret = PointerWkMoveSel( wk->pw,NULL,NULL,NULL,NULL,wk->cur_pos,POINT_MV_LEFT );
		mv  = POINT_MV_LEFT;
	}else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
		ret = PointerWkMoveSel( wk->pw,NULL,NULL,NULL,NULL,wk->cur_pos,POINT_MV_RIGHT );
		mv  = POINT_MV_RIGHT;
	}else{
		ret = POINT_SEL_NOMOVE;
	}

	// �L�[�ړ�
	if( ret != POINT_SEL_NOMOVE ){
		return Main_KeyMove( wk, mv, ret );
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
//		Snd_SePlay( SEQ_SE_DP_DECIDE );		// �e�A�v����SE���ςɂȂ�\��������
		return wk->cur_pos;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
//		Snd_SePlay( SEQ_SE_DP_DECIDE );
		return CURSORMOVE_CANCEL;
	}

	return CURSORMOVE_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * �^�b�`�`�F�b�N
 *
 * @param	wk		�J�[�\���ړ����[�N
 * @param	hit		�^�b�`�ʒu
 *
 * @retval	"CURSORMOVE_NONE = �����Ȉʒu"
 * @retval	"CURSORMOVE_NONE != �L���Ȉʒu"
 */
//--------------------------------------------------------------------------------------------
static u32 Main_TouchCheck( CURSOR_MOVE_WORK * wk, int hit )
{
	if( MoveTableBitCheck( wk->mv_tbl, hit ) == TRUE ){
		wk->cur_pos  = hit;
		wk->old_pos  = DEF_OLDPOS;
		wk->save_pos = DEF_OLDPOS;
		if( wk->cur_flg == TRUE && wk->cur_mode == CURSOR_MODE_VANISH ){
			wk->cur_flg = FALSE;
			wk->func->off( wk->work, wk->cur_pos, wk->old_pos );
		}
		wk->func->touch( wk->work, wk->cur_pos, wk->old_pos );
		return hit;
	}
	return CURSORMOVE_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���\��
 *
 * @param	wk		�J�[�\���ړ����[�N
 *
 * @return	CURSORMOVE_CURSOR_ON
 */
//--------------------------------------------------------------------------------------------
static u32 Main_CursorOn( CURSOR_MOVE_WORK * wk )
{
	wk->cur_flg  = TRUE;
	wk->old_pos  = DEF_OLDPOS;
	wk->save_pos = DEF_OLDPOS;
	wk->func->on( wk->work, wk->cur_pos, wk->old_pos );
	return CURSORMOVE_CURSOR_ON;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��i�L�[���́j
 *
 * @param	wk		�J�[�\���ړ����[�N
 * @param	mv		�ړ�����
 * @param	ret		�ړ���
 *
 * @retval	"CURSORMOVE_CURSOR_MOVE = �ړ�����"
 * @retval	"CURSORMOVE_NONE = �ړ����Ȃ�����"
 */
//--------------------------------------------------------------------------------------------
static u32 Main_KeyMove( CURSOR_MOVE_WORK * wk, u8 mv, u8 ret )
{
	u8	tmp, old;

	// �ړ���̋L���r�b�g�`�F�b�N
	if( ret & CURSORMOVE_RETBIT ){
		if( wk->save_pos != DEF_OLDPOS ){
			ret = wk->save_pos;			// �L���ꏊ��
		}else{
			ret ^= CURSORMOVE_RETBIT;	// �ړ����
		}
	}

	old = 1;
	while(1){
		// �ړ��悪�ړ��\
		if( wk->mv_tbl[ret/32] & (1<<(ret%32)) ){
			break;
		}
		old = 0;
		tmp = PointerWkMoveSel( wk->pw,NULL,NULL,NULL,NULL,ret,mv )&(0xff^CURSORMOVE_RETBIT);
		// �ړ��悪�s���~�܂� or ���̈ʒu�ɖ߂��Ă�����
		if( tmp == ret || tmp == wk->cur_pos ){
			ret = wk->cur_pos;
			break;
		}
		ret = tmp;
	}

	if( wk->cur_pos != ret ){
		// �ړ���̋L���r�b�g�������Ă���L��
		if( OldCursorSetCheck(&wk->pw[ret],mv) == TRUE && old != 0 ){
			wk->save_pos = wk->cur_pos;
		}else{
			wk->save_pos = DEF_OLDPOS;
		}

		wk->old_pos = wk->cur_pos;
		wk->cur_pos = ret;
		wk->func->move( wk->work, wk->cur_pos, wk->old_pos );
//		CURSORMOVE_CursorPosSet( wk, wk->cur_pos );
//		Snd_SePlay( SEQ_SE_DP_DECIDE );
		return CURSORMOVE_CURSOR_MOVE;
	}
	return CURSORMOVE_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ʒu�擾
 *
 * @param	wk		�J�[�\���ړ����[�N
 *
 * @return	�J�[�\���ʒu
 */
//--------------------------------------------------------------------------------------------
u8 CURSORMOVE_PosGet( CURSOR_MOVE_WORK * wk )
{
	return wk->cur_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���L���ʒu�擾
 *
 * @param	wk		�J�[�\���ړ����[�N
 *
 * @return	�J�[�\���L���ʒu
 */
//--------------------------------------------------------------------------------------------
u8 CURSORMOVE_SavePosGet( CURSOR_MOVE_WORK * wk )
{
	return wk->save_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ʒu�ݒ�
 *
 * @param	wk		�J�[�\���ړ����[�N
 * @param	pos		�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CURSORMOVE_PosSet( CURSOR_MOVE_WORK * wk, u8 pos )
{
	wk->cur_pos  = pos;
	wk->old_pos  = DEF_OLDPOS;
	wk->save_pos = DEF_OLDPOS;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ʒu�ݒ�i�g���Łj
 *
 * @param	wk			�J�[�\���ړ����[�N
 * @param	pos			�ʒu
 * @param	old_pos		�O��̈ʒu
 * @param	save_pos	�L���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CURSORMOVE_PosSetEx( CURSOR_MOVE_WORK * wk, u8 new_pos, u8 old_pos, u8 save_pos )
{
	wk->cur_pos  = new_pos;
	wk->old_pos  = old_pos;
	wk->save_pos = save_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���\����Ԏ擾
 *
 * @param	wk		�J�[�\���ړ����[�N
 *
 * @retval	"TRUE = �\��"
 * @retval	"FALSE = ��\��"
 */
//--------------------------------------------------------------------------------------------
BOOL CURSORMOVE_CursorOnOffGet( CURSOR_MOVE_WORK * wk )
{
	return wk->cur_flg;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���\����Ԑݒ�
 *
 * @param	wk		�J�[�\���ړ����[�N
 * @param	flg		TRUE = ON, FALSE = OFF
 *
 * @return	none
 *
 * @li	�\����ON�ɂ����ꍇ�A�O��̈ʒu�ƋL���ʒu�͏���������܂�
 * @li	�R�[���o�b�N��old_pos�ɂ�CURSORMOVE_ONOFF_DIRECT���n����܂�
 */
//--------------------------------------------------------------------------------------------
void CURSORMOVE_CursorOnOffSet( CURSOR_MOVE_WORK * wk, BOOL flg )
{
	wk->cur_flg = flg;
	if( wk->cur_flg == TRUE ){
		wk->old_pos  = DEF_OLDPOS;
		wk->save_pos = DEF_OLDPOS;
		wk->func->on( wk->work, wk->cur_pos, CURSORMOVE_ONOFF_DIRECT );
	}else{
		wk->func->off( wk->work, wk->cur_pos, CURSORMOVE_ONOFF_DIRECT );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��e�[�u��������
 *
 * @param	wk		�J�[�\���ړ����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CURSORMOVE_MoveTableInit( CURSOR_MOVE_WORK * wk )
{
	wk->mv_tbl[0] = DEF_MVTBL;
	wk->mv_tbl[1] = DEF_MVTBL;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��e�[�u���ݒ�
 *
 * @param	wk		�J�[�\���ړ����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CURSORMOVE_MoveTableSet( CURSOR_MOVE_WORK * wk, u32 * tbl )
{
	wk->mv_tbl[0] = tbl[0];
	wk->mv_tbl[1] = tbl[1];
}

//--------------------------------------------------------------------------------------------
/**
 * ����̈ʒu���J�[�\���ړ��s�ɂ���
 *
 * @param	wk		�J�[�\���ړ����[�N
 * @param	pos		�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CURSORMOVE_MoveTableBitOff( CURSOR_MOVE_WORK * wk, u32 pos )
{
	u32	i = pos / 32;
	pos = pos % 32;

	if( ( wk->mv_tbl[i] & (1<<pos) ) != 0 ){
		wk->mv_tbl[i] = wk->mv_tbl[i] & ( 0xffffffff ^ (1<<pos) );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ����̈ʒu���J�[�\���ړ��ɂ���
 *
 * @param	wk		�J�[�\���ړ����[�N
 * @param	pos		�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CURSORMOVE_MoveTableBitOn( CURSOR_MOVE_WORK * wk, u32 pos )
{
	u32	i = pos / 32;
	pos = pos % 32;

	if( ( wk->mv_tbl[i] & (1<<pos) ) == 0 ){
		wk->mv_tbl[i] = wk->mv_tbl[i] ^ (1<<pos);
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ʒu�f�[�^�擾
 *
 * @param	wk		�J�[�\���ړ����[�N
 * @param	pos		�J�[�\���ʒu
 *
 * @return	�J�[�\���ʒu�f�[�^
 */
//--------------------------------------------------------------------------------------------
const POINTER_WORK * CURSORMOVE_PointerWorkGet( CURSOR_MOVE_WORK * wk, u32 pos )
{
	return &wk->pw[pos];
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��e�[�u��������
 *
 * @param	wk			�J�[�\���ړ����[�N
 * @param	rect		�^�b�`����e�[�u��
 * @param	pw			�J�[�\���ʒu���[�N
 * @param	cur_pos		�J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CURSORMOVE_SceneChange(
		CURSOR_MOVE_WORK * wk, const GFL_UI_TP_HITTBL * rect, const POINTER_WORK * pw, u8 cur_pos )
{
	wk->rect = rect;
	wk->pw   = pw;

	wk->cur_pos  = cur_pos;
	wk->old_pos  = DEF_OLDPOS;
	wk->save_pos = DEF_OLDPOS;

	wk->mv_tbl[0] = DEF_MVTBL;
	wk->mv_tbl[1] = DEF_MVTBL;
}

//--------------------------------------------------------------------------------------------
/**
 * �O�̃J�[�\���ʒu�Ɉړ����邩
 *
 * @param	wk		�J�[�\���ʒu���[�N
 * @param	mv		�ړ�����
 *
 * @retval	"TRUE = �ړ�����"
 * @retval	"FALSE = �ړ����Ȃ�"
 */
//--------------------------------------------------------------------------------------------
static BOOL OldCursorSetCheck( const POINTER_WORK * pw, u8 mv )
{
	switch( mv ){
	case POINT_MV_UP:
		if( pw->down & CURSORMOVE_RETBIT ){
			return TRUE;
		}
		break;
	case POINT_MV_DOWN:
		if( pw->up & CURSORMOVE_RETBIT ){
			return TRUE;
		}
		break;
	case POINT_MV_LEFT:
		if( pw->right & CURSORMOVE_RETBIT ){
			return TRUE;
		}
		break;
	case POINT_MV_RIGHT:
		if( pw->left & CURSORMOVE_RETBIT ){
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �f�[�^�L��/�����`�F�b�N
 *
 * @param	tbl		�f�[�^�e�[�u��
 * @param	pos		�ʒu
 *
 * @retval	"TRUE = �L��"
 * @retval	"FALSE = ����"
 */
//--------------------------------------------------------------------------------------------
static BOOL MoveTableBitCheck( u32 * tbl, u32 pos )
{
	if( tbl[pos/32] & (1<<(pos%32)) ){
		return TRUE;
	}
	return FALSE;
}
