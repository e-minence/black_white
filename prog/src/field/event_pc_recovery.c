////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �|�P�Z���񕜃A�j���[�V�����C�x���g
 * @file   event_pc_recovery.c
 * @author obata_toshihiro
 * @date   2009.09.18
 */
//////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "event_pc_recovery.h"
#include "pc_recovery_anime.naix"
#include "field_sound.h"
#include "sound/pm_sndsys.h"


//==============================================================================
// �萔
//==============================================================================
//#define PC_RECOVERY_DEBUG_ON  // �f�o�b�O�o�̗͂L��
#define MAX_BALL_NUM (6)  // �ő�莝���|�P������
#define BALLSET_WAIT (15) // �{�[���Z�b�g�҂�����
#define ANIME_START_WAIT (10) // �{�[���Z�b�g��̉񕜃A�j���[�V�����J�n�҂�����

// �C�x���g�E�V�[�P���X
typedef enum
{
  SEQ_BALLSET,      // �{�[���ݒu
  SEQ_WAIT,         // �҂�
  SEQ_RECOV_ANIME,  // �񕜃A�j���[�V����
  SEQ_EXIT,         // �C�x���g�I��
} SEQID;

//------------------
// �g���I�u�W�F�N�g
//------------------
// ���j�b�g�ԍ�
#define EXOBJUNITID (1)

// �I�u�W�F�N�g�ԍ�
typedef enum
{
  EXOBJ_BALL_0,
  EXOBJ_BALL_1,
  EXOBJ_BALL_2,
  EXOBJ_BALL_3,
  EXOBJ_BALL_4,
  EXOBJ_BALL_5,
  EXOBJ_NUM
} EXOBJID;

// ���\�[�X�e�[�u��
GFL_G3D_UTIL_RES res_table[] = 
{
  // �{�[���̃��f��
  {ARCID_PC_RECOVERY, NARC_pc_recovery_anime_pc_mb_nsbmd, GFL_G3D_UTIL_RESARC},
  {ARCID_PC_RECOVERY, NARC_pc_recovery_anime_pc_mb_nsbmd, GFL_G3D_UTIL_RESARC},
  {ARCID_PC_RECOVERY, NARC_pc_recovery_anime_pc_mb_nsbmd, GFL_G3D_UTIL_RESARC},
  {ARCID_PC_RECOVERY, NARC_pc_recovery_anime_pc_mb_nsbmd, GFL_G3D_UTIL_RESARC},
  {ARCID_PC_RECOVERY, NARC_pc_recovery_anime_pc_mb_nsbmd, GFL_G3D_UTIL_RESARC},
  {ARCID_PC_RECOVERY, NARC_pc_recovery_anime_pc_mb_nsbmd, GFL_G3D_UTIL_RESARC},
  // �{�[���̃A�j���[�V����
  {ARCID_PC_RECOVERY, NARC_pc_recovery_anime_pc_mb_nsbtp, GFL_G3D_UTIL_RESARC},
};
// �A�j���[�V�����e�[�u��
GFL_G3D_UTIL_ANM ball_anm_table[] = 
{
  {6, 0},
};
// �I�u�W�F�N�g�ݒ�e�[�u��
GFL_G3D_UTIL_OBJ obj_table[] = 
{
  {0, 0, 0, ball_anm_table, NELEMS(ball_anm_table)},
  {1, 0, 1, ball_anm_table, NELEMS(ball_anm_table)},
  {2, 0, 2, ball_anm_table, NELEMS(ball_anm_table)},
  {3, 0, 3, ball_anm_table, NELEMS(ball_anm_table)},
  {4, 0, 4, ball_anm_table, NELEMS(ball_anm_table)},
  {5, 0, 5, ball_anm_table, NELEMS(ball_anm_table)},
};
// 3D�Z�b�g�A�b�v�e�[�u��
GFL_G3D_UTIL_SETUP setup = 
{
  res_table, NELEMS(res_table), obj_table, NELEMS(obj_table)
};

