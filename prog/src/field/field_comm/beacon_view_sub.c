//==============================================================================
/**
 * @file    beacon_view_sub.c
 * @brief   ����Ⴂ�ʐM��ԎQ�Ɖ��
 * @author  iwasawa
 * @date    10.01.21
 */
//==============================================================================

#include <gflib.h>
#include "system/main.h"
#include "system/bmp_oam.h"
#include "gamesystem/gamesystem.h"
#include "field/field_subscreen.h"
#include "gamesystem/game_beacon.h"
#include "infowin/infowin.h"
#include "app_menu_common.naix"
#include "arc_def.h"
#include "font/font.naix"
#include "field/beacon_view.h"
#include "print\printsys.h"
#include "message.naix"

#include "field/research_team_def.h"
#include "beacon_status.naix"
#include "wifi_unionobj.naix"
#include "beacon_view_local.h"
#include "beacon_view_sub.h"

#include "arc/wifi_unionobj_plt.cdat"

typedef struct _POINT{
  int x,y;
}POINT;

typedef struct _TRIANGLE{
  POINT p[3];
}TRIANGLE;

///����G�p���[�z��
static const DATA_SpGPowerTbl[ SP_GPOWER_REQ_MAX ] = {
  GPOWER_ID_EGG_INC_S,
  GPOWER_ID_NATSUKI_S,
  GPOWER_ID_SALE_S,
  GPOWER_ID_EXP_INC_S,
  GPOWER_ID_MONEY_S,
  GPOWER_ID_CAPTURE_S,
};

//////////////////////////////////////////////////////////////////
static void sp_gpower_ConditionCheck( BEACON_VIEW_PTR wk, GAMEBEACON_INFO* info, u8 old_log_max );
static void sp_gpower_RequestSet( BEACON_VIEW_PTR wk, u8 req_id );
static GPOWER_ID sp_gpower_RequestGet( BEACON_VIEW_PTR wk );
static BOOL sp_gpower_ZoromeCheck( u32 log_count );

static int calc_Cross( POINT* a, POINT* b ) ;
static void calc_PointMinus( POINT* a, POINT* b, POINT* out );
static BOOL calc_PointInTriangle( POINT* p, TRIANGLE* tri );

static int touchin_CheckPanel( BEACON_VIEW_PTR wk, POINT* tp );
static int touchin_CheckUpDown( BEACON_VIEW_PTR wk, POINT* tp );
static int touchin_CheckMenu( BEACON_VIEW_PTR wk, POINT* tp );

static void draw_LogNumWindow( BEACON_VIEW_PTR wk );
static void draw_MenuWindow( BEACON_VIEW_PTR wk, u8 msg_id );

static u32 sub_GetBeaconMsgID(const GAMEBEACON_INFO *info);
static void sub_PlaySE( u16 se_no );

static void act_SetPosition( GFL_CLWK* act, s16 x, s16 y );
static void act_AnmStart( GFL_CLWK* act, u8 anm_no );
static void obj_MenuIconVisibleSet( BEACON_VIEW_PTR wk, BOOL visible_f );
static void obj_UpDownViewSet( BEACON_VIEW_PTR wk );
static void obj_ThanksViewSet( BEACON_VIEW_PTR wk );

static void panel_VisibleSet( PANEL_WORK* pp, BOOL visible_f );
static void panel_IconVisibleSet( PANEL_WORK* pp, BOOL visible_f );
static void panel_Clear( PANEL_WORK* pp );
static void panel_Entry( PANEL_WORK* pp, u8 data_idx,u8 line );
static PANEL_WORK* panel_GetPanelFromDataIndex( BEACON_VIEW_PTR wk, u8 data_idx );
static void panel_UnionObjUpdate( BEACON_VIEW_PTR wk, PANEL_WORK* pp, GAMEBEACON_INFO* info);
static void panel_IconObjUpdate( BEACON_VIEW_PTR wk, PANEL_WORK* pp, u8 icon);
static u8 panel_FrameColorGet( GAMEBEACON_INFO* info );
static void panel_ColorPlttSet( BEACON_VIEW_PTR wk, PANEL_WORK* pp, GAMEBEACON_INFO* info );
static void panel_MsgPrint( BEACON_VIEW_PTR wk, PANEL_WORK* pp, STRBUF* str, int* task_ct );
static void panel_NamePrint( BEACON_VIEW_PTR wk, PANEL_WORK* pp, BOOL force_f, int* task_ct );
static void panel_Draw( BEACON_VIEW_PTR wk,
    PANEL_WORK* pp, GAMEBEACON_INFO* info, PANEL_DRAW_TYPE draw_type, BOOL icon_cancel_f, int* task_ct );

static void list_TimeOutCheck( BEACON_VIEW_PTR wk );
static void list_UpDownReq( BEACON_VIEW_PTR wk, SCROLL_DIR dir );
static void list_UpDownReq1st( BEACON_VIEW_PTR wk, SCROLL_DIR dir, int ret_seq );
static void list_ScrollReq( BEACON_VIEW_PTR wk, GAMEBEACON_INFO* info, u8 idx, u8 dir, PANEL_DRAW_TYPE draw_type );

static void taskAdd_IconEffect( BEACON_VIEW_PTR wk, PANEL_WORK* pp, GAMEBEACON_INFO* info, BOOL new_f );

static void effReq_PanelScroll( BEACON_VIEW_PTR wk, u8 dir, PANEL_WORK* new_pp, PANEL_WORK* ignore_pp, int* task_ct );
static void effReq_PanelSlideIn( BEACON_VIEW_PTR wk, PANEL_WORK* pp, int* task_ct );
static BOOL effReq_PopupMsg( BEACON_VIEW_PTR wk, GAMEBEACON_INFO* info, BOOL new_f );
static void effReq_PopupMsgFromInfo( BEACON_VIEW_PTR wk, GAMEBEACON_INFO* info );
static void effReq_PopupMsgSys( BEACON_VIEW_PTR wk, u8 msg_id );
static BOOL effReq_PopupMsgGPower( BEACON_VIEW_PTR wk, GAMEBEACON_INFO* info );
static void effReq_PopupMsgGPowerMine( BEACON_VIEW_PTR wk );
static void effReq_PopupMsgGPowerSpecial( BEACON_VIEW_PTR wk, GPOWER_ID sp_gpower_id );
static void effReq_SetPanelFlash( BEACON_VIEW_PTR wk, u8 target_ofs );

static void printReq_BmpwinPrint( BEACON_VIEW_PTR wk, BMP_WIN* win, STRBUF* str, u8 x, u8 y, u8 clear, PRINTSYS_LSB color, int* task_ct );
static void printReq_FOamPrint( BEACON_VIEW_PTR wk, PANEL_WORK* pp, STRBUF* str, PRINTSYS_LSB color, int* task_ct );

//////////////////////////////////////////////////////////////////

/*
 *  @brief  �����`��
 */
void BeaconView_InitialDraw( BEACON_VIEW_PTR wk )
{
  int i;

  PaletteTrans_AutoSet( wk->pfd, TRUE );

  //���݂̃��O��
  draw_LogNumWindow( wk );
  //���݂̃g�[�^������Ⴂ�l��
  draw_MenuWindow( wk, msg_sys_now_record );

  //�ŐV���X�g��`��
  for(i = 0;i < wk->ctrl.view_max;i++){
    int idx, ofs;
    PANEL_WORK* pp;

    ofs = wk->ctrl.view_top+i;
    GAMEBEACON_InfoTblRing_GetBeacon( wk->infoLog, wk->tmpInfo, &wk->tmpTime, ofs );
    idx = GAMEBEACON_InfoTblRing_Ofs2Idx( wk->infoLog, ofs );
  
    pp = panel_GetPanelFromDataIndex( wk, PANEL_DATA_BLANK );
    panel_Entry( pp, idx, PANEL_VIEW_START+i );  //�p�l���V�K�o�^
    panel_Draw( wk, pp, wk->tmpInfo, PANEL_DRAW_INI, FALSE, NULL );   //�p�l���`��
    panel_VisibleSet( pp, TRUE );   //�p�l���`��
  }
  //���ƌ�烁�j���[�̌����ڃZ�b�g
  obj_UpDownViewSet( wk );
  obj_ThanksViewSet( wk );
  if( wk->my_data.power == GPOWER_ID_NULL ){
    act_AnmStart( wk->pAct[ACT_POWER], ACTANM_POWER_OFF );
  }
  GFL_NET_ChangeIconPosition(GFL_WICON_POSX,GFL_WICON_POSY);
  GFL_NET_WirelessIconEasy_HoldLCD( FALSE, wk->tmpHeapID );
  GFL_NET_ReloadIcon();
}

/*
 *  @brief  ���C�����[�v���ł̓��̓`�F�b�N
 */
int BeaconView_CheckInput( BEACON_VIEW_PTR wk )
{
  int ret;
  POINT tp;
  BOOL my_power;

#ifdef PM_DEBUG
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
    BEACON_VIEW_SUB_EventReserve( wk, EV_DEBUG_MENU_CALL );
    return SEQ_MAIN;
  }
#endif

  if( !GFL_UI_TP_GetPointTrg( (u32*)&tp.x, (u32*)&tp.y )){
    return SEQ_MAIN;
  }
  
  //���j���[�����蔻��
  ret = touchin_CheckMenu( wk, &tp );
  if(ret != GFL_UI_TP_HIT_NONE){
    switch( ret ){
    case MENU_POWER:
      return SEQ_GPOWER_USE;
    case MENU_HELLO:
      BEACON_VIEW_SUB_EventReserve( wk, EV_CALL_TALKMSG_INPUT );
      break;
    case MENU_THANKS:
      return SEQ_THANK_YOU;
    case MENU_RETURN:
      BEACON_VIEW_SUB_EventReserve( wk, EV_RETURN_CGEAR );
      break;
    }
    return SEQ_MAIN;
//    return SEQ_GPOWER_USE+ret;
  }

  //�p�l�������蔻��
  ret = touchin_CheckPanel( wk, &tp );
  if(ret != GFL_UI_TP_HIT_NONE){
    wk->ctrl.target = ret;
    effReq_SetPanelFlash( wk, ret );
    BEACON_VIEW_SUB_EventReserve( wk, EV_GPOWER_CHECK );
    return SEQ_MAIN;
//    return SEQ_CALL_DETAIL_VIEW;
  }

  //�㉺��󂠂��蔻��
  ret = touchin_CheckUpDown( wk, &tp );
  if(ret != GFL_UI_TP_HIT_NONE){
    list_UpDownReq1st( wk, ret, SEQ_MAIN );
    return SEQ_LIST_SCROLL_REPEAT;
  }
  return SEQ_MAIN;
}

/*
 *  @brief  ���X�g�㉺�{�^���@�^�b�`����
 */
int BeaconView_CheckInoutTouchUpDown( BEACON_VIEW_PTR wk )
{
  POINT tp;
  
  if( !GFL_UI_TP_GetPointCont( (u32*)&tp.x, (u32*)&tp.y )){
    return GFL_UI_TP_HIT_NONE;
  }
  return touchin_CheckUpDown( wk, &tp );
}

/*
 *  @brief  ����|�b�v�A�b�v�N���`�F�b�N 
 */
BOOL BeaconView_CheckSpecialPopup( BEACON_VIEW_PTR wk )
{
  u16* ret_fword_input =  BEACON_STATUS_GetFreeWordInputResultPointer( wk->b_status );

  //�ꌾ���b�Z�[�W���͂����������`�F�b�N
  if( (*ret_fword_input) == FALSE ){
    return FALSE;
  }
  *ret_fword_input = FALSE; //�t���O���Z�b�g

  WORDSET_RegisterWord( wk->wordset, 0,
    BEACON_STATUS_GetFreeWordBuffer( wk->b_status ), wk->my_data.sex, TRUE, PM_LANG );
  effReq_PopupMsgSys( wk, msg_sys_free_word_send );
  return TRUE;
}

/*
 *  @brief  ����G�p���[�|�b�v�A�b�v�N���`�F�b�N 
 */
