///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_city_unit.c
 * @brief  �XISS���j�b�g
 * @author obata_toshihiro
 * @date   2009.11.02
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include "iss_city_unit.h"
#include "field/field_const.h" // for FX32_TO_GRID
#include "arc_def.h"           // for ARCID_ISS_CITY
#include "../../../resource/fldmapdata/zonetable/zone_id.h" 


//=========================================================================================
// ���萔
//=========================================================================================
//#define DEBUG_PRINT_ON        // �f�o�b�O�o�̓X�C�b�`
#define PRINT_DEST       (1)  // �f�o�b�O���̏o�͐�
#define VOLUME_SPACE_NUM (6)  // 1�̊XISS���j�b�g�������ʋ�Ԃ̐�

#define ABS(n) ((n)<0 ? -(n) : (n))  // ��Βl�}�N��


//=========================================================================================
// ���XISS���j�b�g �f�[�^
//=========================================================================================
typedef struct
{
	// �z�u�ꏊ�̃]�[��ID
	u16 zoneID;
  u16 padding;

	// ���W[grid]
	int gx;
	int gy;
	int gz;

	// ���ʋ��
  u8 volume[ VOLUME_SPACE_NUM ];  // [0, 127]
  u8 xRange[ VOLUME_SPACE_NUM ];  // [grid]
  u8 yRange[ VOLUME_SPACE_NUM ];  // [grid]
  u8 zRange[ VOLUME_SPACE_NUM ];  // [grid]
  
} ISS_C_UNIT_DATA;


//=========================================================================================
// ���XISS���j�b�g
//=========================================================================================
struct _ISS_C_UNIT
{
	// �z�u�ꏊ�̃]�[��ID
	u16 zoneID;

	// ���W[world]
	int x;
	int y;
	int z;

	// ���ʋ��
  u8  volume[ VOLUME_SPACE_NUM ];  // [0,127]
  u16 xRange[ VOLUME_SPACE_NUM ];  // [world]
  u16 yRange[ VOLUME_SPACE_NUM ];  // [world]
  u16 zRange[ VOLUME_SPACE_NUM ];  // [world]
};


