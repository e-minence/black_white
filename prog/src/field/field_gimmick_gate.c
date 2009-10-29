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
#include "fieldmap.h" 
#include "field_gimmick_gate.h" 
#include "arc/arc_def.h" 
#include "arc/gate.naix"
#include "arc/message.naix"
#include "msg/msg_place_name.h" // TEMP:

#include "field/gimmick_obj_elboard.h"
#include "savedata/gimmickwork.h"
#include "field_gimmick_def.h"
#include "gimmick_obj_elboard.h"
#include "elboard_zone_data.h"
#include "arc/gate.naix"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"


//==========================================================================================
// ���d���f���f�[�^�o�^�e�[�u��
//==========================================================================================
typedef struct
{
  u16 zone_id;
  u8  gimmick_id;
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
// �t�B�[���h�}�b�v�ɑΉ�����M�~�b�NID���擾����
static u8 GetGimmickID( FIELDMAP_WORK* fieldmap )
{
  int i;
  u16 zone_id = FIELDMAP_GetZoneID( fieldmap );
  for( i=0; i<NELEMS(entry_table); i++ )
  {
    if( entry_table[i].zone_id == zone_id )
    {
      return entry_table[i].gimmick_id;
    }
  }
  return 0;
}


//==========================================================================================
// ���萔
//==========================================================================================
// �j���[�X�ԍ�
typedef enum
{
  NEWS_DATE,      // ����
  NEWS_WEATHER,   // �V�C
  NEWS_INFO_A,    // ���A
  NEWS_INFO_B,    // ���B
  NEWS_INFO_C,    // ���C
  NEWS_CM,        // �ꌾCM
  NEWS_NUM,
  NEWS_MAX = NEWS_NUM-1
} NEWS_INDEX; 

// ��x�ɕ\������n����̐�
#define LOCAL_INFO_NUM (3)

// �g���I�u�W�F�N�g�E���j�b�g�ԍ�
#define EXPOBJ_UNIT_ELBOARD (1)

// �\���\�Ȉ��ʂɕ\���\�ȕ�����
#define DISPLAY_SIZE (8)

// �j���[�X�Ԃ̊Ԋu(������)
#define NEWS_INTERVAL (3)



//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
// ���\�[�X�C���f�b�N�X
typedef enum
{
  RES_ELBOARD_NSBMD,    // �f���̃��f��
  RES_ELBOARD_NSBTX,    // �f���̃e�N�X�`��
  RES_ELBOARD_NSBTA_1,  // �X�N���[���A�j���[�V����1
  RES_ELBOARD_NSBTA_2,  // �X�N���[���A�j���[�V����2
  RES_ELBOARD_NSBTA_3,  // �X�N���[���A�j���[�V����3
  RES_ELBOARD_NSBTA_4,  // �X�N���[���A�j���[�V����4
  RES_ELBOARD_NSBTA_5,  // �X�N���[���A�j���[�V����5
  RES_ELBOARD_NSBTA_6,  // �X�N���[���A�j���[�V����6
  RES_NUM
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[] = 
{
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_nsbtx, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_1_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_2_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_3_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_4_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_5_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_GATE_GIMMICK, NARC_gate_gelboard01_6_nsbta, GFL_G3D_UTIL_RESARC },
};

// �A�j���C���f�b�N�X
typedef enum
{
  ANM_ELBOARD_DATE,    // ���t
  ANM_ELBOARD_WEATHER, // �V�C
  ANM_ELBOARD_INFO_A,  // ���A
  ANM_ELBOARD_INFO_B,  // ���B
  ANM_ELBOARD_INFO_C,  // ���C
  ANM_ELBOARD_CM,      // �ꌾCM
  ANM_NUM
} ANM_INDEX;
static const GFL_G3D_UTIL_ANM anm_table[] = 
{
  // �A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ELBOARD_NSBTA_1, 0 },
  { RES_ELBOARD_NSBTA_2, 0 },
  { RES_ELBOARD_NSBTA_3, 0 },
  { RES_ELBOARD_NSBTA_4, 0 },
  { RES_ELBOARD_NSBTA_5, 0 },
  { RES_ELBOARD_NSBTA_6, 0 },
};

// �I�u�W�F�N�g�C���f�b�N�X
typedef enum
{
  OBJ_ELBOARD,  // �d���f����
  OBJ_NUM
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[] = 
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
  UNIT_ELBOARD, // �d���f����
  UNIT_NUM
} UNIT_INDEX;
static const GFL_G3D_UTIL_SETUP unit[UNIT_NUM] =
{
  { res_table, RES_NUM, obj_table, OBJ_NUM },
};


//==========================================================================================
// ���j���[�X�p�����[�^
//==========================================================================================
// �A�j���[�V�����E�C���f�b�N�X
static u16 news_anm_index[NEWS_NUM] = 
{
  ANM_ELBOARD_DATE,    // ���t
  ANM_ELBOARD_WEATHER, // �V�C
  ANM_ELBOARD_INFO_A,  // ���A
  ANM_ELBOARD_INFO_B,  // ���B
  ANM_ELBOARD_INFO_C,  // ���C
  ANM_ELBOARD_CM,      // �ꌾCM
};
// �e�N�X�`����
static char* news_tex_name[NEWS_NUM] =
{
  "gelboard_1",
  "gelboard_2",
  "gelboard_3",
  "gelboard_4",
  "gelboard_5",
  "gelboard_6",
};
// �p���b�g��
static char* news_plt_name[NEWS_NUM] =
{
  "gelboard_1_pl",
  "gelboard_2_pl",
  "gelboard_3_pl",
  "gelboard_4_pl",
  "gelboard_5_pl",
  "gelboard_6_pl",
};


//==========================================================================================
// ���M�~�b�N���[�N
//==========================================================================================
typedef struct
{ 
  HEAPID        heapID;   // �g�p����q�[�vID
  GOBJ_ELBOARD* elboard;  // �d���f���Ǘ��I�u�W�F�N�g
} GATEWORK;


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static void GimmickSave( FIELDMAP_WORK* fieldmap );
static void GimmickLoad( GATEWORK* work, FIELDMAP_WORK* fieldmap );
static GATEWORK* CreateGateWork( FIELDMAP_WORK* fieldmap );
static BOOL LoadGateData( ELBOARD_ZONE_DATA* buf, FIELDMAP_WORK* fieldmap );
static void SetElboardPos( GFL_G3D_OBJSTATUS* status, ELBOARD_ZONE_DATA* data );
static void AddNews_DATE( GOBJ_ELBOARD* elboard, const ELBOARD_ZONE_DATA* data );
static void AddNews_WEATHER( GOBJ_ELBOARD* elboard, const ELBOARD_ZONE_DATA* data );
static void AddNews_INFO( GOBJ_ELBOARD* elboard, const ELBOARD_ZONE_DATA* data );
static void AddNews_CM( GOBJ_ELBOARD* elboard, const ELBOARD_ZONE_DATA* data );


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
  u32* gmk_save;  // �M�~�b�N�̎��Z�[�u�f�[�^
  GATEWORK* work;  // GATE�M�~�b�N�Ǘ����[�N
  HEAPID                heap_id = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  GAMESYS_WORK*            gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*               gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK*         sv = GAMEDATA_GetSaveControlWork( gdata );
  GIMMICKWORK*          gmkwork = SaveData_GetGimmickWork( sv );

  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &unit[UNIT_ELBOARD], EXPOBJ_UNIT_ELBOARD );

  // �M�~�b�N�Ǘ����[�N���쐬
  work = CreateGateWork( fieldmap );

  // �M�~�b�N��Ԃ̕��A
  GimmickLoad( work, fieldmap );

  OBATA_Printf( "GATE_GIMMICK_Setup\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 */
//------------------------------------------------------------------------------------------
void GATE_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK* sv = GAMEDATA_GetSaveControlWork( gdata );
  GIMMICKWORK*  gmkwork = SaveData_GetGimmickWork( sv );
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, GetGimmickID(fieldmap) );
  GATEWORK*        work = (GATEWORK*)gmk_save[0]; // gmk_save[0]�̓M�~�b�N�Ǘ����[�N�̃A�h���X

  // �M�~�b�N��Ԃ��Z�[�u
  GimmickSave( fieldmap );
  
  // �d���f���Ǘ����[�N��j��
  if( work->elboard )
  {
    GOBJ_ELBOARD_Delete( work->elboard );
  }

  // �M�~�b�N�Ǘ����[�N��j��
  GFL_HEAP_FreeMemory( work );

  OBATA_Printf( "GATE_GIMMICK_End\n" );
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
  SAVE_CONTROL_WORK* sv = GAMEDATA_GetSaveControlWork( gdata );
  GIMMICKWORK*  gmkwork = SaveData_GetGimmickWork( sv );
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, GetGimmickID(fieldmap) );
  GATEWORK*        work = (GATEWORK*)gmk_save[0]; // gmk_save[0]�̓M�~�b�N�Ǘ����[�N�̃A�h���X

  // �d���f�����C������
  GOBJ_ELBOARD_Main( work->elboard, FX32_ONE ); 

  OBATA_Printf( "GATE_GIMMICK_Move\n" );
}


