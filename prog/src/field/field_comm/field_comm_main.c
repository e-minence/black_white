//======================================================================
/**
 * @file  field_comm_main.c
 * @brief �t�B�[���h�ʐM�̏����̊O�ƒ����Ȃ��֐��Ƃ��Q
 * @author  ariizumi
 * @data  08/11/11
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "infowin/infowin.h"
#include "gamesystem/gamesystem.h"
#include "test/ariizumi/ari_debug.h"
#include "field_comm_main.h"
#include "field_comm_menu.h"
#include "field_comm_func.h"
#include "field_comm_data.h"
#include "field_comm_sys.h"
#include "field/field_comm/palace_sys.h"
#include "test/performance.h"
#include "system/main.h"
#include "field/field_comm_actor.h"
#include "gamesystem/game_comm.h"
#include "fieldmap/zone_id.h"
#include "field/event_mapchange.h"

#include "message.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_invasion.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  enum
//======================================================================

//======================================================================
//  typedef struct
//======================================================================
struct _FIELD_COMM_MAIN
{
  HEAPID  heapID_;
  u8  comm_type;
  u8  menuSeq_;
  u8  talkTrgChara_;
  u8  padding;
  FIELD_COMM_MENU *commMenu_;
#if 0 ////
  FIELD_COMM_FUNC *commFunc_;
#else
 COMM_FIELD_SYS_PTR commField_;
#endif
  u8  commActorIndex_[FIELD_COMM_MEMBER_MAX];
  PALACE_SYS_PTR palace;    ///<�p���X�V�X�e�����[�N�ւ̃|�C���^
  FIELD_COMM_ACTOR_CTRL *actCtrl_;
  GAME_COMM_SYS_PTR game_comm;
  
  u8 debug_palace_comm_seq;
  u8 padding2[3];
};

///�f�o�b�O�p
typedef struct{
  VecFx32 pos;
  FIELD_COMM_MAIN *commSys;
  FIELD_MAIN_WORK *fieldmap;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  ZONEID zoneID;
  u16 wait;
  u8 padding[2];
}DEBUG_SIOEND_WARP;  //DEBUG_SIOEND_MAPCHANGE;

///�f�o�b�O�p
typedef struct{
  FIELD_COMM_MAIN *commSys;
  FIELD_MAIN_WORK *fieldmap;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  u16 wait;
  u8 padding[2];
}DEBUG_SIOEND_CHILD;


//�㍶���E�ɑΉ������O���b�h�ł̃I�t�Z�b�g
static const s8 FCM_dirOfsArr[4][2]={{0,-1},{-1,0},{0,1},{1,0}};

//======================================================================
//  proto
//======================================================================

FIELD_COMM_MAIN* FIELD_COMM_MAIN_InitSystem( HEAPID heapID , HEAPID commHeapID, GAME_COMM_SYS_PTR game_comm );
void FIELD_COMM_MAIN_TermSystem( FIELD_MAIN_WORK *fieldWork, FIELD_COMM_MAIN *commSys );
void  FIELD_COMM_MAIN_UpdateCommSystem( FIELD_MAIN_WORK *fieldWork ,
        GAMESYS_WORK *gameSys , FIELD_PLAYER *pcActor , FIELD_COMM_MAIN *commSys );
static void FIELD_COMM_MAIN_UpdateSelfData( FIELD_MAIN_WORK *fieldWork ,
        GAMESYS_WORK *gameSys , FIELD_PLAYER *pcActor , FIELD_COMM_MAIN *commSys );
static void FIELD_COMM_MAIN_UpdateCharaData( FIELD_MAIN_WORK *fieldWork ,
        GAMESYS_WORK *gameSys , FIELD_COMM_MAIN *commSys );

const BOOL  FIELD_COMM_MAIN_CanTalk( FIELD_COMM_MAIN *commSys );
static  const u8  FIELD_COMM_MAIN_CheckTalkTarget( FIELD_COMM_MAIN *commSys );
void  FIELD_COMM_MAIN_StartTalk( FIELD_COMM_MAIN *commSys );
const BOOL  FIELD_COMM_MAIN_CheckReserveTalk( FIELD_COMM_MAIN *commSys );
//�ڑ��J�n�p���j���[����
//�J�n��
void  FIELD_COMM_MAIN_InitStartCommMenu( FIELD_COMM_MAIN *commSys, FLDMSGBG *fldMsgBG );
void  FIELD_COMM_MAIN_TermStartCommMenu( FIELD_COMM_MAIN *commSys );
//���̎�
void  FIELD_COMM_MAIN_InitStartInvasionMenu( FIELD_COMM_MAIN *commSys, FLDMSGBG *fldMsgBG );
void  FIELD_COMM_MAIN_TermStartInvasionMenu( FIELD_COMM_MAIN *commSys );
const BOOL  FIELD_COMM_MAIN_LoopStartInvasionMenu( GAMESYS_WORK *gsys, FIELD_COMM_MAIN *commSys );

void  FIELD_COMM_MAIN_Disconnect( FIELD_MAIN_WORK *fieldWork , FIELD_COMM_MAIN *commSys );
void FIELD_COMM_MAIN_CommFieldSysFree(COMM_FIELD_SYS_PTR comm_field);

static GMEVENT * DEBUG_EVENT_ChangeMapPosCommEnd(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,
		u16 zone_id, const VecFx32 * pos, FIELD_COMM_MAIN *commSys );
static GMEVENT_RESULT DebugEVENT_MapChangeCommEnd(GMEVENT * event, int *seq, void*work);
static GMEVENT * DEBUG_EVENT_ChildCommEnd(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap, FIELD_COMM_MAIN *commSys);
static GMEVENT_RESULT DebugEVENT_ChildCommEnd(GMEVENT * event, int *seq, void*work);


//--------------------------------------------------------------
//  �t�B�[���h�ʐM�V�X�e��������
//  @param  commHeapID �ʐM�p�ɏ풓����q�[�vID
//      �ʐM���L���ȊԒ��J������Ȃ�Heap���w�肵�Ă�������
//--------------------------------------------------------------
FIELD_COMM_MAIN* FIELD_COMM_MAIN_InitSystem( HEAPID heapID , HEAPID commHeapID, GAME_COMM_SYS_PTR game_comm )
{
  FIELD_COMM_MAIN *commSys;
  commSys = GFL_HEAP_AllocClearMemory( heapID , sizeof(FIELD_COMM_MAIN) );
  commSys->heapID_ = heapID;
  commSys->game_comm = game_comm;
  commSys->palace = PALACE_SYS_Alloc(heapID);
  if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_INVASION){
    commSys->commField_ = GameCommSys_GetAppWork(game_comm);
    PALACE_SYS_SetArea(commSys->palace, FIELD_COMM_SYS_GetInvalidNetID(commSys->commField_));
  }
////  commSys->commField_->commFunc_ = FIELD_COMM_FUNC_InitSystem( commHeapID );

////  FIELD_COMM_DATA_InitSystem( commHeapID );
#if DEB_ARI
  DEBUG_PerformanceSetActive( FALSE );
#endif
  return commSys;
}

//--------------------------------------------------------------
// �t�B�[���h�ʐM�V�X�e���J��
//--------------------------------------------------------------
void FIELD_COMM_MAIN_TermSystem( FIELD_MAIN_WORK *fieldWork, FIELD_COMM_MAIN *commSys )
{
  if(commSys->commField_ != NULL){
    if(GameCommSys_BootCheck(commSys->game_comm) != GAME_COMM_NO_INVASION){
      //�ʐM�Ď����[�N�����݂��Ă���̂ɐؒf����Ă���̂͒ʐM�G���[���������ċ����ؒf���ꂽ�ꍇ
      //���ׁ̈A�t�B�[���h�̔j���Ɠ����ɒʐM�Ǘ����[�N�̔j�����s��
      //�t�B�[���h�̒ʐM�G���[�̎d�l�ɂ���Ă�����ǂ����邩�͌�X���߂�@��check
    #if 0
      GameCommSys_ExitReq(commSys->game_comm);
      if(commSys->palace != NULL){
        PALACE_SYS_Free(commSys->palace);
        commSys->palace = NULL;
      }
    #endif
    }
  }

#if 0
  if( isTermAll == TRUE )
  {
////    FIELD_COMM_DATA_TermSystem();
////    FIELD_COMM_FUNC_TermSystem( commSys->commField_->commFunc_ );
  }
#endif

  if( commSys->actCtrl_ != NULL ){
    FIELD_COMM_ACTOR_CTRL_Delete( commSys->actCtrl_ );
  }

  PALACE_SYS_Free(commSys->palace);
  GFL_HEAP_FreeMemory( commSys );
}

//==================================================================
/**
 * �t�B�[���h�ʐM�Ď����[�N�𐶐�����(�ʐM�J�n���ɐ�������΂悢)
 *
 * @param   commHeapID
 *
 * @retval  COMM_FIELD_SYS_PTR    ���������t�B�[���h�ʐM�Ď����[�N�ւ̃|�C���^
 */
