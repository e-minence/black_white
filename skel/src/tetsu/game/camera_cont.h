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