BOOL BeaconView_CheckSpecialGPower( BEACON_VIEW_PTR wk )
{
  GPOWER_ID power;

  //����G�p���[���N�G�X�g�`�F�b�N
  power = sp_gpower_RequestGet( wk );

  if( power == GPOWER_ID_NULL ){
    return FALSE;
  }
  if( GPOWER_Check_OccurID( power, wk->gpower_data ) == power ){
    return FALSE; //�����p���[�������͎̂Ă�
  }
  //�|�b�v�A�b�v�����N�G�X�g
  effReq_PopupMsgGPowerSpecial( wk, power );
 
  return TRUE;
}

/*
 *  @brief  �X�^�b�N���烍�O���ꌏ���o��
 */
BOOL BeaconView_CheckStack( BEACON_VIEW_PTR wk )
{
  BOOL new_f = FALSE, popup_f = FALSE;
  int ofs,idx,old_max;
  PANEL_WORK* pp;

  //�X�^�b�N�ɐς܂ꂽ�����`�F�b�N
#ifdef PM_DEBUG
  if( wk->deb_stack_check_throw ){
    list_TimeOutCheck( wk );
    return FALSE;
  }
#endif
  if( !GAMEBEACON_Stack_GetInfo( wk->infoStack, wk->newLogInfo, &wk->newLogTime ) ){
    list_TimeOutCheck( wk );
    return FALSE;
  }

  //�G���[�`�F�b�N
  if( GAMEBEACON_Check_Error( wk->newLogInfo)){
    return FALSE;
  }

  //�V�������O������
  wk->newLogOfs = GAMEBEACON_InfoTblRing_SetBeacon( wk->infoLog,
                    wk->newLogInfo, wk->newLogTime, &wk->first_entry_f );
  wk->newLogIdx = GAMEBEACON_InfoTblRing_Ofs2Idx( wk->infoLog, wk->newLogOfs );

  if( !wk->first_entry_f ){
    //�r�[�R���̃g���[�i�[�p�l�����\�������`�F�b�N
    wk->newLogPanel = panel_GetPanelFromDataIndex( wk, wk->newLogIdx );
  }else{
    wk->newLogPanel = NULL;
  }

  //�p�l������\�����ABitmap�̏������ݒ��łȂ����sub_seq��i�߂Ă���
  if( wk->newLogPanel != NULL &&
      wk->newLogPanel->tcb_print != NULL ){
    wk->sub_seq = 0;
  }else{
    wk->sub_seq = 1;
  }

  //�o�^���X�V
  wk->old_list_max = wk->ctrl.max;
  if( wk->ctrl.max < BS_LOG_MAX ){
    wk->ctrl.max += wk->first_entry_f;
  
    if( wk->ctrl.max <= PANEL_VIEW_MAX){
      wk->ctrl.view_max = wk->ctrl.max;
    }
  }
  return TRUE;
}


//---------------------------------------------------
/*
 *  @brief  �A�N�e�B�u�E�p�b�V�u�ؑ�
 */
//---------------------------------------------------
void BeaconView_SetViewPassive( BEACON_VIEW_PTR wk, BOOL passive_f )
{
  if(passive_f){
    SoftFadePfd( wk->pfd, FADE_SUB_BG, 0, 16*BG_PALANM_AREA, 6, 0x0000);
    SoftFadePfd( wk->pfd, FADE_SUB_OBJ, 0, 16*ACT_PAL_WMI, 6, 0x0000);
    G2S_SetBlendAlpha( ALPHA_1ST_PASSIVE, ALPHA_2ND, ALPHA_EV1_PASSIVE, ALPHA_EV2_PASSIVE);

//    GFL_FONTSYS_SetDefaultColor();
  }else{
    SoftFadePfd( wk->pfd, FADE_SUB_BG, 0, 16*BG_PALANM_AREA, 0, 0x0000);
    SoftFadePfd( wk->pfd, FADE_SUB_OBJ, 0, 16*ACT_PAL_WMI, 0, 0x0000);
    G2S_SetBlendAlpha( ALPHA_1ST_NORMAL, ALPHA_2ND, ALPHA_EV1_NORMAL, ALPHA_EV2_NORMAL);
  }
}

//---------------------------------------------------
/*
 *  @brief  ���X�g�X�N���[�����N�G�X�g
 */
//---------------------------------------------------
void BeaconView_ListScrollRepeatReq( BEACON_VIEW_PTR wk )
{
  list_UpDownReq( wk, wk->scr_repeat_dir );
}

//---------------------------------------------------
/*
 *  @brief  �v�����g�L���[�@�����X�s�[�h�A�b�v���N�G�X�g
 *
 *  ���������������炵�A�������ׂ��グ�����ɁA
 *  �v�����g�����I���܂ł̃X�s�[�h���A�b�v����
 */
//---------------------------------------------------
void BeaconView_PrintQueLimmitUpSet( BEACON_VIEW_PTR wk, BOOL flag )
{
  if( flag ){ //�X�s�[�h�A�b�v
    PRINTSYS_QUE_SetLimitTick( wk->printQue, PRINT_QUE_LIMMIT_UP );
  }else{  //�f�t�H���g�ɖ߂�
    PRINTSYS_QUE_SetLimitTick( wk->printQue, PRINT_QUE_LIMMIT_DEFAULT );
  }
}

