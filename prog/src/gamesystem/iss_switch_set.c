/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ISS �X�C�b�`�Z�b�g
 * @file   iss_switch_set.c
 * @author obata
 * @date   2010.03.24
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "iss_switch.h"
#include "iss_switch_set.h"

#include "arc/arc_def.h" // for ARCID_ISS_SWITCH 


//===============================================================================
// ���f�o�b�O���
//===============================================================================
#define DEBUG_MODE       // �f�o�b�O�X�C�b�`
#define PRINT_TARGET (1) // �f�o�b�O���̏o�͐�

#ifdef DEBUG_MODE
static u8 Debug_switchSetNum = 0;
#endif


//===============================================================================
// ���萔
//===============================================================================
#define MAX_VALID_ZONE_NUM (9) // �X�C�b�`��Ԃ��ێ�����]�[���̍ő吔


//===============================================================================
// ��ISS�X�C�b�`�Z�b�g
//===============================================================================
struct _ISS_SWITCH_SET
{ 
  u32 soundIdx;                        // �V�[�P���X�ԍ�
  u16 validZone[ MAX_VALID_ZONE_NUM ]; // �X�C�b�`���L���ȏꏊ�z��
  u8  validZoneNum;                    // �X�C�b�`���L���ȏꏊ�̐�
  ISS_SWITCH* IssSwitch[ SWITCH_NUM ]; // �X�C�b�`

#ifdef DEBUG_MODE
  u8 debugID; // ���ʎq
#endif
};


//===============================================================================
// ���֐��C���f�b�N�X
//===============================================================================
static void InitSwitchSet( ISS_SWITCH_SET* set ); // �X�C�b�`�Z�b�g������������
static ISS_SWITCH_SET* CreateSwitchSet( HEAPID heapID ); // �X�C�b�`�Z�b�g�𐶐�����
static void DeleteSwitchSet( ISS_SWITCH_SET* set ); // �X�C�b�`�Z�b�g��j������
static void CreateSwitches( ISS_SWITCH_SET* set, HEAPID heapID ); // �X�C�b�`�𐶐�����
static void DeleteSwitches( ISS_SWITCH_SET* set ); // �X�C�b�`��j������
static void LoadSwitchData( ISS_SWITCH_SET* set, ARCDATID datID, HEAPID heapID ); // �X�C�b�`�f�[�^��ǂݍ���
static u32 GetSoundIdx( const ISS_SWITCH_SET* set ); // �X�C�b�`�ɑΉ�����V�[�P���X�ԍ����擾����
static u8 GetValidZoneNum( const ISS_SWITCH_SET* set ); // �X�C�b�`���L���ȏꏊ�̐����擾����
static u16 GetValidZoneID( const ISS_SWITCH_SET* set, int idx ); // �X�C�b�`���L���ȏꏊ���擾����
static BOOL CheckValidZone( const ISS_SWITCH_SET* set, u16 zoneID ); // �X�C�b�`���L���ȏꏊ���ǂ����𔻒肷��
static BOOL CheckSwitchOn( const ISS_SWITCH_SET* set, SWITCH_INDEX idx ); // �X�C�b�`��������Ă��邩�ǂ����𔻒肷��
static ISS_SWITCH* GetSwitch( const ISS_SWITCH_SET* set, SWITCH_INDEX idx ); // �X�C�b�`���擾����
static void SetBGMVolume( const ISS_SWITCH_SET* set ); // �X�C�b�`�Z�b�g�̏�Ԃ�BGM�{�����[���ɔ��f����
static void SwitchSetMain( ISS_SWITCH_SET* set ); // �X�C�b�`�Z�b�g�̃��C������
static void SwitchOn( ISS_SWITCH_SET* set, SWITCH_INDEX swIdx ); // �X�C�b�`������
static void SwitchOff( ISS_SWITCH_SET* set, SWITCH_INDEX swIdx ); // �X�C�b�`�𗣂�
static void ResetSwitch( ISS_SWITCH_SET* set ); // �X�C�b�`�����Z�b�g����

