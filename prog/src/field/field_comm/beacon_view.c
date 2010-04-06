//==============================================================================
/**
 * @file    beacon_view.c
 * @brief   ����Ⴂ�ʐM��ԎQ�Ɖ��
 * @author  iwasawa 
 * @date    2009.12.13(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/bmp_oam.h"
#include "gamesystem/gamesystem.h"
#include "field/field_subscreen.h"
#include "gamesystem/game_beacon.h"
#include "gamesystem/msgspeed.h"
#include "infowin/infowin.h"
#include "app_menu_common.naix"
#include "arc_def.h"
#include "font/font.naix"
#include "field/beacon_view.h"
#include "print\printsys.h"
#include "message.naix"
#include "field/event_beacon_detail.h"
#include "field/event_subscreen.h"
#include "field/event_gpower.h"
#include "field/event_freeword_input.h"

#include "beacon_status.naix"
#include "wifi_unionobj.naix"
#include "wmi.naix"
#include "beacon_view_local.h"
#include "beacon_view_sub.h"

//==============================================================================
//  �萔��`
//==============================================================================


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void event_Request( BEACON_VIEW_PTR wk, BEACON_DETAIL_EVENT ev_id);
static void event_RequestReset( BEACON_VIEW_PTR wk );

static int seq_Main( BEACON_VIEW_PTR wk );
static int seq_ViewUpdate( BEACON_VIEW_PTR wk );
static int seq_GPowerUse( BEACON_VIEW_PTR wk );
static int seq_TalkMsgInput( BEACON_VIEW_PTR wk );
static int seq_ThankYou( BEACON_VIEW_PTR wk );
static int seq_ReturnCGear( BEACON_VIEW_PTR wk );
static int seq_CallDetailView( BEACON_VIEW_PTR wk );

static void BEACON_VIEW_TouchUpdata(BEACON_VIEW_PTR view);
static void _sub_DataSetup(BEACON_VIEW_PTR wk);
static void _sub_DataExit(BEACON_VIEW_PTR wk);
static void _sub_SystemSetup(BEACON_VIEW_PTR view);
static void _sub_SystemExit(BEACON_VIEW_PTR view);
static void _sub_BGLoad(BEACON_VIEW_PTR view, ARCHANDLE *handle);
static void _sub_BGUnload(BEACON_VIEW_PTR view);
static void _sub_TMenuInit( BEACON_VIEW_PTR wk );
static void _sub_TMenuRelease( BEACON_VIEW_PTR wk );
static void _sub_ActorResourceLoad(BEACON_VIEW_PTR view, ARCHANDLE *handle);
static void _sub_ActorResourceUnload(BEACON_VIEW_PTR view);
static void _sub_ActorCreate(BEACON_VIEW_PTR view, ARCHANDLE *handle);
static void _sub_ActorDelete(BEACON_VIEW_PTR view);
static void _sub_BmpWinCreate(BEACON_VIEW_PTR view);
static void _sub_BmpWinDelete(BEACON_VIEW_PTR view);

static void obj_ObjResInit( BEACON_VIEW_PTR wk, OBJ_RES_TBL* res, const OBJ_RES_SRC* srcTbl, ARCHANDLE* p_handle );
static void obj_ObjResRelease( BEACON_VIEW_PTR wk, OBJ_RES_TBL* res );
static void act_NormalObjAdd( BEACON_VIEW_PTR wk ) ;
static void act_NormalObjDel( BEACON_VIEW_PTR wk ) ;
static void act_PanelObjAdd( BEACON_VIEW_PTR wk, u8 idx );
static void act_PanelObjDel( BEACON_VIEW_PTR wk, u8 idx );

//==============================================================================
//  �f�[�^
//==============================================================================
///�^�b�`�͈̓e�[�u��
static const GFL_UI_TP_HITTBL TouchRect[] = {
  {//G�p���[
    0x15*8,
    0x18*8,
    0*8,
    6*8,
  },
  {//���߂łƂ�
    0x15*8,
    0x18*8,
    6*8,
    12*8,
  },
  {//�߂�
    0x15*8,
    0x18*8,
    (0x20-3)*8,
    0x20*8,
  },
  { GFL_UI_TP_HIT_END, 0, 0, 0 },
};


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ����Ⴂ�Q�Ɖ�ʁF�����ݒ�
 *
 * @param   gsys		
 *
 * @retval  BEACON_VIEW_PTR		
 */
//==================================================================
BEACON_VIEW_PTR BEACON_VIEW_Init(GAMESYS_WORK *gsys, FIELD_SUBSCREEN_WORK *subscreen)
{
  BEACON_VIEW* wk;
  
  wk = GFL_HEAP_AllocClearMemory(HEAPID_FIELD_SUBSCREEN, sizeof(BEACON_VIEW));
//  wk = GFL_HEAP_AllocClearMemory(HEAPID_FIELDMAP, sizeof(BEACON_VIEW));
  wk->gsys = gsys;
  wk->gdata = GAMESYSTEM_GetGameData(gsys);
  wk->fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  wk->subscreen = subscreen;
 
  wk->heap_sID = HEAPID_FIELD_SUBSCREEN;
  wk->heapID = HEAPID_FIELDMAP;
  wk->tmpHeapID = GFL_HEAP_LOWID( HEAPID_FIELD_SUBSCREEN );

  wk->arc_handle = GFL_ARC_OpenDataHandle(ARCID_BEACON_STATUS, wk->heap_sID);
  wk->active = 0xFF;  //�����l�ŏ�����

  _sub_DataSetup(wk);
  _sub_SystemSetup(wk);
  _sub_BGLoad(wk, wk->arc_handle);
  _sub_TMenuInit( wk );
  _sub_ActorResourceLoad(wk, wk->arc_handle);
  _sub_ActorCreate(wk, wk->arc_handle);
  _sub_BmpWinCreate(wk);
  
 
  //�����`��
  BeaconView_InitialDraw( wk );

  return wk;
}

//==================================================================
/**
 * ����Ⴂ�Q�Ɖ�ʁF�j���ݒ�
 *
 * @param   view		
 */
