
//============================================================================================
/**
 * @file	btlv_efftool.h
 * @brief	�퓬�G�t�F�N�g�c�[��
 * @author	soga
 * @date	2008.11.14
 */
//============================================================================================

#ifndef	__BTLV_EFFTOOL_H_
#define	__BTLV_EFFTOOL_H_

typedef struct
{
	int				move_type;
	VecFx32			start_value;
	VecFx32			end_value;
	VecFx32			vector;
	int				vec_time;
	int				vec_time_tmp;
	int				wait;
	int				wait_tmp;
	int				count;
}EFFTOOL_MOVE_WORK;

//�^�X�N�̎��
enum{
	EFFTOOL_CALCTYPE_DIRECT = 0,		//���ڒl����
	EFFTOOL_CALCTYPE_INTERPOLATION,		//�w��l�܂ł��Ԃ��Ȃ���v�Z
	EFFTOOL_CALCTYPE_ROUNDTRIP,			//�w�肵����Ԃ������v�Z�i�X�^�[�g�n�_�ƃS�[���n�_�̉����j
	EFFTOOL_CALCTYPE_ROUNDTRIP_LONG,	//�w�肵����Ԃ������v�Z�i�X�^�[�g�n�_����Ɂ{�|�����̉����j
};

extern	void	BTLV_EFFTOOL_CalcMoveVector( VecFx32 *start, VecFx32 *end, VecFx32 *out, fx32 flame );
extern	void	BTLV_EFFTOOL_CheckMove( fx32 *now_pos, fx32 *vec, fx32 *move_pos, BOOL *ret );
extern	BOOL	BTLV_EFFTOOL_CalcParam( EFFTOOL_MOVE_WORK *emw, VecFx32 *now_param );
extern	u8		BTLV_EFFTOOL_Pos2Bit( BtlvMcssPos no );

#endif	//__BTLV_EFFTOOL_H_
