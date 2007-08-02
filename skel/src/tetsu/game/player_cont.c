//============================================================================================
/**
 * @file	player_cont.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "setup.h"
#include "player_cont.h"

//============================================================================================
//
//
//	�v���[���[�R���g���[��
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
typedef struct {
	u8		count;
	u8		timer;
	u8		timerTmp;
	s8		value;
}HP_INCDEC_ONTIME;

struct _PLAYER_CONTROL {
	HEAPID					heapID;
	GAME_SYSTEM*			gs;
	int						targetAct;

	PLAYER_CONTROL_COMMAND	contCommand;
	u16						skillCommand;
	s16						damage;
	HP_INCDEC_ONTIME		dotStatus;
	HP_INCDEC_ONTIME		regenerateStatus;
	BOOL					sitDownFlag;
	BOOL					skillBusyFlag;
	BOOL					hitEnableFlag;

	u16						contDirection;
	u16						nowDirection;
	VecFx32					contTrans;
	u16						nowAccesary;

	PLAYER_STATUS			status;
};

#define WALK_SPEED	(1)		//����1/2
#define RUN_SPEED	(4)		//����2
#define HITOFS		(16)
static const PLAYER_STATUS	statusDefault = { {"Null"},1000, 100, 1000, 100, 50, 50, 50, 50, 50 };

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�R���g���[���N��
 */
