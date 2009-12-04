////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �M�~�b�N����(�|�P�������[�O�t�����g��)
 * @file field_gimmick_league_front01.c
 * @author obata
 * @date 2009.12.01
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#include "fieldmap.h"
#include "field_gimmick_league_front01.h"
#include "gamesystem/game_event.h"
#include "gamesystem/gamesystem.h"
#include "savedata/gimmickwork.h"  // for GIMMICKWORK
#include "system/ica_anime.h"  // for ICA_ANIME
#include "field_gimmick_def.h"  // for FLD_GIMMICK_C09P01
#include "field/field_const.h"  // for FIELD_CONST_GRID_SIZE
#include "fld_exp_obj.h"  // for FLD_EXP_OBJ
#include "arc/league_front.naix"  // for NARC_xxxx
#include "../../../resource/fldmapdata/zonetable/zone_id.h"  // for ZONE_ID_C09P02
#include "event_mapchange.h"  // for EVENT_ChangeMapPosNoFade
#include "event_fieldmap_control.h"  // for EVENT_FieldBrightOut
#include "field_gimmick_league_front02.h"  // for EVENT_LEAGUE_FRONT_02_GIMMICK_GetLiftDownEvent


//==========================================================================================
// ���萔
//==========================================================================================
#define ARCID (ARCID_LEAGUE_FRONT_GIMMICK)  // �M�~�b�N�f�[�^�̃A�[�J�C�uID

// ���t�g���W
#define LIFT_POS_X_GRID (30)
#define LIFT_POS_Z_GRID (43)
#define LIFT_POS_X ((LIFT_POS_X_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIFT_POS_Y (2 << FX32_SHIFT)
#define LIFT_POS_Z ((LIFT_POS_Z_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)

// �M�~�b�N���[�N�̃f�[�^�C���f�b�N�X
typedef enum{
  GIMMICKWORK_DATA_WORK_ADRS,  // LF01�M�~�b�N�Ǘ����[�N�̃A�h���X
  GIMMICKWORK_DATA_NUM,
  GIMMICKWORK_DATA_MAX = GIMMICKWORK_DATA_NUM - 1
} GIMMICKWORK_DATA_INDEX;


//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
//----------
// ���\�[�X
//----------
typedef enum{
  RES_LIFT_NSBMD,          // ���t�g�̃��f��
  RES_LIFT_NSBTA_ALL,      // �l�V��(�S��)�@�@��|��������ita
  RES_LIFT_NSBTA_KAKUTOU,  // �l�V��(�i��)�@�@��|��������ita
  RES_LIFT_NSBTA_AKU,      // �l�V��(��)�@�@�@��|��������ita
  RES_LIFT_NSBTA_GHOST,    // �l�V��(�S�[�X�g)��|��������ita
  RES_LIFT_NSBTA_ESPER,    // �l�V��(�G�X�p�[)��|��������ita
  RES_NUM
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[RES_NUM] = 
{
  {ARCID, NARC_league_front_pl_ele_00_nsbmd,    GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_ele_00_00_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_ele_00_01_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_ele_00_02_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_ele_00_03_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_ele_00_04_nsbta, GFL_G3D_UTIL_RESARC},
}; 
//------------------------
// �A�j���[�V����(���t�g)
//------------------------
typedef enum{
  LIFT_ANM_ALL,     // �l�V��(�S��)�@�@��|��������ita
  LIFT_ANM_KAKUTOU, // �l�V��(�i��)�@�@��|��������ita 
  LIFT_ANM_AKU,     // �l�V��(��)�@�@�@��|��������ita
  LIFT_ANM_GHOST,   // �l�V��(�S�[�X�g)��|��������ita
  LIFT_ANM_ESPER,   // �l�V��(�G�X�p�[)��|��������ita
  LIFT_ANM_NUM
} LIFT_ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table_lift[LIFT_ANM_NUM] = 
{
  {RES_LIFT_NSBTA_ALL, 0},
  {RES_LIFT_NSBTA_KAKUTOU, 0},
  {RES_LIFT_NSBTA_AKU, 0},
  {RES_LIFT_NSBTA_GHOST, 0},
  {RES_LIFT_NSBTA_ESPER, 0},
}; 
//-------------
// �I�u�W�F�N�g
//-------------
typedef enum{
  OBJ_LIFT,  // ���t�g
  OBJ_NUM
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[OBJ_NUM] = 
{
  {RES_LIFT_NSBMD, 0, RES_LIFT_NSBMD, anm_table_lift, LIFT_ANM_NUM},  // ���t�g
}; 
//----------
// ���j�b�g
//----------
typedef enum{
  UNIT_GIMMICK,
  UNIT_NUM
} UNIT_INDEX;
static const GFL_G3D_UTIL_SETUP unit[UNIT_NUM] = 
{ 
  {res_table, RES_NUM, obj_table, OBJ_NUM},
};


//==========================================================================================
// ���M�~�b�N���[�N
//==========================================================================================
typedef struct
{ 
  HEAPID heapID;  // �g�p����q�[�vID

} LF01WORK;


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static void Save( FIELDMAP_WORK* fieldmap, GIMMICKWORK_DATA_INDEX idx, u32 data );
static u32 Load( FIELDMAP_WORK* fieldmap, GIMMICKWORK_DATA_INDEX idx );
static void InitGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap );
static void LoadGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap );
static void SaveGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap );


