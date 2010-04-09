//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �a������o�^�A�j���[�V�����C�x���g
 * @file   event_dendou_anime.c
 * @author obata_toshihiro
 * @date   2010.04.07
 */
//////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "event_dendou_anime.h"
#include "fieldmap.h"
#include "field_buildmodel.h"
#include "field_sound.h"  // for FIELD_SOUND_xxxx

#include "gamesystem/game_event.h" 
#include "sound/pm_sndsys.h"
#include "field/field_const.h"  // for FIELD_CONST_GRID_SIZE

#include "pc_recovery_anime.naix"
#include "../../../resource/fldmapdata/build_model/output/buildmodel_indoor.naix"


//========================================================================================
// ���萔
//========================================================================================
#define BALLSET_WAIT      (20) // �{�[���Z�b�g�Ԋu
#define ANIME_START_WAIT  (10) // �{�[���Z�b�g��̉񕜃A�j���[�V�����J�n�҂�����
#define ANIME_FINISH_WAIT (60) // �{�[���Z�b�g��̉񕜃A�j���[�V�����I���҂�����
#define MACHINE_SEARCH_RANGE (FIELD_CONST_GRID_SIZE * 5)  // �}�V�������͈�

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
typedef struct {

  HEAPID            heapID;           // �q�[�vID
  FIELDMAP_WORK*    fieldmap;         // �t�B�[���h�}�b�v
  VecFx32           machinePos;       // �񕜃}�V���̍��W
  u8                pokemonNum;       // �莝���|�P�����̐�
  u8                setBallNum;       // �}�V���ɃZ�b�g�����{�[���̐�
  u16               seqCount;         // �V�[�P���X����t���[���J�E���^
  FIELD_BMODEL_MAN* bmMan;            // �z�u���f���}�l�[�W��
  FIELD_BMODEL*     ballBM[BALL_NUM]; // �{�[���̔z�u���f��
  G3DMAPOBJST*      machineObjSt;     // �񕜃}�V���z�u���f��
  FIELD_BMODEL* machineBM;

} EVENT_WORK;


//========================================================================================
// ���v���g�^�C�v�錾
//========================================================================================
static GMEVENT_RESULT DendouMachineEvent( GMEVENT* event, int* seq, void* wk );
static void SetupEvent( EVENT_WORK* work, GAMESYS_WORK* gsys );
static void ExitEvent( EVENT_WORK* work );
static void SetMonsterBall( EVENT_WORK* work );
static void StartRecoveryAnime( EVENT_WORK* work );
static void StopRecoveryAnime( EVENT_WORK* work );
static BOOL IsRecoveryAnimeEnd( EVENT_WORK* work );
static int GetPokemonNum_withoutEgg( GAMESYS_WORK* gameSystem );


//========================================================================================
// ���O�����J�֐�
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �a������A�j���C�x���g�𐶐�����
 *
 * @param gsys
 * @param parent
 *
 * @return �a������A�j���C�x���g
 */
//----------------------------------------------------------------------------------------
GMEVENT* EVENT_DendouAnime( GAMESYS_WORK* gsys, GMEVENT* parent )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // �C�x���g�𐶐�
  event = GMEVENT_Create( gsys, parent, DendouMachineEvent, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  SetupEvent( work, gsys );

  return event;
}


//========================================================================================
// ������J�֐�
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �a������o�^�A�j���[�V�����C�x���g
 *
 * @param event
 * @param seq
 * @param wk
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//----------------------------------------------------------------------------------------
static GMEVENT_RESULT DendouMachineEvent( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq ) {
  case 0: 
    if( work->seqCount % BALLSET_WAIT == 0 ) {
      // �{�[����ݒu
      SetMonsterBall( work ); 
      // ���ׂẴ{�[����ݒu����
      if( work->pokemonNum <= work->setBallNum ) {
        (*seq)++; 
        work->seqCount = 0;
      }
    }
    break;

  case 1:  
    if( ANIME_START_WAIT < work->seqCount ) {
      StartRecoveryAnime( work ); // �A�j���J�n
      (*seq)++;
      work->seqCount = 0;
    }
    break;

  case 2: 
    if( IsRecoveryAnimeEnd( work ) ) {
      (*seq)++;
    }
    break;

  case 3:  
    ExitEvent( work );
    return GMEVENT_RES_FINISH;
  }
  work->seqCount++;
  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------------------
/**
 * @breif �C�x���g����������
 *
 * @param work
 * @param gsys
 */
//----------------------------------------------------------------------------------------
static void SetupEvent( EVENT_WORK* work, GAMESYS_WORK* gsys )
{
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMAPPER*     mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );

  // �C�x���g���[�N������
  work->heapID       = FIELDMAP_GetHeapID( fieldmap );
  work->fieldmap     = fieldmap;
  work->pokemonNum   = GetPokemonNum_withoutEgg( gsys );
  work->setBallNum   = 0;
  work->bmMan        = FLDMAPPER_GetBuildModelManager( mapper );
  work->machineObjSt = NULL;

  // �O�̂��ߗ�O����
  if( BALL_NUM < work->pokemonNum ) { work->pokemonNum = BALL_NUM; }

  // �񕜃}�V���̍��W���擾
  {
    FIELD_PLAYER* player;
    VecFx32 pos;
    G3DMAPOBJST** objst;
    u32 objnum;
    FLDHIT_RECT rect;
    player = FIELDMAP_GetFieldPlayer( work->fieldmap );
    FIELD_PLAYER_GetPos( player, &pos );
    rect.top    = pos.z - (MACHINE_SEARCH_RANGE << FX32_SHIFT);
    rect.bottom = pos.z + (MACHINE_SEARCH_RANGE << FX32_SHIFT);
    rect.left   = pos.x - (MACHINE_SEARCH_RANGE << FX32_SHIFT);
    rect.right  = pos.x + (MACHINE_SEARCH_RANGE << FX32_SHIFT);
    objst = FIELD_BMODEL_MAN_CreateObjStatusList( work->bmMan, &rect, 
                                                  BM_SEARCH_ID_EVENT, &objnum );
    if( objst ) {
      work->machineObjSt = objst[0];
      G3DMAPOBJST_getPos( objst[0], &work->machinePos );
    }
    GFL_HEAP_FreeMemory( objst ); 
  }

  if( work->machineObjSt ) {
    work->machineBM = FIELD_BMODEL_Create( work->bmMan, work->machineObjSt ); 
  }
}

