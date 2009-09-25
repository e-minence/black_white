//============================================================================================
/**
 * @file	point_sel.c
 * @brief	�ʒu�C���f�b�N�X�I������
 * @author	Hiroyuki Nakamura
 * @date	2004.11.12
 *
 *	���W���[�����FPOINTSEL
 */
//============================================================================================
#include <gflib.h>
#include "system/point_sel.h"


//--------------------------------------------------------------------------------------------
/**
 * �p�����[�^�擾
 *
 * @param	pw		���[�N
 * @param	prm		�擾�t���O
 *
 * @return	�w��p�����[�^
 */
//--------------------------------------------------------------------------------------------
u8 POINTSEL_GetParam( const POINTSEL_WORK * pw, u8 prm )
{
	switch( prm ){
	case POINTSEL_PRM_PX:
		return pw->px;
	case POINTSEL_PRM_PY:
		return pw->py;
	case POINTSEL_PRM_SX:
		return pw->sx;
	case POINTSEL_PRM_SY:
		return pw->sy;
	case POINTSEL_PRM_UP:
		return pw->up;
	case POINTSEL_PRM_DOWN:
		return pw->down;
	case POINTSEL_PRM_LEFT:
		return pw->left;
	case POINTSEL_PRM_RIGHT:
		return pw->right;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ���W�擾
 *
 * @param	pw		���[�N
 * @param	x		X���W�i�[�ꏊ
 * @param	y		Y���W�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void POINTSEL_GetPos( const POINTSEL_WORK * pw, u8 * x, u8 * y )
{
	*x = pw->px;
	*y = pw->py;
}

//--------------------------------------------------------------------------------------------
/**
 * �T�C�Y�擾
 *
 * @param	pw		���[�N
 * @param	x		X�T�C�Y�i�[�ꏊ
 * @param	y		Y�T�C�Y�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void POINTSEL_GetSize( const POINTSEL_WORK * pw, u8 * x, u8 * y )
{
	*x = pw->sx;
	*y = pw->sy;
}

//--------------------------------------------------------------------------------------------
/**
 * �ړ��������w�肵�A���W�A�T�C�Y�A�C���f�b�N�X���擾����
 *
 * @param	pw		���[�N
 * @param	px		X���W�i�[�ꏊ
 * @param	py		Y���W�i�[�ꏊ
 * @param	sx		X�T�C�Y�i�[�ꏊ
 * @param	sy		Y�T�C�Y�i�[�ꏊ
 * @param	now		���݂̃C���f�b�N�X
 * @param	mv		�ړ�����
 *
 * @return	�ړ���̃C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u8 POINTSEL_MoveVec(
		const POINTSEL_WORK * pw, u8 * px, u8 * py, u8 * sx, u8 * sy, u8 now, u8 mv )
{
	u8	next = now;

	switch( mv ){
	case POINTSEL_MV_UP:
		next = pw[now].up;
		break;
	case POINTSEL_MV_DOWN:
		next = pw[now].down;
		break;
	case POINTSEL_MV_LEFT:
		next = pw[now].left;
		break;
	case POINTSEL_MV_RIGHT:
		next = pw[now].right;
		break;
	}
	if( px != NULL ){ *px = pw[next].px; }
	if( py != NULL ){ *py = pw[next].py; }
	if( sx != NULL ){ *sx = pw[next].sx; }
	if( sy != NULL ){ *sy = pw[next].sy; }

	return next;
}

//--------------------------------------------------------------------------------------------
/**
 * �\���L�[�̏�񂩂�A���W�A�T�C�Y�A�C���f�b�N�X���擾����
 *
 * @param	pw		���[�N
 * @param	px		X���W�i�[�ꏊ
 * @param	py		Y���W�i�[�ꏊ
 * @param	sx		X�T�C�Y�i�[�ꏊ
 * @param	sy		Y�T�C�Y�i�[�ꏊ
 * @param	now		���݂̃C���f�b�N�X
 *
 * @return	�ړ���̃C���f�b�N�X�i�\���L�[��������Ă��Ȃ��ꍇ��POINTSEL_MOVE_NONE�j
 *
 * @li	GFL_UI_KEY_GetTrg()���g�p
 */
//--------------------------------------------------------------------------------------------
u8 POINTSEL_MoveTrg( const POINTSEL_WORK * pw, u8 * px, u8 * py, u8 * sx, u8 * sy, u8 now )
{
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
		return POINTSEL_MoveVec( pw, px, py, sx, sy, now, POINTSEL_MV_UP );
	}
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){
		return POINTSEL_MoveVec( pw, px, py, sx, sy, now, POINTSEL_MV_DOWN );
	}
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT ){
		return POINTSEL_MoveVec( pw, px, py, sx, sy, now, POINTSEL_MV_LEFT );
	}
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ){
		return POINTSEL_MoveVec( pw, px, py, sx, sy, now, POINTSEL_MV_RIGHT );
	}
	return POINTSEL_MOVE_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���W���w�肵�A�C���f�b�N�X���擾����
 *
 * @param	pw		���[�N
 * @param	px		X���W
 * @param	py		Y���W
 * @param	siz		�f�[�^�T�C�Y
 *
 * @return	�w����W�̃C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u8 POINTSEL_GetPosIndex( const POINTSEL_WORK * pw, u8 px, u8 py, u8 siz )
{
	u16	i;

	for( i=0; i<siz; i++ ){
		if( pw[i].px <= px && pw[i].px+pw[i].sx > px &&
			pw[i].py <= py && pw[i].py+pw[i].sy > py ){
			break;
		}
	}
	return (u8)i;
}