////////////////////////////////////////////////////////////////////////////
//==========================================================================
//�T�u�V�[�P���X
//==========================================================================
////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/*
 *  @brief  �T�u�V�[�P���X�@���O�G���g���[�@
 */
///////////////////////////////////////////////////////////////////

/*
 *  @brief  �T�u�V�[�P���X�@New���O�G���g���[���C��
 */
BOOL BeaconView_SubSeqLogEntry( BEACON_VIEW_PTR wk )
{
  switch( wk->sub_seq ){
  case 0:
    //�Ȃɂ��v�����g���Ȃ�҂�
    if( wk->newLogPanel->tcb_print != NULL ){
      break;
    }
    wk->sub_seq++;
    break;
  case 1:
    {
      BOOL popup_f;

      //�|�b�v�A�b�v���N�G�X�g
      popup_f = effReq_PopupMsg( wk, wk->newLogInfo, wk->first_entry_f );

      //�V�K�łȂ����̓X�N���[������
      if( !wk->first_entry_f ){
        if(popup_f){
          BeaconView_MenuBarViewSet( wk, MENU_ALL, MENU_ST_OFF );
          sub_PlaySE( BVIEW_SE_ICON );
        }
        if(wk->newLogPanel != NULL){
          //�p�l���������Ă��鎞�̂݃^�[�Q�b�g�p�l��������
          panel_Draw( wk, wk->newLogPanel, wk->newLogInfo, PANEL_DRAW_UPDATE, !popup_f, NULL );
          panel_VisibleSet( wk->newLogPanel, TRUE );
        }
        return TRUE;
      }
    }
    BeaconView_MenuBarViewSet( wk, MENU_ALL, MENU_ST_OFF );
    wk->sub_seq++;
    break;
  case 2:
    //�V�K
    wk->log_count = MISC_CrossComm_IncSuretigaiCount( wk->misc_sv );    

    draw_LogNumWindow( wk );
    draw_MenuWindow( wk, msg_sys_now_record );
//    obj_ThanksViewSet( wk );
  
    //����G�p���[�`�F�b�N
    sp_gpower_ConditionCheck( wk, wk->newLogInfo, wk->old_list_max );
    wk->sub_seq++;
    break;
  case 3:
    if( wk->ctrl.view_top > 0){ //�`�惊�X�g���g�b�v�łȂ����̓X�N���[���̂�
      wk->newLogOfs = wk->ctrl.view_top;
      GAMEBEACON_InfoTblRing_GetBeacon( wk->infoLog, wk->newLogInfo, &wk->newLogTime, wk->newLogOfs );
      wk->newLogIdx = GAMEBEACON_InfoTblRing_Ofs2Idx( wk->infoLog, wk->newLogOfs );
    }
    //�X�N���[�����N�G�X�g
    list_ScrollReq( wk, wk->newLogInfo, wk->newLogIdx, SCROLL_DOWN, PANEL_DRAW_NEW+(wk->ctrl.view_top > 0));
    wk->ctrl.view_top = wk->newLogOfs;
  
//    BeaconView_MenuBarViewSet( wk, MENU_ALL, MENU_ST_OFF );
    sub_PlaySE( BVIEW_SE_NEW_PLAYER );
    return TRUE;
  }
  return FALSE;
}

///////////////////////////////////////////////////////////////////
/*
 *  @brief  �T�u�V�[�P���X�@GPower
 */
///////////////////////////////////////////////////////////////////

/*
 *  @brief  �T�u�V�[�P���X�@GPower�g�p���C��
 */
BOOL BeaconView_SubSeqGPower( BEACON_VIEW_PTR wk )
{
  switch( wk->sub_seq ){
  case 0:
    BeaconView_MenuBarViewSet( wk, MENU_POWER, MENU_ST_ANM );
    BeaconView_PrintQueLimmitUpSet( wk, TRUE );
    effReq_PopupMsgGPowerMine( wk );
    wk->sub_seq++;
    break;
  case 1:
    if( wk->eff_task_ct ){
      break;
    }
    BeaconView_PrintQueLimmitUpSet( wk, FALSE );
    wk->sub_seq = 0;
    return TRUE;
  }
  return FALSE;
}

///////////////////////////////////////////////////////////////////
/*
 *  @brief  �T�u�V�[�P���X�@���
 */
///////////////////////////////////////////////////////////////////
static int sseq_thanks_CheckInput( BEACON_VIEW_PTR wk );

/*
 *  @brief  �T�u�V�[�P���X�@��烁�C��
 */
int BeaconView_SubSeqThanks( BEACON_VIEW_PTR wk )
{
  switch( wk->sub_seq ){
  case SSEQ_THANKS_ICON_ANM:
    BeaconView_MenuBarViewSet( wk, MENU_THANKS, MENU_ST_ANM );
    draw_MenuWindow( wk, msg_sys_thanks );
    wk->sub_seq++;
    break;
  case SSEQ_THANKS_ICON_ANM_WAIT:
    if( BeaconView_MenuBarCheckAnm( wk, MENU_THANKS )){
      break;
    }
    BeaconView_MenuBarViewSet( wk, MENU_POWER, MENU_ST_OFF );
    BeaconView_MenuBarViewSet( wk, MENU_HELLO, MENU_ST_OFF );
    BeaconView_MenuBarViewSet( wk, MENU_RETURN, MENU_ST_OFF );
    wk->sub_seq++;
    break;
  case SSEQ_THANKS_MAIN:
    wk->sub_seq = sseq_thanks_CheckInput( wk );
    if( wk->sub_seq == SSEQ_THANKS_VIEW_UPDATE ){
      return SEQ_LIST_SCROLL_REPEAT;
    }
    break;
  case SSEQ_THANKS_DECIDE:
    {
      u8 idx;
      PANEL_WORK* pp;
      
      idx = GAMEBEACON_InfoTblRing_Ofs2Idx( wk->infoLog, wk->ctrl.target+wk->ctrl.view_top );
      pp = panel_GetPanelFromDataIndex( wk, idx ); 

      //�v���C���[���W�J
      WORDSET_RegisterWord( wk->wordset, 0, pp->name, pp->sex, TRUE, PM_LANG );
      
      BeaconView_PrintQueLimmitUpSet( wk, TRUE );
      effReq_PopupMsgSys( wk, msg_sys_thanks_send );
 
      OS_TPrintf("���肪�Ƃ� �r�[�R���Z�b�g %d\n", pp->tr_id );
      GAMEBEACON_Set_Thankyou( wk->gdata, pp->tr_id );
    }
    wk->sub_seq = SSEQ_THANKS_DECIDE_WAIT;
    break;
  case SSEQ_THANKS_DECIDE_WAIT:
    if( wk->eff_task_ct == 0 ){
      BeaconView_PrintQueLimmitUpSet( wk, FALSE );
      wk->sub_seq = SSEQ_THANKS_END;
    }
    break;
  case SSEQ_THANKS_VIEW_UPDATE:
    if( wk->eff_task_ct == 0 ){
      BeaconView_MenuBarViewSet( wk, MENU_THANKS, MENU_ST_ON );
      wk->sub_seq = SSEQ_THANKS_MAIN;
    }
    break;
  case SSEQ_THANKS_END:
    BeaconView_MenuBarViewSet( wk, MENU_ALL, MENU_ST_ON );
    draw_MenuWindow( wk, msg_sys_now_record );
    wk->sub_seq = 0;
    return SEQ_MAIN;
  }
  return SEQ_THANK_YOU;
}

/*
 *  @brief  ���ΏۑI�����[�v���ł̓��̓`�F�b�N
 */
static int sseq_thanks_CheckInput( BEACON_VIEW_PTR wk )
{
  int ret;
  POINT tp;

  if( !GFL_UI_TP_GetPointTrg( (u32*)&tp.x, (u32*)&tp.y )){
    return SSEQ_THANKS_MAIN;
  }
  
  //���j���[�����蔻��
  ret = touchin_CheckMenu( wk, &tp );
  if(ret == MENU_THANKS){
    return SSEQ_THANKS_END;  //�L�����Z��
  }

  //�p�l�������蔻��
  ret = touchin_CheckPanel( wk, &tp );
  if(ret != GFL_UI_TP_HIT_NONE){
    wk->ctrl.target = ret;
    effReq_SetPanelFlash( wk, ret );
    return SSEQ_THANKS_DECIDE;
  }

  //�㉺��󂠂��蔻��
  ret = touchin_CheckUpDown( wk, &tp );
  if(ret != GFL_UI_TP_HIT_NONE){
    list_UpDownReq1st( wk, ret, SEQ_THANK_YOU );
    return SSEQ_THANKS_VIEW_UPDATE;
  }
  return SSEQ_THANKS_MAIN;
}

/*
 *  @brief  ���j���[�o�[�A�j���Z�b�g
 */
void BeaconView_MenuBarViewSet( BEACON_VIEW_PTR wk, MENU_ID id, MENU_STATE state )
{
  u8 flag;
  
  if( id == MENU_ALL ){
    flag = MENU_F_ALL;
  }else{
    flag = 0x01 << id;
  }

  if( flag & MENU_F_POWER ){
    if( wk->my_data.power != GPOWER_ID_NULL ){
      act_AnmStart( wk->pAct[ACT_POWER], state+ACTANM_POWER_ON );
    }
  }
  if( flag & MENU_F_HELLO ){
    act_AnmStart( wk->pAct[ACT_HELLO], state+ACTANM_HELLO_ON );
  }
  if( flag & MENU_F_THANKS ){
    if( wk->ctrl.max > 0 ){
      act_AnmStart( wk->pAct[ACT_THANKS], state+ACTANM_THANKS_ON );
    }
  }
  if( flag & MENU_F_RETURN ){
    act_AnmStart( wk->pAct[ACT_RETURN], state+ACTANM_RETURN_ON );
  }
}

/*
 *  @brief  ���j���[�o�[�A�j���E�F�C�g
 */
BOOL BeaconView_MenuBarCheckAnm( BEACON_VIEW_PTR wk, MENU_ID id )
{
  return GFL_CLACT_WK_CheckAnmActive( wk->pAct[ACT_POWER+id] );
}

/*
 *  @brief  �X�N���[���{�^���A�j���Z�b�g
 */
void BeaconView_UpDownAnmSet( BEACON_VIEW_PTR wk, SCROLL_DIR dir )
{
  if( wk->scr_repeat_dir == SCROLL_UP ){
    act_AnmStart( wk->pAct[ACT_DOWN], ACTANM_DOWN_ANM );
  }else{
    act_AnmStart( wk->pAct[ACT_UP], ACTANM_UP_ANM );
  }
}

/*
 *  @brief  �X�N���[���{�^����ԃZ�b�g
 */
void BeaconView_UpDownViewSet( BEACON_VIEW_PTR wk )
{
  obj_UpDownViewSet( wk );
}

////////////////////////////////////////////////////////////////////////////
//����G�p���[���������`�F�b�N
////////////////////////////////////////////////////////////////////////////

static void sp_gpower_ConditionCheck( BEACON_VIEW_PTR wk, GAMEBEACON_INFO* info, u8 old_log_max ) 
{
  int i;
  u8 version_ct = 0, sex_ct = 0;
  u16 time;

  //�C�O�Ń����Ƃ����������H
  if( GAMEBEACON_Get_PmLanguage( info ) != PM_LANG ){
    sp_gpower_RequestSet( wk, SP_GPOWER_REQ_HATCH_UP );
  }
  //����Ⴂ�l����100�̔{���ɂȂ������H
  if( ( wk->log_count % 100 ) == 0 ){
    sp_gpower_RequestSet( wk, SP_GPOWER_REQ_SALE );
  }else if( sp_gpower_ZoromeCheck( wk->log_count )){
    //����Ⴂ�l����3���ȏ�̃]���ڂɂȂ������H
    sp_gpower_RequestSet( wk, SP_GPOWER_REQ_CAPTURE_UP );
  }
  //�v���C���[���X�g��30�l���܂������H
  if( old_log_max == (BS_LOG_MAX-1) && wk->ctrl.max == BS_LOG_MAX ){
    sp_gpower_RequestSet( wk, SP_GPOWER_REQ_EXP_UP );
  }

  //���O���J�E���^�[���W
  for( i = 0;i < wk->ctrl.max;i++ ){
    GAMEBEACON_InfoTblRing_GetBeacon( wk->infoLog, wk->tmpInfo2, &time, i );
    if( GAMEBEACON_Get_Sex(wk->tmpInfo2) != wk->my_data.sex ){
      ++sex_ct;
    }
    if( GAMEBEACON_Get_PmVersion( wk->tmpInfo2 ) != wk->my_data.pm_version ){
      ++version_ct; 
    }
  }
  //���O�ɂ��鎩���ƈقȂ鐫�ʂ̃v���C���[��20�l���H
  if( sex_ct == 20 ){
    sp_gpower_RequestSet( wk, SP_GPOWER_REQ_MONEY_UP );
  }
  //���O�ɂ��鎩���ƈقȂ�J�Z�b�g�o�[�W�����̃v���C���[��20�l���H
  if( version_ct == 20 ){
    sp_gpower_RequestSet( wk, SP_GPOWER_REQ_NATSUKI_UP );
  }
}

//----------------------------------------------------------
/*
 *  @brief  ����G�p���[�̔�����\�񂷂�
 */
//----------------------------------------------------------
static void sp_gpower_RequestSet( BEACON_VIEW_PTR wk, u8 req_id )
{
  int i;

  //���ɗ\�񃊃X�g�ɂ��邩�ǂ����H
  for(i = 0;i < wk->sp_gpower_req_ct;i++){
    if(wk->sp_gpower_req[i] == req_id){
      return;
    }
  }
  wk->sp_gpower_req[wk->sp_gpower_req_ct++] = req_id;
}

//----------------------------------------------------------
/*
 *  @brief  �\�񂳂ꂽ����G�p���[�����o��
 *
 *  @note �Ȃ����GPOWER_ID_NULL��Ԃ�
 */
//----------------------------------------------------------
static GPOWER_ID sp_gpower_RequestGet( BEACON_VIEW_PTR wk )
{
  GPOWER_ID gpower_id;

  if( wk->sp_gpower_req[0] == SP_GPOWER_REQ_NULL ){
    return GPOWER_ID_NULL;
  }
  //����o��
  gpower_id = DATA_SpGPowerTbl[ wk->sp_gpower_req[0]-1 ];
  wk->sp_gpower_req_ct--;

  //�e�[�u�����l�߂�
  MI_CpuCopy8( &(wk->sp_gpower_req[1]), &(wk->sp_gpower_req[0]), sizeof(u8)*wk->sp_gpower_req_ct);
  wk->sp_gpower_req[wk->sp_gpower_req_ct] = SP_GPOWER_REQ_NULL;

  return gpower_id;
}

//----------------------------------------------------------
/*
 *  @brief  ����Ⴂ�l����3���ȏ�̃]���ڂ��`�F�b�N 
 */
//----------------------------------------------------------
static BOOL sp_gpower_ZoromeCheck( u32 log_count )
{
  int i,no;
 
  if( (log_count / 100) == 0){
    return FALSE; //2���ȉ�
  }
  no = log_count % 10;  //1���ڎ擾
  log_count /= 10;

  //2���ڈȍ~���擾
  do{
    u8 num = log_count % 10;
    if( num != no ){
      return FALSE;
    }
    log_count /= 10;
  }while( log_count > 0 );

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//�T�u���[�`��
////////////////////////////////////////////////////////////////////////////

/*
 *  @brief  �O�όv�Z
 */
static int calc_Cross( POINT* a, POINT* b ) 
{
  return (a->x*b->y - a->y*b->x);
}

/*
 *  @brief  POINT�̌��Z
 */
static void calc_PointMinus( POINT* a, POINT* b, POINT* out )
{
  out->x = a->x-b->x;
  out->y = a->y-b->y;
}

/*
 *  @brief  �O�p�`�����蔻��
 *
 *  �O�p�`�͔����v���
 * 1-0 p-a, b-a
 * 2-1 p-b, c-b
 * 0-2 p-c, a-c
 */
static BOOL calc_PointInTriangle( POINT* p, TRIANGLE* tri )
{
  int i;
  POINT d1,d2;

  for( i = 0;i < 3;i++ ){
    calc_PointMinus( p, &tri->p[i], &d1 );
    calc_PointMinus( &tri->p[(1+i)%3], &tri->p[i], &d2 );
    if( calc_Cross( &d1, &d2 ) < 0 ){
      return FALSE;
    }
  }
  return TRUE;
}

/*
 *  @brief  �p�l�������蔻��
 */
static int touchin_CheckPanel( BEACON_VIEW_PTR wk, POINT* tp )
{
  int i, ox, oy;
  TRIANGLE tri;

  for(i = 0; i < wk->ctrl.view_max; i++){
    ox = ACT_PANEL_OX*i;
    oy = ACT_PANEL_OY*i;
    tri.p[0].x = TP_PANEL_X1+ox;  
    tri.p[0].y = TP_PANEL_Y1+oy;  
    tri.p[1].x = TP_PANEL_X2+ox;  
    tri.p[1].y = TP_PANEL_Y2+oy;  
    tri.p[2].x = TP_PANEL_X3+ox;  
    tri.p[2].y = TP_PANEL_Y3+oy;  

    if( calc_PointInTriangle( tp, &tri )){
      return i;
    }
    tri.p[0].x = TP_PANEL_X1+ox;  
    tri.p[0].y = TP_PANEL_Y1+oy;  
    tri.p[1].x = TP_PANEL_X3+ox;  
    tri.p[1].y = TP_PANEL_Y3+oy;  
    tri.p[2].x = TP_PANEL_X4+ox;  
    tri.p[2].y = TP_PANEL_Y4+oy;  

    if( calc_PointInTriangle( tp, &tri )){
      return i;
    }
  }
  return GFL_UI_TP_HIT_NONE;
}

/*
 *  @brief  �㉺���L�[�����蔻��
 */
static int touchin_CheckUpDown( BEACON_VIEW_PTR wk, POINT* tp )
{
  int i,j;
  TRIANGLE tri;
  u8 enable = 0;

  static const POINT tbl[2][5] = {
   {
     { TP_UP_X1, TP_UP_Y1 },
     { TP_UP_X2, TP_UP_Y2 },
     { TP_UP_X3, TP_UP_Y3 },
     { TP_UP_X4, TP_UP_Y4 },
     { TP_UP_X5, TP_UP_Y5 },
   },
   {
     { TP_DOWN_X1, TP_DOWN_Y1 },
     { TP_DOWN_X2, TP_DOWN_Y2 },
     { TP_DOWN_X3, TP_DOWN_Y3 },
     { TP_DOWN_X4, TP_DOWN_Y4 },
     { TP_DOWN_X5, TP_DOWN_Y5 },
   },
  };
  static const u8 point[][3] = {
   { 0, 1, 4},
   { 1, 2, 4},
   { 2, 3, 4},
  };

  if( wk->ctrl.view_top > 0 ){  //��L�[�`�F�b�N
    enable |= 1;
  }
  if( (wk->ctrl.view_top+wk->ctrl.view_max) < wk->ctrl.max ){
    enable |= 2;
  }

  for(i = 0;i < 2;i++){
    if( !(enable & (i+1))){
      continue;
    }
    for(j = 0;j < 3;j++){
      tri.p[0] = tbl[i][point[j][0]];
      tri.p[1] = tbl[i][point[j][1]];
      tri.p[2] = tbl[i][point[j][2]];
      if( calc_PointInTriangle( tp, &tri )){
        return (1-i);
      }
    }
  }
  return GFL_UI_TP_HIT_NONE;
}

/*
 *  @brief  ���j���[�o�[�����蔻��
 */
static int touchin_CheckMenu( BEACON_VIEW_PTR wk, POINT* tp )
{
  int i, ret;
  GFL_UI_TP_HITTBL tbl[2];
  static const u16 se_tbl[] = { BVIEW_SE_MENU, BVIEW_SE_MENU, BVIEW_SE_MENU, BVIEW_SE_CANCEL };

  tbl[1].rect.top = GFL_UI_TP_HIT_END;

  for(i = 0;i < ACT_MENU_NUM;i++){
    u8 seq = GFL_CLACT_WK_GetAnmSeq( wk->pAct[ACT_POWER+i] )-(ACTANM_POWER_ON+i*ACT_ANM_SET);
    if( (seq % ACT_ANM_SET) >= 2){
      continue;
    }
    tbl[0].rect.top = ACT_MENU_PY;
    tbl[0].rect.bottom = tbl[0].rect.top+ACT_MENU_SY;
    tbl[0].rect.left = ACT_MENU_PX+ACT_MENU_OX*i;
    tbl[0].rect.right = tbl[0].rect.left+ACT_MENU_SX;

    ret = GFL_UI_TP_HitSelf( tbl, tp->x, tp->y );
    if(ret != GFL_UI_TP_HIT_NONE) {
      sub_PlaySE( se_tbl[i] );
      return i;
    }
  }
  return GFL_UI_TP_HIT_NONE;
}

/*
 *  @brief  �����񐮌`
 *
 *  �w�胁�b�Z�[�W�� wk->str_expand�ɂ߂�
 */
static void print_GetMsgToBuf( BEACON_VIEW_PTR wk, u8 msg_id )
{
  GFL_MSG_GetString( wk->mm_status, msg_id, wk->str_tmp);
  WORDSET_ExpandStr( wk->wordset, wk->str_expand, wk->str_tmp );
}

/*
 *  @brief  ���O�l���\��
 */
static void draw_LogNumWindow( BEACON_VIEW_PTR wk )
{
  WORDSET_RegisterNumber( wk->wordset, 0, wk->ctrl.max, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
  print_GetMsgToBuf( wk, msg_sys_lognum );
  printReq_BmpwinPrint( wk, &wk->win[WIN_LOGNUM], wk->str_expand, 4, 0, FCOL_LOGNUM_BASE, FCOL_LOGNUM, NULL );
}

/*
 *  @brief  ���j���[�E�B���h�E�\��
 */
static void draw_MenuWindow( BEACON_VIEW_PTR wk, u8 msg_id )
{
  WORDSET_RegisterNumber( wk->wordset, 0, wk->log_count, 5, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  print_GetMsgToBuf( wk, msg_id );
  printReq_BmpwinPrint( wk, &wk->win[WIN_MENU], wk->str_expand, 0, 4, 0, FCOL_WHITE_N, NULL );
}

/*
 *  @brief  ���j���[�E�B���h�E�\���N���A
 */
static void draw_ClearMenuWindow( BEACON_VIEW_PTR wk )
{
  GFL_BMP_Clear( wk->win[WIN_MENU].bmp, FCOL_WHITE_N );
  GFL_BMPWIN_TransVramCharacter( wk->win[WIN_MENU].win );
}

/*
 *  @brief  �|�b�v�A�b�v�E�B���h�E������`��
 */
static void print_PopupWindow( BEACON_VIEW_PTR wk, STRBUF* str, int* task_ct )
{
  printReq_BmpwinPrint( wk, &wk->win[WIN_POPUP], str, 0, 0, FCOL_POPUP_BASE, FCOL_POPUP, task_ct );
}

//------------------------------------------------------------
/*
 *  @brief  �|�b�v�A�b�v�@�^�C���E�F�C�g�`�F�b�N
 */
//------------------------------------------------------------
static BOOL print_PopupWindowTimeWaitCheck( u8* p_wait )
{
  u32 x,y;
  u16 pat = 0xFFFE; 

  if( (*p_wait)-- == 0 ){ //�^�C���A�E�g
    *p_wait = 0;
    return TRUE; 
  }
 
  //�^�b�`�`�F�b�N
  if( GFL_UI_TP_GetPointTrg( &x, &y )){
    //�X�N���[���̂��߁A�X�N���[���������W+64dot�̈ʒu�ɂ���̂ŁA������ƈʒu�␳���ă`�F�b�N
    if( GFL_BG_CheckDot( FRM_POPUP, x, y+64, &pat )){
      *p_wait = 0;
      return TRUE; 
    }
  }
  return FALSE;
}

/*
 *  @brief  TaskMenu YesNo����
 */
static void tmenu_Create( BEACON_VIEW_PTR wk, u8 menu_idx )
{
  int i,max,px;
  TMENU_ITEM* tmenu;

  if( menu_idx == TMENU_ID_YESNO ){
    tmenu = wk->tmenu;
    max = TMENU_YN_MAX;
    px = TMENU_YN_PX;
  }else{
    tmenu = wk->tmenu_check;
    max = TMENU_CHECK_MAX;
    px = TMENU_CHECK_PX;
  }
  for( i = 0;i < max;i++){
    tmenu[i].work = APP_TASKMENU_WIN_CreateEx( wk->menuRes, &tmenu[i].item, 
                        px+TMENU_W*i, TMENU_PY, TMENU_W, TMENU_H, TRUE, FALSE ,wk->heap_sID );
  }
}

/*
 *  @brief  TaskMenu �A�b�v�f�[�g
 */
static int tmenu_Update( BEACON_VIEW_PTR wk,u8 menu_idx )
{
  int i,max,ret = -1;
  TMENU_ITEM* tmenu;

  if( menu_idx == TMENU_ID_YESNO ){
    tmenu = wk->tmenu;
    max = TMENU_YN_MAX;
  }else{
    tmenu = wk->tmenu_check;
    max = TMENU_CHECK_MAX;
  }

  for( i = 0;i < max;i++){
    if( APP_TASKMENU_WIN_IsTrg( tmenu[i].work )){
      APP_TASKMENU_WIN_SetDecide( tmenu[i].work, TRUE );
      if(i < (max-1)){
        sub_PlaySE( BVIEW_SE_DECIDE );
      }else{
        sub_PlaySE( BVIEW_SE_CANCEL );
      }
      ret = i;
    }
    APP_TASKMENU_WIN_Update( tmenu[i].work );
  }
  return ret;
}

/*
 *  @brief  TaskMenu �I���A�j���҂�
 */
static BOOL tmenu_YnEndWait( BEACON_VIEW_PTR wk, u8 select )
{
  int i;

  APP_TASKMENU_WIN_Update( wk->tmenu[select].work );

  if( APP_TASKMENU_WIN_IsFinish( wk->tmenu[select].work )){
    if( select == TMENU_YN_CHECK ){
      APP_TASKMENU_WIN_ResetDecide( wk->tmenu[select].work );
      return TRUE;  //<�m�F>��I�񂾏ꍇ�����Ȃ��Ń��Z�b�g
    }
    for( i = 0;i < TMENU_YN_MAX;i++){
      APP_TASKMENU_WIN_Delete( wk->tmenu[i].work );
      wk->tmenu[i].work = NULL;
    }
    return TRUE;
  }
  return FALSE;
}

/*
 *  @brief  TaskMenu G�p���[�m�F�_�C�A���O�I���A�j���҂�
 */
static BOOL tmenu_CheckEndWait( BEACON_VIEW_PTR wk, u8 select )
{
  int i;

  APP_TASKMENU_WIN_Update( wk->tmenu_check[select].work );

  if( APP_TASKMENU_WIN_IsFinish( wk->tmenu_check[select].work )){
    for( i = 0;i < TMENU_CHECK_MAX;i++){
      APP_TASKMENU_WIN_Delete( wk->tmenu_check[i].work );
      wk->tmenu_check[i].work = NULL;
    }
    return TRUE;
  }
  return FALSE;
}


//==================================================================
/**
 * �r�[�R����񂩂�\�����郁�b�Z�[�WID���擾����
 *
 * @param   info		�Ώۂ̃r�[�R�����ւ̃|�C���^
 */
//==================================================================
static u32 sub_GetBeaconMsgID(const GAMEBEACON_INFO *info)
{
  GAMEBEACON_ACTION action = GAMEBEACON_Get_Action_ActionNo(info);

  if(action == GAMEBEACON_ACTION_NULL || action >= GAMEBEACON_ACTION_MAX){
    GF_ASSERT_MSG(0, "action_no = %d\n", action);
    return msg_beacon_001;
  }
  return msg_beacon_001 + action - GAMEBEACON_ACTION_SEARCH;
}

/*
 *  @brief  SE�Đ�
 */
static void sub_PlaySE( u16 se_no )
{
  PMSND_PlaySE( se_no );
}

/*
 *  @brief  �A�N�^�[�@���W�Z�b�g
 */
static void act_SetPosition( GFL_CLWK* act, s16 x, s16 y )
{
  GFL_CLACTPOS pos;
  pos.x = x;
  pos.y = y;
  
  GFL_CLACT_WK_SetPos( act, &pos, ACT_RENDER_ID);
}

/*
 *  @brief  �A�N�^�[ �A�j���X�^�[�g
 */
static void act_AnmStart( GFL_CLWK* act, u8 anm_no )
{
  GFL_CLACT_WK_SetAnmSeq( act, anm_no );
  GFL_CLACT_WK_ResetAnm( act );
}

/*
 *  @brief  ���j���[�A�N�^�[ View�Z�b�g
 */
static void obj_MenuIconVisibleSet( BEACON_VIEW_PTR wk, BOOL visible_f )
{
  int i;

  for(i = ACT_POWER;i <= ACT_RETURN;i++){
    GFL_CLACT_WK_SetDrawEnable( wk->pAct[i], visible_f );
  }
}

/*
 *  @brief  UpDown�A�N�^�[ View�Z�b�g
 */
static void obj_UpDownViewSet( BEACON_VIEW_PTR wk )
{
  if(wk->ctrl.view_top == 0){
    GFL_CLACT_WK_SetAnmSeq( wk->pAct[ACT_UP], ACTANM_UP_OFF );  
  }else{
    GFL_CLACT_WK_SetAnmSeq( wk->pAct[ACT_UP], ACTANM_UP_DEF );  
  }
  if((wk->ctrl.view_top+wk->ctrl.view_max) < (wk->ctrl.max)){
    GFL_CLACT_WK_SetAnmSeq( wk->pAct[ACT_DOWN], ACTANM_DOWN_DEF );  
  }else{
    GFL_CLACT_WK_SetAnmSeq( wk->pAct[ACT_DOWN], ACTANM_DOWN_OFF );  
  }
}

/*
 *  @brief  ���A�N�^�[ View�Z�b�g
 */
static void obj_ThanksViewSet( BEACON_VIEW_PTR wk )
{
  if( wk->ctrl.max == 0){
    GFL_CLACT_WK_SetAnmSeq( wk->pAct[ACT_THANKS], ACTANM_THANKS_OFF );  
  }else{
    GFL_CLACT_WK_SetAnmSeq( wk->pAct[ACT_THANKS], ACTANM_THANKS_ON );  
  }
}

/*
 *  @brief  �p�l�� �\�����On/Off
 */
static void panel_VisibleSet( PANEL_WORK* pp, BOOL visible_f )
{
  BmpOam_ActorSetDrawEnable( pp->msgOam.oam, visible_f );
  GFL_CLACT_WK_SetDrawEnable( pp->cUnion, visible_f );
  GFL_CLACT_WK_SetDrawEnable( pp->cPanel, visible_f );

  if( !visible_f ){ //�B����������������
    panel_IconVisibleSet( pp, visible_f );
  }
}

/*
 *  @brief  �p�l���A�C�R�� �\�����On/Off
 */
static void panel_IconVisibleSet( PANEL_WORK* pp, BOOL visible_f )
{
  GFL_CLACT_WK_SetDrawEnable( pp->cIcon, visible_f );
}

/*
 *  @brief  �p�l���N���A�[
 */
static void panel_Clear( PANEL_WORK* pp )
{
  panel_VisibleSet( pp, FALSE );
  panel_IconVisibleSet( pp, FALSE );

  GFL_BMP_Clear( pp->msgOam.bmp, 0 );
	BmpOam_ActorBmpTrans( pp->msgOam.oam );
  GFL_STR_ClearBuffer( pp->name );
  GFL_STR_ClearBuffer( pp->str );

  pp->data_idx = PANEL_DATA_BLANK;
  pp->n_line = 0;
  pp->rank = 0;
  
  if( pp->tcb_print != NULL ){
#ifdef PM_DEBUG
    if( PRINTSYS_QUE_IsExistTarget( pp->que, pp->msgOam.bmp ) ){
      GF_ASSERT(0);
    }
#endif
    GFL_TCBL_Delete( pp->tcb_print );
    pp->tcb_print = NULL;
  }
  if( pp->tcb_icon != NULL ){
    GFL_TCBL_Delete( pp->tcb_icon );
    pp->tcb_icon = NULL;
  }
}

/*
 *  @brief  �p�l���o�^
 */
static void panel_Entry( PANEL_WORK* pp, u8 data_idx,u8 line )
{
  panel_Clear( pp );
  pp->data_idx = data_idx;
  pp->n_line = line;
}

/*
 *  @brief  �w��f�[�^index�����p�l����T��
 */
static PANEL_WORK* panel_GetPanelFromDataIndex( BEACON_VIEW_PTR wk, u8 data_idx )
{
  int i;
  PANEL_WORK* pp;
  for(i = 0;i < PANEL_MAX;i++){
    if( wk->panel[i].data_idx == data_idx ){
      return &wk->panel[i];
    }
  }
  if(data_idx == PANEL_DATA_BLANK){
    GF_ASSERT(0);
    return NULL;
  }
  return NULL;
}

/*
 *  @brief  �p�l���@Union�A�N�^�[�X�V
 */
static void panel_UnionObjUpdate( BEACON_VIEW_PTR wk, PANEL_WORK* pp, GAMEBEACON_INFO* info)
{
  u8 sex, char_no, p_ofs;
 
  char_no = GAMEBEACON_Get_TrainerView(info);

  //�L�����N�^�]��
  GFL_CLGRP_CGR_Replace(  wk->objResUnion.res[ OBJ_RES_CGR ].tbl[ pp->id ],
                          wk->resCharUnion[ char_no ].p_char );
  //�p���b�g�]��
  p_ofs = 16*sc_wifi_unionobj_plt[char_no];
  PaletteWorkSet( wk->pfd, &wk->resPlttUnion.dat[p_ofs], FADE_SUB_OBJ, (ACT_PAL_UNION+pp->id)*16, 0x20 );
}

/*
 *  @brief  �p�l���@�A�C�R���A�N�^�[�X�V
 */
static void panel_IconObjUpdate( BEACON_VIEW_PTR wk, PANEL_WORK* pp, u8 icon)
{
  //�L�����N�^�]��
  GFL_CLGRP_CGR_Replace(  wk->objResIcon.res[ OBJ_RES_CGR ].tbl[ pp->id ],
                          wk->resCharIcon[ icon ].p_char );
}

/*
 *  @brief  �p�l���J���[�Q�b�g 
 */
static u8 panel_FrameColorGet( GAMEBEACON_INFO* info )
{
  int version = GAMEBEACON_Get_PmVersion(info);

  switch( version ){
  case VERSION_BLACK:
    return ACT_PANEL_PAL_FRM_V_BLACK;
  case VERSION_WHITE:
    return ACT_PANEL_PAL_FRM_V_WHITE;
  }
  return ACT_PANEL_PAL_FRM_V_ELSE;
}

/*
 *  @brief  �p�l���J���[�Z�b�g
 */
static void panel_ColorPlttSet( BEACON_VIEW_PTR wk, PANEL_WORK* pp, GAMEBEACON_INFO* info )
{
  u16 pal[5];
  u8  col;
  
  //�p�l���t���[���J���[
  col = panel_FrameColorGet( info );
  pal[0] = wk->resPlttPanel.dat[ col*16+0 ];
  pal[1] = wk->resPlttPanel.dat[ col*16+1 ];
  pal[2] = wk->resPlttPanel.dat[ col*16+2 ];
  
  //�p�l���x�[�X�J���[
  GAMEBEACON_Get_FavoriteColor((GXRgb*)&pal[3], info);
  SoftFade(&pal[3], &pal[4], 1, 3, 0x0000); //��F���쐬
 
  PaletteWorkSet( wk->pfd, pal, FADE_SUB_OBJ, (ACT_PAL_PANEL+pp->id)*16+1, 2*5 );
}

/*
 *  @brief  �p�l���@������`��
 */
static void panel_MsgPrint( BEACON_VIEW_PTR wk, PANEL_WORK* pp, STRBUF* str, int* task_ct )
{
  printReq_FOamPrint( wk, pp, str, FCOL_FNTOAM_W, task_ct );
}

/*
 *  @brief  �p�l���@���O������`��
 */
static void panel_NamePrint( BEACON_VIEW_PTR wk, PANEL_WORK* pp, BOOL force_f, int* task_ct )
{
  u8  flag;
  u16 time = GAMEBEACON_Get_RecvBeaconTime( pp->tr_id );

  flag = (time >= BEACON_TIMEOUT_FRAME );

  if( flag == pp->timeout_f && !force_f){
    return;
  }
  pp->timeout_f = flag;

  //�ĕ`��
  if( flag ){
    printReq_FOamPrint( wk, pp, pp->name, FCOL_FNTOAM_G, task_ct );
  }else{
    printReq_FOamPrint( wk, pp, pp->name, FCOL_FNTOAM_W, task_ct );
  }
}

/*
 *  @brief  �p�l���@�����N�`��
 */
static void panel_RankSet( BEACON_VIEW_PTR wk, PANEL_WORK* pp, GAMEBEACON_INFO* info )
{
  u8 num;
  u16 pal;
  pp->rank = GAMEBEACON_Get_ResearchTeamRank(info);
  if(pp->rank == 0){
    pal = ACT_PANEL_PAL_RANK_MARK*16;
    num = RESEARCH_TEAM_RANK_5;
  }else{
    pal = ACT_PANEL_PAL_RANK_MARK*16+RESEARCH_TEAM_RANK_5;
    num = pp->rank;
  }
  PaletteWorkSet( wk->pfd, &wk->resPlttPanel.dat[ pal ],
      FADE_SUB_OBJ, (ACT_PAL_PANEL+pp->id)*16+ACT_PANEL_COL_OFS_RANK, 2*num );
}

/*
 *  @brief  �p�l���`��
 */
static void panel_Draw( BEACON_VIEW_PTR wk,
    PANEL_WORK* pp, GAMEBEACON_INFO* info, PANEL_DRAW_TYPE draw_type, BOOL icon_cancel_f, int* task_ct )
{
  //�g���[�i�[ID�擾
  pp->tr_id = GAMEBEACON_Get_TrainerID( info );
  pp->sex = GAMEBEACON_Get_Sex( info );

  //�v���C���[���擾
  GAMEBEACON_Get_PlayerNameToBuf( info, pp->name );
  //���b�Z�[�W�o�b�t�@������
  GFL_STR_ClearBuffer( pp->str );
  GAMEBEACON_Get_SelfIntroduction( info, pp->str );

  //�����N�Z�b�g
  panel_RankSet( wk, pp, info);

  //UnionOBJ���\�[�X�X�V
  panel_UnionObjUpdate( wk, pp, info);

  //�p�l���J���[�]��
  panel_ColorPlttSet( wk, pp, info );

  //�p�l��������o�b�t�@�]��&�A�C�R���\�����N�G�X�g
  if( icon_cancel_f ){
    return;
  }
  switch( draw_type ){
  case PANEL_DRAW_UPDATE:
    {
      GAMEBEACON_ACTION action = GAMEBEACON_Get_Action_ActionNo( info );
      switch( action ){
      case GAMEBEACON_ACTION_THANKYOU:
        GAMEBEACON_Get_ThankyouMessage(info, wk->str_tmp );
        panel_MsgPrint( wk, pp, wk->str_tmp, task_ct ); 
        break;
      case GAMEBEACON_ACTION_FREEWORD:
        GAMEBEACON_Get_FreeWordMessage(info, wk->str_tmp );
        panel_MsgPrint( wk, pp, wk->str_tmp, task_ct ); 
        break;
      }
      taskAdd_IconEffect( wk, pp, info, FALSE );
    }
    break;
  case PANEL_DRAW_NEW:
    panel_MsgPrint( wk, pp, pp->str, task_ct ); 
    taskAdd_IconEffect( wk, pp, info, TRUE );
    break;
  case PANEL_DRAW_INI:
    panel_NamePrint( wk, pp, TRUE, task_ct );
    break;
  }
}

/*
 *  @brief  �p�l���@�ʒu�Z�b�g
 */
static void  panel_SetPos( PANEL_WORK* pp, s16 x, s16 y )
{
  pp->px = x;
  pp->py = y;
  act_SetPosition( pp->cPanel, x, y );
  act_SetPosition( pp->cUnion, x+ACT_UNION_OX, y+ACT_UNION_OY );
  act_SetPosition( pp->cIcon, x+ACT_ICON_OX, y+ACT_ICON_OY );
  BmpOam_ActorSetPos( pp->msgOam.oam, x+ACT_MSG_OX, y+ACT_MSG_OY);
}

/*
 *  @brief  �`��p�l���^�C���A�E�g�`�F�b�N
 */
static void list_TimeOutCheck( BEACON_VIEW_PTR wk )
{
  int i;

   //�ŐV���X�g�̖��O�̂ݕ`�悵�Ȃ���
  for(i = 0;i < wk->ctrl.view_max;i++){
    int idx;
    PANEL_WORK* pp;

    idx = GAMEBEACON_InfoTblRing_Ofs2Idx( wk->infoLog, wk->ctrl.view_top+i );
    pp = panel_GetPanelFromDataIndex( wk, idx );
    if( pp->tcb_icon == NULL || pp->tcb_print ){  //���b�Z�[�W�`�撆�̓X�L�b�v
      panel_NamePrint( wk, pp, FALSE, NULL );
    }
  }
}

/*
 *  @brief  ���X�g�A�b�v�_�E�����N�G�X�g
 */
static void list_UpDownReq( BEACON_VIEW_PTR wk, SCROLL_DIR dir )
{
  u8 ofs,idx;
  
  sub_PlaySE( BVIEW_SE_UPDOWN );

  if( dir == SCROLL_UP ){
    ofs = wk->ctrl.view_top+wk->ctrl.view_max;
    BEACON_STATUS_SetViewTopOffset( wk->b_status, ++wk->ctrl.view_top );
  }else{
    ofs = wk->ctrl.view_top-1;
    BEACON_STATUS_SetViewTopOffset( wk->b_status, --wk->ctrl.view_top );
  }
  GAMEBEACON_InfoTblRing_GetBeacon( wk->infoLog, wk->tmpInfo, &wk->tmpTime, ofs );
  idx = GAMEBEACON_InfoTblRing_Ofs2Idx( wk->infoLog, ofs );
  
  //�X�N���[�����N�G�X�g
  list_ScrollReq( wk, wk->tmpInfo, idx, dir, PANEL_DRAW_INI );
}

/*
 *  @brief  ���X�g�A�b�v�_�E�����񃊃N�G�X�g
 */
static void list_UpDownReq1st( BEACON_VIEW_PTR wk, SCROLL_DIR dir, int ret_seq )
{
  wk->scr_repeat_f = TRUE;
  wk->scr_repeat_end = FALSE;
  wk->scr_repeat_dir = dir;
  wk->scr_repeat_ret_seq = ret_seq;
  wk->scr_repeat_ct = 0;

  BeaconView_PrintQueLimmitUpSet( wk, TRUE );

  if( wk->scr_repeat_dir == SCROLL_UP ){
    act_AnmStart( wk->pAct[ACT_DOWN], ACTANM_DOWN_ON );
  }else{
    act_AnmStart( wk->pAct[ACT_UP], ACTANM_UP_ON );
  }
  BeaconView_MenuBarViewSet( wk, MENU_ALL, MENU_ST_OFF );

  list_UpDownReq( wk, dir );
}

static int list_GetScrollLineNum( BEACON_VIEW_PTR wk, BOOL new_f )
{
  if( !new_f ){
    return 0;
  }
  if( wk->ctrl.max == 1 ) {
     return 0;
  }
  return 1;
}

/////////////////////////////////////////////////////////////////////
/*
 *  @brief  ���X�g�X�N���[�����N�G�X�g�^�X�N���� 
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_LIST_SCROLL{
  u8  seq;
  u8  dir;
  PANEL_DRAW_TYPE draw_type;
  PANEL_WORK* pp;

  BEACON_VIEW_PTR bvp;
  int child_task;
  int* task_ct;
}TASKWK_LIST_SCROLL;

static void tcb_ListScroll( GFL_TCBL *tcb , void* work);

/*
 *  @brief  �r���[ �X�N���[���������N�G�X�g
 */
static void list_ScrollReq( BEACON_VIEW_PTR wk, GAMEBEACON_INFO* info, u8 idx, u8 dir, PANEL_DRAW_TYPE draw_type )
{
  PANEL_WORK* pp;
  GFL_TCBL* tcb;
  TASKWK_LIST_SCROLL* twk;

  //�󂫃p�l������
  pp = panel_GetPanelFromDataIndex( wk, PANEL_DATA_BLANK );
  if( pp == NULL){
    GF_ASSERT(0);
    return; //����̂��߂̃`�F�b�N(�����������Ȃ�NULL�͂Ȃ�)
  }
  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_ListScroll, sizeof(TASKWK_LIST_SCROLL), 0 );
  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_LIST_SCROLL ));

  twk->bvp = wk;
  twk->dir = dir;
  twk->pp = pp;
  twk->draw_type = draw_type;
  twk->task_ct = &wk->eff_task_ct;

  wk->eff_task_ct++;

  panel_Entry( pp, idx, 0 );  //�p�l���V�K�o�^(���C���͌�ŏ�����)
  panel_Draw( wk, pp, info, draw_type, FALSE, &twk->child_task );   //�p�l���`��
}

static void tcb_ListScroll( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_LIST_SCROLL* twk = (TASKWK_LIST_SCROLL*)tcb_wk;
  BEACON_VIEW_PTR bvp = twk->bvp;
  
  if( twk->child_task > 0 ){
    return;
  }
  
  switch( twk->seq ){
  case 0:
    panel_VisibleSet( twk->pp, TRUE );   //�p�l���`��

    //�X�N���[���p�^�[��
    if( twk->draw_type == PANEL_DRAW_NEW ){  //�X���C�h�C��
      effReq_PanelSlideIn( bvp, twk->pp, &twk->child_task );
      panel_IconVisibleSet( twk->pp, TRUE );
    }else{
      effReq_PanelScroll( bvp, twk->dir, twk->pp, NULL, &twk->child_task );
    }
    twk->seq++;
    return;
  default:
    break;
  }
  if( twk->task_ct != NULL ){
    (*twk->task_ct)--;
  }
  GFL_TCBL_Delete(tcb);
}


/////////////////////////////////////////////////////////////////////
/*
 *  @brief  �p�l���X�N���[��
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_PANEL_SCROLL{
  int seq;
  BEACON_VIEW_PTR bvp;
  PANEL_WORK* pp;
  u8  dir;
  u8  deray;
  u8  frame;
  u8  ct;
  s16 epx,epy;
  fx32  x,y;
  fx32  ax,ay;
  
  int* task_ct;
}TASKWK_PANEL_SCROLL;

static void taskAdd_PanelScroll( BEACON_VIEW_PTR wk, PANEL_WORK* pp, u8 dir, u8 wait, int* task_ct );
static void tcb_PanelScroll( GFL_TCBL *tcb , void* work);

/*
 *  @brief  �p�l���X�N���[�����N�G�X�g
 */
static void effReq_PanelScroll( BEACON_VIEW_PTR wk, u8 dir, PANEL_WORK* new_pp, PANEL_WORK* egnore_pp, int* task_ct )
{
  int i;
  PANEL_WORK* pp;

  if( new_pp != NULL ){
    if( dir == SCROLL_UP ){
      new_pp->n_line = PANEL_LINE_END;
    }else{
      new_pp->n_line = 0;
    }
    panel_SetPos( new_pp, new_pp->n_line*ACT_PANEL_OX+ACT_PANEL_PX,
                          new_pp->n_line*ACT_PANEL_OY+ACT_PANEL_PY );
    panel_VisibleSet( new_pp, TRUE );
  }
  for(i = 0;i < PANEL_MAX;i++){
    pp = &wk->panel[i];

    if( (pp->data_idx == PANEL_DATA_BLANK) ||
        ((egnore_pp != NULL) && pp->id == egnore_pp->id)){
      continue;
    }
    taskAdd_PanelScroll( wk, pp, dir, 0, task_ct );
  }
}

/*
 *  @brief  �p�l���X���C�h�C�����N�G�X�g
 */
static void effReq_PanelSlideIn( BEACON_VIEW_PTR wk, PANEL_WORK* pp, int* task_ct )
{
  int i,deray = 0;
  PANEL_WORK* panel;

  pp->n_line = PANEL_VIEW_START;
  panel_SetPos( pp, ACT_PANEL_SI_SX, ACT_PANEL_SI_SY );
  panel_VisibleSet( pp, TRUE );
  if( wk->ctrl.max > 0 ){
    deray = PANEL_SCROLL_FRAME;
  }
  taskAdd_PanelScroll( wk, pp, SCROLL_RIGHT, deray, task_ct );

  effReq_PanelScroll( wk, SCROLL_DOWN, NULL, pp, task_ct );
}

/*
 *  @brief  �p�l���X�N���[���^�X�N�o�^
 */
static void taskAdd_PanelScroll( BEACON_VIEW_PTR wk, PANEL_WORK* pp, u8 dir, u8 wait, int* task_ct )
{
  GFL_TCBL* tcb;
  TASKWK_PANEL_SCROLL* twk;

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_PanelScroll, sizeof(TASKWK_PANEL_SCROLL), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_PANEL_SCROLL));

  twk->dir = dir;
  if( twk->dir == SCROLL_UP ){
    pp->n_line--;
  }else if( twk->dir == SCROLL_DOWN ){
    pp->n_line++;
  }

  twk->bvp = wk;
  twk->pp = pp;
  twk->epx = pp->n_line*ACT_PANEL_OX+ACT_PANEL_PX; 
  twk->epy = pp->n_line*ACT_PANEL_OY+ACT_PANEL_PY; 
  twk->x = FX32_CONST(pp->px);
  twk->y = FX32_CONST(pp->py);
  twk->frame = PANEL_SCROLL_FRAME;
  twk->deray = wait;
  twk->ax = FX_Div( FX32_CONST( twk->epx - pp->px ), FX32_CONST(twk->frame));
  twk->ay = FX_Div( FX32_CONST( twk->epy - pp->py ), FX32_CONST(twk->frame));

  twk->task_ct = task_ct;
  (*task_ct)++;
}

