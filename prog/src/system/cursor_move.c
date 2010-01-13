//============================================================================================
/**
 * @file	cursor_move.c
 * @brief	�J�[�\���ړ�����
 * @author	Hiroyuki Nakamura
 * @date	08.08.06
 *
 *	���W���[�����FCURSORMOVE
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
	const CURSORMOVE_DATA * dat;				// �J�[�\���ړ��f�[�^
	BOOL cur_flg;												// �J�[�\��ON/OFF
	u8	cur_mode;												// �J�[�\���\�����[�h
	u8	cur_pos;												// �J�[�\���ʒu
	u8	old_pos;												// �O��̃J�[�\���ʒu
	u8	save_pos;												// �J�[�\���ʒu�L��
	u32	mv_tbl[2];											// �ړ��e�[�u��
	const CURSORMOVE_CALLBACK * func;		// �R�[���o�b�N�֐�
	void * work;												// �R�[���o�b�N�֐��ɓn�����[�N
};

// �f�[�^�ړ���`
enum {
	CURSOR_MOVE_UP = 0,			// ��
	CURSOR_MOVE_DOWN,				// ��
	CURSOR_MOVE_LEFT,				// �E
	CURSOR_MOVE_RIGHT,			// ��
	CURSOR_MOVE_NONE = 0xff	// �J�[�\���ړ��Ȃ�
};

// �J�[�\���\�����[�h
enum {
	CURSOR_MODE_NORMAL = 0,		// ��ɕ\��
	CURSOR_MODE_VANISH,			// �L�[����ɂ���ĕ\���ؑ�
};

// �L�[�S��
#define	PAD_KEY_ALL	( PAD_KEY_UP | PAD_KEY_DOWN | PAD_KEY_LEFT | PAD_KEY_RIGHT )

#define	DEF_OLDPOS		( 0xff )					// �O��̃J�[�\���ʒu�Ȃ�
#define	DEF_MVTBL			( 0xffffffff )		// �f�t�H���g�̈ړ��e�[�u��


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static BOOL Main_TouchCheck( CURSORMOVE_WORK * wk, int * hit );
static u8 MoveVec( const CURSORMOVE_DATA * dat, u8 now, u8 mv );
static u32 Main_CursorOn( CURSORMOVE_WORK * wk );
static u32 Main_KeyMove( CURSORMOVE_WORK * wk, u8 mv, u8 ret );

static BOOL OldCursorSetCheck( const CURSORMOVE_DATA * dat, u8 mv );
static BOOL MoveTableBitCheck( u32 * tbl, u32 pos );


//============================================================================================
//	�O���[�o��
//============================================================================================

// �^�b�`�e�[�u���I���f�[�^
static const GFL_UI_TP_HITTBL HitRectEnd = { GFL_UI_TP_HIT_END, 0, 0, 0 };




//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�쐬
 *
 * @param		dat				�J�[�\���ړ��f�[�^
 * @param		pw				�J�[�\���ʒu���[�N
 * @param		func			�R�[���o�b�N�֐�
 * @param		work			�R�[���o�b�N�֐��ɓn����郏�[�N
 * @param		cur_flg		�J�[�\���\���t���O
 * @param		cur_pos		�J�[�\���ʒu
 * @param		heapID		�q�[�vID
 *
 * @return	�J�[�\���ړ����[�N
 *
 *	�f�t�H���g�̓J�[�\������ɕ\��
 */
