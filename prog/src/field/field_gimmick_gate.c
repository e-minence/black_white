//////////////////////////////////////////////////////////////////////////////////////////// 
/** 
 *
 * @brief  �M�~�b�N�o�^�֐�(�Q�[�g) 
 * @file   field_gimmick_gate.c 
 * @author obata 
 * @date   2009.10.21 
 *
 * [�d���f���̒ǉ����@]
 * �@�z�u�������]�[���ŃM�~�b�N�����삷��悤��, fld_gimmick_assign.xls �ɓo�^
 * �A�o�^�����]�[���ł̃M�~�b�N����֐���, field_gimmick.c �ɓo�^
 * �B�z�u����]�[���̓d���f���f�[�^���쐬��, �d���f���f�[�^�o�^�e�[�u���ɒǉ�
 *
 */ 
//////////////////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h> 
#include "field_gimmick_gate.h" 
#include "arc/arc_def.h" 
#include "arc/gate.naix"
#include "arc/message.naix"

#include "field/gimmick_obj_elboard.h"
#include "savedata/gimmickwork.h"
#include "gimmick_obj_elboard.h"
#include "elboard_zone_data.h"
#include "elboard_spnews_data.h"
#include "arc/gate.naix"
#include "field_gimmick_def.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "msg/msg_gate.h"
#include "field/zonedata.h"
#include "field/enc_pokeset.h"  // for ENCPOKE_GetGenerateZone

#include "field_task_manager.h"
#include "field_task.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_target_offset.h"


//==========================================================================================
// ���d���f���f�[�^�o�^�e�[�u��
//==========================================================================================
typedef struct
{
  u16 zone_id;
  int gimmick_id;
  u32 dat_id; 
} ENTRY_DATA; 
static const ENTRY_DATA entry_table[] = 
{
  { ZONE_ID_C04R0601, FLD_GIMMICK_C04R0601, NARC_gate_elboard_zone_data_c04r0601_bin },
  { ZONE_ID_C08R0601, FLD_GIMMICK_C08R0601, NARC_gate_elboard_zone_data_c08r0601_bin },
  { ZONE_ID_R13R0201, FLD_GIMMICK_R13R0201, NARC_gate_elboard_zone_data_r13r0201_bin },
  { ZONE_ID_R02R0101, FLD_GIMMICK_R02R0101, NARC_gate_elboard_zone_data_r02r0101_bin },
  { ZONE_ID_C04R0701, FLD_GIMMICK_C04R0701, NARC_gate_elboard_zone_data_c04r0701_bin },
  { ZONE_ID_C04R0801, FLD_GIMMICK_C04R0801, NARC_gate_elboard_zone_data_c04r0801_bin },
  { ZONE_ID_C02R0701, FLD_GIMMICK_C02R0701, NARC_gate_elboard_zone_data_c02r0701_bin },
  { ZONE_ID_R14R0101, FLD_GIMMICK_R14R0101, NARC_gate_elboard_zone_data_r14r0101_bin },
  { ZONE_ID_C08R0501, FLD_GIMMICK_C08R0501, NARC_gate_elboard_zone_data_c08r0501_bin },
  { ZONE_ID_C08R0701, FLD_GIMMICK_C08R0701, NARC_gate_elboard_zone_data_c08r0701_bin },
  { ZONE_ID_H01R0101, FLD_GIMMICK_H01R0101, NARC_gate_elboard_zone_data_h01r0101_bin },
  { ZONE_ID_H01R0201, FLD_GIMMICK_H01R0201, NARC_gate_elboard_zone_data_h01r0201_bin },
  { ZONE_ID_C03R0601, FLD_GIMMICK_C03R0601, NARC_gate_elboard_zone_data_c03r0601_bin },
};

//------------------------------------------------------------------------------------------
/**
 * @brief �w�肵���M�~�b�NID���o�^����Ă��邩�ǂ������肷��
 *
 * @param gmk_id ����ΏۃM�~�b�NID
 *
 * @return �o�^����Ă���ꍇ TRUE
 */
//------------------------------------------------------------------------------------------
static BOOL IsGimmickIDEntried( int gmk_id )
{
  int i;
  for( i=0; i<NELEMS(entry_table); i++ )
  {
    if( entry_table[i].gimmick_id == gmk_id )
    {
      return TRUE;  // ����
    }
  }
  return FALSE; // ������
}


//==========================================================================================
// ���萔
//========================================================================================== 
// ��x�ɕ\������n����̐�
#define LOCAL_INFO_NUM (3)

// �g���I�u�W�F�N�g�E���j�b�g�ԍ�
#define EXPOBJ_UNIT_ELBOARD (1)

// �\���\�Ȉ��ʂɕ\���\�ȕ�����
#define DISPLAY_SIZE (8)

// �j���[�X�Ԃ̊Ԋu(������)
#define NEWS_INTERVAL (3)

// ��x�ɕ\������W�����̍ő吔
#define GYM_NEWS_MAX_NUM (4)


//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
// ���\�[�X�C���f�b�N�X
typedef enum
{
  RES_ELBOARD_NSBMD,    // �f���̃��f��
  RES_ELBOARD_NSBTX,    // �f���̃e�N�X�`��
  RES_ELBOARD_NSBTA_1,  // �j���[�X�E�X�N���[���E�A�j���[�V����1
  RES_ELBOARD_NSBTA_2,  // �j���[�X�E�X�N���[���E�A�j���[�V����2
  RES_ELBOARD_NSBTA_3,  // �j���[�X�E�X�N���[���E�A�j���[�V����3
  RES_ELBOARD_NSBTA_4,  // �j���[�X�E�X�N���[���E�A�j���[�V����4
  RES_ELBOARD_NSBTA_5,  // �j���[�X�E�X�N���[���E�A�j���[�V����5
  RES_ELBOARD_NSBTA_6,  // �j���[�X�E�X�N���[���E�A�j���[�V����6
  RES_ELBOARD_NSBTA_7,  // �j���[�X�E�X�N���[���E�A�j���[�V����7
  RES_MONITOR_NSBTP_1,  // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V����1
  RES_MONITOR_NSBTP_2,  // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V����2
  RES_MONITOR_NSBTP_3,  // ���j�^�[�E�e�N�X�`���p�^�[���E�A�j���[�V����3
  RES_NUM
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[RES_NUM] = 
{
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_nsbtx, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_1_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_2_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_3_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_4_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_5_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_6_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_7_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv01_nsbtp, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv02_nsbtp, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_tv03_nsbtp, GFL_G3D_UTIL_RESARC },
};

// �A�j���C���f�b�N�X
typedef enum
{
  ANM_ELBOARD_DATE,         // �f���j���[�X�E�X�N���[���E���t
  ANM_ELBOARD_WEATHER,      // �f���j���[�X�E�X�N���[���E�V�C
  ANM_ELBOARD_PROPAGATION,  // �f���j���[�X�E�X�N���[���E��ʔ���
  ANM_ELBOARD_INFO_A,       // �f���j���[�X�E�X�N���[���E���A
  ANM_ELBOARD_INFO_B,       // �f���j���[�X�E�X�N���[���E���B
  ANM_ELBOARD_INFO_C,       // �f���j���[�X�E�X�N���[���E���C
  ANM_ELBOARD_CM,           // �f���j���[�X�E�X�N���[���E�ꌾCM
  ANM_MONITOR_1,            // ���j�^�[�E�e�N�X�`���E�A�j���[�V����1
  ANM_MONITOR_2,            // ���j�^�[�E�e�N�X�`���E�A�j���[�V����2
  ANM_MONITOR_3,            // ���j�^�[�E�e�N�X�`���E�A�j���[�V����3
  ANM_NUM
} ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table[ANM_NUM] = 
{
  // �A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ELBOARD_NSBTA_1, 0 },
  { RES_ELBOARD_NSBTA_2, 0 },
  { RES_ELBOARD_NSBTA_3, 0 },
  { RES_ELBOARD_NSBTA_4, 0 },
  { RES_ELBOARD_NSBTA_5, 0 },
  { RES_ELBOARD_NSBTA_6, 0 },
  { RES_ELBOARD_NSBTA_7, 0 },
  { RES_MONITOR_NSBTP_1, 0 },
  { RES_MONITOR_NSBTP_2, 0 },
  { RES_MONITOR_NSBTP_3, 0 },
};

