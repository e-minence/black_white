//============================================================================================
/**
 * @file	camera_cont.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "setup.h"

#include "camera_cont.h"
//============================================================================================
//
//
//	�J�����R���g���[��
//
//
//============================================================================================
#define	MAINCAMERA_POFS_LENGTH	(64)
#define	MAINCAMERA_POFS_HEIGHT	(40)
#define	MAINCAMERA_TOFS_LENGTH	(0)
#define	MAINCAMERA_TOFS_HEIGHT	(16)

#define	VIEWCAMERA_POFS_LENGTH	(24)
#define	VIEWCAMERA_POFS_HEIGHT	(24)
#define	VIEWCAMERA_TOFS_LENGTH	(0)
#define	VIEWCAMERA_TOFS_HEIGHT	(10)

#define	VIEWCAMERA_AUTO_ROTATEVALUE	(0x0080)
//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
struct _CAMERA_CONTROL {
	GAME_SYSTEM*	gs;

	u16				seq;
	VecFx32			mainCameraTrans;
	u16				mainCameraDirection;
	VecFx32			autoCameraTrans;
	u16				autoCameraDirection;
	HEAPID			heapID;
};

//------------------------------------------------------------------
/**
 * @brief	�J�����R���g���[���Z�b�g
 */
//------------------------------------------------------------------
CAMERA_CONTROL* AddCameraControl( GAME_SYSTEM* gs, HEAPID heapID )
{
	CAMERA_CONTROL* cc = GFL_HEAP_AllocClearMemory( heapID, sizeof(CAMERA_CONTROL) );
	cc->heapID = heapID;
	cc->gs = gs;
	cc->mainCameraTrans.x = 0;
	cc->mainCameraTrans.y = 0;
	cc->mainCameraTrans.z = 0;
	cc->mainCameraDirection = 0;
	cc->autoCameraTrans.x = 0;
	cc->autoCameraTrans.y = 0;
	cc->autoCameraTrans.z = 0;
	cc->autoCameraDirection = 0;

	return cc;
}

//------------------------------------------------------------------
/**
 * @brief	�J�����R���g���[���I��
 */
//------------------------------------------------------------------
void RemoveCameraControl( CAMERA_CONTROL* cc )
{
	GFL_HEAP_FreeMemory( cc ); 
}

//------------------------------------------------------------------
/**
 * @brief	�J�������W�̎擾�Ɛݒ�
 */
//------------------------------------------------------------------
void GetCameraControlTrans( CAMERA_CONTROL* cc, VecFx32* trans )
{
	*trans = cc->mainCameraTrans;
}

void SetCameraControlTrans( CAMERA_CONTROL* cc, VecFx32* trans )
{
	cc->mainCameraTrans = *trans;
}

//------------------------------------------------------------------
/**
 * @brief	�J���������̎擾�Ɛݒ�
 */
//------------------------------------------------------------------
void GetCameraControlDirection( CAMERA_CONTROL* cc, u16* value )
{
	*value = cc->mainCameraDirection;
}

void SetCameraControlDirection( CAMERA_CONTROL* cc, u16* value )
{
	cc->mainCameraDirection = *value;
}

//------------------------------------------------------------------
/**
 * @brief	�J���������ƑΏە��̓��ώ擾
 */
//------------------------------------------------------------------
void GetCameraControlDotProduct( CAMERA_CONTROL* cc, VecFx32* trans, int* scalar )
{
	*scalar = GFL_G3D_CAMERA_GetDotProductXZfast( Get_GS_G3Dcamera(cc->gs,MAINCAMERA_ID), trans );
}

//------------------------------------------------------------------
/**
 * @brief	�J�����O�`�F�b�N�F�J�����Ƃ̃X�J���[�l�ɂ��J�����O
 */
