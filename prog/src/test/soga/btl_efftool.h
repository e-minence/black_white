
//============================================================================================
/**
 * @file	btl_efftool.h
 * @brief	�퓬�G�t�F�N�g�c�[��
 * @author	soga
 * @date	2008.11.14
 */
//============================================================================================

#ifndef	__BTL_EFFTOOL_H_
#define	__BTL_EFFTOOL_H_

typedef struct
{
	int				position;
	int				move_type;
	VecFx32			start_pos;
	VecFx32			end_pos;
	VecFx32			vector;
	int				vec_time;
	int				vec_time_tmp;
	int				count;
}EFFTOOL_MOVE_WORK;

//�ړ��^�X�N�̎��
enum{
	EFFTOOL_MOVETYPE_DIRECT = 0,		//���ڃ|�W�V�����Ɉړ�
	EFFTOOL_MOVETYPE_INTERPOLATION,		//�ړ���܂ł��Ԃ��Ȃ���ړ�
	EFFTOOL_MOVETYPE_ROUNDTRIP,			//�w�肵����Ԃ������ړ�
};

extern	void	BTL_EFFTOOL_CalcMoveVector( VecFx32 *start, VecFx32 *end, VecFx32 *out, fx32 flame );
extern	void	BTL_EFFTOOL_CheckMove( fx32 *now_pos, fx32 *vec, fx32 *move_pos, BOOL *ret );
extern	BOOL	BTL_EFFTOOL_CalcMove( EFFTOOL_MOVE_WORK *emw, VecFx32 *now_pos );

#endif	//__BTL_EFFTOOL_H_
