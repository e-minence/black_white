#ifdef PM_DEBUG
////////////////////////////////////////////////////////////////////////
/**
 *  @brief  �f�o�b�O�@�\�F�~�b�V�����I��
 *  @file   event_debug_numinput_mission.c
 *  @author obata
 *  @date   2010.06.14
 */
//////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "fieldmap.h"
#include "field/eventwork.h"
#include "event_debug_local.h"
#include "gamesystem/gamesystem.h"
#include "event_debug_numinput.h"
#include "event_debug_numinput_record.h"

#include "system/main.h"
#include "field/field_msgbg.h"
#include "field_comm/mission_types.h"
#include "field_comm/intrude_mission.h"
#include "print/wordset.h"
#include "savedata/misc.h"
#include "savedata/situation.h"
#include "savedata/intrude_save.h"

#include "arc_def.h"
#include "debug_message.naix"
#include "msg/debug/msg_debug_intrude.h"

#include "event_debug_numinput_mission.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
///���l���̓��X�g�f�[�^
typedef struct {
  u16 gmm_id;
  u32 param;
  const DEBUG_NUMINPUT_PARAM * dni_param;
}DEBUG_NUMINPUT_LIST;

///���l���̓��X�g�������p�����[�^��`
typedef struct{ 
  const u32 count_or_id; // ���X�g�̍ő吔
  const DEBUG_NUMINPUT_LIST * pList; // �e���ڂ̐��l���͎������w��f�[�^
} DEBUG_NUMINPUT_INITIALIZER;

//--------------------------------------------------------------
//--------------------------------------------------------------
static u32 MissionGetFunc( GAMESYS_WORK* gsys, GAMEDATA* gamedata, u32 param );
static void MissionSetFunc( GAMESYS_WORK* gsys, GAMEDATA* gamedata, u32 mission_type, u32 mission_no );

static const DEBUG_NUMINPUT_PARAM DNIParam_Mission = { 0, 0, MissionGetFunc, MissionSetFunc }; 

static const DEBUG_NUMINPUT_LIST DNIList_Mission[] = 
{
  { STR_INTRUDE_MISSION_01, MISSION_TYPE_BASIC,       &DNIParam_Mission },
  { STR_INTRUDE_MISSION_02, MISSION_TYPE_SKILL,       &DNIParam_Mission },
  { STR_INTRUDE_MISSION_03, MISSION_TYPE_VICTORY,     &DNIParam_Mission },
  { STR_INTRUDE_MISSION_04, MISSION_TYPE_PERSONALITY, &DNIParam_Mission },
  { STR_INTRUDE_MISSION_05, MISSION_TYPE_ITEM,        &DNIParam_Mission },
  { STR_INTRUDE_MISSION_06, MISSION_TYPE_ATTRIBUTE,   &DNIParam_Mission },
};

static const DEBUG_NUMINPUT_INITIALIZER DNI_Init = { NELEMS( DNIList_Mission ), DNIList_Mission, };

static const FLDMENUFUNC_LIST DATA_DNumInputMenu[] =
{
  { STR_INTRUDE_MISSION_01, (void*)0 },
  { STR_INTRUDE_MISSION_02, (void*)1 },
  { STR_INTRUDE_MISSION_03, (void*)2 },
  { STR_INTRUDE_MISSION_04, (void*)3 },
  { STR_INTRUDE_MISSION_05, (void*)4 },
  { STR_INTRUDE_MISSION_06, (void*)5 },
}; 

//--------------------------------------------------------------
//-------------------------------------------------------------- 
/// ���l���́@���j���[�w�b�_�[
static const FLDMENUFUNC_HEADER DATA_DNumInput_MenuFuncHeader =
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

static const DEBUG_MENU_INITIALIZER DATA_DNumInput_MenuInitializer = {
  NARC_debug_message_debug_intrude_dat,  //���b�Z�[�W�A�[�J�C�u
  NELEMS(DATA_DNumInputMenu),  //���ڐ�max
  DATA_DNumInputMenu, //���j���[���ڃ��X�g
  &DATA_DNumInput_MenuFuncHeader, //���j���w�b�_
  1, 4, 30, 16,
  NULL,
  NULL,
};