//==================================================================
void BEACON_VIEW_Exit(BEACON_VIEW_PTR wk)
{
  //�X�g���[���������Ă�����폜
  if( wk->printStream != NULL){
    PRINTSYS_PrintStreamDelete( wk->printStream );
    wk->printStream = NULL;
  }
  //�������Ă���S�Ẵ^�X�N���폜
  GFL_TCBL_DeleteAll( wk->pTcbSys );

  _sub_BmpWinDelete( wk );
  _sub_ActorDelete( wk );
  _sub_ActorResourceUnload( wk );
  _sub_TMenuRelease( wk );
  _sub_BGUnload( wk );
  _sub_SystemExit( wk );
  _sub_DataExit(wk);
  
  GFL_ARC_CloseDataHandle(wk->arc_handle);

  GFL_HEAP_FreeMemory( wk );
}

//==================================================================
/**
 * ����Ⴂ�Q�Ɖ�ʁF�X�V
 *
 * @param   view		
 */
//==================================================================
void BEACON_VIEW_Update(BEACON_VIEW_PTR wk, BOOL bActive )
{
  int i;

  GFL_TCBL_Main( wk->pTcbSys );
  
  //�X�^�b�N�e�[�u���X�V
  GAMEBEACON_Stack_Update( wk->infoStack );

  if( wk->active != bActive ){
    wk->active = bActive;
    BeaconView_SetViewPassive( wk, !bActive );
  }
  if( wk->event_id != EV_NONE ){
    if(!bActive){
      //���N�G�X�g�������j��
      event_RequestReset( wk );
    }
    return; //�C�x���g���N�G�X�g���̓��C���������X�L�b�v
  }
  if(!bActive){
    return;
  }
  
  switch( wk->seq ){
  case SEQ_MAIN:
    wk->seq = seq_Main( wk );
    break;
  case SEQ_VIEW_UPDATE:
    wk->seq = seq_ViewUpdate( wk );
    break;
  case SEQ_GPOWER_USE:
    wk->seq = seq_GPowerUse( wk );
    break;
  case SEQ_TALKMSG_INPUT:
    wk->seq = seq_TalkMsgInput( wk );
    break;
  case SEQ_THANK_YOU:
    wk->seq = seq_ThankYou( wk );
    break;
  case SEQ_RETURN_CGEAR:
    wk->seq = seq_ReturnCGear( wk );
    break;
  case SEQ_CALL_DETAIL_VIEW:
    wk->seq = seq_CallDetailView( wk );
    break;
  case SEQ_END:
  default:
    //�O�����N�G�X�g�ɂ��I���҂�
    break;
  }
}

//==================================================================
/**
 * ����Ⴂ�Q�Ɖ�ʁF���͎擾
 *
 * @param   wk		
 */
//==================================================================
GMEVENT* BEACON_VIEW_EventCheck(BEACON_VIEW_PTR wk, BOOL bEvReqOK )
{
  GMEVENT* event = NULL;

  if( !bEvReqOK ){  //�C�x���g�N�����Ă����^�C�~���O��҂�
    return NULL;
  }

  switch( wk->event_id ){
  case EV_RETURN_CGEAR:
    BEACON_STATUS_SetViewTopOffset( wk->b_status, 0 );
    event = EVENT_ChangeSubScreen( wk->gsys, wk->fieldWork, FIELD_SUBSCREEN_NORMAL);
//    FIELD_SUBSCREEN_SetAction( wk->subscreen , FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_CGEAR );
//
    break;
  case EV_CALL_DETAIL_VIEW:
    event = EVENT_BeaconDetail( wk->gsys, wk->fieldWork, wk->ctrl.target+wk->ctrl.view_top );
    break;
  case EV_GPOWER_USE:
    event = EVENT_GPowerEffectStart( wk->gsys, wk->ctrl.g_power, wk->ctrl.mine_power_f );
    wk->ctrl.g_power = GPOWER_ID_NULL;
    break;
  case EV_CALL_TALKMSG_INPUT:
    event = EVENT_FreeWordInput( wk->gsys, wk->fieldWork, NULL, NAMEIN_FREE_WORD, NULL );
    break;
  default:
    return NULL;
  }
  event_RequestReset( wk );
  return event; 
}

//==================================================================
/**
 * ����Ⴂ�Q�Ɖ�ʁF�`��
 *
 * @param   wk		
 */
//==================================================================
void BEACON_VIEW_Draw(BEACON_VIEW_PTR wk)
{
  BEACON_VIEW_TouchUpdata( wk );
}

/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
static void tcb_VInter( GFL_TCB* tcb, void * work)
{
  BEACON_VIEW_PTR wk = (BEACON_VIEW_PTR)work;

  if( wk->pfd != NULL){
    PaletteFadeTrans( wk->pfd );
  }
}

/*
 *  @brief  �C�x���g���N�G�X�g
 */
static void event_Request( BEACON_VIEW_PTR wk, BEACON_DETAIL_EVENT ev_id)
{
  wk->event_id = ev_id;
}

/*
 *  @brief  �C�x���g���N�G�X�g���Z�b�g
 */
static void event_RequestReset( BEACON_VIEW_PTR wk )
{
  wk->event_id = EV_NONE;
}

/*
 *  @brief  ���C���@�ҋ@
 */
static int seq_Main( BEACON_VIEW_PTR wk )
{
  int ret;
  

  //���C�����̓`�F�b�N
  ret = BeaconView_CheckInput( wk );
  if( ret != SEQ_MAIN ){
    return ret;
  }

  if( wk->io_interval ){
    wk->io_interval--;
    return SEQ_MAIN;
  }
  
  //�X�^�b�N�`�F�b�N
  if( BeaconView_CheckStack( wk ) == FALSE){
    return SEQ_MAIN;
  }
  wk->io_interval = 30*5; //�C���^�[�o����ݒ�
  return SEQ_VIEW_UPDATE;
}

/*
 *  @brief  �p�l���X�V��
 */
