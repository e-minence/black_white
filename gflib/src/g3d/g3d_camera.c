//=============================================================================================
/**
 * @file	g3d_camera.c                                                  
 * @brief	�J�����ݒ�
 * @date	
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	�^�錾
//=============================================================================================
struct _GFL_G3D_CAMERA {
	GFL_G3D_PROJECTION	projection; 
	GFL_G3D_LOOKAT		lookAt; 
};

//=============================================================================================
/**
 *
 *
 * �Ǘ��V�X�e��
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �J�����쐬
 *
 * @param	type		�ˉe�^�C�v
 * @param	param1		�p�����[�^�P�i�ˉe�^�C�v�ɂ���ĈقȂ�Bg3d_system.h �Q�Ɓj
 * @param	param2		�p�����[�^�Q�i�ˉe�^�C�v�ɂ���ĈقȂ�Bg3d_system.h �Q�Ɓj
 * @param	param3		�p�����[�^�R�i�ˉe�^�C�v�ɂ���ĈقȂ�Bg3d_system.h �Q�Ɓj
 * @param	param4		�p�����[�^�S�i�ˉe�^�C�v�ɂ���ĈقȂ�Bg3d_system.h �Q�Ɓj
 * @param	near		���_����near�N���b�v�ʂ܂ł̋���	
 * @param	far			���_����far�N���b�v�ʂ܂ł̋���	
 * @param	scaleW		�r���[�{�����[���̐��x�����p�����[�^�i�g�p���Ȃ��Ƃ���0�j
 * @param	camPos		�J�����ʒu�x�N�g���|�C���^
 * @param	camUp		�J�����̏�����̃x�N�g���ւ̃|�C���^
 * @param	target		�J�����œ_�ւ̃|�C���^
 *
 * @return	g3Dcamera	�J�����n���h��
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_Create
		( const GFL_G3D_PROJECTION_TYPE type, 
			const fx32 param1, const fx32 param2, const fx32 param3, const fx32 param4, 
				const fx32 near, const fx32 far, const fx32 scaleW,
					const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target,
						HEAPID heapID )
{
	GFL_G3D_CAMERA*	g3Dcamera;
	//�Ǘ��̈�m��
	g3Dcamera = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_G3D_CAMERA) );

	g3Dcamera->projection.type		= type;
	g3Dcamera->projection.param1	= param1;
	g3Dcamera->projection.param2	= param2;
	g3Dcamera->projection.param3	= param3;
	g3Dcamera->projection.param4	= param4;
	g3Dcamera->projection.near		= near;
	g3Dcamera->projection.far		= far;
	g3Dcamera->projection.scaleW	= scaleW;
	g3Dcamera->lookAt.camPos		= *camPos;
	g3Dcamera->lookAt.camUp			= *camUp;
	g3Dcamera->lookAt.target		= *target;

	return g3Dcamera;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����j��
 *
 * @param	g3Dcamera	�J�����n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_CAMERA_Delete
		( GFL_G3D_CAMERA* g3Dcamera )
{
	GF_ASSERT( g3Dcamera );

	GFL_HEAP_FreeMemory( g3Dcamera );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�������f
 *
 * @param	g3Dcamera	�J�����n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_CAMERA_Switching
		( GFL_G3D_CAMERA* g3Dcamera )
{
	GF_ASSERT( g3Dcamera );

	//�ˉe�ݒ�
	GFL_G3D_SetSystemProjection(	g3Dcamera->projection.type,
									g3Dcamera->projection.param1, g3Dcamera->projection.param2,
									g3Dcamera->projection.param3, g3Dcamera->projection.param4,
									g3Dcamera->projection.near, g3Dcamera->projection.far,
									g3Dcamera->projection.scaleW );
	//�J�����s��ݒ�
	GFL_G3D_SetSystemLookAt(		&g3Dcamera->lookAt.camPos, 
									&g3Dcamera->lookAt.camUp,
									&g3Dcamera->lookAt.target );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�������W�̎擾�ƕύX
 *
 * @param	g3Dcamera	�J�����n���h��
 * @param	pos			���W�x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_CAMERA_GetPos
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camPos )
{
	GF_ASSERT( g3Dcamera );
	*camPos = g3Dcamera->lookAt.camPos;
}

void
	GFL_G3D_CAMERA_SetPos
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camPos )
{
	GF_ASSERT( g3Dcamera );
	g3Dcamera->lookAt.camPos = *camPos;
}

//--------------------------------------------------------------------------------------------
/**
 * �J����������̎擾�ƕύX
 *
 * @param	g3Dcamera	�J�����n���h��
 * @param	pos			������x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_CAMERA_GetCamUp
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camUp )
{
	GF_ASSERT( g3Dcamera );
	*camUp = g3Dcamera->lookAt.camUp;
}

void
	GFL_G3D_CAMERA_SetCamUp
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camUp )
{
	GF_ASSERT( g3Dcamera );
	g3Dcamera->lookAt.camUp = *camUp;
}

//--------------------------------------------------------------------------------------------
/**
 * �J���������_�̎擾�ƕύX
 *
 * @param	g3Dcamera	�J�����n���h��
 * @param	target		�����_�x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_CAMERA_GetTarget
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* target )
{
	GF_ASSERT( g3Dcamera );
	*target = g3Dcamera->lookAt.target;
}

void
	GFL_G3D_CAMERA_SetTarget
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* target )
{
	GF_ASSERT( g3Dcamera );
	g3Dcamera->lookAt.target = *target;
}

//--------------------------------------------------------------------------------------------
/**
 * �J���������ƑΏۈʒu�Ƃ̓��ς��擾����
 *
 * @param	g3Dcamera	�J�����n���h��
 * @param	objPos		�I�u�W�F�N�g�ʒu�x�N�g��
 *
 * ��ɊȈՃJ�����O�ȂǂɎg�p����
 * ���ς̒l���@	0	:�Ώۃx�N�g���͐����i�J�����̌����ɑ΂��Đ����̃x�N�g���j�Ɉʒu����
 *				��	:�Ώۃx�N�g���͑O���Ɉʒu����
 *				��	:�Ώۃx�N�g���͌���Ɉʒu����
 */