//==================================================================
COMM_FIELD_SYS_PTR FIELD_COMM_MAIN_CommFieldSysAlloc(HEAPID commHeapID, GAME_COMM_SYS_PTR game_comm)
{
  return FIELD_COMM_SYS_Alloc(commHeapID, game_comm);
}

//==================================================================
/**
 * �t�B�[���h�ʐM�Ď����[�N��j��(�ʐM�I�����ɔj��)
 *
 * @param   commSys   �t�B�[���h�ʐM�V�X�e�����[�N�ւ̃|�C���^
 */
//==================================================================
void FIELD_COMM_MAIN_CommFieldSysFree(COMM_FIELD_SYS_PTR comm_field)
{
  FIELD_COMM_SYS_Free(comm_field);
}

//==================================================================
/**
 * �ʐM�p�������܂܁A�}�b�v�؂�ւ��������ꍇ�̏���������
 *
 * @param   comm_field
 */
//==================================================================
void FIELD_COMM_MAIN_CommFieldMapInit(COMM_FIELD_SYS_PTR comm_field)
{
  FIELD_COMM_DATA *commData;

  if(comm_field == NULL){
    return;
  }
  commData = FIELD_COMM_SYS_GetCommDataWork(comm_field);
  FIELD_COMM_DATA_DataReset(commData);
}

//==================================================================
/**
 * �t�B�[���h�ʐM�^�C�v���Z�b�g
 *
 * @param   commSys		
 * @param   comm_type		�ʐM�^�C�v
 */
//==================================================================
void FIELD_COMM_MAIN_SetCommType(FIELD_COMM_MAIN *commSys, FIELD_COMM_TYPE comm_type)
{
  commSys->comm_type = comm_type;
}

//==================================================================
/**
 * �t�B�[���h�ʐM�^�C�v�擾
 *
 * @param   commSys		
 *
 * @retval  FIELD_COMM_TYPE		�ʐM�^�C�v
 */
//==================================================================
FIELD_COMM_TYPE FIELD_COMM_MAIN_GetCommType(FIELD_COMM_MAIN *commSys)
{
  return commSys->comm_type;
}

//==================================================================
/**
 * �t�B�[���h�ʐM�A�N�^�[�ݒ�
 */
//==================================================================
void FIELD_COMM_MAIN_SetCommActor(FIELD_COMM_MAIN *commSys, MMDLSYS *fmmdlsys)
{
  int max = 4;
  commSys->actCtrl_ = FIELD_COMM_ACTOR_CTRL_Create( max, fmmdlsys, commSys->heapID_ );
}




//==================================================================
/**
 *   �f�o�b�O�p �p���X�ɔ�ԃC�x���g���쐬����
 *  @param   fieldWork		
 *  @param   gameSys		
 *  @param   pcActor		
 *  @retval  GMEVENT *		
 */
//==================================================================

GMEVENT * DEBUG_PalaceJamp(FIELD_MAIN_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  VecFx32 pos;

	pos.x = 760 << FX32_SHIFT;
	pos.y = 0;
	pos.z = 234 << FX32_SHIFT;
	return DEBUG_EVENT_ChangeMapPos(gameSys, fieldWork, ZONE_ID_PALACETEST, &pos, 0);
}