//////////////////////////////////////////////////////////////// 
static GMEVENT * DEBUG_EVENT_MissionSelect( GAMESYS_WORK * gsys, const DEBUG_NUMINPUT_PARAM * dni_param, u32 id );

typedef struct {
  HEAPID heapID;
  GAMESYS_WORK *gmSys;
  GAMEDATA * gamedata;
  FIELDMAP_WORK * fieldWork; 
  DEBUG_MENU_CALLPROC call_proc;
  FLDMENUFUNC *menuFunc; 
  const DEBUG_NUMINPUT_INITIALIZER * init;
} MISSION_TYPE_SELECT_WORK;

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT_RESULT MissionTypeSelectEvent( GMEVENT *event, int *seq, void *wk )
{
	MISSION_TYPE_SELECT_WORK * new_wk = wk;
  EVENTWORK *evwork = GAMEDATA_GetEventWork( new_wk->gamedata );
  const DEBUG_NUMINPUT_INITIALIZER * init = new_wk->init;
  
  switch( (*seq) ){
  case 0:
    new_wk->menuFunc = DEBUGFLDMENU_InitEx( new_wk->fieldWork, new_wk->heapID,  &DATA_DNumInput_MenuInitializer, new_wk );
    (*seq)++;
    break;

  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( new_wk->menuFunc );
      
      if( ret == FLDMENUFUNC_NULL ){ break; } // ���얳��

      if( ret == FLDMENUFUNC_CANCEL ) {  //�L�����Z��
        FLDMENUFUNC_DeleteMenu( new_wk->menuFunc );
        return( GMEVENT_RES_FINISH );
      }

      GMEVENT_CallEvent( event, 
          DEBUG_EVENT_MissionSelect( new_wk->gmSys, init->pList[ret].dni_param, init->pList[ret].param ) );
      break;
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
GMEVENT * DEBUG_NUM_INPUT_EVENT_MissionTypeSelect( GAMESYS_WORK * gsys )
{
	GMEVENT * new_event = GMEVENT_Create( gsys, NULL, MissionTypeSelectEvent, sizeof(MISSION_TYPE_SELECT_WORK) );
	MISSION_TYPE_SELECT_WORK * new_wk = GMEVENT_GetEventWork( new_event );

	GFL_STD_MemClear( new_wk, sizeof(MISSION_TYPE_SELECT_WORK) );
  new_wk->heapID = HEAPID_PROC;
  new_wk->gmSys = gsys;
  new_wk->gamedata = GAMESYSTEM_GetGameData( gsys );
  new_wk->fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  new_wk->call_proc = NULL;
  new_wk->menuFunc = NULL; 
  new_wk->init = &DNI_Init;

  return new_event;
}


////////////////////////////////////////////////////////////////

enum {
  _DISP_INITX = 1,
  _DISP_INITY = 18,
  _DISP_SIZEX = 30,
  _DISP_SIZEY = 4,

  PANO_FONT = 14,
  FBMP_COL_WHITE = 15,
}; 

#define WINCLR_COL(col) (((col)<<4)|(col))

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  /// ����Ώێw��ID
  u32 select_id;
  /// ���쐧��p�����[�^
  const DEBUG_NUMINPUT_PARAM * dni_param;

  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  FIELDMAP_WORK * fieldmap;
  HEAPID heapID;
  GFL_FONT * fontHandle;

	GFL_MSGDATA* msgman;						//���b�Z�[�W�}�l�[�W���[
  WORDSET * wordset;
  GFL_BMPWIN * bmpwin;

  u32 value;
  int mission_no_search_work;
} DEBUG_NUMINPUT_WORK;

//--------------------------------------------------------------
/// ���l���̓E�B���h�E�F����
//--------------------------------------------------------------
static void createNumWin( DEBUG_NUMINPUT_WORK * wk )
{
	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;
  
  wk->wordset = WORDSET_Create( wk->heapID );
  wk->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_debug_intrude_dat, wk->heapID);

	bmpwin = GFL_BMPWIN_Create( FLDBG_MFRM_MSG, _DISP_INITX, _DISP_INITY, _DISP_SIZEX, _DISP_SIZEY, PANO_FONT, GFL_BMP_CHRAREA_GET_B );
  wk->bmpwin = bmpwin;
  
	bmp = GFL_BMPWIN_GetBmp( bmpwin );
	
	GFL_BMP_Clear( bmp, WINCLR_COL(FBMP_COL_WHITE) );
	GFL_BMPWIN_MakeScreen( bmpwin );
	GFL_BMPWIN_TransVramCharacter( bmpwin );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( bmpwin ) );
}

