/////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_unit_load.h
 * @brief  ���HISS���j�b�g
 * @author obata_toshihiro
 * @date   2009.07.29
 */
/////////////////////////////////////////////////////////////////////////////////////////////
#include "iss_unit_load.h"
#include "field_sound.h"
#include "gamesystem/playerwork.h"



//===========================================================================================
/**
 * @brief �萔
 */
//===========================================================================================
#define MIN_VOLUME     (0)	// �Œቹ��
#define MAX_VOLUME   (127)	// �ő剹��
#define FADE_IN_SPEED  (4)	// �t�F�[�h�E�C�����x
#define FADE_OUT_SPEED (4)	// �t�F�[�h�E�A�E�g���x


//===========================================================================================
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//===========================================================================================

// 2�̃x�N�g�������������ǂ����𔻒肷��
BOOL IsVecEqual( const VecFx32* p_vec1, const VecFx32* p_vec2 );



//===========================================================================================
/**
 * @breif ���HISS���j�b�g�\����
 */
//===========================================================================================
struct _ISS_UNIT_LOAD
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
 * @brief  ���HISS���j�b�g���쐬����
 *
 * @param  p_player �Ď��Ώۂ̃v���C���[
 * @param  zone_id  �]�[��ID
 * @param  heap_id  �g�p����q�[�vID
 * 
 * @return ���HISS���j�b�g
 */
//----------------------------------------------------------------------------
ISS_UNIT_LOAD* ISS_UNIT_LOAD_Create( PLAYER_WORK* p_player, u16 zone_id, HEAPID heap_id )
{
	ISS_UNIT_LOAD* p_unit;

	// �������m��
	p_unit = (ISS_UNIT_LOAD*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_UNIT_LOAD ) );

	// �����ݒ�
	p_unit->heapID        = heap_id;
	p_unit->isActive      = FALSE;
	p_unit->volume        = MIN_VOLUME;
	p_unit->pPlayer       = p_player;
	p_unit->prevPlayerPos = *( PLAYERWORK_getPosition( p_player ) );
	
	// �쐬�������HISS���j�b�g��Ԃ�
	return p_unit;
}

//----------------------------------------------------------------------------
/**
 * @brief  ���HISS���j�b�g��j������
 *
 * @param p_unit �j�����铹�HISS���j�b�g 
 */
//----------------------------------------------------------------------------
void ISS_UNIT_LOAD_Delete( ISS_UNIT_LOAD* p_unit )
{
	GFL_HEAP_FreeMemory( p_unit );
}

//----------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param p_unit ����Ώۂ̃��j�b�g
 */
//----------------------------------------------------------------------------
void ISS_UNIT_LOAD_Update( ISS_UNIT_LOAD* p_unit )
{
	const VecFx32* p_player_pos = NULL;

	// �N�����Ă��Ȃ����, �������Ȃ�
	if( p_unit->isActive != TRUE ) return;

	// ��l���̍��W���擾
	p_player_pos = PLAYERWORK_getPosition( p_unit->pPlayer );

	// ���ʒ���
	if( IsVecEqual( p_player_pos, &p_unit->prevPlayerPos ) )
	{
		p_unit->volume -= FADE_OUT_SPEED;
		if( p_unit->volume < MIN_VOLUME ) p_unit->volume = MIN_VOLUME;
	}
	else
	{
		p_unit->volume += FADE_IN_SPEED;
		if( MAX_VOLUME < p_unit->volume ) p_unit->volume = MAX_VOLUME;
	}
	FIELD_SOUND_ChangeBGMActionVolume( p_unit->volume );

	// �L����l�����W���X�V
	p_unit->prevPlayerPos = *p_player_pos;

	// DEBUG: �f�o�b�O�o��
	if( p_unit->isActive )
	{
		OBATA_Printf( "-----------------------\n" );
		OBATA_Printf( "Load ISS Unit is active\n" );
		OBATA_Printf( "volume = %d\n", p_unit->volume );
	}
}

//----------------------------------------------------------------------------
/**
 * @brief �����Ԃ�ݒ肷��
 *
 * @param active ���삳���邩�ǂ���
 */
//----------------------------------------------------------------------------
extern void ISS_UNIT_LOAD_SetActive( ISS_UNIT_LOAD* p_unit, BOOL active )
{
	p_unit->isActive = active;
	p_unit->volume   = MIN_VOLUME;	// ���ʂ��ŏ��ɖ߂�
}

//----------------------------------------------------------------------------
/**
 * @breif �����Ԃ��擾����
 *
 * @param p_unit ��Ԃ𒲂ׂ�ISS���j�b�g
 * 
 * @return ���쒆���ǂ���
 */
//----------------------------------------------------------------------------
extern BOOL ISS_UNIT_LOAD_IsActive( const ISS_UNIT_LOAD* p_unit )
{
	return p_unit->isActive;
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