//==================================================================
/**
 *   �؂ɏd�Ȃ�����T1�փ��[�v
 *
 * @param   fieldWork		
 * @param   gameSys		
 * @param   pcActor		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * DEBUG_PalaceTreeMapWarp(FIELD_MAIN_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor, FIELD_COMM_MAIN *commSys)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  VecFx32 pos;

  //�f�o�b�O�p��R+Y,B�Ńp���X�AT01�փ��[�v�ł���悤�ɂ���
#if 0
  if(GFL_UI_KEY_GetCont() == (PAD_BUTTON_R | PAD_BUTTON_Y)){
    pos.x = 12040 << FX32_SHIFT;
    pos.y = 0;
    pos.z = 13080 << FX32_SHIFT;
    return DEBUG_EVENT_ChangeMapPos(gameSys, fieldWork, ZONE_ID_T01, &pos, 0);
  }
  else if(GFL_UI_KEY_GetCont() == (PAD_BUTTON_R | PAD_BUTTON_B)){
    pos.x = 760 << FX32_SHIFT;
    pos.y = 0;
    pos.z = 234 << FX32_SHIFT;
    return DEBUG_EVENT_ChangeMapPos(gameSys, fieldWork, ZONE_ID_PALACETEST, &pos, 0);
  }
#endif

  if(commSys != NULL && commSys->commField_ != NULL){
    if(FIELD_COMM_SYS_GetExitReq(commSys->commField_) == TRUE){
      return DEBUG_EVENT_ChildCommEnd(gameSys, fieldWork, commSys);
    }
  }
  
  if(zone_id != ZONE_ID_PALACETEST){
    return NULL;
  }
  
  FIELD_PLAYER_GetPos( pcActor, &pos );
  if(pos.x >= FX32_CONST(744) && pos.x <= FX32_CONST(776) 
      && pos.z >= FX32_CONST(72) && pos.z <= FX32_CONST(88-1)){
    pos.x = 12040 << FX32_SHIFT;
    pos.y = 0;
    pos.z = 13080 << FX32_SHIFT;
    
    if(commSys != NULL 
        && GFL_NET_GetConnectNum() > 1 
        && GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gameSys)) == GAME_COMM_NO_INVASION
        && GFL_NET_IsParentMachine() == TRUE
        && FIELD_COMM_SYS_GetInvalidNetID(FIELD_COMM_MAIN_GetCommFieldSysPtr(commSys)) == GFL_NET_SystemGetCurrentID()){
      //�e���ʐM��ԂŎ����̊X�ɓ���ꍇ�͐ؒf
      return DEBUG_EVENT_ChangeMapPosCommEnd(gameSys, fieldWork, ZONE_ID_T01, &pos, commSys);
    }
    return DEBUG_EVENT_ChangeMapPos(gameSys, fieldWork, ZONE_ID_T01, &pos, 0);
  }
  
  {//���@�̍��W���Ď����A�N�Ƃ��ʐM���Ă��Ȃ��̂Ƀp���X�̋��̈��ʒu�܂ŗ�����
   //���Ӄ��b�Z�[�W���o���Ĉ����Ԃ�����
    VecFx32 pos;
    BOOL left_right;
    
    FIELD_PLAYER_GetPos( pcActor, &pos );
    pos.x >>= FX32_SHIFT;
    if(GFL_NET_GetConnectNum() <= 1 && (pos.x <= 536 || pos.x >= 1000)){
      left_right = pos.x <= 536 ? 0 : 1;
      return EVENT_DebugPalaceNGWin( gameSys, fieldWork, pcActor, left_right );
    }
  }
  
  return NULL;
}


//==================================================================
/**
 * ���f�o�b�O�p�@�ʐM���I�������Ă���}�b�v�ړ�
 *
 * @param   gsys		
 * @param   fieldmap		
 * @param   zone_id		
 * @param   pos		
 * @param   dir		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
static GMEVENT * DEBUG_EVENT_ChangeMapPosCommEnd(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,
		u16 zone_id, const VecFx32 * pos, FIELD_COMM_MAIN *commSys )
{
  DEBUG_SIOEND_WARP *dsw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, DebugEVENT_MapChangeCommEnd, sizeof(DEBUG_SIOEND_WARP));
	dsw = GMEVENT_GetEventWork(event);
	GFL_STD_MemClear(dsw, sizeof(DEBUG_SIOEND_WARP));
	dsw->fieldmap = fieldmap;
	dsw->zoneID = zone_id;
	dsw->pos = *pos;
	dsw->commSys = commSys;
	
	OS_TPrintf("�e�@�F�ʐM�ؒf�C�x���g�N��\n");
	return event;
}

static GMEVENT_RESULT DebugEVENT_MapChangeCommEnd(GMEVENT * event, int *seq, void*work)
{
	DEBUG_SIOEND_WARP * dsw = work;
	GAMESYS_WORK  * gsys = GMEVENT_GetGameSysWork(event);
	FIELD_MAIN_WORK * fieldmap = dsw->fieldmap;
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	
	switch (*seq) {
	case 0:
    GameCommSys_ExitReq(game_comm); //�ʐM�I�����N�G�X�g
	  {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(dsw->fieldmap);
      dsw->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      dsw->msgWin = FLDMSGWIN_AddTalkWin( msgBG, dsw->msgData );
      FLDMSGWIN_Print( dsw->msgWin, 0, 0, msg_invasion_test06_01 );
      GXS_SetMasterBrightness(-16);
      (*seq)++;
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(dsw->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    dsw->wait++;
    if(dsw->wait > 60){
      if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL){
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
        int i;
        PLAYER_WORK *plwork;
        VecFx32 pos = {0,0,0};
        
        for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
          plwork = GAMEDATA_GetPlayerWork( gamedata , i );
          PLAYERWORK_setPosition(plwork, &pos);
        }
        dsw->commSys->commField_ = NULL;
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( dsw->msgWin );
    GFL_MSG_Delete( dsw->msgData );
    GXS_SetMasterBrightness(0);
    (*seq)++;
    break;
	case 4:
  	GMEVENT_CallEvent(
  	  event, DEBUG_EVENT_ChangeMapPos(gsys, dsw->fieldmap, dsw->zoneID, &dsw->pos, 0));
    (*seq)++;
    break;
	default:
		return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���f�o�b�O�p�@�q�@�̒ʐM�I��
 *
 * @param   gsys		
 * @param   fieldmap		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * DEBUG_EVENT_ChildCommEnd(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap, FIELD_COMM_MAIN *commSys)
{
  DEBUG_SIOEND_CHILD *dsc;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, DebugEVENT_ChildCommEnd, sizeof(DEBUG_SIOEND_CHILD));
	dsc = GMEVENT_GetEventWork(event);
	GFL_STD_MemClear(dsc, sizeof(DEBUG_SIOEND_CHILD));
	dsc->fieldmap = fieldmap;
	dsc->commSys = commSys;
	
	OS_TPrintf("�q�@�F�ʐM�ؒf�C�x���g�N��\n");
	return event;
}

static GMEVENT_RESULT DebugEVENT_ChildCommEnd(GMEVENT * event, int *seq, void*work)
{
  DEBUG_SIOEND_CHILD *dsc = work;
	GAMESYS_WORK  * gsys = GMEVENT_GetGameSysWork(event);
	FIELD_MAIN_WORK * fieldmap = dsc->fieldmap;
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	
	switch (*seq) {
	case 0:
    GameCommSys_ExitReq(game_comm); //�ʐM�I�����N�G�X�g
	  {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(dsc->fieldmap);
      dsc->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      dsc->msgWin = FLDMSGWIN_AddTalkWin( msgBG, dsc->msgData );
      FLDMSGWIN_Print( dsc->msgWin, 0, 0, msg_invasion_test06_01 );
      GXS_SetMasterBrightness(-16);
      (*seq)++;
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(dsc->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    dsc->wait++;
    if(dsc->wait > 60){
      if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL){
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
        int i;
        PLAYER_WORK *plwork;
        VecFx32 pos = {0,0,0};
        
        for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
          plwork = GAMEDATA_GetPlayerWork( gamedata , i );
          PLAYERWORK_setPosition(plwork, &pos);
        }
        dsc->commSys->commField_ = NULL;
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( dsc->msgWin );
    GFL_MSG_Delete( dsc->msgData );
    GXS_SetMasterBrightness(0);
    (*seq)++;
    break;
	default:
		return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �f�o�b�O�F�p���X�}�b�v�ɓ����Ă������`�F�b�N���A�����Ă���ΒʐM�N���Ȃǂ�����
 *   ��check�@�����I�ɃX�N���v�g�Ń}�b�v���Ɏ����N���C�x���g���o����悤�ɂȂ����炱��͍폜����
 *
 * @param   fieldWork		
 * @param   gameSys		
 */
