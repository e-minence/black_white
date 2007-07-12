//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
typedef struct _SKILL_CONTROL	SKILL_CONTROL;

typedef enum {
	SKILL_NOP = 0,
	SKILL_SWORD,
	SKILL_ARROW,
	SKILL_STAFF,
}SKILL_CONTROL_COMMAND;

//�X�L���R���g���[���Z�b�g
extern SKILL_CONTROL* AddSkillControl( GAME_SYSTEM* gs, HEAPID heapID );
//�X�L���R���g���[���I��
extern void RemoveSkillControl( SKILL_CONTROL* sc );
//����v���[���[�̒ǉ�
extern void AddSkillCheckPlayer( SKILL_CONTROL* sc, PLAYER_CONTROL* pc );
//����v���[���[�̍폜
extern void RemoveSkillCheckPlayer( SKILL_CONTROL* sc, PLAYER_CONTROL* pc );
//�X�L���̒ǉ�
extern void SetSkillControlCommand( SKILL_CONTROL* sc, PLAYER_CONTROL* pc, int skillCommand );
//�X�L�����C���R���g���[��
extern void MainSkillControl( SKILL_CONTROL* sc );