//=========================================================================================
// ������J�֐�
//=========================================================================================
// ����
static void SetupUnit( const ISS_C_UNIT_DATA* unitData, ISS_C_UNIT* unit );
// �{�����[���Z�o
static u8 GetVolume          ( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 GetVolumeX         ( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 GetVolumeY         ( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 GetVolumeZ         ( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 SearchBasePriorityX( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 SearchBasePriorityY( const ISS_C_UNIT* unit, const VecFx32* playerPos );
static u8 SearchBasePriorityZ( const ISS_C_UNIT* unit, const VecFx32* playerPos ); 
static int CalcLerp( int startVal, int endVal, int startPos, int endPos, int calcPos );
// �f�o�b�O
static void DebugPrint_unitParam( const ISS_C_UNIT* unit );


//=========================================================================================
// ���쐬/�j��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @breif ���j�b�g���쐬����
 *
 * @param heapID �g�p����q�[�vID
 * @param datID  �ǂݍ��ރ��j�b�g�f�[�^�̃A�[�J�C�u���C���f�b�N�X
 */
//-----------------------------------------------------------------------------------------
ISS_C_UNIT* ISS_C_UNIT_Create( HEAPID heapID, ARCDATID datID )
{
  ISS_C_UNIT_DATA* unitData;
  ISS_C_UNIT* unit;

  // ����
  unit     = GFL_HEAP_AllocMemory( heapID, sizeof(ISS_C_UNIT) );
  unitData = GFL_ARC_LoadDataAllocOfs( ARCID_ISS_CITY, datID, heapID, 0, sizeof(ISS_C_UNIT_DATA) );
  SetupUnit( unitData, unit );
  GFL_HEAP_FreeMemory( unitData );

  // DEBUG:
#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C-UNIT: create\n" );
  DebugPrint_unitParam( unit );
#endif

  return unit; 
}

//-----------------------------------------------------------------------------------------
/**
 * @breif ���j�b�g��j������
 *
 * @param unit �j�����郆�j�b�g
 */
//-----------------------------------------------------------------------------------------
void ISS_C_UNIT_Delete( ISS_C_UNIT* unit )
{
  GFL_HEAP_FreeMemory( unit );

  // DEBUG:
#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C-UNIT: delete\n" );
#endif
}


//=========================================================================================
// ���擾
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �]�[��ID���擾����
 *
 * @param unit
 *
 * @return �w�肵�����j�b�g�̃]�[��ID
 */
//-----------------------------------------------------------------------------------------
u16 ISS_C_UNIT_GetZoneID( const ISS_C_UNIT* unit )
{
  return unit->zoneID;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���ʂ��擾����
 *
 * @param unit
 * @param pos  �v���C���[���W
 *
 * @return �w�肵���v���C���[���W�ɂ����鉹��[0, 127]
 */
//-----------------------------------------------------------------------------------------
int ISS_C_UNIT_GetVolume( const ISS_C_UNIT* unit, const VecFx32* pos )
{
  return GetVolume( unit, pos );
}


//=========================================================================================
// ������J�֐�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�I�u�W�F�N�g���쐬����
 *
 * @param unitData ���j�b�g�f�[�^
 * @param unit     �ϊ������f�[�^�̊i�[��
 */
//-----------------------------------------------------------------------------------------
static void SetupUnit( const ISS_C_UNIT_DATA* unitData, ISS_C_UNIT* unit )
{
  int i;

  // �]�[��ID
  unit->zoneID = unitData->zoneID;

  // ���W ( �O���b�h���烏�[���h�֕ϊ� )
  unit->x = unitData->gx * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_SIZE / 2;
  unit->y = unitData->gy * FIELD_CONST_GRID_SIZE;
  unit->z = unitData->gz * FIELD_CONST_GRID_SIZE + FIELD_CONST_GRID_SIZE / 2;

  // ���ʋ�� ( �O���b�h���烏�[���h�֕ϊ� )
  for( i=0; i<VOLUME_SPACE_NUM; i++ )
  {
    unit->volume[i] = unitData->volume[i];
    unit->xRange[i] = unitData->xRange[i] * FIELD_CONST_GRID_SIZE;
    unit->yRange[i] = unitData->yRange[i] * FIELD_CONST_GRID_SIZE;
    unit->zRange[i] = unitData->zRange[i] * FIELD_CONST_GRID_SIZE;
  } 
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �{�����[�����擾����
 *
 * @param unit
 * @param playerPos �v���C���[���W
 *
 * @return �w�肵�����W�ɂ�����{�����[��
 */
//-----------------------------------------------------------------------------------------
static u8 GetVolume( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  u8 volume[3];
  u8 vol;

  volume[0] = GetVolumeX( unit, playerPos );
  volume[1] = GetVolumeY( unit, playerPos );
  volume[2] = GetVolumeZ( unit, playerPos );

  vol = (volume[0] * volume[1] * volume[2]) / (127 * 127);
  return vol;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����W�ɂ�����{�����[�����擾����
 *
 * @param unit
 * @param playerPos
 *
 * @return �w�肵���v���C���[���W�ł̃{�����[��
 */
//-----------------------------------------------------------------------------------------
static u8 GetVolumeX( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int i;
  u8 basePri, destPri;
  u8 volume;

  basePri = SearchBasePriorityX( unit, playerPos );

  if( basePri == 0 ){ destPri = 0; }
  else{ destPri = basePri - 1; }

  if( basePri == destPri )
  {
    return unit->volume[ basePri ];
  }
  else
  {
    int dist, playerX;
    int startVal, endVal;
    int startPos, endPos;

    playerX = FX_Whole( playerPos->x );
    dist    = ABS( playerX - unit->x );

    startVal = unit->volume[ basePri ];
    endVal   = unit->volume[ destPri ];
    startPos = unit->xRange[ basePri ];
    endPos   = unit->xRange[ destPri ];

    return CalcLerp( startPos, startVal, endPos, endVal, dist );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����W�ɂ�����{�����[�����擾����
 *
 * @param unit
 * @param playerPos
 *
 * @return �w�肵���v���C���[���W�ł̃{�����[��
 */
//-----------------------------------------------------------------------------------------
static u8 GetVolumeY( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int i;
  u8 basePri, destPri;
  u8 volume;

  basePri = SearchBasePriorityY( unit, playerPos );

  if( basePri == 0 ){ destPri = 0; }
  else{ destPri = basePri - 1; }

  if( basePri == destPri )
  {
    return unit->volume[ basePri ];
  }
  else
  {
    int dist, playerY;
    int startVal, endVal;
    int startPos, endPos;

    playerY = FX_Whole( playerPos->y );
    dist    = ABS( playerY - unit->y );

    startVal = unit->volume[ basePri ];
    endVal   = unit->volume[ destPri ];
    startPos = unit->yRange[ basePri ];
    endPos   = unit->yRange[ destPri ];

    return CalcLerp( startPos, startVal, endPos, endVal, dist );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����W�ɂ�����{�����[�����擾����
 *
 * @param unit
 * @param playerPos
 *
 * @return �w�肵���v���C���[���W�ł̃{�����[��
 */
//-----------------------------------------------------------------------------------------
static u8 GetVolumeZ( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int i;
  u8 basePri, destPri;
  u8 volume;

  basePri = SearchBasePriorityZ( unit, playerPos );

  if( basePri == 0 ){ destPri = 0; }
  else{ destPri = basePri - 1; }

  if( basePri == destPri )
  {
    return unit->volume[ basePri ];
  }
  else
  {
    int dist, playerZ;
    int startVal, endVal;
    int startPos, endPos;

    playerZ = FX_Whole( playerPos->z );
    dist    = ABS( playerZ - unit->z );

    startVal = unit->volume[ basePri ];
    endVal   = unit->volume[ destPri ];
    startPos = unit->zRange[ basePri ];
    endPos   = unit->zRange[ destPri ];

    return CalcLerp( startPos, startVal, endPos, endVal, dist );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @breif ��Ԃ̊�ƂȂ�v���C�I���e�B�����肷��
 *
 * @param unit
 * @param playerPos �v���C���[���W
 *
 * @return �w�肵�����W�ł̕�Ԃ̊�ƂȂ�v���C�I���e�B
 */
//-----------------------------------------------------------------------------------------
static u8 SearchBasePriorityX( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int spaceIdx;
  int dist, playerX;

  // ���������߂�
  playerX = FX_Whole( playerPos->x );
  dist    = ABS( playerX - unit->x );

  // ����
  for( spaceIdx=0; spaceIdx < VOLUME_SPACE_NUM; spaceIdx++ )
  {
    if( unit->xRange[ spaceIdx ] <= dist )
    {
      return spaceIdx;
    }
  }
  GF_ASSERT(0);
  return 0;
}
//-----------------------------------------------------------------------------------------
/**
 * @breif ��Ԃ̊�ƂȂ�v���C�I���e�B�����肷��
 *
 * @param unit
 * @param playerPos �v���C���[���W
 *
 * @return �w�肵�����W�ł̕�Ԃ̊�ƂȂ�v���C�I���e�B
 */
//-----------------------------------------------------------------------------------------
static u8 SearchBasePriorityY( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int spaceIdx;
  int dist, playerY;

  // ���������߂�
  playerY = FX_Whole( playerPos->y );
  dist    = ABS( playerY - unit->y );

  // ����
  for( spaceIdx=0; spaceIdx < VOLUME_SPACE_NUM; spaceIdx++ )
  {
    if( unit->yRange[ spaceIdx ] <= dist )
    {
      return spaceIdx;
    }
  }
  GF_ASSERT(0);
  return 0;
}
//-----------------------------------------------------------------------------------------
/**
 * @breif ��Ԃ̊�ƂȂ�v���C�I���e�B�����肷��
 *
 * @param unit
 * @param playerPos �v���C���[���W
 *
 * @return �w�肵�����W�ł̕�Ԃ̊�ƂȂ�v���C�I���e�B
 */
//-----------------------------------------------------------------------------------------
static u8 SearchBasePriorityZ( const ISS_C_UNIT* unit, const VecFx32* playerPos )
{
  int spaceIdx;
  int dist, playerZ;

  // ���������߂�
  playerZ = FX_Whole( playerPos->z );
  dist    = ABS( playerZ - unit->z );

  // ����
  for( spaceIdx=0; spaceIdx < VOLUME_SPACE_NUM; spaceIdx++ )
  {
    if( unit->zRange[ spaceIdx ] <= dist )
    {
      return spaceIdx;
    }
  } 
  GF_ASSERT(0);
  return 0;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief ���`���
 *
 * @param startPos ��ԓ_1
 * @param startVal ��ԓ_1�̒l
 * @param endPos   ��ԓ_2
 * @param endVal   ��ԓ_2�̒l
 * @param calcPos  ��ԓ_3
 *
 * @return ��ԓ_3�̒l
 */
//-----------------------------------------------------------------------------------------
static int CalcLerp( int startPos, int startVal, int endPos, int endVal, int calcPos )
{
  int x1 = startPos;
  int y1 = startVal;
  int x2 = endPos;
  int y2 = endVal;
  int x3 = calcPos;
  //float t  = (float)(x3 - x1) / (float)(x2 - x1);
  //int y3 = (y2 - y1) * t + y1;
  int y3 = (y2 - y1) * (x3 - x1) / (x2 - x1) + y1;

  return y3;
}


#ifdef DEBUG_PRINT_ON
//-----------------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�����o�͂���
 *
 * @param unit �o�͑Ώۃ��j�b�g
 */
//-----------------------------------------------------------------------------------------
static void DebugPrint_unitParam( const ISS_C_UNIT* unit )
{
  int spaceIdx;

  OS_TFPrintf( PRINT_DEST, "ISS-C-UNIT: zoneID = %d\n", unit->zoneID );
  OS_TFPrintf( PRINT_DEST, "ISS-C-UNIT: pos = (%d, %d, %d)\n", unit->x, unit->y, unit->z );

  for( spaceIdx=0; spaceIdx < VOLUME_SPACE_NUM; spaceIdx++ )
  {
    OS_TFPrintf( PRINT_DEST, 
                 "ISS-C-UNIT: volume, range = %d, (%d, %d, %d)\n",
                 unit->volume[ spaceIdx ], 
                 unit->xRange[ spaceIdx ], unit->yRange[ spaceIdx ], unit->zRange[ spaceIdx ] );
  }
}
#endif
