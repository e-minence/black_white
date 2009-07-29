////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_unit.h
 * @brief  �XISS���j�b�g
 * @author obata_toshihiro
 * @date   2009.07.16
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "iss_unit.h"
#include "sound/pm_sndsys.h"
#include "arc/arc_def.h"
#include "arc/iss_unit.naix"
#include "arc/fieldmap/zone_id.h"
#include "field_sound.h"
#include "field_player.h"
#include "field/field_const.h"


//=====================================================================================================
/**
 * @brief �萔�E�}�N��
 */
//===================================================================================================== 
// 1�̊XISS���j�b�g�������ʋ�Ԃ̐�
#define VOLUME_SPACE_NUM 4

// ��Βl���擾����
#define ABS(n) ( (n)>0? (n) : -(n) )


//=====================================================================================================
/**
 * @brief ���ʋ�� �\����
 *	�����Ԃ̑傫���ƁA���̋�ԓ��ł̉��ʂ�ێ�����
 */
//=====================================================================================================
typedef struct 
{
	u8 volume;	// ����
	u8 x_range;	// x�����͈�
	u8 y_range; // y�����͈�
	u8 z_range; // z�����͈�
}
VOLUME_SPACE;

//-----------------------------------------------------------------
/**
 * ����J�֐��̃v���g�^�C�v�錾
 */
//-----------------------------------------------------------------
// �w�肵�����W����ԓ��ɂ��邩�ǂ����𔻒肷��
static BOOL IsCover( const VOLUME_SPACE* p_vs, int cx, int cy, int cz, int x, int y, int z );


//=====================================================================================================
/**
 * @breif �XISS���j�b�g�\����
 */
//=====================================================================================================
struct _ISS_UNIT_CITY
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �Ď��Ώۃv���C���[
	FIELD_PLAYER* pPlayer;

	// �N�����
	BOOL isActive;

	// �XISS���j�b�g�̍��W(�P��:�O���b�h)
	int x;
	int y;
	int z;
	
	// �XISS���j�b�g�̉��ʋ��
	VOLUME_SPACE volumeSpace[ VOLUME_SPACE_NUM ];
};

//-----------------------------------------------------------------
/**
 * ����J�֐��̃v���g�^�C�v�錾
 */
//-----------------------------------------------------------------

// �O���b�h���W�����߂�
static void CalcGrid( const VecFx32* p_vec, int* dest_x, int* dest_y, int* dest_z );

// �w��ʒu�ɊY�����鉹�ʂ��擾����
static int GetVolumeByGrid( const ISS_UNIT_CITY* p_unit, const VecFx32* p_pos );

// ���g���G���f�B�A����u16�l�擾�֐�
static u16 GetU16( u8* data, int pos );

// �w�肵���]�[���ɉ������XISS���j�b�g�ɃZ�b�g�A�b�v����
static void SetupCityISSUnit( ISS_UNIT_CITY* p_unit, u16 zone_id );


//=====================================================================================================
/**
 * ���J�֐��̎���
 */
//===================================================================================================== 

//----------------------------------------------------------------------------
/**
 * @brief  �XISS���j�b�g���쐬����
 *
 * @param  p_player �Ď��Ώۂ̃v���C���[
 * @param  zone_id  �]�[��ID
 * @param  heap_id  �g�p����q�[�vID
 * 
 * @return �XISS���j�b�g
 */
//----------------------------------------------------------------------------
ISS_UNIT_CITY* ISS_UNIT_CITY_Create( FIELD_PLAYER* p_player, u16 zone_id, HEAPID heap_id )
{
	ISS_UNIT_CITY* p_unit;

	// �������m��
	p_unit = (ISS_UNIT_CITY*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_UNIT_CITY ) );

	// �����ݒ�
	p_unit->heapID  = heap_id;
	p_unit->pPlayer = p_player;
	ISS_UNIT_CITY_ZoneChange( p_unit, zone_id );
	
	// �쐬�����XISS���j�b�g��Ԃ�
	return p_unit;
}

//----------------------------------------------------------------------------
/**
 * @brief  �XISS���j�b�g��j������
 *
 * @param p_unit �j������XISS���j�b�g 
 */
