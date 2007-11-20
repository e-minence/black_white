//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
typedef struct _CAMERA_CONTROL	CAMERA_CONTROL;

// �J�����R���g���[���Z�b�g
extern CAMERA_CONTROL* AddCameraControl( GAME_SYSTEM* gs, int targetAct, HEAPID heapID );
// �J�����R���g���[�����C��
extern void MainCameraControl( CAMERA_CONTROL* cc );
// �J�����R���g���[���I��
extern void RemoveCameraControl( CAMERA_CONTROL* cc );
// �J������]�l�̎擾�Ɛݒ�
extern void GetCameraControlDirection( CAMERA_CONTROL* cc, u16* value );
extern void SetCameraControlDirection( CAMERA_CONTROL* cc, u16* value );
// �J�����^�[�Q�b�g�h�c�̎擾�Ɛݒ�
extern void GetCameraControlTargetID( CAMERA_CONTROL* cc, int* ID );
extern void SetCameraControlTargetID( CAMERA_CONTROL* cc, int* ID );
// �J���������ƑΏە��̓��ώ擾
extern void GetCameraControlDotProduct( CAMERA_CONTROL* cc, VecFx32* trans, int* scalar );
// �J�����O�`�F�b�N�F�J�����Ƃ̃X�J���[�l�ɂ��J�����O
extern BOOL CullingCameraScalar( CAMERA_CONTROL* cc, VecFx32* trans, int scalarLimit );
// �J�����O�`�F�b�N�F�J�����̎��E�ɂ��J�����O�i�w�y�̂݁j
extern BOOL CullingCameraBitween( CAMERA_CONTROL* cc, VecFx32* trans, u16 theta );
// �J�����O�`�F�b�N�F�J�����Ώە��̎��͈ʒu�ɂ��J�����O�i�w�y�̂݁j
extern BOOL CullingCameraAround( CAMERA_CONTROL* cc, VecFx32* trans, fx32 len );