static void tcb_PanelScroll( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_PANEL_SCROLL* twk = (TASKWK_PANEL_SCROLL*)tcb_wk;
  BEACON_VIEW_PTR bvp = twk->bvp;
 
  switch( twk->seq ){
  case 0:
    if( twk->deray > 0 ){
      twk->deray--;
      return;
    }
    if( ++twk->ct < twk->frame){
      twk->x += twk->ax;
      twk->y += twk->ay;
      panel_SetPos( twk->pp, FX_Whole(twk->x), FX_Whole(twk->y));
      return;
    }
    panel_SetPos( twk->pp, twk->epx, twk->epy);
  
    if( twk->pp->n_line == 0 || twk->pp->n_line == PANEL_LINE_END){
      twk->seq++;
      return;
    }
    break; 
  case 1:
  default:
    if( twk->pp->tcb_print != NULL ){
      return;
    }
    panel_Clear( twk->pp );  //�p�l���j��
    break;
  }
  --(*twk->task_ct);
  GFL_TCBL_Delete(tcb);
}

/////////////////////////////////////////////////////////////////////
/*
 *  @brief  ���b�Z�[�W�E�B���h�E �A�b�v�_�E�� 
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_MSG_UPDOWN{
  u8  dir;
  u8  frame;
  u8  ct;
  s8  y;
  s8  diff;

  int* task_ct;
}TASKWK_MSG_UPDOWN;

static void taskAdd_MsgUpdown( BEACON_VIEW_PTR wk, u8 dir, int* task_ct );
static void tcb_MsgUpdown( GFL_TCBL *tcb , void* work);

/*
 *  @brief  ���b�Z�[�W�E�B���h�E �A�b�v�_�E���^�X�N�o�^
 */
