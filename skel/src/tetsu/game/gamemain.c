//============================================================================================
/**
 * @file	gamemain.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "textprint.h"

#include "setup.h"
#include "game_net.h"
#include "player_cont.h"
#include "camera_cont.h"
#include "text_cont.h"
#include "skill_cont.h"

void	GameBoot( HEAPID heapID );
void	GameEnd( void );
BOOL	GameMain( void );

static void _initRecvBuffer( void );
static void _sendGameKey( u16 trg, u16 cont );
#define CAMERA_MOVE_SPEED	(0x0100)
//------------------------------------------------------------------
/**
 * @brief	���f�[�^
 */
//------------------------------------------------------------------
static const PLAYER_STATUS	status1 = { {"Player1"},1000, 100, 1000, 100, 50, 50, 50, 50, 50 };
static const PLAYER_STATUS	status2 = { {"Player2"},1000, 100, 1000, 100, 50, 50, 50, 50, 50 };
static const PLAYER_STATUS	status3 = { {"Player3"},1000, 100, 1000, 100, 50, 50, 50, 50, 50 };
static const PLAYER_STATUS	status4 = { {"Player4"},1000, 100, 1000, 100, 50, 50, 50, 50, 50 };
static const PLAYER_STATUS	status5 = { {"Player5"},1000, 100, 1000, 100, 50, 50, 50, 50, 50 };
static const PLAYER_STATUS	status6 = { {"Player6"},1000, 100, 1000, 100, 50, 50, 50, 50, 50 };
static const PLAYER_STATUS	status7 = { {"Player7"},1000, 100, 1000, 100, 50, 50, 50, 50, 50 };
static const PLAYER_STATUS	status8 = { {"Player8"},1000, 100, 1000, 100, 50, 50, 50, 50, 50 };

static const PLAYER_STATUS*	testStatus[] = { 
	&status1, &status2, &status3, &status4, &status5, &status6, &status7, &status8,
};

//============================================================================================
//
//
//	���C���R���g���[��
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
#define COMM_STACK_MAX (5)
typedef struct {
	u16 trg;
	u16 cont;
}COMM_KEYDATA;

typedef struct {
	int						keyStackCount;
	COMM_KEYDATA			keyStack[COMM_STACK_MAX]; 
}COMM_KEY;

typedef struct {
	HEAPID					heapID;
	GAME_SYSTEM*			gs;
	CAMERA_CONTROL*			cc; 
	SKILL_CONTROL*			sc; 
	PLAYER_CONTROL*			pc[PLAYER_SETUP_NUM]; 
	STATUSWIN_CONTROL*		swc[PLAYER_SETUP_NUM]; 
	int						myID;
	int						seq;
	int						playerSide;
	int						timer;

	COMM_KEY				commKey[PLAYER_SETUP_NUM]; 

}GAME_WORK;

//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
static BOOL GameEndCheck( void );
static void ControlKey( PLAYER_CONTROL* pc, COMM_KEYDATA* key );
static void StatusWinUpdate( void );

GAME_WORK* gw;

//------------------------------------------------------------------
/**
 * @brief	���[�N�̊m�ۂƔj��
 */
//------------------------------------------------------------------
void	GameBoot( HEAPID heapID )
{
	gw = GFL_HEAP_AllocClearMemory( heapID, sizeof(GAME_WORK) );
	gw->heapID = heapID;
}