//--------------------------------------------------------------
static void DEBUG_PalaceMapInCheck(FIELD_MAIN_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_COMM_MAIN *commSys, FIELD_PLAYER *pcActor)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
//  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  ZONEID zone_id = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(GAMESYSTEM_GetGameData(gameSys)));
  GAME_COMM_NO comm_no;
  
  switch(commSys->debug_palace_comm_seq){
  case 0:
    comm_no = GameCommSys_BootCheck(commSys->game_comm);
    //�q�Ƃ��Đڑ������ꍇ
    if(comm_no == GAME_COMM_NO_INVASION && commSys->commField_ == NULL){
      commSys->debug_palace_comm_seq = 2;
      break;
    }
    
    {
      VecFx32 pos;
      
      FIELD_PLAYER_GetPos( pcActor, &pos );
      pos.x >>= FX32_SHIFT;
      if( !((pos.x >= 512 && pos.x <= 648) || (pos.x >= 888 && pos.x <= 1016)) ){
        //���ł͂Ȃ��ꏊ�ɂ���Ȃ�ʐM�͊J�n���Ȃ�
        return;
      }
    }
    
    //�r�[�R���T�[�`��ԂŃp���X�ɓ����Ă����ꍇ
    if(zone_id == ZONE_ID_PALACETEST && comm_no == GAME_COMM_NO_FIELD_BEACON_SEARCH){
      OS_TPrintf("�r�[�R���T�[�`���I��\n");
      GameCommSys_ExitReq(commSys->game_comm);
      commSys->debug_palace_comm_seq++;
      break;
    }
    
    //�e�Ƃ��ċN�����Ă���ꍇ�̃`�F�b�N
    if(zone_id != ZONE_ID_PALACETEST || GFL_NET_IsExit() == FALSE || comm_no != GAME_COMM_NO_NULL){
      return;
    }
    commSys->debug_palace_comm_seq++;
    break;
  case 1:
    if(GameCommSys_BootCheck(commSys->game_comm) == GAME_COMM_NO_NULL){
      FIELD_INVALID_PARENT_WORK *invalid_parent;
      
      invalid_parent = GFL_HEAP_AllocClearMemory(
          GFL_HEAP_LOWID(GFL_HEAPID_APP), sizeof(FIELD_INVALID_PARENT_WORK));
      invalid_parent->my_invasion = TRUE;
      invalid_parent->game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
      GameCommSys_Boot(commSys->game_comm, GAME_COMM_NO_INVASION, invalid_parent);

      OS_TPrintf("�p���X�ʐM�����N��\n");
      commSys->debug_palace_comm_seq++;
    }
    break;
  case 2:
    if(GameCommSys_CheckSystemWaiting(commSys->game_comm) == FALSE){
      commSys->commField_ = GameCommSys_GetAppWork(commSys->game_comm);
      OS_TPrintf("�p���X�ʐM�����N������\n");
      commSys->debug_palace_comm_seq++;
    }
    break;
  case 3:
    if(zone_id == ZONE_ID_PALACETEST){
//      PALACE_DEBUG_CreateNumberAct(commSys->palace, GFL_HEAP_LOWID(GFL_HEAPID_APP));
      commSys->debug_palace_comm_seq++;
    }
    break;
  case 4:
    if(zone_id != ZONE_ID_PALACETEST){
//      PALACE_DEBUG_DeleteNumberAct(commSys->palace);
      commSys->debug_palace_comm_seq = 3;
    }
    break;
  }
}

