////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_city_sys.h
 * @brief  �XISS���j�b�g
 * @author obata_toshihiro
 * @date   2009.07.16
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "iss_city_sys.h"
#include "../field/field_sound.h"
#include "gamesystem/playerwork.h"
#include "../../include/field/field_const.h"		// FIELD_CONST_GRID_FX32_SIZE
#include "arc/arc_def.h"
#include "arc/iss_unit.naix"


//=====================================================================================================
/**
 * @brief �萔�E�}�N��
 */
//===================================================================================================== 
// 1�̊XISS���j�b�g�������ʋ�Ԃ̐�
#define VOLUME_SPACE_NUM (4)

// ��Βl���擾����
#define ABS(n) ( (n)>0? (n) : -(n) )

// �������j�b�g�ԍ�(�XISS��BGM�����Ă��邪, ISS���j�b�g���z�u����Ă��Ȃ���)
#define INVALID_UNIT_NO (0xff)


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
typedef struct
{
	// �z�u�ꏊ�̃]�[��ID
	u16 zoneID;

	// ���W(�P��:�O���b�h)
	int x;
	int y;
	int z;

	// ���ʋ��
	VOLUME_SPACE volumeSpace[ VOLUME_SPACE_NUM ];
}
UNIT;

//-----------------------------------------------------------------
/**
 * ����J�֐��̃v���g�^�C�v�錾
 */
//-----------------------------------------------------------------

// �O���b�h���W�����߂�
static void CalcGrid( const VecFx32* p_vec, int* dest_x, int* dest_y, int* dest_z ); 

// �w��ʒu�ɊY�����鉹�ʂ��擾����
static int GetVolume( const UNIT* p_unit, const VecFx32* p_pos );


//=====================================================================================================
/**
 * @breif �XISS�V�X�e���\����
 */
//=====================================================================================================
struct _ISS_CITY_SYS
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �Ď��Ώۃv���C���[
	PLAYER_WORK* pPlayer;

	// �N�����
	BOOL isActive;		// ���쒆���ǂ���
	u8   activeUnitNo;	// ���쒆�̃��j�b�g�ԍ�

	// ISS���j�b�g���
	u8    unitNum;		// ���j�b�g��
	UNIT* unitData;		// ���j�b�g�z��
};

//-----------------------------------------------------------------
/**
 * ����J�֐��̃v���g�^�C�v�錾
 */
//-----------------------------------------------------------------

// ���g���G���f�B�A����u16�l�擾�֐�
static u16 GetU16( u8* data, int pos );

// �S�Ă̊XISS���j�b�g�̃f�[�^��ǂݍ���
static void LoadUnitData( ISS_CITY_SYS* p_sys );



//=====================================================================================================
/**
 * ���J�֐��̎���
 */
//===================================================================================================== 

//----------------------------------------------------------------------------
/**
 * @brief  �XISS�V�X�e�����쐬����
 *
 * @param  p_player �Ď��Ώۂ̃v���C���[
 * @param  heap_id  �g�p����q�[�vID
 * 
 * @return �XISS�V�X�e��
 */
//----------------------------------------------------------------------------
ISS_CITY_SYS* ISS_CITY_SYS_Create( PLAYER_WORK* p_player, HEAPID heap_id )
{
	ISS_CITY_SYS* p_sys;

	// �������m��
	p_sys = (ISS_CITY_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_CITY_SYS ) );

	// ������
	p_sys->heapID       = heap_id;
	p_sys->pPlayer      = p_player;
	p_sys->isActive     = FALSE;
	p_sys->activeUnitNo = INVALID_UNIT_NO;
	p_sys->unitNum      = 0;
	p_sys->unitData     = NULL;

	// ���j�b�g���̓ǂݍ���
	LoadUnitData( p_sys );
	
	// �쐬�����XISS�V�X�e����Ԃ�
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 * @brief  �XISS�V�X�e����j������
 *
 * @param p_sys �j������XISS�V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_CITY_SYS_Delete( ISS_CITY_SYS* p_sys )
{
	// �e���j�b�g��j��
	GFL_HEAP_FreeMemory( p_sys->unitData );

	// �{�̂̔j��
	GFL_HEAP_FreeMemory( p_sys );
}