void	GameEnd( void )
{
	GFL_HEAP_FreeMemory( gw );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
static void Debug_Regenerate( void );
//------------------------------------------------------------------
BOOL	GameMain( void )
{
	BOOL return_flag = FALSE;
	int i;
	gw->timer++;

	switch( gw->seq ){

	case 0:
		//�ʐM������
		InitGameNet();
		_initRecvBuffer();

		//�V�X�e���Z�b�g�A�b�v
		gw->gs = SetupGameSystem( gw->heapID );

		//�v���[���[�L�����o�^
		for( i=0; i<PLAYER_SETUP_NUM; i++ ){ 
			gw->pc[i] = AddPlayerControl( gw->gs, G3DSCOBJ_PLAYER1+i, gw->heapID );
			SetPlayerStatus( gw->pc[i], testStatus[i] ); 
			gw->swc[i] = AddStatusWinControl(	GetPlayerStatusPointer( gw->pc[i] ),
												Get_GS_BmpWin( gw->gs, G2DBMPWIN_P1+i ), 
												gw->heapID );
		}
		{//�J�����o�^
			gw->cc = AddCameraControl( gw->gs, G3DSCOBJ_PLAYER1, gw->heapID );
		}
		{//�X�L���R���g���[���o�^
			gw->sc = AddSkillControl( gw->gs, gw->heapID );
			for( i=0; i<PLAYER_SETUP_NUM; i++ ){ 
				AddSkillCheckPlayer( gw->sc, gw->pc[i] );
			}
		}
		gw->playerSide = 0;

		gw->seq++;
		break;

	case 1:
		if( ConnectGameNet() == TRUE ){
			//�l�b�g�h�c���v���[���[�z��h�c�Ƃ��Ă݂Ȃ�
			gw->myID = G3DSCOBJ_PLAYER1 + ((int)GetNetID() -1 );

			SetCameraControlTargetID( gw->cc, &gw->myID );
			gw->seq++;
		}
		break;

	case 2:
		//MainGameNet();

		if( GameEndCheck() == TRUE ){
			gw->seq++;
		}
		{
			//�L�[�����ʐM
			int	trg = GFL_UI_KEY_GetTrg();
			int	cont = GFL_UI_KEY_GetCont();

			_sendGameKey( trg, cont );
		}
		{
			int i, j;
			COMM_KEY* commKey;

			for( i=0; i<PLAYER_SETUP_NUM; i++ ){ 
				commKey = &gw->commKey[i];
				for( j=0; j<commKey->keyStackCount; j++ ){

					ControlKey( gw->pc[i], &commKey->keyStack[j] );

					if( i == gw->myID ){
						u16 direction;
						//�J�������f
						GetPlayerControlDirection( gw->pc[i], &direction );
						SetCameraControlDirection( gw->cc, &direction );
					}
				}
			}
			_initRecvBuffer();
		}
		for( i=0; i<PLAYER_SETUP_NUM; i++ ){ 
			MainPlayerControl( gw->pc[i] );
			SetSkillControlCommand( gw->sc, gw->pc[i], GetPlayerSkillCommand( gw->pc[i] ));
			ResetPlayerSkillCommand( gw->pc[i] );
			MainStatusWinControl( gw->swc[i] );
		}
		MainCameraControl( gw->cc );
		MainSkillControl( gw->sc );
		MainGameSystemPref( gw->gs );
		MainGameSystemAfter( gw->gs );
		StatusWinUpdate();
		break;

	case 3:
		if( ExitGameNet() == TRUE ){
			gw->seq++;
		}
		break;

	case 4:
		RemoveSkillControl( gw->sc );
		RemoveCameraControl( gw->cc );
		for( i=0; i<PLAYER_SETUP_NUM; i++ ){ 
			RemoveStatusWinControl( gw->swc[i] );
			RemovePlayerControl( gw->pc[i] );
		}
		RemoveGameSystem( gw->gs );
		return_flag = TRUE;
		break;
	}
	return return_flag;
}

//------------------------------------------------------------------
/**
 * @brief	�I���`�F�b�N
 */
//------------------------------------------------------------------
static BOOL GameEndCheck( void )
{
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
		return TRUE;
	} else {
		return FALSE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�R���g���[��
 */
//------------------------------------------------------------------
static void ControlKey( PLAYER_CONTROL* pc, COMM_KEYDATA* key )
{
	int	trg = key->trg;
	int	cont = key->cont;
	int	speedupFlag = FALSE;

	{
		//�����ݒ�
		u16 direction;
		
		if(( cont & PAD_BUTTON_L )&&( cont & PAD_BUTTON_R )){	//���Z�b�g�i�v���[���[�O���j
			//GetPlayerDirection( pc, &direction );
			//SetPlayerControlDirection( pc, &direction );
		} else if( cont & PAD_BUTTON_L ){	//���ړ�
			GetPlayerControlDirection( pc, &direction );
			direction += CAMERA_MOVE_SPEED;
			SetPlayerControlDirection( pc, &direction );
		} else if( cont & PAD_BUTTON_R ){	//�E�ړ�
			GetPlayerControlDirection( pc, &direction );
			direction -= CAMERA_MOVE_SPEED;
			SetPlayerControlDirection( pc, &direction );
		}
	}
	//����̕ύX
	if( trg & PAD_BUTTON_Y ){
		SetPlayerControlCommand( pc, PCC_WEPONCHANGE );
		return;
	}
	//�U��
	if( trg & PAD_BUTTON_A ){
		SetPlayerControlCommand( pc, PCC_ATTACK );
		return;
	}
	//�u���A�E��
	if( trg & PAD_BUTTON_B ){
		SetPlayerControlCommand( pc, PCC_PUTON );
		return;
	}
	//����
	if( cont & PAD_BUTTON_X ){
		SetPlayerControlCommand( pc, PCC_SIT );
		return;
	}
	//���x�`�F�b�N
//	if( cont & PAD_BUTTON_B ){
		speedupFlag = TRUE;
//	}
	//�ړ�
	if( cont & PAD_KEY_UP ){
		if( cont & PAD_KEY_LEFT ){
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( pc, PCC_RUN_FRONT_LEFT );
			} else {
				SetPlayerControlCommand( pc, PCC_WALK_FRONT );
			}
		} else if( cont & PAD_KEY_RIGHT ){
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( pc, PCC_RUN_FRONT_RIGHT );
			} else {
				SetPlayerControlCommand( pc, PCC_WALK_FRONT_RIGHT );
			}
		} else {
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( pc, PCC_RUN_FRONT );
			} else {
				SetPlayerControlCommand( pc, PCC_WALK_FRONT );
			}
		}
		return;
	}
	if( cont & PAD_KEY_DOWN ){
		if( cont & PAD_KEY_LEFT ){
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( pc, PCC_RUN_BACK_LEFT );
			} else {
				SetPlayerControlCommand( pc, PCC_WALK_BACK_LEFT );
			}
		} else if( cont & PAD_KEY_RIGHT ){
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( pc, PCC_RUN_BACK_RIGHT );
			} else {
				SetPlayerControlCommand( pc, PCC_WALK_BACK_RIGHT );
			}
		} else {
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( pc, PCC_RUN_BACK );
			} else {
				SetPlayerControlCommand( pc, PCC_WALK_BACK );
			}
		}
		return;
	}
	if( cont & PAD_KEY_LEFT ){
		if( speedupFlag == TRUE ){
			SetPlayerControlCommand( pc, PCC_RUN_LEFT );
		} else {
			SetPlayerControlCommand( pc, PCC_WALK_LEFT );
		}
		return;
	}
	if( cont & PAD_KEY_RIGHT ){
		if( speedupFlag == TRUE ){
			SetPlayerControlCommand( pc, PCC_RUN_RIGHT );
		} else {
			SetPlayerControlCommand( pc, PCC_WALK_RIGHT );
		}
		return;
	}
	SetPlayerControlCommand( pc, PCC_STAY );
}

