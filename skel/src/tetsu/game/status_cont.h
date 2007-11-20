//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�X�e�[�^�X�o�[�R���g���[��
 */
//------------------------------------------------------------------
typedef struct _STATUS_CONTROL	STATUS_CONTROL;

typedef struct {
	HEAPID				heapID;
	GAME_SYSTEM*		gs;
	TEAM_CONTROL**		p_tc;
	int					teamCount;
	CAMERA_CONTROL*		cc;
	PLAYER_CONTROL*		myPc;
}STATUS_SETUP;

// �X�e�[�^�X�R���g���[���Z�b�g
extern STATUS_CONTROL* AddStatusControl( STATUS_SETUP* setup );
// �X�e�[�^�X�R���g���[�����C��
extern void MainStatusControl( STATUS_CONTROL* stc );
// �X�e�[�^�X�R���g���[���I��
extern void RemoveStatusControl( STATUS_CONTROL* stc );


extern void ChangeStatusControl( STATUS_CONTROL* stc , PLAYER_CONTROL* pc );