//--------------------------------------------------------------
/// ���l���̓E�B���h�E�F�폜
//--------------------------------------------------------------
static void deleteNumWin( DEBUG_NUMINPUT_WORK * wk )
{
  GFL_BMPWIN * bmpwin = wk->bmpwin;

	GFL_BMPWIN_ClearScreen( bmpwin );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( bmpwin ) );

  GFL_BMPWIN_Delete( bmpwin );

  GFL_MSG_Delete( wk->msgman );
  WORDSET_Delete( wk->wordset );
}

//--------------------------------------------------------------
/// ���l���̓E�B���h�E�F�\���X�V
//--------------------------------------------------------------
static void printNumWin( DEBUG_NUMINPUT_WORK * wk, u32 num )
{
  const DEBUG_NUMINPUT_PARAM * def = wk->dni_param;
  GFL_BMPWIN * bmpwin = wk->bmpwin;

  STRBUF * strbuf = GFL_STR_CreateBuffer( 64, wk->heapID );
  STRBUF * expandBuf = GFL_STR_CreateBuffer( 64, wk->heapID );

  GFL_MSG_GetString( wk->msgman, STR_INTRUDE_NUMBER, strbuf );
  WORDSET_RegisterNumber(wk->wordset, 0, num, 10, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( wk->wordset, expandBuf, strbuf );

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp( bmpwin ), WINCLR_COL(FBMP_COL_WHITE) );

  GFL_FONTSYS_SetColor( 1, 2, 15 );
	PRINTSYS_Print( GFL_BMPWIN_GetBmp( bmpwin ), 1, 10, expandBuf, wk->fontHandle );		

  GFL_BMPWIN_TransVramCharacter( bmpwin );  //transfer characters
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( bmpwin ) );  //transfer screen

  GFL_STR_DeleteBuffer( strbuf );
  GFL_STR_DeleteBuffer( expandBuf );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void dniSetValue( DEBUG_NUMINPUT_WORK * dni_wk , u32 value )
{
  int mission_type = dni_wk->select_id;
  int mission_no = dni_wk->value;
  OCCUPY_INFO* occupy_info = GAMEDATA_GetMyOccupyInfo( dni_wk->gamedata );
  DEBUG_MISSION_TypeNoSet( occupy_info, mission_type, mission_no );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static u32 dniGetValue( DEBUG_NUMINPUT_WORK * dni_wk )
{
  int mission_type = dni_wk->select_id;
  OCCUPY_INFO* occupy_info = GAMEDATA_GetMyOccupyInfo( dni_wk->gamedata );
  return DEBUG_MISSION_TypeNoGet( occupy_info, mission_type );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static u32 dniGetNextValue( DEBUG_NUMINPUT_WORK * dni_wk )
{
  int mission_type = dni_wk->select_id;
  int mission_no = dni_wk->value;

  mission_no = DEBUG_MISSION_TypeSearch( mission_type, &dni_wk->mission_no_search_work );

  // ��荞��
  if( mission_no == -1 ) {
    dni_wk->mission_no_search_work = 0;
    mission_no = DEBUG_MISSION_TypeSearch( mission_type, &dni_wk->mission_no_search_work );
  }

  return mission_no;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static u32 dniGetPrevValue( DEBUG_NUMINPUT_WORK * dni_wk )
{
  int mission_type = dni_wk->select_id;
  int mission_no = dni_wk->value;
  int prev_no = dni_wk->value;

  mission_no = DEBUG_MISSION_TypeSearch( mission_type, &dni_wk->mission_no_search_work );

  while( mission_no != dni_wk->value )
  {
    if( mission_no == -1 ) { // ��荞��
      dni_wk->mission_no_search_work = 0;
    }
    else {
      prev_no = mission_no;
    }
    mission_no = DEBUG_MISSION_TypeSearch( mission_type, &dni_wk->mission_no_search_work );
  }

  return prev_no;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT_RESULT DebugNumInputEvent( GMEVENT * event , int *seq, void * work )
{
  DEBUG_NUMINPUT_WORK * dni_wk = work;
  EVENTWORK *evwork = GAMEDATA_GetEventWork( dni_wk->gamedata );
  const DEBUG_NUMINPUT_PARAM * def = dni_wk->dni_param;

  switch(*seq) {
  case 0:
    dni_wk->value = dniGetValue( dni_wk );
    createNumWin( dni_wk );
    printNumWin( dni_wk, dni_wk->value );
    (*seq) ++;
    break;
  case 1:
    {
      int trg = GFL_UI_KEY_GetRepeat();
      int cont = GFL_UI_KEY_GetCont();
      u32 after = dni_wk->value;
      u32 before = dni_wk->value;

      if( trg & PAD_BUTTON_B ) { // �L�����Z��
        (*seq) ++;
        break;
      }
      if( trg & PAD_BUTTON_A ) { // ����
        dniSetValue( dni_wk, dni_wk->value );
        (*seq) ++;
        break;
      }

      if( trg & PAD_KEY_UP ) {
        after = dniGetNextValue( dni_wk );
      } 
      else if( trg & PAD_KEY_DOWN ) {
        after = dniGetPrevValue( dni_wk );
      }
      else if( trg & PAD_KEY_RIGHT ) {
        int i;
        for( i=0; i<5; i++ ) {
          after = dniGetNextValue( dni_wk );
        }
      }
      else if( trg & PAD_KEY_LEFT ) {
        int i;
        for( i=0; i<5; i++ ) {
          after = dniGetPrevValue( dni_wk );
        }
      }

      if( after != before ) {
        printNumWin( dni_wk, after );
        dni_wk->value = after;
      }
    }
    break;

  case 2:
    deleteNumWin( dni_wk );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * DEBUG_EVENT_MissionSelect( GAMESYS_WORK * gsys, const DEBUG_NUMINPUT_PARAM * dni_param, u32 id )
{
  GMEVENT * event;
  DEBUG_NUMINPUT_WORK * wk;
  event = GMEVENT_Create( gsys, NULL, DebugNumInputEvent, sizeof(DEBUG_NUMINPUT_WORK) );
  wk = GMEVENT_GetEventWork( event );

  wk->select_id = id;
  wk->dni_param = dni_param;
  wk->gsys = gsys;
  wk->gamedata = GAMESYSTEM_GetGameData( gsys );
  wk->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  wk->fontHandle = FLDMSGBG_GetFontHandle( FIELDMAP_GetFldMsgBG( wk->fieldmap ) );
  wk->heapID = FIELDMAP_GetHeapID( wk->fieldmap );
  wk->mission_no_search_work = 0;

  return event;
}

//--------------------------------------------------------------
//-------------------------------------------------------------- 
static u32 MissionGetFunc( GAMESYS_WORK* gsys, GAMEDATA* gamedata, u32 param )
{
  return 0;
}

static void MissionSetFunc( GAMESYS_WORK* gsys, GAMEDATA* gamedata, u32 mission_type, u32 mission_no )
{
}

#endif  // PM_DEBUG