// �I�u�W�F�N�g�C���f�b�N�X
typedef enum
{
  OBJ_ELBOARD,  // �d���f����
  OBJ_NUM
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[OBJ_NUM] = 
{
  // ���f�����\�[�XID, 
  // ���f���f�[�^ID(���\�[�X����INDEX), 
  // �e�N�X�`�����\�[�XID,
  // �A�j���e�[�u��, 
  // �A�j�����\�[�X��
  { RES_ELBOARD_NSBMD, 0, RES_ELBOARD_NSBTX, anm_table, ANM_NUM },
}; 

// ���j�b�g�C���f�b�N�X
typedef enum
{
  UNIT_ELBOARD, // �d���f���� + ���j�^�[
  UNIT_NUM
} UNIT_INDEX;
static const GFL_G3D_UTIL_SETUP unit[UNIT_NUM] =
{
  { res_table, RES_NUM, obj_table, OBJ_NUM },
};


//==========================================================================================
// ���j���[�X�p�����[�^
//==========================================================================================
// �j���[�X�^�C�v
typedef enum {
  NEWS_TYPE_NULL,         // ��
  NEWS_TYPE_DATE,         // ���t
  NEWS_TYPE_WEATHER,      // �V�C
  NEWS_TYPE_PROPAGATION,  // ��ʔ���
  NEWS_TYPE_INFO_A,       // �n����A
  NEWS_TYPE_INFO_B,       // �n����B
  NEWS_TYPE_INFO_C,       // �n����C
  NEWS_TYPE_CM,           // �ꌾCM 
  NEWS_TYPE_SPECIAL,      // �Վ��j���[�X
  NEWS_TYPE_NUM,
  NEWS_TYPE_MAX = NEWS_TYPE_NUM - 1
} NEWS_TYPE;

// �j���[�X�o�^�ԍ�
typedef enum {
  NEWS_INDEX_DATE,         // ���t
  NEWS_INDEX_WEATHER,      // �V�C
  NEWS_INDEX_PROPAGATION,  // ��ʔ���
  NEWS_INDEX_INFO_A,       // �n����A
  NEWS_INDEX_INFO_B,       // �n����B
  NEWS_INDEX_INFO_C,       // �n����C
  NEWS_INDEX_CM,           // �ꌾCM
  NEWS_INDEX_NUM,
  NEWS_INDEX_MAX = NEWS_INDEX_NUM -1
} NEWS_INDEX; 

// �X�N���[���E�A�j���[�V�����E�C���f�b�N�X
static u16 news_anm_index[NEWS_INDEX_NUM] = 
{
  ANM_ELBOARD_DATE,        // ���t
  ANM_ELBOARD_WEATHER,     // �V�C
  ANM_ELBOARD_PROPAGATION, // ��ʔ���
  ANM_ELBOARD_INFO_A,      // �n����A
  ANM_ELBOARD_INFO_B,      // �n����B
  ANM_ELBOARD_INFO_C,      // �n����C
  ANM_ELBOARD_CM,          // �ꌾCM
};
// �e�N�X�`����
static char* news_tex_name[NEWS_INDEX_NUM] =
{
  "gelboard_1",
  "gelboard_2",
  "gelboard_3",
  "gelboard_4",
  "gelboard_5",
  "gelboard_6",
  "gelboard_7",
};
// �p���b�g��
static char* news_plt_name[NEWS_INDEX_NUM] =
{
  "gelboard_1_pl",
  "gelboard_2_pl",
  "gelboard_3_pl",
  "gelboard_4_pl",
  "gelboard_5_pl",
  "gelboard_6_pl",
  "gelboard_7_pl",
};
// �V�C�Ɏg�p���郁�b�Z�[�W
u32 str_id_weather[WEATHER_NO_NUM] = 
{
  msg_gate_sunny,     // ����
  msg_gate_snow,      // ��
  msg_gate_rain,      // �J
  msg_gate_storm,     // ����
  msg_gate_spark,     // ���J
  msg_gate_snowstorm, // ����
  msg_gate_arare,     // ��
  msg_gate_mirage,    // 凋C�O
};

// �W�����̓o�^�ꏊ
static const NEWS_INDEX gym_news_idx[GYM_NEWS_MAX_NUM] = 
{ 
  NEWS_INDEX_PROPAGATION, 
  NEWS_INDEX_INFO_A, 
  NEWS_INDEX_INFO_B, 
  NEWS_INDEX_INFO_C
};


//==========================================================================================
// �����j�^�[�p�����[�^
//==========================================================================================
// �A�j���[�V�����ԍ�
typedef enum {
  MONITOR_ANIME_1,
  MONITOR_ANIME_2,
  MONITOR_ANIME_3,
  MONITOR_ANIME_NUM,
  MONITOR_ANIME_MAX = MONITOR_ANIME_NUM - 1
} MONITOR_ANIME_INDEX;

// ���A�j���[�V�����ԍ�
static u16 monitor_anime[MONITOR_ANIME_NUM] = 
{ 
  ANM_MONITOR_1,
  ANM_MONITOR_2,
  ANM_MONITOR_3,
};


//==========================================================================================
// ���j���[�X�o�^��
//==========================================================================================
typedef struct
{
  u8                          newsNum;  // �o�^�����j���[�X�̐�
  NEWS_TYPE  newsType[NEWS_INDEX_NUM];  // �o�^�����j���[�X
  u32      spNewsFlag[NEWS_INDEX_NUM];  // �o�^�����Վ��j���[�X�ɑΉ�����t���O

} NEWS_ENTRY_DATA;

//------------------------------------------------------------------------------------------
/**
 * @brief �o�^�󋵂��X�V����
 *
 * @param data �ǉ���̃f�[�^
 * @param type �ǉ�����j���[�X�̃^�C�v
 * @param flag �Վ��j���[�X�̏ꍇ, �Ή�����t���O���w��
 */
//------------------------------------------------------------------------------------------
static void AddNewsEntryData( NEWS_ENTRY_DATA* data, NEWS_TYPE type, u32 flag )
{
  // ���łɍő吔���o�^����Ă���
  if( NEWS_INDEX_NUM <= data->newsNum ){ return; }

  // �o�^
  data->newsType[ data->newsNum ]   = type;
  data->spNewsFlag[ data->newsNum ] = flag;
  data->newsNum++;
}


//==========================================================================================
// ���M�~�b�N �Z�[�u���[�N
//==========================================================================================
typedef struct
{
  void*                gateWork;  // �M�~�b�N�Ǘ����[�N�ւ̃|�C���^
  u32             recoveryFrame;  // ���A�t���[��
  NEWS_ENTRY_DATA newsEntryData;  // �j���[�X�o�^��

} SAVEWORK;


//==========================================================================================
// ���M�~�b�N�Ǘ����[�N
//==========================================================================================
typedef struct
{ 
  HEAPID                   heapID;  // �g�p����q�[�vID
  FIELDMAP_WORK*         fieldmap;  // �t�B�[���h�}�b�v
  GOBJ_ELBOARD*           elboard;  // �d���f���Ǘ��I�u�W�F�N�g
  u32               recoveryFrame;  // ���A�t���[��
  ELBOARD_ZONE_DATA*     gateData;  // �Q�[�g�f�[�^
  ELBOARD_SPNEWS_DATA* spNewsData;  // �Վ��j���[�X�f�[�^
  u8                spNewsDataNum;  // �Վ��j���[�X�f�[�^��
  NEWS_ENTRY_DATA   newsEntryData;  // �j���[�X�o�^��

} GATEWORK;


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static void GimmickSave( const GATEWORK* work );
static void GimmickLoad( GATEWORK* work );
static GATEWORK* CreateGateWork( FIELDMAP_WORK* fieldmap );
static void DeleteGateWork( GATEWORK* work );
static BOOL LoadGateData( GATEWORK* work, FIELDMAP_WORK* fieldmap );
static void DeleteGateData( GATEWORK* work );
static void LoadSpNewsData( GATEWORK* work );
static void DeleteSpNewsData( GATEWORK* work );
static void SetElboardPos( GFL_G3D_OBJSTATUS* status, ELBOARD_ZONE_DATA* data );
static void SetupElboardNews( GATEWORK* work );
static void SetupElboardNews_Normal( GATEWORK* work );
static void SetupElboardNews_Special( GATEWORK* work );
static const ELBOARD_SPNEWS_DATA* SearchTopNews( GATEWORK* work );
static const ELBOARD_SPNEWS_DATA* SearchGymNews( GATEWORK* work );
static const ELBOARD_SPNEWS_DATA* SearchFlagNews( GATEWORK* work, u32 flag );
static BOOL CheckSpecialNews( GATEWORK* work );
static void AddNews_DATE( GATEWORK* work );
static void AddNews_PROPAGATION( GATEWORK* work );
static void AddNews_WEATHER( GATEWORK* work );
static void AddNews_INFO_A( GATEWORK* work );
static void AddNews_INFO_B( GATEWORK* work );
static void AddNews_INFO_C( GATEWORK* work );
static void AddNews_CM( GATEWORK* work );
static void AddNews_SPECIAL( GATEWORK* work, const ELBOARD_SPNEWS_DATA* news );
static void AddSpNews_DIRECT( GATEWORK* work,
                              const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx );
static void AddSpNews_CHAMP( GATEWORK* work,
                             const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx );
static void AddSpNews_GYM( GATEWORK* work );


//==========================================================================================
// ���M�~�b�N�o�^�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  GATEWORK*                work = NULL;  // GATE�M�~�b�N�Ǘ����[�N
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &unit[UNIT_ELBOARD], EXPOBJ_UNIT_ELBOARD );

  // �M�~�b�N�Ǘ����[�N���쐬
  work = CreateGateWork( fieldmap );

  // �M�~�b�N�̃Z�[�u�f�[�^��ǂݍ���
  GimmickLoad( work );

  // �j���[�X�Z�b�g�A�b�v
  {
    GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
    GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
    GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
    int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
    SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );

    if( save_buf->newsEntryData.newsNum == 0 )
    { // �V�K
      SetupElboardNews( work );
    }
    else
    { // ���A
      GATE_GIMMICK_Elboard_Recovery( fieldmap );
    }
  }

  // DEBUG:
  OBATA_Printf( "GIMMICK-GATE: setup\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;

  // �M�~�b�N��Ԃ��Z�[�u
  GimmickSave( work );
  
  // �d���f���Ǘ����[�N��j��
  if( work->elboard )
  {
    GOBJ_ELBOARD_Delete( work->elboard );
  }

  // �M�~�b�N�Ǘ����[�N��j��
  DeleteGateWork( work );

  // DEBUG:
  OBATA_Printf( "GIMMICK-GATE: end\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief ����֐�
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;

  // �d���f�����C������
  GOBJ_ELBOARD_Main( work->elboard, FX32_ONE ); 

  // ���j�^�[�A�j���[�V�����Đ�
  {
    FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
    FLD_EXP_OBJ_PlayAnime( exobj_cnt );
  }

#if 0
  // TEST:
  {
    int key = GFL_UI_KEY_GetCont();
    int trg = GFL_UI_KEY_GetTrg();
    if( key & PAD_BUTTON_L )
    {
      if( trg & PAD_BUTTON_A )
      { 
        GATE_GIMMICK_Camera_LookElboard( fieldmap, 30 );
      }
      if( trg & PAD_BUTTON_B )
      { 
        GATE_GIMMICK_Camera_Reset( fieldmap, 30 );
      }
    }
  }
#endif
}


//==========================================================================================
// �� �f����
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �f���ɕ���ʂ�̃j���[�X��ݒ肷��
 *
 * @param fieldmap �t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Elboard_SetupNormalNews( FIELDMAP_WORK* fieldmap )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;

  // ����j���[�X���\��
  SetupElboardNews_Normal( work );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���Ɏw�肵���j���[�X��ǉ�����
 *
 * @param fieldmap �t�B�[���h�}�b�v
 * @param str_id   �ǉ�����j���[�X�̃��b�Z�[�W�ԍ�
 * @param wordset  �w�胁�b�Z�[�W�ɓW�J���郏�[�h�Z�b�g
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Elboard_AddSpecialNews( FIELDMAP_WORK* fieldmap, 
                                          u32 str_id, WORDSET* wordset )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;

  // �d���f�����o�^����Ă��Ȃ��ꏊ�ŌĂ΂ꂽ�牽�����Ȃ�
  if( IsGimmickIDEntried( gmk_id ) != TRUE ){ return; }

  // �j���[�X��ǉ�
  {
    int index;
    NEWS_PARAM news;

    index = GOBJ_ELBOARD_GetNewsNum( work->elboard );
    if( MAX_NEWS_NUM <= index )
    {
      OBATA_Printf( "==========================================\n" );
      OBATA_Printf( "���łɍő吔�̃j���[�X���ݒ肳��Ă��܂��B\n" );
      OBATA_Printf( "==========================================\n" );
      return;
    }
    news.animeIndex = news_anm_index[index];
    news.texName    = news_tex_name[index];
    news.pltName    = news_plt_name[index];
    news.msgArcID   = ARCID_MESSAGE;
    news.msgDatID   = NARC_message_gate_dat;
    news.msgStrID   = str_id;
    news.wordset    = wordset;
    GOBJ_ELBOARD_AddNews( work->elboard, &news );
  }

}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̏�Ԃ𕜋A������(���A�|�C���g�͎����I�ɋL��)
 *
 * @param fieldmap �t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Elboard_Recovery( FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;
  NEWS_ENTRY_DATA* data = &save_buf->newsEntryData;

  // �d���f�����o�^����Ă��Ȃ��ꏊ�ŌĂ΂ꂽ�牽�����Ȃ�
  if( IsGimmickIDEntried( gmk_id ) != TRUE )
  {
    return;
  } 

  // �t���O����
  for( i=0; i<data->newsNum; i++)
  {
    EVENTWORK* evwork = GAMEDATA_GetEventWork( gdata );
    if( data->newsType[i] == NEWS_TYPE_SPECIAL )
    {
      EVENTWORK_SetEventFlag( evwork, data->spNewsFlag[i] );
    }
  }
  // �ăZ�b�g�A�b�v
  SetupElboardNews( work );
#if 0
  // �j���[�X���A
  for( i=0; i<data->newsNum; i++ )
  {
    switch( data->newsType[i] )
    {
    default:                    break;
    case NEWS_TYPE_NULL:        break;
    case NEWS_TYPE_DATE:        AddNews_DATE(work);         break;
    case NEWS_TYPE_WEATHER:     AddNews_WEATHER(work);      break;
    case NEWS_TYPE_PROPAGATION: AddNews_PROPAGATION(work);  break;
    case NEWS_TYPE_INFO_A:      AddNews_INFO_A(work);       break;
    case NEWS_TYPE_INFO_B:      AddNews_INFO_B(work);       break;
    case NEWS_TYPE_INFO_C:      AddNews_INFO_C(work);       break;
    case NEWS_TYPE_CM:          AddNews_CM(work);           break;
    case NEWS_TYPE_SPECIAL:     // �Վ��j���[�X
                                {
                                  u32 flag;
                                  const ELBOARD_SPNEWS_DATA* news;
                                  flag = data->spNewsFlag[i];
                                  news = SearchFlagNews( work, flag );
                                  AddNews_SPECIAL( work, news );
                                }
                                break;
    }
  } 
#endif
  // �f�����A����
  GOBJ_ELBOARD_SetFrame( work->elboard, work->recoveryFrame << FX32_SHIFT );
}


//==========================================================================================
// ���J����
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �J������d���f���Ɍ�����
 *
 * @param fieldmap �t�B�[���h�}�b�v
 * @param frame    �J�����̓���ɗv����t���[����
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Camera_LookElboard( FIELDMAP_WORK* fieldmap, u16 frame )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*        work = (GATEWORK*)save_buf->gateWork;
  fx32 val_len;
  u16 val_pitch, val_yaw;
  VecFx32 val_target;

  // �d���f���f�[�^���擾
  if( !work->gateData )
  { // �擾���s
    OBATA_Printf( "==========================================\n" );
    OBATA_Printf( "GIMMICK-GATE: �d���f���f�[�^�̎擾�Ɏ��s\n" );
    OBATA_Printf( "==========================================\n" );
    return;
  }
  // �d���f���̌����ŃJ�����̉�]������
  switch( work->gateData->dir )
  {
  case DIR_DOWN:
    val_pitch = 0x0ee5;
    val_yaw   = 0;
    val_len   = 0x0086 << FX32_SHIFT;
    VEC_Set( &val_target, 0, 0x001b<<FX32_SHIFT, 0xfff94000 );
    break;
  case DIR_RIGHT: 
    val_pitch = 0x1ad3;
    val_yaw   = 0x3f5d;
    val_len   = 0x0058 << FX32_SHIFT;
    VEC_Set( &val_target, 0xfff5d000, 0x001a<<FX32_SHIFT, 0xfffff000 );
    break;
  case DIR_UP:
  case DIR_LEFT:
  default:
    OBATA_Printf( "==================================\n" );
    OBATA_Printf( "GIMMICK-GATE: �f���̌��������Ή�\n" );
    OBATA_Printf( "==================================\n" );
    return;
  } 
  // �^�X�N�o�^
  {
    FIELD_TASK_MAN* man;
    FIELD_TASK* zoom;
    FIELD_TASK* pitch;
    FIELD_TASK* yaw;
    FIELD_TASK* target;
    zoom   = FIELD_TASK_CameraLinearZoom( fieldmap, frame, val_len );
    pitch  = FIELD_TASK_CameraRot_Pitch( fieldmap, frame, val_pitch );
    yaw    = FIELD_TASK_CameraRot_Yaw( fieldmap, frame, val_yaw );
    target = FIELD_TASK_CameraTargetOffset( fieldmap, frame, &val_target );
    man = FIELDMAP_GetTaskManager( fieldmap );
    FIELD_TASK_MAN_AddTask( man, zoom, NULL );
    FIELD_TASK_MAN_AddTask( man, pitch, NULL );
    FIELD_TASK_MAN_AddTask( man, yaw, NULL );
    FIELD_TASK_MAN_AddTask( man, target, NULL );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �J���������ɖ߂�
 *
 * @param fieldmap �t�B�[���h�}�b�v
 * @param frame    �J�����̓���ɗv����t���[����
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_Camera_Reset( FIELDMAP_WORK* fieldmap, u16 frame )
{
  fx32 val_len;
  u16 val_pitch, val_yaw;
  VecFx32 val_target = {0, 0, 0};
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );
  FLD_CAMERA_PARAM def_param;
  // �f�t�H���g�p�����[�^�擾
  FIELD_CAMERA_GetInitialParameter( camera, &def_param );
  val_len   = def_param.Distance << FX32_SHIFT;
  val_pitch = def_param.Angle.x;
  val_yaw   = def_param.Angle.y;
  // �^�X�N�o�^
 {
    FIELD_TASK_MAN* man;
    FIELD_TASK* zoom;
    FIELD_TASK* pitch;
    FIELD_TASK* yaw;
    FIELD_TASK* target;
    zoom   = FIELD_TASK_CameraLinearZoom( fieldmap, frame, val_len );
    pitch  = FIELD_TASK_CameraRot_Pitch( fieldmap, frame, val_pitch );
    yaw    = FIELD_TASK_CameraRot_Yaw( fieldmap, frame, val_yaw );
    target = FIELD_TASK_CameraTargetOffset( fieldmap, frame, &val_target );
    man = FIELDMAP_GetTaskManager( fieldmap );
    FIELD_TASK_MAN_AddTask( man, zoom, NULL );
    FIELD_TASK_MAN_AddTask( man, pitch, NULL );
    FIELD_TASK_MAN_AddTask( man, yaw, NULL );
    FIELD_TASK_MAN_AddTask( man, target, NULL );
  }
}


//==========================================================================================
// �� ����J�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N��Ԃ�ۑ�����
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void GimmickSave( const GATEWORK* work )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  GATEWORK*       gwork = (GATEWORK*)save_buf->gateWork;

  // �f���̓���t���[�������L��
  save_buf->gateWork      = (void*)work;
  save_buf->recoveryFrame = GOBJ_ELBOARD_GetFrame( gwork->elboard ) >> FX32_SHIFT; 
  save_buf->newsEntryData = gwork->newsEntryData;

  // DEBUG:
  {
    int i;
    OBATA_Printf( "GIMMICK-GATE: gimmick save\n" );
    OBATA_Printf( "-recoveryFrame = %d\n", save_buf->recoveryFrame );
    OBATA_Printf( "-newsEntryData.newsNum = %d\n",  save_buf->newsEntryData.newsNum );
    for( i=0; i<NEWS_INDEX_NUM; i++ )
    {
      OBATA_Printf( "-newsEntryData.newsType[%d] = ", i );
      switch( save_buf->newsEntryData.newsType[i] )
      {
      case NEWS_TYPE_NULL:        OBATA_Printf( "NULL\n" );        break;
      case NEWS_TYPE_DATE:        OBATA_Printf( "DATE\n" );        break;
      case NEWS_TYPE_WEATHER:     OBATA_Printf( "WEATHER\n" );     break;
      case NEWS_TYPE_PROPAGATION: OBATA_Printf( "PROPAGATION\n" ); break;
      case NEWS_TYPE_INFO_A:      OBATA_Printf( "INFO_A\n" );      break;
      case NEWS_TYPE_INFO_B:      OBATA_Printf( "INFO_B\n" );      break;
      case NEWS_TYPE_INFO_C:      OBATA_Printf( "INFO_C\n" );      break;
      case NEWS_TYPE_CM:          OBATA_Printf( "CM\n" );          break;
      case NEWS_TYPE_SPECIAL:     OBATA_Printf( "SPECIAL\n" );     break;
      default:                    OBATA_Printf( "UNKNOWN\n" );     break;
      }
    }
    for( i=0; i<NEWS_INDEX_NUM; i++ )
    {
      OBATA_Printf( "-newsEntryData.spNewsFlag[%d] = %d\n",
                    i, save_buf->newsEntryData.spNewsFlag[i] );
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�̃Z�[�u�f�[�^��ǂݍ���
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void GimmickLoad( GATEWORK* work )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork( gdata );
  int            gmk_id = GIMMICKWORK_GetAssignID( gmkwork );
  SAVEWORK*    save_buf = NULL;

  // �Z�[�u�f�[�^�擾
  save_buf = (SAVEWORK*)GIMMICKWORK_Get( gmkwork, gmk_id );
  work->recoveryFrame = save_buf->recoveryFrame;  // �f���̕��A�|�C���g

  // �M�~�b�N�Ǘ����[�N�̃A�h���X���L��
  save_buf->gateWork = work; 

  // DEBUG:
  {
    int i;
    OBATA_Printf( "GIMMICK-GATE: gimmick load\n" );
    OBATA_Printf( "-recoveryFrame = %d\n", save_buf->recoveryFrame );
    OBATA_Printf( "-newsEntryData.newsNum = %d\n",  save_buf->newsEntryData.newsNum );
    for( i=0; i<NEWS_INDEX_NUM; i++ )
    {
      OBATA_Printf( "-newsEntryData.newsType[%d] = ", i );
      switch( save_buf->newsEntryData.newsType[i] )
      {
      case NEWS_TYPE_NULL:        OBATA_Printf( "NULL\n" );        break;
      case NEWS_TYPE_DATE:        OBATA_Printf( "DATE\n" );        break;
      case NEWS_TYPE_WEATHER:     OBATA_Printf( "WEATHER\n" );     break;
      case NEWS_TYPE_PROPAGATION: OBATA_Printf( "PROPAGATION\n" ); break;
      case NEWS_TYPE_INFO_A:      OBATA_Printf( "INFO_A\n" );      break;
      case NEWS_TYPE_INFO_B:      OBATA_Printf( "INFO_B\n" );      break;
      case NEWS_TYPE_INFO_C:      OBATA_Printf( "INFO_C\n" );      break;
      case NEWS_TYPE_CM:          OBATA_Printf( "CM\n" );          break;
      case NEWS_TYPE_SPECIAL:     OBATA_Printf( "SPECIAL\n" );     break;
      default:                    OBATA_Printf( "UNKNOWN\n" );     break;
      }
    }
    for( i=0; i<NEWS_INDEX_NUM; i++ )
    {
      OBATA_Printf( "-newsEntryData.spNewsFlag[%d] = %d\n",
                    i, save_buf->newsEntryData.spNewsFlag[i] );
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�Ǘ����[�N���쐬����
 *
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 *
 * @return �쐬�����M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static GATEWORK* CreateGateWork( FIELDMAP_WORK* fieldmap )
{
  GATEWORK*                work = NULL;
  HEAPID                heap_id = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �M�~�b�N�Ǘ����[�N���쐬
  work = (GATEWORK*)GFL_HEAP_AllocMemory( heap_id, sizeof(GATEWORK) );
  GFL_STD_MemClear( work, sizeof(GATEWORK) );

  // ������
  work->heapID   = heap_id;
  work->fieldmap = fieldmap;

  // �d���f���Ǘ����[�N���쐬
  {
    ELBOARD_PARAM param;
    param.heapID       = heap_id;
    param.maxNewsNum   = NEWS_INDEX_NUM;
    param.dispSize     = DISPLAY_SIZE;
    param.newsInterval = NEWS_INTERVAL;
    param.g3dObj       = FLD_EXP_OBJ_GetUnitObj( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD );
    work->elboard      = GOBJ_ELBOARD_Create( &param );
  } 

  // �d���f���f�[�^���擾
  if( !LoadGateData( work, fieldmap ) )
  {
    // �f�[�^���擾�ł��Ȃ�������, �ȍ~�̏������𒆒f
    return work;
  }
  // �Վ��j���[�X�f�[�^���擾
  LoadSpNewsData( work );

  // �d���f����z�u
  { 
    GFL_G3D_OBJSTATUS* status = 
      FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD );
    SetElboardPos( status, work->gateData );
  } 

  // ���j�^�[�E�A�j���[�V�����J�n
  FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD, 
                           monitor_anime[work->gateData->monitorAnimeIndex], TRUE ); 

  return work;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�Ǘ����[�N��j������
 *
 * @param work �j�����郏�[�N
 */
//------------------------------------------------------------------------------------------
static void DeleteGateWork( GATEWORK* work )
{
  DeleteGateData( work );      // �Q�[�g�f�[�^
  DeleteSpNewsData( work );    // �Վ��j���[�X�f�[�^
  GFL_HEAP_FreeMemory( work ); // �{��
}

//------------------------------------------------------------------------------------------
/**
 * @brief �]�[���ɉ������d���f���f�[�^���擾����
 *
 * @param work     �ǂݍ��񂾃f�[�^��ێ����郏�[�N
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �f�[�^���������ǂݍ��߂��� TRUE
 */
//------------------------------------------------------------------------------------------
static BOOL LoadGateData( GATEWORK* work, FIELDMAP_WORK* fieldmap )
{
  int i;
  u16 zone_id;

  // ���łɓǂݍ��܂�Ă���
  if( work->gateData ){ return FALSE; }

  // �]�[��ID���擾
  zone_id = FIELDMAP_GetZoneID( fieldmap );

  // �d���f���f�[�^�e�[�u������, �Y������f�[�^������
  for( i=0; i<NELEMS(entry_table); i++ )
  {
    if( entry_table[i].zone_id == zone_id )
    {
      // �d���f���f�[�^��ǂݍ���
      work->gateData = GFL_HEAP_AllocMemory( work->heapID, sizeof(ELBOARD_ZONE_DATA) );
      ELBOARD_ZONE_DATA_Load( work->gateData, ARCID_GATE_GIMMICK, entry_table[i].dat_id );
      return TRUE;
    }
  } 

  // ���݂̃]�[���ɑΉ�����d���f���f�[�^���o�^����Ă��Ȃ��ꍇ
  OBATA_Printf( "=============================================\n" );
  OBATA_Printf( "error: �d���f���f�[�^���o�^����Ă��܂���B\n" );
  OBATA_Printf( "=============================================\n" );
  return FALSE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Q�[�g�f�[�^��j������
 *
 * @param work �j������Q�[�g�f�[�^��ێ����郏�[�N
 */
//------------------------------------------------------------------------------------------
static void DeleteGateData( GATEWORK* work )
{
  if( work->gateData )
  {
    GFL_HEAP_FreeMemory( work->gateData );
    work->gateData = NULL;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Վ��j���[�X�f�[�^��ǂݍ���
 *
 * @param work �ǂݍ��񂾃f�[�^��ێ����郏�[�N
 */
//------------------------------------------------------------------------------------------
static void LoadSpNewsData( GATEWORK* work )
{

  // ���łɓǂݍ��܂�Ă���
  if( work->spNewsData ){ return; }

  // �S�f�[�^��ǂݍ���
  {
    int data_idx;
    int data_num;

    // �f�[�^���擾
    data_num = GFL_ARC_GetDataFileCnt( ARCID_ELBOARD_SPNEWS );

    // �e�f�[�^���擾
    work->spNewsData = GFL_HEAP_AllocMemory( work->heapID, 
                                             sizeof(ELBOARD_SPNEWS_DATA) * data_num );
    for( data_idx=0; data_idx<data_num; data_idx++ )
    {
      ELBOARD_SPNEWS_DATA_Load( &work->spNewsData[data_idx], ARCID_ELBOARD_SPNEWS, data_idx );
    }
    work->spNewsDataNum = data_num;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Վ��j���[�X�f�[�^��j������
 *
 * @param �j������f�[�^��ێ����郏�[�N
 */
//------------------------------------------------------------------------------------------
static void DeleteSpNewsData( GATEWORK* work )
{
  if( work->spNewsData )
  {
    GFL_HEAP_FreeMemory( work->spNewsData );
    work->spNewsData = NULL;
    work->spNewsDataNum = 0;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �d���f����z�u����
 *
 * @param status �d���f���̃I�u�W�F�X�e�[�^�X
 * @param data   �d���f���f�[�^
 */
//------------------------------------------------------------------------------------------
static void SetElboardPos( GFL_G3D_OBJSTATUS* status, ELBOARD_ZONE_DATA* data )
{
  u16 rot;

  // ���W��ݒ�
  status->trans.x = data->x << FX32_SHIFT;
  status->trans.y = data->y << FX32_SHIFT;
  status->trans.z = data->z << FX32_SHIFT;

  // ������ݒ�
  switch( data->dir )
  {
  case DIR_DOWN:  rot = 0;    break;
  case DIR_RIGHT: rot = 90;   break;
  case DIR_UP:    rot = 180;  break;
  case DIR_LEFT:  rot = 270;  break;
  default:        rot = 0;    break;
  }
  rot *= 182;  // 65536��360 = 182.044...
  GFL_CALC3D_MTX_CreateRot( 0, rot, 0, &status->rotate );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �d���f���� �j���[�X�Z�b�g�A�b�v
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews( GATEWORK* work )
{
  if( CheckSpecialNews(work) )  // if(�Վ��j���[�X�L)
  { // �Վ��j���[�X
    SetupElboardNews_Special( work ); 
  }
  else
  { // ����j���[�X
    SetupElboardNews_Normal( work );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���ɕ\��������� ����j���[�X�ō\������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews_Normal( GATEWORK* work )
{
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );

  // ���łɑ��̃j���[�X���o�^����Ă���ꍇ
  {
    int num = GOBJ_ELBOARD_GetNewsNum( work->elboard );
    if( num != 0 )
    {
      OBATA_Printf( "======================================\n" );
      OBATA_Printf( "���łɑ��̃j���[�X���ݒ肳��Ă��܂��B\n" );
      OBATA_Printf( "======================================\n" );
      return;
    }
  } 
  // �f�[�^�������Ă��Ȃ�
  if( !work->gateData ){ return; }

  // �j���[�X��ǉ�
  AddNews_DATE( work );         // ���t
  AddNews_WEATHER( work );      // �V�C
  AddNews_PROPAGATION( work );  // ��ʔ���
  AddNews_INFO_A( work );       // �n����A
  AddNews_INFO_B( work );       // �n����B
  AddNews_INFO_C( work );       // �n����C
  AddNews_CM( work );           // �ꌾCM
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���ɕ\��������� �Վ��j���[�X�ō\������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void SetupElboardNews_Special( GATEWORK* work )
{ 
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );

  // ���łɑ��̃j���[�X���o�^����Ă���ꍇ
  {
    int num = GOBJ_ELBOARD_GetNewsNum( work->elboard );
    if( num != 0 )
    {
      OBATA_Printf( "======================================\n" );
      OBATA_Printf( "���łɑ��̃j���[�X���ݒ肳��Ă��܂��B\n" );
      OBATA_Printf( "======================================\n" );
      return;
    }
  } 
  // �f�[�^�������Ă��Ȃ�
  if( !work->spNewsData ){ return; }

  // �j���[�X��ǉ�
  AddNews_DATE( work );         // ���t
  AddNews_WEATHER( work );      // �V�C
  AddNews_PROPAGATION( work );  // ��ʔ���
  { // �Վ��j���[�X
    const ELBOARD_SPNEWS_DATA* news;
    news = SearchTopNews( work );
    AddNews_SPECIAL( work, news );
  }
  AddNews_CM( work );           // �ꌾCM
}

//------------------------------------------------------------------------------------------
/**
 * @brief �\�����ׂ��j���[�X�̒���, �ł��D�揇�ʂ̍����Վ��j���[�X����������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 *
 * @return �ł��D�揇�ʂ̍���, �\�����ׂ��Վ��j���[�X
 *         �\�����ׂ��j���[�X���Ȃ��ꍇ NULL
 */
//------------------------------------------------------------------------------------------
static const ELBOARD_SPNEWS_DATA* SearchTopNews( GATEWORK* work )
{
  int i;
  u32        zone_id = FIELDMAP_GetZoneID( work->fieldmap );
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
  EVENTWORK*  evwork = GAMEDATA_GetEventWork( gdata );

  // �f�[�^�������Ă��Ȃ�
  if( !work->spNewsData ){ return NULL; }

  // �Վ��j���[�X�f�[�^������
  for( i=0; i<work->spNewsDataNum; i++ )
  { 
    BOOL flag_hit = EVENTWORK_CheckEventFlag( evwork, work->spNewsData[i].flag );
    BOOL zone_hit = ELBOARD_SPNEWS_DATA_CheckZoneHit( &work->spNewsData[i], zone_id );
    if( flag_hit && zone_hit )  // if(�t���OON && �]�[����v)
    {
      return &work->spNewsData[i];
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �\�����ׂ��j���[�X�̒���, �ł��D�揇�ʂ̍����Վ��W���j���[�X����������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 *
 * @return �ł��D�揇�ʂ̍���, �\�����ׂ��Վ��j���[�X
 *         �\�����ׂ��j���[�X���Ȃ��ꍇ NULL
 */
//------------------------------------------------------------------------------------------
static const ELBOARD_SPNEWS_DATA* SearchGymNews( GATEWORK* work )
{
  int i;
  u32        zone_id = FIELDMAP_GetZoneID( work->fieldmap );
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
  EVENTWORK*  evwork = GAMEDATA_GetEventWork( gdata );

  // �f�[�^�������Ă��Ȃ�
  if( !work->spNewsData ){ return NULL; }

  // �Վ��j���[�X�f�[�^������
  for( i=0; i<work->spNewsDataNum; i++ )
  { 
    BOOL flag_hit = EVENTWORK_CheckEventFlag( evwork, work->spNewsData[i].flag );
    BOOL zone_hit = ELBOARD_SPNEWS_DATA_CheckZoneHit( &work->spNewsData[i], zone_id );
    if( flag_hit && zone_hit )  // if(�t���OON && �]�[����v)
    {
      if( work->spNewsData[i].newsType == SPNEWS_TYPE_GYM )  // if(�W���j���[�X)
      {
        return &work->spNewsData[i];
      }
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �w�肵���t���O�ɑΉ�����j���[�X����������
 *
 * @param work �M�~�b�N�Ǘ����[�N
 * @param flag ��������t���O
 *
 * @return �w�肵���t���O�ɑΉ�����j���[�X
 *         �j���[�X���Ȃ��ꍇ NULL
 */
//------------------------------------------------------------------------------------------
static const ELBOARD_SPNEWS_DATA* SearchFlagNews( GATEWORK* work, u32 flag )
{
  int i;

  // �f�[�^�������Ă��Ȃ�
  if( !work->spNewsData ){ return NULL; }

  // �Վ��j���[�X�f�[�^������
  for( i=0; i<work->spNewsDataNum; i++ )
  { 
    if( work->spNewsData[i].flag == flag )
    {
      return &work->spNewsData[i];
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Վ��j���[�X�̗L���𒲂ׂ�
 *
 * @param work �M�~�b�N�Ǘ����[�N
 *
 * @return �\�����ׂ��Վ��j���[�X������ꍇ TRUE, �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------------------
static BOOL CheckSpecialNews( GATEWORK* work )
{
  return (SearchTopNews(work) != NULL);
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(���t)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_DATE( GATEWORK* work )
{
  NEWS_PARAM news;
  WORDSET* wordset;
  RTCDate date;

  // ���[�h�Z�b�g�쐬
  wordset = WORDSET_Create( work->heapID );
  RTC_GetDate( &date );
  WORDSET_RegisterNumber( wordset, 0, date.month, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterNumber( wordset, 1, date.day,   2, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );

  // �j���[�X�p�����[�^���쐬
  news.animeIndex = news_anm_index[NEWS_INDEX_DATE];
  news.texName    = news_tex_name[NEWS_INDEX_DATE];
  news.pltName    = news_plt_name[NEWS_INDEX_DATE];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = work->gateData->msgID_date;
  news.wordset    = wordset;

  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // ���[�h�Z�b�g�j��
  WORDSET_Delete( wordset );

  // �o�^�󋵂��X�V
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_DATE, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�V�C)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_WEATHER( GATEWORK* work )
{
  int i;
  NEWS_PARAM news;
  WORDSET* wordset;
  u32 zone_id[WEATHER_ZONE_NUM];

  // �\������]�[�����X�g���쐬
  zone_id[0] = work->gateData->zoneID_weather_1;
  zone_id[1] = work->gateData->zoneID_weather_2;
  zone_id[2] = work->gateData->zoneID_weather_3;
  zone_id[3] = work->gateData->zoneID_weather_4;

  // ���[�h�Z�b�g�쐬
  wordset = WORDSET_CreateEx( WEATHER_ZONE_NUM, 256, work->heapID );

  // ���[�h�Z�b�g�o�^����
  {
    GFL_MSGDATA* msg_place_name;
    GFL_MSGDATA* msg_gate;

    // ���b�Z�[�W�f�[�^ �n���h���I�[�v��
    msg_place_name = GFL_MSG_Create( 
        GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_place_name_dat, work->heapID ); 
    msg_gate = GFL_MSG_Create( 
        GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_gate_dat, work->heapID ); 

    // �e�]�[���̒n���{�V�C��o�^
    for( i=0; i<WEATHER_ZONE_NUM; i++ )
    {
      STRBUF* strbuf_zone;    // �n��
      STRBUF* strbuf_weather; // �V�C
      STRBUF* strbuf_set;     // �n���{�V�C

      // �]�[���������l�Ȃ�\�����Ȃ�
      if( zone_id[i] == ZONE_ID_MAX ) 
      {
        continue;
      }
      // �n�����擾
      {
        int str_id = ZONEDATA_GetPlaceNameID( zone_id[i] );
        strbuf_zone = GFL_MSG_CreateString( msg_place_name, str_id );
      }
      // �V�C���擾
      {
        int weather = ZONEDATA_GetWeatherID( zone_id[i] );
        strbuf_weather = GFL_MSG_CreateString( msg_gate, str_id_weather[weather] );
      }
      // �n���{�V�C�̃Z�b�g���쐬
      {
        WORDSET* wordset_expand = WORDSET_CreateEx( 2, 256, work->heapID );
        STRBUF* strbuf_expand = GFL_MSG_CreateString( msg_gate, msg_gate_weather );
        strbuf_set = GFL_STR_CreateBuffer( 64, work->heapID );
        WORDSET_RegisterWord( wordset_expand, 0, strbuf_zone, 0, TRUE, 0 );
        WORDSET_RegisterWord( wordset_expand, 1, strbuf_weather, 0, TRUE, 0 );
        WORDSET_ExpandStr( wordset_expand, strbuf_set, strbuf_expand );
        GFL_STR_DeleteBuffer( strbuf_expand );
        WORDSET_Delete( wordset_expand );
      }
      // �쐬��������������[�h�Z�b�g�ɓo�^
      WORDSET_RegisterWord( wordset, i, strbuf_set, 0, TRUE, 0 );
      // ��n��
      GFL_STR_DeleteBuffer( strbuf_zone );
      GFL_STR_DeleteBuffer( strbuf_weather );
      GFL_STR_DeleteBuffer( strbuf_set );
    }

    // ���b�Z�[�W�f�[�^ �n���h���N���[�Y
    GFL_MSG_Delete( msg_gate );
    GFL_MSG_Delete( msg_place_name );
  }

  // �j���[�X�p�����[�^���쐬
  news.animeIndex = news_anm_index[NEWS_INDEX_WEATHER];
  news.texName    = news_tex_name[NEWS_INDEX_WEATHER];
  news.pltName    = news_plt_name[NEWS_INDEX_WEATHER];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = work->gateData->msgID_weather;
  news.wordset    = wordset;

  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // ���[�h�Z�b�g�j��
  WORDSET_Delete( wordset );

  // �o�^�󋵂��X�V
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_WEATHER, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(��ʔ���)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_PROPAGATION( GATEWORK* work )
{
  GAMESYS_WORK* gsys;
  GAMEDATA* gdata;
  NEWS_PARAM news;
  WORDSET* wordset;
  u16 zone_id;

  // ��ʔ������N���Ă���]�[�����擾
  gsys    = FIELDMAP_GetGameSysWork( work->fieldmap );
  gdata   = GAMESYSTEM_GetGameData( gsys );
  zone_id = ENCPOKE_GetGenerateZone( gdata ); 
  // ��ʔ������N���Ă��Ȃ�
  if( zone_id == 0xFFFF ){ return; }

  // ���[�h�Z�b�g�쐬
  wordset = WORDSET_Create( work->heapID );

  // ���[�h�Z�b�g�ɒn�����Z�b�g
  {
    GFL_MSGDATA* msg_place_name;
    int str_id;
    STRBUF* zone_name;

    msg_place_name = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
                                     ARCID_MESSAGE, NARC_message_place_name_dat, work->heapID ); 
    str_id         = ZONEDATA_GetPlaceNameID( zone_id );
    zone_name      = GFL_MSG_CreateString( msg_place_name, str_id );
    WORDSET_RegisterWord( wordset, 0, zone_name, 0, TRUE, 0 );
    GFL_STR_DeleteBuffer( zone_name );
    GFL_MSG_Delete( msg_place_name );
  }

  // �j���[�X�p�����[�^���쐬
  news.animeIndex = news_anm_index[NEWS_INDEX_PROPAGATION];
  news.texName    = news_tex_name[NEWS_INDEX_PROPAGATION];
  news.pltName    = news_plt_name[NEWS_INDEX_PROPAGATION];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = work->gateData->msgID_propagation;
  news.wordset    = wordset;

  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // ���[�h�Z�b�g�j��
  WORDSET_Delete( wordset );

  // �o�^�󋵂��X�V
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_PROPAGATION, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�n����A)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_INFO_A( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // �����f�[�^���擾
  RTC_GetDate( &date ); 

  // �j���ɉ���������I��
  switch( date.week )
  {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_infoG;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_infoD;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_infoA;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_infoE;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_infoB;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_infoF;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_infoC;  break;
  }

  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_A];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_A];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_A];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;

  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // �o�^�󋵂��X�V
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_INFO_A, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�n����B)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_INFO_B( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // �����f�[�^���擾
  RTC_GetDate( &date ); 

  // �j���ɉ���������I��
  switch( date.week )
  {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_infoH;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_infoE;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_infoB;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_infoF;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_infoC;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_infoG;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_infoD;  break;
  }

  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_B];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_B];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_B];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;

  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // �o�^�󋵂��X�V
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_INFO_B, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�n����C)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_INFO_C( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // �����f�[�^���擾
  RTC_GetDate( &date ); 

  // �j���ɉ���������I��
  switch( date.week )
  {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_infoI;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_infoF;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_infoC;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_infoG;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_infoD;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_infoH;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_infoE;  break;
  }

  news.animeIndex = news_anm_index[NEWS_INDEX_INFO_C];
  news.texName    = news_tex_name[NEWS_INDEX_INFO_C];
  news.pltName    = news_plt_name[NEWS_INDEX_INFO_C];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;

  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // �o�^�󋵂��X�V
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_INFO_C, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�ꌾCM)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddNews_CM( GATEWORK* work )
{
  NEWS_PARAM news;
  RTCDate date;

  // �����f�[�^���擾
  RTC_GetDate( &date );

  // �j���ɉ�����CM��I��
  switch( date.week )
  {
  case RTC_WEEK_SUNDAY:    news.msgStrID = work->gateData->msgID_cmSun;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = work->gateData->msgID_cmMon;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = work->gateData->msgID_cmTue;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = work->gateData->msgID_cmWed;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = work->gateData->msgID_cmThu;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = work->gateData->msgID_cmFri;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = work->gateData->msgID_cmSat;  break;
  }
  news.animeIndex = news_anm_index[NEWS_INDEX_CM];
  news.texName    = news_tex_name[NEWS_INDEX_CM];
  news.pltName    = news_plt_name[NEWS_INDEX_CM];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.wordset    = NULL;
  
  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // �o�^�󋵂��X�V
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_CM, 0 );
} 

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̃j���[�X��ǉ�����(�Վ��j���[�X)
 * 
 * @param work   �M�~�b�N�Ǘ����[�N
 * @param news   �ǉ�����Վ��j���[�X�f�[�^
 */
//------------------------------------------------------------------------------------------
static void AddNews_SPECIAL( GATEWORK* work, const ELBOARD_SPNEWS_DATA* news )
{ 
  // �j���[�X��ǉ�
  switch( news->newsType )
  {
  case SPNEWS_TYPE_DIRECT: AddSpNews_DIRECT( work, news, NEWS_INDEX_INFO_A );  break;
  case SPNEWS_TYPE_CHAMP:  AddSpNews_CHAMP( work, news, NEWS_INDEX_INFO_A );   break;
  case SPNEWS_TYPE_GYM:    AddSpNews_GYM( work );                              break;
  }
} 

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̗Վ��j���[�X��ǉ�����(���b�Z�[�W���̂܂�)
 *
 * @param work     �M�~�b�N�Ǘ����[�N
 * @param spnews   �Վ��j���[�X�f�[�^
 * @param news_idx �j���[�X�̒ǉ��ꏊ���w��
 */
//------------------------------------------------------------------------------------------
static void AddSpNews_DIRECT( GATEWORK* work, 
                              const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx )
{
  NEWS_PARAM news;

  news.animeIndex = news_anm_index[news_idx];
  news.texName    = news_tex_name[news_idx];
  news.pltName    = news_plt_name[news_idx];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = spnews->msgID;
  news.wordset    = NULL;
  
  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // �t���O����
  if( spnews->flagControl == FLAG_CONTROL_RESET )
  { // �t���O�𗎂Ƃ�
    GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
    GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
    EVENTWORK*  evwork = GAMEDATA_GetEventWork( gdata ); 
    EVENTWORK_ResetEventFlag( evwork, spnews->flag );
  }

  // �o�^�󋵂��X�V
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_SPECIAL, spnews->flag );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̗Վ��j���[�X��ǉ�����(�`�����s�I�����)
 *
 * @param work     �M�~�b�N�Ǘ����[�N
 * @param spnews   �Վ��j���[�X�f�[�^
 * @param news_idx �j���[�X�̒ǉ��ꏊ���w��
 *
 * @todo �|�P�������𐳂����Z�b�g����
 */
//------------------------------------------------------------------------------------------
static void AddSpNews_CHAMP( GATEWORK* work,
                             const ELBOARD_SPNEWS_DATA* spnews, NEWS_INDEX news_idx )
{
  HEAPID heap_id; 
  NEWS_PARAM news;
  WORDSET* wordset;

  // ���[�h�Z�b�g�쐬
  heap_id = GOBJ_ELBOARD_GetHeapID( work->elboard );
  wordset = WORDSET_Create( heap_id );

  // �|�P���������Z�b�g
  WORDSET_RegisterPokeMonsNameNo( wordset, 0, 1 );
  WORDSET_RegisterPokeMonsNameNo( wordset, 1, 2 );
  WORDSET_RegisterPokeMonsNameNo( wordset, 2, 3 );
  WORDSET_RegisterPokeMonsNameNo( wordset, 3, 4 );
  WORDSET_RegisterPokeMonsNameNo( wordset, 4, 5 );
  WORDSET_RegisterPokeMonsNameNo( wordset, 5, 6 );

  // �j���[�X�p�����[�^���쐬
  news.animeIndex = news_anm_index[news_idx];
  news.texName    = news_tex_name[news_idx];
  news.pltName    = news_plt_name[news_idx];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = NARC_message_gate_dat;
  news.msgStrID   = spnews->msgID;
  news.wordset    = wordset;

  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( work->elboard, &news );

  // ���[�h�Z�b�g�j��
  WORDSET_Delete( wordset );

  // �t���O����
  if( spnews->flagControl == FLAG_CONTROL_RESET )
  { // �t���O�𗎂Ƃ�
    GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( work->fieldmap );
    GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
    EVENTWORK*  evwork = GAMEDATA_GetEventWork( gdata ); 
    EVENTWORK_ResetEventFlag( evwork, spnews->flag );
  }

  // �o�^�󋵂��X�V
  AddNewsEntryData( &work->newsEntryData, NEWS_TYPE_SPECIAL, spnews->flag );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �f���̗Վ��j���[�X��ǉ�����(�W�����)
 *
 * @param work �M�~�b�N�Ǘ����[�N
 */
//------------------------------------------------------------------------------------------
static void AddSpNews_GYM( GATEWORK* work )
{
  // �j���[�X�ǉ��ꏊ
  const ELBOARD_SPNEWS_DATA* news;
  int count = 0;

  // �S�ẴW���j���[�X��\������
  news = SearchGymNews( work ); 
  while( news && count<NELEMS(gym_news_idx) )
  {
    // �j���[�X��ǉ�
    AddSpNews_DIRECT( work, news, gym_news_idx[count++] );

    // ���̃W���j���[�X���擾
    news = SearchGymNews( work );
  } 
}