// �{�[���ݒu���W (�񕜃}�V������̃I�t�Z�b�g)
VecFx32 ball_offset[] = 
{
  {  -4*FX32_ONE,   14*FX32_ONE,  -51*FX32_ONE},
  {   4*FX32_ONE,   14*FX32_ONE,  -51*FX32_ONE},
  {  -9*FX32_ONE,   14*FX32_ONE,  -44*FX32_ONE},
  {   9*FX32_ONE,   14*FX32_ONE,  -44*FX32_ONE},
  {  -4*FX32_ONE,   14*FX32_ONE,  -40*FX32_ONE},
  {   4*FX32_ONE,   14*FX32_ONE,  -40*FX32_ONE},
};
// �{�[���̃I�u�W�F�N�g�ԍ�
EXOBJID ball_obj_id[] = 
{
  EXOBJ_BALL_0,
  EXOBJ_BALL_1,
  EXOBJ_BALL_2,
  EXOBJ_BALL_3,
  EXOBJ_BALL_4,
  EXOBJ_BALL_5,
};

//==============================================================================
// �C�x���g���[�N
//==============================================================================
typedef struct
{
  HEAPID           heapID;  // �q�[�vID
  FIELDMAP_WORK* fieldmap;  // �t�B�[���h�}�b�v
  VecFx32      machinePos;  // �񕜃}�V���̍��W
  u8           pokemonNum;  // �莝���|�P�����̐�
  u8           setBallNum;  // �}�V���ɃZ�b�g�����{�[���̐�
  u16            seqCount;  // �V�[�P���X����t���[���J�E���^

} RECOVERY_WORK;


//==============================================================================
// �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT EVENT_FUNC_PcRecoveryAnime( GMEVENT* event, int* seq, void* work );
static void SetupEvent( 
    RECOVERY_WORK* work, GAMESYS_WORK* gsys, VecFx32* mechine_pos, u8 pokemon_num );
static void ExitEvent( RECOVERY_WORK* work );
static void ChangeSequence( RECOVERY_WORK* work, int* seq, SEQID next_seq );
static void SetMonsterBall( RECOVERY_WORK* work );
static void StartRecoveryAnime( RECOVERY_WORK* work );
static void StopRecoveryAnime( RECOVERY_WORK* work );
static BOOL IsRecoveryAnimeEnd( RECOVERY_WORK* work );
static void PlayRecoveryAnime( RECOVERY_WORK* work );


//------------------------------------------------------------------------------
/**
 * @brief �|�P�Z���񕜃A�j���[�V�����C�x���g���쐬����
 *
 * @param gsys        �Q�[���V�X�e��
 * @param parent      �e�C�x���g
 * @param machine_pos �񕜃}�V���̍��W
 * @param pokemon_num �莝���|�P�����̐�
 *
 * @return �쐬�����񕜃A�j���C�x���g
 */
//------------------------------------------------------------------------------
GMEVENT* EVENT_PcRecoveryAnime( 
    GAMESYS_WORK* gsys, GMEVENT* parent, VecFx32* machine_pos, u8 pokemon_num )
{
  GMEVENT* event;
  RECOVERY_WORK* work;

  // �C�x���g�쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_PcRecoveryAnime, sizeof(RECOVERY_WORK) );

  // �C�x���g���[�N������
  work = (RECOVERY_WORK*)GMEVENT_GetEventWork( event );
  SetupEvent( work, gsys, machine_pos, pokemon_num );

  return event;
}