static void taskAdd_MsgUpdown( BEACON_VIEW_PTR wk, u8 dir, int* task_ct )
{
  GFL_TCBL* tcb;
  TASKWK_MSG_UPDOWN* twk;

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_MsgUpdown, sizeof(TASKWK_MSG_UPDOWN), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_MSG_UPDOWN ));

  twk->dir = dir;
  if( twk->dir == SCROLL_UP ){
    twk->y = 0;
    twk->diff = POPUP_DIFF;
  }else{
    twk->y = POPUP_HEIGHT;
    twk->diff = -POPUP_DIFF;
  }
  twk->frame = POPUP_COUNT;

//  IWASAWA_Printf("MsgWin UpDown Req %d\n", twk->dir );
  twk->task_ct = task_ct;
  (*task_ct)++;
}

static void tcb_MsgUpdown( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_MSG_UPDOWN* twk = (TASKWK_MSG_UPDOWN*)tcb_wk;
 
  if( twk->ct++ < twk->frame ){
    twk->y += twk->diff;
    GFL_BG_SetScroll( FRM_POPUP, GFL_BG_SCROLL_Y_SET, twk->y );
    return;
  }
//  IWASAWA_Printf("MsgWin UpDown End %d\n", twk->dir );

  --(*twk->task_ct);
  GFL_TCBL_Delete(tcb);
}

