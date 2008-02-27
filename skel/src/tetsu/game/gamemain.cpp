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
#include "game_cont.h"
#include "camera_cont.h"
//#include "text_cont.h"

//#define NET_WORK_ON
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
typedef struct {
	HEAPID			heapID;
	GAME_SYSTEM*	gs;
	GAME_CONTROL*	gc; 

	int				myNetID;
	int				seq;
	int				timer;
	int				trg;
	int				cont;
//-------------
	int				totalPlayerCount;
	int				playNetID;
//-------------
}GAME_WORK;

//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
static BOOL GameEndCheck( int cont );

GAME_WORK* gw;

//------------------------------------------------------------------
/**
 * @brief	���[�N�̊m�ۂƔj��
 */
//------------------------------------------------------------------
void	GameBoot( HEAPID heapID )
{
	gw = (GAME_WORK *)GFL_HEAP_AllocClearMemory( heapID, sizeof(GAME_WORK) );
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
#define MAPOFFS 0//((64-3) * 8 * FX32_ONE)
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
		gw->seq++;
		break;

	case 1:
		if( ConnectGameNet() == TRUE ){
			gw->myNetID = ((int)GetNetID() -1 );
			gw->playNetID = gw->myNetID;
			gw->seq++;
		}
		break;

	case 2:
		//�V�X�e���Z�b�g�A�b�v
		gw->gs = SetupGameSystem( gw->heapID );

		//�Q�[���R���g���[���ݒ�
		{
			int i;
			GAME_CONT_SETUP* gcSetup = (GAME_CONT_SETUP*)GFL_HEAP_AllocClearMemory
										( gw->heapID, sizeof(GAME_CONT_SETUP) );
			VecFx32			startTrans1 = { -MAPOFFS, 0, -MAPOFFS };
			VecFx32			startTrans2 = { MAPOFFS, 0, MAPOFFS };
			int				pcount = 0;

			for( i=0; i<TEAM_PLAYER_COUNT_MAX; i++ ){
				gcSetup->team[0].player[i].netID = pcount;
				gcSetup->team[0].player[i].objID = pcount;
				gcSetup->team[0].player[i].ps = (PLAYER_STATUS*)testStatus[pcount];
				pcount++;
			}
			gcSetup->team[0].playerCount = i;
			gcSetup->team[0].startTrans = startTrans1;

			for( i=0; i<TEAM_PLAYER_COUNT_MAX; i++ ){
				gcSetup->team[1].player[i].netID = pcount;
				gcSetup->team[1].player[i].objID = pcount;
				gcSetup->team[1].player[i].ps = (PLAYER_STATUS*)testStatus[pcount];
				pcount++;
			}
			gcSetup->team[1].playerCount = i;
			gcSetup->team[1].startTrans = startTrans2;

			gcSetup->teamCount = 2;
			gcSetup->myID = gw->myNetID;
			gw->gc = AddGameControl( gw->gs, gcSetup, gw->heapID );

			GFL_HEAP_FreeMemory(gcSetup); 

			ResetAllGameControlKeyCommand( gw->gc ); 

			gw->totalPlayerCount = pcount;
		}
		gw->trg = 0;
		gw->cont = 0;

		gw->seq++;
		break;

	case 3:
		//MainGameNet();
		gw->trg |= GFL_UI_KEY_GetTrg();
		gw->cont |= GFL_UI_KEY_GetCont();

		if( !(gw->timer&1) ){
			//---------------
			{
				if( gw->trg & PAD_BUTTON_SELECT ){
					gw->playNetID++;
					gw->playNetID %= gw->totalPlayerCount;
					ChangeControlPlayer( gw->gc, gw->playNetID );
				}
			}
			//---------------
			if( GameEndCheck( gw->cont ) == TRUE ){
				gw->seq++;
			}
			{
				//�L�[�����ʐM
				_sendGameKey( gw->trg, gw->cont );
			}
			MainGameControl( gw->gc );
			ResetAllGameControlKeyCommand( gw->gc ); 
			MainGameSystemPref( gw->gs );
			gw->trg = 0;
			gw->cont = 0;
		} else {
			MainGameSystemAfter( gw->gs );
		}
		break;

	case 4:
		RemoveGameControl( gw->gc );
		RemoveGameSystem( gw->gs );
		gw->seq++;
		break;

	case 5:
		if( ExitGameNet() == TRUE ){
			gw->seq++;
		}
		break;

	case 6:
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
static BOOL GameEndCheck( int cont )
{
	int resetCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START;

	if( (cont & resetCont ) == resetCont ){
		return TRUE;
	} else {
		return FALSE;
	}
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
#ifdef NET_WORK_ON
	COMMWORK_KEY commData;
	commData.keyTrg = trg;
	commData.keyCont = cont;

	SendGameNet( _GAME_COM_KEY, &commData );
#else
	//SetGameControlKeyCommand( gw->gc, gw->myNetID, trg, cont );
	SetGameControlKeyCommand( gw->gc, gw->playNetID, trg, cont );
#endif
}

static void _recvGameKey
	(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	COMMWORK_KEY* commDataP = (COMMWORK_KEY*)pData;
	int	workp = netID-1;	//DS�ʐM�͐e=0�i�����B���\���j��1orgin

    if( GFL_NET_IsParentHandle(pNetHandle) == FALSE ){
		SetGameControlKeyCommand( gw->gc, workp, commDataP->keyTrg, commDataP->keyCont );
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

