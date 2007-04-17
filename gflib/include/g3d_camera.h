//=============================================================================================
/**
 * @file	g3d_camera.h
 * @brief	�J�����ݒ�
 * @date	
 */
//=============================================================================================
#ifndef _G3D_CAMERA_H_
#define _G3D_CAMERA_H_

//=============================================================================================
//	�^�錾
//=============================================================================================
typedef struct _GFL_G3D_CAMERA	GFL_G3D_CAMERA;

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
extern GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_Create
		( const GFL_G3D_PROJECTION_TYPE type, 
			const fx32 param1, const fx32 param2, const fx32 param3, const fx32 param4, 
				const fx32 near, const fx32 far, const fx32 scaleW,
					const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target,
						HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * ��ʓI�ȓ����ˉe�J�����쐬(inline)
 *
 * @param	fovy		�c(Y)�����̎��E�p�x
 * @param	camPos		�J�����ʒu�x�N�g���|�C���^
 * @param	camUp		�J�����̏�����̃x�N�g���ւ̃|�C���^
 * @param	target		�J�����œ_�ւ̃|�C���^
 *
 * @return	g3Dcamera	�J�����n���h��
 */
//--------------------------------------------------------------------------------------------
#define defaultCameraFovy	( 40 )
#define defaultCameraAspect	( FX32_ONE * 4 / 3 )
#define defaultCameraNear	( 1 << FX32_SHIFT )
#define defaultCameraFar	( 1024 << FX32_SHIFT )

inline GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_CreateDefault
		( const VecFx32* camPos, const VecFx32* target, HEAPID heapID )
{
	VecFx32 defaultCameraUp = { 0, FX32_ONE, 0 };
 
	return GFL_G3D_CAMERA_Create(	GFL_G3D_PRJPERS, 
									FX_SinIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
									FX_CosIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
									defaultCameraAspect, 0,
									defaultCameraNear, defaultCameraFar, 0,
									camPos, &defaultCameraUp, target, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * ���E�p�x�ɂ�铧���ˉe�J�����쐬(inline)
 *
 * @param	fovy		�c(Y)�����̎��E�p�x
 * @param	aspect		�c����F���^�����~FX32_ONE
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
inline GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_CreatePerspective
		( const u16 fovy, const fx32 aspect, const fx32 near, const fx32 far, const fx32 scaleW,
			const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target, HEAPID heapID )
{
	return GFL_G3D_CAMERA_Create(	GFL_G3D_PRJPERS, 
									FX_SinIdx( fovy/2 *PERSPWAY_COEFFICIENT ),
									FX_CosIdx( fovy/2 *PERSPWAY_COEFFICIENT ),
									aspect, 0, near, far, scaleW,
									camPos, camUp, target, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * �N���b�v���W�w��ɂ�铧���ˉe�J�����쐬(inline)
 *
 * @param	top			near�N���b�v�ʏ�ӂ�Y���W
 * @param	bottom		near�N���b�v�ʉ��ӂ�Y���W
 * @param	left		near�N���b�v�ʍ��ӂ�Y���W
 * @param	right		near�N���b�v�ʉE�ӂ�Y���W
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
inline GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_CreateFrustum
		( const fx32 top, const fx32 bottom, const fx32 left, const fx32 right, 
			const fx32 near, const fx32 far, const fx32 scaleW,
				const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target, HEAPID heapID )
{
	return GFL_G3D_CAMERA_Create(	GFL_G3D_PRJFRST, top, bottom, left, right,
									near, far, scaleW, camPos, camUp, target, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * �N���b�v���W�w��ɂ�鐳�ˉe�J�����쐬(inline)
 *
 * @param	top			near�N���b�v�ʏ�ӂ�Y���W
 * @param	bottom		near�N���b�v�ʉ��ӂ�Y���W
 * @param	left		near�N���b�v�ʍ��ӂ�Y���W
 * @param	right		near�N���b�v�ʉE�ӂ�Y���W
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
inline GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_CreateOrtho
		( const fx32 top, const fx32 bottom, const fx32 left, const fx32 right, 
			const fx32 near, const fx32 far, const fx32 scaleW,
				const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target, HEAPID heapID )
{
	return GFL_G3D_CAMERA_Create(	GFL_G3D_PRJORTH, top, bottom, left, right,
									near, far, scaleW, camPos, camUp, target, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����j��
 *
 * @param	g3Dcamera	�J�����n���h��
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_CAMERA_Delete
		( GFL_G3D_CAMERA* g3Dcamera );

//--------------------------------------------------------------------------------------------
/**
 * �J�������f
 *
 * @param	g3Dcamera	�J�����n���h��
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_CAMERA_Switching
		( GFL_G3D_CAMERA* g3Dcamera );

//--------------------------------------------------------------------------------------------
/**
 * �J�������W�̎擾�ƕύX
 *
 * @param	g3Dcamera	�J�����n���h��
 * @param	pos			���W�x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_CAMERA_GetPos( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camPos );
extern void GFL_G3D_CAMERA_SetPos( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camPos );
//--------------------------------------------------------------------------------------------
/**
 * �J����������̎擾�ƕύX
 *
 * @param	g3Dcamera	�J�����n���h��
 * @param	pos			������x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_CAMERA_GetCamUp( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camUp );
extern void GFL_G3D_CAMERA_SetCamUp( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camUp );
//--------------------------------------------------------------------------------------------
/**
 * �J���������_�̎擾�ƕύX
 *
 * @param	g3Dcamera	�J�����n���h��
 * @param	target		�����_�x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_CAMERA_GetTarget( GFL_G3D_CAMERA* g3Dcamera, VecFx32* target );
extern void GFL_G3D_CAMERA_SetTarget( GFL_G3D_CAMERA* g3Dcamera, VecFx32* target );
//--------------------------------------------------------------------------------------------
/**
 * �j�A�t�@�[�N���b�v�̎擾�ƕύX
 *
 * @param	g3Dcamera	�J�����n���h��
 * @param	near or far	�j�A�t�@�[�l�̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_CAMERA_GetNear( GFL_G3D_CAMERA* g3Dcamera, fx32* near );
extern void GFL_G3D_CAMERA_SetNear( GFL_G3D_CAMERA* g3Dcamera, fx32* near );
extern void GFL_G3D_CAMERA_GetFar( GFL_G3D_CAMERA* g3Dcamera, fx32* far );
extern void GFL_G3D_CAMERA_SetFar( GFL_G3D_CAMERA* g3Dcamera, fx32* far );

//--------------------------------------------------------------------------------------------
/**
 * �J���������ƑΏۈʒu�Ƃ̓��ς��擾����
 *
 * @param	g3Dcamera	�J�����n���h��
 * @param	objPos		�I�u�W�F�N�g�ʒu�x�N�g��
 *
 * ��ɊȈՃJ�����O�ȂǂɎg�p����
 * ���ς̒l���@	0	:�Ώۂ͐����i�J�����̌����ɑ΂��Đ����̃x�N�g���j�Ɉʒu����
 *				��	:�Ώۂ͑O���Ɉʒu����
 *				��	:�Ώۂ͌���Ɉʒu����
 */
//--------------------------------------------------------------------------------------------
//���K��
extern fx32
	GFL_G3D_CAMERA_GetDotProduct
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* objPos );

//�ȈՔŁ@���Q�c�i�w�y���W�j��p�B�X�J���[���x�͗����邪��⍂��
extern int
	GFL_G3D_CAMERA_GetDotProductXZfast
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* objPos );


#endif