/////////////////////////////////////////////////////////////////////
/*
 *  @brief  ���b�Z�[�W�E�B���h�E �|�b�v�A�b�v 
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_WIN_POPUP{
  u8  seq;
  u8  wait;
  int child_task;
 
  BEACON_VIEW_PTR bvp;
  int* task_ct;
}TASKWK_WIN_POPUP;

static void taskAdd_WinPopup( BEACON_VIEW_PTR wk, STRBUF* str, int* task_ct );
static void tcb_WinPopup( GFL_TCBL *tcb , void* work);

static BOOL effReq_PopupMsg( BEACON_VIEW_PTR wk, GAMEBEACON_INFO* info, BOOL new_f )
{
  GAMEBEACON_ACTION action = GAMEBEACON_Get_Action_ActionNo( info );

  if( new_f ){
    GAMEBEACON_InfoWordset( info, wk->wordset, wk->tmpHeapID );
    effReq_PopupMsgSys( wk, msg_beacon_001 );
    return TRUE;
  }
  switch( action ){
  case GAMEBEACON_ACTION_GPOWER:
    return effReq_PopupMsgGPower( wk, info );
  
  case GAMEBEACON_ACTION_THANKYOU:
    //�����󂯂��񐔃C���N�������g
    GAMEBEACON_Set_ThankyouOver( MISC_CrossComm_IncThanksRecvCount( wk->misc_sv ) );

    //�u���C�N�X���[
  default:
    effReq_PopupMsgFromInfo( wk, info );
    break;
  }
  return TRUE;
}

static void effReq_PopupMsgFromInfo( BEACON_VIEW_PTR wk, GAMEBEACON_INFO* info )
{
  GAMEBEACON_InfoWordset( info, wk->wordset, wk->tmpHeapID );
  print_GetMsgToBuf( wk, sub_GetBeaconMsgID(info) );
  taskAdd_WinPopup( wk, wk->str_expand, &wk->eff_task_ct);
}

static void effReq_PopupMsgSys( BEACON_VIEW_PTR wk, u8 msg_id )
{
  print_GetMsgToBuf( wk, msg_id );
  taskAdd_WinPopup( wk, wk->str_expand, &wk->eff_task_ct);
}

/*
 *  @brief  ���b�Z�[�W�E�B���h�E �|�b�v�A�b�v�^�X�N�o�^
 */
