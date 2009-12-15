//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �|�P�Z���񕜃A�j���[�V�����C�x���g
 * @file   event_pc_recovery.c
 * @author obata_toshihiro
 * @date   2009.09.18
 */
//////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "event_pc_recovery.h"
#include "pc_recovery_anime.naix"
#include "sound/pm_sndsys.h"
#include "field_buildmodel.h"
#include "../../../resource/fldmapdata/build_model/output/buildmodel_indoor.naix"
#include "field/field_const.h"  // for FIELD_CONST_GRID_SIZE
#include "field_sound.h"  // for FIELD_SOUND_xxxx


//========================================================================================
// ���萔
//========================================================================================
#define BALLSET_WAIT     (15) // �{�[���Z�b�g�Ԋu
#define ANIME_START_WAIT (10) // �{�[���Z�b�g��̉񕜃A�j���[�V�����J�n�҂�����
#define PCMACHINE_SEARCH_RANGE (FIELD_CONST_GRID_SIZE * 5)  // PC�}�V�������͈�

// �C�x���g�E�V�[�P���X
typedef enum {
  SEQ_BALLSET,      // �{�[���ݒu
  SEQ_WAIT,         // �҂�
  SEQ_RECOV_ANIME,  // �񕜃A�j���[�V����
  SEQ_EXIT,         // �C�x���g�I��
} SEQID;

// �{�[���ԍ�
typedef enum {
  BALL_1,
  BALL_2,
  BALL_3,
  BALL_4,
  BALL_5,
  BALL_6,
  BALL_NUM
} BALL_INDEX;

// �{�[���ݒu���W (�񕜃}�V������̃I�t�Z�b�g)
static const VecFx32 ball_offset[BALL_NUM] = 
{
  {  -4*FX32_ONE,   14*FX32_ONE,  -3*FX32_ONE},
  {   4*FX32_ONE,   14*FX32_ONE,  -3*FX32_ONE},
  {   9*FX32_ONE,   14*FX32_ONE,   4*FX32_ONE},
  {   4*FX32_ONE,   14*FX32_ONE,   8*FX32_ONE},
  {  -4*FX32_ONE,   14*FX32_ONE,   8*FX32_ONE},
  {  -9*FX32_ONE,   14*FX32_ONE,   4*FX32_ONE},
};

//========================================================================================
// ���C�x���g���[�N
//========================================================================================
typedef struct
{
  HEAPID                  heapID;  // �q�[�vID
  FIELDMAP_WORK*        fieldmap;  // �t�B�[���h�}�b�v
  VecFx32             machinePos;  // �񕜃}�V���̍��W
  u8                  pokemonNum;  // �莝���|�P�����̐�
  u8                  setBallNum;  // �}�V���ɃZ�b�g�����{�[���̐�
  u16                   seqCount;  // �V�[�P���X����t���[���J�E���^
  FIELD_BMODEL_MAN*        bmMan;  // �z�u���f���}�l�[�W��
  FIELD_BMODEL* ballBM[BALL_NUM];  // �{�[���̔z�u���f��
  G3DMAPOBJST*      machineObjSt;  // �񕜃}�V���z�u���f��

} RECOVERY_WORK;


//========================================================================================
// ���v���g�^�C�v�錾
//========================================================================================
static GMEVENT_RESULT EVENT_FUNC_PcRecoveryAnime( GMEVENT* event, int* seq, void* work );
static void SetupEvent( 
    RECOVERY_WORK* work, GAMESYS_WORK* gsys, u8 pokemon_num );
static void ExitEvent( RECOVERY_WORK* work );
static void ChangeSequence( RECOVERY_WORK* work, int* seq, SEQID next_seq );
static void SetMonsterBall( RECOVERY_WORK* work );
static void StartRecoveryAnime( RECOVERY_WORK* work );
static void StopRecoveryAnime( RECOVERY_WORK* work );
static BOOL IsRecoveryAnimeEnd( RECOVERY_WORK* work );


