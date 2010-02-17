//======================================================================
/**
 * @file	map_replace.c
 * @brief	�����ɂ��}�b�v��������
 * @author	tamada GAMEFREAK inc.
 * @date	09.12.11
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "gamesystem/gamesystem.h"
#include "field/map_matrix.h"
#include "field/field_comm/intrude_work.h"

#include "arc/fieldmap/map_replace.naix"
#include "arc/fieldmap/map_replace.h"
#include "map_replace_def.h"

#include "map_replace.h"

#include "eventwork.h"  //EVENTWORK_

#include "../../../resource/fldmapdata/flagwork/work_define.h"  //WK_SYS_MAPREPLACE�`

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
enum {
  FILE_ID = NARC_map_replace_map_replace_bin
};

enum {
  MAPREPLACE_EVENT_TYPE_NUM = 10,
};

//--------------------------------------------------------------
//--------------------------------------------------------------
enum {
  REPID_BASE_KEY = 0,
  REPID_VAR_A,
  REPID_VAR_B,
  REPID_VAR_C,
  REPID_VAR_D,

  REPID_MAX,
};

//--------------------------------------------------------------
/**
 * @brief   �}�b�v���������f�[�^
 * @note  �R���o�[�^�ɂ�萶�������o�C�i���f�[�^�ƑΉ����Ă���B
 */
//--------------------------------------------------------------
typedef struct {
  u16 matrix_id;    ///<�ΏۂƂȂ�}�g���b�N�X�w��
  u8 layer_id;      ///<���������郌�C���[�i�n�`�u���b�N���A�}�g���b�N�X���H�j
  u8 type;          ///<�������������̃^�C�v
  u16 keys[ REPID_MAX ];    ///<���������f�[�^
  u16 dummy;
}REPLACE_DATA;

SDK_COMPILER_ASSERT( sizeof(REPLACE_DATA) == MAPREPLACE_DATASIZE );

//--------------------------------------------------------------
/**
 * @brief ���������I�t�Z�b�g���[�N
 */
//--------------------------------------------------------------
typedef struct {
  u8 season_pos;      ///<���������^�C�v�G�߂̏ꍇ�̃I�t�Z�b�g�l
  u8 version_pos;     ///<���������^�C�vVer.�̏ꍇ�̃I�t�Z�b�g�l
  u8 season_ver_pos;  ///<���������^�C�v�G�߁{Ver.�̏ꍇ�̃I�t�Z�b�g�l
  ///�t���O�ɂ�鏑�������̏ꍇ�̃I�t�Z�b�g�l
  u8 flag_pos[MAPREPLACE_EVENT_TYPE_NUM];
}REPLACE_OFFSET;

//--------------------------------------------------------------
/**
 * @brief �}�b�v�u����������p���[�N
 */
//--------------------------------------------------------------
struct _MAPREPLACE_CTRL {
  HEAPID heapID;
  ARCHANDLE * pHandle;      ///<�}�b�v�u�������w��f�[�^�̃n���h��
  u32 data_max;             ///<�}�b�v�u�������w��f�[�^��
  REPLACE_OFFSET offsets;   ///<�u�������I�t�Z�b�g�w���ێ����郏�[�N
  REPLACE_DATA rep_data;    ///<�}�b�v�u�������f�[�^�̓ǂݍ��ݗ̈�
};


//======================================================================
//======================================================================

static void MAPREPLACE_makeReplaceOffset ( REPLACE_OFFSET * offsets, GAMESYS_WORK * gamesystem );
static int getReplaceTablePos( MAPREPLACE_TYPE rep_type );

