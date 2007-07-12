//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
typedef struct _STATUSWIN_CONTROL	STATUSWIN_CONTROL;

// �X�e�[�^�X�E�C���h�E�R���g���[���Z�b�g
extern STATUSWIN_CONTROL* AddStatusWinControl
			( PLAYER_STATUS* targetStatus, GFL_BMPWIN* targetBmpwin, HEAPID heapID );
// �X�e�[�^�X�E�C���h�E�R���g���[�����C��
extern void MainStatusWinControl( STATUSWIN_CONTROL* swc );
// �X�e�[�^�X�E�C���h�E�R���g���[���I��
extern void RemoveStatusWinControl( STATUSWIN_CONTROL* swc );

// �����[�h�Z�b�g
extern void SetStatusWinReload( STATUSWIN_CONTROL* swc );

