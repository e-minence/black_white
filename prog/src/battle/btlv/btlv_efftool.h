
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

//�^�X�N�̎��
#define	EFFTOOL_CALCTYPE_DIRECT			( 0 )	//���ڒl����
#define	EFFTOOL_CALCTYPE_INTERPOLATION	( 1 )	//�w��l�܂ł��Ԃ��Ȃ���v�Z
#define	EFFTOOL_CALCTYPE_ROUNDTRIP		( 2 )	//�w�肵����Ԃ������v�Z�i�X�^�[�g�n�_�ƃS�[���n�_�̉����j
#define	EFFTOOL_CALCTYPE_ROUNDTRIP_LONG	( 3 )	//�w�肵����Ԃ������v�Z�i�X�^�[�g�n�_����Ɂ{�|�����̉����j

#ifndef __ASM_NO_DEF_
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

extern	void	BTLV_EFFTOOL_CalcMoveVector( VecFx32 *start, VecFx32 *end, VecFx32 *out, fx32 flame );
extern	void	BTLV_EFFTOOL_CheckMove( fx32 *now_pos, fx32 *vec, fx32 *move_pos, BOOL *ret );
extern	BOOL	BTLV_EFFTOOL_CalcParam( EFFTOOL_MOVE_WORK *emw, VecFx32 *now_param );
extern	u8		BTLV_EFFTOOL_Pos2Bit( BtlvMcssPos no );

#endif __ASM_NO_DEF_

#endif	//__BTLV_EFFTOOL_H_
