////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �M�~�b�N����(�|�P�������[�O�t�����g��)
 * @file field_gimmick_league_front01.c
 * @author obata
 * @date 2009.12.01
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#include "fieldmap.h"
#include "gmk_tmp_wk.h"
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
#include "eventwork.h"  // for EVENTWORK_
#include "../../../resource/fldmapdata/flagwork/flag_define.h"  // for SYS_FLAG_BIGFOUR_xxxx
#include "sound/pm_sndsys.h" // for PMSND_xxxx


//==========================================================================================
// ���萔
//==========================================================================================
// �M�~�b�N���[�N�̃A�T�C��ID
#define GIMMICK_WORK_ASSIGN_ID (0)

// �M�~�b�N�f�[�^�̃A�[�J�C�uID
#define ARCID (ARCID_LEAGUE_FRONT_GIMMICK)  

// �l�V�����C�g�����t�g�ƈꏏ�ɍ~�����邩�ǂ���
#define LIGHT_DOWN

// �M�~�b�N���[�N�̃f�[�^�C���f�b�N�X
typedef enum{
  GIMMICKWORK_DATA_NUM,
  GIMMICKWORK_DATA_MAX = GIMMICKWORK_DATA_NUM - 1
} GIMMICKWORK_DATA_INDEX;

// ���t�g�A�b�v���̃p�����[�^
#define LIFT_UP_FRAME (20)
#define LIFT_UP_CAMERA_X (0x000001f9 << FX32_SHIFT) 
#define LIFT_UP_CAMERA_Y (0x0000006d << FX32_SHIFT) 
#define LIFT_UP_CAMERA_Z (0x00000329 << FX32_SHIFT)
#define LIFT_UP_TARGET_X (0x000001f9 << FX32_SHIFT) 
#define LIFT_UP_TARGET_Y (0x0000003c << FX32_SHIFT) 
#define LIFT_UP_TARGET_Z (0x000002d0 << FX32_SHIFT)