//------------------------------------------------------------------
/**
 * @brief	�X�e�[�^�X�E�C���h�E�̃A�b�v�f�[�g
 *			���������^�C�~���O�₻�̑��̏����̏d���i���������j������邽�߂̊Ǘ�
 */
//------------------------------------------------------------------
static void StatusWinUpdate( void )
{
	int win = gw->timer % PLAYER_SETUP_NUM;

	SetStatusWinReload( gw->swc[win] );
}



//============================================================================================
//
//
//	�ʐM�֐�
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	��M���[�N������
 */
//------------------------------------------------------------------
static void _initRecvBuffer( void )
{
	int	i,j;

	for( i=0; i<PLAYER_SETUP_NUM;  i++ ){
		for( j=0; j<COMM_STACK_MAX; j++ ){
			gw->commKey[i].keyStackCount = 0;
			gw->commKey[i].keyStack[j].trg = 0;
			gw->commKey[i].keyStack[j].cont = 0;
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	��M�֐�
 */
//------------------------------------------------------------------
// ���[�J���ʐM�R�}���h�̒�`
enum _gameCommand_e {
	_GAME_COM_KEY = GFL_NET_CMD_COMMAND_MAX,
};

//------------------------------------------------------------------
// �ʒu����M
typedef struct {
    u16 keyTrg;
    u16 keyCont;
} COMMWORK_KEY;

static void _sendGameKey( u16 trg, u16 cont )
{
	COMMWORK_KEY commData;
	commData.keyTrg = trg;
	commData.keyCont = cont;

	SendGameNet( _GAME_COM_KEY, &commData );
}

static void _recvGameKey
	(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	COMMWORK_KEY* commDataP = (COMMWORK_KEY*)pData;
	int	workp = netID-1;	//DS�ʐM�͐e=0�i�����B���\���j��1orgin

    if( GFL_NET_IsParentHandle(pNetHandle) == FALSE ){
		int	i;
		COMM_KEY* commKey = &gw->commKey[workp];

		if( commKey->keyStackCount < COMM_STACK_MAX ){
			commKey->keyStack[commKey->keyStackCount].trg = commDataP->keyTrg;
			commKey->keyStack[commKey->keyStackCount].cont = commDataP->keyCont;
			OS_TPrintf(" netID = %d, keyTrg = %x, keyCont = %x\n", 
				        netID, commDataP->keyTrg, commDataP->keyCont );
			commKey->keyStackCount++;
		} else {
			OS_TPrintf("��M�o�b�t�@�I�[�o�[");
		}
    }
}

//------------------------------------------------------------------
// ���[�J���ʐM�e�[�u��
const NetRecvFuncTable _CommPacketTbl[] = {
    { _recvGameKey, GFL_NET_COMMAND_SIZE(sizeof(COMMWORK_KEY)), NULL },
};



//------------------------------------------------------------------
/**
 * @brief	�f�o�b�O
 */
//------------------------------------------------------------------