//----------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param p_sys ����Ώۂ̊XISS�V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_CITY_SYS_Update( ISS_CITY_SYS* p_sys )
{
	int volume;
	const VecFx32* p_pos = NULL;

	// �N�����Ă��Ȃ����, �������Ȃ�
	if( p_sys->isActive != TRUE ) return;

	// ISS���j�b�g���z�u����Ă��Ȃ����, ����0
	if( p_sys->activeUnitNo == INVALID_UNIT_NO )
	{
		FIELD_SOUND_ChangeBGMActionVolume( 0 ); 
		OBATA_Printf( "City ISS Unit is active. But there is not a unit. volume = 0\n" );
		return;
	}

	// ���ʂ𒲐�����
	p_pos  = PLAYERWORK_getPosition( p_sys->pPlayer );
	volume = GetVolume( &p_sys->unitData[ p_sys->activeUnitNo ], p_pos );
	FIELD_SOUND_ChangeBGMActionVolume( volume );

	// DEBUG: �f�o�b�O�o��
	if( p_sys->isActive )
	{
		UNIT* p_unit = &p_sys->unitData[ p_sys->activeUnitNo ];
		OBATA_Printf( "---------------------\n" );
		OBATA_Printf( "City ISS Unit is active\n" );
		OBATA_Printf( "active unit no = %d\n", p_sys->activeUnitNo );
		OBATA_Printf( "volume = %d\n", volume );
		OBATA_Printf( "x, y, z = %d, %d, %d\n", p_unit->x, p_unit->y, p_unit->z );
	}
}
	

//---------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param p_sys        �ʒm�Ώۂ̊XISS�V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//---------------------------------------------------------------------------
void ISS_CITY_SYS_ZoneChange( ISS_CITY_SYS* p_sys, u16 next_zone_id )
{
	int i;

	// �V�����]�[��ID�������j�b�g��T��
	for( i=0; i<p_sys->unitNum; i++ )
	{
		// ���� ==> ���j�b�g�ԍ����X�V
		if( p_sys->unitData[i].zoneID == next_zone_id )
		{ 
			p_sys->activeUnitNo = i;
			return;
		}
	}

	// �w��]�[��ID��ISS���j�b�g�����݂��Ȃ��ꍇ
	p_sys->activeUnitNo = INVALID_UNIT_NO;

	// DEBUG:
	{
		int i;
		OBATA_Printf( "next zone id = %d\n", next_zone_id );
		for( i=0; i<p_sys->unitNum; i++ )
		{
			OBATA_Printf( "zone if of unit[%d] = %d\n", i, p_sys->unitData[i].zoneID );
		}
	}
}

//----------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param p_sys �N������V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_CITY_SYS_On( ISS_CITY_SYS* p_sys )
{
	// ���łɋN�����Ă���ꍇ, �������Ȃ�
	if( p_sys->isActive != TRUE )
	{
		// �N����, ISS���j�b�g���X�V����
		p_sys->isActive = TRUE;
		ISS_CITY_SYS_ZoneChange( p_sys, PLAYERWORK_getZoneID( p_sys->pPlayer ) );
	}
}