//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
//----------
// ���\�[�X
//----------
typedef enum{
  RES_LIFT_NSBMD,             // ���t�g�̃��f��
  RES_LIFT_NSBTA,             // ���t�g��ita
  RES_LIGHT_FIGHT_NSBMD,      // ���C�g(�i��)    ���f��
  RES_LIGHT_FIGHT_OFF_NSBTA,  // ���C�g(�i��)    OFF
  RES_LIGHT_FIGHT_ON_NSBTA,   // ���C�g(�i��)    ON
  RES_LIGHT_EVIL_NSBMD,       // ���C�g(��)      ���f��
  RES_LIGHT_EVIL_OFF_NSBTA,   // ���C�g(��)      OFF
  RES_LIGHT_EVIL_ON_NSBTA,    // ���C�g(��)      ON
  RES_LIGHT_GHOST_NSBMD,      // ���C�g(�S�[�X�g)���f��
  RES_LIGHT_GHOST_OFF_NSBTA,  // ���C�g(�S�[�X�g)OFF
  RES_LIGHT_GHOST_ON_NSBTA,   // ���C�g(�S�[�X�g)ON
  RES_LIGHT_ESPER_NSBMD,      // ���C�g(�G�X�p�[)���f��
  RES_LIGHT_ESPER_OFF_NSBTA,  // ���C�g(�G�X�p�[)OFF
  RES_LIGHT_ESPER_ON_NSBTA,   // ���C�g(�G�X�p�[)ON
  RES_NUM
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[RES_NUM] = 
{
  {ARCID, NARC_league_front_pl_ele_00_nsbmd,    GFL_G3D_UTIL_RESARC}, 
  {ARCID, NARC_league_front_pl_ele_00_00_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_lite1_nsbmd,     GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_lite1_off_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_lite1_on_nsbta,  GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_lite2_nsbmd,     GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_lite2_off_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_lite2_on_nsbta,  GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_lite3_nsbmd,     GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_lite3_off_nsbta, GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_lite3_on_nsbta,  GFL_G3D_UTIL_RESARC},
  {ARCID, NARC_league_front_pl_lite4_nsbmd,     GFL_G3D_UTIL_RESARC}, 
  {ARCID, NARC_league_front_pl_lite4_off_nsbta, GFL_G3D_UTIL_RESARC}, 
  {ARCID, NARC_league_front_pl_lite4_on_nsbta,  GFL_G3D_UTIL_RESARC}, 
}; 
//------------------------
// �A�j���[�V����(���t�g)
//------------------------
typedef enum{
  LIFT_ANM_TA,  // �e�N�X�`�� �A�j���[�V����
  LIFT_ANM_NUM
} LIFT_ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table_lift[LIFT_ANM_NUM] = 
{
  {RES_LIFT_NSBTA, 0},
}; 
//------------------------
// �A�j���[�V����(���C�g)
//------------------------
// �i��
typedef enum{
  LIGHT_FIGHT_ANM_OFF,  // OFF
  LIGHT_FIGHT_ANM_ON,   // ON
  LIGHT_FIGHT_ANM_NUM
} LIGHT_FIGHT_ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table_light_fight[LIGHT_FIGHT_ANM_NUM] = 
{
  {RES_LIGHT_FIGHT_OFF_NSBTA, 0},
  {RES_LIGHT_FIGHT_ON_NSBTA,  0},
};
// ��
typedef enum{
  LIGHT_EVIL_ANM_OFF,  // OFF
  LIGHT_EVIL_ANM_ON,   // ON
  LIGHT_EVIL_ANM_NUM
} LIGHT_EVIL_ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table_light_evil[LIGHT_EVIL_ANM_NUM] = 
{
  {RES_LIGHT_EVIL_OFF_NSBTA, 0},
  {RES_LIGHT_EVIL_ON_NSBTA,  0},
};
// �S�[�X�g
typedef enum{
  LIGHT_GHOST_ANM_OFF,  // OFF
  LIGHT_GHOST_ANM_ON,   // ON
  LIGHT_GHOST_ANM_NUM
} LIGHT_GHOST_ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table_light_ghost[LIGHT_GHOST_ANM_NUM] = 
{
  {RES_LIGHT_GHOST_OFF_NSBTA, 0},
  {RES_LIGHT_GHOST_ON_NSBTA,  0},
};
// �G�X�p�[
typedef enum{
  LIGHT_ESPER_ANM_OFF,  // OFF
  LIGHT_ESPER_ANM_ON,   // ON
  LIGHT_ESPER_ANM_NUM
} LIGHT_ESPER_ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table_light_esper[LIGHT_ESPER_ANM_NUM] = 
{
  {RES_LIGHT_ESPER_OFF_NSBTA, 0},
  {RES_LIGHT_ESPER_ON_NSBTA,  0},
};
//-------------
// �I�u�W�F�N�g
//-------------
typedef enum{
  OBJ_LIFT,         // ���t�g
  OBJ_LIGHT_FIGHT,  // ���C�g(�i��)
  OBJ_LIGHT_EVIL,   // ���C�g(��)
  OBJ_LIGHT_GHOST,  // ���C�g(�S�[�X�g)
  OBJ_LIGHT_ESPER,  // ���C�g(�G�X�p�[)
  OBJ_NUM
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[OBJ_NUM] = 
{
  {RES_LIFT_NSBMD,        0, RES_LIFT_NSBMD,        anm_table_lift,        LIFT_ANM_NUM},
  {RES_LIGHT_FIGHT_NSBMD, 0, RES_LIGHT_FIGHT_NSBMD, anm_table_light_fight, LIGHT_FIGHT_ANM_NUM},
  {RES_LIGHT_EVIL_NSBMD,  0, RES_LIGHT_EVIL_NSBMD,  anm_table_light_evil,  LIGHT_EVIL_ANM_NUM},
  {RES_LIGHT_GHOST_NSBMD, 0, RES_LIGHT_GHOST_NSBMD, anm_table_light_ghost, LIGHT_GHOST_ANM_NUM},
  {RES_LIGHT_ESPER_NSBMD, 0, RES_LIGHT_ESPER_NSBMD, anm_table_light_esper, LIGHT_ESPER_ANM_NUM},
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
// ���e�I�u�W�F�̔z�u
//========================================================================================== 
// ���t�g���W
#define LIFT_POS_X_GRID (32)
#define LIFT_POS_Y_GRID (0)
#define LIFT_POS_Z_GRID (42)
#define LIFT_POS_X ((LIFT_POS_X_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIFT_POS_Y ((LIFT_POS_Y_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIFT_POS_Z ((LIFT_POS_Z_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT) 
// ���C�g���W(�i��)
#define LIGHT_FIGHT_POS_X_GRID (28)
#define LIGHT_FIGHT_POS_Y_GRID (0)
#define LIGHT_FIGHT_POS_Z_GRID (39)
#define LIGHT_FIGHT_POS_X ((LIGHT_FIGHT_POS_X_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIGHT_FIGHT_POS_Y ((LIGHT_FIGHT_POS_Y_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIGHT_FIGHT_POS_Z ((LIGHT_FIGHT_POS_Z_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
// ���C�g���W(��)
#define LIGHT_EVIL_POS_X_GRID (34)
#define LIGHT_EVIL_POS_Y_GRID (0)
#define LIGHT_EVIL_POS_Z_GRID (39)
#define LIGHT_EVIL_POS_X ((LIGHT_EVIL_POS_X_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIGHT_EVIL_POS_Y ((LIGHT_EVIL_POS_Y_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIGHT_EVIL_POS_Z ((LIGHT_EVIL_POS_Z_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
// ���C�g���W(�S�[�X�g)
#define LIGHT_GHOST_POS_X_GRID (28)
#define LIGHT_GHOST_POS_Y_GRID (0)
#define LIGHT_GHOST_POS_Z_GRID (44)
#define LIGHT_GHOST_POS_X ((LIGHT_GHOST_POS_X_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIGHT_GHOST_POS_Y ((LIGHT_GHOST_POS_Y_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIGHT_GHOST_POS_Z ((LIGHT_GHOST_POS_Z_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
// ���C�g���W(�G�X�p�[)
#define LIGHT_ESPER_POS_X_GRID (34)
#define LIGHT_ESPER_POS_Y_GRID (0)
#define LIGHT_ESPER_POS_Z_GRID (44)
#define LIGHT_ESPER_POS_X ((LIGHT_ESPER_POS_X_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIGHT_ESPER_POS_Y ((LIGHT_ESPER_POS_Y_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)
#define LIGHT_ESPER_POS_Z ((LIGHT_ESPER_POS_Z_GRID * FIELD_CONST_GRID_SIZE) << FX32_SHIFT)

// �z�u���W
static const VecFx32 obj_pos[OBJ_NUM] = 
{
  {LIFT_POS_X,        LIFT_POS_Y,        LIFT_POS_Z},
  {LIGHT_FIGHT_POS_X, LIGHT_FIGHT_POS_Y, LIGHT_FIGHT_POS_Z},
  {LIGHT_EVIL_POS_X,  LIGHT_EVIL_POS_Y,  LIGHT_EVIL_POS_Z},
  {LIGHT_GHOST_POS_X, LIGHT_GHOST_POS_Y, LIGHT_GHOST_POS_Z},
  {LIGHT_ESPER_POS_X, LIGHT_ESPER_POS_Y, LIGHT_ESPER_POS_Z},
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
  work         = (LF01WORK*)GMK_TMP_WK_AllocWork( fieldmap, GIMMICK_WORK_ASSIGN_ID,  heap_id, sizeof(LF01WORK) );
  work->heapID = heap_id; 

  // �M�~�b�N������
  InitGimmick( work, fieldmap );
  LoadGimmick( work, fieldmap ); 
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
  work = GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );
  SaveGimmick( work, fieldmap );

  // �M�~�b�N�Ǘ����[�N�j��
  GMK_TMP_WK_FreeWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );

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
  {
    static BOOL def = TRUE;
    int key = GFL_UI_KEY_GetCont();
    int trg = GFL_UI_KEY_GetTrg();
    if( key & PAD_BUTTON_DEBUG )
    {
      if( trg & PAD_BUTTON_A )
      {
        FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );

        if( def )
        {
          FLD_CAM_MV_PARAM moveParam;
          FIELD_CAMERA_SetRecvCamParam( camera );
          moveParam.Chk.Shift = FALSE;
          moveParam.Chk.Pitch = FALSE;
          moveParam.Chk.Yaw = FALSE;
          moveParam.Chk.Dist = FALSE;
          moveParam.Chk.Fovy = FALSE;
          moveParam.Chk.Pos = TRUE;
          VEC_Set( &moveParam.Core.CamPos, LIFT_UP_CAMERA_X, LIFT_UP_CAMERA_Y, LIFT_UP_CAMERA_Z ); 
          VEC_Set( &moveParam.Core.TrgtPos, LIFT_UP_TARGET_X, LIFT_UP_TARGET_Y, LIFT_UP_TARGET_Z ); 
          FIELD_CAMERA_SetLinerParamDirect( camera, 
              &moveParam.Core.CamPos, &moveParam.Core.TrgtPos, &moveParam.Chk, LIFT_UP_FRAME );
        }
        else
        {
          FIELD_CAMERA_RecvLinerParamDefault( camera, LIFT_UP_FRAME );
        }
        def = !def; 
      }
    }
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

  // �e�I�u�W�F�̔z�u
  {
    int obj_idx;
    GFL_G3D_OBJSTATUS* objstatus;
    for( obj_idx=0; obj_idx<OBJ_NUM; obj_idx++ )
    {
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, UNIT_GIMMICK, obj_idx );
      VEC_Set( &objstatus->trans, obj_pos[obj_idx].x, obj_pos[obj_idx].y, obj_pos[obj_idx].z );
    }
  }

  // ���t�g�̃A�j���[�V����
  {
    GAMESYS_WORK* gsys;
    GAMEDATA* gdata;
    EVENTWORK* evwork;
    BOOL fight, evil, ghost, esper;
    // �t���O�`�F�b�N
    gsys   = FIELDMAP_GetGameSysWork( fieldmap );
    gdata  = GAMESYSTEM_GetGameData( gsys );
    evwork = GAMEDATA_GetEventWork( gdata );
    fight  = EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_BIGFOUR_FIGHTWIN );
    evil   = EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_BIGFOUR_EVILWIN );
    ghost  = EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_BIGFOUR_GHOSTWIN );
    esper  = EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_BIGFOUR_ESPWIN );
    // �e�l�V���N���A�ɉ������A�j�����Đ�
    if( fight && evil && ghost && esper )
    {
      FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT, LIFT_ANM_TA, TRUE );
    }
  }

  // ���C�g�̃A�j���[�V����
  {
    GAMESYS_WORK* gsys;
    GAMEDATA* gdata;
    EVENTWORK* evwork;
    BOOL fight, evil, ghost, esper;
    // �t���O�`�F�b�N
    gsys   = FIELDMAP_GetGameSysWork( fieldmap );
    gdata  = GAMESYSTEM_GetGameData( gsys );
    evwork = GAMEDATA_GetEventWork( gdata );
    fight  = EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_BIGFOUR_FIGHTWIN );
    evil   = EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_BIGFOUR_EVILWIN );
    ghost  = EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_BIGFOUR_GHOSTWIN );
    esper  = EVENTWORK_CheckEventFlag( evwork, SYS_FLAG_BIGFOUR_ESPWIN );
    // �A�j���Đ�
    if( fight ){ FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_FIGHT, LIGHT_FIGHT_ANM_ON,  TRUE ); }
    else       { FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_FIGHT, LIGHT_FIGHT_ANM_OFF, TRUE ); }
    if( evil  ){ FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_EVIL,  LIGHT_EVIL_ANM_ON,   TRUE ); }
    else       { FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_EVIL,  LIGHT_EVIL_ANM_OFF,  TRUE ); }
    if( ghost ){ FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_GHOST, LIGHT_GHOST_ANM_ON,  TRUE ); }
    else       { FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_GHOST, LIGHT_GHOST_ANM_OFF, TRUE ); }
    if( esper ){ FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_ESPER, LIGHT_ESPER_ANM_ON,  TRUE ); }
    else       { FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_ESPER, LIGHT_ESPER_ANM_OFF, TRUE ); }
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
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  // �J�����ړ��J�n
  case 0:
    { // ���`�J�����ݒ�
      FLD_CAM_MV_PARAM moveParam;
      moveParam.Chk.Shift = FALSE;
      moveParam.Chk.Pitch = FALSE;
      moveParam.Chk.Yaw = FALSE;
      moveParam.Chk.Dist = FALSE;
      moveParam.Chk.Fovy = FALSE;
      moveParam.Chk.Pos = TRUE;
      VEC_Set( &moveParam.Core.CamPos, LIFT_UP_CAMERA_X, LIFT_UP_CAMERA_Y, LIFT_UP_CAMERA_Z ); 
      VEC_Set( &moveParam.Core.TrgtPos, LIFT_UP_TARGET_X, LIFT_UP_TARGET_Y, LIFT_UP_TARGET_Z ); 
      FIELD_CAMERA_SetLinerParamDirect( camera, 
          &moveParam.Core.CamPos, &moveParam.Core.TrgtPos, &moveParam.Chk, LIFT_UP_FRAME );
    }
    (*seq)++;
    OBATA_Printf( "GIMMICK-LF01 LIFT DOWN EVENT: seq ==> %d\n", *seq );
    break;
  // �J�����ړ��I���҂�
  case 1:
    if( FIELD_CAMERA_CheckMvFunc(camera) == FALSE )
    { 
      (*seq)++; 
      OBATA_Printf( "GIMMICK-LF01 LIFT DOWN EVENT: seq ==> %d\n", *seq );
    }
    break;
  // ���t�g�ړ��J�n
  case 2:
    {
      HEAPID heap_id;
      heap_id = FIELDMAP_GetHeapID( work->fieldmap );
      work->liftAnime = ICA_ANIME_CreateAlloc( heap_id, ARCID, 
                                               NARC_league_front_pl_ele_00_ica_bin );
    }
    // �G���x�[�^�ғ����J�n
    PMSND_PlaySE( SEQ_SE_FLD_148 );
    ++(*seq);
    OBATA_Printf( "GIMMICK-LF01 LIFT DOWN EVENT: seq ==> %d\n", *seq );
    break;
  // ���t�g�ړ��I���҂�
  case 3:
    // ���@, ���t�g�̍��W���X�V
    {
      VecFx32 trans, pos;
      FIELD_PLAYER* player;
      GFL_G3D_OBJSTATUS* objstatus;
      FLD_EXP_OBJ_CNT_PTR exobj_cnt;
      // ���t�g
      ICA_ANIME_GetTranslate( work->liftAnime, &trans );
      exobj_cnt = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, UNIT_GIMMICK, OBJ_LIFT );
      objstatus->trans.y = trans.y;
