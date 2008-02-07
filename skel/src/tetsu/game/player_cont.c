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
	VecFx32					moveVecNormal;
	int						moveYcount;
	int						attackID;

	PLAYER_STATUS			status;
	BOOL					anmSetFlag;	//�ʐM�p
	int						anmSetID;	//�ʐM�p
#if 0
	GFL_CLWK*				statusObj;
#endif
	SCENE_ACT*				sceneAct;
	u16						work[8];
};

#define WALK_SPEED	(4 << FX32_SHIFT)
#define RUN_SPEED	(4 << FX32_SHIFT)
#define JUMP_SPEED	(4 << FX32_SHIFT)
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
	pc->sceneAct = Create3Dact( Get_GS_G3Dscene(gs), heapID );

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

	pc->contTrans.x = 0;
	pc->contTrans.y = 0;
	pc->contTrans.z = 0;

	pc->nowAccesary = 0;
	pc->recoverTimer = 0;
	pc->jumpCalcWork = 0;
	pc->moveYcount = 0;
	pc->attackID = 0;

	pc->subSeq = 0;
	pc->nowCommand = 0;
	pc->status = statusDefault;

	Set3DactDrawSw( pc->sceneAct, TRUE );

	{
		int i;
		for( i=0; i<8; i++ ){
			pc->work[i] = 0;
		}
	}
	return pc;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�R���g���[���I��
 */
//------------------------------------------------------------------
void RemovePlayerControl( PLAYER_CONTROL* pc )
{
	Delete3Dact( pc->sceneAct );
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
	Set3DactTrans( pc->sceneAct, &pc->contTrans );
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
 * @brief	�v���[���[�U���R�}���h�̐ݒ�
 */
//------------------------------------------------------------------
void SetPlayerAttackCommand
	( PLAYER_CONTROL* pc, const PLAYER_CONTROL_COMMAND command, int attackID )
{
	pc->contCommand = command;
	pc->attackID = attackID;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�ړ��R�}���h�̐ݒ�iXZ���j
 */
//------------------------------------------------------------------
void SetPlayerMoveCommand
	( PLAYER_CONTROL* pc, PLAYER_CONTROL_COMMAND command, VecFx32* mvDir )
{
	VecFx32 tmpVec, rotVec;
	fx32 speed = WALK_SPEED;

	if( pc->jumpFlag == TRUE ){	//�W�����v������s��
		return;
	}
	pc->contCommand = command;
	//�i�s�x�N�g���̎Z�o
	VEC_Subtract( mvDir, &pc->contTrans, &tmpVec );
	tmpVec.y = 0;	//XZ���ɂ��Ă̐��x�������邽��0�Œ�
	VEC_Normalize( &tmpVec, &tmpVec );	//���K��

	if( command == PCC_RUN ){
		speed = RUN_SPEED;
	}
	pc->moveVecNormal.x = FX_Mul( tmpVec.x, speed );
	pc->moveVecNormal.z = FX_Mul( tmpVec.z, speed );

	//�����ݒ�
	rotVec.x = 0;
	rotVec.y = FX_Atan2Idx( -pc->moveVecNormal.z, pc->moveVecNormal.x ) + 0x4000;
	rotVec.z = 0;

	pc->nowDirection = rotVec.y - 0x8000;

	Set3DactRotate( pc->sceneAct, &rotVec );
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
 * @brief	�l�b�g���[�N�p�X�e�[�^�X�̎擾�ƃ��Z�b�g
 */
//------------------------------------------------------------------
void GetPlayerNetStatus( PLAYER_CONTROL* pc, PLAYER_STATUS_NETWORK* pNetStatus )
{
	pNetStatus->trans = pc->contTrans;
	pNetStatus->direction = pc->nowDirection;
	pNetStatus->anmSetFlag = pc->anmSetFlag;
	pNetStatus->anmSetID = pc->anmSetID;
	pNetStatus->skillCommand = pc->skillCommand;
	pNetStatus->buildCommand = pc->buildCommand;
}

void ResetPlayerNetStatus( PLAYER_STATUS_NETWORK* pNetStatus )
{
	pNetStatus->trans.x = 0;
	pNetStatus->trans.y = 0;
	pNetStatus->trans.z = 0;
	pNetStatus->direction = 0;
	pNetStatus->anmSetFlag = FALSE;
	pNetStatus->anmSetID = 0;
	pNetStatus->skillCommand = 0;
	pNetStatus->buildCommand = 0;
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
static int dirOffsTable[] = 
	{ 0x0000, 0x8000, 0x4000,-0x4000, 0x2000,-0x2000, 0x6000,-0x6000, };

//------------------------------------------------------------------
static void anmReset( PLAYER_CONTROL* pc )
{
	pc->anmSetFlag = FALSE;
	pc->anmSetID = ACTANM_CMD_STAY;	//=0
}

//------------------------------------------------------------------
static void anmSet( PLAYER_CONTROL* pc, int anmID )
{
	if( Set3DactPlayerChrAnimeCmd( pc->sceneAct, anmID ) == TRUE ){
		pc->anmSetFlag = TRUE;
		pc->anmSetID = anmID;
	}
}

//------------------------------------------------------------------
static void anmSetForce( PLAYER_CONTROL* pc, int anmID )
{
	Set3DactPlayerChrAnimeForceCmd( pc->sceneAct, anmID );
	pc->anmSetFlag = TRUE;
	pc->anmSetID = anmID;
}

//------------------------------------------------------------------
static void directionSet( PLAYER_CONTROL* pc, u16 direction )
{
	VecFx32	rotVec;		

	pc->nowDirection = direction;
	rotVec.x = 0;
	rotVec.y = direction + 0x8000;
	rotVec.z = 0;
	Set3DactRotate( pc->sceneAct, &rotVec );
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
static BOOL damageControl( PLAYER_CONTROL* pc )
{
	if( pc->recoverTimer ){
		if( pc->recoverTimer < 10 + 10*30 ){	//���A�҂��P�O�b
			pc->recoverTimer++;
		} else {
			u8 alpha = 31;

			anmSetForce( pc, ACTANM_CMD_STAY );
			pc->recoverTimer = 0;	//���A�҂��J�E���^������
			Set3DactBlendAlpha( pc->sceneAct, &alpha );
			pc->status.hp = pc->status.hpMax;	//���A
			pc->deadFlag = FALSE;
			pc->subSeq = 0;
		}
	} else {
		if( damageSet(pc) == TRUE ){
			if( pc->status.hp ){
				//�_���[�W�������
				anmSetForce( pc, ACTANM_CMD_HIT );
				pc->hitEnableFlag = FALSE;	//���ꃂ�[�V�������̓q�b�g��������Ȃ��i�b��j
			} else {
				u8 alpha = 20;	//���������o
				//���S
				anmSetForce( pc, ACTANM_CMD_DEAD );
				Set3DactBlendAlpha( pc->sceneAct, &alpha );
				pc->deadFlag = TRUE;
				pc->hitEnableFlag = FALSE;	//���S���̓q�b�g��������Ȃ�
				pc->recoverTimer = 10;	//���A�҂��J�E���^�J�n
			}
		} else {
			//�s������
			if( Check3DactPlayerBusy( pc->sceneAct ) == TRUE ){
				return FALSE;	//�����A�j����
			}
			if( pc->skillBusyFlag ){
				return FALSE;	//�X�L�����s��
			}
			pc->hitEnableFlag = TRUE;
		}
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
static BOOL moveControl( PLAYER_CONTROL* pc )
{
	fx32	gravity = (9.8f * FX32_ONE)/8;

	VecFx32 limitTrans;
	VecFx32 nextTrans = pc->contTrans;

	nextTrans.x += pc->moveVecNormal.x;
	nextTrans.y += pc->moveVecNormal.y;
	nextTrans.z += pc->moveVecNormal.z;

	if( pc->moveVecNormal.y ){	//�㉺�ړ��v�f����
		//Y���ړ��i�d�̓R���g���[���j
		nextTrans.y -= ( gravity * pc->moveYcount / 2 );
		pc->moveYcount++;
	}
	if( CheckHitMapGroundLimit( &pc->contTrans, &nextTrans, &limitTrans ) == TRUE ){
		//�ڒn
		pc->moveVecNormal.y = 0;
		pc->moveYcount = 0;
		nextTrans = limitTrans;
	}
	pc->contTrans = nextTrans;

	pc->moveVecNormal.x = 0;
	pc->moveVecNormal.z = 0;

	Set3DactTrans( pc->sceneAct, &pc->contTrans );

	return TRUE;
}

//------------------------------------------------------------------
#define JUMP_SPEEDY (FX32_ONE*3)

static BOOL jumpControl( PLAYER_CONTROL* pc )
{
	BOOL retFlag = TRUE;

	switch( pc->subSeq ){
	case 0:
		anmSet( pc, ACTANM_CMD_JUMP_RDY );
		pc->jumpFlag = TRUE;
		pc->subSeq++;
		retFlag = FALSE;
		break;
	case 1:
		anmSet( pc, ACTANM_CMD_JUMPUP );
		pc->moveVecNormal.y = JUMP_SPEEDY;
		pc->moveYcount = 0;
		pc->subSeq++;
		retFlag = FALSE;
		break;
	case 2:
		if( !pc->moveVecNormal.y ){
			anmSet( pc, ACTANM_CMD_JUMP_END );
			pc->subSeq++;
		} else {
			//�ړ��ݒ�
			VecFx32 tmpTrans;

			tmpTrans.x = -(fx32)FX_SinIdx( pc->nowDirection );
			tmpTrans.y = 0;
			tmpTrans.z = -(fx32)FX_CosIdx( pc->nowDirection );

			VEC_Normalize( &tmpTrans, &tmpTrans );	//���K��

			pc->moveVecNormal.x = FX_Mul( tmpTrans.x, JUMP_SPEED );
			pc->moveVecNormal.z = FX_Mul( tmpTrans.z, JUMP_SPEED );
		}
		retFlag = TRUE;
		break;
	case 3:
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

	anmReset( pc );			//�ʐM�A�j���R�}���h���Z�b�g

	if( damageControl( pc ) == FALSE ){	//�_���[�W������s�����ǂ���
		return;
	}
	if( moveControl( pc ) == FALSE ){
		return;
	}

	if( pc->subSeq ){
		pc->contCommand = pc->nowCommand;//�T�u�V�[�P���X���쒆
	}
	if(( pc->contCommand != PCC_SIT )&&( pc->sitDownFlag == TRUE )){	//���Ⴊ�ݒ��������菈��
		anmSetForce( pc, ACTANM_CMD_STANDUP );
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
	case PCC_STAYJUMP:
		jumpControl( pc );
		break;
	case PCC_WALK:
		anmSet( pc, ACTANM_CMD_WALK );
		break;
	case PCC_RUN:
		anmSet( pc, ACTANM_CMD_RUN );
		break;
	case PCC_ATTACK:
		//����ɂ���ă��[�V�����ω�
#if 0
		switch( pc->nowAccesary ){
		defasult:
			anmSetForce( pc, ACTANM_CMD_STAY );
			break;
		case 1:
			directionSet( pc, pc->contDirection );	//�J���������Ɍ�������
			anmSetForce( pc, ACTANM_CMD_ATTACK );
			pc->skillCommand = PSC_ATTACK_SWORD;
			break;
		case 2:
			directionSet( pc, pc->contDirection );	//�J���������Ɍ�������
			anmSetForce( pc, ACTANM_CMD_SHOOT );
			pc->skillCommand = PSC_ATTACK_BOW;
			break;
		case 3:
			directionSet( pc, pc->contDirection );	//�J���������Ɍ�������
			anmSetForce( pc, ACTANM_CMD_SPELL );
			pc->skillCommand = PSC_ATTACK_FIRE;
			break;
		}
#else
		directionSet( pc, pc->contDirection );	//�J���������Ɍ�������
		anmSetForce( pc, ACTANM_CMD_SWORD_ATTACK1 + pc->attackID );
		pc->skillCommand = PSC_ATTACK_SWORD;
#endif
		break;
	case PCC_BUILD:
		anmSet( pc, ACTANM_CMD_TAKE );
		pc->buildCommand = PBC_BUILD_CASTLE;
		break;
	case PCC_SUMMON:
		anmSet( pc, ACTANM_CMD_TAKE );
		pc->buildCommand = PBC_SUMMON;
		break;
	case PCC_SIT:
		anmSet( pc, ACTANM_CMD_SITDOWN );
		pc->sitDownFlag = TRUE;
		break;
	case PCC_PUTON:		
		anmSetForce( pc, ACTANM_CMD_TAKE );
		break;
	case PCC_TAKEOFF:		
		anmSetForce( pc, ACTANM_CMD_TAKE );
		break;
	case PCC_WEPONCHANGE:		
		//�i���o�[�؂�ւ�
		pc->nowAccesary++;
		pc->nowAccesary&= 3;
		Set3DactPlayerChrAccesory( pc->sceneAct, pc->nowAccesary );
		break;
	case PCC_TEST:		
		pc->work[0]++;
		pc->work[0]&=3;
		ChangeEquipNum( pc->sceneAct, pc->work[0] );
		anmSetForce( pc, ACTANM_CMD_STAY );
		break;
	}
	pc->nowCommand = pc->contCommand;
}

//------------------------------------------------------------------
void MainNetWorkPlayerControl( PLAYER_CONTROL* pc, PLAYER_STATUS_NETWORK* psn )
{
	SetPlayerControlTrans( pc, &psn->trans );
	directionSet( pc, psn->direction );
	pc->skillCommand = psn->skillCommand;
	pc->buildCommand = psn->buildCommand;

	if( damageControl( pc ) == FALSE ){	//�_���[�W������s�����ǂ���
		return;
	}
	if( psn->anmSetFlag == TRUE ){
		Set3DactPlayerChrAnimeForceCmd( pc->sceneAct, psn->anmSetID );
		psn->anmSetFlag = FALSE;
	}
}

