//============================================================================================
/**
 * @file	eventdata_system.c
 * @brief	�C�x���g�N���p�f�[�^�̃��[�h�E�ێ��V�X�e��
 * @author	tamada GAME FREAK inc.
 * @date	2008.11.20
 *
 */
//============================================================================================

#include <gflib.h>
#include "gamesystem/gamesystem.h"

#include "system/main.h"
#include "field/field_const.h"

#include "eventdata_local.h"
#include "field/eventdata_system.h"
#include "field/eventdata_sxy.h"

#include "field/location.h"
#include "field/rail_location.h"
#include "field_rail.h"

#include "fldmmdl.h"
#include "script.h"
#include "script_hideitem.h"

#include "fieldmap/zone_id.h"

#include "field/zonedata.h"
#include "field/encount_data.h"

//============================================================================================
//============================================================================================
#ifdef PM_DEBUG

//#define DEBUG_EVENTDATA_PRINT // �ǂݍ��񂾃C�x���g�f�[�^���o��

#endif  // PM_DEBUG

//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
	EVDATA_SIZE = 0x800,
	SPSCR_DATA_SIZE = 0x100,
};

//------------------------------------------------------------------
//------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�N���f�[�^�V�X�e���̒�`
 */
//------------------------------------------------------------------
struct _EVDATA_SYS {
	HEAPID heapID;

	u16 now_zone_id;
	u16 bg_count;
	u16 npc_count;
	u16 connect_count;
	u16 pos_count;
	const BG_TALK_DATA * bg_data;
	const MMDL_HEADER *npc_data;
	const CONNECT_DATA * connect_data;
	const POS_EVENT_DATA * pos_data;

	ENCOUNT_DATA encount_work;
	u32 load_buffer[EVDATA_SIZE / sizeof(u32)];
	u32 spscr_buffer[SPSCR_DATA_SIZE / sizeof(u32)];
};


//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {

  u8 bg_count;
  u8 npc_count;
  u8 connect_count;
  u8 pos_count;

  u32 buf[];  ///<�f�[�^�@TalkBG NPC CONNECT POS �̏��Ɋi�[

}EVENTDATA_TABLE;

typedef struct {
  u16 zone_id;
  const EVENTDATA_TABLE * table;
}EVENTDATA_TOTAL_TABLES;
//------------------------------------------------------------------
//------------------------------------------------------------------

#include "arc/fieldmap/zone_id.h"
#include "eventwork_def.h"

#define WORKID_DUMMY    LOCAL_WORK_START
//���������C�x���g�f�[�^��S���C���N���[�h
//binary��ǂݍ��ތ`�ɕύX  prog/arc/fieldmap/eventdata.narc
//#include "../../../resource/fldmapdata/eventdata/eventdata_table.cdat"


#if 0
static const CONNECT_DATA ConnectData_H01[] = {
	{//DOOR_ID_H01_EXIT01 = 0
		{559, 0, 2139},
		//{559, 0, 2143},
		ZONE_ID_H01P01, DOOR_ID_H01P01_EXIT04,
		2,//direction
		0 //type
	},
	{//DOOR_ID_H01_EXIT02 = 1
		{703, 0, 128},
		ZONE_ID_C03, 0,
		2,//direction
		0 //type
	},
};
static const int ConnectCount_H01 = NELEMS(ConnectData_H01);
#endif

#if 0
static const CONNECT_DATA ConnectData_C03[] = {
	{//DOOR_ID_C03_EXIT01 = 0
		{1101, 0, 493},
		ZONE_ID_H01, 1 /* DOOR_ID_H01_EXIT01*/,
		2,//direction
		0 //type
	},
#if 0
	{//DOOR_ID_C03_EXIT02 = 1
		{703, 0, 128},
		ZONE_ID_T01, DOOR_ID_T01_EXIT02
		2,//direction
		0 //type
	},
#endif
};
static const int ConnectCount_C03 = NELEMS(ConnectData_C03);
#endif


//============================================================================================
//============================================================================================
#ifdef DEBUG_EVENTDATA_PRINT
static void debugPrint_Connect( const CONNECT_DATA* cp_data );
static void debugPrint_BGTalk( const BG_TALK_DATA* cp_data );
static void debugPrint_MMLD( const MMDL_HEADER* cp_data );
static void debugPrint_PosData( const POS_EVENT_DATA* cp_data );
#endif  //DEBUG_EVENTDATA_PRINT

//============================================================================================
//============================================================================================
static void loadEventDataTable(EVENTDATA_SYSTEM * evdata, u16 zone_id);
static void loadSpecialScriptData( EVENTDATA_SYSTEM * evdata, u16 zone_id );
static void loadEncountDataTable(EVENTDATA_SYSTEM* evdata, u16 zone_id, u8 season_id );


//============================================================================================
// CONNECT_DATA �|�W�V����
//============================================================================================
static void ConnectData_GPOS_GetPos( const CONNECT_DATA* cp_data, VecFx32* p_pos );
static BOOL ConnectData_GPOS_IsHit( const CONNECT_DATA* cp_data, const VecFx32* cp_pos );
static void ConnectData_RPOS_GetLocation( const CONNECT_DATA* cp_data, RAIL_LOCATION* p_location );
static BOOL ConnectData_RPOS_IsHit( const CONNECT_DATA* cp_data, const RAIL_LOCATION* cp_location );


//============================================================================================
// BG_TALK_DATA �|�W�V����
//============================================================================================
static void BGTalkData_GPOS_GetPos( const BG_TALK_DATA* cp_data, VecFx32* p_pos );
static BOOL BGTalkData_GPOS_IsHit( const BG_TALK_DATA* cp_data, const VecFx32* cp_pos );
static void BGTalkData_RPOS_GetLocation( const BG_TALK_DATA* cp_data, RAIL_LOCATION* p_location );
static BOOL BGTalkData_RPOS_IsHit( const BG_TALK_DATA* cp_data, const RAIL_LOCATION* cp_location );

//============================================================================================
// POS_EVENT_DATA �|�W�V����
//============================================================================================
static void PosEventData_GPOS_GetPos( const POS_EVENT_DATA* cp_data, VecFx32* p_pos );
static BOOL PosEventData_GPOS_IsHit( const POS_EVENT_DATA* cp_data, const VecFx32* cp_pos );
static void PosEventData_RPOS_GetLocation( const POS_EVENT_DATA* cp_data, RAIL_LOCATION* p_location );
static BOOL PosEventData_RPOS_IsHit( const POS_EVENT_DATA* cp_data, const RAIL_LOCATION* cp_location );


//============================================================================================
//============================================================================================

