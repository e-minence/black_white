//==============================================================================
/**
 * @file	quaternion.h
 * @brief	�N�H�[�^�j�I��
 * @author	goto
 * @date	2007.10.09(��)
 *
 * �����ɐF�X�ȉ�����������Ă��悢
 *
 */
//==============================================================================

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include	<nitro/types.h>
#include	<nitro/fx/fx.h>
#include	<nitro/fx/fx_cp.h>
#include	<nitro/fx/fx_mtx44.h>
#include	<nitro/fx/fx_trig.h>

#if 1
//FX32�v�Z�o�[�W����
//
// -----------------------------------------
//
//	�N�H�[�^�j�I��
//
// -----------------------------------------
typedef union {
	
	struct {		
		fx32	wr;		///< ����
		fx32	xi;		///< ����
		fx32	yi;		///< ����
		fx32	zi;		///< ����
	};
	fx32 q[ 4 ];
	
} GFL_QUATERNION;


// -----------------------------------------
//
//	�N�H�[�^�j�I���}�g���b�N�X ( 4x4 )
//
// -----------------------------------------
typedef union {
	
	struct {
		fx32 _00, _01, _02, _03;
        fx32 _10, _11, _12, _13;
        fx32 _20, _21, _22, _23;
        fx32 _30, _31, _32, _33;
	};
	fx32 m[ 4 ][ 4 ];
	fx32 a[ 16 ];
	
} GFL_QUATERNION_MTX44;

// �N�H�[�^�j�I���̏�����
extern void GFL_QUAT_Identity( GFL_QUATERNION* q );

// �N�H�[�^�j�I���̃R�s�[
extern void GFL_QUAT_Copy( GFL_QUATERNION* q1, GFL_QUATERNION* q2 );

// �N�H�[�^�j�I���������Ƃ̉�]�l���琶��
extern	void GFL_QUAT_MakeQuaternionXYZ( GFL_QUATERNION* qt, u16 rot_x, u16 rot_y,u16 rot_z );

// �N�H�[�^�j�I���̐�
extern void GFL_QUAT_Mul( GFL_QUATERNION* q, const GFL_QUATERNION* q1, const GFL_QUATERNION* q2 );

// �N�H�[�^�j�I���̉��Z
extern void GFL_QUAT_Add( GFL_QUATERNION* q, const GFL_QUATERNION* q1, const GFL_QUATERNION* q2 );

// �N�H�[�^�j�I���̌��Z
extern void GFL_QUAT_Sub( GFL_QUATERNION* q, const GFL_QUATERNION* q1, const GFL_QUATERNION* q2 );

// �N�H�[�^�j�I���̃m���� || a || ^ 2
extern fx32  GFL_QUAT_GetNormSqrt( const GFL_QUATERNION* q );

// �N�H�[�^�j�I���̃m���� || a ||
extern fx32  GFL_QUAT_GetNorm( const GFL_QUATERNION* q );

// �N�H�[�^�j�I�����}�g���b�N�X�ɐݒ�
extern void GFL_QUAT_SetMtx44( MtxFx44* mtx, const GFL_QUATERNION* q );

// �N�H�[�^�j�I�����}�g���b�N�X�ɐݒ�
extern void GFL_QUAT_SetQMtx44( GFL_QUATERNION_MTX44* qmtx, const GFL_QUATERNION* q );

// �N�H�[�^�j�I���}�g���b�N�X��fx32�^4x4�}�g���b�N�X�ɕϊ�
extern void GFL_QUAT_SetQMtx44_to_MtxFx44( const GFL_QUATERNION_MTX44* qmtx, MtxFx44* mtx );

// �N�H�[�^�j�I���}�g���b�N�X��fx32�^4x3�}�g���b�N�X�ɕϊ�
extern void GFL_QUAT_SetQMtx44_to_MtxFx43( const GFL_QUATERNION_MTX44* qmtx, MtxFx43* mtx );

// �N�H�[�^�j�I���̕ێ����Ă����]��
extern u16  GFL_QUAT_GetRotation( const GFL_QUATERNION* q );
#endif

//f32�v�Z�o�[�W����
#if 0
// -----------------------------------------
//
//	�N�H�[�^�j�I��
//
// -----------------------------------------
typedef union {
	
	struct {		
		f32	wr;		///< ����
		f32	xi;		///< ����
		f32	yi;		///< ����
		f32	zi;		///< ����
	};
	f32 q[ 4 ];
	
} GFL_QUATERNION;


// -----------------------------------------
//
//	�N�H�[�^�j�I���}�g���b�N�X ( 4x4 )
//
// -----------------------------------------
typedef union {
	
	struct {
		f32 _00, _01, _02, _03;
        f32 _10, _11, _12, _13;
        f32 _20, _21, _22, _23;
        f32 _30, _31, _32, _33;
	};
	f32 m[ 4 ][ 4 ];
	f32 a[ 16 ];
	
} GFL_QUATERNION_MTX44;




// �N�H�[�^�j�I���̏�����
extern void GFL_QUAT_Identity( GFL_QUATERNION* q );

// �N�H�[�^�j�I���̃R�s�[
extern void GFL_QUAT_Copy( GFL_QUATERNION* q1, GFL_QUATERNION* q2 );

// �N�H�[�^�j�I���������Ƃ̉�]�l���琶��
extern	void GFL_QUAT_MakeQuaternionXYZ( GFL_QUATERNION* qt, u16 rot_x, u16 rot_y,u16 rot_z );

// �N�H�[�^�j�I���̐�
extern void GFL_QUAT_Mul( GFL_QUATERNION* q, const GFL_QUATERNION* q1, const GFL_QUATERNION* q2 );

// �N�H�[�^�j�I���̉��Z
extern void GFL_QUAT_Add( GFL_QUATERNION* q, const GFL_QUATERNION* q1, const GFL_QUATERNION* q2 );

// �N�H�[�^�j�I���̌��Z
extern void GFL_QUAT_Sub( GFL_QUATERNION* q, const GFL_QUATERNION* q1, const GFL_QUATERNION* q2 );

// �N�H�[�^�j�I���̃m���� || a || ^ 2
extern f32  GFL_QUAT_GetNormSqrt( const GFL_QUATERNION* q );

// �N�H�[�^�j�I���̃m���� || a ||
extern f32  GFL_QUAT_GetNorm( const GFL_QUATERNION* q );

// �N�H�[�^�j�I�����}�g���b�N�X�ɐݒ�
extern void GFL_QUAT_SetMtx44( MtxFx44* mtx, const GFL_QUATERNION* q );

// �N�H�[�^�j�I�����}�g���b�N�X�ɐݒ�
extern void GFL_QUAT_SetQMtx44( GFL_QUATERNION_MTX44* qmtx, const GFL_QUATERNION* q );

// �N�H�[�^�j�I���}�g���b�N�X��fx32�^4x4�}�g���b�N�X�ɕϊ�
extern void GFL_QUAT_SetQMtx44_to_MtxFx44( const GFL_QUATERNION_MTX44* qmtx, MtxFx44* mtx );

// �N�H�[�^�j�I���}�g���b�N�X��fx32�^4x3�}�g���b�N�X�ɕϊ�
extern void GFL_QUAT_SetQMtx44_to_MtxFx43( const GFL_QUATERNION_MTX44* qmtx, MtxFx43* mtx );

// �N�H�[�^�j�I���̕ێ����Ă����]��
extern u16  GFL_QUAT_GetRotation( const GFL_QUATERNION* q );
#endif

#endif