//--------------------------------------------------------------
// �t�B�[���h�ʐM�V�X�e���X�V
//  @param  FIELD_MAIN_WORK �t�B�[���h���[�N
//  @param  GAMESYS_WORK  �Q�[���V�X�e�����[�N(PLAYER_WORK�擾�p
//  @param  PC_ACTCONT    �v���C���[�A�N�^�[(�v���C���[���l�擾�p
//  @param  FIELD_COMM_MAIN �ʐM���[�N
//  �����̃L�����̐��l���擾���ĒʐM�p�ɕۑ�
//  ���L�����̏����擾���A�ʐM����ݒ�
//--------------------------------------------------------------
void  FIELD_COMM_MAIN_UpdateCommSystem( FIELD_MAIN_WORK *fieldWork ,
        GAMESYS_WORK *gameSys , FIELD_PLAYER *pcActor , FIELD_COMM_MAIN *commSys )
{
  //�p���X�}�b�v�ɗ������`�F�b�N
  DEBUG_PalaceMapInCheck(fieldWork, gameSys, commSys, pcActor);

  if(GameCommSys_BootCheck(commSys->game_comm) != GAME_COMM_NO_INVASION){
    if(commSys->palace != NULL){ //��������������ۂ����Ǐ��F�f�o�b�O�p�̃A�N�^�[�Ȃ̂�
      PALACE_DEBUG_EnableNumberAct(commSys->palace, FALSE);
    }
    return;
  }

  if(commSys->commField_ != NULL)
  {
    u8 i;
////    FIELD_COMM_FUNC_UpdateSystem( commSys->commField_ );
    PALACE_SYS_Update(commSys->palace, GAMESYSTEM_GetMyPlayerWork( gameSys ), pcActor, commSys->commField_);
    if( FIELD_COMM_FUNC_GetMemberNum() > 1 && FIELD_COMM_SYS_GetExitReq(commSys->commField_) == FALSE )
    //if( FIELD_COMM_FUNC_GetCommMode( commFunc ) == FIELD_COMM_MODE_CONNECT )
    {
      FIELD_COMM_MAIN_UpdateSelfData( fieldWork , gameSys , pcActor , commSys );
      FIELD_COMM_MAIN_UpdateCharaData( fieldWork , gameSys , commSys );
    }
  }

#if DEB_ARI
  if(commSys->commField_ != NULL)
  {
    if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_L )
      FIELD_COMM_MENU_SwitchDebugWindow( FCM_BGPLANE_MSG_WINDOW, FIELDMAP_GetFldMsgBG(fieldWork) );
    FIELD_COMM_MENU_UpdateDebugWindow( commSys );
  }
#endif  //DEB_ARI
}

//--------------------------------------------------------------
// �����̃L�����̍X�V
//--------------------------------------------------------------
static void FIELD_COMM_MAIN_UpdateSelfData( FIELD_MAIN_WORK *fieldWork ,
        GAMESYS_WORK *gameSys , FIELD_PLAYER *pcActor , FIELD_COMM_MAIN *commSys )
{
  ZONEID zoneID;
  VecFx32 pos;
  u16 dir;
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
  FIELD_COMM_FUNC *commFunc = FIELD_COMM_SYS_GetCommFuncWork(commSys->commField_);
  FIELD_COMM_DATA *commData = FIELD_COMM_SYS_GetCommDataWork(commSys->commField_);

  //���L�������W���X�V
  zoneID = PLAYERWORK_getZoneID( plWork );
  FIELD_PLAYER_GetPos( pcActor, &pos );
  dir = FIELD_PLAYER_GetDir( pcActor );
  //dir = FieldMainGrid_GetPlayerDir( fieldWork );
  FIELD_COMM_DATA_SetSelfData_Pos( commData, &zoneID , &pos , &dir );
  if(FIELD_COMM_SYS_GetExitReq(commSys->commField_) == FALSE){
    FIELD_COMM_FUNC_Send_SelfData( 
      commFunc, commData, FIELD_COMM_SYS_GetInvalidNetID(commSys->commField_ ));
  }
}

//--------------------------------------------------------------
// ���̃L�����̍X�V
//--------------------------------------------------------------
static void FIELD_COMM_MAIN_UpdateCharaData( FIELD_MAIN_WORK *fieldWork ,
        GAMESYS_WORK *gameSys , FIELD_COMM_MAIN *commSys )
{
  FIELD_COMM_FUNC *commFunc = FIELD_COMM_SYS_GetCommFuncWork(commSys->commField_);
  FIELD_COMM_DATA *commData = FIELD_COMM_SYS_GetCommDataWork(commSys->commField_);
  u8 i;
  //�͂����f�[�^�̃`�F�b�N
  for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
  {
#if DEB_ARI&0
    if( FIELD_COMM_DATA_GetCharaData_IsValid(commData, i) == TRUE )
#else
    if( i != FIELD_COMM_FUNC_GetSelfIndex(commFunc) &&
      FIELD_COMM_DATA_GetCharaData_IsValid(commData, i) == TRUE )
#endif
    {
      //�L���ȃf�[�^�������Ă���
      switch( FIELD_COMM_DATA_GetCharaData_State( commData, i ) )
      {
      case FCCS_NONE:
      case FCCS_CONNECT:
        //�ڍ׃f�[�^�������̂ŁA�f�[�^�����N�G�X�g
        FIELD_COMM_FUNC_Send_RequestData( i , FCRT_PROFILE , commFunc );
        FIELD_COMM_DATA_SetCharaData_State( commData, i , FCCS_REQ_DATA );
        break;
      case FCCS_REQ_DATA:
        //�f�[�^��M���Ȃ̂ő҂�
        break;
      case FCCS_EXIST_DATA:
        FIELD_COMM_DATA_SetCharaData_State( commData, i , FCCS_FIELD );
        //break through
      case FCCS_FIELD:
        {
          GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
          PLAYER_WORK *setPlWork = GAMEDATA_GetPlayerWork( gameData , i );
          PLAYER_WORK *charaWork = FIELD_COMM_DATA_GetCharaData_PlayerWork(commData, i);
          GFL_STD_MemCopy( (void*)charaWork , (void*)setPlWork , sizeof(PLAYER_WORK) );

          PALACE_SYS_FriendPosConvert(commSys->palace, PLAYERWORK_getPalaceArea(setPlWork), 
            GAMEDATA_GetMyPlayerWork( gameData ), setPlWork);

          if( FIELD_COMM_DATA_GetCharaData_IsExist(commData, i) == FALSE )
          {
            BOOL *vanish_flag = FIELD_COMM_SYS_GetCommActorVanishFlag(commSys->commField_, i);
            *vanish_flag = TRUE;
            //���������ȃL�����Ȃ̂ŁA����������
            FIELD_COMM_ACTOR_CTRL_AddActor( commSys->actCtrl_,
                0, HERO, &setPlWork->direction, &setPlWork->position, vanish_flag );
            FIELD_COMM_DATA_SetCharaData_IsExist(commData, i,TRUE);
            OS_TPrintf("�o�^�o�j�b�V��Flag = %d\n", i);
          }
          FIELD_COMM_DATA_SetCharaData_IsValid(commData, i,FALSE);
        }
      }
    }
  }
}

