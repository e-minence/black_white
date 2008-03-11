//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
#include "team_cont.h"

#define TEAM_COUNT_MAX	(2) 

typedef struct {
	int				netID;
	int				objID;
	PLAYER_STATUS*	ps;
}GAME_CONT_SETUP_PLAYER;

typedef struct {
	GAME_CONT_SETUP_PLAYER	player[TEAM_PLAYER_COUNT_MAX];
	int						playerCount;
	VecFx32					startTrans;
}GAME_CONT_SETUP_TEAM;

typedef struct {
	GAME_CONT_SETUP_TEAM	team[TEAM_COUNT_MAX];
	int						teamCount;
	int						myID;
}GAME_CONT_SETUP;

typedef struct {
	PLAYER_STATUS_NETWORK	psn;
}GAME_NETWORK_PLAYDATA;

typedef struct {
	u32		keyTrg;
	u32		keyCont;
	BOOL	tpTrg;
	BOOL	tpCont;
	u32		tpx;
	u32		tpy;
}GAME_CONT_KEYDATA;

typedef struct _GAME_CONTROL	GAME_CONTROL;

// �Q�[���R���g���[���Z�b�g
extern GAME_CONTROL* AddGameControl( GAME_SYSTEM* gs, GAME_CONT_SETUP* setup, HEAPID heapID );
// �Q�[���R���g���[�����C��
extern void MainGameControl( GAME_CONTROL* gc );
//	�Q�[�����C���R���g���[����̃t���O���Z�b�g����
extern void ResetGameControl( GAME_CONTROL* gc );
// �Q�[���R���g���[���I��
extern void RemoveGameControl( GAME_CONTROL* gc );

//	�Q�[���L�[�f�[�^�ݒ�
extern void SetGameControlKey( GAME_CONTROL* gc, GAME_CONT_KEYDATA* data );
extern void ResetGameControlKey( GAME_CONTROL* gc );

// �ʐM�R�}���h�Z�b�g
//�Q�[���v���C���f�[�^
extern void GetGameNetWorkPlayData( GAME_CONTROL* gc, int netID, GAME_NETWORK_PLAYDATA* gnd ); 
extern void SetGameNetWorkPlayData( GAME_CONTROL* gc, int netID, GAME_NETWORK_PLAYDATA* gnd ); 
extern void ResetAllGameNetWorkPlayData( GAME_CONTROL* gc ); 

// �`�[���R���g���[���擾
extern TEAM_CONTROL*	GetTeamControl( GAME_CONTROL* gc, int teamID ); 
// �`�[�����擾
extern int				GetTeamCount( GAME_CONTROL* gc );


// �v���[���[�؂�ւ��i�f�o�b�O�p�j
extern void	ChangeControlPlayer( GAME_CONTROL* gc, int playNetID );