//--------------------------------------------------------------------------------------------
//���K��
fx32
	GFL_G3D_CAMERA_GetDotProduct
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* objPos )
{
	VecFx32	viewVec, objVec;
	fx32	scalar;
			
	GF_ASSERT( g3Dcamera );

	//���E�x�N�g���v�Z
	VEC_Subtract( &g3Dcamera->lookAt.target, &g3Dcamera->lookAt.camPos, &viewVec );
	//�Ώە��̃x�N�g���v�Z
	VEC_Subtract( objPos, &g3Dcamera->lookAt.camPos, &objVec );

	//���E�x�N�g���ƑΏە��̃x�N�g���̓��όv�Z
	return VEC_DotProduct( &viewVec, &objVec );
}

//�ȈՔŁ@���Q�c�i�w�y���W�j��p�B�X�J���[���x�͗����邪��⍂��
int
	GFL_G3D_CAMERA_GetDotProductXZfast
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* objPos )
{
	int viewVecX, viewVecZ, objVecX, objVecZ;
			
	GF_ASSERT( g3Dcamera );

	//���E�x�N�g���v�Z�i�������̂݁j
	viewVecX = ( g3Dcamera->lookAt.target.x - g3Dcamera->lookAt.camPos.x )/FX32_ONE;
	viewVecZ = ( g3Dcamera->lookAt.target.z - g3Dcamera->lookAt.camPos.z )/FX32_ONE;

	//�Ώە��̃x�N�g���v�Z�i�������̂݁j
	objVecX = ( objPos->x - g3Dcamera->lookAt.camPos.x )/FX32_ONE;
	objVecZ = ( objPos->z - g3Dcamera->lookAt.camPos.z )/FX32_ONE;

	//���E�x�N�g���ƑΏە��̃x�N�g���̓��όv�Z�i�w�y�j
	return viewVecX * objVecX + viewVecZ * objVecZ;
}