//==========================================================================================
// ���M�~�b�N�o�^�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void LEAGUE_FRONT_01_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  LF01WORK* work;
  HEAPID heap_id;

  // DEBUG:
  OBATA_Printf( "GIMMICK-LF01: setup\n" ); 

  // �M�~�b�N�Ǘ����[�N�쐬
  heap_id      = FIELDMAP_GetHeapID( fieldmap );
  work         = (LF01WORK*)GFL_HEAP_AllocMemory( heap_id, sizeof(LF01WORK) );
  work->heapID = heap_id; 

  // �M�~�b�N������
  InitGimmick( work, fieldmap );
  LoadGimmick( work, fieldmap ); 

  // LF01�M�~�b�N�Ǘ����[�N�̃A�h���X��ۑ�
  Save( fieldmap, GIMMICKWORK_DATA_WORK_ADRS, (u32)work );

}

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void LEAGUE_FRONT_01_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  LF01WORK* work;

  // �Z�[�u
  work = (LF01WORK*)Load( fieldmap, GIMMICKWORK_DATA_WORK_ADRS );
  SaveGimmick( work, fieldmap );

  // �M�~�b�N�Ǘ����[�N�j��
  GFL_HEAP_FreeMemory( work );

  // DEBUG:
  OBATA_Printf( "GIMMICK-LF01: end\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief ����֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void LEAGUE_FRONT_01_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  // �g���I�u�W�F�N�g�̃A�j���Đ�
  {
    FLD_EXP_OBJ_CNT_PTR exobj_cnt;
    exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
    FLD_EXP_OBJ_PlayAnime( exobj_cnt );
  } 
}