static int seq_ViewUpdate( BEACON_VIEW_PTR wk )
{
  if( wk->eff_task_ct ){
    return SEQ_VIEW_UPDATE;
  }
  if( wk->ctrl.g_power != GPOWER_ID_NULL){
    event_Request( wk, EV_GPOWER_USE );
  }
  BeaconView_MenuBarViewSet( wk, MENU_ALL, MENU_ST_ON );
  return SEQ_MAIN;
}

/*
 *  @brief  ������G�p���[���g�p
 */
static int seq_GPowerUse( BEACON_VIEW_PTR wk )
{
  if( !BeaconView_SubSeqGPower( wk )){
    return SEQ_GPOWER_USE;
  }
  if( wk->ctrl.g_power != GPOWER_ID_NULL){
    event_Request( wk, EV_GPOWER_USE );
  }
  return SEQ_MAIN;
}

/*
 *  @brief  �ꌾ���b�Z�[�W����́����M
 */
static int seq_TalkMsgInput( BEACON_VIEW_PTR wk )
{
  switch( wk->sub_seq ){
  case 0:
    BeaconView_MenuBarViewSet( wk, MENU_HELLO, MENU_ST_ANM );
    wk->sub_seq++;
    break;
  case 1:
    if(BeaconView_MenuBarCheckAnm( wk, MENU_HELLO )){
      break;
    }
    wk->sub_seq = 0;
    event_Request( wk, EV_CALL_TALKMSG_INPUT );
    return SEQ_END;
  }
  return SEQ_TALKMSG_INPUT;
}

/*
 *  @brief  �������鑊���I��
 */
static int seq_ThankYou( BEACON_VIEW_PTR wk )
{
  if( BeaconView_SubSeqThanks( wk )){
    wk->sub_seq = 0;
    return SEQ_MAIN;
  }
  return SEQ_THANK_YOU;
}

/*
 *  @brief  CGear�ɖ߂�
 */
static int seq_ReturnCGear( BEACON_VIEW_PTR wk )
{
  switch( wk->sub_seq ){
  case 0:
    BeaconView_MenuBarViewSet( wk, MENU_RETURN, MENU_ST_ANM );
    wk->sub_seq++;
    break;
  case 1:
    if( BeaconView_MenuBarCheckAnm( wk, MENU_RETURN )){
      break;
    }
    event_Request( wk , EV_RETURN_CGEAR );
    wk->sub_seq = 0;
    return SEQ_END;
  }
  return SEQ_RETURN_CGEAR;
}

/*
 *  @brief  �ڍ׉�ʌĂяo��
 */
static int seq_CallDetailView( BEACON_VIEW_PTR wk )
{
  if( wk->eff_task_ct ){
    return SEQ_CALL_DETAIL_VIEW;
  }
  event_Request( wk , EV_CALL_DETAIL_VIEW );
  return SEQ_END;
}


