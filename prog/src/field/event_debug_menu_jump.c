//======================================================================
/**
 *
 * @file  event_debug_menu_jump.c
 * @brief �t�B�[���h�f�o�b�O���j���[�F�}�b�v�J�ڊ֘A
 * @author  GAMEFREAK inc.
 * @date  2009.11.29
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "arc_def.h"


#include "message.naix"
#include "msg/msg_d_field.h"

#include "fieldmap.h"
#include "field/zonedata.h"
#include "arc/fieldmap/zone_id.h"

#include "event_debug_menu.h"
#include "event_mapchange.h"


#include "app/townmap.h"




#include "field/field_comm/intrude_field.h" //PALACE_MAP_LEN

#include "event_debug_local.h"

#include "debug/debug_str_conv.h" // for DEB_STR_CONV_SjisToStrcode

static void DEBUG_SetMenuWorkZoneIDNameAll(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work );
static u16 DEBUG_GetZoneIDNameMax( GAMESYS_WORK* gsys, void* cb_work );

//======================================================================
//  �f�o�b�O���j���[ �ǂ��ł��W�����v
//======================================================================
//--------------------------------------------------------------
/// DEBUG_ZONESEL_EVENT_WORK �ǂ��ł��W�����v�����p���[�N
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GAMEDATA * gdata;
  FIELDMAP_WORK *fieldWork;
  FLDMENUFUNC *menuFunc;
}DEBUG_ZONESEL_EVENT_WORK;

//--------------------------------------------------------------
//--------------------------------------------------------------
static void initDZEW(DEBUG_ZONESEL_EVENT_WORK * dzew,
    GAMESYS_WORK * gsys, HEAPID heapID)
{
	GFL_STD_MemClear( dzew, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
	
	dzew->gmSys = gsys;
	dzew->heapID = heapID;
	dzew->fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
}
//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuZoneSelectEvent(
    GMEVENT *event, int *seq, void *work );

/// �ǂ��ł��W�����v�@���j���[�w�b�_�[
const FLDMENUFUNC_HEADER DATA_DebugMenuList_ZoneSel =
{
  1,    //���X�g���ڐ�
  9,    //�\���ő區�ڐ�
  0,    //���x���\���w���W
  13,   //���ڕ\���w���W
  0,    //�J�[�\���\���w���W
  0,    //�\���x���W
  1,    //�\�������F
  15,   //�\���w�i�F
  2,    //�\�������e�F
  0,    //�����Ԋu�w
  1,    //�����Ԋu�x
  FLDMENUFUNC_SKIP_LRKEY, //�y�[�W�X�L�b�v�^�C�v
  12,   //�����T�C�YX(�h�b�g
  12,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
};

//--------------------------------------------------------------
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER DebugZoneSelectData = {
  NARC_message_d_field_dat,
  0,
  NULL,
  &DATA_DebugMenuList_ZoneSel,
  1, 1, 20, 16,
  DEBUG_SetMenuWorkZoneIDNameAll,
  DEBUG_GetZoneIDNameMax,
};

//--------------------------------------------------------------
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_FLDMENU_JumpAllZone( GAMESYS_WORK * gsys, HEAPID heapID )
{
  GMEVENT * new_event = GMEVENT_Create(
      gsys, NULL, debugMenuZoneSelectEvent, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
  DEBUG_ZONESEL_EVENT_WORK * work = GMEVENT_GetEventWork( new_event );
  initDZEW( work, gsys, heapID );
  return new_event;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�ǂ��ł��W�����v
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuZoneSelectEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_ZONESEL_EVENT_WORK *work = wk;
  
  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugZoneSelectData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
      
      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }
      
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      
      if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        return( GMEVENT_RES_FINISH );
      }
      
      {
        GMEVENT * mapchange_event;
        mapchange_event = DEBUG_EVENT_QuickChangeMapDefaultPos( work->gmSys, work->fieldWork, ret );
        GMEVENT_ChangeEvent( event, mapchange_event );
      }
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  �f�o�b�O���j���[ �W�����v
//======================================================================
//--------------------------------------------------------------
/// proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuZoneJump(GMEVENT *event, int *seq, void *wk );
static void DEBUG_SetSTRBUF_ZoneIDName(u32 heapID, u32 zoneID, STRBUF *strBuf );

//--------------------------------------------------------------
//  data
//--------------------------------------------------------------
///�u�W�����v�v�R�}���h�őI���\�ȃ]�[��ID
static const u16 JumpZoneID_ListTbl[] = {
  ZONE_ID_UNION,
  ZONE_ID_PALACE01,
  ZONE_ID_PALACE02,
  ZONE_ID_GT,
  ZONE_ID_D09,
  
#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko) | defined(DEBUG_ONLY_FOR_iwao_kazumasa)
  ZONE_ID_C04R0201,
  ZONE_ID_C04R0202,
#endif
};

//--------------------------------------------------------------
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_FLDMENU_JumpEasy( GAMESYS_WORK * gsys, HEAPID heapID )
{
  GMEVENT * new_event;
  DEBUG_ZONESEL_EVENT_WORK *work;
  
  new_event = GMEVENT_Create( gsys, NULL, debugMenuZoneJump, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
  work = GMEVENT_GetEventWork( new_event );
  initDZEW( work, gsys, heapID );

  return new_event;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkZoneID_SelectZone(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  int i;
  STRBUF *strBuf = GFL_STR_CreateBuffer( 64, heapID );
  
  for(i = 0; i < NELEMS(JumpZoneID_ListTbl); i++){
    GFL_STR_ClearBuffer( strBuf );
    DEBUG_SetSTRBUF_ZoneIDName( heapID, JumpZoneID_ListTbl[i], strBuf );
    FLDMENUFUNC_AddStringListData( list, strBuf, JumpZoneID_ListTbl[i], heapID );
  }
  
  GFL_HEAP_FreeMemory( strBuf );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER DebugMenuZoneJumpData = {
  NARC_message_d_field_dat,
  NELEMS(JumpZoneID_ListTbl),
  NULL,
  &DATA_DebugMenuList_ZoneSel, //���p
  1, 1, 11, 16,
  DEBUG_SetMenuWorkZoneID_SelectZone,
  NULL,
};

//--------------------------------------------------------------
/**
 * �C�x���g�F�W�����v
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuZoneJump(GMEVENT *event, int *seq, void *wk )
{
  DEBUG_ZONESEL_EVENT_WORK *work = wk;
  
  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugMenuZoneJumpData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );
      
      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }
      
      FLDMENUFUNC_DeleteMenu( work->menuFunc );
      
      if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        return( GMEVENT_RES_FINISH );
      }
      
      {
        GMEVENT * mapchange_event;
        if(ret == ZONE_ID_UNION){
          mapchange_event = EVENT_ChangeMapToUnion(work->gmSys, work->fieldWork);
        }
        else if(ret == ZONE_ID_PALACE01){
          VecFx32 pos = {PALACE_MAP_LEN/2, 32*FX32_ONE, 488*FX32_ONE};
          mapchange_event = EVENT_ChangeMapPos(work->gmSys, work->fieldWork, ret, &pos, 0);
        }
        else{
          mapchange_event = DEBUG_EVENT_ChangeMapDefaultPos( work->gmSys, work->fieldWork, ret );
        }
        GMEVENT_ChangeEvent( event, mapchange_event );
      }
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  �f�o�b�O���j���[�@�l�G�W�����v
//======================================================================
//--------------------------------------------------------------
//  proto
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuSeasonSelectEvent(
    GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_FLDMENU_ChangeSeason( GAMESYS_WORK * gsys, HEAPID heapID )
{
  GMEVENT * new_event = GMEVENT_Create(
      gsys, NULL, debugMenuSeasonSelectEvent, sizeof(DEBUG_ZONESEL_EVENT_WORK) );
  DEBUG_ZONESEL_EVENT_WORK * work = GMEVENT_GetEventWork( new_event );
  initDZEW( work, gsys, heapID );
  return new_event;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
#include "gamesystem/pm_season.h"
static const FLDMENUFUNC_LIST DATA_SeasonMenuList[PMSEASON_TOTAL] =
{
  { DEBUG_FIELD_STR_SPRING, (void*)PMSEASON_SPRING },
  { DEBUG_FIELD_STR_SUMMER, (void*)PMSEASON_SUMMER },
  { DEBUG_FIELD_STR_AUTUMN, (void*)PMSEASON_AUTUMN },
  { DEBUG_FIELD_STR_WINTER, (void*)PMSEASON_WINTER },
};

static const DEBUG_MENU_INITIALIZER DebugSeasonJumpMenuData = {
  NARC_message_d_field_dat,
  NELEMS(DATA_SeasonMenuList),
  DATA_SeasonMenuList,
  &DATA_DebugMenuList_ZoneSel, //���p
  1, 1, 16, 17,
  NULL,
  NULL
};

//--------------------------------------------------------------
/**
 * �C�x���g�F�l�G�W�����v
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT debugMenuSeasonSelectEvent(
    GMEVENT *event, int *seq, void *wk )
{
  DEBUG_ZONESEL_EVENT_WORK *work = wk;

  switch( (*seq) ){
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugSeasonJumpMenuData );
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }
      
      FLDMENUFUNC_DeleteMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        return( GMEVENT_RES_FINISH );
      }else{
        GMEVENT *mapchange_event;
        GAMEDATA *gdata = GAMESYSTEM_GetGameData( work->gmSys );
        PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
        const VecFx32 *pos = PLAYERWORK_getPosition( player );
        const RAIL_LOCATION *location = PLAYERWORK_getRailPosition( player );
        u16 dir = PLAYERWORK_getDirection( player );
        ZONEID zone_id = PLAYERWORK_getZoneID(player);
        
        if( (dir>0x2000) && (dir<0x6000) ){
          dir = EXIT_DIR_LEFT;
        }else if( (dir >= 0x6000) && (dir <= 0xa000) ){
          dir = EXIT_DIR_DOWN;
        }else if( (dir > 0xa000) && (dir < 0xe000) ){
          dir = EXIT_DIR_RIGHT;
        }else{
          dir = EXIT_DIR_UP;
        }

        { //DS�̕ێ����Ă���RTC�𒼐ڐG���Ă���
          u16 now_season = GAMEDATA_GetSeasonID( gdata );
          RTCDate date;
          GFL_RTC_GetDate( &date );
          if ( now_season < ret ) {
            date.month += ret - now_season;
          } else {
            date.month += PMSEASON_TOTAL + ret - now_season;
          }
          if (date.month > 12 ) {
            date.month = 1;
            date.year += 1;
          }
          RTC_SetDate( &date );
        }
        //GAMEDATA_SetSeasonID(gdata, ret);
        if( FIELDMAP_GetMapControlType( work->fieldWork ) == FLDMAP_CTRLTYPE_GRID )
        {
          mapchange_event = EVENT_ChangeMapPos(
            work->gmSys, work->fieldWork, zone_id, pos, dir );
        }
        else
        {
          mapchange_event = DEBUG_EVENT_ChangeMapRailLocation(
            work->gmSys, work->fieldWork, zone_id, location, dir );
        }
        GMEVENT_ChangeEvent( event, mapchange_event );
        OS_Printf( "x = %xH, z = %xH\n", pos->x, pos->z );
      }
    }
    break;
  }
  
  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * Zone ID Name -> BMP_MENULIST_DATA 
 * @param heapID  �e���|����������m�ۗpheapID
 * @param zoneID  ��������擾������zoneID
 * @param strBuf  ������i�[��STRBUF
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetSTRBUF_ZoneIDName(
    u32 heapID, u32 zoneID, STRBUF *strBuf )
{
  u16 *pStrCode;
  char name8[64];
  
  pStrCode = GFL_HEAP_AllocClearMemory( heapID, sizeof(u16)*64 );

  ZONEDATA_DEBUG_GetZoneName( name8, zoneID );
  DEB_STR_CONV_SjisToStrcode( name8, pStrCode, 64 );
  GFL_STR_SetStringCode( strBuf, pStrCode );

  GFL_HEAP_FreeMemory( pStrCode );
}

static void setDebugZoneIDStr( const char * allName, u32 zoneID, STRBUF * strBuf )
{
  u16 pStrCode[64];
  
  //pStrCode = GFL_HEAP_AllocClearMemory( heapID, sizeof(u16)*64 );

  DEB_STR_CONV_SjisToStrcode( &allName[ ZONEDATA_NAME_LENGTH * zoneID ], pStrCode, 64 );
  GFL_STR_SetStringCode( strBuf, pStrCode );

  //GFL_HEAP_FreeMemory( pStrCode );
}
//--------------------------------------------------------------
/**
 * �����ł��W�����v�pBMP_MENULIST_DATA�Z�b�g
 * @param list  �Z�b�g��BMP_MENULIST_DATA
 * @param heapID  ������o�b�t�@�m�ۗpHEAPID
 * @retval  nothing
 */
