/////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_road_sys.h
 * @brief  ���HISS�V�X�e��
 * @author obata_toshihiro
 * @date   2009.07.29
 */
/////////////////////////////////////////////////////////////////////////////////////////////
#include "iss_road_sys.h"
#include "../field/field_sound.h"
#include "gamesystem/playerwork.h"



//===========================================================================================
/**
 * @brief �萔
 */
//===========================================================================================
#define MIN_VOLUME     (0)	// �Œቹ��
#define MAX_VOLUME   (127)	// �ő剹��
#define FADE_IN_SPEED  (8)	// �t�F�[�h�E�C�����x
#define FADE_OUT_SPEED (8)	// �t�F�[�h�E�A�E�g���x


//===========================================================================================
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//===========================================================================================

// 2�̃x�N�g�������������ǂ����𔻒肷��
BOOL IsVecEqual( const VecFx32* p_vec1, const VecFx32* p_vec2 );



//===========================================================================================
/**
 * @breif ���HISS�V�X�e���\����
 */
//===========================================================================================
struct _ISS_ROAD_SYS
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �N�����
	BOOL isActive;

	// ����
	int volume;

	// �Ď��Ώۃv���C���[
	PLAYER_WORK* pPlayer;

	// �O��Ď����̃v���C���[���W
	VecFx32 prevPlayerPos;
};


//===========================================================================================
/**
 * ���J�֐��̎���
 */
//===========================================================================================

//----------------------------------------------------------------------------
/**
 * @brief  ���HISS�V�X�e�����쐬����
 *
 * @param  p_player �Ď��Ώۂ̃v���C���[
 * @param  heap_id  �g�p����q�[�vID
 * 
 * @return ���HISS�V�X�e��
 */
//----------------------------------------------------------------------------
ISS_ROAD_SYS* ISS_ROAD_SYS_Create( PLAYER_WORK* p_player, HEAPID heap_id )
{
	ISS_ROAD_SYS* p_sys;

	// �������m��
	p_sys = (ISS_ROAD_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_ROAD_SYS ) );

	// �����ݒ�
	p_sys->heapID        = heap_id;
	p_sys->isActive      = FALSE;
	p_sys->volume        = MIN_VOLUME;
	p_sys->pPlayer       = p_player;
	p_sys->prevPlayerPos = *( PLAYERWORK_getPosition( p_player ) );
	
	// �쐬�������HISS�V�X�e����Ԃ�
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 * @brief  ���HISS�V�X�e����j������
 *
 * @param p_sys �j�����铹�HISS�V�X�e�� 
 */
//----------------------------------------------------------------------------
void ISS_ROAD_SYS_Delete( ISS_ROAD_SYS* p_sys )
{
	GFL_HEAP_FreeMemory( p_sys );
}

//----------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param p_sys ����Ώۂ̃V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_ROAD_SYS_Update( ISS_ROAD_SYS* p_sys )
{
	const VecFx32* p_player_pos = NULL;

	// �N�����Ă��Ȃ����, �������Ȃ�
	if( p_sys->isActive != TRUE ) return;

	// ��l���̍��W���擾
	p_player_pos = PLAYERWORK_getPosition( p_sys->pPlayer );

	// ���ʒ���
	if( IsVecEqual( p_player_pos, &p_sys->prevPlayerPos ) )
	{
		p_sys->volume -= FADE_OUT_SPEED;
		if( p_sys->volume < MIN_VOLUME ) p_sys->volume = MIN_VOLUME;
		OBATA_Printf( "Load ISS Volume DOWN\n" );
	}
	else
	{
		p_sys->volume += FADE_IN_SPEED;
		if( MAX_VOLUME < p_sys->volume ) p_sys->volume = MAX_VOLUME;
		OBATA_Printf( "Load ISS Volume UP\n" );
	}
	FIELD_SOUND_ChangeBGMActionVolume( p_sys->volume );

	// �L����l�����W���X�V
	p_sys->prevPlayerPos = *p_player_pos;

	// DEBUG: �f�o�b�O�o��
  /*
	if( p_sys->isActive )
	{
		OBATA_Printf( "-----------------------\n" );
		OBATA_Printf( "Load ISS Unit is active\n" );
		OBATA_Printf( "volume = %d\n", p_sys->volume );
	}
  */
}

//----------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param p_sys �N������V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_ROAD_SYS_On( ISS_ROAD_SYS* p_sys )
{
	p_sys->isActive = TRUE;
	p_sys->volume   = MIN_VOLUME;	// ���ʂ��ŏ��ɖ߂�
}

//----------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~������
 *
 * @param p_sys ��~������V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_ROAD_SYS_Off( ISS_ROAD_SYS* p_sys )
{
	p_sys->isActive = FALSE;
}

//----------------------------------------------------------------------------
/**
 * @breif �����Ԃ��擾����
 *
 * @param p_sys ��Ԃ𒲂ׂ�ISS�V�X�e��
 * 
 * @return ���쒆���ǂ���
 */
//----------------------------------------------------------------------------
BOOL ISS_ROAD_SYS_IsOn( const ISS_ROAD_SYS* p_sys )
{
	return p_sys->isActive;
}



//===========================================================================================
/**
 * @brief ����J�֐��̎���
 */
//===========================================================================================

//----------------------------------------------------------------------------
/**
 * @brief 2�̃x�N�g�������������ǂ����𔻒肷��
 *
 * @param p_vec1 ���肷��x�N�g��1
 * @param p_vec2 ���肷��x�N�g��2
 */
//----------------------------------------------------------------------------
BOOL IsVecEqual( const VecFx32* p_vec1, const VecFx32* p_vec2 )
{
	return ( ( p_vec1->x == p_vec2->x ) &&
			 ( p_vec1->y == p_vec2->y ) &&
			 ( p_vec1->z == p_vec2->z ) );
}
