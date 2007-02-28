//=============================================================================================
/**
 * @file	camera.c                                                  
 * @brief	�J�����v���O����
 * @date	
 */
//=============================================================================================
#include "gflib.h"
#include "g3d_camera.h"

//=============================================================================================
//	�^�錾
//=============================================================================================
typedef struct _GFL_G3D_CAMERA GFL_G3D_CAMERA; 

struct _GFL_G3D_CAMERA {
	GFL_G3D_PROJECTION_TYPE type; 
	fx32					param1;
	fx32					param2;
	fx32					param3;
	fx32					param4;
	fx32					near;
	fx32					far;
	fx32					scaleW;
	VecFx32					camPos;
	VecFx32					camUp;
	VecFx32					target;
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
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_CAMERA*
	GFL_G3D_CameraCreate
		( const GFL_G3D_PROJECTION_TYPE type, 
			const fx32 param1, const fx32 param2, const fx32 param3, const fx32 param4, 
				const fx32 near, const fx32 far, const fx32 scaleW,
					const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target,
						HEAPID heapID )
{
	GFL_G3D_CAMERA*	g3Dcamera;
	//�Ǘ��̈�m��
	g3Dcamera = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_G3D_CAMERA) );

	g3Dcamera->type		= type;
	g3Dcamera->param1	= param1;
	g3Dcamera->param2	= param2;
	g3Dcamera->param3	= param3;
	g3Dcamera->param4	= param4;
	g3Dcamera->near		= near;
	g3Dcamera->far		= far;
	g3Dcamera->scaleW	= scaleW;
	g3Dcamera->camPos	= *camPos;
	g3Dcamera->camUp	= *camUp;
	g3Dcamera->target	= *target;

	return g3Dcamera;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����j��
 *
 * @param	g3Dcamera	�J�����|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_CameraDelete
		( GFL_G3D_CAMERA* g3Dcamera )
{
	GF_ASSERT( g3Dcamera );

	GFL_HEAP_FreeMemory( g3Dcamera );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����n�m
 *
 * @param	g3Dcamera	�J�����|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_CameraOn
		( GFL_G3D_CAMERA* g3Dcamera )
{
	GF_ASSERT( g3Dcamera );

	//�ˉe�ݒ�
	GFL_G3D_sysProjectionSet(	g3Dcamera->type,
								g3Dcamera->param1, g3Dcamera->param2,
								g3Dcamera->param3, g3Dcamera->param4,
								g3Dcamera->near, g3Dcamera->far,
								g3Dcamera->scaleW );
	//�J�����s��ݒ�
	GFL_G3D_sysLookAtSet( &g3Dcamera->camPos, &g3Dcamera->camUp, &g3Dcamera->target );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�������W�̎擾�ƕύX
 *
 * @param	g3Dcamera	�J�����|�C���^
 * @param	pos			���W�x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_CameraPosGet
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camPos )
{
	GF_ASSERT( g3Dcamera );
	*camPos = g3Dcamera->camPos;
}

void
	GFL_G3D_CameraPosSet
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camPos )
{
	GF_ASSERT( g3Dcamera );
	g3Dcamera->camPos = *camPos;
}

//--------------------------------------------------------------------------------------------
/**
 * �J����������̎擾�ƕύX
 *
 * @param	g3Dcamera	�J�����|�C���^
 * @param	pos			������x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_CameraUpGet
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camUp )
{
	GF_ASSERT( g3Dcamera );
	*camUp = g3Dcamera->camUp;
}

void
	GFL_G3D_CameraUpSet
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camUp )
{
	GF_ASSERT( g3Dcamera );
	g3Dcamera->camUp = *camUp;
}

//--------------------------------------------------------------------------------------------
/**
 * �J���������_�̎擾�ƕύX
 *
 * @param	g3Dcamera	�J�����|�C���^
 * @param	target		�����_�x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_CameraTargetGet
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* target )
{
	GF_ASSERT( g3Dcamera );
	*target = g3Dcamera->target;
}

void
	GFL_G3D_CameraTargetSet
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* target )
{
	GF_ASSERT( g3Dcamera );
	g3Dcamera->target = *target;
}