//----------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�I������
 *
 * @param work �C�x���g���[�N
 */
//----------------------------------------------------------------------------------------
static void ExitEvent( EVENT_WORK* work )
{
  int i;

#if 0
  // �{�[���̔z�u���f����j��
  for( i=0; i<work->pokemonNum; i++ ) {
    FIELD_BMODEL_Delete( work->ballBM[i] );
  }
#endif

  if( work->machineObjSt ) {
    FIELD_BMODEL_Delete( work->machineBM );
  }
}

//----------------------------------------------------------------------------------------
/**
 * @brief �����X�^�[�{�[�����񕜃}�V���ɃZ�b�g����
 *
 * @param work  �C�x���g���[�N
 */
//----------------------------------------------------------------------------------------
static void SetMonsterBall( EVENT_WORK* work )
{
  int index;  // �Z�b�g����{�[���ԍ�

  // ���łɑS�{�[���̃Z�b�g�������ς݂Ȃ�, �������Ȃ�
  if( work->pokemonNum <= work->setBallNum ) { return; }

  // �z�u���f����o�^
  index = work->setBallNum; 

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
static void StartRecoveryAnime( EVENT_WORK* work )
{
  int i;

  // �{�[��
  for( i=0; i<work->setBallNum; i++ ) {
    FIELD_BMODEL_SetAnime( work->ballBM[i], 0, BMANM_REQ_LOOP );
  }

  // �}�V��
  if( work->machineObjSt ) {
    FIELD_BMODEL_StartAnime( work->machineBM, 0 );
  } 
}

//----------------------------------------------------------------------------------------
/**
 * @brief �񕜃A�j���[�V�����I������
 *
 * @param work �C�x���g���[�N
 */
//----------------------------------------------------------------------------------------
static void StopRecoveryAnime( EVENT_WORK* work )
{
  int i;

  // �{�[���̃A�j���[�V�����I��
  for( i=0; i<work->setBallNum; i++ ) {
    FIELD_BMODEL_SetAnime( work->ballBM[i], 0, BMANM_REQ_STOP );
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
static BOOL IsRecoveryAnimeEnd( EVENT_WORK* work )
{
#if 0
  // �S�{�[���̃A�j���[�V�������I�����Ă��邩�ǂ���
  for( i=0; i<work->setBallNum; i++ )
  {
    BOOL stop = FIELD_BMODEL_GetAnimeStatus( work->ballBM[i], 0 );
    if( stop != TRUE ) { return FALSE; }
  }
  return TRUE;
#endif

  // �}�V���̃A�j���Ŕ���
  if( work->machineObjSt ) {
    return FIELD_BMODEL_WaitCurrentAnime( work->machineBM );
  }
  else {
    return TRUE;
  }
} 

//----------------------------------------------------------------------------------------
/**
 * @brief �^�}�S���������莝���|�P�����̐����擾����
 *
 * @param gameSystem
 *
 * @return �莝���|�P�����̐�
 */
//----------------------------------------------------------------------------------------
static int GetPokemonNum_withoutEgg( GAMESYS_WORK* gameSystem )
{
  GAMEDATA*  gameData = GAMESYSTEM_GetGameData( gameSystem );
  POKEPARTY* party    = GAMEDATA_GetMyPokemon( gameData );
  return PokeParty_GetPokeCountNotEgg( party );
}
