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
	GAME_SYSTEM*			gs;
	int						targetAct;
	int						netID;

	u16						subSeq;
	u16						nowCommand;

	PLAYER_MOVE_DIR			moveDir;
	PLAYER_CONTROL_COMMAND	contCommand;
	PLAYER_SKILL_COMMAND	skillCommand;
	PLAYER_BUILD_COMMAND	buildCommand;
	s16						damage;
	HP_INCDEC_ONTIME		dotStatus;
	HP_INCDEC_ONTIME		regenerateStatus;

	BOOL					sitDownFlag;
	BOOL					jumpFlag;
	BOOL					skillBusyFlag;
	BOOL					hitEnableFlag;
	BOOL					deadFlag;

	u16						contDirection;
	u16						nowDirection;
	VecFx32					contTrans;
	u16						nowAccesary;
	u16						recoverTimer;
	u16						jumpCalcWork;

	PLAYER_STATUS			status;
#if 0
	GFL_CLWK*				statusObj;
#endif
};

#define WALK_SPEED	(2)//(1)		//����1/2
#define RUN_SPEED	(8)//(4)		//����2
#define JUMP_SPEED	(16)//(4)		//����2
#define HITOFS		(16)
static const PLAYER_STATUS	statusDefault = { {"Null"},1000, 100, 1000, 100, 50, 50, 50, 50, 50 };
static const GFL_CLWK_DATA clwkData = { 0, 0, 0, 0, 0 }; 
//------------------------------------------------------------------
/**
 * @brief	�v���[���[�R���g���[���N��
 */
