//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
#include "player_cont.h"

#define TEAM_SUMMON_COUNT_MAX	(6)
#define TEAM_PLAYER_COUNT_MAX	(1)

typedef struct _TEAM_CONTROL	TEAM_CONTROL;

//	�`�[���R���g���[���N��
extern TEAM_CONTROL* AddTeamControl( GAME_SYSTEM* gs, HEAPID heapID );
// �`�[���R���g���[���I��
extern void RemoveTeamControl( TEAM_CONTROL* tc );
// �`�[�����C���R���g���[��
extern void MainTeamControl( TEAM_CONTROL* tc, BOOL onGameFlag );

// �v���[���[�o�^
extern int RegistPlayer
			( TEAM_CONTROL* tc, PLAYER_STATUS* ps, int playerObjID, int netID, VecFx32* trans );
// �v���[���[�o�^�폜
extern void ReleasePlayer( TEAM_CONTROL* tc, int playerID );
// �v���[���[�o�^���擾
extern int GetTeamPlayerCount( TEAM_CONTROL* tc );
// �v���[���[�R���g���[���擾
extern PLAYER_CONTROL* GetTeamPlayerControl( TEAM_CONTROL* tc, int playerID );
// ���ׂĂ̓o�^�v���[���[�̃R���g���[���ɑ΂��ăR�[���o�b�N���������s����
//  ���R�[���o�b�N��`:����( �R���g���[��, ���݂̎��s���ԃJ�E���^, ���[�N )
typedef void	(TEAM_PLAYER_CALLBACK)( PLAYER_CONTROL*, int, void* );	//
extern void ProcessingAllTeamPlayer( TEAM_CONTROL* tc, TEAM_PLAYER_CALLBACK* proc, void* work );

// ���_�G���A�_���[�W�擾
extern void GetCastleAreaDamage( TEAM_CONTROL* tc, int* damage );
// ���_�_���[�W�ݒ�
extern void SetCastleDamage( TEAM_CONTROL* tc, int* damage );
// ���_�ݒu
extern int CreateCastle( TEAM_CONTROL* tc, VecFx32* trans );
// ���_�폜
extern void DeleteCastle( TEAM_CONTROL* tc );
// ���_�g�o�擾
extern BOOL GetCastleHP( TEAM_CONTROL* tc, int* hp, int* hpMax );
// ���_���W�擾
extern BOOL GetCastleTrans( TEAM_CONTROL* tc, VecFx32* trans );

// �������ݒu
int CreateSummonObject( TEAM_CONTROL* tc, int summonTypeID, VecFx32* trans );
// �������폜
extern void DeleteSummonObject( TEAM_CONTROL* tc, int summonID );
// �������`��X�C�b�`�ݒ�
extern void SetSummonObjectDrawSW( TEAM_CONTROL* tc, int summonID, BOOL* drawSW );
// �������`��X�C�b�`�擾
extern void GetSummonObjectDrawSW( TEAM_CONTROL* tc, int summonID, BOOL* drawSW );
// �������_���[�W�ݒ�
extern void SetSummonObjectDamage( TEAM_CONTROL* tc, int summonID, int* damage );
// �������g�o�擾
extern void GetSummonObjectHP( TEAM_CONTROL* tc, int summonID, int* hp, int* hpMax );
// ���������W�擾
extern void GetSummonObjectTrans( TEAM_CONTROL* tc, int summonID, VecFx32* trans );
// ���ׂĂ̓o�^�������ɑ΂��ăR�[���o�b�N���������s����
//  ���R�[���o�b�N��`:����( �o�^�h�c, ���݂̎��s���ԃJ�E���^, ���[�N )
typedef void	(TEAM_SUMMON_CALLBACK)( TEAM_CONTROL* , int, int, void* );	//
extern void ProcessingAllTeamSummonObject
			( TEAM_CONTROL* tc, TEAM_SUMMON_CALLBACK* proc, void* work );

// �̈�}�X�N�r�b�g�}�b�v�쐬
extern void MakeTeamMapAreaMask( TEAM_CONTROL* tc );
// �̈�}�X�N�r�b�g�}�b�v�ύX�t���O�擾
extern BOOL GetMapAreaMaskDrawFlag( TEAM_CONTROL* tc );
// �̈�}�X�N�r�b�g�}�b�v�ύX�t���O�Z�b�g
extern void SetMapAreaMaskDrawFlag( TEAM_CONTROL* tc, BOOL* sw );
// �̈攻��}�X�N�r�b�g�}�b�v�擾
extern void GetMapAreaMask( TEAM_CONTROL* tc, GFL_BMP_DATA** mapArea );
// �̈�}�X�N��̍��W�擾
extern void GetMapAreaMaskPos( VecFx32* trans, u16* posx, u16* posy );
// �̈�Ǘ��r�b�g�}�b�v��̏�Ԏ擾
extern BOOL GetMapAreaMaskStatus( TEAM_CONTROL* tc, VecFx32* trans );

