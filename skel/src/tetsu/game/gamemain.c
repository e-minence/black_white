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
#include "player_cont.h"
#include "camera_cont.h"

void	GameBoot( HEAPID heapID );
void	GameEnd( void );
BOOL	GameMain( void );

#define CAMERA_MOVE_SPEED	(0x0100)
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
	HEAPID					heapID;
	GAME_SYSTEM*			gs;
	PLAYER_CONTROL*			pc; 
	CAMERA_CONTROL*			cc; 
	int						seq;

}GAME_WORK;

//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
static BOOL GameEndCheck( void );
static void ControlKey( void );

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
//------------------------------------------------------------------
BOOL	GameMain( void )
{
	BOOL return_flag = FALSE;
	int i;

	switch( gw->seq ){

	case 0:
		gw->gs = SetupGameSystem( gw->heapID );
		gw->pc = AddPlayerControl( gw->gs, G3DSCOBJ_PLAYER, gw->heapID );
		gw->cc = AddCameraControl( gw->gs, G3DSCOBJ_PLAYER, gw->heapID );
		gw->seq++;
		break;

	case 1:
		if( GameEndCheck() == TRUE ){
			gw->seq++;
		}
		ControlKey();
		MainGameSystemPref( gw->gs );
		MainPlayerControl( gw->pc );
		MainCameraControl( gw->cc );
		MainGameSystemAfter( gw->gs );
		break;

	case 2:
		RemoveCameraControl( gw->cc );
		RemovePlayerControl( gw->pc );
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
static void ControlKey( void )
{
	int	trg = GFL_UI_KEY_GetTrg();
	int	cont = GFL_UI_KEY_GetCont();
	int	speedupFlag = FALSE;

	//����̕ύX
	if( trg & PAD_BUTTON_Y ){
		SetPlayerControlCommand( gw->pc, PCC_WEPONCHANGE );
		return;
	}
	//�U��
	if( trg & PAD_BUTTON_A ){
		SetPlayerControlCommand( gw->pc, PCC_ATTACK );
		return;
	}
	//����
	if( cont & PAD_BUTTON_X ){
		SetPlayerControlCommand( gw->pc, PCC_SIT );
		return;
	}
	{
		//�J��������
		u16 direction;
		
		if(( cont & PAD_BUTTON_L )&&( cont & PAD_BUTTON_R )){	//���Z�b�g�i�v���[���[�O���j
			//GetPlayerDirection( gw->pc, &direction );
			//SetPlayerControlDirection( gw->pc, &direction );
			//SetCameraControlDirection( gw->cc, &direction );
		} else if( cont & PAD_BUTTON_L ){	//���ړ�
			GetPlayerControlDirection( gw->pc, &direction );
			direction += CAMERA_MOVE_SPEED;
			SetPlayerControlDirection( gw->pc, &direction );
			SetCameraControlDirection( gw->cc, &direction );
		} else if( cont & PAD_BUTTON_R ){	//�E�ړ�
			GetPlayerControlDirection( gw->pc, &direction );
			direction -= CAMERA_MOVE_SPEED;
			SetPlayerControlDirection( gw->pc, &direction );
			SetCameraControlDirection( gw->cc, &direction );
		}
	}
	//���x�`�F�b�N
	if( cont & PAD_BUTTON_B ){
		speedupFlag = TRUE;
	}
	//�ړ�
	if( cont & PAD_KEY_UP ){
		if( cont & PAD_KEY_LEFT ){
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( gw->pc, PCC_RUN_FRONT_LEFT );
			} else {
				SetPlayerControlCommand( gw->pc, PCC_WALK_FRONT );
			}
		} else if( cont & PAD_KEY_RIGHT ){
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( gw->pc, PCC_RUN_FRONT_RIGHT );
			} else {
				SetPlayerControlCommand( gw->pc, PCC_WALK_FRONT_RIGHT );
			}
		} else {
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( gw->pc, PCC_RUN_FRONT );
			} else {
				SetPlayerControlCommand( gw->pc, PCC_WALK_FRONT );
			}
		}
		return;
	}
	if( cont & PAD_KEY_DOWN ){
		if( cont & PAD_KEY_LEFT ){
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( gw->pc, PCC_RUN_BACK_LEFT );
			} else {
				SetPlayerControlCommand( gw->pc, PCC_WALK_BACK_LEFT );
			}
		} else if( cont & PAD_KEY_RIGHT ){
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( gw->pc, PCC_RUN_BACK_RIGHT );
			} else {
				SetPlayerControlCommand( gw->pc, PCC_WALK_BACK_RIGHT );
			}
		} else {
			if( speedupFlag == TRUE ){
				SetPlayerControlCommand( gw->pc, PCC_RUN_BACK );
			} else {
				SetPlayerControlCommand( gw->pc, PCC_WALK_BACK );
			}
		}
		return;
	}
	if( cont & PAD_KEY_LEFT ){
		if( speedupFlag == TRUE ){
			SetPlayerControlCommand( gw->pc, PCC_RUN_LEFT );
		} else {
			SetPlayerControlCommand( gw->pc, PCC_WALK_LEFT );
		}
		return;
	}
	if( cont & PAD_KEY_RIGHT ){
		if( speedupFlag == TRUE ){
			SetPlayerControlCommand( gw->pc, PCC_RUN_RIGHT );
		} else {
			SetPlayerControlCommand( gw->pc, PCC_WALK_RIGHT );
		}
		return;
	}
	SetPlayerControlCommand( gw->pc, PCC_STAY );
}

