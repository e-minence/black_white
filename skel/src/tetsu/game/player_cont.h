//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
typedef struct _PLAYER_CONTROL	PLAYER_CONTROL;

typedef struct {
	char		name[10];
	s16			hp;
	s16			sp;
	s16			hpMax;
	s16			spMax;
	s16			attack;
	s16			defence;
	s16			agility;
	s16			intel;
	s16			mind;
}PLAYER_STATUS;

typedef enum {
	PCC_NOP = 0,
	PCC_STAY,

	PCC_WALK,
	PCC_RUN,
	PCC_JUMP,
	PCC_STAYJUMP,
	PCC_SLIDE,

	PCC_ATTACK,
	PCC_SIT,
	PCC_WEPONCHANGE,

	PCC_PUTON,
	PCC_TAKEOFF,

	PCC_BUILD,
	PCC_SUMMON,

	PCC_HIT,
	PCC_DEAD,

	PCC_TEST,

}PLAYER_CONTROL_COMMAND;

typedef enum {
	PMV_FRONT		=  0x0000,
	PMV_BACK		=  0x8000,
	PMV_LEFT		=  0x4000,
	PMV_RIGHT		= -0x4000,
	PMV_FRONT_LEFT	=  0x2000,
	PMV_FRONT_RIGHT	= -0x2000,
	PMV_BACK_LEFT	=  0x6000,
	PMV_BACK_RIGHT	= -0x6000,

}PLAYER_MOVE_DIR;

typedef enum {
	PMS_WALK = 0,
	PMS_RUN,
	PMS_JUMP

}PLAYER_MOVE_STATUS;

typedef enum {
	PSC_NOP = 0,
	PSC_ATTACK_SWORD,
	PSC_ATTACK_BOW,
	PSC_ATTACK_FIRE,

}PLAYER_SKILL_COMMAND;

typedef enum {
	PBC_NOP = 0,
	PBC_BUILD_CASTLE,
	PBC_SUMMON,

}PLAYER_BUILD_COMMAND;

typedef struct {
	VecFx32					trans;
	u16						direction;
	BOOL					anmSetFlag;
	BOOL					anmSetID;
	PLAYER_SKILL_COMMAND	skillCommand;
	PLAYER_BUILD_COMMAND	buildCommand;

}PLAYER_STATUS_NETWORK;

// �v���[���[�R���g���[���Z�b�g
extern PLAYER_CONTROL* AddPlayerControl
			( GAME_SYSTEM* gs, int targetAct, int netID, HEAPID heapID );
// �v���[���[�R���g���[�����C��
extern void MainPlayerControl( PLAYER_CONTROL* pc );
// �v���[���[�R���g���[�����C��(�l�b�g���[�N)
extern void MainNetWorkPlayerControl( PLAYER_CONTROL* pc, PLAYER_STATUS_NETWORK* psn );
// �v���[���[�R���g���[���I��
extern void RemovePlayerControl( PLAYER_CONTROL* pc );
// �v���[���[�֘A�t��3DactID�̎擾
extern void GetPlayerAct3dID( PLAYER_CONTROL* pc, int* act3dID );
// �v���[���[�����̎擾
extern void GetPlayerDirection( PLAYER_CONTROL* pc, u16* direction );
// �v���[���[�R���g���[�����W�̎擾�Ɛݒ�
extern void GetPlayerControlTrans( PLAYER_CONTROL* pc, VecFx32* trans );
extern void SetPlayerControlTrans( PLAYER_CONTROL* pc, const VecFx32* trans );
//�v���[���[�R���g���[�������̎擾�Ɛݒ�
extern void GetPlayerControlDirection( PLAYER_CONTROL* pc, u16* direction );
extern void SetPlayerControlDirection( PLAYER_CONTROL* pc, const u16* direction );
//�v���[���[�X�e�[�^�X�|�C���^�̎擾
extern PLAYER_STATUS* GetPlayerStatusPointer( PLAYER_CONTROL* pc );
//�v���[���[�R���g���[���\���ǂ����̃`�F�b�N
extern BOOL CheckPlayerControlEnable( PLAYER_CONTROL* pc );
//�v���[���[�X�e�[�^�X�̎擾�Ɛݒ�
extern void GetPlayerStatus( PLAYER_CONTROL* pc, PLAYER_STATUS* status );
extern void SetPlayerStatus( PLAYER_CONTROL* pc, const PLAYER_STATUS* status );
// �v���[���[�R�}���h�̐ݒ�
extern void SetPlayerControlCommand( PLAYER_CONTROL* pc, const PLAYER_CONTROL_COMMAND command );
// �v���[���[�U���R�}���h�̐ݒ�
extern void SetPlayerAttackCommand
	( PLAYER_CONTROL* pc, const PLAYER_CONTROL_COMMAND command, int attackID );
// �v���[���[�ړ��R�}���h�̐ݒ�
extern void SetPlayerMoveCommand
	( PLAYER_CONTROL* pc, PLAYER_CONTROL_COMMAND command, VecFx32* mvDir );
// �X�L���R�}���h�̎擾�ƃ��Z�b�g
extern PLAYER_SKILL_COMMAND GetPlayerSkillCommand( PLAYER_CONTROL* pc );
extern void ResetPlayerSkillCommand( PLAYER_CONTROL* pc );
// ���z�R�}���h�̎擾�ƃ��Z�b�g
extern PLAYER_BUILD_COMMAND GetPlayerBuildCommand( PLAYER_CONTROL* pc );
extern void ResetPlayerBuildCommand( PLAYER_CONTROL* pc );
// �X�L�����s���t���O�̐ݒ�ƃ��Z�b�g
extern void SetPlayerSkillBusyFlag( PLAYER_CONTROL* pc );
extern void ResetPlayerSkillBusyFlag( PLAYER_CONTROL* pc );
// �q�b�g����\�t���O�̎擾
extern BOOL GetPlayerHitEnableFlag( PLAYER_CONTROL* pc );
// ���S�t���O�̎擾�ƃ��Z�b�g
extern BOOL GetPlayerDeadFlag( PLAYER_CONTROL* pc );
extern void ResetPlayerDeadFlag( PLAYER_CONTROL* pc );
// �l�b�g�h�c�̎擾
extern int GetPlayerNetID( PLAYER_CONTROL* pc );
// �l�b�g���[�N�p�X�e�[�^�X�̎擾�ƃ��Z�b�g
extern void GetPlayerNetStatus( PLAYER_CONTROL* pc, PLAYER_STATUS_NETWORK* pNetStatus );
extern void ResetPlayerNetStatus( PLAYER_STATUS_NETWORK* pNetStatus );
// �g�o�����l�̐ݒ�
extern void SetPlayerDamage( PLAYER_CONTROL* pc, const s16 damage );
// �c�n�s�i���Ԍo�߂ɂ��g�o�����l�j�̐ݒ�
extern void SetPlayerDamageOnTime
				( PLAYER_CONTROL* pc, const u8 count, const u8 timer, const s8 value );
// ���W�F�l���[�g�l�̐ݒ�
extern void SetPlayerRegenerate
				( PLAYER_CONTROL* pc, const u8 count, const u8 timer, const s8 value );


