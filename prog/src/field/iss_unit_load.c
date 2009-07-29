////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_unit_load.h
 * @brief  ���HISS���j�b�g
 * @author obata_toshihiro
 * @date   2009.07.29
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "iss_unit_load.h"
#include "sound/pm_sndsys.h"
#include "arc/arc_def.h"
#include "arc/iss_unit.naix"
#include "arc/fieldmap/zone_id.h"
#include "field_sound.h"
#include "field_player.h"
#include "field/field_const.h"


//=====================================================================================================
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//===================================================================================================== 

// 2�̃x�N�g�������������ǂ����𔻒肷��
BOOL IsVecEqual( const VecFx32* p_vec1, const VecFx32* p_vec2 );



//=====================================================================================================
/**
 * @breif ���HISS���j�b�g�\����
 */
//=====================================================================================================
struct _ISS_UNIT_LOAD
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �N�����
	BOOL isActive;

	// �Ď��Ώۃv���C���[
	FIELD_PLAYER* pPlayer;

	// �O��Ď����̃v���C���[���W
	VecFx32 prevPlayerPos;
};


//=====================================================================================================
/**
 * ���J�֐��̎���
 */
//===================================================================================================== 

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
ISS_UNIT_LOAD* ISS_UNIT_LOAD_Create( FIELD_PLAYER* p_player, u16 zone_id, HEAPID heap_id )
{
	ISS_UNIT_LOAD* p_unit;

	// �������m��
	p_unit = (ISS_UNIT_LOAD*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_UNIT_LOAD ) );

	// �����ݒ�
	p_unit->heapID   = heap_id;
	p_unit->isActive = FALSE;
	p_unit->pPlayer  = p_player;
	FIELD_PLAYER_GetPos( p_player, &p_unit->prevPlayerPos );
	
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
	int volume;
	VecFx32 player_pos;

	// �N�����Ă��Ȃ����, �������Ȃ�
	if( p_unit->isActive != TRUE ) return;

	// ��l���̍��W���擾
	FIELD_PLAYER_GetPos( p_unit->pPlayer, &player_pos );

	// ���ʒ���
	if( IsVecEqual( &player_pos, &p_unit->prevPlayerPos ) )
	{
		volume = 0;
	}
	else
	{
		volume = 127;
	}
	FIELD_SOUND_ChangeBGMActionVolume( volume );

	// DEBUG: �f�o�b�O�o��
	if( p_unit->isActive )
	{
		OBATA_Printf( "-----------------------\n" );
		OBATA_Printf( "Load ISS Unit is active\n" );
		OBATA_Printf( "volume = %d\n", volume );
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



//=====================================================================================================
/**
 * @brief ����J�֐��̎���
 */
//=====================================================================================================

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