//----------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~������
 *
 * @param p_sys ��~������V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_CITY_SYS_Off( ISS_CITY_SYS* p_sys )
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
BOOL ISS_CITY_SYS_IsOn( const ISS_CITY_SYS* p_sys )
{
	return p_sys->isActive;
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
static int GetVolume( const UNIT* p_unit, const VecFx32* p_pos )
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

		// ��l����������, �ł���������Ԃɐݒ肳�ꂽ���ʂ��擾����
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
 * @brief �S�Ă̊XISS���j�b�g�f�[�^��ǂݍ���
 *
 * @param p_sys   �f�[�^��ݒ肷��XISS�V�X�e��
 * @param zone_id �]�[��ID
 *
 * [�o�C�i���E�f�[�^�E�t�H�[�}�b�g]
 * 
 * [0] ���j�b�g��
 * 
 * [1] ���j�b�g�̃]�[��ID
 * [2] 
 * [3] ���j�b�g�f�[�^�ւ�, �擪����̃I�t�Z�b�g
 * [4] 
 * 
 * [1]-[4]�����j�b�g����������
 *
 * [x+ 0] �]�[��ID 
 * [x+ 1] 
 * [x+ 2] x���W 
 * [x+ 3] 
 * [x+ 4] y���W 
 * [x+ 5] 
 * [x+ 6] z���W 
 * [x+ 7] 
 * [x+ 8] ����1
 * [x+ 9] ����2
 * [x+10] ����3
 * [x+11] ����4
 * [x+12] x�͈�1 
 * [x+13] x�͈�2 
 * [x+14] x�͈�3 
 * [x+15] x�͈�4 
 * [x+16] y�͈�1 
 * [x+17] y�͈�2 
 * [x+18] y�͈�3 
 * [x+19] y�͈�4 
 * [x+20] z�͈�1 
 * [x+21] z�͈�2 
 * [x+22] z�͈�3 
 * [x+23] z�͈�4 
 *
 * [x+0]-[x+23]�����j�b�g����������
 */   
//---------------------------------------------------------------------------- 
static void LoadUnitData( ISS_CITY_SYS* p_sys )
{
	int i;
	int pos = 0;	// �Q�ƈʒu
	u8* header;		// �w�b�_���̐擪�A�h���X
	u8 unit_num;	// ���j�b�g��
	u16* zone_id;	// �e���j�b�g�̃]�[��ID
	u16* offset;	// �e���j�b�g�f�[�^�擪���ւ̃I�t�Z�b�g

	// �A�[�J�C�u�f�[�^�̓ǂݍ���
	header = (u8*)GFL_ARC_UTIL_Load( ARCID_ISS_UNIT, NARC_iss_unit_iss_unit_bin, FALSE, p_sys->heapID );

	// ���j�b�g�����擾
	unit_num = header[ pos++ ];	

	// �e�[�u�����쐬
	zone_id = (u16*)GFL_HEAP_AllocMemoryLo( p_sys->heapID, sizeof( u16 ) * unit_num );
	offset  = (u16*)GFL_HEAP_AllocMemoryLo( p_sys->heapID, sizeof( u16 ) * unit_num );

	// �e�[�u���f�[�^���擾
	for( i=0; i<unit_num; i++ )
	{
		zone_id[i] = GetU16( header, pos );		pos += 2;	// �]�[��ID
		offset[i]  = GetU16( header, pos );		pos += 2;	// �I�t�Z�b�g
	}
	
	// �S���j�b�g�̃Z�b�g�A�b�v
	p_sys->unitNum  = unit_num;
	p_sys->unitData = (UNIT*)GFL_HEAP_AllocMemory( p_sys->heapID, sizeof( UNIT ) * unit_num );
	for( i=0; i<unit_num; i++ )
	{
		u8* data = header + offset[i];				// �f�[�^���̐擪�A�h���X
		p_sys->unitData[i].zoneID = zone_id[i];
		p_sys->unitData[i].x = GetU16( data, 2 );
		p_sys->unitData[i].y = GetU16( data, 4 );
		p_sys->unitData[i].z = GetU16( data, 6 );
		p_sys->unitData[i].volumeSpace[0].volume = *( (u8*)( data +  8 ) );
		p_sys->unitData[i].volumeSpace[1].volume = *( (u8*)( data +  9 ) );
		p_sys->unitData[i].volumeSpace[2].volume = *( (u8*)( data + 10 ) );
		p_sys->unitData[i].volumeSpace[3].volume = *( (u8*)( data + 11 ) );
		p_sys->unitData[i].volumeSpace[0].x_range = *( (u8*)( data + 12 ) );
		p_sys->unitData[i].volumeSpace[1].x_range = *( (u8*)( data + 13 ) );
		p_sys->unitData[i].volumeSpace[2].x_range = *( (u8*)( data + 14 ) );
		p_sys->unitData[i].volumeSpace[3].x_range = *( (u8*)( data + 15 ) );
		p_sys->unitData[i].volumeSpace[0].y_range = *( (u8*)( data + 16 ) );
		p_sys->unitData[i].volumeSpace[1].y_range = *( (u8*)( data + 17 ) );
		p_sys->unitData[i].volumeSpace[2].y_range = *( (u8*)( data + 18 ) );
		p_sys->unitData[i].volumeSpace[3].y_range = *( (u8*)( data + 19 ) );
		p_sys->unitData[i].volumeSpace[0].z_range = *( (u8*)( data + 20 ) );
		p_sys->unitData[i].volumeSpace[1].z_range = *( (u8*)( data + 21 ) );
		p_sys->unitData[i].volumeSpace[2].z_range = *( (u8*)( data + 22 ) );
		p_sys->unitData[i].volumeSpace[3].z_range = *( (u8*)( data + 23 ) );
	}
	
	// ��n��
	GFL_HEAP_FreeMemory( zone_id );
	GFL_HEAP_FreeMemory( offset );
	GFL_HEAP_FreeMemory( header );
} 