//--------------------------------------------------------------------------------------------
CURSORMOVE_WORK * CURSORMOVE_Create(
					const CURSORMOVE_DATA * dat,
					const CURSORMOVE_CALLBACK * func,
					void * work,
					BOOL cur_flg,
					u8	cur_pos,
					HEAPID heapID )
{
 
	CURSORMOVE_WORK * wk = GFL_HEAP_AllocMemory( heapID, sizeof(CURSORMOVE_WORK) );

	wk->dat = dat;

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
void CURSORMOVE_Exit( CURSORMOVE_WORK * wk )
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
void CURSORMOVE_VanishModeSet( CURSORMOVE_WORK * wk )
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
u32 CURSORMOVE_Main( CURSORMOVE_WORK * wk )
{
	int	hit;
	u8	mv, ret, tmp, old;

	// �^�b�`�`�F�b�N
	if( Main_TouchCheck( wk, &hit ) == TRUE ){
		return hit;
	}

	// �J�[�\���\���`�F�b�N
	if( wk->cur_flg == FALSE && wk->cur_mode == CURSOR_MODE_VANISH ){
		if( GFL_UI_KEY_GetTrg() & ( PAD_KEY_ALL | PAD_BUTTON_CANCEL | PAD_BUTTON_DECIDE ) ){
			return Main_CursorOn( wk );
		}
		return CURSORMOVE_NONE;
	}

	// �ړ���擾
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
		ret = MoveVec( wk->dat, wk->cur_pos, CURSOR_MOVE_UP );
		mv  = CURSOR_MOVE_UP;
	}else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){
		ret = MoveVec( wk->dat, wk->cur_pos, CURSOR_MOVE_DOWN );
		mv  = CURSOR_MOVE_DOWN;
	}else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT ){
		ret = MoveVec( wk->dat, wk->cur_pos, CURSOR_MOVE_LEFT );
		mv  = CURSOR_MOVE_LEFT;
	}else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ){
		ret = MoveVec( wk->dat, wk->cur_pos, CURSOR_MOVE_RIGHT );
		mv  = CURSOR_MOVE_RIGHT;
	}else{
		ret = CURSOR_MOVE_NONE;
	}

	// �L�[�ړ�
	if( ret != CURSOR_MOVE_NONE ){
		return Main_KeyMove( wk, mv, ret );
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
//		Snd_SePlay( SEQ_SE_DP_DECIDE );		// �e�A�v����SE���ςɂȂ�\��������
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return wk->cur_pos;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
//		Snd_SePlay( SEQ_SE_DP_DECIDE );
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
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
u32 CURSORMOVE_MainCont( CURSORMOVE_WORK * wk )
{
	int	hit;
	u8	mv, ret, tmp, old;

	// �^�b�`�`�F�b�N
	if( Main_TouchCheck( wk, &hit ) == TRUE ){
		return hit;
	}

	// �J�[�\���\���`�F�b�N
	if( wk->cur_flg == FALSE && wk->cur_mode == CURSOR_MODE_VANISH ){
		if( GFL_UI_KEY_GetTrg() & ( PAD_KEY_ALL | PAD_BUTTON_CANCEL | PAD_BUTTON_DECIDE ) ){
			return Main_CursorOn( wk );
		}
		return CURSORMOVE_NONE;
	}

	// �ړ���擾
	if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP  ){
		ret = MoveVec( wk->dat, wk->cur_pos, CURSOR_MOVE_UP );
		mv  = CURSOR_MOVE_UP;
	}else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
		ret = MoveVec( wk->dat, wk->cur_pos, CURSOR_MOVE_DOWN );
		mv  = CURSOR_MOVE_DOWN;
	}else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
		ret = MoveVec( wk->dat, wk->cur_pos, CURSOR_MOVE_LEFT );
		mv  = CURSOR_MOVE_LEFT;
	}else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
		ret = MoveVec( wk->dat, wk->cur_pos, CURSOR_MOVE_RIGHT );
		mv  = CURSOR_MOVE_RIGHT;
	}else{
		ret = CURSOR_MOVE_NONE;
	}

	// �L�[�ړ�
	if( ret != CURSOR_MOVE_NONE ){
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
 * @param		wk		�J�[�\���ړ����[�N
 * @param		hit		�^�b�`�ʒu�i�[�ꏊ
 *
 * @retval	"TRUE = �^�b�`���ꂽ"
 * @retval	"FALSE = ����ȊO"
 *
 * @li	"*hit = CURSORMOVE_NONE : �����Ȉʒu"
 */
//--------------------------------------------------------------------------------------------
static BOOL Main_TouchCheck( CURSORMOVE_WORK * wk, int * hit )
{
	GFL_UI_TP_HITTBL	rect[2];
	BOOL	ret;
	int	p;
	u16	cnt, flg;

	rect[1] = HitRectEnd;
	ret = FALSE;
	cnt = 0;
	flg = 0;

	*hit = CURSORMOVE_NONE;

	while( 1 ){
		if( wk->dat[cnt].rect.rect.top == GFL_UI_TP_HIT_END ){
			break;
		}
		rect[0] = wk->dat[cnt].rect;
		p = GFL_UI_TP_HitTrg( rect );
		if( p != GFL_UI_TP_HIT_NONE ){
			ret = TRUE;
			if( MoveTableBitCheck( wk->mv_tbl, cnt ) == TRUE ){
				wk->cur_pos  = cnt;
				wk->old_pos  = DEF_OLDPOS;
				wk->save_pos = DEF_OLDPOS;
				if( wk->cur_flg == TRUE && wk->cur_mode == CURSOR_MODE_VANISH ){
					wk->cur_flg = FALSE;
					wk->func->off( wk->work, wk->cur_pos, wk->old_pos );
				}
				wk->func->touch( wk->work, wk->cur_pos, wk->old_pos );
				*hit = cnt;
				GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
				break;
			}
		}
		cnt++;
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��������w�肵�A���W�A�T�C�Y�A�C���f�b�N�X���擾����
 *
 * @param		dat		�J�[�\���ړ��f�[�^
 * @param		now		���݂̃C���f�b�N�X
 * @param		mv		�ړ�����
 *
 * @return	�ړ���̃C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
static u8 MoveVec( const CURSORMOVE_DATA * dat, u8 now, u8 mv )
{
	u8	next = now;

	switch( mv ){
	case CURSOR_MOVE_UP:
		next = dat[now].up;
		break;
	case CURSOR_MOVE_DOWN:
		next = dat[now].down;
		break;
	case CURSOR_MOVE_LEFT:
		next = dat[now].left;
		break;
	case CURSOR_MOVE_RIGHT:
		next = dat[now].right;
		break;
	}

	return next;
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
static u32 Main_CursorOn( CURSORMOVE_WORK * wk )
{
	GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );

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
static u32 Main_KeyMove( CURSORMOVE_WORK * wk, u8 mv, u8 ret )
{
	u8	tmp, old;

	GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );

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
		tmp = MoveVec( wk->dat, ret, mv ) & (0xff^CURSORMOVE_RETBIT);
		// �ړ��悪�s���~�܂� or ���̈ʒu�ɖ߂��Ă�����
		if( tmp == ret || tmp == wk->cur_pos ){
			ret = wk->cur_pos;
			break;
		}
		ret = tmp;
	}

	if( wk->cur_pos != ret ){
		// �ړ���̋L���r�b�g�������Ă���L��
		if( OldCursorSetCheck(&wk->dat[ret],mv) == TRUE && old != 0 ){
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

	if( mv == CURSOR_MOVE_UP ){
		return CURSORMOVE_NO_MOVE_UP;			// �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
	}
	if( mv == CURSOR_MOVE_DOWN ){
		return CURSORMOVE_NO_MOVE_DOWN;		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	}
	if( mv == CURSOR_MOVE_LEFT ){
		return CURSORMOVE_NO_MOVE_LEFT;		// �\���L�[���������ꂽ���A�ړ��Ȃ�
	}
	if( mv == CURSOR_MOVE_RIGHT ){
		return CURSORMOVE_NO_MOVE_RIGHT;	// �\���L�[�E�������ꂽ���A�ړ��Ȃ�
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
u8 CURSORMOVE_PosGet( CURSORMOVE_WORK * wk )
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
u8 CURSORMOVE_SavePosGet( CURSORMOVE_WORK * wk )
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
void CURSORMOVE_PosSet( CURSORMOVE_WORK * wk, u8 pos )
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
void CURSORMOVE_PosSetEx( CURSORMOVE_WORK * wk, u8 new_pos, u8 old_pos, u8 save_pos )
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
BOOL CURSORMOVE_CursorOnOffGet( CURSORMOVE_WORK * wk )
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
void CURSORMOVE_CursorOnOffSet( CURSORMOVE_WORK * wk, BOOL flg )
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
void CURSORMOVE_MoveTableInit( CURSORMOVE_WORK * wk )
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
void CURSORMOVE_MoveTableSet( CURSORMOVE_WORK * wk, u32 * tbl )
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
void CURSORMOVE_MoveTableBitOff( CURSORMOVE_WORK * wk, u32 pos )
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
void CURSORMOVE_MoveTableBitOn( CURSORMOVE_WORK * wk, u32 pos )
{
	u32	i = pos / 32;
	pos = pos % 32;

	if( ( wk->mv_tbl[i] & (1<<pos) ) == 0 ){
		wk->mv_tbl[i] = wk->mv_tbl[i] ^ (1<<pos);
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ����̈ʒu�̃J�[�\���ړ���Ԃ𒲂ׂ�
 *
 * @param	wk		�J�[�\���ړ����[�N
 * @param	pos		�ʒu
 *
 * @retval	"TRUE = �ړ���"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL CURSORMOVE_MoveTableBitCheck( CURSORMOVE_WORK * wk, u32 pos )
{
	u32	i = pos / 32;
	pos = pos % 32;

	if( ( wk->mv_tbl[i] & (1<<pos) ) != 0 ){
		return TRUE;
	}
	return FALSE;
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
const CURSORMOVE_DATA * CURSORMOVE_GetMoveData( CURSORMOVE_WORK * wk, u32 pos )
{
	return &wk->dat[pos];
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
/*
void CURSORMOVE_SceneChange(
		CURSORMOVE_WORK * wk, const GFL_UI_TP_HITTBL * rect, const CURSORMOVE_DATA * dat, u8 cur_pos )
{
	wk->rect = rect;
	wk->pw   = pw;

	wk->cur_pos  = cur_pos;
	wk->old_pos  = DEF_OLDPOS;
	wk->save_pos = DEF_OLDPOS;

	wk->mv_tbl[0] = DEF_MVTBL;
	wk->mv_tbl[1] = DEF_MVTBL;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �O�̃J�[�\���ʒu�Ɉړ����邩
 *
 * @param	wk		�J�[�\���ړ��f�[�^
 * @param	mv		�ړ�����
 *
 * @retval	"TRUE = �ړ�����"
 * @retval	"FALSE = �ړ����Ȃ�"
 */
//--------------------------------------------------------------------------------------------
static BOOL OldCursorSetCheck( const CURSORMOVE_DATA * dat, u8 mv )
{
	switch( mv ){
	case CURSOR_MOVE_UP:
		if( dat->down & CURSORMOVE_RETBIT ){
			return TRUE;
		}
		break;
	case CURSOR_MOVE_DOWN:
		if( dat->up & CURSORMOVE_RETBIT ){
			return TRUE;
		}
		break;
	case CURSOR_MOVE_LEFT:
		if( dat->right & CURSORMOVE_RETBIT ){
			return TRUE;
		}
		break;
	case CURSOR_MOVE_RIGHT:
		if( dat->left & CURSORMOVE_RETBIT ){
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