//========================================================================================
// ���O�����J�֐�
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �|�P�Z���񕜃A�j���[�V�����C�x���g���쐬����
 *
 * @param gsys        �Q�[���V�X�e��
 * @param parent      �e�C�x���g
 * @param pokemon_num �莝���|�P�����̐�
 *
 * @return �쐬�����񕜃A�j���C�x���g
 */
//----------------------------------------------------------------------------------------
GMEVENT* EVENT_PcRecoveryAnime( 
    GAMESYS_WORK* gsys, GMEVENT* parent, u8 pokemon_num )
{
  GMEVENT* event;
  RECOVERY_WORK* work;

  // �C�x���g�쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_PcRecoveryAnime, sizeof(RECOVERY_WORK) );

  // �C�x���g���[�N������
  work = (RECOVERY_WORK*)GMEVENT_GetEventWork( event );
  SetupEvent( work, gsys, pokemon_num ); 
  return event;
}


//========================================================================================
// ������J�֐�
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �C�x���g����֐�
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X
 * @param work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//----------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_PcRecoveryAnime( GMEVENT* event, int* seq, void* work )
{
  RECOVERY_WORK* rw = (RECOVERY_WORK*)work;

  switch( *seq )
  {
  case SEQ_BALLSET: // �{�[���Z�b�g
    // ���^�C�~���O�Ń{�[�����Z�b�g
    if( rw->seqCount % BALLSET_WAIT == 0 )
    {
      SetMonsterBall( rw );

      // �S�{�[�����Z�b�g����
      if( rw->pokemonNum <= rw->setBallNum )
      {
        ChangeSequence( rw, seq, SEQ_WAIT );
      }
    }
    break;
  case SEQ_WAIT:  // �{�[���Z�b�g��̑҂�
    if( ANIME_START_WAIT < rw->seqCount )
    {
        StartRecoveryAnime( rw );
        ChangeSequence( rw, seq, SEQ_RECOV_ANIME );
    }
    break;
  case SEQ_RECOV_ANIME: // �A�j���[�V�����Đ�
    if( IsRecoveryAnimeEnd( rw ) )
    {
        StopRecoveryAnime( rw );
        ChangeSequence( rw, seq, SEQ_EXIT );
    }
    break;
  case SEQ_EXIT:  // �C�x���g�I��
    ExitEvent( rw );
    return GMEVENT_RES_FINISH;
  } 
  rw->seqCount++;
  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------------------
/**
 * @breif �C�x���g����������
 *
 * @param work        �C�x���g���[�N
 * @param gsys        �Q�[���V�X�e��
 * @param pokemon_num �莝���|�P�����̐�
 */
//----------------------------------------------------------------------------------------
static void SetupEvent( 
    RECOVERY_WORK* work, GAMESYS_WORK* gsys, u8 pokemon_num )
{
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );
  FLDMAPPER*   g3d_mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );

  // �C�x���g���[�N������
  work->heapID       = heap_id;
  work->fieldmap     = fieldmap;
  work->pokemonNum   = pokemon_num;
  work->setBallNum   = 0;
  work->bmMan        = FLDMAPPER_GetBuildModelManager( g3d_mapper );
  work->machineObjSt = NULL;

  // �O�̂��ߗ�O����
  if( BALL_NUM < work->pokemonNum ) work->pokemonNum = BALL_NUM;

  // �񕜃}�V���̍��W���擾
  {
    FIELD_PLAYER* player;
    VecFx32 pos;
    G3DMAPOBJST** objst;
    u32 objnum;
    FLDHIT_RECT rect;
    player = FIELDMAP_GetFieldPlayer( work->fieldmap );
    FIELD_PLAYER_GetPos( player, &pos );
    rect.top    = pos.z - (PCMACHINE_SEARCH_RANGE << FX32_SHIFT);
    rect.bottom = pos.z + (PCMACHINE_SEARCH_RANGE << FX32_SHIFT);
    rect.left   = pos.x - (PCMACHINE_SEARCH_RANGE << FX32_SHIFT);
    rect.right  = pos.x + (PCMACHINE_SEARCH_RANGE << FX32_SHIFT);
    objst = FIELD_BMODEL_MAN_CreateObjStatusList( work->bmMan, &rect, 
                                                  BM_SEARCH_ID_PCMACHINE, &objnum );
    if( objst )
    {
      work->machineObjSt = objst[0];
      G3DMAPOBJST_getPos( objst[0], &work->machinePos );
    }
    GFL_HEAP_FreeMemory( objst ); 
  }

}

//----------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�I������
 *
 * @param work �C�x���g���[�N
 */
//----------------------------------------------------------------------------------------
static void ExitEvent( RECOVERY_WORK* work )
{
  int i;

  // �{�[���̔z�u���f����j��
  { 
    // �쐬�������f�����폜
    for( i=0; i<work->pokemonNum; i++ )
    {
      FIELD_BMODEL_Delete( work->ballBM[i] );
    }
  }
}

//----------------------------------------------------------------------------------------
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work     �C�x���g���[�N
 * @param seq      �C�x���g�V�[�P���X���[�N
 * @param next_seq ���̃V�[�P���X
 */
//----------------------------------------------------------------------------------------
static void ChangeSequence( RECOVERY_WORK* work, int* seq, SEQID next_seq )
{
  // �V�[�P���X���X�V��, �J�E���^������
  *seq = next_seq;
  work->seqCount = 0;
}

//----------------------------------------------------------------------------------------
/**
 * @brief �����X�^�[�{�[�����񕜃}�V���ɃZ�b�g����
 *
 * @param work  �C�x���g���[�N
 */
//----------------------------------------------------------------------------------------
static void SetMonsterBall( RECOVERY_WORK* work )
{
  int index;  // �Z�b�g����{�[���ԍ�

  // ���łɑS�{�[���̃Z�b�g�������ς݂Ȃ�, �������Ȃ�
  if( work->pokemonNum <= work->setBallNum ) return;

  // �z�u���f����o�^
  index = work->setBallNum; 
  //FIELD_BMODEL_MAN_EntryBuildModel( work->bmMan, work->ballBM[index] );
  {
      BMODEL_ID bmodel_id = NARC_output_buildmodel_indoor_mball01_nsbmd;
      GFL_G3D_OBJSTATUS status;
      VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
      GFL_CALC3D_MTX_CreateRot( 0, 0, 0, &status.rotate );
      VEC_Add( &work->machinePos, &ball_offset[index], &status.trans );
      work->ballBM[index] = FIELD_BMODEL_Create_Direct( work->bmMan, bmodel_id, &status );
  }

  // �Z�b�g�����{�[���̐����C���N�������g
  work->setBallNum++;

  // SE�Đ�
  PMSND_PlaySE( SEQ_SE_BOWA1 ); 
}

//----------------------------------------------------------------------------------------
/**
 * @brief �񕜃A�j���[�V�����J�n����
 *
 * @param work �C�x���g���[�N
 */
//----------------------------------------------------------------------------------------
static void StartRecoveryAnime( RECOVERY_WORK* work )
{
  int i;

  // �{�[��
  for( i=0; i<work->setBallNum; i++ )
  {
    FIELD_BMODEL_SetAnime( work->ballBM[i], 0, BMANM_REQ_START );
  }

  // �}�V��
  if( work->machineObjSt )
  {
    G3DMAPOBJST_setAnime( work->bmMan, work->machineObjSt, 0, BMANM_REQ_START );
  }

  // ME�Đ�
  {
    GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
    GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
    FIELD_SOUND*  fsnd = GAMEDATA_GetFieldSound( gdata );
    FIELD_SOUND_PushPlayJingleBGM( fsnd, SEQ_ME_ASA );
  }
}

//----------------------------------------------------------------------------------------
/**
 * @brief �񕜃A�j���[�V�����I������
 *
 * @param work �C�x���g���[�N
 */
//----------------------------------------------------------------------------------------
static void StopRecoveryAnime( RECOVERY_WORK* work )
{
  int i;

  // �{�[���̃A�j���[�V�����I��
  for( i=0; i<work->setBallNum; i++ )
  {
    FIELD_BMODEL_SetAnime( work->ballBM[i], 0, BMANM_REQ_STOP );
  }

  // BGM���A
  {
    GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
    GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
    FIELD_SOUND*  fsnd = GAMEDATA_GetFieldSound( gdata );
    FIELD_SOUND_PopBGM( fsnd );
  }
}

//----------------------------------------------------------------------------------------
/**
 * @brief �񕜃A�j���[�V�����I���`�F�b�N
 *
 * @param work �C�x���g���[�N
 *
 * @param �A�j���[�V�����I������ TRUE
 */
//----------------------------------------------------------------------------------------
static BOOL IsRecoveryAnimeEnd( RECOVERY_WORK* work )
{
  int i;

  // �S�{�[���̃A�j���[�V�������I�����Ă��邩�ǂ���
  for( i=0; i<work->setBallNum; i++ )
  {
    BOOL stop = FIELD_BMODEL_GetAnimeStatus( work->ballBM[i], 0 );
    if( stop != TRUE )
    {
      return FALSE;
    }
  }
  return TRUE;
} 