//--------------------------------------------------------------
// ��b���ł��邩�H(��b�^�[�Q�b�g�����邩�H
// �����ŉ�b�^�[�Q�b�g���m�肵�Ă��܂��A���̂܂ܕێ����Ă���
// ����netID���ŋ߂��ق���D�悷��̂ŁE�E�E
//--------------------------------------------------------------
const BOOL  FIELD_COMM_MAIN_CanTalk( FIELD_COMM_MAIN *commSys )
{
  FIELD_COMM_FUNC *commFunc;
  FIELD_COMM_DATA *commData;

  if(GameCommSys_BootCheck(commSys->game_comm) != GAME_COMM_NO_INVASION
        || commSys->commField_ == NULL || FIELD_COMM_SYS_GetExitReq(commSys->commField_) == TRUE){
    return FALSE;
  }

  commFunc = FIELD_COMM_SYS_GetCommFuncWork(commSys->commField_);
  commData = FIELD_COMM_SYS_GetCommDataWork(commSys->commField_);

  if( FIELD_COMM_FUNC_GetCommMode(commFunc) == FIELD_COMM_MODE_CONNECT)
  {
    if( FIELD_COMM_DATA_GetTalkState( commData, FCD_SELF_INDEX ) == FCTS_NONE )
    {
      commSys->talkTrgChara_ = FIELD_COMM_MAIN_CheckTalkTarget( commSys );
      if( commSys->talkTrgChara_ < FIELD_COMM_MEMBER_MAX )
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}
static  const u8 FIELD_COMM_MAIN_CheckTalkTarget( FIELD_COMM_MAIN *commSys )
{
  u8 i;
  FIELD_COMM_FUNC *commFunc = FIELD_COMM_SYS_GetCommFuncWork(commSys->commField_);
  FIELD_COMM_DATA *commData = FIELD_COMM_SYS_GetCommDataWork(commSys->commField_);
  const PLAYER_WORK *plWork = FIELD_COMM_DATA_GetCharaData_PlayerWork( commData, FCD_SELF_INDEX );
  const u16 selfDir = plWork->direction >> 14;
  int selfX,selfZ;
  int member_num;
  int my_palace_area, target_palace_area;
  PLAYER_WORK *target_plWork;
  ZONEID my_zone_id, target_zone_id;
  
  FIELD_COMM_DATA_GetGridPos_AfterMove( commData, FCD_SELF_INDEX,&selfX,&selfZ );
  selfX += FCM_dirOfsArr[selfDir][0];
  selfZ += FCM_dirOfsArr[selfDir][1];
  member_num = FIELD_COMM_FUNC_GetMemberNum();
  my_palace_area = PALACE_SYS_GetArea(commSys->palace);
  my_zone_id = PLAYERWORK_getZoneID( plWork );
  for( i=0;i<member_num;i++ )
  {
    target_plWork = FIELD_COMM_DATA_GetCharaData_PlayerWork(commData, i);
    target_palace_area = PLAYERWORK_getPalaceArea( target_plWork );
    target_zone_id = PLAYERWORK_getZoneID( target_plWork );
    OS_TPrintf("check my_area = %d, target_area = %d, my_zone=%d, trg_zone=%d\n", my_palace_area, target_palace_area, my_zone_id, target_zone_id);
    if( i != FIELD_COMM_FUNC_GetSelfIndex(commFunc) 
        && (my_palace_area == target_palace_area 
        || (my_zone_id == target_zone_id && my_zone_id == ZONE_ID_PALACETEST)) )
    {
      int px,pz;
      FIELD_COMM_DATA_GetGridPos_AfterMove( commData, i,&px,&pz );
      OS_TPrintf("check px = %d, pz = %d, selfX = %d, selfZ = %d, selfDir=%d\n", px, pz, selfX, selfZ, selfDir);
      if( px == selfX && pz == selfZ )
      {
        return i;
      }
    }
  }
  return 0xFF;
}

//--------------------------------------------------------------
// ��b�J�n����
//--------------------------------------------------------------
const BOOL  FIELD_COMM_MAIN_CheckReserveTalk( FIELD_COMM_MAIN *commSys )
{
  FIELD_COMM_FUNC *commFunc;
  FIELD_COMM_DATA *commData;

  if(GameCommSys_BootCheck(commSys->game_comm) != GAME_COMM_NO_INVASION
      || commSys->commField_ == NULL || FIELD_COMM_SYS_GetExitReq(commSys->commField_) == TRUE){
    return FALSE;
  }

  commFunc = FIELD_COMM_SYS_GetCommFuncWork(commSys->commField_);
  commData = FIELD_COMM_SYS_GetCommDataWork(commSys->commField_);

  if( FIELD_COMM_DATA_GetTalkState( commData, FCD_SELF_INDEX ) == FCTS_RESERVE_TALK )
  {
    int selfX,selfZ;
    u8  postID;
    u16 postX,postZ;
    const BOOL isMove = FIELD_COMM_DATA_GetGridPos_AfterMove( commData, FCD_SELF_INDEX ,&selfX,&selfZ);
    FIELD_COMM_FUNC_GetTalkParterData_ID( &postID , commFunc );
    FIELD_COMM_FUNC_GetTalkParterData_Pos( &postX , &postZ , commFunc );
    if( selfX == postX && selfZ == postZ )
    {
      if( isMove == FALSE )
      {
        return TRUE;
      }
      else
      {
        //��~����܂ő҂���
        return FALSE;
      }
    }
    else
    {
      //���łɒʂ�߂��Ă��܂��Ă��邽�߃L�����Z������
      FIELD_COMM_DATA_SetTalkState( commData, FCD_SELF_INDEX , FCTS_NONE );
      FIELD_COMM_FUNC_Send_CommonFlg( commFunc, FCCF_TALK_UNPOSSIBLE , 0 , 0, postID );
      return FALSE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------
// ��b�J�n����
//--------------------------------------------------------------
void  FIELD_COMM_MAIN_StartTalk( FIELD_COMM_MAIN *commSys )
{
  FIELD_COMM_FUNC *commFunc = FIELD_COMM_SYS_GetCommFuncWork(commSys->commField_);
  FIELD_COMM_DATA *commData = FIELD_COMM_SYS_GetCommDataWork(commSys->commField_);
  const PLAYER_WORK *plWork = FIELD_COMM_DATA_GetCharaData_PlayerWork( commData, FCD_SELF_INDEX );
  const u16 selfDir = plWork->direction >> 14;
  int selfX,selfZ;
  FIELD_COMM_DATA_GetGridPos_AfterMove( commData, FCD_SELF_INDEX,&selfX,&selfZ );
  selfX += FCM_dirOfsArr[selfDir][0];
  selfZ += FCM_dirOfsArr[selfDir][1];
  FIELD_COMM_FUNC_InitCommData_StartTalk( commFunc );
  FIELD_COMM_FUNC_Send_CommonFlg( commFunc, FCCF_TALK_REQUEST , selfX , selfZ, commSys->talkTrgChara_ );
  FIELD_COMM_DATA_SetTalkState( commData, FCD_SELF_INDEX , FCTS_WAIT_TALK );
}
//--------------------------------------------------------------
// ��b(�b���������鑤)�J�n����
//--------------------------------------------------------------
void  FIELD_COMM_MAIN_StartTalkPartner( FIELD_COMM_MAIN *commSys )
{
  FIELD_COMM_FUNC *commFunc = FIELD_COMM_SYS_GetCommFuncWork(commSys->commField_);
  FIELD_COMM_DATA *commData = FIELD_COMM_SYS_GetCommDataWork(commSys->commField_);
  u8  postID;
  FIELD_COMM_FUNC_InitCommData_StartTalk( commFunc );
  FIELD_COMM_FUNC_GetTalkParterData_ID( &postID , commFunc );
  FIELD_COMM_FUNC_Send_CommonFlg( commFunc, FCCF_TALK_ACCEPT , 0 , 0, postID );
  FIELD_COMM_DATA_SetTalkState( commData, FCD_SELF_INDEX , FCTS_REPLY_TALK );
}

//--------------------------------------------------------------
// �ʐM�J�n���j���[������
//--------------------------------------------------------------
void  FIELD_COMM_MAIN_InitStartCommMenu( FIELD_COMM_MAIN *commSys, FLDMSGBG *fldMsgBG )
{
  commSys->commMenu_ = FIELD_COMM_MENU_InitCommMenu( commSys->heapID_, fldMsgBG );
  FIELD_COMM_MENU_InitBG_MsgPlane( commSys->commMenu_ );
  FIELD_COMM_MENU_InitBG_MenuPlane( commSys->commMenu_ );
  FIELD_COMM_MENU_OpenMessageWindow( FCM_BGPLANE_MSG_WINDOW , commSys->commMenu_ );
  FIELD_COMM_MENU_OpenYesNoMenu( FCM_BGPLANE_YESNO_WINDOW , commSys->commMenu_ );
  FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR00 , commSys->commMenu_ );
  commSys->menuSeq_ = 0;
}

//--------------------------------------------------------------
//  �ʐM�J�n���j���[ �J��
//--------------------------------------------------------------
void  FIELD_COMM_MAIN_TermStartCommMenu( FIELD_COMM_MAIN *commSys )
{
  FIELD_COMM_MENU_CloseMessageWindow( commSys->commMenu_ );
  FIELD_COMM_MENU_TermCommMenu( commSys->commMenu_ );
  FIELD_COMM_MENU_TermBG_MsgPlane( commSys->commMenu_ );
  FIELD_COMM_MENU_TermBG_MenuPlane( commSys->commMenu_ );
}

//--------------------------------------------------------------
// �ʐM�J�n���j���[�X�V
//--------------------------------------------------------------
const BOOL  FIELD_COMM_MAIN_LoopStartCommMenu( FIELD_COMM_MAIN *commSys, GAMESYS_WORK *gsys )
{
  FIELD_COMM_MENU_UpdateMessageWindow( commSys->commMenu_ );
  switch( commSys->menuSeq_ )
  {
  case 0:
    {
      const u8 ret = FIELD_COMM_MENU_UpdateYesNoMenu( commSys->commMenu_ );
      if( ret == YNR_YES ){
        FIELD_COMM_MENU_CloseYesNoMenu( commSys->commMenu_ );
        commSys->menuSeq_++;
      }
      else if( ret == YNR_NO ){
        FIELD_COMM_MENU_CloseYesNoMenu( commSys->commMenu_ );
        return (TRUE);
      }
    }
    break;
  case 1:
    commSys->menuSeq_ = 3;
    break;
  case 2:
    if( GFL_NET_IsExit() == TRUE )
    {
      commSys->menuSeq_++;
    }
    break;
  case 3:
    //���������̂Ƃ���������������
////    if( FIELD_COMM_FUNC_IsFinishInitCommSystem( commSys->commField_->commFunc_ ) == FALSE ){
    if(GameCommSys_BootCheck(commSys->game_comm) == GAME_COMM_NO_NULL){
      FIELD_INVALID_PARENT_WORK *invalid_parent;
      
      invalid_parent = GFL_HEAP_AllocClearMemory(
          GFL_HEAP_LOWID(GFL_HEAPID_APP), sizeof(FIELD_INVALID_PARENT_WORK));
      invalid_parent->my_invasion = TRUE;
      invalid_parent->game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
      GameCommSys_Boot(commSys->game_comm, GAME_COMM_NO_INVASION, invalid_parent);
    #if 0
      if(commSys->comm_type == FIELD_COMM_TYPE_PALACE){
        commSys->palace = PALACE_SYS_Alloc(GFL_HEAP_LOWID(GFL_HEAPID_APP));
      }
    #endif
    }
    commSys->menuSeq_++;
    break;
  case 4:
    {
      if(GameCommSys_CheckSystemWaiting(commSys->game_comm) == FALSE){
        commSys->commField_ = GameCommSys_GetAppWork(commSys->game_comm);
        commSys->menuSeq_++;
        return (TRUE);
      }
    }
    break;
  }
  return (FALSE);
}

//--------------------------------------------------------------
// �N���J�n���j���[ ������
//--------------------------------------------------------------
void  FIELD_COMM_MAIN_InitStartInvasionMenu( FIELD_COMM_MAIN *commSys, FLDMSGBG *fldMsgBG )
{
  commSys->commMenu_ = FIELD_COMM_MENU_InitCommMenu( commSys->heapID_, fldMsgBG );
  FIELD_COMM_MENU_InitBG_MsgPlane( commSys->commMenu_ );
  FIELD_COMM_MENU_InitBG_MenuPlane( commSys->commMenu_ );
  FIELD_COMM_MENU_OpenMessageWindow( FCM_BGPLANE_MSG_WINDOW , commSys->commMenu_ );
  FIELD_COMM_MENU_OpenYesNoMenu( FCM_BGPLANE_YESNO_WINDOW , commSys->commMenu_ );
  FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR01 , commSys->commMenu_ );
  commSys->menuSeq_ = 0;
}

//--------------------------------------------------------------
// �N���J�n���j���[�J��
//--------------------------------------------------------------
void  FIELD_COMM_MAIN_TermStartInvasionMenu( FIELD_COMM_MAIN *commSys )
{
  FIELD_COMM_MENU_CloseMessageWindow( commSys->commMenu_ );
  FIELD_COMM_MENU_TermCommMenu( commSys->commMenu_ );
  FIELD_COMM_MENU_TermBG_MsgPlane( commSys->commMenu_ );
  FIELD_COMM_MENU_TermBG_MenuPlane( commSys->commMenu_ );
}

//--------------------------------------------------------------
// �N���J�n���j���[�X�V
//--------------------------------------------------------------
const BOOL  FIELD_COMM_MAIN_LoopStartInvasionMenu( GAMESYS_WORK *gsys, FIELD_COMM_MAIN *commSys )
{
  FIELD_COMM_MENU_UpdateMessageWindow( commSys->commMenu_ );
  switch( commSys->menuSeq_ )
  {
  case 0:
    {
      const u8 ret = FIELD_COMM_MENU_UpdateYesNoMenu( commSys->commMenu_ );
      if( ret == YNR_YES ){
        FIELD_COMM_MENU_CloseYesNoMenu( commSys->commMenu_ );
        commSys->menuSeq_++;
      }
      else if( ret == YNR_NO ){
        FIELD_COMM_MENU_CloseYesNoMenu( commSys->commMenu_ );
        return (TRUE);
      }
    }
    break;
  case 1:
    commSys->menuSeq_ = 3;
    break;
  case 2:
    if( GFL_NET_IsExit() == TRUE )
    {
      commSys->menuSeq_++;
    }
    break;
  case 3:
    //���������̂Ƃ���������������
////    if( FIELD_COMM_FUNC_IsFinishInitCommSystem( commSys->commField_->commFunc_ ) == FALSE ){
    if(GameCommSys_BootCheck(commSys->game_comm) == GAME_COMM_NO_NULL){
      FIELD_INVALID_PARENT_WORK *invalid_parent;
      
      invalid_parent = GFL_HEAP_AllocClearMemory(
          GFL_HEAP_LOWID(GFL_HEAPID_APP), sizeof(FIELD_INVALID_PARENT_WORK));
      invalid_parent->game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
      GameCommSys_Boot(commSys->game_comm, GAME_COMM_NO_INVASION, invalid_parent);
    #if 0
      if(commSys->comm_type == FIELD_COMM_TYPE_PALACE){
        commSys->palace = PALACE_SYS_Alloc(GFL_HEAP_LOWID(GFL_HEAPID_APP));
      }
    #endif
    }
    commSys->menuSeq_++;
    break;
  case 4:
    {
      if(GameCommSys_CheckSystemWaiting(commSys->game_comm) == FALSE){
        commSys->commField_ = GameCommSys_GetAppWork(commSys->game_comm);
        commSys->menuSeq_++;
        return (TRUE);
      }
    }
    break;
  }
  return (FALSE);
}

//==================================================================
/**
 * �ʐM�ؒf���N�G�X�g
 *
 * @param   fieldWork
 * @param   commSys
 *
 * �����̊֐��ďo����A�K��FIELD_COMM_MAIN_DisconnectWait�֐��Őؒf�����̊m�F�҂������鎖
 */
//==================================================================
void  FIELD_COMM_MAIN_Disconnect( FIELD_MAIN_WORK *fieldWork , FIELD_COMM_MAIN *commSys )
{
  //FieldMain_CommActorFreeAll( fieldWork );
////  FIELD_COMM_FUNC_TermCommSystem();
  GameCommSys_ExitReq(commSys->game_comm);
}

//==================================================================
/**
 * �ʐM�ؒf�I���҂�
 *
 * @param   fieldWork
 * @param   commSys
 *
 * @retval  BOOL    TRUE:�ؒf�����B�@FALSE:�ؒf�҂�
 */
//==================================================================
BOOL FIELD_COMM_MAIN_DisconnectWait( FIELD_MAIN_WORK *fieldWork, FIELD_COMM_MAIN *commSys )
{
  if(GameCommSys_BootCheck(commSys->game_comm) != GAME_COMM_NO_INVASION){
    commSys->commField_ = NULL;
    if(commSys->palace != NULL){
        PALACE_SYS_InitWork(commSys->palace);
    }
    return TRUE;
  }
  return FALSE;
}

//======================================================================
//  �ȉ� field_comm_event �p�Bextern��`���Y���\�[�X�ɏ���
//======================================================================
const int FIELD_COMM_MAIN_GetWorkSize(void)
{
  return sizeof(FIELD_COMM_MAIN);
}

COMM_FIELD_SYS_PTR FIELD_COMM_MAIN_GetCommFieldSysPtr( FIELD_COMM_MAIN *commSys )
{
  return commSys->commField_;
}

FIELD_COMM_MENU** FIELD_COMM_MAIN_GetCommMenuWork( FIELD_COMM_MAIN *commSys )
{
  return &commSys->commMenu_;
}
const HEAPID FIELD_COMM_MAIN_GetHeapID( FIELD_COMM_MAIN *commSys )
{
  return commSys->heapID_;
}