//==========================================================================================
// �� ����J�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N��Ԃ�ۑ�����
 *
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
static void GimmickSave( FIELDMAP_WORK* fieldmap )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK* sv = GAMEDATA_GetSaveControlWork( gdata );
  GIMMICKWORK*  gmkwork = SaveData_GetGimmickWork( sv );
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, GetGimmickID(fieldmap) );
  GATEWORK*        work = (GATEWORK*)gmk_save[0]; // gmk_save[0]�̓M�~�b�N�Ǘ����[�N�̃A�h���X

  // �f���̓���t���[�������L��
  gmk_save[1] = GOBJ_ELBOARD_GetFrame( work->elboard ) >> FX32_SHIFT;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N��Ԃ𕜋A����
 *
 * @param work     �M�~�b�N�Ǘ����[�N
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
static void GimmickLoad( GATEWORK* work, FIELDMAP_WORK* fieldmap )
{
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK* sv = GAMEDATA_GetSaveControlWork( gdata );
  GIMMICKWORK*  gmkwork = SaveData_GetGimmickWork( sv );
  u32*         gmk_save = NULL;

  // �L���ȃZ�[�u�f�[�^������ꍇ
  if( GIMMICKWORK_GetAssignID( gmkwork ) == GetGimmickID(fieldmap) )
  {
    u32 frame;

    // �Z�[�u�f�[�^�擾
    gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, GetGimmickID(fieldmap) );
    frame    = gmk_save[1];
    GOBJ_ELBOARD_SetFrame( work->elboard, frame << FX32_SHIFT );
  }
  // �Z�[�u�f�[�^�������ꍇ
  else
  {
    // �V�K�Z�[�u�f�[�^���쐬
    gmk_save = (u32*)GIMMICKWORK_Assign( gmkwork, GetGimmickID(fieldmap) );
  }

  // �M�~�b�N�Ǘ����[�N�̃A�h���X���L��
  gmk_save[0] = (int)work; 
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
  int i;
  GATEWORK*                work = NULL;
  HEAPID                heap_id = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  ELBOARD_ZONE_DATA elboard_data;

  // �M�~�b�N�Ǘ����[�N���쐬
  work = (GATEWORK*)GFL_HEAP_AllocMemory( heap_id, sizeof(GATEWORK) );
  GFL_STD_MemClear( work, sizeof(GATEWORK) );

  // �q�[�vID���L��
  work->heapID = heap_id;

  // �d���f���f�[�^���擾
  if( !LoadGateData( &elboard_data, fieldmap ) )
  {
    // �f�[�^���擾�ł��Ȃ�������, �ȍ~�̏������𒆒f
    return work;
  }

  // �d���f����z�u
  { 
    GFL_G3D_OBJSTATUS* status = 
      FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD );
    SetElboardPos( status, &elboard_data );
  } 
  // �d���f���Ǘ����[�N���쐬
  {
    ELBOARD_PARAM param;
    param.heapID       = work->heapID;
    param.maxNewsNum   = NEWS_NUM;
    param.dispSize     = DISPLAY_SIZE;
    param.newsInterval = NEWS_INTERVAL;
    param.g3dObj       = FLD_EXP_OBJ_GetUnitObj( exobj_cnt, EXPOBJ_UNIT_ELBOARD, OBJ_ELBOARD );
    work->elboard = GOBJ_ELBOARD_Create( &param );
  }
  // �j���[�X��ǉ�
  AddNews_DATE( work->elboard, &elboard_data );
  AddNews_WEATHER( work->elboard, &elboard_data );
  AddNews_INFO( work->elboard, &elboard_data );
  AddNews_CM( work->elboard, &elboard_data );

  return work;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �]�[���ɉ������d���f���f�[�^���擾����
 *
 * @param buf      �ǂݍ��񂾃f�[�^�̊i�[��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �f�[�^���������ǂݍ��߂��� TRUE
 */
//------------------------------------------------------------------------------------------
static BOOL LoadGateData( ELBOARD_ZONE_DATA* buf, FIELDMAP_WORK* fieldmap )
{
  int i;
  u16 zone_id;

  // �]�[��ID���擾
  zone_id = FIELDMAP_GetZoneID( fieldmap );

  // �d���f���f�[�^�e�[�u������, �Y������f�[�^������
  for( i=0; i<NELEMS(entry_table); i++ )
  {
    if( entry_table[i].zone_id == zone_id )
    {
      // �d���f���f�[�^��ǂݍ���
      ELBOARD_ZONE_DATA_Load( buf, ARCID_GATE_GIMMICK, entry_table[i].dat_id ); 
      return TRUE;
    }
  } 

  // ���݂̃]�[���ɑΉ�����d���f���f�[�^���o�^����Ă��Ȃ��ꍇ
  OBATA_Printf( "---------------------------------------------\n" );
  OBATA_Printf( "error: �d���f���f�[�^���o�^����Ă��܂���B\n" );
  OBATA_Printf( "---------------------------------------------\n" );
  return FALSE;
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
  rot *= 182;  // 65536/360 = 182.044...
  GFL_CALC3D_MTX_CreateRot( 0, rot, 0, &status->rotate );
}

//------------------------------------------------------------------------------------------
/**
 * @breif �f���̃j���[�X��ǉ�����(���t)
 *
 * @param elboard �ǉ�����f����
 * @param data    �d���f���f�[�^
 */
//------------------------------------------------------------------------------------------
static void AddNews_DATE( GOBJ_ELBOARD* elboard, const ELBOARD_ZONE_DATA* data )
{
  NEWS_PARAM news;

  // �j���[�X�p�����[�^���쐬
  news.animeIndex = news_anm_index[NEWS_DATE];
  news.texName    = news_tex_name[NEWS_DATE];
  news.pltName    = news_plt_name[NEWS_DATE];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = data->arcDatID;
  news.msgStrID   = data->msgID_date;

  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( elboard, &news );
}

//------------------------------------------------------------------------------------------
/**
 * @breif �f���̃j���[�X��ǉ�����(�V�C)
 *
 * @param elboard �ǉ�����f����
 * @param data    �d���f���f�[�^
 */
//------------------------------------------------------------------------------------------
static void AddNews_WEATHER( GOBJ_ELBOARD* elboard, const ELBOARD_ZONE_DATA* data )
{
  NEWS_PARAM news;

  // �j���[�X�p�����[�^���쐬
  news.animeIndex = news_anm_index[NEWS_WEATHER];
  news.texName    = news_tex_name[NEWS_WEATHER];
  news.pltName    = news_plt_name[NEWS_WEATHER];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = data->arcDatID;
  news.msgStrID   = data->msgID_weather;

  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( elboard, &news );
}

//------------------------------------------------------------------------------------------
/**
 * @breif �f���̃j���[�X��ǉ�����(�n����)
 *
 * @param elboard �ǉ�����f����
 * @param data    �d���f���f�[�^
 */
//------------------------------------------------------------------------------------------
static void AddNews_INFO( GOBJ_ELBOARD* elboard, const ELBOARD_ZONE_DATA* data )
{
  int i;
  NEWS_PARAM news[LOCAL_INFO_NUM];
  RTCDate date;

  // �����f�[�^���擾
  RTC_GetDate( &date ); 

  // �j���ɉ���������I��
  switch( date.week )
  {
  case RTC_WEEK_SUNDAY:    
    news[0].msgStrID = data->msgID_infoG;  
    news[1].msgStrID = data->msgID_infoH;  
    news[2].msgStrID = data->msgID_infoI;  
    break;
  case RTC_WEEK_MONDAY:    
    news[0].msgStrID = data->msgID_infoD;  
    news[1].msgStrID = data->msgID_infoE;  
    news[2].msgStrID = data->msgID_infoF;  
    break;
  case RTC_WEEK_TUESDAY:   
    news[0].msgStrID = data->msgID_infoA;  
    news[1].msgStrID = data->msgID_infoB;  
    news[2].msgStrID = data->msgID_infoC;  
    break;
  case RTC_WEEK_WEDNESDAY: 
    news[0].msgStrID = data->msgID_infoE;  
    news[1].msgStrID = data->msgID_infoF;  
    news[2].msgStrID = data->msgID_infoG;  
    break;
  case RTC_WEEK_THURSDAY:  
    news[0].msgStrID = data->msgID_infoB;  
    news[1].msgStrID = data->msgID_infoC;  
    news[2].msgStrID = data->msgID_infoD;  
    break;
  case RTC_WEEK_FRIDAY:    
    news[0].msgStrID = data->msgID_infoF;  
    news[1].msgStrID = data->msgID_infoG;  
    news[2].msgStrID = data->msgID_infoH;  
    break;
  case RTC_WEEK_SATURDAY:  
    news[0].msgStrID = data->msgID_infoC;  
    news[1].msgStrID = data->msgID_infoD;  
    news[2].msgStrID = data->msgID_infoE;  
    break;
  }

  news[0].animeIndex = news_anm_index[NEWS_INFO_A];
  news[0].texName    = news_tex_name[NEWS_INFO_A];
  news[0].pltName    = news_plt_name[NEWS_INFO_A];
  news[0].msgArcID   = ARCID_MESSAGE;
  news[0].msgDatID   = data->arcDatID;

  news[1].animeIndex = news_anm_index[NEWS_INFO_B];
  news[1].texName    = news_tex_name[NEWS_INFO_B];
  news[1].pltName    = news_plt_name[NEWS_INFO_B];
  news[1].msgArcID   = ARCID_MESSAGE;
  news[1].msgDatID   = data->arcDatID;

  news[2].animeIndex = news_anm_index[NEWS_INFO_C];
  news[2].texName    = news_tex_name[NEWS_INFO_C];
  news[2].pltName    = news_plt_name[NEWS_INFO_C];
  news[2].msgArcID   = ARCID_MESSAGE;
  news[2].msgDatID   = data->arcDatID;

  // �j���[�X��ǉ�
  for( i=0; i<LOCAL_INFO_NUM; i++ )
  {
    GOBJ_ELBOARD_AddNews( elboard, &news[i] );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @breif �f���̃j���[�X��ǉ�����(�ꌾCM)
 *
 * @param elboard �ǉ�����f����
 */
//------------------------------------------------------------------------------------------
static void AddNews_CM( GOBJ_ELBOARD* elboard, const ELBOARD_ZONE_DATA* data )
{
  NEWS_PARAM news;
  RTCDate date;

  // �����f�[�^���擾
  RTC_GetDate( &date );

  // �j���ɉ�����CM��I��
  switch( date.week )
  {
  case RTC_WEEK_SUNDAY:    news.msgStrID = data->msgID_cmSun;  break;
  case RTC_WEEK_MONDAY:    news.msgStrID = data->msgID_cmMon;  break;
  case RTC_WEEK_TUESDAY:   news.msgStrID = data->msgID_cmTue;  break;
  case RTC_WEEK_WEDNESDAY: news.msgStrID = data->msgID_cmWed;  break;
  case RTC_WEEK_THURSDAY:  news.msgStrID = data->msgID_cmThu;  break;
  case RTC_WEEK_FRIDAY:    news.msgStrID = data->msgID_cmFri;  break;
  case RTC_WEEK_SATURDAY:  news.msgStrID = data->msgID_cmSat;  break;
  }
  news.animeIndex = news_anm_index[NEWS_CM];
  news.texName    = news_tex_name[NEWS_CM];
  news.pltName    = news_plt_name[NEWS_CM];
  news.msgArcID   = ARCID_MESSAGE;
  news.msgDatID   = data->arcDatID;
  
  // �j���[�X��ǉ�
  GOBJ_ELBOARD_AddNews( elboard, &news );
}