//==========================================================================================
// �� ����J�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N���[�N�ւ̃Z�[�u����
 *
 * @param fieldmap �t�B�[���h�}�b�v
 * @param idx      �i�[��C���f�b�N�X
 * @param data     �i�[������f�[�^
 */
//------------------------------------------------------------------------------------------
static void Save( FIELDMAP_WORK* fieldmap, GIMMICKWORK_DATA_INDEX idx, u32 data )
{
  GAMESYS_WORK*   gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*      gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK* gmkwork = GAMEDATA_GetGimmickWork( gdata );
  u32*       save_work = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_C09P01 );

  // �M�~�b�N���[�N�ɕۑ�
  save_work[idx] = data;

  // DEBUG:
  OBATA_Printf( "GIMMICK-FL01: save[%d] = %d\n", idx, data );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N���[�N����̃��[�h����
 *
 * @param fieldmap �t�B�[���h�}�b�v
 * @param idx      �擾�f�[�^�̃C���f�b�N�X
 *
 * @return �w�肵���C���f�b�N�X�Ɋi�[�����l
 */
//------------------------------------------------------------------------------------------
static u32 Load( FIELDMAP_WORK* fieldmap, GIMMICKWORK_DATA_INDEX idx )
{
  GAMESYS_WORK*   gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*      gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK* gmkwork = GAMEDATA_GetGimmickWork( gdata );
  u32*       save_work = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_C09P01 );
  u32              val = 0;

  // �M�~�b�N���[�N����擾
  val = save_work[idx];

  // DEBUG:
  OBATA_Printf( "GIMMICK-FL01: load[%d] = %d\n", idx, val );

  return val;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N������������
 *
 * @param work     �������Ώۃ��[�N
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
static void InitGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;

  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &unit[UNIT_GIMMICK], UNIT_GIMMICK );

  // ���t�g�̃A�j���[�V����������
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_ALL,     TRUE );
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_KAKUTOU, FALSE );
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_AKU,     FALSE );
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_GHOST,   FALSE );
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_ESPER,   FALSE );

  // ���t�g�̍��W��������
  {
    GFL_G3D_OBJSTATUS* objstatus;
    objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT );
    VEC_Set( &objstatus->trans, LIFT_POS_X, LIFT_POS_Y, LIFT_POS_Z );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�̏�Ԃ𕜋A����
 *
 * @param work     ���A�ΏۃM�~�b�N�̊Ǘ����[�N
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
static void LoadGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap )
{
} 

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�̏�Ԃ�ۑ�����
 *
 * @param work     �ۑ��ΏۃM�~�b�N�̊Ǘ����[�N
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
static void SaveGimmick( LF01WORK* work, FIELDMAP_WORK* fieldmap )
{
}


//==========================================================================================
// �� �C�x���g
//==========================================================================================
typedef struct
{
  GAMESYS_WORK* gsys;
  FIELDMAP_WORK* fieldmap;
  ICA_ANIME* liftAnime;  // ���t�g�̈ړ��A�j���[�V����

} LIFTDOWN_EVENTWORK;

//------------------------------------------------------------------------------------------
/**
 * @brief ���t�g�~���C�x���g����
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT LiftDownEvent( GMEVENT* event, int* seq, void* wk )
{
  LIFTDOWN_EVENTWORK* work = (LIFTDOWN_EVENTWORK*)wk;

  switch( *seq )
  {
  // �A�j���J�n
  case 0:
    {
      HEAPID heap_id;
      heap_id = FIELDMAP_GetHeapID( work->fieldmap );
      work->liftAnime = ICA_ANIME_CreateAlloc( heap_id, ARCID, 
                                               NARC_league_front_pl_ele_00_ica_bin );
    }
    ++(*seq);
    break;
  // �A�j���I���҂�
  case 1:
    // ���@, ���t�g�̍��W���X�V
    {
      VecFx32 trans, pos;
      FIELD_PLAYER* player;
      GFL_G3D_OBJSTATUS* lift_status;
      FLD_EXP_OBJ_CNT_PTR exobj_cnt;
      // ���t�g
      ICA_ANIME_GetTranslate( work->liftAnime, &trans );
      exobj_cnt = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
      lift_status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT );
      lift_status->trans.y = trans.y;
      // ���@
      player = FIELDMAP_GetFieldPlayer( work->fieldmap );
      FIELD_PLAYER_GetPos( player, &pos );
      pos.y = trans.y;
      FIELD_PLAYER_SetPos( player, &pos );
    }
    // �A�j���[�V�����X�V
    if( ICA_ANIME_IncAnimeFrame( work->liftAnime, FX32_ONE ) )  // if(�A�j���I��)
    { 
      ICA_ANIME_Delete( work->liftAnime );
      ++(*seq); 
    }
    break;
  // �t�F�[�h�A�E�g
  case 2:
    {
      GMEVENT* new_event;
      new_event = EVENT_FieldFadeOut( work->gsys, work->fieldmap,
                                      FIELD_FADE_BLACK, 
                                      FIELD_FADE_WAIT );
      GMEVENT_CallEvent( event, new_event );
    }
    ++(*seq);
    break;
  // �}�b�v�`�F���W
  case 3:
    {
      GMEVENT* new_event;
      VecFx32 pos;
      VEC_Set( &pos, 0, 0, 0 );
      new_event = EVENT_ChangeMapPosNoFade( work->gsys, work->fieldmap, 
                                            ZONE_ID_C09P02, &pos, DIR_DOWN );
      GMEVENT_CallEvent( event, new_event );
    }
    ++(*seq);
    break;
  // ���̃C�x���g�֐؂�ւ�
  case 4:
    {
      GMEVENT* next_event;
      next_event = LEAGUE_FRONT_02_GIMMICK_GetLiftDownEvent( work->gsys, work->fieldmap );
      GMEVENT_CallEvent( event, next_event );
    }
    ++(*seq);
    break;
  // �I��
  case 5:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���t�g�~���C�x���g���쐬����
 * 
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return ���t�g�~���C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* LEAGUE_FRONT_01_GIMMICK_GetLiftDownEvent( GAMESYS_WORK* gsys, 
                                                   FIELDMAP_WORK* fieldmap )
{
  GMEVENT* event;
  LIFTDOWN_EVENTWORK* evwork;
  LF01WORK* gmkwork = (LF01WORK*)Load( fieldmap, GIMMICKWORK_DATA_WORK_ADRS );

  // ����
  event = GMEVENT_Create( gsys, NULL, LiftDownEvent, sizeof(LIFTDOWN_EVENTWORK) );
  // ������
  evwork = GMEVENT_GetEventWork( event );
  evwork->gsys      = gsys;
  evwork->fieldmap  = fieldmap;
  evwork->liftAnime = NULL;
  return event;
}
