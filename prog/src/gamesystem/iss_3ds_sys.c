/////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief 3D�T�E���h�V�X�e��
 * @file iss_3d_sys.h
 * @author obata
 * @date 2009.09.08
 * 
 */
/////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_3ds_sys.h"
#include "sound/pm_sndsys.h"


//===============================================================================
/**
 * @brief �V�X�e�����[�N
 */
//===============================================================================
struct _ISS_3DS_SYS
{
  HEAPID                heapID;  // �q�[�vID
  u8                maxUnitNum;  // ���j�b�g�ő吔
  u8                   unitNum;  // ���j�b�g�ێ���
  ISS_3DS_UNIT**          unit;  // ���j�b�g�z��
  const GFL_G3D_CAMERA* camera;  // �J����
};


//===============================================================================
// �v���g�^�C�v�錾
//===============================================================================
static void InitSys( ISS_3DS_SYS* system, 
    HEAPID heap_id, u8 max_unit_num, const GFL_G3D_CAMERA* camera );
static void UpdateVolume( ISS_3DS_SYS* system, u8 index );
static void UpdatePan( ISS_3DS_SYS* system, u8 index );


//===============================================================================
// �쐬�E�j���E����
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����쐬����
 *
 * @param heap_id      �g�p����q�[�vID
 * @param max_unit_num ���j�b�g�ő吔
 * @param camera       �g�p����J����
 *
 * @return �쐬�����V�X�e��
 */
//-------------------------------------------------------------------------------
ISS_3DS_SYS* ISS_3DS_SYS_Create( 
    HEAPID heap_id, u8 max_unit_num, const GFL_G3D_CAMERA* camera )
{
  ISS_3DS_SYS* system;

  // �V�X�e�����[�N�쐬
  system = GFL_HEAP_AllocMemory( heap_id, sizeof(ISS_3DS_SYS) );

  // �V�X�e�����[�N������
  InitSys( system, heap_id, max_unit_num, camera );

  return system;
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e����j������
 *
 * @param system �j������V�X�e��
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_Delete( ISS_3DS_SYS* system )
{
  int i;

  // �S���j�b�g�j��
  for( i=0; i<system->maxUnitNum; i++ )
  {
    ISS_3DS_SYS_DeleteUnit( system, i );
  } 
  GFL_HEAP_FreeMemory( system->unit );

  // �V�X�e���j��
  GFL_HEAP_FreeMemory( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̓��쏈��
 *
 * @param system �������V�X�e��
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_Main( ISS_3DS_SYS* system )
{
  int i;

  // �e���j�b�g��K�p
  for( i=0; i<system->maxUnitNum; i++ )
  {
    UpdateVolume( system, i );
    UpdatePan( system, i );
  }
}


//===============================================================================
// ���j�b�g�̒ǉ��E�폜�E���o��
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ���j�b�g��ǉ�����
 * 
 * @param system     �ǉ���V�X�e��
 *
 * @return �ǉ��������j�b�g�̊Ǘ��C���f�b�N�X
 */
//-------------------------------------------------------------------------------
u8 ISS_3DS_SYS_AddUnit( ISS_3DS_SYS* system )
{
  u8 index;

  // �󂫗v�f������
  for( index=0; index<system->maxUnitNum; index++ )
  {
    if( system->unit[index] == NULL ) break;
  }

  // �󂫂��Ȃ��ꍇ
  if( system->maxUnitNum <= index )
  {
    OBATA_Printf( "-----------------------------------\n" );
    OBATA_Printf( "ERROR : ISS_3DS_SYS unit over flow\n" );
    OBATA_Printf( "-----------------------------------\n" );
    return 0;
  }

  // ���j�b�g���쐬
  system->unit[index] = ISS_3DS_UNIT_Create( system->heapID );
  system->unitNum++;

  // �쐬�������j�b�g�̃C���f�b�N�X��Ԃ�
  return index;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���j�b�g��j������
 *
 * @param system ���j�b�g���o�^����Ă���V�X�e��
 * @param index  �폜�Ώۃ��j�b�g�̊Ǘ��C���f�b�N�X
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_DeleteUnit( ISS_3DS_SYS* system, u8 index )
{
  // �w��C���f�b�N�X�Ƀ��j�b�g���o�^����Ă�����폜����
  if( system->unit[index] != NULL )
  {
    ISS_3DS_UNIT_Delete( system->unit[index] );
    system->unitNum--;
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �w�胆�j�b�g�����o��
 *
 * @param system ���j�b�g���o�^����Ă���V�X�e��
 * @param index  ���o���Ώۃ��j�b�g�̊Ǘ��C���f�b�N�X
 *
 * @return �w�肳�ꂽ�Ǘ��ԍ��̃��j�b�g
 */
//-------------------------------------------------------------------------------
ISS_3DS_UNIT* ISS_3DS_SYS_GetUnit( ISS_3DS_SYS* system, u8 index )
{
  return system->unit[index];
}


//===============================================================================
// ����J�֐�
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����[�N������������
 *
 * @param system       ����������V�X�e�����[�N
 * @param heap_id      �g�p����q�[�vID
 * @param max_unit_num �ő僆�j�b�g��
 * @param camera       �g�p����J����
 */
//-------------------------------------------------------------------------------
static void InitSys( ISS_3DS_SYS* system, 
    HEAPID heap_id, u8 max_unit_num, const GFL_G3D_CAMERA* camera )
{
  int i;

  system->heapID     = heap_id;
  system->maxUnitNum = max_unit_num;
  system->unitNum    = 0;
  system->unit       = GFL_HEAP_AllocMemory( heap_id, sizeof(ISS_3DS_UNIT*) * max_unit_num ); 
  system->camera     = camera;

  // ���j�b�g�z�񏉊���
  for( i=0; i<max_unit_num; i++ )
  {
    system->unit[i] = NULL;
  }
} 

//-------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�K�p����( ���� )
 *
 * @param system �V�X�e��
 * @param index  �K�p���郆�j�b�g�̊Ǘ��ԍ�
 */
//-------------------------------------------------------------------------------
static void UpdateVolume( ISS_3DS_SYS* system, u8 index )
{
  u16 track;
  ISS_3DS_UNIT*  unit = system->unit[index];
  const GFL_G3D_CAMERA* cam = system->camera;

  // �w��ԍ��Ƀ��j�b�g���o�^����ĂȂ��ꍇ
  if( unit == NULL ) return;

  // �g���b�N�}�X�N���擾
  track = ISS_3DS_UNIT_GetTrackBit( unit );

  // �{�����[�����v�Z
  {
    VecFx32 cam_pos, unit_pos, vec_to_unit;
    fx32 dist, range;
    int volume;

    // �������v�Z
    GFL_G3D_CAMERA_GetPos( cam, &cam_pos ); 
    ISS_3DS_UNIT_GetPos( unit, &unit_pos );
    VEC_Subtract( &unit_pos, &cam_pos, &vec_to_unit );
    dist  = VEC_Mag( &vec_to_unit );
    range = ISS_3DS_UNIT_GetRange( unit );

    // ���ʌ���
    if( range < dist )  // �����͂��Ȃ������Ȃ�[��
    {
      volume = 0;
    }
    else                // �����͂������Ȃ���`���
    {
      int   max   = ISS_3DS_UNIT_GetMaxVolume( unit );
      float rate  = FX_FX32_TO_F32( FX_Div( range - dist, range ) ); 
      volume      = max * rate;
    }

    // ���ʐݒ�
    PMSND_ChangeBGMVolume( track, volume );
  } 
}

//-------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�K�p����( �p�� )
 *
 * @param system �V�X�e��
 * @param index  �K�p���郆�j�b�g�̊Ǘ��ԍ�
 */
//-------------------------------------------------------------------------------
static void UpdatePan( ISS_3DS_SYS* system, u8 index )
{
  u16 track;
  ISS_3DS_UNIT*  unit = system->unit[index];
  const GFL_G3D_CAMERA* cam = system->camera;

  // �w��ԍ��Ƀ��j�b�g���o�^����ĂȂ��ꍇ
  if( unit == NULL ) return;

  // �g���b�N�}�X�N���擾
  track = ISS_3DS_UNIT_GetTrackBit( unit );

  // �p�����v�Z
  {
    VecFx32 v0, v1, v2, v3, vup, vp;
    VecFx32 cx, cy, cz;
    VecFx32 cam_pos, target, unit_pos;
    int pan;
    fx32 dot;

    // �J�����E�����_�E�������ꂼ��̈ʒu
    GFL_G3D_CAMERA_GetPos( cam, &cam_pos );
    GFL_G3D_CAMERA_GetTarget( cam, &target );
    ISS_3DS_UNIT_GetPos( unit, &unit_pos );

    // �J�����̌����x�N�g��v0
    VEC_Subtract( &target, &cam_pos, &v0 );

    // �J�����������x�N�g��v1
    VEC_Subtract( &unit_pos, &cam_pos, &v1 );

    // �J��������x�N�g��vup
    GFL_G3D_CAMERA_GetCamUp( cam, &vup );

    // �J�������W��cx, cy, cz
    VEC_Normalize( &v0, &cz );
    VEC_CrossProduct( &cz, &vup, &cx );   VEC_Normalize( &cx, &cx );
    VEC_CrossProduct( &cx, &cz, &cy );    VEC_Normalize( &cy, &cy );

    // v1���J�������W�n��xz���ʂ֎ˉe�����x�N�g��v2
    VEC_Normalize( &cy, &cy );
    dot = VEC_DotProduct( &v1, &cy );
    VEC_Set( &vp, FX_Mul(cy.x, dot), FX_Mul(cy.y, dot), FX_Mul(cy.z, dot) );
    VEC_Subtract( &v1, &vp, &v2 );  // v1��cy����������
    VEC_Normalize( &v2, &v2 );

    // �p�� = cx�Ev2
    dot = VEC_DotProduct( &cx, &v2 );
    pan = 128 * FX_FX32_TO_F32(dot);

    // �p���̒l��␳
    if( pan < -128 ) pan = -128;
    if( 127 < pan  ) pan =  127;

    // �p����ݒ�
    PMSND_SetStatusBGM_EX( track, 256, 0, pan );
    OBATA_Printf( "pan = %d\n", pan );
  }
}