#ifdef DEBUG_MODE
static void Debug_PrintSwitchSet( const ISS_SWITCH_SET* set ); // �X�C�b�`�̏�Ԃ��o�͂���
#endif



//===============================================================================
// ��public method
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�𐶐�����
 *
 * @param heapID
 *
 * @return ���������X�C�b�`�Z�b�g
 */
//-------------------------------------------------------------------------------
ISS_SWITCH_SET* ISS_SWITCH_SET_Create( HEAPID heapID )
{
  ISS_SWITCH_SET* set;

  set = CreateSwitchSet( heapID ); // �X�C�b�`�Z�b�g�𐶐�
  InitSwitchSet( set ); // �X�C�b�`�Z�b�g��������
  CreateSwitches( set, heapID ); // �X�C�b�`�𐶐�
  return set;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g��j������
 *
 * @param set
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SET_Delete( ISS_SWITCH_SET* set )
{
  GF_ASSERT( set );

  DeleteSwitches( set ); // �X�C�b�`��j��
  DeleteSwitchSet( set ); // �X�C�b�`�Z�b�g��j��
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�f�[�^�����[�h����
 *
 * @param set
 * @param datID  �ǂݍ��ރf�[�^�̃A�[�J�C�u���C���f�b�N�X
 * @param heapID �g�p����q�[�vID
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SET_Load( ISS_SWITCH_SET* set, ARCDATID datID, HEAPID heapID )
{
  GF_ASSERT( set );

  LoadSwitchData( set, datID, heapID );

#ifdef DEBUG_MODE
  Debug_PrintSwitchSet( set );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�̃��C������
 *
 * @param set
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SET_Main( ISS_SWITCH_SET* set )
{
  GF_ASSERT( set );

  SwitchSetMain( set );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g������
 *
 * @param set
 * @param swIdx �����X�C�b�`���w��
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SET_SwitchOn( ISS_SWITCH_SET* set, SWITCH_INDEX swIdx )
{
  GF_ASSERT( set );

  SwitchOn( set, swIdx );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�𗣂�
 *
 * @param set
 * @param swIdx �����X�C�b�`���w��
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SET_SwitchOff( ISS_SWITCH_SET* set, SWITCH_INDEX swIdx )
{
  GF_ASSERT( set );

  SwitchOff( set, swIdx );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�����Z�b�g����
 *
 * @param set
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SET_ResetSwitch( ISS_SWITCH_SET* set )
{
  GF_ASSERT( set );

  ResetSwitch( set );
}

//-------------------------------------------------------------------------------
/**
 * @brief �Ή�����V�[�P���X�ԍ����擾����
 *
 * @param set
 *
 * @return �V�[�P���X�ԍ�
 */
//-------------------------------------------------------------------------------
u32 ISS_SWITCH_SET_GetSoundIdx( const ISS_SWITCH_SET* set )
{
  GF_ASSERT( set );

  return GetSoundIdx( set );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g���L���ȏꏊ���ǂ����𔻒肷��
 *
 * @param set
 * @param zoneID ���肷��ꏊ
 *
 * @return �w�肵���]�[����, �X�C�b�`�Z�b�g���L���ȏꏊ�Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
BOOL ISS_SWITCH_SET_IsValidAt( const ISS_SWITCH_SET* set, u16 zoneID )
{ 
  GF_ASSERT( set );

  return CheckValidZone( set, zoneID );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`��������Ă��邩�ǂ����𔻒肷��
 *
 * @param set
 *
 * @return �X�C�b�`�Z�b�g��������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
BOOL ISS_SWITCH_SET_CheckSwitchOn( const ISS_SWITCH_SET* set, SWITCH_INDEX idx )
{
  GF_ASSERT( set );

  return CheckSwitchOn( set, idx );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�̏�Ԃ�BGM�{�����[���ɔ��f������
 *
 * @param set
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SET_SetBGMVolume( const ISS_SWITCH_SET* set )
{
  GF_ASSERT( set );

  SetBGMVolume( set );
}


//===============================================================================
// ��private method
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g������������
 *
 * @param set
 */
//-------------------------------------------------------------------------------
static void InitSwitchSet( ISS_SWITCH_SET* set )
{
  int i;

  set->soundIdx = 0;
  set->validZoneNum = 0; 
  for( i=0; i<MAX_VALID_ZONE_NUM; i++ ) { set->validZone[i] = 0; } 
  for( i=0; i<SWITCH_NUM; i++ ) { set->IssSwitch[i] = NULL; } 

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, 
      "ISS-SWITCH-SET[%d]: init switch-set\n", set->debugID );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�𐶐�����
 *
 * @param heapID �g�p����q�[�vID
 *
 * @return ���������X�C�b�`�Z�b�g
 */
//-------------------------------------------------------------------------------
static ISS_SWITCH_SET* CreateSwitchSet( HEAPID heapID )
{
  ISS_SWITCH_SET* set;

  // �X�C�b�`�Z�b�g�𐶐�
  set = GFL_HEAP_AllocMemory( heapID, sizeof(ISS_SWITCH_SET) ); 

#ifdef DEBUG_MODE
  set->debugID = Debug_switchSetNum++;
#endif

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, 
      "ISS-SWITCH-SET[%d]: create switch-set\n", set->debugID );
#endif

  return set;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g��j������
 *
 * @param set
 */
//-------------------------------------------------------------------------------
static void DeleteSwitchSet( ISS_SWITCH_SET* set )
{
#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, 
      "ISS-SWITCH-SET[%d]: delete switch-set\n", set->debugID );
#endif

  GFL_HEAP_FreeMemory( set ); 
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�𐶐�����
 *
 * @param set
 * @param heapID
 */
//-------------------------------------------------------------------------------
static void CreateSwitches( ISS_SWITCH_SET* set, HEAPID heapID )
{
  int i;

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, 
      "ISS-SWITCH-SET[%d]: ��create switches\n", set->debugID );
#endif

  for( i=0; i<SWITCH_NUM; i++ )
  {
    GF_ASSERT( set->IssSwitch[i] == NULL );

    set->IssSwitch[i] = ISS_SWITCH_Create( heapID );
  }

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, 
      "ISS-SWITCH-SET[%d]: ��create switches\n", set->debugID );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`��j������
 *
 * @param set
 */
//-------------------------------------------------------------------------------
static void DeleteSwitches( ISS_SWITCH_SET* set )
{
  int i;

  for( i=0; i<SWITCH_NUM; i++ )
  {
    ISS_SWITCH_Delete( set->IssSwitch[i] );
    set->IssSwitch[i] = NULL;
  }

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, 
      "ISS-SWITCH-SET[%d]: delete switches\n", set->debugID );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�f�[�^��ǂݍ���
 *
 * @param set
 * @param datID  �ǂݍ��ރf�[�^ID
 * @param heapID �g�p����q�[�vID
 */
//-------------------------------------------------------------------------------
static void LoadSwitchData( ISS_SWITCH_SET* set, ARCDATID datID, HEAPID heapID )
{ 
  int i;
  int offset = 0;
  int size = 0;
  u16 trackBit[ SWITCH_NUM ];
  u16 fadeFrame;

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, 
      "��ISS-SWITCH-SET[%d]: load switch data(%d)\n", set->debugID, datID );
#endif

  // �V�[�P���X�ԍ�
  size = sizeof(u32);
  GFL_ARC_LoadDataOfs( 
      &(set->soundIdx), ARCID_ISS_SWITCH, datID, offset, size );
  offset += size;

  // �e�X�C�b�`�̑���Ώۃg���b�N�r�b�g
  for( i=0; i<SWITCH_NUM; i++ )
  {
    size = sizeof(u16);
    GFL_ARC_LoadDataOfs( &trackBit[i], ARCID_ISS_SWITCH, datID, offset, size );
    offset += size;
  }

  // �t�F�[�h����
  size = sizeof(u16);
  GFL_ARC_LoadDataOfs( &fadeFrame, ARCID_ISS_SWITCH, datID, offset, size );
  offset += size;

  // �X�C�b�`���L���ȏꏊ�̐�
  size = sizeof(u8);
  GFL_ARC_LoadDataOfs( 
      &(set->validZoneNum), ARCID_ISS_SWITCH, datID, offset, size );
  offset += size;

  // �X�C�b�`���L���ȏꏊ
  size = sizeof(u16) * set->validZoneNum;
  GFL_ARC_LoadDataOfs( 
      &(set->validZone), ARCID_ISS_SWITCH, datID, offset, size );
  offset += size;

  // �e�X�C�b�`�̃p�����[�^��ݒ�
  for( i=0; i<SWITCH_NUM; i++ )
  {
    ISS_SWITCH_SetSwitchParam( GetSwitch(set, i), fadeFrame, trackBit[i] );
  }

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, 
      "��ISS-SWITCH-SET[%d]: load switch data(%d)\n", set->debugID, datID );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�̃��C������
 *
 * @param set
 */
//-------------------------------------------------------------------------------
static void SwitchSetMain( ISS_SWITCH_SET* set )
{
  int i;
  ISS_SWITCH* sw;

  for( i=0; i<SWITCH_NUM; i++ )
  {
    sw = GetSwitch( set, i );
    ISS_SWITCH_Main( sw );
  }
} 

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`������
 *
 * @param set
 * @param swIdx �����X�C�b�`���w��
 */
//-------------------------------------------------------------------------------
static void SwitchOn( ISS_SWITCH_SET* set, SWITCH_INDEX swIdx )
{
  ISS_SWITCH* sw;

  GF_ASSERT( swIdx != SWITCH_00 );
  GF_ASSERT( swIdx <= SWITCH_MAX );

  sw = GetSwitch( set, swIdx );
  ISS_SWITCH_On( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�𗣂�
 *
 * @param set
 * @param swIdx �����X�C�b�`���w��
 */
//-------------------------------------------------------------------------------
static void SwitchOff( ISS_SWITCH_SET* set, SWITCH_INDEX swIdx )
{
  ISS_SWITCH* sw;

  GF_ASSERT( swIdx != SWITCH_00 );
  GF_ASSERT( swIdx <= SWITCH_MAX );

  sw = GetSwitch( set, swIdx );
  ISS_SWITCH_Off( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�����Z�b�g���� ( �X�C�b�`0�݂̂�������Ă����Ԃɂ��� )
 *
 * @param set
 */
//-------------------------------------------------------------------------------
static void ResetSwitch( ISS_SWITCH_SET* set )
{
  int i;
  ISS_SWITCH* sw;

  // �X�C�b�`0��ON�ɂ���
  sw = GetSwitch( set, SWITCH_00 );
  ISS_SWITCH_ForceOn( sw );

  // ����ȊO�̃X�C�b�`��OFF�ɂ���
  for( i=SWITCH_01; i<SWITCH_NUM; i++ )
  {
    sw = GetSwitch( set, i ); 
    ISS_SWITCH_ForceOff( sw );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�ɑΉ�����V�[�P���X�ԍ����擾����
 *
 * @param switchSst
 *
 * @return �w�肵���X�C�b�`�ɑΉ�����V�[�P���X�ԍ�
 */
//-------------------------------------------------------------------------------
static u32 GetSoundIdx( const ISS_SWITCH_SET* switchSst )
{
  return switchSst->soundIdx;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`���L���ȏꏊ�̐����擾����
 *
 * @param set
 *
 * @return �w�肵���X�C�b�`���L���ȏꏊ�̐�
 */
//-------------------------------------------------------------------------------
static u8 GetValidZoneNum( const ISS_SWITCH_SET* set )
{
  return set->validZoneNum;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`���L���ȏꏊ���擾����
 *
 * @param set
 * @param idx       �擾����ꏊ�̃C���f�b�N�X
 *
 * @return �w�肵���C���f�b�N�X�ɂ���, �X�C�b�`�L���]�[����ID
 */
//-------------------------------------------------------------------------------
static u16 GetValidZoneID( const ISS_SWITCH_SET* set, int idx )
{
  GF_ASSERT( 0 <= idx );
  GF_ASSERT( idx < GetValidZoneNum(set) );

  return set->validZone[ idx ];
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`���L���ȏꏊ���ǂ����𔻒肷��
 *
 * @param set
 * @param zoneID
 *
 * @return �w�肵���]�[����, �X�C�b�`���L���ȏꏊ�Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL CheckValidZone( const ISS_SWITCH_SET* set, u16 zoneID )
{
  int i;
  int zoneNum;

  zoneNum = GetValidZoneNum( set );

  for( i=0; i < zoneNum; i++ )
  {
    if( GetValidZoneID(set, i) == zoneID ) { return TRUE; }
  } 
  return FALSE;
} 

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`��������Ă��邩�ǂ����𔻒肷��
 *
 * @param set
 * @param idx ���肷��X�C�b�`���w��
 *
 * @return �w�肵���X�C�b�`��������Ă���ꍇ, TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL CheckSwitchOn( const ISS_SWITCH_SET* set, SWITCH_INDEX idx )
{
  ISS_SWITCH* sw;

  GF_ASSERT( idx <= SWITCH_MAX );

  sw = GetSwitch( set, idx );
  return ISS_SWITCH_IsOn( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`���擾����
 *
 * @param set
 * @param switchIdx �X�C�b�`�ԍ�
 *
 * @return �w�肵���C���f�b�N�X�̃X�C�b�`
 */
//-------------------------------------------------------------------------------
static ISS_SWITCH* GetSwitch( const ISS_SWITCH_SET* set, SWITCH_INDEX switchIdx )
{
  GF_ASSERT( switchIdx <= SWITCH_MAX );

  return set->IssSwitch[ switchIdx ];
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�̏�Ԃ�BGM�{�����[���ɔ��f����
 *
 * @param set
 */
//-------------------------------------------------------------------------------
static void SetBGMVolume( const ISS_SWITCH_SET* set )
{
  int i;
  ISS_SWITCH* sw;

  // �e�X�C�b�`�̏�Ԃ�BGM�{�����[���ɔ��f
  for( i=SWITCH_00; i<SWITCH_NUM; i++ )
  {
    sw = GetSwitch( set, i ); 
    ISS_SWITCH_SetBGMVolume( sw, 127 );
  }
}

#ifdef DEBUG_MODE
//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�̏�Ԃ��o�͂���
 *
 * @param set
 */
//-------------------------------------------------------------------------------
static void Debug_PrintSwitchSet( const ISS_SWITCH_SET* set )
{
  int i;

  // BGM�ԍ�
  OS_TFPrintf( PRINT_TARGET, 
      "ISS-SWITCH-SET[%d]: soundIdx = %d\n", set->debugID, set->soundIdx );

  // �L���]�[���̐�
  OS_TFPrintf( PRINT_TARGET, 
      "ISS-SWITCH-SET[%d]: validZoneNum = %d\n", set->debugID, set->validZoneNum );

  // �L���]�[��
  for( i=0; i<set->validZoneNum; i++ )
  {
    OS_TFPrintf( PRINT_TARGET, 
        "ISS-SWITCH-SET[%d]: validZone[%d] = %d\n", 
        set->debugID, i, set->validZone[i] );
  }
}
#endif