//------------------------------------------------------------------
PLAYER_CONTROL* AddPlayerControl( GAME_SYSTEM* gs, int targetAct, int netID, HEAPID heapID )
{
	PLAYER_CONTROL* pc = GFL_HEAP_AllocClearMemory( heapID, sizeof(PLAYER_CONTROL) );
	pc->gs = gs;
	pc->targetAct = targetAct;
	pc->netID = netID;

	pc->moveDir = 0;
	pc->contCommand = PCC_NOP;
	pc->skillCommand = PSC_NOP;
	pc->buildCommand = PBC_NOP;
	pc->damage = 0;
	pc->dotStatus.count = 0;
	pc->regenerateStatus.count = 0;

	pc->sitDownFlag = FALSE;
	pc->jumpFlag = FALSE;
	pc->skillBusyFlag = FALSE;
	pc->hitEnableFlag = FALSE;
	pc->deadFlag = FALSE;

	pc->contDirection = 0;
	pc->nowDirection = 0;
	Get3DactTrans( Get_GS_SceneAct( pc->gs ), pc->targetAct, &pc->contTrans );
	pc->nowAccesary = 0;
	pc->recoverTimer = 0;
	pc->jumpCalcWork = 0;

	pc->subSeq = 0;
	pc->nowCommand = 0;
	pc->status = statusDefault;

	Set3DactDrawSw( Get_GS_SceneAct( pc->gs ), pc->targetAct, TRUE );

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
 * @brief	�v���[���[�֘A�t��3DactID�̎擾
 */
//------------------------------------------------------------------
void GetPlayerAct3dID( PLAYER_CONTROL* pc, int* act3dID )
{
	*act3dID = pc->targetAct;
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
	Set3DactTrans( Get_GS_SceneAct( pc->gs ), pc->targetAct, &pc->contTrans );
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
 * @brief	�ړ��R�}���h�̐ݒ�
 */
//------------------------------------------------------------------
void SetPlayerMoveCommand
	( PLAYER_CONTROL* pc, PLAYER_CONTROL_COMMAND command, PLAYER_MOVE_DIR dir )
{
	if( pc->jumpFlag == TRUE ){	//�W�����v������s��
		return;
	}
	pc->contCommand = command;
	pc->moveDir = dir;
}

//------------------------------------------------------------------
/**
 * @brief	�X�L���R�}���h�̎擾�ƃ��Z�b�g
 */
//------------------------------------------------------------------
PLAYER_SKILL_COMMAND GetPlayerSkillCommand( PLAYER_CONTROL* pc )
{
	return pc->skillCommand;
}

void ResetPlayerSkillCommand( PLAYER_CONTROL* pc )
{
	pc->skillCommand = PSC_NOP;
}

//------------------------------------------------------------------
/**
 * @brief	���z�R�}���h�̎擾�ƃ��Z�b�g
 */
//------------------------------------------------------------------
PLAYER_BUILD_COMMAND GetPlayerBuildCommand( PLAYER_CONTROL* pc )
{
	return pc->buildCommand;
}

void ResetPlayerBuildCommand( PLAYER_CONTROL* pc )
{
	pc->buildCommand = PBC_NOP;
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
 * @brief	���S�t���O�̎擾�ƃ��Z�b�g
 */
//------------------------------------------------------------------
BOOL GetPlayerDeadFlag( PLAYER_CONTROL* pc )
{
	return pc->deadFlag;
}

void ResetPlayerDeadFlag( PLAYER_CONTROL* pc )
{
	pc->deadFlag = FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�l�b�g�h�c�̎擾
 */
//------------------------------------------------------------------
int GetPlayerNetID( PLAYER_CONTROL* pc )
{
	return pc->netID;
}

//------------------------------------------------------------------
/**
 * @brief	�g�o�����l�̐ݒ�
 */
//------------------------------------------------------------------
void SetPlayerDamage( PLAYER_CONTROL* pc, const s16 damage )
{
	if( pc->status.hp ){
		pc->damage = damage;
	}
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
#define JUMP_HEIGHT (FX32_ONE*48)

static int dirOffsTable[] = 
	{ 0x0000, 0x8000, 0x4000,-0x4000, 0x2000,-0x2000, 0x6000,-0x6000, };

//------------------------------------------------------------------
static BOOL anmSet( PLAYER_CONTROL* pc, int anmID )
{
	if( pc->jumpFlag == FALSE ){
		Set3DactPlayerChrAnimeCmd( Get_GS_SceneAct( pc->gs ), pc->targetAct, anmID );
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
static BOOL moveSet( PLAYER_CONTROL* pc, int speed, u16 direction )
{
	VecFx32	rotVec, tmpTransVec;		
	u16		mapAttr;
	BOOL	areaOver;
	BOOL	mvf;

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
		mvf = TRUE;
	} else {
		mvf = FALSE;
	}
	rotVec.x = 0;
	rotVec.y = direction + 0x8000;
	rotVec.z = 0;

	Set3DactTrans( Get_GS_SceneAct( pc->gs ), pc->targetAct, &pc->contTrans );
	Set3DactRotate( Get_GS_SceneAct( pc->gs ), pc->targetAct, &rotVec );
	return mvf;
}

//------------------------------------------------------------------
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
static BOOL damageSet( PLAYER_CONTROL* pc )
{
	if( pc->status.hp == 0 ){
		return TRUE;
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
	if( pc->damage < 0 ){	//���ڃ_���[�W
		pc->status.hp += pc->damage;
		if( pc->status.hp < 0 ) {
			pc->status.hp = 0;
		}
		pc->damage = 0;
		return TRUE;	//�_���[�W���炢����
	}
	if( pc->damage > 0 ){	//��
		pc->status.hp += pc->damage;
		if( pc->status.hp > pc->status.hpMax ) {
			pc->status.hp = pc->status.hpMax;
		}
		pc->damage = 0;
	}
	return FALSE;
}

//------------------------------------------------------------------
static BOOL jumpControl( PLAYER_CONTROL* pc )
{
	BOOL retFlag = TRUE;

	switch( pc->subSeq ){
	case 0:
		Set3DactPlayerChrAnimeCmd(Get_GS_SceneAct(pc->gs), pc->targetAct, ACTANM_CMD_JUMP_RDY);
		pc->moveDir = pc->contDirection + pc->moveDir;
		pc->jumpFlag = TRUE;
		pc->subSeq++;
		retFlag = FALSE;
		break;
	case 1:
		Set3DactPlayerChrAnimeCmd(Get_GS_SceneAct(pc->gs), pc->targetAct, ACTANM_CMD_JUMPUP);
		pc->jumpCalcWork = 0;
		pc->subSeq++;
		retFlag = FALSE;
		break;
	case 2:
		if( pc->jumpCalcWork < 0x6000 ){
			pc->contTrans.y = (JUMP_HEIGHT * FX_SinIdx( pc->jumpCalcWork )/ FX16_ONE);
			pc->jumpCalcWork += 0x0200;
		} else {
			Set3DactPlayerChrAnimeCmd
				(Get_GS_SceneAct(pc->gs), pc->targetAct, ACTANM_CMD_JUMPDOWN);
			pc->subSeq++;
		}
	case 3:
		if( pc->jumpCalcWork < 0x8000 ){
			pc->contTrans.y = (JUMP_HEIGHT * FX_SinIdx( pc->jumpCalcWork )/ FX16_ONE);
			pc->jumpCalcWork += 0x0800;
		} else {
			Set3DactPlayerChrAnimeCmd
				(Get_GS_SceneAct(pc->gs), pc->targetAct, ACTANM_CMD_JUMP_END);
			pc->contTrans.y = 0;
			pc->subSeq++;
			retFlag = FALSE;
		}
		Set3DactTrans( Get_GS_SceneAct( pc->gs ), pc->targetAct, &pc->contTrans );
		break;
	case 4:
		pc->contCommand = PCC_STAY;
		pc->jumpFlag = FALSE;
		pc->subSeq = 0;
		retFlag = FALSE;
		break;
	}
	return retFlag;
}

//------------------------------------------------------------------
void MainPlayerControl( PLAYER_CONTROL* pc )
{
	BOOL mvf;

	if( damageSet(pc) == TRUE ){
		if( pc->status.hp ){
			pc->contCommand = PCC_HIT;	//�_���[�W�������
		} else {
			pc->contCommand = PCC_DEAD;	//���S
		}
	} else {
		//�s������
		if( Check3DactPlayerBusy( Get_GS_SceneAct( pc->gs ), pc->targetAct ) == TRUE ){
			return;	//�����A�j����
		}
		if( pc->skillBusyFlag ){
			return;	//�X�L�����s��
		}
		pc->hitEnableFlag = TRUE;
	}
	if( pc->subSeq ){
		pc->contCommand = pc->nowCommand;//�T�u�V�[�P���X���쒆
	}
	if(( pc->contCommand != PCC_SIT )&&( pc->sitDownFlag == TRUE )){	//���Ⴊ�ݒ��������菈��
		anmSet( pc, ACTANM_CMD_STANDUP );
		pc->sitDownFlag = FALSE;
		return;
	}
	switch( pc->contCommand ){
	case PCC_NOP:
		break;
	case PCC_STAY:
		anmSet( pc, ACTANM_CMD_STAY );
		break;
	case PCC_JUMP:
		if( jumpControl( pc ) == TRUE ){
			//mvf = moveSet( pc, JUMP_SPEED, pc->contDirection + pc->moveDir );
			mvf = moveSet( pc, JUMP_SPEED, pc->moveDir );
		}
		break;
	case PCC_STAYJUMP:
		jumpControl( pc );
		break;
	case PCC_WALK:
		mvf = moveSet( pc, WALK_SPEED, pc->contDirection + pc->moveDir );
		if( mvf == TRUE ){
			anmSet( pc, ACTANM_CMD_WALK );
		} else {
			anmSet( pc, ACTANM_CMD_STAY );
		}
		break;
	case PCC_RUN:
		mvf = moveSet( pc, RUN_SPEED, pc->contDirection + pc->moveDir );
		if( mvf == TRUE ){
			anmSet( pc, ACTANM_CMD_RUN );
		} else {
			anmSet( pc, ACTANM_CMD_STAY );
		}
		break;
	case PCC_ATTACK:
		//����ɂ���ă��[�V�����ω�
		switch( pc->nowAccesary ){
		defasult:
			anmSet( pc, ACTANM_CMD_STAY );
			break;
		case 1:
			directionSet( pc, pc->contDirection );	//�J���������Ɍ�������
			anmSet( pc, ACTANM_CMD_ATTACK );
			pc->skillCommand = PSC_ATTACK_SWORD;
			break;
		case 2:
			directionSet( pc, pc->contDirection );	//�J���������Ɍ�������
			anmSet( pc, ACTANM_CMD_SHOOT );
			pc->skillCommand = PSC_ATTACK_BOW;
			break;
		case 3:
			directionSet( pc, pc->contDirection );	//�J���������Ɍ�������
			anmSet( pc, ACTANM_CMD_SPELL );
			pc->skillCommand = PSC_ATTACK_FIRE;
			break;
		}
		break;
	case PCC_SIT:
		anmSet( pc, ACTANM_CMD_SITDOWN );
		pc->sitDownFlag = TRUE;
		pc->buildCommand = PBC_SUMMON;
		break;
	case PCC_WEPONCHANGE:		
		//�i���o�[�؂�ւ�
		pc->nowAccesary++;
		pc->nowAccesary&= 3;
		Set3DactPlayerChrAccesory( Get_GS_SceneAct( pc->gs ), pc->targetAct, pc->nowAccesary );
		break;
	case PCC_PUTON:		
		anmSet( pc, ACTANM_CMD_TAKE );
		pc->buildCommand = PBC_BUILD_CASTLE;
		break;
	case PCC_HIT:		
		Set3DactPlayerChrAnimeForceCmd( Get_GS_SceneAct( pc->gs ), pc->targetAct, ACTANM_CMD_HIT );
		pc->hitEnableFlag = FALSE;	//���ꃂ�[�V�������̓q�b�g��������Ȃ����Ƃ��ł���
		break;
	case PCC_DEAD:		
		switch( pc->subSeq ){
		case 0:
			Set3DactPlayerChrAnimeForceCmd
							( Get_GS_SceneAct( pc->gs ), pc->targetAct, ACTANM_CMD_DEAD );
			{
				u8 alpha = 20;	//���������o
				Set3DactBlendAlpha( Get_GS_SceneAct( pc->gs ), pc->targetAct, &alpha );
			}
			pc->deadFlag = TRUE;
			pc->recoverTimer = 10*60;	//���A�҂��P�O�b
			pc->hitEnableFlag = FALSE;	//���ꃂ�[�V�������̓q�b�g��������Ȃ����Ƃ��ł���
			pc->subSeq++;
			break;
		case 1:
			if( pc->recoverTimer ){
				pc->recoverTimer--;
			} else {
				u8 alpha = 31;
				Set3DactBlendAlpha( Get_GS_SceneAct( pc->gs ), pc->targetAct, &alpha );
				pc->status.hp = pc->status.hpMax;	//���A
				pc->deadFlag = FALSE;
				pc->subSeq = 0;
			}
			break;
		}
		break;
	}
	pc->nowCommand = pc->contCommand;
}

