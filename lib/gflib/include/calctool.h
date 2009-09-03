//=============================================================================================
/**
 * @file	calctool.h
 * @brief	�v�Z�֌W
 * @author	mori
 * @date	2004.10.28
 *
 */
//=============================================================================================
#ifndef	__CALCTOOL_H__
#define	__CALCTOOL_H__
#undef GLOBAL
#ifdef __CALCTOOL_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

#include "assert.h"


#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================================
//	�萔��`
//=============================================================================================

// FX_SinIdx CosIdx�̈����̃��W�A���l�����߂�
#define GFL_CALC_GET_ROTA_NUM(x)	( (x*0xffff) / 360 )	

//=============================================================================================
//	�v���g�^�C�v�錾
//=============================================================================================
#define ERRCHECK_ON_DEG_TO_XXX	//���̒�`�𖳌��ɂ����DEG�֐��̃G���[�`�F�b�N�𖳌��ɂł���

//GLOBAL const fx32 FixSinCosTbl[360+90];
//GLOBAL const u16 RotKey360Tbl[360];
//#define _Sin360( a ) (FixSinCosTbl[(a)])
//#define _Cos360( a ) (FixSinCosTbl[(a)+90])
//#define _RotKey( a ) (RotKey360Tbl[a])

/**
 *	@brief	�x���@�Ŏw�肵���p�x����A���ꂼ��̑Ή�����l���擾����
 *	@param	deg	u16:0-359�܂ł̕ϐ�
 */
 ///SinIdx(),CosIdx(),Index�L�[�̒l��Ԃ�(��荞�݂Ȃ�)
GLOBAL fx32 GFL_CALC_Sin360(u16 deg);
GLOBAL fx32 GFL_CALC_Cos360(u16 deg);
GLOBAL u16	GFL_CALC_RotKey(u16 deg);
///SinIdx(),CosIdx(),Index�L�[�̒l��Ԃ�(��荞�݂���)
GLOBAL fx32 GFL_CALC_Sin360R(u16 deg);
GLOBAL fx32 GFL_CALC_Cos360R(u16 deg);
GLOBAL u16	GFL_CALC_RotKeyR(u16 deg);
///SinIdx(),CosIdx()�̒l��Ԃ�(fx32�^�������Ɏ��A��荞�݂���)
GLOBAL fx32 GFL_CALC_Sin360FX(fx32 deg);
GLOBAL fx32 GFL_CALC_Cos360FX(fx32 deg);

//-----------------------------------------------------------------------------
/**
 *      GameFreak�@�^�b�`�~���o�c�[��
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �^�b�`���W�̕ω�����A�p�x�̋ߎ��l�����߂�
 *
 *	@param	x0          �^�b�`A���W
 *	@param	y0
 *	@param	x1          �^�b�`B���W
 *	@param	y1
 *	@param	center_x    ���S���W
 *	@param	center_y 
 *	@param  r           ��ƂȂ�~�̔��a
 *
 *	@return �}��]�p�x
 */
//-----------------------------------------------------------------------------
extern s32 GFL_CALC_GetCircleTouchRotate( s32 x0, s32 y0, s32 x1, s32 y1, s32 center_x, s32 center_y, s32 r );
//----------------------------------------------------------------------------
/**
 *	@brief  �^�b�`���W�̕ω�����A��]���l���ɓ��ꂽ�A�ړ����������߂�
 *
 *	@param	x0          �^�b�`A���W
 *	@param	y0
 *	@param	x1          �^�b�`B���W
 *	@param	y1
 *	@param	center_x    ���S���W
 *	@param	center_y 
 *
 *	@return �ړ�����(���W�A�� 0�`0x10000)
 */
//-----------------------------------------------------------------------------
extern s32 GFL_CALC_GetCircleTouchDistance( s32 x0, s32 y0, s32 x1, s32 y1, s32 center_x, s32 center_y );


//-----------------------------------------------------------------------------
/**
 *
 */
//-----------------------------------------------------------------------------
///==========================================
//�x�N�g�����Z�n�̃}�N��
///==========================================
//�x�N�g���R�s�[
#define VEC_COPY(a,b)	{ \
 (a)->x = (b)->x;	\
 (a)->y = (b)->y;	\
 (a)->z = (b)->z;	\
}

//�x�N�g����Z
#define VEC_MUL(dest,a,b)	{	\
 (dest)->x = FX_Mul((a)->x,(b)->x);	\
 (dest)->y = FX_Mul((a)->y,(b)->y);	\
 (dest)->z = FX_Mul((a)->z,(b)->z);	\
}
//�x�N�g�����Z
#define VEC_DIV(dest,a,b)	{	\
 (dest)->x = FX_Div((a)->x,(b)->x);	\
 (dest)->y = FX_Div((a)->y,(b)->y);	\
 (dest)->z = FX_Div((a)->z,(b)->z);	\
}
//�x�N�g���̎����{
#define VEC_APPLYM(dest,a,fval) {	\
 (dest)->x = FX_MUL((a)->x,fval);	\
 (dest)->y = FX_MUL((a)->y,fval);	\
 (dest)->z = FX_MUL((a)->z,fval);	\
}
//�x�N�g���������Ŋ���
#define VEC_APPLYD(dest,a,fval) {	\
 (dest)->x = FX_Div((a)->x,fval);	\
 (dest)->y = FX_Div((a)->y,fval);	\
 (dest)->z = FX_Div((a)->z,fval);	\
}





#undef GLOBAL
#ifdef __cplusplus
}/* extern "C" */
#endif

#endif	/*__CALCTOOL_H__*/