//------------------------------------------------------------------
PLAYER_CONTROL* AddPlayerControl( GAME_SYSTEM* gs, int targetAct, HEAPID heapID )
{
	PLAYER_CONTROL* pc = GFL_HEAP_AllocClearMemory( heapID, sizeof(PLAYER_CONTROL) );
	pc->heapID = heapID;
	pc->gs = gs;
	pc->targetAct = targetAct;

	pc->contCommand = PCC_NOP;
	pc->skillCommand = 0;
	pc->damage = 0;
	pc->dotStatus.count = 0;
	pc->regenerateStatus.count = 0;

	pc->contDirection = 0;
	pc->nowDirection = 0;
	Get3DactTrans( Get_GS_SceneAct( pc->gs ), pc->targetAct, &pc->contTrans );
	pc->nowAccesary = 0;
	pc->sitDownFlag = FALSE;
	pc->skillBusyFlag = FALSE;
	pc->hitEnableFlag = FALSE;

	pc->status = statusDefault;
	return pc;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�R���g���[���I��
 */
//------------------------------------------------------------------
void RemovePlayerControl( PLAYER_CONTROL* pc )
{
	GFL_HEAP_FreeMemory( pc ); 
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�����̎擾
 */
//------------------------------------------------------------------
void GetPlayerDirection( PLAYER_CONTROL* pc, u16* direction )
{
	*direction = pc->nowDirection;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�R���g���[�����W�̎擾�Ɛݒ�
 */
//------------------------------------------------------------------
void GetPlayerControlTrans( PLAYER_CONTROL* pc, VecFx32* trans )
{
	*trans = pc->contTrans;
}

void SetPlayerControlTrans( PLAYER_CONTROL* pc, const VecFx32* trans )
{
	pc->contTrans = *trans;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�R���g���[�������̎擾�Ɛݒ�
 */
//------------------------------------------------------------------
void GetPlayerControlDirection( PLAYER_CONTROL* pc, u16* direction )
{
	*direction = pc->contDirection;
}

void SetPlayerControlDirection( PLAYER_CONTROL* pc, const u16* direction )
{
	pc->contDirection = *direction;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�X�e�[�^�X�|�C���^�̎擾
 */
//------------------------------------------------------------------
PLAYER_STATUS* GetPlayerStatusPointer( PLAYER_CONTROL* pc )
{
	return &pc->status;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�X�e�[�^�X�̎擾�Ɛݒ�
 */
//------------------------------------------------------------------
void GetPlayerStatus( PLAYER_CONTROL* pc, PLAYER_STATUS* status )
{
	*status = pc->status;
}

void SetPlayerStatus( PLAYER_CONTROL* pc, const PLAYER_STATUS* status )
{
	pc->status = *status;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�R�}���h�̐ݒ�
 */
//------------------------------------------------------------------
void SetPlayerControlCommand( PLAYER_CONTROL* pc, const PLAYER_CONTROL_COMMAND command )
{
	pc->contCommand = command;
}

//------------------------------------------------------------------
/**
 * @brief	�X�L���R�}���h�̎擾�ƃ��Z�b�g
 */
//------------------------------------------------------------------
int GetPlayerSkillCommand( PLAYER_CONTROL* pc )
{
	return pc->skillCommand;
}

void ResetPlayerSkillCommand( PLAYER_CONTROL* pc )
{
	pc->skillCommand = 0;
}

//------------------------------------------------------------------
/**
 * @brief	�X�L�����s���t���O�̐ݒ�ƃ��Z�b�g
 */
//------------------------------------------------------------------
void SetPlayerSkillBusyFlag( PLAYER_CONTROL* pc )
{
	pc->skillBusyFlag = TRUE;
}

void ResetPlayerSkillBusyFlag( PLAYER_CONTROL* pc )
{
	pc->skillBusyFlag = FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�q�b�g����\�t���O�̎擾
 */
//------------------------------------------------------------------
BOOL GetPlayerHitEnableFlag( PLAYER_CONTROL* pc )
{
	return pc->hitEnableFlag;
}

//------------------------------------------------------------------
/**
 * @brief	�g�o�����l�̐ݒ�
 */
//------------------------------------------------------------------
void SetPlayerDamage( PLAYER_CONTROL* pc, const s16 damage )
{
	pc->damage = damage;
}

//------------------------------------------------------------------
/**
 * @brief	�c�n�s�i���Ԍo�߂ɂ��g�o�����l�j�̐ݒ�
 */
//------------------------------------------------------------------
void SetPlayerDamageOnTime( PLAYER_CONTROL* pc, const u8 count, const u8 timer, const s8 value )
{
	if(( value < 0 )&&( !pc->dotStatus.count )){
		pc->dotStatus.count = count;
		pc->dotStatus.timer = timer;
		pc->dotStatus.timerTmp = timer;
		pc->dotStatus.value = value;
	}
}

//------------------------------------------------------------------
/**
 * @brief	���W�F�l���[�g�l�̐ݒ�
 */
//------------------------------------------------------------------
void SetPlayerRegenerate( PLAYER_CONTROL* pc, const u8 count, const u8 timer, const s8 value )
{
	if( value > 0 ){
		pc->regenerateStatus.count = count;
		pc->regenerateStatus.timer = timer;
		pc->regenerateStatus.timerTmp = timer;
		pc->regenerateStatus.value = value;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[���C���R���g���[��
 */
//------------------------------------------------------------------
static void moveSet( PLAYER_CONTROL* pc, u16 direction, int speed, int movecmd )
{
	VecFx32	rotVec, tmpTransVec;		
	u16		mapAttr;
	BOOL	areaOver;

	//�ȈՃq�b�g�`�F�b�N(�����𒆐S�Ƃ����~�Փ˔���)
	pc->nowDirection = direction;
	tmpTransVec.x = pc->contTrans.x + ( -( speed + HITOFS ) * FX_SinIdx( direction ) /2 );
	tmpTransVec.y = pc->contTrans.y;
	tmpTransVec.z = pc->contTrans.z + ( -( speed + HITOFS ) * FX_CosIdx( direction ) /2 );

	//�ȈՃA�g���r���[�g�擾
	areaOver = Get3DmapAttr( Get_GS_SceneMap( pc->gs ), &tmpTransVec, &mapAttr );

	if(( areaOver == TRUE )&&( mapAttr == 0 )){
		pc->contTrans.x += ( -speed * FX_SinIdx( direction ) /2 );
		pc->contTrans.z += ( -speed * FX_CosIdx( direction ) /2 );
		Set3DactPlayerChrAnimeCmd( Get_GS_SceneAct( pc->gs ), pc->targetAct, movecmd );
	} else {
		Set3DactPlayerChrAnimeCmd( Get_GS_SceneAct( pc->gs ), pc->targetAct, ACTANM_CMD_STAY );
	}
	rotVec.x = 0;
	rotVec.y = direction + 0x8000;
	rotVec.z = 0;

	Set3DactTrans( Get_GS_SceneAct( pc->gs ), pc->targetAct, &pc->contTrans );
	Set3DactRotate( Get_GS_SceneAct( pc->gs ), pc->targetAct, &rotVec );
}

static void directionSet( PLAYER_CONTROL* pc, u16 direction )
{
	VecFx32	rotVec;		

	pc->nowDirection = direction;
	rotVec.x = 0;
	rotVec.y = direction + 0x8000;
	rotVec.z = 0;

	Set3DactRotate( Get_GS_SceneAct( pc->gs ), pc->targetAct, &rotVec );
}

//------------------------------------------------------------------
void MainPlayerControl( PLAYER_CONTROL* pc )
{
	VecFx32 move = { 0, 0, 0 };
	BOOL	moveFlag = FALSE;

	//�s���s�\����
	if( pc->status.hp <= 0 ){
		return;	//�s���s�\
	}

	//�g�o��������
	if( pc->dotStatus.count ){	//�c�n�s����
		if( pc->dotStatus.timerTmp ){
			pc->dotStatus.timerTmp--;
		} else {
			pc->dotStatus.timerTmp = pc->dotStatus.timer;
			pc->dotStatus.count--;
			pc->status.hp += pc->dotStatus.value;
			if( pc->status.hp < 0 ){
				pc->status.hp = 1;
			}
		}
	}
	if( pc->regenerateStatus.count ){	//���W�F�l���[�g����
		if( pc->regenerateStatus.timerTmp ){
			pc->regenerateStatus.timerTmp--;
		} else {
			pc->regenerateStatus.timerTmp = pc->regenerateStatus.timer;
			pc->regenerateStatus.count--;
			pc->status.hp += pc->regenerateStatus.value;
			if( pc->status.hp > pc->status.hpMax ){
				pc->status.hp = pc->status.hpMax;
			}
		}
	}
	if( pc->damage ){	//���ڃ_���[�W
		//�_���[�W���炢����
		Set3DactPlayerChrAnimeForceCmd( Get_GS_SceneAct( pc->gs ), pc->targetAct, ACTANM_CMD_HIT );
		pc->status.hp += pc->damage;
		if( pc->status.hp < 0 ){
			pc->status.hp = 0;
		} else if( pc->status.hp > pc->status.hpMax ) {
			pc->status.hp = pc->status.hpMax;
		}
		pc->damage = 0;
		pc->hitEnableFlag = FALSE;	//���ꃂ�[�V�������̓q�b�g��������Ȃ����Ƃ��ł���
		return;
	}

	//�s������
	if( Check3DactPlayerBusy( Get_GS_SceneAct( pc->gs ), pc->targetAct ) == TRUE ){
		return;	//�����A�j����
	}
	pc->hitEnableFlag = TRUE;

	if( pc->skillBusyFlag ){
		return;	//�X�L�����s��
	}
	if(( pc->contCommand != PCC_SIT )&&( pc->sitDownFlag == TRUE )){	//���Ⴊ�ݒ��������菈��
		Set3DactPlayerChrAnimeCmd( Get_GS_SceneAct( pc->gs ), pc->targetAct, ACTANM_CMD_STANDUP );
		pc->sitDownFlag = FALSE;
		return;
	}
	switch( pc->contCommand ){
	case PCC_NOP:
		break;
	case PCC_STAY:
		Set3DactPlayerChrAnimeCmd( Get_GS_SceneAct( pc->gs ), pc->targetAct, ACTANM_CMD_STAY );
		break;
	case PCC_WALK_FRONT:
		moveSet( pc, pc->contDirection + 0x0000, WALK_SPEED, ACTANM_CMD_WALK );
		break;
	case PCC_WALK_BACK:
		moveSet( pc, pc->contDirection + 0x8000, WALK_SPEED, ACTANM_CMD_WALK );
		break;
	case PCC_WALK_LEFT:
		moveSet( pc, pc->contDirection + 0x4000, WALK_SPEED, ACTANM_CMD_WALK );
		break;
	case PCC_WALK_RIGHT:
		moveSet( pc, pc->contDirection - 0x4000, WALK_SPEED, ACTANM_CMD_WALK );
		break;
	case PCC_WALK_FRONT_LEFT:
		moveSet( pc, pc->contDirection + 0x2000, WALK_SPEED, ACTANM_CMD_WALK );
		break;
	case PCC_WALK_FRONT_RIGHT:
		moveSet( pc, pc->contDirection - 0x2000, WALK_SPEED, ACTANM_CMD_WALK );
		break;
	case PCC_WALK_BACK_LEFT:
		moveSet( pc, pc->contDirection + 0x6000, WALK_SPEED, ACTANM_CMD_WALK );
		break;
	case PCC_WALK_BACK_RIGHT:
		moveSet( pc, pc->contDirection - 0x6000, WALK_SPEED, ACTANM_CMD_WALK );
		break;
	case PCC_RUN_FRONT:
		moveSet( pc, pc->contDirection + 0x0000, RUN_SPEED, ACTANM_CMD_RUN );
		break;
	case PCC_RUN_BACK:
		moveSet( pc, pc->contDirection + 0x8000, RUN_SPEED, ACTANM_CMD_RUN );
		break;
	case PCC_RUN_LEFT:
		moveSet( pc, pc->contDirection + 0x4000, RUN_SPEED, ACTANM_CMD_RUN );
		break;
	case PCC_RUN_RIGHT:
		moveSet( pc, pc->contDirection - 0x4000, RUN_SPEED, ACTANM_CMD_RUN );
		break;
	case PCC_RUN_FRONT_LEFT:
		moveSet( pc, pc->contDirection + 0x2000, RUN_SPEED, ACTANM_CMD_RUN );
		break;
	case PCC_RUN_FRONT_RIGHT:
		moveSet( pc, pc->contDirection - 0x2000, RUN_SPEED, ACTANM_CMD_RUN );
		break;
	case PCC_RUN_BACK_LEFT:
		moveSet( pc, pc->contDirection + 0x6000, RUN_SPEED, ACTANM_CMD_RUN );
		break;
	case PCC_RUN_BACK_RIGHT:
		moveSet( pc, pc->contDirection - 0x6000, RUN_SPEED, ACTANM_CMD_RUN );
		break;
	case PCC_ATTACK:
		//����ɂ���ă��[�V�����ω�
		switch( pc->nowAccesary ){
		defasult:
			Set3DactPlayerChrAnimeCmd( Get_GS_SceneAct(pc->gs), pc->targetAct, ACTANM_CMD_STAY );
			break;
		case 1:
			directionSet( pc, pc->contDirection );	//�J���������Ɍ�������
			Set3DactPlayerChrAnimeCmd( Get_GS_SceneAct(pc->gs), pc->targetAct, ACTANM_CMD_ATTACK );
			pc->skillCommand = 1;
			break;
		case 2:
			directionSet( pc, pc->contDirection );	//�J���������Ɍ�������
			Set3DactPlayerChrAnimeCmd( Get_GS_SceneAct(pc->gs), pc->targetAct, ACTANM_CMD_SHOOT );
			pc->skillCommand = 2;
			break;
		case 3:
			directionSet( pc, pc->contDirection );	//�J���������Ɍ�������
			Set3DactPlayerChrAnimeCmd( Get_GS_SceneAct(pc->gs), pc->targetAct, ACTANM_CMD_SPELL );
			pc->skillCommand = 3;
			break;
		}
		break;
	case PCC_SIT:
		Set3DactPlayerChrAnimeCmd
			( Get_GS_SceneAct( pc->gs ), pc->targetAct, ACTANM_CMD_SITDOWN );
		pc->sitDownFlag = TRUE;
		break;
	case PCC_WEPONCHANGE:		
		//�i���o�[�؂�ւ�
		pc->nowAccesary++;
		pc->nowAccesary&= 3;
		Set3DactPlayerChrAccesory( Get_GS_SceneAct( pc->gs ), pc->targetAct, pc->nowAccesary );
		break;
	case PCC_PUTON:		
		Set3DactPlayerChrAnimeCmd( Get_GS_SceneAct(pc->gs), pc->targetAct, ACTANM_CMD_TAKE );
		AddObject3Dmap( Get_GS_SceneMap( pc->gs ), EXMAPOBJ_WALL, &pc->contTrans );
		break;
	}
	//pc->contCommand = PCC_NOP;
}

