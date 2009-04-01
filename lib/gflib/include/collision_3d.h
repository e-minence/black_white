//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		collision_3d.h
 *	@brief		�����蔻��c�[��
 *	@author		tomoya takahashi
 *	@data		2008.09.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __COLLISION_3D_H__
#define __COLLISION_3D_H__

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
/**
 *			������
 *
 *		OBB����
 *
 *		�R���W���������́A�Q�[���v���O���~���O�̂��߂́A���A���^�C���Փ˔�������Ȃ���쐬���܂����B
 *		page.104
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	�L�����E�{�b�N�X OBB
//=====================================
typedef struct {
	VecFx32	center;		// ���S���W
	MtxFx33	rotate;		// �����s��
	VecFx32	width;		// �e���̕��̔����̃T�C�Y
} GFL_COLLISION3D_OBB;

//-------------------------------------
///	���胊�U���g
//=====================================
typedef struct {
	BOOL	hit;		// ����������

	// ���㑝����\��������
} GFL_COLLISION3D_OBB_RESULT;


// ����
extern BOOL GFL_COLLISION3D_OBB_CheckOBB( const GFL_COLLISION3D_OBB* cp_a, const GFL_COLLISION3D_OBB* cp_b, GFL_COLLISION3D_OBB_RESULT* p_result );
extern BOOL GFL_COLLISION3D_OBB_CheckRay( const GFL_COLLISION3D_OBB* cp_obb, const VecFx32* cp_rayPos, const VecFx32* cp_rayWay, fx32* p_dist );

// ��{�{�b�N�X���̐ݒ�
extern void GFL_COLLISION3D_OBB_SetData( GFL_COLLISION3D_OBB* p_obb, const VecFx32* cp_center, const MtxFx33* cp_rotate, const VecFx32* cp_width );
extern void GFL_COLLISION3D_OBB_Trans( GFL_COLLISION3D_OBB* p_obb, const VecFx32* cp_trans );
extern void GFL_COLLISION3D_OBB_Rotate( GFL_COLLISION3D_OBB* p_obb, const MtxFx33* cp_rotate );
extern void GFL_COLLISION3D_OBB_Scale( GFL_COLLISION3D_OBB* p_obb, const VecFx32* cp_scale );

//-----------------------------------------------------------------------------
/**
 *			��
 */
//-----------------------------------------------------------------------------

//-------------------------------------
///	��
//=====================================
typedef struct {
	VecFx32	center;
	fx32	r;
} GFL_COLLISION3D_CIRCLE;


//-------------------------------------
///	���胊�U���g
//=====================================
typedef struct {
	BOOL	hit;		// ����������
	VecFx32 way;		// �ǂ̕�����
	fx32	dist;		// �ǂ̂��炢�H������ł��邩
} GFL_COLLISION3D_CIRCLE_RESULT;

// ����
extern BOOL GFL_COLLISION3D_CIRCLE_CheckCIRCLE( const GFL_COLLISION3D_CIRCLE* cp_a, const GFL_COLLISION3D_CIRCLE* cp_b, GFL_COLLISION3D_CIRCLE_RESULT* p_result );

// ��{���̐ݒ�
extern void GFL_COLLISION3D_CIRCLE_SetData( GFL_COLLISION3D_CIRCLE* p_circle, const VecFx32* cp_center, fx32 r );

//-----------------------------------------------------------------------------
/**
 *			�~���Ɖ~�̓����蔻��
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	�~��
//=====================================
typedef struct {
	VecFx32	start;	//�~���̍����J�n�_
	VecFx32	end;	//�~���̍����I���_
	fx32	r;		//�~���̔��a�i���a��0�ɂ����ꍇ�A�����Ɖ~�̓����蔻��ɂȂ�܂��j
} GFL_COLLISION3D_CYLINDER;

//-------------------------------------
///	���胊�U���g
//=====================================
typedef struct {
	BOOL	hit;		// ����������
	VecFx32 way;		// �ǂ̕�����
	fx32	dist;		// �ǂ̂��炢�H������ł��邩
} GFL_COLLISION3D_CYLXCIR_RESULT;

// ����
extern BOOL GFL_COLLISION3D_CYLXCIR_Check( const GFL_COLLISION3D_CYLINDER* cp_l, const GFL_COLLISION3D_CIRCLE* cp_c, GFL_COLLISION3D_CYLXCIR_RESULT* p_result );

// �~�����̐ݒ�
extern void GFL_COLLISION3D_CYLINDER_SetData( GFL_COLLISION3D_CYLINDER* p_cyl, const VecFx32* cp_start, const VecFx32* cp_end, fx32 r );



//-----------------------------------------------------------------------------
/**
 *			���C�g���[�X
 */
//-----------------------------------------------------------------------------
//	�v�Z���ʒ�`
typedef enum {
	GFL_COLLISION3D_FALSE = 0,	///<���s
	GFL_COLLISION3D_TRUE,		///<����
	GFL_COLLISION3D_OUTRANGE,	///<�͈͊O
}GFL_COLLISION3D_RESULT;

//--------------------------------------------------------------------------------------------
/**
 * ���ʌv�Z
 * �@�_�����ʏ�ɂ��邩�ǂ����̔���
 *
 *		�w�肳�ꂽ�_�����ʂ̕����� (P - P1).N = 0�i���όv�Z�j
 *			��P1:���ʏ�̔C�ӂ̓_,N:�@���x�N�g��
 *		���݂������ǂ�������
 *
 * @param	pos			�w��ʒu
 * @param	posRef		���ʏ�̈�_�̈ʒu
 * @param	vecN		���ʂ̖@���x�N�g��
 * @param	margin		���e����v�Z�덷��
 *
 * @return	GFL_COLLISION3D_RESULT		���茋��
 */
//--------------------------------------------------------------------------------------------
extern GFL_COLLISION3D_RESULT
	GFL_COLLISION3D_CheckPointOnPlane
		( const VecFx32* pos, const VecFx32* posRef, const VecFx32* vecN, const fx32 margin );
//--------------------------------------------------------------------------------------------
/**
 * ���C�g���[�X�v�Z
 * �@���C�ƕ��ʂ̌�_�x�N�g�����Z�o
 *
 *		�����̕����� P = P0 + t * V		
 *			��P:���݈ʒu,P0:�����ʒu,t:�o�߃I�t�Z�b�g�i���ԁj,V:�i�s�x�N�g�� 
 *		�ƕ��ʂ̕����� (P - P0).N = 0�i���όv�Z�j
 *			��P�����P0:���ʏ�̔C�ӂ̂Q�_,N:�@���x�N�g��
 *		�𓯎��ɂ݂����_P����_�ł��邱�Ƃ𗘗p���ĎZ�o
 *
 * @param	posRay		���C�̔��ˈʒu
 * @param	vecRay		���C�̐i�s�x�N�g��
 * @param	posRef		���ʏ�̈�_�̈ʒu
 * @param	vecN		���ʂ̖@���x�N�g��
 * @param	dest		��_�̈ʒu
 * @param	margin		���e����v�Z�덷��
 *
 * @return	GFL_COLLISION3D_RESULT		�Z�o����
 */
//--------------------------------------------------------------------------------------------
extern GFL_COLLISION3D_RESULT
	GFL_COLLISION3D_GetClossPointRayPlane
		( const VecFx32* posRay, const VecFx32* vecRay, 
			const VecFx32* posRef, const VecFx32* vecN, VecFx32* dest, const fx32 margin );
//--------------------------------------------------------------------------------------------
/**
 * ���C�g���[�X�v�Z�i�����������j
 * �@���C�ƕ��ʂ̌�_�x�N�g�����Z�o
 *
 *		�����̕����� P = P0 + t * V		
 *			��P:���݈ʒu,P0:�����ʒu,t:�o�߃I�t�Z�b�g�i���ԁj,V:�i�s�x�N�g�� 
 *		�ƕ��ʂ̕����� (P - P1).N = 0�i���όv�Z�j
 *			��P�����P1:���ʏ�̔C�ӂ̂Q�_,N:�@���x�N�g��
 *		�𓯎��ɂ݂����_P����_�ł��邱�Ƃ𗘗p���ĎZ�o
 *
 *		������A������ P = P1 + ((P1 - P0 ).N / V.N) * V
 *		�Ƃ����������𓾂�
 *
 * @param	posRay		���C�̔��ˈʒu
 * @param	posRayEnd	���C�̍ŏI���B�ʒu
 * @param	posRef		���ʏ�̈�_�̈ʒu
 * @param	vecN		���ʂ̖@���x�N�g��
 * @param	dest		��_�̈ʒu
 * @param	margin		���e����v�Z�덷��
 *
 * @return	GFL_COLLISION3D_RESULT		�Z�o����
 */
//--------------------------------------------------------------------------------------------
extern GFL_COLLISION3D_RESULT
	GFL_COLLISION3D_GetClossPointRayPlaneLimit
		( const VecFx32* posRay, const VecFx32* posRayEnd, 
			const VecFx32* posRef, const VecFx32* vecN, VecFx32* dest, const fx32 margin );




#ifdef __cplusplus
}/* extern "C" */
#endif


#endif		// __COLLISION_3D_H__