//------------------------------------------------------------------
BOOL CullingCameraScalar( CAMERA_CONTROL* cc, VecFx32* trans, int scalarLimit )
{
	GFL_G3D_CAMERA* g3Dcamera = Get_GS_G3Dcamera( cc->gs, MAINCAMERA_ID );
	int scalar;

	//�X�J���[�l�擾
	scalar = GFL_G3D_CAMERA_GetDotProductXZfast( g3Dcamera, trans ); 

	//�J�����Ƃ̃X�J���[�ɂ��ʒu����(0�͐����A���͑O���A���͌��)
	if( scalar < scalarLimit ){
		return FALSE;
	} 
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�J�����O�`�F�b�N�F�J�����̎��E�ɂ��J�����O�i�w�y�̂݁j
 */
//------------------------------------------------------------------
BOOL CullingCameraBitween( CAMERA_CONTROL* cc, VecFx32* trans, u16 theta )
{
	GFL_G3D_CAMERA* g3Dcamera = Get_GS_G3Dcamera( cc->gs, MAINCAMERA_ID );
	VecFx32 cameraPos, cameraTarget;
	VecFx32 vecView, vecObj;
	VecFx32 vecA_cross, vecB_cross;
	fx32	sin, cos;
	int		scalarA, scalarB;
	s16		tmpTheta;

	//�J�������擾
	GFL_G3D_CAMERA_GetPos( g3Dcamera, &cameraPos );
	GFL_G3D_CAMERA_GetTarget( g3Dcamera, &cameraTarget );

	//���E�x�N�g���v�Z�i�������̂݁j
	vecView.x = ( cameraTarget.x - cameraPos.x ) >> FX32_SHIFT;
	vecView.z = ( cameraTarget.z - cameraPos.z ) >> FX32_SHIFT;

	//�Ώە��̃x�N�g���v�Z�i�������̂݁j
	vecObj.x = ( trans->x - cameraPos.x ) >> FX32_SHIFT;
	vecObj.z = ( trans->z - cameraPos.z ) >> FX32_SHIFT;

	//�J�����̕������獶�����E�x�N�g���`�ɒ�������x�N�g�����쐬
	tmpTheta = -theta + 0x4000;	//��������+90���̊p�x�l
	sin = FX_SinIdx((u16)tmpTheta);
	cos = FX_CosIdx((u16)tmpTheta);
	vecA_cross.x = (vecView.x * cos ) - (vecView.z * sin );
	vecA_cross.z = (vecView.x * sin ) + (vecView.z * cos );

	//�J�����̕�������E�����E�x�N�g���a�ɒ�������x�N�g�����쐬
	tmpTheta = theta - 0x4000;	//��������-90���̊p�x�l
	sin = FX_SinIdx((u16)tmpTheta);
	cos = FX_CosIdx((u16)tmpTheta);
	vecB_cross.x = (vecView.x * cos ) - (vecView.z * sin );
	vecB_cross.z = (vecView.x * sin ) + (vecView.z * cos );

	//�x�N�g���`�ɒ�������x�N�g���ƃx�N�g���a�ɒ�������x�N�g���ɑ΂��Ă̓��ς�
	//�Ƃ��Ɏw��X�J���[�l�͈͓��ł���΂Q�����Ԃɑ��݂���ƌ��肷��
	scalarA = vecA_cross.x * vecObj.x + vecA_cross.z * vecObj.z;
	scalarB = vecB_cross.x * vecObj.x + vecB_cross.z * vecObj.z;
	if(( scalarA < 0 )||( scalarB < 0 )){
		return FALSE;
	} else {
		return TRUE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�J�����O�`�F�b�N�F�J�����Ώە��̎��͈ʒu�ɂ��J�����O�i�w�y�̂݁j
 */
//------------------------------------------------------------------
BOOL CullingCameraAround( CAMERA_CONTROL* cc, VecFx32* trans, fx32 len )
{
	GFL_G3D_CAMERA* g3Dcamera = Get_GS_G3Dcamera( cc->gs, MAINCAMERA_ID );
	VecFx32 cameraTarget;
	fx32	diffX, diffZ, diffLen;
				
	GFL_G3D_CAMERA_GetTarget( g3Dcamera, &cameraTarget );

	diffX = ( cameraTarget.x - trans->x ) >> FX32_SHIFT;
	diffZ = ( cameraTarget.z - trans->z ) >> FX32_SHIFT;
	diffLen = ( diffX * diffX + diffZ * diffZ );
	if( diffLen > len ){
		return FALSE;
	} else {
		return TRUE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�J�����R���g���[�����C��
 */
//------------------------------------------------------------------
void MainCameraControl( CAMERA_CONTROL* cc )
{
	GFL_G3D_CAMERA* g3Dcamera;
	VecFx32 targetTrans;
	VecFx32 cameraPos, cameraTarget;

	//���J��������
	targetTrans = cc->mainCameraTrans;

	cameraPos.x = targetTrans.x + MAINCAMERA_POFS_LENGTH * FX_SinIdx(cc->mainCameraDirection);
	cameraPos.y = targetTrans.y + MAINCAMERA_POFS_HEIGHT * FX32_ONE;
	cameraPos.z = targetTrans.z + MAINCAMERA_POFS_LENGTH * FX_CosIdx(cc->mainCameraDirection);

	cameraTarget.x = targetTrans.x + MAINCAMERA_TOFS_LENGTH * FX_SinIdx(cc->mainCameraDirection);
	cameraTarget.y = targetTrans.y + MAINCAMERA_TOFS_HEIGHT * FX32_ONE;
	cameraTarget.z = targetTrans.z + MAINCAMERA_TOFS_LENGTH * FX_CosIdx(cc->mainCameraDirection);

	g3Dcamera = Get_GS_G3Dcamera( cc->gs, MAINCAMERA_ID );
	GFL_G3D_CAMERA_SetPos( g3Dcamera, &cameraPos );
	GFL_G3D_CAMERA_SetTarget( g3Dcamera, &cameraTarget );

	//��J��������
	targetTrans = cc->autoCameraTrans;

	cameraPos.x = targetTrans.x + VIEWCAMERA_POFS_LENGTH * FX_SinIdx(cc->autoCameraDirection);
	cameraPos.y = targetTrans.y + VIEWCAMERA_POFS_HEIGHT * FX32_ONE;
	cameraPos.z = targetTrans.z + VIEWCAMERA_POFS_LENGTH * FX_CosIdx(cc->autoCameraDirection);

	cameraTarget.x = targetTrans.x + VIEWCAMERA_TOFS_LENGTH * FX_SinIdx(cc->autoCameraDirection);
	cameraTarget.y = targetTrans.y + VIEWCAMERA_TOFS_HEIGHT * FX32_ONE;
	cameraTarget.z = targetTrans.z + VIEWCAMERA_TOFS_LENGTH * FX_CosIdx(cc->autoCameraDirection);

	g3Dcamera = Get_GS_G3Dcamera( cc->gs, SUBCAMERA_ID );
	GFL_G3D_CAMERA_SetPos( g3Dcamera, &cameraPos );
	GFL_G3D_CAMERA_SetTarget( g3Dcamera, &cameraTarget );

	cc->autoCameraDirection += VIEWCAMERA_AUTO_ROTATEVALUE;
}



