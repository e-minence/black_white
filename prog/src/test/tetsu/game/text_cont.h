//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�X�e�[�^�X�E�C���h�E
 */
//------------------------------------------------------------------
typedef struct _STATUSWIN_CONTROL	STATUSWIN_CONTROL;

typedef struct {
	HEAPID				heapID;
	TEAM_CONTROL**		p_tc;
	int					teamCount;
	GFL_BMPWIN*			targetBmpwin;
}STATUSWIN_SETUP;

// �X�e�[�^�X�E�C���h�E�R���g���[���Z�b�g
extern STATUSWIN_CONTROL* AddStatusWinControl( STATUSWIN_SETUP* setup );
// �X�e�[�^�X�E�C���h�E�R���g���[�����C��
extern void MainStatusWinControl( STATUSWIN_CONTROL* swc );
// �X�e�[�^�X�E�C���h�E�R���g���[���I��
extern void RemoveStatusWinControl( STATUSWIN_CONTROL* swc );

// �����[�h�Z�b�g
extern void SetStatusWinReload( STATUSWIN_CONTROL* swc );

//------------------------------------------------------------------
/**
 * @brief	���b�Z�[�W�E�C���h�E
 */
//------------------------------------------------------------------
typedef struct _MSGWIN_CONTROL	MSGWIN_CONTROL;

typedef struct {
	HEAPID				heapID;
	GFL_BMPWIN*			targetBmpwin;
}MSGWIN_SETUP;

// ���b�Z�[�W�E�C���h�E�R���g���[���Z�b�g
extern MSGWIN_CONTROL* AddMessageWinControl( MSGWIN_SETUP* setup );
// ���b�Z�[�W�E�C���h�E�R���g���[�����C��
extern void MainMessageWinControl( MSGWIN_CONTROL* mwc );
// ���b�Z�[�W�E�C���h�E�R���g���[���I��
extern void RemoveMessageWinControl( MSGWIN_CONTROL* mwc );

typedef enum {
	GMSG_NULL = 0,
	GMSG_GAME_INFO,
}MSGID;

// ���b�Z�[�W�Z�b�g
extern void PutMessageWin( MSGWIN_CONTROL* mwc, MSGID msgID );

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�E�C���h�E
 */
//------------------------------------------------------------------
typedef struct _MAPWIN_CONTROL	MAPWIN_CONTROL;

typedef struct {
	HEAPID				heapID;
	GFL_BMPWIN*			targetBmpwin;
	GAME_SYSTEM*		gs;
	TEAM_CONTROL**		p_tc;
	int					teamCount;
	PLAYER_CONTROL*		myPc;
	TEAM_CONTROL*		myTc;
}MAPWIN_SETUP;

// �}�b�v�E�C���h�E�R���g���[���Z�b�g
extern MAPWIN_CONTROL* AddMapWinControl( MAPWIN_SETUP* setup );
// �}�b�v�E�C���h�E�R���g���[�����C��
extern void MainMapWinControl( MAPWIN_CONTROL* mpwc );
// �}�b�v�E�C���h�E�R���g���[���I��
extern void RemoveMapWinControl( MAPWIN_CONTROL* mpwc );

// �f�o�b�O�p
extern void ChangeMapWinControl( MAPWIN_CONTROL* mpwc, PLAYER_CONTROL* pc, TEAM_CONTROL* tc );