static void taskAdd_WinPopup( BEACON_VIEW_PTR wk, STRBUF* str, int* task_ct )
{
  GFL_TCBL* tcb;
  TASKWK_WIN_POPUP* twk;

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_WinPopup, sizeof(TASKWK_WIN_POPUP), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_WIN_POPUP ));

  GFL_STR_CopyBuffer( wk->str_popup, str );
  
  twk->bvp = wk;
  twk->wait = POPUP_WAIT;
  
//  IWASAWA_Printf("WinPopup Req\n");
  BeaconView_MenuBarViewSet( wk, MENU_ALL, MENU_ST_OFF );

  twk->task_ct = task_ct;
  (*task_ct)++;
}

static void tcb_WinPopup( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_WIN_POPUP* twk = (TASKWK_WIN_POPUP*)tcb_wk;
  BEACON_VIEW_PTR bvp = twk->bvp;
  
  if( twk->child_task ){
    return;
  }

  switch( twk->seq ){
  case 0:
    print_PopupWindow( twk->bvp, twk->bvp->str_popup, &twk->child_task );
    twk->seq++;
    return;
  case 1:
    taskAdd_MsgUpdown( twk->bvp, SCROLL_UP, &twk->child_task );
    twk->seq++;
    return;
  case 2:
    if( print_PopupWindowTimeWaitCheck( &twk->wait ) == FALSE ){
      return;
    }
    taskAdd_MsgUpdown( twk->bvp, SCROLL_DOWN, &twk->child_task );
    twk->seq++;
    return;
  default:
    break;
  }
//  IWASAWA_Printf("WinPopup End\n" );
  --(*twk->task_ct);
  GFL_TCBL_Delete(tcb);
}


/////////////////////////////////////////////////////////////////////
/*
 *  @brief  ���b�Z�[�W�E�B���h�E G�p���[�g�p�m�F 
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_WIN_GPOWER{
  u8  seq;
  u8  seq_next;
  u8  wait;
  u8  type;
  u8  cancel_f;
  u8  ret;
  int child_task;

  GPOWER_ID  g_power;
  u16 item_num;
  u16 item_use;
  u16 mypower_use;
  u16 mypower_min;
  u16 mypower_sec;
  BEACON_VIEW_PTR bvp;
  int* task_ct;
}TASKWK_WIN_GPOWER;

static const u8 DATA_GPowerUseMsg[GPOWER_USE_MAX] = {
 msg_sys_gpower_use00,
 msg_sys_gpower_use01,
 msg_sys_gpower_use02,
};

static void taskAdd_WinGPower( BEACON_VIEW_PTR wk, GPOWER_ID g_power, u8 type, int* task_ct );
static void tcb_WinGPowerYesNo( GFL_TCBL *tcb , void* work);
static void tcb_WinGPowerCheck( GFL_TCBL *tcb , void* work);

static BOOL effReq_PopupMsgGPower( BEACON_VIEW_PTR wk, GAMEBEACON_INFO* info )
{
  GPOWER_ID gpower_id = GAMEBEACON_Get_GPowerID( info );

  //�܂����������p���[���������Ȃ�X���[����
  if( GPOWER_Check_OccurID( gpower_id, wk->gpower_data ) == gpower_id ){
    return FALSE;
  }
  GAMEBEACON_InfoWordset( info, wk->wordset, wk->tmpHeapID );
  WORDSET_ExpandStr( wk->wordset, wk->str_expand, wk->str_tmp );
  taskAdd_WinGPower( wk, gpower_id, GPOWER_USE_BEACON, &wk->eff_task_ct);
  return TRUE;
}
static void effReq_PopupMsgGPowerMine( BEACON_VIEW_PTR wk )
{
  taskAdd_WinGPower( wk,
      wk->my_data.power, GPOWER_USE_MINE, &wk->eff_task_ct );
}
static void effReq_PopupMsgGPowerSpecial( BEACON_VIEW_PTR wk, GPOWER_ID sp_gpower_id )
{
  taskAdd_WinGPower( wk, sp_gpower_id, GPOWER_USE_SPECIAL, &wk->eff_task_ct );
}


/*
 *  @brief  ���b�Z�[�W�E�B���h�E G�p���[�g�p�m�F�^�X�N�o�^
 */
static void taskAdd_WinGPower( BEACON_VIEW_PTR wk, GPOWER_ID g_power, u8 type, int* task_ct )
{
  GFL_TCBL* tcb;
  TASKWK_WIN_GPOWER* twk;

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_WinGPowerYesNo, sizeof(TASKWK_WIN_GPOWER), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_WIN_GPOWER ));

  wk->ctrl.g_power = GPOWER_ID_NULL;
  wk->ctrl.mine_power_f = ( type == GPOWER_USE_MINE);

  twk->bvp = wk;
  twk->wait = POPUP_WAIT;
  twk->g_power = g_power;
  twk->type = type;

  WORDSET_RegisterGPowerName( wk->wordset, 1, g_power );

  //�g�p�|�C���g�擾
  if( twk->type == GPOWER_USE_MINE ){
    twk->item_use = GPOWER_ID_to_Point( wk->gpower_data, g_power );
    twk->item_num = MYITEM_GetItemNum(wk->item_sv, ITEM_DERUDAMA, wk->tmpHeapID);
    
    WORDSET_RegisterNumber( wk->wordset, 2, twk->item_use, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( wk->wordset, 3, twk->item_num, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  
    if( GPOWER_Check_OccurID( g_power, wk->gpower_data) == g_power ){
      twk->mypower_use = GPOWER_Check_MyPower();
      if( twk->mypower_use > 0 ){
        twk->mypower_min = twk->mypower_use/60;
        twk->mypower_sec = twk->mypower_use%60;
        WORDSET_RegisterNumber( wk->wordset, 4, twk->mypower_min, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
        WORDSET_RegisterNumber( wk->wordset, 5, twk->mypower_sec, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      }
    }
  }
  if( twk->type != GPOWER_USE_MINE ){
    BeaconView_MenuBarViewSet( wk, MENU_POWER, MENU_ST_OFF );
  }
  BeaconView_MenuBarViewSet( wk, MENU_HELLO, MENU_ST_OFF );
  BeaconView_MenuBarViewSet( wk, MENU_THANKS, MENU_ST_OFF );
  BeaconView_MenuBarViewSet( wk, MENU_RETURN, MENU_ST_OFF );

  if( twk->mypower_use || (twk->item_num < twk->item_use )){
    GFL_TCBL_ChangeFunc( tcb, tcb_WinGPowerCheck );
  }
  twk->task_ct = task_ct;
  (*task_ct)++;
}

static void tcb_WinGPowerYesNo( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_WIN_GPOWER* twk = (TASKWK_WIN_GPOWER*)tcb_wk;
  BEACON_VIEW_PTR bvp = twk->bvp;

  if( twk->child_task > 0 ){
    return;
  }

  switch( twk->seq ){
  case 0:
    print_GetMsgToBuf( bvp, DATA_GPowerUseMsg[twk->type] );
    print_PopupWindow( bvp, bvp->str_expand, &twk->child_task );
    twk->seq++;
    return;
  case 1:
    taskAdd_MsgUpdown( bvp, SCROLL_UP, &twk->child_task );
    twk->seq++;
    return;
  case 2:
    BeaconView_PrintQueLimmitUpSet( bvp, TRUE );
    tmenu_Create( bvp, TMENU_ID_YESNO );
    draw_ClearMenuWindow( bvp );
    obj_MenuIconVisibleSet( bvp, FALSE );
    twk->seq++;
    return;
  case 3: //�I��҂�
    {
      int ret = tmenu_Update( bvp, TMENU_ID_YESNO );
      if( ret < 0 ){
        return;
      }
      twk->ret = ret;
      twk->seq++;
    }
    return;
  case 4: //����A�j���҂�
    if( !tmenu_YnEndWait( bvp, twk->ret )){
      return;
    }

    switch( twk->ret ){
    case TMENU_YN_CHECK:
      BEACON_VIEW_SUB_EventReserve( bvp, EV_GPOWER_CHECK );
      twk->seq = 3;
      return;
    case TMENU_YN_NO:
      twk->seq = 6;
      break;
    case TMENU_YN_YES:
      print_GetMsgToBuf( bvp, msg_sys_gpower_use_end );
      print_PopupWindow( bvp, bvp->str_expand, &twk->child_task );

      //�g��G�p���[���o���Ă���
      bvp->ctrl.g_power = twk->g_power;
      twk->seq++;
      break;
    }
    draw_MenuWindow( bvp, msg_sys_now_record );
    obj_MenuIconVisibleSet( bvp, TRUE );
    return;
  case 5:
    if( print_PopupWindowTimeWaitCheck( &twk->wait ) ){
      twk->seq++;
    }
    return;
  case 6:
    taskAdd_MsgUpdown( bvp, SCROLL_DOWN, &twk->child_task );
    twk->seq++;
    return;
  default:
    break;
  }
  BeaconView_MenuBarViewSet( bvp, MENU_ALL, MENU_ST_ON );
  BeaconView_PrintQueLimmitUpSet( bvp, FALSE );
  
  --(*twk->task_ct);
  GFL_TCBL_Delete(tcb);
}

static void tcb_WinGPowerCheck( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_WIN_GPOWER* twk = (TASKWK_WIN_GPOWER*)tcb_wk;
  BEACON_VIEW_PTR bvp = twk->bvp;

  if( twk->child_task > 0 ){
    return;
  }

  switch( twk->seq ){
  case 0:
    if( twk->mypower_use ){ //���Ɏg�p��
      print_GetMsgToBuf( bvp, msg_sys_gpower_use_err02+(twk->mypower_min != 0) );
    }else if( twk->item_num < twk->item_use ){  //�A�C�e��������Ȃ�
      print_GetMsgToBuf( bvp, msg_sys_gpower_use_err01);
    }
    print_PopupWindow( bvp, bvp->str_expand, &twk->child_task );
    twk->seq++;
    return;
  case 1:
    taskAdd_MsgUpdown( bvp, SCROLL_UP, &twk->child_task );
    twk->seq++;
    return;
  case 2:
    obj_MenuIconVisibleSet( bvp, FALSE );
    twk->seq++;
    return;
  case 3:
    tmenu_Create( bvp, TMENU_ID_CHECK );
    draw_ClearMenuWindow( bvp );
    twk->seq++;
    return;
  case 4: //�I��҂�
    {
      int ret = tmenu_Update( bvp, TMENU_ID_CHECK );
      if( ret < 0 ){
        return;
      }
      twk->ret = ret;
      twk->seq++;
    }
    return;
  case 5: //����A�j���҂�
    if( !tmenu_CheckEndWait( bvp, twk->ret )){
      return;
    }
    draw_MenuWindow( bvp, msg_sys_now_record );
    obj_MenuIconVisibleSet( bvp, TRUE );
    taskAdd_MsgUpdown( bvp, SCROLL_DOWN, &twk->child_task );
    twk->seq++;
    return;
  default:
    //�I��
    break;
  }
  bvp->gpower_check_req = ( twk->ret == TMENU_CHECK_CALL );
  BeaconView_MenuBarViewSet( twk->bvp, MENU_ALL, MENU_ST_ON );
  
  --(*twk->task_ct);
  GFL_TCBL_Delete(tcb);
}

/////////////////////////////////////////////////////////////////////
/*
 *  @brief  �p�l���A�C�R���\�� 
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_ICON_EFF{
  u8  seq;
  u8  wait;
  int child_task;

  PANEL_WORK* pp;
  GAMEBEACON_ACTION action;
  BEACON_VIEW_PTR bvp;
  int* task_ct;
}TASKWK_ICON_EFF;

static void tcb_IconEffect( GFL_TCBL *tcb , void* work);

/*
 *  @brief  ���b�Z�[�W�E�B���h�E�@�A�C�R���\���^�X�N�o�^
 */
static void taskAdd_IconEffect( BEACON_VIEW_PTR wk, PANEL_WORK* pp, GAMEBEACON_INFO* info, BOOL new_f )
{
  GFL_TCBL* tcb;
  TASKWK_ICON_EFF* twk;

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_IconEffect, sizeof(TASKWK_ICON_EFF), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_ICON_EFF ));

  twk->bvp = wk;
  twk->pp = pp;
  twk->wait = ICON_POP_TIME;
  twk->pp->tcb_icon = tcb;

  if( new_f ){
    twk->action = GAMEBEACON_ACTION_SEARCH;
  }else{
    twk->action = GAMEBEACON_Get_Action_ActionNo( info );
  }
  panel_IconObjUpdate( wk, pp, GAMEBEACON_GetActionDataIconType( twk->action ) );
  panel_IconVisibleSet( pp, !new_f );
}

static void tcb_IconEffect( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_ICON_EFF* twk = (TASKWK_ICON_EFF*)tcb_wk;

  switch( twk->seq ){
  case 0:
    if( twk->wait-- > 0 ){
      return;
    }
    switch( twk->action ){
    case GAMEBEACON_ACTION_SEARCH:
    case GAMEBEACON_ACTION_THANKYOU:
    case GAMEBEACON_ACTION_FREEWORD:
      panel_NamePrint( twk->bvp, twk->pp, TRUE, NULL );
      twk->seq++;
      return;
    }
    break;
  default:
    //�v�����g�҂�
    if( twk->pp->tcb_print != NULL ){
      return;
    }
    break;
  }
  twk->pp->tcb_icon = NULL;
  panel_IconVisibleSet( twk->pp, FALSE );
  GFL_TCBL_Delete(tcb);
}

/////////////////////////////////////////////////////////////////////
/*
 *  @brief  �p�l�� �_�ŕ\�� 
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_PANEL_FLASH{
  u8  ct;
  u8  wait;
  u8  interval;
  u8  pal_pos;
  int child_task;

  PANEL_WORK* pp;
  BEACON_VIEW_PTR bvp;
  int* task_ct;
}TASKWK_PANEL_FLASH;

static void taskAdd_PanelFlash( BEACON_VIEW_PTR wk, PANEL_WORK* pp, int* task_ct );
static void tcb_PanelFlash( GFL_TCBL *tcb , void* work);

static void effReq_SetPanelFlash( BEACON_VIEW_PTR wk, u8 target_ofs )
{
  PANEL_WORK* pp;
  u8 idx = GAMEBEACON_InfoTblRing_Ofs2Idx( wk->infoLog, target_ofs+wk->ctrl.view_top );
  
  pp = panel_GetPanelFromDataIndex( wk, idx );
  taskAdd_PanelFlash( wk, pp, &wk->eff_task_ct );
  sub_PlaySE( BVIEW_SE_DECIDE );
}

/*
 *  @brief  ���b�Z�[�W�E�B���h�E �p�l���t���b�V���^�X�N�o�^
 */
static void taskAdd_PanelFlash( BEACON_VIEW_PTR wk, PANEL_WORK* pp, int* task_ct )
{
  GFL_TCBL* tcb;
  TASKWK_PANEL_FLASH* twk;

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_PanelFlash, sizeof(TASKWK_PANEL_FLASH), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_PANEL_FLASH ));

  twk->bvp = wk;
  twk->pp = pp;
  twk->interval = 2;
  twk->pal_pos = (ACT_PAL_PANEL+twk->pp->id)*16;
  
  SoftFadePfd( twk->bvp->pfd, FADE_SUB_OBJ, twk->pal_pos, 16, 4, 0x7FFF );

  if( task_ct != NULL ){
    twk->task_ct = task_ct;
    (*twk->task_ct)++;
  }
}

