#include <gflib.h>
#include "fieldmap.h"
#include "gamesystem/iss_3ds_sys.h"
#include "sound_obj.h"

#include "gamesystem/iss_3ds_unit.h"
#include "system/ica_anime.h"
#include "h03.naix"


//=========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//=========================================================================================
static void AdjustPosition( SOUNDOBJ* soundObj );
static void AdjustDirection( SOUNDOBJ* soundObj );


//========================================================================================
// �����I�u�W�F�N�g
//========================================================================================
struct _SOUNDOBJ
{
  HEAPID             heapID;
  FIELDMAP_WORK*     fieldmap;
  ISS_3DS_SYS*       iss3dsSystem;
  ISS3DS_UNIT_INDEX  iss3dsUnitIdx;  // ���g��3D�T�E���h���j�b�g�̊Ǘ��C���f�b�N�X
  ICA_ANIME*         icaAnime;       // �A�j���[�V�����f�[�^
  GFL_G3D_OBJSTATUS* g3dObjStatus;   // ����ΏۃX�e�[�^�X
};


//=========================================================================================
// ���쐬�E�j��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g���쐬����
 *
 * @param fieldmap
 * @param g3dObjStatus   ����Ώ�3D�I�u�W�F�N�g�̃X�e�[�^�X
 * @param iss3dsUnitIdx  �o�^����3D�T�E���h���j�b�g���w��
 * @param effectiveRange �����͂�����
 * @param maxVolume      �ő�{�����[��
 */
//-----------------------------------------------------------------------------------------
SOUNDOBJ* SOUNDOBJ_Create( FIELDMAP_WORK* fieldmap, GFL_G3D_OBJSTATUS* g3dObjStatus,
                           ISS3DS_UNIT_INDEX iss3dsUnitIdx, fx32 effectiveRange, int maxVolume )
{
  SOUNDOBJ* soundObj;
  HEAPID heapID;
  GAMESYS_WORK* gameSystem;
  ISS_SYS* issSystem;
  ISS_3DS_SYS* iss3dsSystem;

  heapID       = FIELDMAP_GetHeapID( fieldmap );
  gameSystem   = FIELDMAP_GetGameSysWork( fieldmap );
  issSystem    = GAMESYSTEM_GetIssSystem( gameSystem );
  iss3dsSystem = ISS_SYS_GetIss3DSSystem( issSystem );

  // ����
  soundObj = (SOUNDOBJ*)GFL_HEAP_AllocMemory( heapID, sizeof(SOUNDOBJ) );

  // ������
  soundObj->heapID        = heapID;
  soundObj->fieldmap      = fieldmap;
  soundObj->iss3dsSystem  = iss3dsSystem;
  soundObj->iss3dsUnitIdx = iss3dsUnitIdx;
  soundObj->icaAnime      = NULL;
  soundObj->g3dObjStatus  = g3dObjStatus;

  // 3D�T�E���h���j�b�g�o�^
  if( ISS_3DS_SYS_IsUnitRegistered( iss3dsSystem, iss3dsUnitIdx ) == FALSE )
  {
    ISS_3DS_SYS_RegisterUnit( iss3dsSystem, iss3dsUnitIdx, effectiveRange, maxVolume );
  }

  return soundObj;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �_�~�[�����I�u�W�F�N�g���쐬����
 *
 * @param fieldmap
 * @param g3dObjStatus ����Ώ�3D�I�u�W�F�N�g�̃X�e�[�^�X
 *
 * ��BGM�̑�����s��Ȃ������I�u�W�F�N�g�𐶐�����B
 */
//-----------------------------------------------------------------------------------------
SOUNDOBJ* SOUNDOBJ_CreateDummy( FIELDMAP_WORK* fieldmap, GFL_G3D_OBJSTATUS* g3dObjStatus )
{
  SOUNDOBJ* soundObj;
  HEAPID heapID;

  heapID = FIELDMAP_GetHeapID( fieldmap );

  // ����
  soundObj = (SOUNDOBJ*)GFL_HEAP_AllocMemory( heapID, sizeof(SOUNDOBJ) );

  // ������
  soundObj->heapID       = heapID;
  soundObj->fieldmap     = fieldmap;
  soundObj->iss3dsSystem = NULL;
  soundObj->icaAnime     = NULL;
  soundObj->g3dObjStatus = g3dObjStatus;

  return soundObj;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g��j������
 *
 * @param soundObj
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_Delete( SOUNDOBJ* soundObj )
{
  // �A�j���[�V�����f�[�^�j��
  if( soundObj->icaAnime )
  {
    ICA_ANIME_Delete( soundObj->icaAnime );
  }

  // �{�̂�j��
  GFL_HEAP_FreeMemory( soundObj );
}


//=========================================================================================
// ���������֐�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V������o�^����
 *
 * @param arcID       �ݒ肷��A�j���[�V�����f�[�^�̃A�[�J�C�uID
 * @param datID       �ݒ肷��A�j���[�V�����f�[�^�̃A�[�J�C�u���f�[�^ID
 * @param bufInterval �X�g���[�~���O�Ԋu
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_SetAnime( SOUNDOBJ* soundObj, ARCID arcID, ARCDATID datID, int bufInterval )
{
  // �o�^�ς݃f�[�^��j��
  if( soundObj->icaAnime )
  {
    ICA_ANIME_Delete( soundObj->icaAnime );
  }

  // �A�j���[�V�����쐬
  soundObj->icaAnime = ICA_ANIME_CreateStreamingAlloc( soundObj->heapID, arcID, datID, bufInterval );
}


//=========================================================================================
// ������Ǘ�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V������i�߂�
 *
 * @param soundObj
 * @param frame    �i�߂�t���[����
 *
 * @return ���[�v������ TRUE
 */
//-----------------------------------------------------------------------------------------
BOOL SOUNDOBJ_IncAnimeFrame( SOUNDOBJ* soundObj, fx32 frame )
{
  BOOL loop = FALSE;

  // �A�j���[�V�������o�^
  if( soundObj->icaAnime == NULL ){ return FALSE; }

  // �A�j���[�V�������X�V
  loop = ICA_ANIME_IncAnimeFrame( soundObj->icaAnime, frame );

  // �ʒu���X�V
  AdjustPosition( soundObj );
  AdjustDirection( soundObj );

  // ���[�v�������ǂ�����Ԃ�
  return loop;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����t���[������ݒ肷��
 *
 * @param soundObj
 * @param frame    �ݒ肷��t���[�������w��
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_SetAnimeFrame( SOUNDOBJ* soundObj, fx32 frame )
{
  // �A�j���[�V�������o�^
  if( soundObj->icaAnime == NULL ){ return; }

  // �A�j���[�V�������X�V
  ICA_ANIME_SetAnimeFrame( soundObj->icaAnime, frame );

  // �ʒu���X�V
  AdjustPosition( soundObj );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����t���[�������擾����
 *
 * @param soundObj
 *
 * @return ���݂̃A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------------------
fx32 SOUNDOBJ_GetAnimeFrame( SOUNDOBJ* soundObj ) 
{
  // �A�j���[�V�������o�^
  if( soundObj->icaAnime == NULL ){ return 0; }

  // ���݂̃A�j���[�V�����t���[������Ԃ�
  return ICA_ANIME_GetNowFrame( soundObj->icaAnime );
}


//=========================================================================================
// ������J�֐�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �g���I�u�W�F�E�����I�u�W�F�̈ʒu���A�j���[�V�����ɍ��킹��
 *
 * @param soundObj �X�V����I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------------------
static void AdjustPosition( SOUNDOBJ* soundObj )
{
  VecFx32 pos;
  ISS_3DS_UNIT* unit;

  // �A�j���[�V�������o�^
  if( soundObj->icaAnime == NULL ){ return; }

  // �ʒu���擾
  if( ICA_ANIME_GetTranslate( soundObj->icaAnime, &pos ) != TRUE ) { return; }

  // 3D�X�e�[�^�X��ݒ�
  VEC_Set( &soundObj->g3dObjStatus->trans, pos.x, pos.y, pos.z );

  // �����ʒu�����킹��
  if( soundObj->iss3dsSystem )
  {
    ISS_3DS_SYS_SetUnitPos( soundObj->iss3dsSystem, soundObj->iss3dsUnitIdx, &pos );
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �g���I�u�W�F�̌������A�j���[�V�����ɍ��킹��
 *
 * @param soundObj �X�V����I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------------------
static void AdjustDirection( SOUNDOBJ* soundObj )
{
  VecFx32 vec;

  // �A�j���[�V�������o�^
  if( soundObj->icaAnime == NULL ){ return; }

  // ��]�p�x���擾
  if( ICA_ANIME_GetRotate( soundObj->icaAnime, &vec ) != TRUE ) { return; }

  // 3D�X�e�[�^�X��ݒ�
  {
    float x, y, z;
    u16 rx, ry, rz;
    x = FX_FX32_TO_F32( vec.x );
    y = FX_FX32_TO_F32( vec.y );
    z = FX_FX32_TO_F32( vec.z );
    while( x < 0 ) x += 360.0f;
    while( y < 0 ) y += 360.0f;
    while( z < 0 ) z += 360.0f;
    rx = x / 360.0f * 0xffff;
    ry = y / 360.0f * 0xffff;
    rz = z / 360.0f * 0xffff; 
    MTX_Identity33( &soundObj->g3dObjStatus->rotate );
    GFL_CALC3D_MTX_CreateRot( rx, ry, rz, &soundObj->g3dObjStatus->rotate );
  }
}
