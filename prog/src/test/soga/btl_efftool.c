
//============================================================================================
/**
 * @file	btl_efftool.c
 * @brief	�퓬�G�t�F�N�g�c�[��
 * @author	soga
 * @date	2008.11.27
 */
//============================================================================================

#include <gflib.h>

#include "btl_efftool.h"

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

void	BTL_EFFTOOL_CalcMoveVector( VecFx32 *start, VecFx32 *end, VecFx32 *out, fx32 flame );
void	BTL_EFFTOOL_CheckMove( fx32 *now_pos, fx32 *vec, fx32 *move_pos, BOOL *ret );
BOOL	BTL_EFFTOOL_CalcMove( EFFTOOL_MOVE_WORK *emw, VecFx32 *now_pos );

//============================================================================================
/**
 *	�ړ��ʂ��v�Z
 *
 * @param[in]	start	�ړ���
 * @param[in]	endif	�ړ���
 * @param[out]	out		�ړ��ʂ��i�[���郏�[�N�ւ̃|�C���^
 * @param[in]	flame	�ړ��t���[����
 */
//============================================================================================
void	BTL_EFFTOOL_CalcMoveVector( VecFx32 *start, VecFx32 *end, VecFx32 *out, fx32 flame )
{
	out->x = 0;
	out->y = 0;
	out->z = 0;
	if( end->x - start->x ){
		out->x = FX_Div( end->x - start->x, flame );
		if( out->x == 0 ){
			out->x = 1;
		}
	}
	if( end->y - start->y ){
		out->y = FX_Div( end->y - start->y, flame );
		if( out->y == 0 ){
			out->y = 1;
		}
	}
	if( end->z - start->z ){
		out->z = FX_Div( end->z - start->z, flame );
		if( out->z == 0 ){
			out->z = 1;
		}
	}
}

//============================================================================================
/**
 *	��������l����ړ��ł��邩�`�F�b�N���Ĉړ��ł��Ȃ��Ƃ��͉������Ȃ�
 *
 *	@param[in]	now_pos		���ݒl
 *	@param[in]	vec			�ړ���
 *	@param[in]	move_pos	�ړ���
 *	@param[out]	ret			�ړ��������ǂ�����Ԃ��iFALSE:�ړ����Ȃ��j
 */
//============================================================================================
void	BTL_EFFTOOL_CheckMove( fx32 *now_pos, fx32 *vec, fx32 *move_pos, BOOL *ret )
{
	*now_pos += *vec;

	if( *vec < 0 ){
		if( *now_pos <= *move_pos ){
			*now_pos = *move_pos;
		}
		else{
			*ret = FALSE;
		}
	}
	else{
		if( *now_pos >= *move_pos ){
			*now_pos = *move_pos;
		}
		else{
			*ret = FALSE;
		}
	}
}

//============================================================================================
/**
 *	�ړ��v�Z
 *
 *	@param[in]	emw		�ړ��v�Z�p�����[�^�\���̂ւ̃|�C���^
 *	@param[in]	now_pos	���ݒl
 *
 *	@retval	FALSE:�ړ����@TRUE:�ړ��I��
 */
//============================================================================================
BOOL	BTL_EFFTOOL_CalcMove( EFFTOOL_MOVE_WORK *emw, VecFx32 *now_pos )
{
	BOOL	ret = TRUE;

	switch( emw->move_type ){
	case EFFTOOL_MOVETYPE_DIRECT:			//���ڃ|�W�V�����Ɉړ�
		now_pos->x = emw->end_pos.x;
		now_pos->y = emw->end_pos.y;
		now_pos->z = emw->end_pos.z;
		break;
	case EFFTOOL_MOVETYPE_INTERPOLATION:	//�ړ���܂ł��Ԃ��Ȃ���ړ�
		BTL_EFFTOOL_CheckMove( &now_pos->x, &emw->vector.x, &emw->end_pos.x, &ret );
		BTL_EFFTOOL_CheckMove( &now_pos->y, &emw->vector.y, &emw->end_pos.y, &ret );
		BTL_EFFTOOL_CheckMove( &now_pos->z, &emw->vector.z, &emw->end_pos.z, &ret );
		break;
	case EFFTOOL_MOVETYPE_ROUNDTRIP:		//�w�肵����Ԃ������ړ�
		now_pos->x += emw->vector.x;
		now_pos->y += emw->vector.y;
		now_pos->z += emw->vector.z;
		if( --emw->vec_time == 0 ){
			emw->vector.x *= -1;
			emw->vector.y *= -1;
			emw->vector.z *= -1;
			emw->vec_time = emw->vec_time_tmp;
		}
		if( --emw->count ){
			ret = FALSE;
		}
		else{
			now_pos->x = emw->start_pos.x;
			now_pos->y = emw->start_pos.y;
			now_pos->z = emw->start_pos.z;
		}
		break;
	}
	return ret;
}