//----------------------------------------------------------------------------
void ISS_UNIT_CITY_Delete( ISS_UNIT_CITY* p_unit )
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
void ISS_UNIT_CITY_Update( const ISS_UNIT_CITY* p_unit )
{
	int volume;
	VecFx32 pos;

	// �N�����Ă��Ȃ����, �������Ȃ�
	if( p_unit->isActive != TRUE ) return;

	// ���ʂ𒲐�����
	FIELD_PLAYER_GetPos( p_unit->pPlayer, &pos );
	volume = GetVolumeByGrid( p_unit, &pos );
	FIELD_SOUND_ChangeBGMActionVolume( volume );

	// DEBUG: �f�o�b�O�o��
	if( p_unit->isActive )
	{
		OBATA_Printf( "---------------------\n" );
		OBATA_Printf( "City ISS Unit is active\n" );
		OBATA_Printf( "volume = %d\n", volume );
		OBATA_Printf( "x, y, z = %d, %d, %d\n", p_unit->x, p_unit->y, p_unit->z );
	}
}
	

//---------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param p_unit       �ʒm�Ώۂ̊XISS���j�b�g
 * @param next_zone_id �V�����]�[��ID
 */
//---------------------------------------------------------------------------
void ISS_UNIT_CITY_ZoneChange( ISS_UNIT_CITY* p_unit, u16 next_zone_id )
{
	// �A�[�J�C�u����f�[�^��ǂݒ���
	SetupCityISSUnit( p_unit, next_zone_id );
}


//=====================================================================================================
/**
 * ����J�֐��̎���
 */
//===================================================================================================== 

//---------------------------------------------------------------------------- 
/**
 * @brief �w�肵�����W����ԓ��ɂ��邩�ǂ����𔻒肷��
 *
 * @param p_vs	����Ώۂ̉��ʋ��
 * @param cx	���ʋ�Ԓ��S����x���W
 * @param cy	���ʋ�Ԓ��S����y���W
 * @param cz	���ʋ�Ԓ��S����z���W
 * @param x		��l����x���W
 * @param y		��l����y���W
 * @param z		��l����z���W
 *
 * @return (x, y, z) �� p_vs��Ԃ̓����ɂ���ꍇ TRUE
 */
//---------------------------------------------------------------------------- 
static BOOL IsCover( const VOLUME_SPACE* p_vs, int cx, int cy, int cz, int x, int y, int z )
{
	int min_x, min_y, min_z;
	int max_x, max_y, max_z;

	// ��Ԃ̍ŏ��l�E�ő�l�����߂�
	min_x = cx - p_vs->x_range;
	min_y = cy - p_vs->y_range;
	min_z = cz - p_vs->z_range; 
	max_x = cx + p_vs->x_range;
	max_y = cy + p_vs->y_range;
	max_z = cz + p_vs->z_range;

	// ����
	if( ( x < min_x ) | ( max_x < x ) |
		( y < min_y ) | ( max_y < y ) |
		( z < min_z ) | ( max_z < z ) )
	{
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 * @brief �O���b�h���W�����߂�
 *
 * @param p_vec  ���[���h���W�n
 * @param dest_x ���߂��O���b�hx���W�̊i�[��
 * @param dest_y ���߂��O���b�hy���W�̊i�[��
 * @param dest_z ���߂��O���b�hz���W�̊i�[��
 */
//----------------------------------------------------------------------------
static void CalcGrid( const VecFx32* p_vec, int* dest_x, int* dest_y, int* dest_z )
{
	*dest_x = (int)( ( FX_Div( p_vec->x, FIELD_CONST_GRID_FX32_SIZE) & FX32_INT_MASK ) >> FX32_SHIFT );
	*dest_y = (int)( ( FX_Div( p_vec->y, FIELD_CONST_GRID_FX32_SIZE) & FX32_INT_MASK ) >> FX32_SHIFT );
	*dest_z = (int)( ( FX_Div( p_vec->z, FIELD_CONST_GRID_FX32_SIZE) & FX32_INT_MASK ) >> FX32_SHIFT );
}

//---------------------------------------------------------------------------- 
/**
 * @brief ���ʂ��擾����
 *
 * @param p_unit ����Ώۂ̃��j�b�g
 * @param p_pos  ��l���̍��W[�P�ʁF���[���h���W]
 *
 * @return �w�肵����l�����W�ɂ����鉹��[0, 127]
 */
//---------------------------------------------------------------------------- 
static int GetVolumeByGrid( const ISS_UNIT_CITY* p_unit, const VecFx32* p_pos )
{
	int i;
	int volume = 0;
	int x, y, z;

	// �O���b�h���W���v�Z
	CalcGrid( p_pos, &x, &y, &z );

	// �傫�ȋ�Ԃ��画�肵, �����Ȃ���Ԃ���������, �����I��
	for( i=0; i<VOLUME_SPACE_NUM; i++ )
	{
		if( IsCover( &p_unit->volumeSpace[i], p_unit->x, p_unit->y, p_unit->z, x, y, z ) != TRUE )
		{
			break;
		}

		volume = p_unit->volumeSpace[i].volume;
	}

	return volume;
}


//---------------------------------------------------------------------------- 
/**
 * @brief data + pos �̈ʒu����n�܂�2�o�C�g��, 
 *        ���g���G���f�B�A���ŕ���u16�̃f�[�^�Ƃ��ĉ��߂��l���擾����
 *
 * @param data �f�[�^�J�n�ʒu
 * @param pos  �J�n�ʒu�I�t�Z�b�g
 *
 * @return u16
 */
//---------------------------------------------------------------------------- 
static u16 GetU16( u8* data, int pos )
{
	u16 lower = (u16)( data[ pos ] );
	u16 upper = (u16)( data[ pos + 1 ] );
	u16 value = ( upper << 8 ) | lower;

	return value;
}

//---------------------------------------------------------------------------- 
/**
 * @brief �w�肵���]�[���ɉ��������j�b�g�ɃZ�b�g�A�b�v����
 *
 * @param p_unit  �f�[�^��ݒ肷�郆�j�b�g
 * @param zone_id �]�[��ID
 */
//---------------------------------------------------------------------------- 
static void SetupCityISSUnit( ISS_UNIT_CITY* p_unit, u16 zone_id )
{
	int i;
	int pos;		// �Q�ƈʒu
	u8* header;		// �w�b�_���̐擪�A�h���X
	u16 offset;		// �f�[�^���擪�ւ̃I�t�Z�b�g
	u8 table_size;	// �Q�ƃe�[�u���ɓo�^����Ă���f�[�^�̐�

	// �A�[�J�C�u�f�[�^�̓ǂݏo��
	header = (u8*)GFL_ARC_UTIL_Load( ARCID_ISS_UNIT, NARC_iss_unit_iss_unit_bin, FALSE, p_unit->heapID );

	// �w��]�[��ID���e�[�u���ɓo�^����Ă��邩�ǂ���������
	offset     = 0;
	pos        = 0;
	table_size = header[ pos++ ];	// �e�[�u���T�C�Y���擾
	for( i=0; i<table_size; i++ )
	{
		u16 id = GetU16( header, pos );
		pos += 2;
		
		// �w��]�[��ID�𔭌�������, �I�t�Z�b�g���擾�������I��
		if( id == zone_id )
		{
			offset = GetU16( header, pos );
			break;
		} 
		pos += 2;
	}
	
	// ���j�b�g�̃Z�b�g�A�b�v
	if( 0 < offset )	// (�w��]�[���ɊXISS���j�b�g�����݂���ꍇ)
	{
		u8* data = header + offset;		// �f�[�^���̐擪�A�h���X
		p_unit->isActive = TRUE;
		p_unit->x = GetU16( data, 2 );
		p_unit->y = GetU16( data, 4 );
		p_unit->z = GetU16( data, 6 );
		p_unit->volumeSpace[0].volume = *( (u8*)( data +  8 ) );
		p_unit->volumeSpace[1].volume = *( (u8*)( data +  9 ) );
		p_unit->volumeSpace[2].volume = *( (u8*)( data + 10 ) );
		p_unit->volumeSpace[3].volume = *( (u8*)( data + 11 ) );
		p_unit->volumeSpace[0].x_range = *( (u8*)( data + 12 ) );
		p_unit->volumeSpace[1].x_range = *( (u8*)( data + 13 ) );
		p_unit->volumeSpace[2].x_range = *( (u8*)( data + 14 ) );
		p_unit->volumeSpace[3].x_range = *( (u8*)( data + 15 ) );
		p_unit->volumeSpace[0].y_range = *( (u8*)( data + 16 ) );
		p_unit->volumeSpace[1].y_range = *( (u8*)( data + 17 ) );
		p_unit->volumeSpace[2].y_range = *( (u8*)( data + 18 ) );
		p_unit->volumeSpace[3].y_range = *( (u8*)( data + 19 ) );
		p_unit->volumeSpace[0].z_range = *( (u8*)( data + 20 ) );
		p_unit->volumeSpace[1].z_range = *( (u8*)( data + 21 ) );
		p_unit->volumeSpace[2].z_range = *( (u8*)( data + 22 ) );
		p_unit->volumeSpace[3].z_range = *( (u8*)( data + 23 ) );
	}
	else		// (�w��]�[���ɊXISS���j�b�g�����݂��Ȃ��ꍇ)
	{
		p_unit->isActive = FALSE;
	}
	
	// ��n��
	GFL_HEAP_FreeMemory( header );
}