//--------------------------------------------------------------
/**
 * �^�b�`����
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void BEACON_VIEW_TouchUpdata(BEACON_VIEW_PTR wk)
{
  u32 tp_x, tp_y;
  
  int tp_ret = GFL_UI_TP_HitTrg(TouchRect);
  
  switch(tp_ret){
  case 0: //G�p���[
    OS_TPrintf("G�p���[ �r�[�R���Z�b�g\n");
    GAMEBEACON_Set_GPower( 1 );

//    GAMEBEACON_Set_EncountDown();
    break;
  case 1: //���߂łƂ�
    OS_TPrintf("���߂łƂ� �r�[�R���Z�b�g\n");
    GAMEBEACON_Set_Thankyou( wk->gdata, 0x12345678 );

//    GAMEBEACON_Set_Congratulations();
    break;
  case 2: //�߂�
//    FIELD_SUBSCREEN_SetAction( wk->subscreen , FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_CGEAR);
    break;
  }
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * �f�[�^�֘A�̃Z�b�g�A�b�v
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void _sub_DataSetup(BEACON_VIEW_PTR wk)
{
  SAVE_CONTROL_WORK* save;

  wk->b_status = GAMEDATA_GetBeaconStatus( wk->gdata );
  wk->infoLog = BEACON_STATUS_GetInfoTbl( wk->b_status );

  wk->ctrl.max = GAMEBEACON_InfoTblRing_GetEntryNum( wk->infoLog );
  wk->ctrl.view_top = BEACON_STATUS_GetViewTopOffset( wk->b_status );

  if( wk->ctrl.max < PANEL_VIEW_MAX ){
    wk->ctrl.view_max = wk->ctrl.max;
  }else{
    wk->ctrl.view_max = PANEL_VIEW_MAX;
  }
  wk->ctrl.g_power = GPOWER_ID_NULL;

  //���b�Z�[�W�X�s�[�h�擾
  wk->msg_spd  = MSGSPEED_GetWait();
 
  //�Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( wk->gdata ); 
  {
    INTRUDE_SAVE_WORK* int_sv = SaveData_GetIntrude(save);
    wk->my_data.power = ISC_SAVE_GetGPowerID(int_sv);
#ifdef DEBUG_ONLY_FOR_iwasawa
    wk->my_data.power = 1;
#endif
    wk->my_power_f = (wk->my_data.power == GPOWER_ID_NULL);
  }
  wk->item_sv = GAMEDATA_GetMyItem( wk->gdata );
  wk->misc_sv = (MISC*)SaveData_GetMiscConst( save );
  wk->log_count = MISC_CrossComm_GetSuretigaiCount( wk->misc_sv );

  {
    MYSTATUS* my = SaveData_GetMyStatus(save);
    wk->my_data.tr_id = MyStatus_GetID( my ) & 0xFFFF;  //����2byte
    wk->my_data.sex = MyStatus_GetMySex( my );
    wk->my_data.union_char = MyStatus_GetTrainerView( my );
  }
  
  //�X�^�b�N���[�N�̈�擾
  wk->infoStack = GAMEBEACON_InfoTbl_Alloc( wk->heap_sID );
  wk->tmpInfo = GAMEBEACON_Alloc( wk->heap_sID );

  //G�p���[�f�[�^�擾
  wk->gpower_data = GPOWER_PowerData_LoadAlloc( wk->heap_sID );
}

//--------------------------------------------------------------
/**
 * �f�[�^�֘A�̔j�����I������
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void _sub_DataExit(BEACON_VIEW_PTR wk)
{
  GAMEBEACON_Stack_PutBack( wk->infoStack );

  GPOWER_PowerData_Unload( wk->gpower_data );

  GFL_HEAP_FreeMemory( wk->tmpInfo );
  GFL_HEAP_FreeMemory( wk->infoStack );
}

//--------------------------------------------------------------
/**
 * �V�X�e���֘A�̃Z�b�g�A�b�v
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void _sub_SystemSetup(BEACON_VIEW_PTR wk)
{
  wk->pTcbSys = GFL_TCBL_Init( wk->heap_sID, wk->heap_sID, 16, 128 );

  //V�^�X�N�ǉ�
  wk->tcbVIntr = GFUser_VIntr_CreateTCB( tcb_VInter, (void*)wk, 0);

  wk->pfd = PaletteFadeInit( wk->heap_sID );
  PaletteFadeWorkAllocSet( wk->pfd, FADE_SUB_BG,  0x20*BG_PALANM_AREA, wk->heap_sID );
  PaletteFadeWorkAllocSet( wk->pfd, FADE_SUB_OBJ, 0x200, wk->heap_sID );

	wk->printQue = PRINTSYS_QUE_Create( wk->heap_sID );
  wk->fontHandle = FLDMSGBG_GetFontHandle( FIELDMAP_GetFldMsgBG(wk->fieldWork));

  GFL_FONTSYS_SetDefaultColor();
 
  wk->wordset = WORDSET_Create( wk->heap_sID);

	wk->mm_status = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_beacon_status_dat, wk->heap_sID );

  wk->str_tmp = GFL_STR_CreateBuffer( BUFLEN_TMP_MSG, wk->heap_sID );
  wk->str_expand = GFL_STR_CreateBuffer( BUFLEN_TMP_MSG, wk->heap_sID );
  wk->str_popup = GFL_STR_CreateBuffer( BUFLEN_TMP_MSG, wk->heap_sID );
}

//--------------------------------------------------------------
/**
 * �V�X�e���֘A�̔j��
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void _sub_SystemExit(BEACON_VIEW_PTR wk)
{

  GFL_STR_DeleteBuffer(wk->str_popup);
  GFL_STR_DeleteBuffer(wk->str_expand);
  GFL_STR_DeleteBuffer(wk->str_tmp);

  GFL_MSG_Delete(wk->mm_status);
	WORDSET_Delete(wk->wordset);
  
  GFL_FONTSYS_SetDefaultColor();
//  GFL_FONT_Delete(wk->fontHandle);
  PRINTSYS_QUE_Delete(wk->printQue);
 
  PaletteFadeWorkAllocFree( wk->pfd, FADE_SUB_OBJ );
  PaletteFadeWorkAllocFree( wk->pfd, FADE_SUB_BG );
  PaletteFadeFree( wk->pfd );
  wk->pfd = NULL;

  GFL_TCB_DeleteTask( wk->tcbVIntr );
 
  GFL_TCBL_Exit( wk->pTcbSys );
}

//--------------------------------------------------------------
/**
 * BG���[�h
 *
 * @param   view		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _sub_BGLoad( BEACON_VIEW_PTR wk, ARCHANDLE *handle )
{
	static const GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
  	{//GFL_BG_FRAME1_S
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, 0x6000,GX_BG_EXTPLTT_01,
  		0, 0, 0, FALSE
  	},
  	{//GFL_BG_FRAME1_S
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, 0x6000,GX_BG_EXTPLTT_01,
  		1, 0, 0, FALSE
  	},
  	{//GFL_BG_FRAME2_S
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000, 0x6000,GX_BG_EXTPLTT_01,
  		2, 0, 0, FALSE
  	},
  	{//GFL_BG_FRAME3_S
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000, 0x6000,GX_BG_EXTPLTT_01,
  		3, 0, 0, FALSE
  	},
	};
	GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &TextBgCntDat[3], GFL_BG_MODE_TEXT );

	//BG VRAM�N���A
	GFL_STD_MemClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);

	//���W�X�^OFF
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2S_BlendNone();
  
  //�p���b�g�]��
/*
  GFL_ARCHDL_UTIL_TransVramPalette(handle, NARC_beacon_status_bstatus_bg_nclr, 
    PALTYPE_SUB_BG, 0, 0x20 * FONT_PAL, wk->tmpHeapID);
  //�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
    0x20*FONT_PAL, 0x20, wk->tmpHeapID);
*/  
  PaletteWorkSet_ArcHandle( wk->pfd, handle, NARC_beacon_status_bstatus_bg_nclr,
      wk->tmpHeapID, FADE_SUB_BG, 0x20*FONT_PAL, 0 );
  PaletteWorkSet_Arc( wk->pfd, ARCID_FONT, NARC_font_default_nclr,
      wk->tmpHeapID, FADE_SUB_BG, 0x20, FONT_PAL*16 );
  
  //�L�����]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    handle, NARC_beacon_status_bstatus_bg_lz_ncgr, GFL_BG_FRAME3_S, 0, 
    0, TRUE, wk->tmpHeapID);
  
  //�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreen(
    handle, NARC_beacon_status_bstatus_bg01_lz_nscr, GFL_BG_FRAME1_S, 0, 
    0x800, TRUE, wk->tmpHeapID);
  GFL_ARCHDL_UTIL_TransVramScreen(
    handle, NARC_beacon_status_bstatus_bg02_lz_nscr, GFL_BG_FRAME2_S, 0, 
    0x800, TRUE, wk->tmpHeapID);
  GFL_ARCHDL_UTIL_TransVramScreen(
    handle, NARC_beacon_status_bstatus_bg03_lz_nscr, GFL_BG_FRAME3_S, 0, 
    0x800, TRUE, wk->tmpHeapID);

  G2S_SetBlendAlpha( ALPHA_1ST_NORMAL, ALPHA_2ND, ALPHA_EV1_NORMAL, ALPHA_EV2_NORMAL);

	GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
	GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
	GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );
	GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_ON );
}