//============================================================================================
//
//	�C�x���g�N���f�[�^�V�X�e���֘A
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
EVENTDATA_SYSTEM * EVENTDATA_SYS_Create(HEAPID heapID)
{
	EVENTDATA_SYSTEM * evdata = GFL_HEAP_AllocClearMemory(heapID, sizeof(EVENTDATA_SYSTEM));
	return evdata;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void EVENTDATA_SYS_Delete(EVENTDATA_SYSTEM * evdata)
{
	GFL_HEAP_FreeMemory(evdata);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void EVENTDATA_SYS_Clear(EVENTDATA_SYSTEM * evdata)
{
	evdata->bg_count = 0;
	evdata->npc_count = 0;
	evdata->connect_count = 0;
	evdata->pos_count = 0;
	evdata->bg_data = NULL;
	evdata->npc_data = NULL;
	evdata->connect_data = NULL;
	evdata->pos_data = NULL;
	GFL_STD_MemClear(evdata->load_buffer, EVDATA_SIZE);
	GFL_STD_MemClear(evdata->spscr_buffer, SPSCR_DATA_SIZE);
}



//------------------------------------------------------------------
//------------------------------------------------------------------
void EVENTDATA_SYS_Load( EVENTDATA_SYSTEM * evdata, u16 zone_id, u8 season_id )
{
	EVENTDATA_SYS_Clear(evdata);
	evdata->now_zone_id = zone_id;

  loadSpecialScriptData( evdata, zone_id );
  loadEventDataTable(evdata, zone_id);
  loadEncountDataTable( evdata, zone_id, season_id );

	/* �e�X�g�I�ɐڑ��f�[�^��ݒ� */
	switch (zone_id) {
#if 0
 case ZONE_ID_H01:
    evdata->connect_count = ConnectCount_H01;
    evdata->connect_data = ConnectData_H01;
    break;
 case ZONE_ID_C03:
    evdata->connect_count = ConnectCount_C03;
    evdata->connect_data = ConnectData_C03;
    break;
#endif
	}
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void * EVENTDATA_GetSpecialScriptData( EVENTDATA_SYSTEM * evdata )
{
  return &evdata->spscr_buffer;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void loadSpecialScriptData( EVENTDATA_SYSTEM * evdata, u16 zone_id )
{
  u16 sp_scr_id = ZONEDATA_GetSpScriptArcID( zone_id );
  GFL_ARC_LoadData(evdata->spscr_buffer, ARCID_SCRSEQ,  sp_scr_id );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void loadEventDataTable(EVENTDATA_SYSTEM * evdata, u16 zone_id)
{
  int i;
  const EVENTDATA_TABLE* table;
  s32 ofs;
  const u8* buf;
  u32 arcID;

  arcID = ZONEDATA_GetEventDataArcID(zone_id);
  GFL_ARC_LoadData(evdata->load_buffer, ARCID_FLD_EVENTDATA,  
      ZONEDATA_GetEventDataArcID(zone_id) );

  // �T�C�Y�I�[�o�[�`�F�b�N
  GF_ASSERT( EVDATA_SIZE > GFL_ARC_GetDataSize( ARCID_FLD_EVENTDATA, ZONEDATA_GetEventDataArcID(zone_id) ) );
  
  table = (const EVENTDATA_TABLE*)evdata->load_buffer; 
  buf = (const u8*)table->buf;
  ofs = 0;

  // �f�[�^�i�[
  evdata->bg_count = table->bg_count;
  evdata->bg_data = (const BG_TALK_DATA*)&buf[ofs];
  ofs += sizeof(BG_TALK_DATA) * table->bg_count;
  evdata->npc_count = table->npc_count;
  evdata->npc_data = (const MMDL_HEADER*)&buf[ofs];
  ofs += sizeof(MMDL_HEADER) * table->npc_count;
  evdata->connect_count = table->connect_count;
  evdata->connect_data = (const CONNECT_DATA*)&buf[ofs];
  ofs += sizeof(CONNECT_DATA) * table->connect_count;
  evdata->pos_count = table->pos_count;
  evdata->pos_data = (const POS_EVENT_DATA*)&buf[ofs];

  for (i = 0; i < evdata->connect_count; i++)
  {
    const CONNECT_DATA * cnct = &evdata->connect_data[i];
    if( cnct->pos_type == EVENTDATA_POSTYPE_GRID )
    {
      const CONNECT_DATA_GPOS* cp_pos = (const CONNECT_DATA_GPOS*)cnct->pos_buf;
      TAMADA_Printf("CNCT:ID%02d (%08x, %08x, %08x)", i, cp_pos->x, cp_pos->y, cp_pos->z);
      TAMADA_Printf(" (%3d, %3d, %3d)\n",
          cp_pos->x / FIELD_CONST_GRID_SIZE,
          cp_pos->y / FIELD_CONST_GRID_SIZE,
          cp_pos->z / FIELD_CONST_GRID_SIZE);
    }
    else
    {
      const CONNECT_DATA_RPOS* cp_pos = (const CONNECT_DATA_RPOS*)cnct->pos_buf;
      TAMADA_Printf("CNCT:ID%02d (rail_index:%d line:%d width:%d)\n", i, cp_pos->rail_index, cp_pos->front_grid, cp_pos->side_grid);
    }
  }


#ifdef DEBUG_EVENTDATA_PRINT
  {
    OS_TPrintf( "DEBUG PRINT Event Load ZoneID %d\n", zone_id );
    
    // �ڑ����
    for( i=0; i<evdata->connect_count; i++ )
    {
      debugPrint_Connect( &evdata->connect_data[i] );
    }

    for( i=0; i<evdata->bg_count; i++ )
    {
      debugPrint_BGTalk( &evdata->bg_data[i] );
    }

    for( i=0; i<evdata->npc_count; i++ )
    {
      debugPrint_MMLD( &evdata->npc_data[i] );
    }

    for( i=0; i<evdata->pos_count; i++ )
    {
      debugPrint_PosData( &evdata->pos_data[i] );
    }
  }
#endif // DEBUG_EVENTDATA_PRINT
}

//------------------------------------------------------------------
/*
 *  @brief  �]�[���G���J�E���g�f�[�^���[�h
 *
 *  @retval true  �G���J�E���g�f�[�^�擾�ɐ���
 *  @retval false �w��]�[���ɂ̓G���J�E���g�f�[�^���Ȃ�
 */
//------------------------------------------------------------------
static void loadEncountDataTable(EVENTDATA_SYSTEM* evdata, u16 zone_id, u8 season_id )
{
  u32 size;
  u16 id,season;
  ENCOUNT_TABLE* tbl;
  MI_CpuClear8(&evdata->encount_work,sizeof(ENCOUNT_DATA));

  id = ZONEDATA_GetEncountDataID(zone_id);
  if(id == ENC_DATA_INVALID_ID){
    return;
  }
  //�f�[�^�̌����擾
  size = GFL_ARC_GetDataSize(ARCID_ENCOUNT,id) / sizeof(ENCOUNT_DATA);
  if(size != 1 && size != 4){
    GF_ASSERT(0);
    return;
  }
  //�e���|�����m��
  tbl = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(ENCOUNT_TABLE));
  //���[�h
  GFL_ARC_LoadData(tbl,ARCID_ENCOUNT,id);
  //�G�߂��擾
  if(size == 1){
    season = 0;
  }else{
    season = season_id; //GAMEDATA_GetSeasonID();
  }
  MI_CpuCopy8(&tbl->enc_tbl[season],&evdata->encount_work,sizeof(ENCOUNT_DATA));
  evdata->encount_work.enable_f = TRUE;
  GFL_HEAP_FreeMemory(tbl);
}

//------------------------------------------------------------------
/*
 *  @brief  ���]�[���G���J�E���g�f�[�^�擾
 *  @return ���݂̃]�[���ɓǂݍ��܂�Ă���G���J�E���g�f�[�^�e�[�u���ւ̃|�C���^
 */
//------------------------------------------------------------------
void* EVENTDATA_GetEncountDataTable(EVENTDATA_SYSTEM* evdata)
{
  return &evdata->encount_work;
}


//============================================================================================
//============================================================================================

//============================================================================================
//
//	�o�����ڑ��f�[�^�֘A
//
//============================================================================================
#define	ZONE_ID_SPECIAL		(0x0fff)
#define	EXIT_ID_SPECIAL		(0x0100)
enum {
  CONNECT_POS_OFS_X = 0,
  CONNECT_POS_OFS_Y = 0,
  CONNECT_POS_OFS_Z = 0,

  CONNECT_DATA_RANGE = 2,
};
//------------------------------------------------------------------
//------------------------------------------------------------------
int EVENTDATA_SearchConnectIDByPos(const EVENTDATA_SYSTEM * evdata, const VecFx32 * pos)
{
	int i;
	const CONNECT_DATA * cnct = evdata->connect_data;
	for (i = 0; i < evdata->connect_count; i++, cnct++ ) {
    if( ConnectData_GPOS_IsHit( cnct, pos ) )
    {
  		TAMADA_Printf("CNCT:zone,exit,type=%d,%d,%d\n",cnct->link_zone_id,cnct->link_exit_id,cnct->exit_type);
      
	  	//TAMADA_Printf("CNCT:x %d(%08x), y %d(%08x), z %d(%08x)\n",x,pos->x, y,pos->y, z,pos->z);
		  return i;
    }
	}
	return EXIT_ID_NONE;
}

int EVENTDATA_SearchConnectIDByRailLocation(const EVENTDATA_SYSTEM * evdata, const RAIL_LOCATION* rail_location)
{
	int i;
	const CONNECT_DATA * cnct = evdata->connect_data;
	for (i = 0; i < evdata->connect_count; i++, cnct++ ) {
    if( ConnectData_RPOS_IsHit( cnct, rail_location ) )
    {
  		TAMADA_Printf("CNCT:zone,exit,type=%d,%d,%d\n",cnct->link_zone_id,cnct->link_exit_id,cnct->exit_type);
      
	  	//TAMADA_Printf("CNCT:x %d(%08x), y %d(%08x), z %d(%08x)\n",x,pos->x, y,pos->y, z,pos->z);
		  return i;
    }
	}
	return EXIT_ID_NONE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const CONNECT_DATA * EVENTDATA_GetConnectByID(const EVENTDATA_SYSTEM * evdata, u16 exit_id)
{
	const CONNECT_DATA * cnct = evdata->connect_data;
	if (cnct == NULL) {
		return NULL;
	}
	if (exit_id >= evdata->connect_count) {
		GF_ASSERT(0);
		return NULL;
	}
	return &cnct[exit_id];
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL CONNECTDATA_IsSpecialExit(const CONNECT_DATA * connect)
{
	return (connect->link_exit_id == EXIT_ID_SPECIAL);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL EVENTDATA_SetLocationByExitID(const EVENTDATA_SYSTEM * evdata, LOCATION * loc, u16 exit_id)
{
  /** TODO:�@�����ڊ܂߂ăI�t�Z�b�g���Čv�Z���邱�� */
  enum{ GRIDSIZE = 8, MV = GRIDSIZE * FX32_ONE};
//  static const VecFx32 ofs = { CONNECT_POS_OFS_X * FX32_ONE, CONNECT_POS_OFS_Y * FX32_ONE, 0 * FX32_ONE }; GPOS_GetPos���ōs���Ă��܂��B

	const CONNECT_DATA * connect = EVENTDATA_GetConnectByID(evdata, exit_id);
	if (connect == NULL) {
		return FALSE;
	}

	//CONNECTDATA_SetLocation(connect, loc);
	LOCATION_Init(loc);
	if (connect->link_exit_id == EXIT_ID_SPECIAL) {
		loc->type = LOCATION_TYPE_SPID;
	} else {
		loc->type = LOCATION_TYPE_EXITID;
	}
	//loc->zone_id = connect->link_zone_id;
	//loc->exit_id = connect->link_exit_id;
	loc->dir_id = connect->exit_dir;
	loc->zone_id = evdata->now_zone_id;
	loc->exit_id = exit_id;

	//loc->pos = connect->pos;
	ConnectData_GPOS_GetPos( connect, &loc->pos );

	return TRUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void CONNECTDATA_SetNextLocation(const CONNECT_DATA * connect, LOCATION * loc)
{
	LOCATION_SetID(loc, connect->link_zone_id, connect->link_exit_id);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
int EVENTDATA_SearchConnectIDBySphere(const EVENTDATA_SYSTEM * evdata, const VecFx32 * sphere)
{
  enum { HIT_RANGE = FX32_ONE * 16 };
	int i;
  VecFx32 target, check;
  fx32 len;

	const CONNECT_DATA * cnct = evdata->connect_data;
  target = *sphere;
  target.y -= FX32_ONE * (FIELD_CONST_GRID_SIZE / 2);
  if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_R)
  {
    TAMADA_Printf("CNCT:check position (%d, %d, %d)\n",
        FX_Whole(sphere->x), FX_Whole(sphere->y), FX_Whole(sphere->z));
  }

	for (i = 0; i < evdata->connect_count; i++, cnct++ ) {
    ConnectData_GPOS_GetPos(cnct, &check );
    len = VEC_Distance(&check, sphere); 
    
    if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_R)
    {
      TAMADA_Printf("CNCT:ID%02d (%08x, %08x, %08x)", i, FX_Whole(check.x), FX_Whole(check.y), FX_Whole(check.z));
      TAMADA_Printf(" (%3d, %3d, %3d)",
          FX_Whole(check.x) / FIELD_CONST_GRID_SIZE,
          FX_Whole(check.y) / FIELD_CONST_GRID_SIZE,
          FX_Whole(check.z) / FIELD_CONST_GRID_SIZE);
      TAMADA_Printf("\tlen = %d\n", FX_Whole(len) );
    }
    if ( len > HIT_RANGE) continue;
		TAMADA_Printf("CNCT:zone,exit,type=%d,%d,%d\n",cnct->link_zone_id,cnct->link_exit_id,cnct->exit_type);
		//TAMADA_Printf("CNCT:x %d(%08x), y %d(%08x), z %d(%08x)\n",x,sphere->x, y,sphere->y, z,sphere->z);
		return i;
	}
	return EXIT_ID_NONE;
}

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
EXIT_DIR CONNECTDATA_GetExitDir(const CONNECT_DATA * connect)
{
  return connect->exit_dir;
}
//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
EXIT_TYPE CONNECTDATA_GetExitType(const CONNECT_DATA * connect)
{
  return connect->exit_type;
}

//============================================================================================
//		���샂�f���֘A
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	���샂�f���w�b�_�[���擾
 * @param	evdata		�C�x���g�f�[�^�ւ̃|�C���^
 * @retval	MMDL_HEADER*
 */
//------------------------------------------------------------------
const MMDL_HEADER * EVENTDATA_GetNpcData( const EVENTDATA_SYSTEM *evdata )
{
	return( evdata->npc_data );
}

//------------------------------------------------------------------
/**
 * @brief	���샂�f���������擾
 * @param	evdata		�C�x���g�f�[�^�ւ̃|�C���^
 * @retval	u16
 */
//------------------------------------------------------------------
u16 EVENTDATA_GetNpcCount( const EVENTDATA_SYSTEM *evdata )
{
	return( evdata->npc_count );
}

//============================================================================================
//  POS�C�x���g�֘A
//============================================================================================
enum {
  POS_EVENT_DATA_RANGE = 2,
};
//------------------------------------------------------------------
/**
 * @brief	���W�C�x���g�`�F�b�N
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param evwork �C�x���g���[�N�ւ̃|�C���^ 
 * @param pos �`�F�b�N������W
 * @retval POS_EVENT_DATA* (NULL = �C�x���g�Ȃ�)
 */
//------------------------------------------------------------------
const POS_EVENT_DATA * EVENTDATA_GetPosEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos )
{
  const POS_EVENT_DATA *data = evdata->pos_data;
  
  if( data != NULL )
  {
    u16 i = 0;
    u16 *work_val;
    u16 max = evdata->pos_count;

    for( ; i < max; i++, data++ )
    {
      if( !PosEventData_GPOS_IsHit(data, pos) )
      {
        continue;
      }
      {
        work_val = EVENTWORK_GetEventWorkAdrs( evwork, data->workID );
        if( (*work_val) == data->param ){
          return data;
        }
      }
    }
  }
  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	���W�C�x���g���擾
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param evwork �C�x���g���[�N�ւ̃|�C���^ 
 * @param location �`�F�b�N���郌�[�����P�[�V����
 * @retval NULL = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
const POS_EVENT_DATA * EVENTDATA_GetPosEventRailLocation( 
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const RAIL_LOCATION* location )
{
  const POS_EVENT_DATA *data = evdata->pos_data;
  
  if( data != NULL )
  {
    u16 i = 0;
    u16 *work_val;
    u16 max = evdata->pos_count;

    for( ; i < max; i++, data++ )
    {
      if( !PosEventData_RPOS_IsHit(data, location) )
      {
        continue;
      }
      {
        work_val = EVENTWORK_GetEventWorkAdrs( evwork, data->workID );
        if( (*work_val) == data->param ){
          return data;
        }
      }
    }
  }
  return NULL;
}



//------------------------------------------------------------------
/**
 * @brief	���W�C�x���g�`�F�b�N
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param evwork �C�x���g���[�N�ւ̃|�C���^ 
 * @param pos �`�F�b�N������W
 * @retval u16 EVENTDATA_ID_NONE = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
u16 EVENTDATA_CheckPosEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos )
{
  const POS_EVENT_DATA* data = EVENTDATA_GetPosEvent( evdata,evwork,pos );
  
  if( data != NULL ){
    return data->id;
  }
  return EVENTDATA_ID_NONE;
}

//----------------------------------------------------------------------------
/**
 * @brief	���W�C�x���g�`�F�b�N
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param evwork �C�x���g���[�N�ւ̃|�C���^ 
 * @param pos �`�F�b�N������W
 * @retval u16 EVENTDATA_ID_NONE = �C�x���g�Ȃ�
 */
//-----------------------------------------------------------------------------
u16 EVENTDATA_CheckPosEventRailLocation(
  const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const RAIL_LOCATION *pos )
{
  const POS_EVENT_DATA* data = EVENTDATA_GetPosEventRailLocation( evdata,evwork,pos );
  
  if( data != NULL ){
    return data->id;
  }
  return EVENTDATA_ID_NONE;
}


//============================================================================================
//  BGTALK�C�x���g�֘A
//============================================================================================
enum {
  BG_TALK_DATA_RANGE = 2,
};

//------------------------------------------------------------------
/**
 * @brief	BG�b���|���C�x���g�`�F�b�N    ������POSTYPE�𔻒f
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param pos �`�F�b�N������W�@�@VecFx32* or RAIL_LOCATION*
 * @param talk_dir �b���|���Ώۂ̌���
 * @retval u16 EVENTDATA_ID_NONE = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
static u16 EventData_CheckTalkBGEventBase( 
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork,
    const void *pos, u16 talk_dir )
{
  const BG_TALK_DATA *data = evdata->bg_data;
  
  if( data != NULL )
  {
    u16 i = 0;
    u16 max = evdata->bg_count;
    for( ; i < max; i++, data++ )
    {
      if(data->pos_type == EVENTDATA_POSTYPE_GRID)
      {
        if( !BGTalkData_GPOS_IsHit( data, pos ) )
        {
          continue;
        }
      }
      else
      {
        if( !BGTalkData_RPOS_IsHit( data, pos ) )
        {
          continue;
        }
      }
      
      if( data->type == BG_TALK_TYPE_HIDE )
      {
        u16 flag = SCRIPT_GetHideItemFlagNoByScriptID( data->id );
          
        if( EVENTWORK_CheckEventFlag(evwork,flag) == FALSE ){
          return data->id;
        }
      }
      else
      {
        switch( data->dir )
        {
        case BG_TALK_DIR_DOWN:
          if( talk_dir == DIR_UP ){
            return data->id;
          }
          break;
        case BG_TALK_DIR_LEFT:
          if( talk_dir == DIR_RIGHT ){
            return data->id;
          }
          break;
        case BG_TALK_DIR_RIGHT:
          if( talk_dir == DIR_LEFT ){
            return data->id;
          }
          break;
        case BG_TALK_DIR_UP:
          if( talk_dir == DIR_DOWN ){
             return data->id;
          }
          break;
        case BG_TALK_DIR_ALL:
          return data->id;
        case BG_TALK_DIR_SIDE:
          if( talk_dir == DIR_LEFT || talk_dir == DIR_RIGHT ){
            return data->id;
          }
          break;
        case BG_TALK_DIR_UPDOWN:
          if( talk_dir == DIR_UP || talk_dir == DIR_DOWN ){
            return data->id;
          }
          break;
        default:
          GF_ASSERT( 0 );
        }
      }
    }
  }
  
  return EVENTDATA_ID_NONE;
}
  
//------------------------------------------------------------------
/**
 * @brief	BG�b���|���C�x���g�`�F�b�N
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param pos �`�F�b�N������W
 * @param talk_dir �b���|���Ώۂ̌���
 * @retval u16 EVENTDATA_ID_NONE = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
u16 EVENTDATA_CheckTalkBGEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork,
    const VecFx32 *pos, u16 talk_dir )
{
  return EventData_CheckTalkBGEventBase( evdata, evwork, pos, talk_dir );
}

//------------------------------------------------------------------
/**
 * @brief	BG�b���|���C�x���g�`�F�b�N
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param pos �`�F�b�N������W
 * @param talk_dir �b���|���Ώۂ̌���
 * @retval u16 EVENTDATA_ID_NONE = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
u16 EVENTDATA_CheckTalkBGEventRailLocation(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork,
    const RAIL_LOCATION *pos, u16 talk_dir )
{
  return EventData_CheckTalkBGEventBase( evdata, evwork, pos, talk_dir );
}



//------------------------------------------------------------------
/**
 * @brief	�Ŕb���|���C�x���g�`�F�b�N    ������POSTYPE����
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param pos �`�F�b�N������W
 * @param talk_dir �b���|���Ώۂ̌���
 * @retval u16 EVENTDATA_ID_NONE = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
static u16 EventData_CheckTalkBoardEventBase( 
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork,
    const void *pos, u16 talk_dir )
{
  const BG_TALK_DATA *data = evdata->bg_data;
  
  if( talk_dir != DIR_UP && data != NULL )
  {
    u16 i = 0;
    u16 max = evdata->bg_count;
    
    for( ; i < max; i++, data++ )
    {
      if( data->pos_type == EVENTDATA_POSTYPE_GRID )
      {
        if( !BGTalkData_GPOS_IsHit( data, pos ) )
        {
          continue;
        }
      }
      else
      {
        if( !BGTalkData_RPOS_IsHit( data, pos ) )
        {
          continue;
        }
      }
      return data->id;
    }
  }
  
  return EVENTDATA_ID_NONE;
}

//------------------------------------------------------------------
/**
 * @brief	�Ŕb���|���C�x���g�`�F�b�N
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param pos �`�F�b�N������W
 * @param talk_dir �b���|���Ώۂ̌���
 * @retval u16 EVENTDATA_ID_NONE = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
u16 EVENTDATA_CheckTalkBoardEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork,
    const VecFx32 *pos, u16 talk_dir )
{
  return EventData_CheckTalkBoardEventBase( evdata, evwork, pos, talk_dir );
}

//------------------------------------------------------------------
/**
 * @brief	�Ŕb���|���C�x���g�`�F�b�N
 * @param	evdata �C�x���g�f�[�^�ւ̃|�C���^
 * @param pos �`�F�b�N������W
 * @param talk_dir �b���|���Ώۂ̌���
 * @retval u16 EVENTDATA_ID_NONE = �C�x���g�Ȃ�
 */
//------------------------------------------------------------------
u16 EVENTDATA_CheckTalkBoardEventRailLocation(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork,
    const RAIL_LOCATION *pos, u16 talk_dir )
{
  return EventData_CheckTalkBoardEventBase( evdata, evwork, pos, talk_dir );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �o������C�x���g�@���S�RD���W���擾
 *  
 *	@param	data    �C�x���g
 *	@param	pos     ���W�i�[��
 */
//-----------------------------------------------------------------------------
void EVENTDATA_GetConnectCenterPos( const CONNECT_DATA * data, VecFx32* pos )
{
  GF_ASSERT( data );
  GF_ASSERT( pos );
  ConnectData_GPOS_GetPos( data, pos );
}
void EVENTDATA_GetConnectCenterRailLocation( const CONNECT_DATA * data, RAIL_LOCATION* location )
{
  GF_ASSERT( data );
  GF_ASSERT( location );
  ConnectData_RPOS_GetLocation( data, location );
}

//----------------------------------------------------------------------------
/**
 *	@brief  BG�b�������C�x���g�@���S�RD���W���擾
 *  
 *	@param	data    �C�x���g
 *	@param	pos     ���W�i�[��
 */
//-----------------------------------------------------------------------------
void EVENTDATA_GetBGTalkCenterPos( const BG_TALK_DATA * data, VecFx32* pos )
{
  GF_ASSERT( data );
  GF_ASSERT( pos );
  BGTalkData_GPOS_GetPos( data, pos );
}
void EVENTDATA_GetBGTalkCenterRailLocation( const BG_TALK_DATA * data, RAIL_LOCATION* location )
{
  GF_ASSERT( data );
  GF_ASSERT( location );
  BGTalkData_RPOS_GetLocation( data, location );
}


//----------------------------------------------------------------------------
/**
 *	@brief  POS�����C�x���g�@���S�RD���W���擾
 *  
 *	@param	data    �C�x���g
 *	@param	pos     ���W�i�[��
 */
//-----------------------------------------------------------------------------
void EVENTDATA_GetPosEventCenterPos( const POS_EVENT_DATA * data, VecFx32* pos )
{
  GF_ASSERT( data );
  GF_ASSERT( pos );
  PosEventData_GPOS_GetPos( data, pos );
}
void EVENTDATA_GetPosEventCenterRailLocation( const POS_EVENT_DATA * data, RAIL_LOCATION* location )
{
  GF_ASSERT( data );
  GF_ASSERT( location );
  PosEventData_RPOS_GetLocation( data, location );
}




//============================================================================================
//
//		�T���v���f�[�^
//		�����ۂɂ͊O���ŃR���o�[�g���ꂽ���̂��t�@�C������ǂݍ���
//
//============================================================================================

#if 0
const MMDL_HEADER SampleFldMMdlHeader_R01[] = {
	{
		0,		///<����ID
		BOY1,	///<�\������OBJ�R�[�h
		MV_RND,	///<����R�[�h
		0,	///<�C�x���g�^�C�v
		0,	///<�C�x���g�t���O
		0,	///<�C�x���gID
		DIR_DOWN,	///<�w�����
		0,	///<�w��p�����^ 0
		0,	///<�w��p�����^ 1
		0,	///<�w��p�����^ 2
		2,	///<X�����ړ�����
		2,	///<Z�����ړ�����
		757,	///<�O���b�hX
		788,	///<�O���b�hZ
		0,	///<Y�l fx32�^
	},
	{
		1,		///<����ID
		WOMAN1,	///<�\������OBJ�R�[�h
		MV_RND,	///<����R�[�h
		0,	///<�C�x���g�^�C�v
		0,	///<�C�x���g�t���O
		0,	///<�C�x���gID
		DIR_DOWN,	///<�w�����
		0,	///<�w��p�����^ 0
		0,	///<�w��p�����^ 1
		0,	///<�w��p�����^ 2
		2,	///<X�����ړ�����
		2,	///<Z�����ړ�����
		756,	///<�O���b�hX
		796,	///<�O���b�hZ
		0,	///<Y�l fx32�^
	},
};

const int SampleFldMMdlHeaderCount_R01 = NELEMS(SampleFldMMdlHeader_R01);
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------





//============================================================================================
// CONNECT_DATA �|�W�V����
//============================================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���S�@�RD���W�̎擾
 *
 *	@param	cp_data   �f�[�^
 *	@param	p_pos     ���W�i�[��
 */
//-----------------------------------------------------------------------------
static void ConnectData_GPOS_GetPos( const CONNECT_DATA* cp_data, VecFx32* p_pos )
{
  const CONNECT_DATA_GPOS * cp_pos;
  
  GF_ASSERT( cp_data );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_GRID );

  cp_pos = (const CONNECT_DATA_GPOS *)cp_data->pos_buf;

  p_pos->x = (cp_pos->x + (cp_pos->sizex/2) + CONNECT_POS_OFS_X)<<FX32_SHIFT;
  p_pos->y = (cp_pos->y + CONNECT_POS_OFS_Y)<<FX32_SHIFT;
  p_pos->z = (cp_pos->z + (cp_pos->sizez/2) + CONNECT_POS_OFS_Z)<<FX32_SHIFT;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���b�h���W�ƂRD���W�̔���
 *
 *	@param	cp_data   �f�[�^
 *	@param	cp_pos    �RD���W
 *
 *	@retval TRUE  �q�b�g����
 *	@retval FALSE �q�b�g���Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL ConnectData_GPOS_IsHit( const CONNECT_DATA* cp_data, const VecFx32* cp_pos )
{
  const CONNECT_DATA_GPOS * cp_gpos;
  int x, y, z;
  
  GF_ASSERT( cp_data );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_GRID );

  // �O���b�h�|�W�V�����擾
  cp_gpos = (const CONNECT_DATA_GPOS *)cp_data->pos_buf;


	x = FX_Whole(cp_pos->x) - CONNECT_POS_OFS_X;
	y = FX_Whole(cp_pos->y) - CONNECT_POS_OFS_Y;
	z = FX_Whole(cp_pos->z) - CONNECT_POS_OFS_Z;

  if( (cp_gpos->x <= x) && ((cp_gpos->x+cp_gpos->sizex) > x) )
  {
    if( ((cp_gpos->y-CONNECT_DATA_RANGE) <= y) && ((cp_gpos->y+CONNECT_DATA_RANGE) > y) )
    {
      if( (cp_gpos->z <= z) && ((cp_gpos->z+cp_gpos->sizez) > z) )
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���S�@���[�����P�[�V�����̎擾
 *
 *	@param	cp_data     �f�[�^
 *	@param	p_location  ���[�����P�[�V�����̊i�[��
 */
//-----------------------------------------------------------------------------
static void ConnectData_RPOS_GetLocation( const CONNECT_DATA* cp_data, RAIL_LOCATION* p_location )
{
  const CONNECT_DATA_RPOS * cp_pos;
  
  GF_ASSERT( cp_data );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_RAIL );

  cp_pos = (const CONNECT_DATA_RPOS *)cp_data->pos_buf;

  p_location->rail_index  = cp_pos->rail_index;
  p_location->type        = FIELD_RAIL_TYPE_LINE;
  p_location->key         = FIELD_RAIL_TOOL_ConvertDirToRailKey( cp_data->exit_dir );
  p_location->width_grid  = cp_pos->side_grid + (cp_pos->side_grid_size/2);
  p_location->line_grid   = cp_pos->front_grid + (cp_pos->front_grid_size/2);
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����W�̔���
 *
 *	@param	cp_data       �f�[�^
 *	@param	cp_location   ���[�����P�[�V����
 *
 *	@retval TRUE  �͈͓�
 *	@retval FALSE �͈͊O
 */
//-----------------------------------------------------------------------------
static BOOL ConnectData_RPOS_IsHit( const CONNECT_DATA* cp_data, const RAIL_LOCATION* cp_location )
{
  const CONNECT_DATA_RPOS * cp_pos;
  
  GF_ASSERT( cp_data );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_RAIL );

  cp_pos = (const CONNECT_DATA_RPOS *)cp_data->pos_buf;

  if( cp_pos->rail_index == cp_location->rail_index )
  {
    if( (cp_pos->front_grid <= cp_location->line_grid) && ((cp_pos->front_grid+cp_pos->front_grid_size) > cp_location->line_grid) )
    {
      if( (cp_pos->side_grid <= cp_location->width_grid) && ((cp_pos->side_grid+cp_pos->side_grid_size) > cp_location->width_grid) )
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}



//============================================================================================
// BG_TALK_DATA �|�W�V����
//============================================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  BGTalk�@�O���b�h�}�b�v�@�RD���W�̎擾
 *
 *	@param	cp_data �f�[�^
 *	@param	p_pos   �RD���W�i�[��
 */
//-----------------------------------------------------------------------------
static void BGTalkData_GPOS_GetPos( const BG_TALK_DATA* cp_data, VecFx32* p_pos )
{
  const BG_TALK_DATA_GPOS* cp_pos;

  GF_ASSERT( cp_data );
  GF_ASSERT( p_pos );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_GRID );

  cp_pos = (const BG_TALK_DATA_GPOS*)cp_data->pos_buf; 

  VEC_Set( p_pos, GRID_TO_FX32(cp_pos->gx), cp_pos->height<<FX32_SHIFT, GRID_TO_FX32(cp_pos->gz) );
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  BGTalk �O���b�h�}�b�v�@���W�����蔻��
 *
 *	@param	cp_data   �f�[�^
 *	@param	cp_pos    �RD���W
 *
 *	@retval TRUE  �͈͓�
 *	@retval FALSE �͈͊O
 */
//-----------------------------------------------------------------------------
static BOOL BGTalkData_GPOS_IsHit( const BG_TALK_DATA* cp_data, const VecFx32* cp_pos )
{
  const BG_TALK_DATA_GPOS* cp_gpos;
  int gx, gz, y;

  GF_ASSERT( cp_data );
  GF_ASSERT( cp_pos );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_GRID );

  cp_gpos = (const BG_TALK_DATA_GPOS*)cp_data->pos_buf; 

  gx  = SIZE_GRID_FX32(cp_pos->x);
  gz  = SIZE_GRID_FX32(cp_pos->z);
  y   = FX_Whole(cp_pos->y);
  
  if( (cp_gpos->gx == gx) && (cp_gpos->gz == gz) )
  {
    if( ((cp_gpos->height-BG_TALK_DATA_RANGE) <= y) && ((cp_gpos->height+BG_TALK_DATA_RANGE) > y) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BGTalk  ���[�����P�[�V�����̎擾
 *
 *	@param	cp_data       �f�[�^
 *	@param	p_location    ���[�����P�[�V�����i�[��
 */
//-----------------------------------------------------------------------------
static void BGTalkData_RPOS_GetLocation( const BG_TALK_DATA* cp_data, RAIL_LOCATION* p_location )
{
  const BG_TALK_DATA_RPOS * cp_pos;
  
  GF_ASSERT( cp_data );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_RAIL );

  cp_pos = (const BG_TALK_DATA_RPOS *)cp_data->pos_buf;

  p_location->rail_index  = cp_pos->rail_index;
  p_location->type        = FIELD_RAIL_TYPE_LINE;
  p_location->key         = FIELD_RAIL_TOOL_ConvertDirToRailKey( cp_data->dir );
  p_location->width_grid  = cp_pos->side_grid;
  p_location->line_grid   = cp_pos->front_grid;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����W�̔���
 *
 *	@param	cp_data       �f�[�^
 *	@param	cp_location   ���[�����P�[�V����
 *
 *	@retval TRUE  �͈͓�
 *	@retval FALSE �͈͊O
 */
//-----------------------------------------------------------------------------
static BOOL BGTalkData_RPOS_IsHit( const BG_TALK_DATA* cp_data, const RAIL_LOCATION* cp_location )
{
  const BG_TALK_DATA_RPOS * cp_pos;
  
  GF_ASSERT( cp_data );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_RAIL );

  cp_pos = (const BG_TALK_DATA_RPOS *)cp_data->pos_buf;

  if( cp_pos->rail_index == cp_location->rail_index )
  {
    if( cp_pos->front_grid == cp_location->line_grid )
    {
      if( cp_pos->side_grid == cp_location->width_grid )
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

//============================================================================================
// POS_EVENT_DATA �|�W�V����
//============================================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  PosEvent�@�O���b�h�}�b�v�@�RD���W�̎擾
 *
 *	@param	cp_data �f�[�^
 *	@param	p_pos   �RD���W�i�[��
 */
//-----------------------------------------------------------------------------
static void PosEventData_GPOS_GetPos( const POS_EVENT_DATA* cp_data, VecFx32* p_pos )
{
  const POS_EVENT_DATA_GPOS* cp_pos;

  GF_ASSERT( cp_data );
  GF_ASSERT( p_pos );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_GRID );

  cp_pos = (const POS_EVENT_DATA_GPOS*)cp_data->pos_buf; 

  VEC_Set( p_pos, 
      GRID_TO_FX32(cp_pos->gx) + FX_Div( GRID_TO_FX32(cp_pos->sx), FX32_CONST(2) ),
      cp_pos->height<<FX32_SHIFT,
      GRID_TO_FX32(cp_pos->gz) + FX_Div( GRID_TO_FX32(cp_pos->sz), FX32_CONST(2) ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  PosEvent �O���b�h�}�b�v�@���W�����蔻��
 *
 *	@param	cp_data   �f�[�^
 *	@param	cp_pos    �RD���W
 *
 *	@retval TRUE  �͈͓�
 *	@retval FALSE �͈͊O
 */
//-----------------------------------------------------------------------------
static BOOL PosEventData_GPOS_IsHit( const POS_EVENT_DATA* cp_data, const VecFx32* cp_pos )
{
  const POS_EVENT_DATA_GPOS* cp_gpos;
  int gx, gz, y;

  GF_ASSERT( cp_data );
  GF_ASSERT( cp_pos );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_GRID );

  cp_gpos = (const POS_EVENT_DATA_GPOS*)cp_data->pos_buf; 

  gx  = SIZE_GRID_FX32(cp_pos->x);
  gz  = SIZE_GRID_FX32(cp_pos->z);
  y   = FX_Whole(cp_pos->y);
  
  if( (cp_gpos->gx <= gx) && ((cp_gpos->gx+cp_gpos->sx) > gx) )
  {
    if( (cp_gpos->gz <= gz) && ((cp_gpos->gz+cp_gpos->sz) > gz) )
    {
      if( ((cp_gpos->height-BG_TALK_DATA_RANGE) <= y) && ((cp_gpos->height+BG_TALK_DATA_RANGE) > y) )
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  PosEvent  ���[�����P�[�V�����̎擾
 *
 *	@param	cp_data       �f�[�^
 *	@param	p_location    ���[�����P�[�V�����i�[��
 */
//-----------------------------------------------------------------------------
static void PosEventData_RPOS_GetLocation( const POS_EVENT_DATA* cp_data, RAIL_LOCATION* p_location )
{
  const POS_EVENT_DATA_RPOS * cp_pos;
  
  GF_ASSERT( cp_data );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_RAIL );

  cp_pos = (const POS_EVENT_DATA_RPOS *)cp_data->pos_buf;

  p_location->rail_index  = cp_pos->rail_index;
  p_location->type        = FIELD_RAIL_TYPE_LINE;
  p_location->key         = 0;
  p_location->width_grid  = cp_pos->side_grid + (cp_pos->side_grid_size/2);
  p_location->line_grid   = cp_pos->front_grid + (cp_pos->front_grid_size/2);
}

//----------------------------------------------------------------------------
/**
 *	@brief  PosEvent ���[�����W�̔���
 *
 *	@param	cp_data       �f�[�^
 *	@param	cp_location   ���[�����P�[�V����
 *
 *	@retval TRUE  �͈͓�
 *	@retval FALSE �͈͊O
 */
//-----------------------------------------------------------------------------
static BOOL PosEventData_RPOS_IsHit( const POS_EVENT_DATA* cp_data, const RAIL_LOCATION* cp_location )
{
  const POS_EVENT_DATA_RPOS * cp_pos;
  
  GF_ASSERT( cp_data );
  GF_ASSERT( cp_data->pos_type == EVENTDATA_POSTYPE_RAIL );

  cp_pos = (const POS_EVENT_DATA_RPOS *)cp_data->pos_buf;

  if( cp_pos->rail_index == cp_location->rail_index )
  {
    if( (cp_pos->front_grid <= cp_location->line_grid) && ((cp_pos->front_grid+cp_pos->front_grid_size) > cp_location->line_grid) )
    {
      if( (cp_pos->side_grid <= cp_location->width_grid) && ((cp_pos->side_grid+cp_pos->side_grid_size) > cp_location->width_grid) )
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}



#ifdef DEBUG_EVENTDATA_PRINT


//----------------------------------------------------------------------------
/**
 *	@brief  �h�A�f�[�^�̕\��
 */
//-----------------------------------------------------------------------------
static void debugPrint_Connect( const CONNECT_DATA* cp_data )
{
  OS_TPrintf( "print CONNECT_DATA\n" );
  OS_TPrintf( "link_zone_id=%d\n", cp_data->link_zone_id );
  OS_TPrintf( "link_exit_id=%d\n", cp_data->link_exit_id );
  OS_TPrintf( "exit_dir=%d\n", cp_data->exit_dir );
  OS_TPrintf( "exit_type=%d\n", cp_data->exit_type );

  OS_TPrintf( "pos_type=%d\n", cp_data->pos_type );

  if( cp_data->pos_type == EVENTDATA_POSTYPE_GRID )
  {
    const CONNECT_DATA_GPOS* cp_pos = (const CONNECT_DATA_GPOS*)cp_data->pos_buf;
    OS_TPrintf( "x=%d\n", cp_pos->x );
    OS_TPrintf( "y=%d\n", cp_pos->y );
    OS_TPrintf( "z=%d\n", cp_pos->z );
    OS_TPrintf( "sizex=%d\n", cp_pos->sizex );
    OS_TPrintf( "sizez=%d\n", cp_pos->sizez );
  }
  else
  {
    const CONNECT_DATA_RPOS* cp_pos = (const CONNECT_DATA_RPOS*)cp_data->pos_buf;
    OS_TPrintf( "rail_index=%d\n", cp_pos->rail_index );
    OS_TPrintf( "front_grid=%d\n", cp_pos->front_grid );
    OS_TPrintf( "side_grid=%d\n", cp_pos->side_grid );
    OS_TPrintf( "front_grid_size=%d\n", cp_pos->front_grid_size );
    OS_TPrintf( "side_grid_size=%d\n", cp_pos->side_grid_size );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  BG�b�������f�[�^�̕\��
 */
//-----------------------------------------------------------------------------
static void debugPrint_BGTalk( const BG_TALK_DATA* cp_data )
{
  OS_TPrintf( "print BG_TALK_DATA\n" );
  OS_TPrintf( "id=%d\n", cp_data->id );
  OS_TPrintf( "type=%d\n", cp_data->type );
  OS_TPrintf( "dir=%d\n", cp_data->dir );

  OS_TPrintf( "pos_type=%d\n", cp_data->pos_type );

  if( cp_data->pos_type == EVENTDATA_POSTYPE_GRID )
  {
    const BG_TALK_DATA_GPOS* cp_pos = (const BG_TALK_DATA_GPOS*)cp_data->pos_buf;
    OS_TPrintf( "gx=%d\n", cp_pos->gx );
    OS_TPrintf( "gz=%d\n", cp_pos->gz );
    OS_TPrintf( "height=%d\n", cp_pos->height );
  }
  else
  {
    const BG_TALK_DATA_RPOS* cp_pos = (const BG_TALK_DATA_RPOS*)cp_data->pos_buf;
    OS_TPrintf( "rail_index=%d\n", cp_pos->rail_index );
    OS_TPrintf( "front_grid=%d\n", cp_pos->front_grid );
    OS_TPrintf( "side_grid=%d\n", cp_pos->side_grid );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  NPC���̕\��
 */
//-----------------------------------------------------------------------------
static void debugPrint_MMLD( const MMDL_HEADER* cp_data )
{
  OS_TPrintf( "print MMDL_HEADER\n" );
  OS_TPrintf( "id=%d\n", cp_data->id );
  OS_TPrintf( "obj_code=%d\n", cp_data->obj_code );
  OS_TPrintf( "move_code=%d\n", cp_data->move_code );
  OS_TPrintf( "event_type=%d\n", cp_data->event_type );
  OS_TPrintf( "event_flag=%d\n", cp_data->event_flag );
  OS_TPrintf( "event_id=%d\n", cp_data->event_id );
  OS_TPrintf( "dir=%d\n", cp_data->dir );
  OS_TPrintf( "param0=%d\n", cp_data->param0 );
  OS_TPrintf( "param1=%d\n", cp_data->param1 );
  OS_TPrintf( "param2=%d\n", cp_data->param2 );
  OS_TPrintf( "move_limit_x=%d\n", cp_data->move_limit_x );
  OS_TPrintf( "move_limit_z=%d\n", cp_data->move_limit_z );

  OS_TPrintf( "pos_type=%d\n", cp_data->pos_type );

  if( cp_data->pos_type == MMDL_HEADER_POSTYPE_GRID )
  {
    const MMDL_HEADER_GRIDPOS* cp_pos = (const MMDL_HEADER_GRIDPOS*)cp_data->pos_buf;
    OS_TPrintf( "gx=%d\n", cp_pos->gx );
    OS_TPrintf( "gz=%d\n", cp_pos->gz );
    OS_TPrintf( "y=%d\n", cp_pos->y );
  }
  else
  {
    const MMDL_HEADER_RAILPOS* cp_pos = (const MMDL_HEADER_RAILPOS*)cp_data->pos_buf;
    OS_TPrintf( "rail_index=%d\n", cp_pos->rail_index );
    OS_TPrintf( "front_grid=%d\n", cp_pos->front_grid );
    OS_TPrintf( "side_grid=%d\n", cp_pos->side_grid );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  POS�����f�[�^�̕\��
 */
//-----------------------------------------------------------------------------
static void debugPrint_PosData( const POS_EVENT_DATA* cp_data )
{
  OS_TPrintf( "print POS_EVENT_DATA\n" );
  OS_TPrintf( "id=%d\n", cp_data->id );
  OS_TPrintf( "param=%d\n", cp_data->param );
  OS_TPrintf( "workID=%d\n", cp_data->workID );

  OS_TPrintf( "pos_type=%d\n", cp_data->pos_type );

  if( cp_data->pos_type == EVENTDATA_POSTYPE_GRID )
  {
    const POS_EVENT_DATA_GPOS* cp_pos = (const POS_EVENT_DATA_GPOS*)cp_data->pos_buf;
    OS_TPrintf( "gx=%d\n", cp_pos->gx );
    OS_TPrintf( "gz=%d\n", cp_pos->gz );
    OS_TPrintf( "sx=%d\n", cp_pos->sx );
    OS_TPrintf( "sz=%d\n", cp_pos->sz );
    OS_TPrintf( "height=%d\n", cp_pos->height );
  }
  else
  {
    const POS_EVENT_DATA_RPOS* cp_pos = (const POS_EVENT_DATA_RPOS*)cp_data->pos_buf;
    OS_TPrintf( "rail_index=%d\n", cp_pos->rail_index );
    OS_TPrintf( "front_grid=%d\n", cp_pos->front_grid );
    OS_TPrintf( "side_grid=%d\n", cp_pos->side_grid );
    OS_TPrintf( "front_grid_size=%d\n", cp_pos->front_grid_size );
    OS_TPrintf( "side_grid_size=%d\n", cp_pos->side_grid_size );
  }
}

#endif //DEBUG_EVENTDATA_PRINT