//--------------------------------------------------------------
static void DEBUG_SetMenuWorkZoneIDNameAll(
    GAMESYS_WORK * gsys, FLDMENUFUNC_LISTDATA *list, HEAPID heapID, GFL_MSGDATA* msgData, void* cb_work )
{
  int id,max = ZONEDATA_GetZoneIDMax();
  const char * nameAllBuf = ZONEDATA_GetAllZoneName( heapID );
  STRBUF *strBuf1 = GFL_STR_CreateBuffer( 64, heapID );
  STRBUF *strBuf2 = GFL_STR_CreateBuffer( 64, heapID );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMSGBG * msgBG = FIELDMAP_GetFldMsgBG( fieldmap );
  GFL_MSGDATA * pMsgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_place_name_dat );
  
  for( id = 0; id < max; id++ ){
    u16 str_id = ZONEDATA_GetPlaceNameID( id );
    GFL_STR_ClearBuffer( strBuf1 );
    GFL_MSG_GetString( pMsgData,  str_id, strBuf1 );  // �n����������擾
    GFL_STR_ClearBuffer( strBuf2 );
    setDebugZoneIDStr( nameAllBuf, id, strBuf2 );
 //   DEBUG_SetSTRBUF_ZoneIDName( heapID, id, strBuf2 );
    GFL_STR_AddString( strBuf2, strBuf1 );
    FLDMENUFUNC_AddStringListData( list, strBuf2, id, heapID );
  }
  
  GFL_MSG_Delete( pMsgData );
  GFL_HEAP_FreeMemory( strBuf1 );
  GFL_HEAP_FreeMemory( strBuf2 );
  GFL_HEAP_FreeMemory( (void*)nameAllBuf );
}

//--------------------------------------------------------------
/**
 * @brief �����ł��W�����v�p���X�g�ő�l�擾�֐�
 * @param fieldmap
 * @return  �}�b�v�ő吔
 */
//--------------------------------------------------------------
static u16 DEBUG_GetZoneIDNameMax( GAMESYS_WORK* gsys, void* cb_work )
{
  return ZONEDATA_GetZoneIDMax();
}

