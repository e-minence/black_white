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
}PLAYER_CONTROL_COMMAND;

typedef enum {
	PSC_NOP = 0,
}PLAYER_SKILL_COMMAND;

typedef struct {
	VecFx32					trans;
	u16						direction;
	BOOL					anmSetFlag;
	BOOL					anmSetID;
	PLAYER_SKILL_COMMAND	skillCommand;

}PLAYER_STATUS_NETWORK;

// �v���[���[�R���g���[���Z�b�g
extern PLAYER_CONTROL* AddPlayerControl( GAME_SYSTEM* gs, int netID, HEAPID heapID );
// �v���[���[�R���g���[�����C��
extern void MainPlayerControl( PLAYER_CONTROL* pc );
// �v���[���[�R���g���[�����C��(�l�b�g���[�N)
extern void MainNetWorkPlayerControl( PLAYER_CONTROL* pc, PLAYER_STATUS_NETWORK* psn );
// �v���[���[�R���g���[���I��
extern void RemovePlayerControl( PLAYER_CONTROL* pc );
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
// �X�L�����s���t���O�̐ݒ�ƃ��Z�b�g
extern void SetPlayerSkillBusyFlag( PLAYER_CONTROL* pc );
extern void ResetPlayerSkillBusyFlag( PLAYER_CONTROL* pc );
// �q�b�g����\�t���O�̎擾
extern BOOL GetPlayerHitEnableFlag( PLAYER_CONTROL* pc );
// �l�b�g�h�c�̎擾
extern int GetPlayerNetID( PLAYER_CONTROL* pc );
// �l�b�g���[�N�p�X�e�[�^�X�̎擾�ƃ��Z�b�g
extern void GetPlayerNetStatus( PLAYER_CONTROL* pc, PLAYER_STATUS_NETWORK* pNetStatus );
extern void ResetPlayerNetStatus( PLAYER_STATUS_NETWORK* pNetStatus );
// �v���[���[�ړ��J�n�ݒ�
extern void SetPlayerMove
				( PLAYER_CONTROL* pc, VecFx32* mvDir, fx32 speed, u16 theta );
// �v���[���[�ړ��I������
extern BOOL CheckPlayerMoveEnd( PLAYER_CONTROL* pc );


