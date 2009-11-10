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
static void AdjustPosition( SOUNDOBJ* sobj );


//========================================================================================
/**
 * @brief �����I�u�W�F�N�g
 */
//========================================================================================
struct _SOUNDOBJ
{
  HEAPID             heapID;  // �g�p����q�[�vID
  FIELDMAP_WORK*   fieldmap;  // �t�B�[���h�}�b�v
  ISS_3DS_SYS*    iss3dsSys;  // �o�^��3D�T�E���h�V�X�e��
  u8          iss3dsUnitIdx;  // ���g��3D�T�E���h���j�b�g�̊Ǘ��C���f�b�N�X
  ICA_ANIME*       icaAnime;  // �A�j���[�V�����f�[�^
  GFL_G3D_OBJSTATUS* status;  // ����ΏۃX�e�[�^�X
};


//=========================================================================================
// ���쐬�E�j��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g���쐬����
 *
 * @param fieldmap �\����t�B�[���h�}�b�v
 * @param iss_sys  �o�^��3D�T�E���h�V�X�e��
 * @param status   ����ΏۃX�e�[�^�X
 */
//-----------------------------------------------------------------------------------------
SOUNDOBJ* SOUNDOBJ_Create( 
    FIELDMAP_WORK* fieldmap, ISS_3DS_SYS* iss_sys, GFL_G3D_OBJSTATUS* status )
{
  SOUNDOBJ* sobj;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );

  // �C���X�^���X����
  sobj = (SOUNDOBJ*)GFL_HEAP_AllocMemory( heap_id, sizeof(SOUNDOBJ) );

  // �C���X�^���X������
  sobj->heapID        = heap_id;
  sobj->fieldmap      = fieldmap;
  sobj->iss3dsSys     = iss_sys;
  sobj->iss3dsUnitIdx = ISS_3DS_SYS_AddUnit( iss_sys );
  sobj->icaAnime      = NULL;
  sobj->status        = status;
  return sobj;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g��j������
 *
 * @param sobj �j������I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_Delete( SOUNDOBJ* sobj )
{
  // 3D�T�E���h���j�b�g��j��
  ISS_3DS_SYS_DeleteUnit( sobj->iss3dsSys, sobj->iss3dsUnitIdx );

  // �A�j���[�V�����f�[�^��j��
  if( sobj->icaAnime )
  {
    ICA_ANIME_Delete( sobj->icaAnime );
  }

  // �C���X�^���X��j��
  GFL_HEAP_FreeMemory( sobj );
}


//=========================================================================================
// ���������֐�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V������o�^����
 *
 * @param arc_id       �ݒ肷��A�j���[�V�����f�[�^�̃A�[�J�C�uID
 * @param dat_id       �ݒ肷��A�j���[�V�����f�[�^�̃A�[�J�C�u���f�[�^ID
 * @param buf_interval �X�g���[�~���O�Ԋu
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_SetAnime( SOUNDOBJ* sobj, ARCID arc_id, ARCDATID dat_id, int buf_interval )
{
  // �o�^�ς݃f�[�^��j��
  if( sobj->icaAnime )
  {
    ICA_ANIME_Delete( sobj->icaAnime );
  }

  // �A�j���[�V�����쐬
  sobj->icaAnime = ICA_ANIME_CreateStreamingAlloc( 
                             sobj->heapID, arc_id, dat_id, buf_interval );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief 3D�T�E���h���j�b�g�̐ݒ���t�@�C�����烍�[�h����
 *
 * @param arc_id       �ݒ肷��f�[�^�̃A�[�J�C�uID
 * @param dat_id       �ݒ肷��f�[�^�̃A�[�J�C�u���f�[�^ID
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_Set3DSUnitStatus( SOUNDOBJ* sobj, ARCID arc_id, ARCDATID dat_id )
{
  ISS_3DS_UNIT* unit = ISS_3DS_SYS_GetUnit( sobj->iss3dsSys, sobj->iss3dsUnitIdx );
  ISS_3DS_UNIT_Load( unit, arc_id, dat_id, sobj->heapID );
}


//=========================================================================================
// ������Ǘ�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V������i�߂�
 *
 * @param sobj  �X�V�ΏۃI�u�W�F�N�g
 * @param frame �i�߂�t���[����
 *
 * @return ���[�v������ TRUE
 */
//-----------------------------------------------------------------------------------------
BOOL SOUNDOBJ_IncAnimeFrame( SOUNDOBJ* sobj, fx32 frame )
{
  BOOL loop = FALSE;

  // �A�j���[�V�������o�^�Ȃ�, �������Ȃ�
  if( sobj->icaAnime == NULL )
  {
    return FALSE;
  }

  // �A�j���[�V�������X�V
  loop = ICA_ANIME_IncAnimeFrame( sobj->icaAnime, frame );

  // �ʒu���X�V
  AdjustPosition( sobj );

  // ���[�v�������ǂ�����Ԃ�
  return loop;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����t���[������ݒ肷��
 *
 * @param sobj  �X�V�ΏۃI�u�W�F�N�g
 * @param frame �t���[�������w��
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_SetAnimeFrame( SOUNDOBJ* sobj, fx32 frame )
{
  // �A�j���[�V�������o�^�Ȃ�, �������Ȃ�
  if( sobj->icaAnime == NULL )
  {
    return;
  }

  // �A�j���[�V�������X�V
  ICA_ANIME_SetAnimeFrame( sobj->icaAnime, frame );

  // �ʒu���X�V
  AdjustPosition( sobj );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����t���[�������擾����
 *
 * @param sobj �擾�ΏۃI�u�W�F�N�g
 *
 * @return ���݂̃A�j���[�V�����t���[����
 */
//-----------------------------------------------------------------------------------------
u32 SOUNDOBJ_GetAnimeFrame( SOUNDOBJ* sobj ) 
{
  // �A�j���[�V�������o�^�Ȃ�, �������Ȃ�
  if( sobj->icaAnime == NULL )
  {
    return 0;
  }

  // ���݂̃A�j���[�V�����t���[������Ԃ�
  return ICA_ANIME_GetNowFrame( sobj->icaAnime );
}


//=========================================================================================
// ���擾
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�����삷��g���b�N�̃r�b�g�}�X�N���擾����
 *
 * @param sobj �擾�ΏۃI�u�W�F�N�g
 *
 * @return �w�胆�j�b�g�����삷��g���b�N�r�b�g�}�X�N
 */
//-----------------------------------------------------------------------------------------
u16 SOUNDOBJ_GetTrackBit( const SOUNDOBJ* sobj )
{
  ISS_3DS_UNIT* unit;
  u16 track_bit;

  // 3D�T�E���h���j�b�g���擾
  unit = ISS_3DS_SYS_GetUnit( sobj->iss3dsSys, sobj->iss3dsUnitIdx );

  // �g���b�N�}�X�N���擾
  track_bit = ISS_3DS_UNIT_GetTrackBit( unit );
  return track_bit;
}


//=========================================================================================
// ������J�֐��̒�`
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �g���I�u�W�F�E�����I�u�W�F�̈ʒu���A�j���[�V�����ɍ��킹��
 *
 * @param sobj �X�V����I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------------------
static void AdjustPosition( SOUNDOBJ* sobj )
{
  VecFx32 pos;
  ISS_3DS_UNIT* unit;

  // �ʒu���擾
  ICA_ANIME_GetTranslate( sobj->icaAnime, &pos );

  // 3D�X�e�[�^�X��ݒ�
  VEC_Set( &sobj->status->trans, pos.x, pos.y, pos.z );

  // �����ʒu�����킹��
  unit = ISS_3DS_SYS_GetUnit( sobj->iss3dsSys, sobj->iss3dsUnitIdx );
  ISS_3DS_UNIT_SetPos( unit, &pos );
}