static void tcb_PanelFlash( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_PANEL_FLASH* twk = (TASKWK_PANEL_FLASH*)tcb_wk;

  if( twk->ct < 2 ){
    if( twk->wait++ < twk->interval ){
      return;
    }
    SoftFadePfd( twk->bvp->pfd, FADE_SUB_OBJ, twk->pal_pos, 16, 4*(twk->ct%2), 0x7FFF );

    twk->ct++;
    twk->wait = 0;
    return;
  }
  SoftFadePfd( twk->bvp->pfd, FADE_SUB_OBJ, twk->pal_pos, 16, 0, 0x7FFF );
  if( twk->task_ct != NULL ){
    (*twk->task_ct)--;
  }
  GFL_TCBL_Delete(tcb);
}

/////////////////////////////////////////////////////////////////////
/*
 *  @brief  BMP�E�B���h�E���b�Z�[�W�\���L���[���� 
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_BMPWIN_PRINT{
  u8  seq;
  BMP_WIN* win;
  
  BEACON_VIEW_PTR bvp;
  int* task_ct;
}TASKWK_BMPWIN_PRINT;

static void taskAdd_BmpwinPrint( BEACON_VIEW_PTR wk, BMP_WIN* win, int* task_ct );
static void tcb_BmpwinPrint( GFL_TCBL *tcb , void* work);

static void printReq_BmpwinPrint( BEACON_VIEW_PTR wk, BMP_WIN* win, STRBUF* str, u8 x, u8 y, u8 clear, PRINTSYS_LSB color, int* task_ct )
{
  GFL_BMP_Clear( win->bmp, clear );
  if( wk->init_f ){ //���������̓_�C���N�g�v�����g
	  PRINTSYS_PrintColor( win->bmp, x, y, str, wk->fontHandle, color );
    GFL_BMPWIN_MakeTransWindow( win->win );
  }else{
    PRINT_UTIL_PrintColor( &win->putil, wk->printQue, x, y, str, wk->fontHandle, color );
    taskAdd_BmpwinPrint( wk, win, task_ct );
  }
}

/*
 *  @brief  ���b�Z�[�W�E�B���h�E �p�l���t���b�V���^�X�N�o�^
 */
static void taskAdd_BmpwinPrint( BEACON_VIEW_PTR wk, BMP_WIN* win, int* task_ct )
{
  GFL_TCBL* tcb;
  TASKWK_BMPWIN_PRINT* twk;

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_BmpwinPrint, sizeof(TASKWK_BMPWIN_PRINT), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_BMPWIN_PRINT ));

  twk->bvp = wk;
  twk->win = win;

  if( task_ct != NULL ){
    twk->task_ct = task_ct;
    (*twk->task_ct)++;
  }
}

static void tcb_BmpwinPrint( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_BMPWIN_PRINT* twk = (TASKWK_BMPWIN_PRINT*)tcb_wk;
  BEACON_VIEW_PTR bvp = twk->bvp;

  if( !PRINT_UTIL_Trans( &twk->win->putil, bvp->printQue )){
    return;
  }
  GFL_BMPWIN_MakeScreen( twk->win->win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( twk->win->win ) );

  if( twk->task_ct != NULL ){
    (*twk->task_ct)--;
  }
  GFL_TCBL_Delete(tcb);
}

/////////////////////////////////////////////////////////////////////
/*
 *  @brief  FontOam���b�Z�[�W�\���L���[���� 
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_FOAM_PRINT{
  PANEL_WORK* pp;
  FONT_OAM* foam;
   
  BEACON_VIEW_PTR bvp;
  int* task_ct;
}TASKWK_FOAM_PRINT;

static void taskAdd_FOamPrint( BEACON_VIEW_PTR wk, PANEL_WORK* pp, int* task_ct );
static void tcb_FOamPrint( GFL_TCBL *tcb , void* work);

static void printReq_FOamPrint( BEACON_VIEW_PTR wk, PANEL_WORK* pp, STRBUF* str, PRINTSYS_LSB color,int* task_ct )
{
  FONT_OAM* foam = &pp->msgOam;

  GFL_BMP_Clear( foam->bmp, 0 );
  if( wk->init_f ){ //���������̓_�C���N�g�v�����g
	  PRINTSYS_PrintColor( foam->bmp, 0, 0, str, wk->fontHandle, color );
	  BmpOam_ActorBmpTrans( foam->oam );
  }else{
    PRINTSYS_PrintQueColor( wk->printQue, foam->bmp, 0, 0, str, wk->fontHandle, color );
    taskAdd_FOamPrint( wk, pp, task_ct );
  }
}

/*
 *  @brief  �p�l�����b�Z�[�W�`��^�X�N�o�^
 */
static void taskAdd_FOamPrint( BEACON_VIEW_PTR wk, PANEL_WORK* pp, int* task_ct )
{
  GFL_TCBL* tcb;
  TASKWK_FOAM_PRINT* twk;

#ifdef PM_DEBUG
  GF_ASSERT( pp->tcb_print == NULL );
#endif

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_FOamPrint, sizeof(TASKWK_FOAM_PRINT), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_FOAM_PRINT ));

  twk->bvp = wk;
  twk->pp = pp;
  twk->foam = &pp->msgOam;
  twk->pp->tcb_print = tcb;

  if( task_ct != NULL ){
    twk->task_ct = task_ct;
    (*twk->task_ct)++;
  }
}

static void tcb_FOamPrint( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_FOAM_PRINT* twk = (TASKWK_FOAM_PRINT*)tcb_wk;
  BEACON_VIEW_PTR bvp = twk->bvp;

  if( PRINTSYS_QUE_IsExistTarget( bvp->printQue, twk->foam->bmp ) ){
    return;
  }
	BmpOam_ActorBmpTrans( twk->foam->oam );

  if( twk->task_ct != NULL ){
    (*twk->task_ct)--;
  }

  twk->pp->tcb_print = NULL;
  GFL_TCBL_Delete(tcb);
}

//============================================================================
///�f�o�b�O�֐�
//============================================================================
#ifdef PM_DEBUG

void DEBUG_BEACON_VIEW_SuretigaiCountSet( BEACON_VIEW_PTR wk, int value )
{
  wk->init_f = TRUE;  //�����񑦎����f�̂���

  //���݂̃g�[�^������Ⴂ�l���Z�b�g
  wk->log_count = value;
  draw_MenuWindow( wk, msg_sys_now_record );
  
  wk->init_f = FALSE;
}

void DEBUG_BEACON_VIEW_MemberListClear( BEACON_VIEW_PTR wk )
{
  int i;
  
  wk->init_f = TRUE;  //�����񑦎����f�̂���

  GAMEBEACON_InfoTbl_Clear( wk->infoLog );
  PRINTSYS_QUE_Clear( wk->printQue );

  for(i = 0;i < PANEL_MAX;i++){
    panel_Clear( &wk->panel[i] );
  }
  wk->ctrl.max = 0;
  wk->ctrl.view_top = 0;
  wk->ctrl.view_max = 0;
  wk->ctrl.target = 0;

  //���݂̃��O�����N���A
  draw_LogNumWindow( wk );
  obj_UpDownViewSet( wk );
  obj_ThanksViewSet( wk );

  wk->init_f = FALSE;
}


#endif  //PM_DEBUG