//==============================================================================
/**
 * @brief �C�x���g����֐�
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X
 * @param work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//==============================================================================
static GMEVENT_RESULT EVENT_FUNC_PcRecoveryAnime( GMEVENT* event, int* seq, void* work )
{
  RECOVERY_WORK* rw = (RECOVERY_WORK*)work;

#ifdef PC_RECOVERY_DEBUG_ON
  switch( *seq )
  {
  case SEQ_BALLSET:     OBATA_Printf( "SEQ_BALLSET\n" );     break;
  case SEQ_WAIT:        OBATA_Printf( "SEQ_BALLSET\n" );     break;
  case SEQ_RECOV_ANIME: OBATA_Printf( "SEQ_RECOV_ANIME\n" ); break;
  case SEQ_EXIT:        OBATA_Printf( "SEQ_EXIT\n" );        break;
  }
#endif


  switch( *seq )
  {
  case SEQ_BALLSET:
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
  case SEQ_WAIT:
    if( ANIME_START_WAIT < rw->seqCount )
    {
        StartRecoveryAnime( rw );
        ChangeSequence( rw, seq, SEQ_RECOV_ANIME );
    }
    break;
  case SEQ_RECOV_ANIME:
    PlayRecoveryAnime( rw );
    if( IsRecoveryAnimeEnd( rw ) )
    {
        StopRecoveryAnime( rw );
        ChangeSequence( rw, seq, SEQ_EXIT );
    }
    break;
  case SEQ_EXIT:
    ExitEvent( rw );
    return GMEVENT_RES_FINISH;
  } 
  rw->seqCount++;
  return GMEVENT_RES_CONTINUE;
}


//==============================================================================
/**
 * @breif �C�x���g����������
 *
 * @param work        �C�x���g���[�N
 * @param gsys        �Q�[���V�X�e��
 * @param machine_pos �񕜃}�V���̍��W
 * @param pokemon_num �莝���|�P�����̐�
 */
//==============================================================================
static void SetupEvent( 
    RECOVERY_WORK* work, GAMESYS_WORK* gsys, VecFx32* machine_pos, u8 pokemon_num )
{
  FIELDMAP_WORK*        fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  HEAPID                 heap_id = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cont = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �C�x���g���[�N������
  work->heapID   = heap_id;
  work->fieldmap = fieldmap;
  VEC_Set( &work->machinePos, machine_pos->x, machine_pos->y, machine_pos->z );
  work->pokemonNum = 6;//pokemon_num;
  work->setBallNum = 0;

  // �g���I�u�W�F�N�g���j�b�g�ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cont, &setup, EXOBJUNITID );
}

//==============================================================================
/**
 * @brief �C�x���g�I������
 *
 * @param work �C�x���g���[�N
 */
//==============================================================================
static void ExitEvent( RECOVERY_WORK* work )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cont = FIELDMAP_GetExpObjCntPtr( work->fieldmap );

  // �g���I�u�W�F�N�g���j�b�g��j��
  FLD_EXP_OBJ_DelUnit( exobj_cont, EXOBJUNITID );
}

//==============================================================================
/**
 * @brief �V�[�P���X��ύX����
 *
 * @param work     �C�x���g���[�N
 * @param seq      �C�x���g�V�[�P���X���[�N
 * @param next_seq ���̃V�[�P���X
 */
//==============================================================================
static void ChangeSequence( RECOVERY_WORK* work, int* seq, SEQID next_seq )
{
  // �V�[�P���X���X�V��, �J�E���^������
  *seq = next_seq;
  work->seqCount = 0;
}

//==============================================================================
/**
 * @brief �����X�^�[�{�[�����񕜃}�V���ɃZ�b�g����
 *
 * @param work  �C�x���g���[�N
 */
//==============================================================================
static void SetMonsterBall( RECOVERY_WORK* work )
{
  int index;// �Z�b�g����{�[���ԍ�
  GFL_G3D_OBJSTATUS* status;
  FLD_EXP_OBJ_CNT_PTR exobj_cont;

  // ���łɑS�{�[���̃Z�b�g�������ς݂Ȃ�, �������Ȃ�
  if( work->pokemonNum <= work->setBallNum ) return;

  // �{�[�����Z�b�g
  index      = work->setBallNum; 
  exobj_cont = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
  status     = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cont, EXOBJUNITID, ball_obj_id[index] );
  VEC_Add( &work->machinePos, &ball_offset[index], &status->trans );
  VEC_Set( &status->scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status->rotate );

  // �Z�b�g�����{�[���̐����C���N�������g
  work->setBallNum++;

  // SE�Đ�
  PMSND_PlaySE( SEQ_SE_CANCEL2 );