//======================================================================
//
//
//    �O�����J�֐�
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �}�b�v�u���������䃏�[�N�̉������
 * @param ctrl  ���䃏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
MAPREPLACE_CTRL * MAPREPLACE_Create( HEAPID heapID, GAMESYS_WORK * gamesys )
{
  u16 size;
  MAPREPLACE_CTRL * ctrl = GFL_HEAP_AllocClearMemory( heapID, sizeof(MAPREPLACE_CTRL) );
  ctrl->heapID = heapID;
  ctrl->pHandle = GFL_ARC_OpenDataHandle( ARCID_FLDMAP_REPLACE, heapID );
  size = GFL_ARC_GetDataSizeByHandle( ctrl->pHandle, FILE_ID );
  ctrl->data_max = size / MAPREPLACE_DATASIZE;
  //REPLACE_OFFSET offsets;
  //REPLACE_DATA rep_data;


  GF_ASSERT( ctrl->data_max = MAP_REPLACE_DATA_COUNT );
  GF_ASSERT( size % MAPREPLACE_DATASIZE == 0 );

  //�u�������I�t�Z�b�g�l�����O�ɎZ�o���Ă���
  MAPREPLACE_makeReplaceOffset( &ctrl->offsets, gamesys );
  return ctrl;
}
//--------------------------------------------------------------
/**
 * @brief
 * @param ctrl  ���䃏�[�N�ւ̃|�C���^
 * @return u32  �ێ����Ă���ʃ}�b�v�u�������f�[�^�̌�
 */
//--------------------------------------------------------------
u32 MAPREPLACE_GetDataMax( const MAPREPLACE_CTRL * ctrl )
{
  return ctrl->data_max;
}
//--------------------------------------------------------------
/**
 * @brief �}�b�v�u�������ʃf�[�^�̓ǂݍ���
 * @param ctrl  ���䃏�[�N�ւ̃|�C���^
 * @param idx   �ǂݍ��ރf�[�^�̃C���f�b�N�X
 * @return  u32 �ǂݍ��񂾃f�[�^���Ή�����}�g���b�N�XID
 */
//--------------------------------------------------------------
u32 MAPREPLACE_Load( MAPREPLACE_CTRL * ctrl, u32 idx )
{
  GF_ASSERT( idx < ctrl->data_max );
  GFL_ARC_LoadDataOfsByHandle( ctrl->pHandle, FILE_ID,
      MAPREPLACE_DATASIZE * idx, MAPREPLACE_DATASIZE, &ctrl->rep_data );
  GF_ASSERT( ctrl->rep_data.layer_id < MAPREPLACE_LAYER_MAX );
  GF_ASSERT( ctrl->rep_data.type <  MAPREPLACE_TYPE_MAX );

  return ctrl->rep_data.matrix_id;
}
//--------------------------------------------------------------
/**
 * @brief �}�b�v�u���������䃏�[�N�̉������
 * @param ctrl  ���䃏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
void MAPREPLACE_Delete( MAPREPLACE_CTRL * ctrl )
{
  GFL_ARC_CloseDataHandle( ctrl->pHandle );
  GFL_HEAP_FreeMemory( ctrl );
}


//--------------------------------------------------------------
/**
 * @brief �}�b�v�u����������
 * @param	pMat MAP_MATRIX
 * @param offsets   ���������I�t�Z�b�g�W��
 * @param rep_data  ���������f�[�^
 */
//--------------------------------------------------------------
REPLACE_REQUEST MAPREPLACE_GetReplaceValue( const MAPREPLACE_CTRL * ctrl, u32 *before, u32 *after )
{
  const REPLACE_DATA * rep_data = &ctrl->rep_data;
  const REPLACE_OFFSET * offsets = &ctrl->offsets;
  u32 pos;
  u32 before_val, after_val;
  before_val = rep_data->keys[ REPID_BASE_KEY ];
  after_val = before_val;
  switch ( rep_data->type )
  {
  case MAPREPLACE_TYPE_SEASON:
    after_val = rep_data->keys[ offsets->season_pos ];
    break;

  case MAPREPLACE_TYPE_VERSION:
    after_val = rep_data->keys[ offsets->version_pos ];
    break;

  case MAPREPLACE_TYPE_SEASON_VERSION:
    after_val = rep_data->keys[ offsets->season_ver_pos ];
    break;

  default:
    pos = getReplaceTablePos( rep_data->type );
    if ( pos >= 0 ) {
      after_val = rep_data->keys[ offsets->flag_pos[ pos ] ];
      break;
    }
    GF_ASSERT(0);
    break;
  }
  *before = before_val;
  *after = after_val;

  if ( before_val == after_val ) return REPLACE_REQ_NON;

  if (rep_data->layer_id == MAPREPLACE_LAYER_MATRIX) {
    return REPLACE_REQ_MATRIX;
  } else if (rep_data->layer_id == MAPREPLACE_LAYER_MAPBLOCK) {
    return REPLACE_REQ_BLOCK;
  }
  GF_ASSERT(0);
  return REPLACE_REQ_NON;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef enum {
  REPLACE_FLAG_VALUE01 = 0x1220,  //kagaya
  REPLACE_FLAG_VALUE02 = 0x0224,  //nozomu
  REPLACE_FLAG_VALUE03 = 0x0206,  //tomoya
  REPLACE_FLAG_VALUE04 = 0x1209,  //iwasawa
  REPLACE_FLAG_VALUE05 = 0x0316,  //obata
  REPLACE_FLAG_VALUE06 = 0x1227,  //masafumi
  REPLACE_FLAG_VALUE07 = 0x0830,  //nakatsui
  REPLACE_FLAG_VALUE08 = 0x0408,  //matsumiya
  REPLACE_FLAG_VALUE09 = 0x0120,  //fuchino
  REPLACE_FLAG_VALUE10 = 0x1028,  //watanabe
}REPLACE_FLAGS;

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  u8 rep_data_type;
  u8 rep_data_id;
  u16 wk_id;
  u16 wk_value;
}REPLACE_EVENT_DATA;

//--------------------------------------------------------------
//--------------------------------------------------------------
static const REPLACE_EVENT_DATA replaceEventTable[] = {
  { MAPREPLACE_TYPE_EVENT01, MAPREPLACE_ID_01, WK_SYS_MAPREPLACE01, REPLACE_FLAG_VALUE01 },
  { MAPREPLACE_TYPE_EVENT02, MAPREPLACE_ID_02, WK_SYS_MAPREPLACE02, REPLACE_FLAG_VALUE02 },
  { MAPREPLACE_TYPE_EVENT03, MAPREPLACE_ID_03, WK_SYS_MAPREPLACE03, REPLACE_FLAG_VALUE03 },
  { MAPREPLACE_TYPE_EVENT04, MAPREPLACE_ID_04, WK_SYS_MAPREPLACE04, REPLACE_FLAG_VALUE04 },
  { MAPREPLACE_TYPE_EVENT05, MAPREPLACE_ID_05, WK_SYS_MAPREPLACE05, REPLACE_FLAG_VALUE05 },
  { MAPREPLACE_TYPE_EVENT06, MAPREPLACE_ID_06, WK_SYS_MAPREPLACE06, REPLACE_FLAG_VALUE06 },
  { MAPREPLACE_TYPE_EVENT07, MAPREPLACE_ID_07, WK_SYS_MAPREPLACE07, REPLACE_FLAG_VALUE07 },
  { MAPREPLACE_TYPE_EVENT08, MAPREPLACE_ID_08, WK_SYS_MAPREPLACE08, REPLACE_FLAG_VALUE08 },
  { MAPREPLACE_TYPE_EVENT09, MAPREPLACE_ID_09, WK_SYS_MAPREPLACE09, REPLACE_FLAG_VALUE09 },
  { MAPREPLACE_TYPE_EVENT10, MAPREPLACE_ID_10, WK_SYS_MAPREPLACE10, REPLACE_FLAG_VALUE10 },
};

SDK_COMPILER_ASSERT( NELEMS(replaceEventTable) == MAPREPLACE_EVENT_TYPE_NUM );

//--------------------------------------------------------------
/**
 * @brief
 * @param rep_type
 */
//--------------------------------------------------------------
static int getReplaceTablePos( MAPREPLACE_TYPE rep_type )
{
  int i;
  for (i = 0; i < NELEMS(replaceEventTable); i++ )
  {
    if ( replaceEventTable[i].rep_data_type == rep_type )
    {
      return i;
    }
  }
  return -1;
}

//--------------------------------------------------------------
/**
 * @brief �������������I�t�Z�b�g�̐���
 * @param offsets
 * @param gamesystem
 *
 * @todo  �C�x���g�t���O�̑Ή����s���K�v������
 */
//--------------------------------------------------------------
static void MAPREPLACE_makeReplaceOffset ( REPLACE_OFFSET * offsets, GAMESYS_WORK * gamesystem )
{
  int i;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gamesystem );
  EVENTWORK * ev = GAMEDATA_GetEventWork( gamedata );
  int season = GAMEDATA_GetSeasonID( gamedata );
  GAME_COMM_SYS_PTR commsys = GAMESYSTEM_GetGameCommSysPtr( gamesystem );
  
  offsets->season_pos = season;
  offsets->version_pos = Intrude_GetRomVersion( commsys ) == VERSION_BLACK? 0 : 1;
  if (offsets->version_pos == 0 )
  {
    offsets->season_ver_pos = 0;
  } else {
    offsets->season_ver_pos = 1 + season;
  }
  for ( i = 0; i < MAPREPLACE_EVENT_TYPE_NUM; i++)
  {
    const REPLACE_EVENT_DATA * evData = &replaceEventTable[ i ];
    u16 * work = EVENTWORK_GetEventWorkAdrs( ev, evData->wk_id );
    if ( *work == evData->wk_value )
    {
      offsets->flag_pos[i] = 1;
    } else {
      offsets->flag_pos[i] = 0; //�Ƃ肠����
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief �}�b�v�u�������F�t���O�̔��f����
 * @param gamedata  �Q�[���f�[�^�ւ̃|�C���^
 * @param id        �t���O�w��ID
 * @param flag      BOOL
 */
//--------------------------------------------------------------
void MAPREPLACE_ChangeFlag( GAMEDATA * gamedata, u32 id, BOOL flag )
{
  int i;
  for ( i = 0; i < NELEMS(replaceEventTable); i++ )
  {
    const REPLACE_EVENT_DATA * evData = &replaceEventTable[ i ];
    if ( evData->rep_data_id == id )
    {
      EVENTWORK * ev = GAMEDATA_GetEventWork( gamedata );
      u16 * work = EVENTWORK_GetEventWorkAdrs( ev, evData->wk_id );
      if (flag) {
        *work = evData->wk_value;
      } else {
        *work = 0;
      }
      OS_Printf("REPLACE EVENT(%d) SET: %04x\n", id, evData->wk_value );
      return;
    }
  }
  GF_ASSERT_MSG( 0, "�����ȃ}�b�v�u������ID�i%d)�ł�", id );
}