#ifdef LIGHT_DOWN
      // ���C�g
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_FIGHT );
      objstatus->trans.y = trans.y;
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_EVIL );
      objstatus->trans.y = trans.y;
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_GHOST );
      objstatus->trans.y = trans.y;
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, UNIT_GIMMICK, OBJ_LIGHT_ESPER );
      objstatus->trans.y = trans.y;
#endif
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
      OBATA_Printf( "GIMMICK-LF01 LIFT DOWN EVENT: seq ==> %d\n", *seq );
    }
    break;
  // �t�F�[�h�A�E�g
  case 4:
    {
      GMEVENT* new_event;
      new_event = EVENT_FieldFadeOut_Black( work->gsys, work->fieldmap, FIELD_FADE_WAIT );
      GMEVENT_CallEvent( event, new_event );
    }
    ++(*seq);
    OBATA_Printf( "GIMMICK-LF01 LIFT DOWN EVENT: seq ==> %d\n", *seq );
    break;
  // �}�b�v�`�F���W
  case 5:
    {
      GMEVENT* new_event;
      VecFx32 pos;
      VEC_Set( &pos, 0, 0, 0 );
      new_event = EVENT_ChangeMapPosNoFade( work->gsys, work->fieldmap, 
                                            ZONE_ID_C09P02, &pos, DIR_DOWN );
      GMEVENT_CallEvent( event, new_event );
    }
    ++(*seq);
    OBATA_Printf( "GIMMICK-LF01 LIFT DOWN EVENT: seq ==> %d\n", *seq );
    break;
  // ���̃C�x���g�֐؂�ւ�
  case 6:
    /*
    {
      GMEVENT* next_event;
      next_event = LEAGUE_FRONT_02_GIMMICK_GetLiftDownEvent( work->gsys, work->fieldmap );
      GMEVENT_ChangeEvent( event, next_event );
    }
    */
    ++(*seq);
    OBATA_Printf( "GIMMICK-LF01 LIFT DOWN EVENT: seq ==> %d\n", *seq );
    break;
  // �I��
  case 7:
    OBATA_Printf( "GIMMICK-LF01 LIFT DOWN EVENT: seq ==> finish" );
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
  LF01WORK* gmkwork = GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );

  // ����
  event = GMEVENT_Create( gsys, NULL, LiftDownEvent, sizeof(LIFTDOWN_EVENTWORK) );
  // ������
  evwork = GMEVENT_GetEventWork( event );
  evwork->gsys      = gsys;
  evwork->fieldmap  = fieldmap;
  evwork->liftAnime = NULL;
  return event;
}