#ifdef PC_RECOVERY_DEBUG_ON
  OBATA_Printf( "SetMonsterBall\n" );
#endif
}

//==============================================================================
/**
 * @brief �񕜃A�j���[�V�����J�n����
 *
 * @param work �C�x���g���[�N
 */
//==============================================================================
static void StartRecoveryAnime( RECOVERY_WORK* work )
{
  int i;

  // �{�[���̃A�j���[�V�����J�n
  for( i=0; i<work->setBallNum; i++ )
  {
    FLD_EXP_OBJ_CNT_PTR exobj_cont = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
    EXP_OBJ_ANM_CNT_PTR anm_cont = FLD_EXP_OBJ_GetAnmCnt( exobj_cont, EXOBJUNITID, i, 0 );
    FLD_EXP_OBJ_ChgAnmStopFlg( anm_cont, FALSE );
    FLD_EXP_OBJ_ValidCntAnm( exobj_cont, EXOBJUNITID, ball_obj_id[i], 0, TRUE );
  }

  // ME�Đ�
  PMSND_PauseBGM( TRUE );
  PMSND_PushBGM();
  PMSND_PlayBGM( SEQ_ME_ASA );

#ifdef PC_RECOVERY_DEBUG_ON
  OBATA_Printf( "StartRecoveryAnime\n" );
#endif
}

//==============================================================================
/**
 * @brief �񕜃A�j���[�V�����I������
 *
 * @param work �C�x���g���[�N
 */
//==============================================================================
static void StopRecoveryAnime( RECOVERY_WORK* work )
{
  int i;

  // �{�[���̃A�j���[�V�����I��
  for( i=0; i<work->setBallNum; i++ )
  {
    FLD_EXP_OBJ_CNT_PTR exobj_cont = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
    EXP_OBJ_ANM_CNT_PTR anm_cont = FLD_EXP_OBJ_GetAnmCnt( exobj_cont, EXOBJUNITID, i, 0 );
    FLD_EXP_OBJ_ChgAnmStopFlg( anm_cont, FALSE );
  }

  // BGM�Đ��ĊJ
  PMSND_PopBGM();
  PMSND_PauseBGM( FALSE );

#ifdef PC_RECOVERY_DEBUG_ON
  OBATA_Printf( "StopRecoveryAnime\n" );
#endif
}

//==============================================================================
/**
 * @brief �񕜃A�j���[�V�����I���`�F�b�N
 *
 * @param work �C�x���g���[�N
 *
 * @param �A�j���[�V�����I������ TRUE
 */
//==============================================================================
static BOOL IsRecoveryAnimeEnd( RECOVERY_WORK* work )
{
  int i;

  // �S�{�[���̃A�j���[�V�������I�����Ă��邩�ǂ���
  for( i=0; i<work->setBallNum; i++ )
  {
    FLD_EXP_OBJ_CNT_PTR exobj_cont = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
    EXP_OBJ_ANM_CNT_PTR anm_cont = FLD_EXP_OBJ_GetAnmCnt( exobj_cont, EXOBJUNITID, i, 0 );
    if( FLD_EXP_OBJ_ChkAnmEnd( anm_cont ) == FALSE )
    {
      return FALSE;
    }
  }
  return TRUE;
}

//==============================================================================
/**
 * @brief �񕜃A�j���[�V�����Đ�����
 *
 * @param work �C�x���g���[�N
 */
//==============================================================================
static void PlayRecoveryAnime( RECOVERY_WORK* work )
{
  // �{�[���A�j���[�V�����Đ�
  FLD_EXP_OBJ_CNT_PTR exobj_cont = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
  FLD_EXP_OBJ_PlayAnime( exobj_cont );

  
#ifdef PC_RECOVERY_DEBUG_ON
  {
    fx32 frame = FLD_EXP_OBJ_GetObjAnmFrm( exobj_cont, EXOBJUNITID, 0, 0 );
    OBATA_Printf( "anmframe = %d\n", frame>>FX32_SHIFT );
  }
#endif
}