//--------------------------------------------------------------
/**
 * BG�A�����[�h
 *
 * @param   view		
 */
//--------------------------------------------------------------
static void _sub_BGUnload(BEACON_VIEW_PTR wk )
{
	GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );

  G2S_BlendNone();

	GFL_BG_FreeBGControl(GFL_BG_FRAME0_S);
	GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
	GFL_BG_FreeBGControl(GFL_BG_FRAME2_S);
	GFL_BG_FreeBGControl(GFL_BG_FRAME3_S);
}

//--------------------------------------------------------------
/**
 * ���j���[������
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void _sub_TMenuInit( BEACON_VIEW_PTR wk )
{
  int i;

  wk->key_cursor = APP_KEYCURSOR_Create( FCOL_POPUP_BASE, FALSE, TRUE, wk->heap_sID );

  wk->menuRes = APP_TASKMENU_RES_Create( FRM_MENUMSG, BG_PAL_TASKMENU, wk->fontHandle, wk->printQue, wk->heap_sID );

  //�A�C�e��������
  for( i = 0;i < TMENU_MAX;i++){
    wk->tmenu[i].item.str      = GFL_MSG_CreateString( wk->mm_status, msg_sys_tmenu_yes+i ); //���j���[�ɕ\�����镶����
    wk->tmenu[i].item.msgColor = APP_TASKMENU_ITEM_MSGCOLOR;   //�����F�B�f�t�H���g�ł悢�Ȃ��APP_TASKMENU_ITEM_MSGCOLOR
    wk->tmenu[i].item.type     = APP_TASKMENU_WIN_TYPE_NORMAL; //�߂�}�[�N�̕\��
    wk->tmenu[i].work = NULL;
  }
}

//--------------------------------------------------------------
/**
 * ���j���[���
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void _sub_TMenuRelease( BEACON_VIEW_PTR wk )
{
  int i;

  for( i = 0;i < TMENU_MAX; i++){
    if( wk->tmenu[i].work != NULL ){
      APP_TASKMENU_WIN_Delete( wk->tmenu[i].work );
    }
    GFL_STR_DeleteBuffer( wk->tmenu[i].item.str );
  }
  APP_TASKMENU_RES_Delete( wk->menuRes );

  APP_KEYCURSOR_Delete( wk->key_cursor );
}


//--------------------------------------------------------------
/**
 * �A�N�^�[�Ŏg�p���郊�\�[�X�̃��[�h
 *
 * @param   view		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _sub_ActorResourceLoad( BEACON_VIEW_PTR wk, ARCHANDLE *handle)
{
  int i;
  ARCHANDLE* handle_union = GFL_ARC_OpenDataHandle( ARCID_WIFIUNIONCHAR, wk->tmpHeapID );

  {
    const OBJ_RES_SRC srcNormal[OBJ_RES_MAX] = {
      { 1, NARC_beacon_status_bstatus_obj_nclr },
      { 1, NARC_beacon_status_bstatus_obj_ncgr },
      { 1, NARC_beacon_status_bstatus_obj_ncer },
    };
    obj_ObjResInit( wk, &wk->objResNormal, srcNormal, handle );
  }
  {
    const OBJ_RES_SRC srcIcon[OBJ_RES_MAX] = {
      { 0, 0 },
      { PANEL_MAX, NARC_beacon_status_bstatus_icon01_ncgr },
      { 1, NARC_beacon_status_bstatus_icon01_ncer },
    };
    obj_ObjResInit( wk, &wk->objResIcon, srcIcon, handle );
  }
  
  {
    const OBJ_RES_SRC srcUnion[OBJ_RES_MAX] = {
     { 0, 0 },
     { PANEL_MAX, NARC_wifi_unionobj_front00_NCGR },
     { 1,NARC_wifi_unionobj_front00_NCER },
    };
    obj_ObjResInit( wk, &wk->objResUnion, srcUnion, handle_union );
  }

  //�ē]���p�̃��\�[�X�����[�h���Ă���
  PaletteWorkSet_ArcHandle( wk->pfd, handle, NARC_beacon_status_bstatus_obj_nclr,
      wk->tmpHeapID, FADE_SUB_OBJ, 0x20*ACT_PAL_WMI, 0 );
  PaletteWorkSet_Arc( wk->pfd, ARCID_WMI, NARC_wmi_wm_NCLR,
      wk->tmpHeapID, FADE_SUB_OBJ, 0x20, ACT_PAL_WMI*16 );
  wk->resPlttPanel.buf = GFL_ARC_LoadDataAllocByHandle( handle,
                          NARC_beacon_status_bstatus_panel_nclr,
                          wk->heap_sID );
  NNS_G2dGetUnpackedPaletteData( wk->resPlttPanel.buf, &wk->resPlttPanel.p_pltt );
  wk->resPlttPanel.dat = (u16*)wk->resPlttPanel.p_pltt->pRawData;

  wk->resPlttUnion.buf = GFL_ARC_LoadDataAllocByHandle( handle_union,
                          NARC_wifi_unionobj_wifi_union_obj_NCLR,
                          wk->heap_sID );
  NNS_G2dGetUnpackedPaletteData( wk->resPlttUnion.buf, &wk->resPlttUnion.p_pltt );
  wk->resPlttUnion.dat = (u16*)wk->resPlttUnion.p_pltt->pRawData;
  
  for(i = 0;i < UNION_CHAR_MAX;i++){
    wk->resCharUnion[i].buf = GFL_ARC_LoadDataAllocByHandle( handle_union,
                              NARC_wifi_unionobj_front00_NCGR+i,
                              wk->heap_sID );
    NNS_G2dGetUnpackedCharacterData( wk->resCharUnion[i].buf, &wk->resCharUnion[i].p_char );
  }
  for(i = 0;i < BEACON_ICON_MAX;i++){
    wk->resCharIcon[i].buf = GFL_ARC_LoadDataAllocByHandle( handle,
                          NARC_beacon_status_bstatus_icon01_ncgr+i,
                          wk->heap_sID );
    NNS_G2dGetUnpackedCharacterData( wk->resCharIcon[i].buf, &wk->resCharIcon[i].p_char );
  }
  DC_FlushAll();

  GFL_ARC_CloseDataHandle( handle_union );
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�Ŏg�p����A�N�^�[�̃A�����[�h
 *
 * @param   view		
 */
