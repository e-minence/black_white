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

	PCC_WALK_FRONT,
	PCC_WALK_BACK,
	PCC_WALK_LEFT,
	PCC_WALK_RIGHT,
	PCC_WALK_FRONT_LEFT,
	PCC_WALK_FRONT_RIGHT,
	PCC_WALK_BACK_LEFT,
	PCC_WALK_BACK_RIGHT,

	PCC_RUN_FRONT,
	PCC_RUN_BACK,
	PCC_RUN_LEFT,
	PCC_RUN_RIGHT,
	PCC_RUN_FRONT_LEFT,
	PCC_RUN_FRONT_RIGHT,
	PCC_RUN_BACK_LEFT,
	PCC_RUN_BACK_RIGHT,

	PCC_ATTACK,
	PCC_SIT,
	PCC_WEPONCHANGE,

	PCC_PUTON,

}PLAYER_CONTROL_COMMAND;

// �v���[���[�R���g���[���Z�b�g
extern PLAYER_CONTROL* AddPlayerControl( GAME_SYSTEM* gs, int targetAct, HEAPID heapID );
// �v���[���[�R���g���[�����C��
extern void MainPlayerControl( PLAYER_CONTROL* pc );
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
//�v���[���[�X�e�[�^�X�̎擾�Ɛݒ�
extern void GetPlayerStatus( PLAYER_CONTROL* pc, PLAYER_STATUS* status );
extern void SetPlayerStatus( PLAYER_CONTROL* pc, const PLAYER_STATUS* status );
// �v���[���[�R�}���h�̐ݒ�
extern void SetPlayerControlCommand( PLAYER_CONTROL* pc, const PLAYER_CONTROL_COMMAND command );
// �X�L���R�}���h�̎擾�ƃ��Z�b�g
extern int GetPlayerSkillCommand( PLAYER_CONTROL* pc );
extern void ResetPlayerSkillCommand( PLAYER_CONTROL* pc );
// �X�L�����s���t���O�̐ݒ�ƃ��Z�b�g
extern void SetPlayerSkillBusyFlag( PLAYER_CONTROL* pc );
extern void ResetPlayerSkillBusyFlag( PLAYER_CONTROL* pc );
// �q�b�g����\�t���O�̎擾
extern BOOL GetPlayerHitEnableFlag( PLAYER_CONTROL* pc );
// �g�o�����l�̐ݒ�
extern void SetPlayerDamage( PLAYER_CONTROL* pc, const s16 damage );
// �c�n�s�i���Ԍo�߂ɂ��g�o�����l�j�̐ݒ�
extern void SetPlayerDamageOnTime
				( PLAYER_CONTROL* pc, const u8 count, const u8 timer, const s8 value );
// ���W�F�l���[�g�l�̐ݒ�
extern void SetPlayerRegenerate
				( PLAYER_CONTROL* pc, const u8 count, const u8 timer, const s8 value );