//--------------------------------------------------------------
static void _sub_ActorResourceUnload( BEACON_VIEW_PTR wk )
{
  int i;
  for( i = 0;i < BEACON_ICON_MAX;i++){
    GFL_HEAP_FreeMemory( wk->resCharIcon[i].buf );
  }
  for( i = 0;i < UNION_CHAR_MAX;i++){
    GFL_HEAP_FreeMemory( wk->resCharUnion[i].buf );
  }
  GFL_HEAP_FreeMemory( wk->resPlttPanel.buf );
  GFL_HEAP_FreeMemory( wk->resPlttUnion.buf );

  obj_ObjResRelease( wk, &wk->objResUnion );
  obj_ObjResRelease( wk, &wk->objResIcon );
  obj_ObjResRelease( wk, &wk->objResNormal );
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�쐬
 *
 * @param   wk		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _sub_ActorCreate( BEACON_VIEW_PTR wk, ARCHANDLE *handle )
{
  int i;

  //�Z���n�V�X�e���̍쐬
  wk->cellUnit = GFL_CLACT_UNIT_Create( BEACON_VIEW_OBJ_MAX , 0 , wk->heapID );
 
  //�����_���[�쐬
  {
    const GFL_REND_SURFACE_INIT renderInitData = { 0,512,256,192,CLSYS_DRAW_SUB, CLSYS_REND_CULLING_TYPE_NORMAL};
    
    wk->cellRender = GFL_CLACT_USERREND_Create( &renderInitData , 1 , wk->heap_sID );
    GFL_CLACT_UNIT_SetUserRend( wk->cellUnit, wk->cellRender );
  }

  //�T�u�T�[�t�F�X�̈ʒu��ݒ�
//  GFL_CLACT_USERREND_GetSurfacePos( , ACT_RENDER_ID, &wk->cellSurfacePos );

  //FontOAM�V�X�eAdd��
  wk->bmpOam = BmpOam_Init( wk->heap_sID, wk->cellUnit );
  
  act_NormalObjAdd( wk );

  for(i = 0; i < PANEL_MAX;i++){
    act_PanelObjAdd( wk, i );
  }

  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�폜
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void _sub_ActorDelete( BEACON_VIEW_PTR wk )
{
  int i;

  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_OFF);
 
  for(i = 0; i < PANEL_MAX;i++){
    act_PanelObjDel( wk, i );
  }
  act_NormalObjDel( wk );

  BmpOam_Exit( wk->bmpOam );

  GFL_CLACT_USERREND_Delete( wk->cellRender );
  GFL_CLACT_UNIT_Delete( wk->cellUnit );
}

/*
 *  @brief  BmpWin�ʒǉ�
 */
static void bmpwin_Add( BMP_WIN* win, u8 frm, u8 pal, u8 px, u8 py, u8 sx, u8 sy )
{
  win->win = GFL_BMPWIN_Create( frm, px, py, sx, sy, pal, GFL_BMP_CHRAREA_GET_B );
  win->bmp = GFL_BMPWIN_GetBmp( win->win );
  PRINT_UTIL_Setup( &win->putil, win->win );
  GFL_BMPWIN_MakeTransWindow( win->win );
}

//--------------------------------------------------------------
/**
 * BMPWIN�쐬
 *
 * @param   view		
 */
//--------------------------------------------------------------
static void _sub_BmpWinCreate(BEACON_VIEW_PTR wk)
{
	BMPOAM_ACT_DATA	finit;
  
  bmpwin_Add( &wk->win[WIN_POPUP],
      BMP_POPUP_FRM, BMP_POPUP_PAL, BMP_POPUP_PX, BMP_POPUP_PY,BMP_POPUP_SX, BMP_POPUP_SY );
  bmpwin_Add( &wk->win[WIN_MENU],
      BMP_MENU_FRM, BMP_MENU_PAL, BMP_MENU_PX, BMP_MENU_PY,BMP_MENU_SX, BMP_MENU_SY );

  //�l���\��
 	wk->foamLogNum.bmp = GFL_BMP_Create( BMP_LOGNUM_OAM_SX, BMP_LOGNUM_OAM_SY, GFL_BMP_16_COLOR, wk->heap_sID );

	finit.bmp = wk->foamLogNum.bmp;
	finit.x = BMP_LOGNUM_OAM_PX;
	finit.y = BMP_LOGNUM_OAM_PY;
	finit.pltt_index = wk->objResNormal.res[OBJ_RES_PLTT].tbl[0];
	finit.pal_offset = ACT_PAL_FONT;		// pltt_index�̃p���b�g���ł̃I�t�Z�b�g
	finit.soft_pri = BMP_LOGNUM_OAM_SPRI;			// �\�t�g�v���C�I���e�B
	finit.bg_pri = BMP_LOGNUM_OAM_BGPRI;				// BG�v���C�I���e�B
	finit.setSerface = ACT_RENDER_ID;
	finit.draw_type  = CLSYS_DRAW_SUB;

	wk->foamLogNum.oam = BmpOam_ActorAdd( wk->bmpOam, &finit );
  GFL_BMP_Clear( wk->foamLogNum.bmp, 0 );
	BmpOam_ActorBmpTrans( wk->foamLogNum.oam );
}

//--------------------------------------------------------------
/**
 * BMPWIN�폜
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void _sub_BmpWinDelete(BEACON_VIEW_PTR wk)
{
  int i;
	
  BmpOam_ActorSetDrawEnable( wk->foamLogNum.oam, FALSE );
  BmpOam_ActorDel( wk->foamLogNum.oam );
	GFL_BMP_Delete( wk->foamLogNum.bmp );
  
  for(i = 0; i < WIN_MAX; i++){
    GFL_BMPWIN_Delete( wk->win[i].win );
  }
}


//--------------------------------------------------------------
/**
 * �A�N�^�[���\�[�X�擾
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void obj_ObjResInit( BEACON_VIEW_PTR wk, OBJ_RES_TBL* res, const OBJ_RES_SRC* srcTbl, ARCHANDLE* p_handle )
{
  int i;

  MI_CpuClear8( res, sizeof(OBJ_RES_TBL));

  for(i = 0;i < OBJ_RES_MAX;i++){
    res->res[i].num = srcTbl[i].num;
    if(res->res[i].num != 0){
      res->res[i].tbl = GFL_HEAP_AllocClearMemory( wk->heapID, sizeof(u32)* res->res[i].num );
    }
  }

  for(i = 0;i < res->res[OBJ_RES_PLTT].num;i++){
    IWASAWA_Printf( "pal res idx = %d, src = %d\n",i,srcTbl[OBJ_RES_PLTT].srcID+i);
    res->res[OBJ_RES_PLTT].tbl[i] = 
      GFL_CLGRP_PLTT_Register(  p_handle, srcTbl[OBJ_RES_PLTT].srcID+i,
                                CLSYS_DRAW_SUB, 0, wk->heapID );
  }
  for(i = 0;i < res->res[OBJ_RES_CGR].num;i++){
    IWASAWA_Printf( "char res idx = %d, src = %d\n",i,srcTbl[OBJ_RES_CGR].srcID+i);
    res->res[OBJ_RES_CGR].tbl[i] = 
      GFL_CLGRP_CGR_Register( p_handle, srcTbl[OBJ_RES_CGR].srcID+i,
                              FALSE, CLSYS_DRAW_SUB, wk->heapID );
  }
  for(i = 0;i < res->res[OBJ_RES_CELLANIM].num;i++){
    IWASAWA_Printf( "cell res idx = %d, src = %d\n",i,srcTbl[OBJ_RES_CELLANIM].srcID+(i*2));
    res->res[OBJ_RES_CELLANIM].tbl[i] = 
      GFL_CLGRP_CELLANIM_Register( p_handle, 
          srcTbl[OBJ_RES_CELLANIM].srcID+(i*2),
          srcTbl[OBJ_RES_CELLANIM].srcID+1+(i*2),
          wk->heapID );
  }
}

//--------------------------------------------------------------
/**
 * �A�N�^�[���\�[�X���
 *
 * @param   wk		
 */
//--------------------------------------------------------------
static void obj_ObjResRelease( BEACON_VIEW_PTR wk, OBJ_RES_TBL* res )
{
  int i;

  for(i = 0;i < res->res[OBJ_RES_CELLANIM].num;i++){
    GFL_CLGRP_CELLANIM_Release( res->res[OBJ_RES_CELLANIM].tbl[i] );
  }
  for(i = 0;i < res->res[OBJ_RES_CGR].num;i++){
    GFL_CLGRP_CGR_Release( res->res[OBJ_RES_CGR].tbl[i] );
  }
  for(i = 0;i < res->res[OBJ_RES_PLTT].num;i++){
    GFL_CLGRP_PLTT_Release( res->res[OBJ_RES_PLTT].tbl[i] );
  }
  for(i = 0;i < OBJ_RES_MAX;i++){
    if(res->res[i].tbl != NULL){
      GFL_HEAP_FreeMemory( res->res[i].tbl );
    }
  }
}

/*
 *  @brief  �A�N�^�[�ǉ�
 */
static GFL_CLWK* obj_ObjAdd(
  BEACON_VIEW_PTR wk, u32 cgrNo, u32 palNo, u32 cellNo,
  s16 x, s16 y, u8 anm, u8 bg_pri, u8 spri)
{
  GFL_CLWK_DATA ini;
  GFL_CLWK* obj;

  //�Z���̐���
  ini.pos_x = x;
  ini.pos_y = y;
  ini.anmseq = anm;
  ini.bgpri = bg_pri;
  ini.softpri = spri;
  
  obj = GFL_CLACT_WK_Create( wk->cellUnit,
          cgrNo,palNo,cellNo,
          &ini, ACT_RENDER_ID, wk->heapID );
  GFL_CLACT_WK_SetAutoAnmFlag( obj, TRUE );

  return obj;
}

/*
 *  @brief  �m�[�}���I�u�W�F����
 */
static void act_NormalObjAdd( BEACON_VIEW_PTR wk ) 
{
  int i;
  static const u16 pos[ACT_MAX][2] = {
   { ACT_MENU_PX+ACT_MENU_OX*0 ,ACT_MENU_PY },
   { ACT_MENU_PX+ACT_MENU_OX*1 ,ACT_MENU_PY },
   { ACT_MENU_PX+ACT_MENU_OX*2 ,ACT_MENU_PY },
   { ACT_MENU_PX+ACT_MENU_OX*3 ,ACT_MENU_PY },
   { ACT_UP_PX ,ACT_UP_PY },
   { ACT_DOWN_PX ,ACT_DOWN_PY },
  };

  for(i = 0;i < ACT_MAX;i++){
    wk->pAct[ACT_POWER+i] = obj_ObjAdd( wk,
      wk->objResNormal.res[OBJ_RES_CGR].tbl[0],
      wk->objResNormal.res[OBJ_RES_PLTT].tbl[0],
      wk->objResNormal.res[OBJ_RES_CELLANIM].tbl[0],
      pos[i][0], pos[i][1],
      ACTANM_POWER_ON+(ACT_ANM_SET*i), OBJ_MENU_BG_PRI, OBJ_SPRI_MENU);
  
    GFL_CLACT_WK_SetDrawEnable( wk->pAct[ACT_POWER+i], TRUE );
  }
}

/*
 *  @brief  �m�[�}���I�u�W�F�j��
 */
static void act_NormalObjDel( BEACON_VIEW_PTR wk )
{
  int i;

  for(i = 0;i < ACT_MAX;i++){
    GFL_CLACT_WK_SetDrawEnable( wk->pAct[ACT_POWER+i], FALSE );
    GFL_CLACT_WK_Remove( wk->pAct[ACT_POWER+i]);
  }
}


/**
 *  @brief  �p�l������
 */
static void act_PanelObjAdd( BEACON_VIEW_PTR wk, u8 idx )
{
	BMPOAM_ACT_DATA	finit;
  PANEL_WORK* pp = &(wk->panel[idx]);

  MI_CpuClear8( pp, sizeof(PANEL_WORK));

  pp->id = idx;
  pp->data_idx = PANEL_DATA_BLANK;
  pp->n_line = 0;

  pp->px = ACT_PANEL_OX*idx;
  pp->py = ACT_PANEL_OY*idx;

  //�p�l���A�N�^�[
  pp->cPanel = obj_ObjAdd( wk,
    wk->objResNormal.res[OBJ_RES_CGR].tbl[0],
    wk->objResNormal.res[OBJ_RES_PLTT].tbl[0],
    wk->objResNormal.res[OBJ_RES_CELLANIM].tbl[0],
    pp->px, pp->py, ACTANM_PANEL, OBJ_BG_PRI, OBJ_SPRI_PANEL+idx);
  GFL_CLACT_WK_SetPlttOffs( pp->cPanel, idx, CLWK_PLTTOFFS_MODE_OAM_COLOR );
  
  //�����N�A�N�^�[
  pp->cRank = obj_ObjAdd( wk,
    wk->objResNormal.res[OBJ_RES_CGR].tbl[0],
    wk->objResNormal.res[OBJ_RES_PLTT].tbl[0],
    wk->objResNormal.res[OBJ_RES_CELLANIM].tbl[0],
    pp->px+ACT_RANK_OX, pp->py+ACT_RANK_OY, ACTANM_RANK, OBJ_BG_PRI, OBJ_SPRI_RANK+idx);
  GFL_CLACT_WK_SetAutoAnmFlag( pp->cRank, FALSE );

  //Union�I�u�W�F�A�N�^�[
  pp->cUnion = obj_ObjAdd( wk,
    wk->objResUnion.res[OBJ_RES_CGR].tbl[idx],
    wk->objResNormal.res[OBJ_RES_PLTT].tbl[0],
    wk->objResUnion.res[OBJ_RES_CELLANIM].tbl[0],
    pp->px+ACT_UNION_OX, pp->py+ACT_UNION_OY, 0, OBJ_BG_PRI, OBJ_SPRI_UNION+idx);
  GFL_CLACT_WK_SetPlttOffs( pp->cUnion, idx + ACT_PAL_UNION , CLWK_PLTTOFFS_MODE_PLTT_TOP );

  //Icon�I�u�W�F�A�N�^�[
  pp->cIcon = obj_ObjAdd( wk,
    wk->objResIcon.res[OBJ_RES_CGR].tbl[idx],
    wk->objResNormal.res[OBJ_RES_PLTT].tbl[0],
    wk->objResIcon.res[OBJ_RES_CELLANIM].tbl[0],
    pp->px+ACT_ICON_OX, pp->py+ACT_ICON_OY, 0, OBJ_BG_PRI, OBJ_SPRI_ICON+idx);

	pp->msgOam.bmp = GFL_BMP_Create( BMP_PANEL_OAM_SX, BMP_PANEL_OAM_SY, GFL_BMP_16_COLOR, wk->heap_sID );

	finit.bmp = pp->msgOam.bmp;
	finit.x = pp->px + ACT_MSG_OX;
	finit.y = pp->py + ACT_MSG_OY;
	finit.pltt_index = wk->objResNormal.res[OBJ_RES_PLTT].tbl[0];
	finit.pal_offset = ACT_PAL_FONT;		// pltt_index�̃p���b�g���ł̃I�t�Z�b�g
	finit.soft_pri = OBJ_SPRI_MSG;			// �\�t�g�v���C�I���e�B
	finit.bg_pri = OBJ_BG_PRI;				// BG�v���C�I���e�B
	finit.setSerface = ACT_RENDER_ID;
	finit.draw_type  = CLSYS_DRAW_SUB;

	pp->msgOam.oam = BmpOam_ActorAdd( wk->bmpOam, &finit );

  pp->str = GFL_STR_CreateBuffer( BUFLEN_PANEL_MSG, wk->heap_sID );
  pp->name = GFL_STR_CreateBuffer( BUFLEN_TR_NAME, wk->heap_sID );
  GFL_BMP_Clear( pp->msgOam.bmp, 0 );
	BmpOam_ActorBmpTrans( pp->msgOam.oam );

	BmpOam_ActorSetDrawEnable( pp->msgOam.oam, FALSE );
  GFL_CLACT_WK_SetDrawEnable( pp->cPanel, FALSE );
  GFL_CLACT_WK_SetDrawEnable( pp->cRank, FALSE );
  GFL_CLACT_WK_SetDrawEnable( pp->cUnion, FALSE );
  GFL_CLACT_WK_SetDrawEnable( pp->cIcon, FALSE );
}

/**
 *  @brief  �p�l���폜
 */
static void act_PanelObjDel( BEACON_VIEW_PTR wk, u8 idx )
{
  PANEL_WORK* pp = &(wk->panel[idx]);

  GFL_STR_DeleteBuffer( pp->name );
  GFL_STR_DeleteBuffer( pp->str );

	BmpOam_ActorSetDrawEnable( pp->msgOam.oam, FALSE );
  BmpOam_ActorDel( pp->msgOam.oam );
	GFL_BMP_Delete( pp->msgOam.bmp );

  GFL_CLACT_WK_SetDrawEnable( pp->cIcon, FALSE );
  GFL_CLACT_WK_Remove( pp->cIcon );
  GFL_CLACT_WK_SetDrawEnable( pp->cUnion, FALSE );
  GFL_CLACT_WK_Remove( pp->cUnion );
  GFL_CLACT_WK_SetDrawEnable( pp->cRank, FALSE );
  GFL_CLACT_WK_Remove( pp->cRank );
  GFL_CLACT_WK_SetDrawEnable( pp->cPanel, FALSE );
  GFL_CLACT_WK_Remove( pp->cPanel );
 
  MI_CpuClear8( pp, sizeof(PANEL_WORK));
}

