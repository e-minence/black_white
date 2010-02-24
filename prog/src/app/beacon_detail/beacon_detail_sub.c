//=============================================================================
/**
 *
 *	@file		beacon_detail_sub.c
 *	@brief  ����Ⴂ�ڍ׉�� �T�u
 *	@author	Miyuki Iwasawa
 *	@data		2010.02.01
 *
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

// �ȈՉ�b�\���V�X�e��
#include "system/pms_draw.h"
#include "system/palanm.h"

//�e�N�X�`����OAM�ɓ]��
#include "system/nsbtx_to_clwk.h"

//FONT
#include "print/gf_font.h"
#include "font/font.naix"

//PRINT_QUE
#include "print/printsys.h"

#include "print/wordset.h"

//INFOWIN
#include "infowin/infowin.h"

//�`��ݒ�
#include "beacon_detail_gra.h"

//�Ȉ�CLWK�ǂݍ��݁��J�����[�e�B���e�B�[
#include "ui/ui_easy_clwk.h"

//�^�b�`�o�[
#include "ui/touchbar.h"

//�|�P�p���A�|�P����NO
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

//�A�[�J�C�u
#include "arc_def.h"

#include "field/zonedata.h"
//�O�����J
#include "app/beacon_detail.h"

#include "message.naix"
#include "msg/msg_beacon_detail.h"  // GMM
#include "beacon_status.naix"		// �^�b�`�o�[�J�X�^���{�^���p�T���v���Ƀ^�E���}�b�v���\�[�X
#include "townmap_gra.naix"		// �^�b�`�o�[�J�X�^���{�^���p�T���v���Ƀ^�E���}�b�v���\�[�X
#include "wifi_unionobj.naix"
#include "wmi.naix"

#include "beacon_detail_local.h"
#include "beacon_detail_sub.h"


#include "arc/wifi_unionobj_plt.cdat" //���j�I���L�����p���b�g�Q�ƃe�[�u��

//////////////////////////////////////////////////////////////
//�萔��`

typedef struct BEACON_WIN_COLOR{
 PRINTSYS_LSB color;
 u8           base;
}BEACON_WIN_COLOR;

static const BEACON_WIN_COLOR DATA_BeaconWinColor[] = {
  { FCOL_WIN01, FCOL_WIN_BASE1 }, 
  { FCOL_WIN02, FCOL_WIN_BASE2 }, 
};

#define FCOL_BEACON_BASE( n ) ( DATA_BeaconWinColor[n].base ) 
#define FCOL_BEACON_COL( n )  ( DATA_BeaconWinColor[n].color )

typedef struct _BEACON_DETAIL{
  u8  action;
  u8  msg_id;
  u8  icon;
}BEACON_DETAIL;

static const BEACON_DETAIL DATA_BeaconDetail[] = {
  { GAMEBEACON_ACTION_SEARCH, msg_beacon_000, 0 },
  { GAMEBEACON_ACTION_BATTLE_WILD_POKE_START, msg_beacon_001, 1 },
  { GAMEBEACON_ACTION_BATTLE_SP_POKE_START, msg_beacon_002, 1 },
  { GAMEBEACON_ACTION_BATTLE_TRAINER_START, msg_beacon_003, 2 },
  { GAMEBEACON_ACTION_BATTLE_LEADER_START, msg_beacon_004, 2 },
  { GAMEBEACON_ACTION_BATTLE_BIGFOUR_START, msg_beacon_005, 2 },
  { GAMEBEACON_ACTION_BATTLE_CHAMPION_START, msg_beacon_006, 2 },
};

#define BEACON_DETAIL_PARAM_NUM (NELEMS(DATA_BeaconDetail))

//////////////////////////////////////////////////////////////
//�v���g�^�C�v
static BOOL input_CheckHitTrIcon( BEACON_DETAIL_WORK* wk );

static void sub_PlaySE( u16 se_no );
static void sub_PlttVramTrans( u16* p_data, u8 target, u16 pos, u16 num );
static void sub_UpDownButtonActiveControl( BEACON_DETAIL_WORK* wk );
static u8 sub_WinFrameColorGet( GAMEBEACON_INFO* info );
static const BEACON_DETAIL* sub_GetBeaconDetailParam( GAMEBEACON_ACTION action );
static void sub_DetailWordset(const GAMEBEACON_INFO *info, WORDSET *wordset );

static void act_AnmStart( GFL_CLWK* act, u8 anm_no );
static void act_SetPosition( GFL_CLWK* act, s16 x, s16 y, u16 setsf );
static void print_GetMsgToBuf( BEACON_DETAIL_WORK* wk, u8 msg_id );


static void draw_UnionObjUpdate( BEACON_DETAIL_WORK* wk, u8 char_no );
static void draw_BeaconWindowIni( BEACON_DETAIL_WORK* wk );
static void draw_BeaconData( BEACON_DETAIL_WORK* wk, BMP_WIN* win, STRBUF* str, u8 px, u8 fx, u8 sx, u8 sy, u8 col_idx );
static void draw_BeaconWindowScroll( BEACON_WIN* bp, s16 py );
static void draw_BeaconWindowVisibleSet( BEACON_DETAIL_WORK* wk, u8 idx, BOOL visible_f );
static void draw_BeaconWindow( BEACON_DETAIL_WORK* wk, GAMEBEACON_INFO* info, u16 time, u8 idx );
static void draw_UpdateUnderView( BEACON_DETAIL_WORK* wk );

static void effReq_PopupMsg( BEACON_DETAIL_WORK* wk );
static void effReq_PageScroll( BEACON_DETAIL_WORK* wk, u8 dir );

/*
 *  @brief  �r�[�R���ڍ׉�ʏ����`��
 */
void BeaconDetail_InitialDraw( BEACON_DETAIL_WORK* wk )
{
  int i;

  draw_BeaconWindowIni( wk );

  GAMEBEACON_InfoTblRing_GetBeacon( wk->infoLog, wk->tmpInfo, &wk->tmpTime, wk->list[0]);
  draw_BeaconWindow( wk, wk->tmpInfo, wk->tmpTime, wk->flip_sw );
  draw_BeaconWindowVisibleSet( wk, wk->flip_sw, TRUE );
  draw_BeaconWindowVisibleSet( wk, wk->flip_sw^1, FALSE );
  draw_UpdateUnderView( wk );

  sub_UpDownButtonActiveControl( wk );

  GFL_NET_ReloadIcon();
}

/*
 *  @brief  �r�[�R���ڍ׉�ʁ@���͎擾
 */
int BeaconDetail_InputCheck( BEACON_DETAIL_WORK* wk )
{
  TOUCHBAR_ICON icon = TOUCHBAR_GetTrg( wk->touchbar );
	
  if( icon == TOUCHBAR_ICON_RETURN || icon == TOUCHBAR_ICON_CLOSE ){
    wk->param->ret_mode = ( icon == TOUCHBAR_ICON_CLOSE );
    return SEQ_FADEOUT;
  }
  if( icon == TOUCHBAR_ICON_CUR_U && wk->list_top > 0 ){
    effReq_PageScroll( wk, SCROLL_DOWN );
    return SEQ_EFF_WAIT;
  }
  if( icon == TOUCHBAR_ICON_CUR_D && wk->list_top < (wk->list_max-1) ){
    effReq_PageScroll( wk, SCROLL_UP );
    return SEQ_EFF_WAIT;
  }

  //�L�����A�C�R�������蔻��
  if( input_CheckHitTrIcon(wk) ){
    sub_PlaySE( BDETAIL_SE_DECIDE );
    effReq_PopupMsg( wk );
    return SEQ_EFF_WAIT; 
  }

  return SEQ_MAIN;
}

/*
 *  @brief  �L�����A�C�R�������蔻��
 */
static BOOL input_CheckHitTrIcon( BEACON_DETAIL_WORK* wk )
{
  u32 x,y;
  if( GFL_UI_TP_GetPointTrg( &x, &y ) )
  {
    if( ((u32)( x - (wk->icon_x-ACT_ICON_SXH)) <= (u32)(ACT_ICON_SX))
			&	((u32)( y - (wk->icon_y-ACT_ICON_SYH)) <= (u32)(ACT_ICON_SY)))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*
 *  @brief  SE�Đ�
 */
static void sub_PlaySE( u16 se_no )
{
  PMSND_PlaySEVolume( se_no, 127 );
}

/*
 *  @�p���b�g�]��
 */
static void sub_PlttVramTrans( u16* p_data, u8 target, u16 pos, u16 num  )
{
  u32 adrs = pos*2;
  u32 siz = num*2;

  DC_FlushRange( p_data, siz );

  switch( target ){
  case FADE_MAIN_BG:
    GX_LoadBGPltt( p_data, adrs, siz );
    break;
  case FADE_SUB_BG:
    GXS_LoadBGPltt( p_data, adrs, siz );
    break;
  case FADE_MAIN_OBJ:
    GX_LoadOBJPltt( p_data, adrs, siz );
    break;
  case FADE_SUB_OBJ:
    GXS_LoadOBJPltt( p_data, adrs, siz );
    break;
  }
}

/*
 *  @brief  UpDown�{�^���A�N�e�B�u�Z�b�g
 */
static void sub_UpDownButtonActiveControl( BEACON_DETAIL_WORK* wk )
{
  if( wk->list_top > 0 ){
    TOUCHBAR_SetActive( wk->touchbar,  TOUCHBAR_ICON_CUR_U, TRUE );
  }else{
    TOUCHBAR_SetActive( wk->touchbar,  TOUCHBAR_ICON_CUR_U, FALSE );
  }
  if( wk->list_top < (wk->list_max-1) ){
    TOUCHBAR_SetActive( wk->touchbar,  TOUCHBAR_ICON_CUR_D, TRUE );
  }else{
    TOUCHBAR_SetActive( wk->touchbar,  TOUCHBAR_ICON_CUR_D, FALSE );
  }
}

/*
 *  @brief  �E�B���h�E�p�l���J���[�Q�b�g 
 */
static u8 sub_WinFrameColorGet( GAMEBEACON_INFO* info )
{
  int version = GAMEBEACON_Get_PmVersion(info);

  switch( version ){
  case VERSION_BLACK:
    return 0;
  case VERSION_WHITE:
    return 1;
  }
  return 2;
}

/*
 *  @brief  �A�N�V�����i���o�[����ڍו`��^�C�v���擾
 */
static const BEACON_DETAIL* sub_GetBeaconDetailParam( GAMEBEACON_ACTION action )
{
  int i;

  for(i = 0;i < BEACON_DETAIL_PARAM_NUM; i++){
    if( action == DATA_BeaconDetail[i].action ){
      return &DATA_BeaconDetail[i];
    }
  }
  return &DATA_BeaconDetail[0];
}

//==================================================================
/**
 * �r�[�R���ڍ׏��̓��e��WORDSET����
 *
 * @param   info		        �Ώۂ̃r�[�R�����ւ̃|�C���^
 * @param   wordset		
 * @param   temp_heap_id		�����Ńe���|�����Ƃ��Ďg�p����q�[�vID
 */
//==================================================================
static void sub_DetailWordset(const GAMEBEACON_INFO *info, WORDSET *wordset )
{
  GAMEBEACON_DETAILS_NO details = GAMEBEACON_Get_Details_DetailsNo(info);

  switch( details ){
  case GAMEBEACON_DETAILS_NO_BATTLE_WILD_POKE:        ///<�쐶�|�P�����Ƒΐ풆
  case GAMEBEACON_DETAILS_NO_BATTLE_SP_POKE:          ///<���ʂȃ|�P�����Ƒΐ풆
    WORDSET_RegisterPokeMonsNameNo( wordset, 1,GAMEBEACON_Get_Details_BattleMonsNo(info));
    break;
  case GAMEBEACON_DETAILS_NO_BATTLE_TRAINER:
  case GAMEBEACON_DETAILS_NO_BATTLE_JIMLEADER:
  case GAMEBEACON_DETAILS_NO_BATTLE_BIGFOUR:
  case GAMEBEACON_DETAILS_NO_BATTLE_CHAMPION:
    WORDSET_RegisterTrainerName( wordset, 0, GAMEBEACON_Get_Details_BattleTrainer(info) );
    break;
  default:
    WORDSET_RegisterPlaceName( wordset, 0, ZONEDATA_GetPlaceNameID(GAMEBEACON_Get_ZoneID( info )) );
    break;
  }
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
 *  @brief  �A�N�^�[�@���W�Z�b�g
 */
static void act_SetPosition( GFL_CLWK* act, s16 x, s16 y, u16 setsf )
{
  GFL_CLACTPOS pos;
  pos.x = x;
  pos.y = y;
  
  GFL_CLACT_WK_SetPos( act, &pos, setsf );
}

/*
 *  @brief  �����񐮌`
 *
 *  �w�胁�b�Z�[�W�� wk->str_expand�ɂ߂�
 */
static void print_GetMsgToBuf( BEACON_DETAIL_WORK* wk, u8 msg_id )
{
  GFL_MSG_GetString( wk->msg, msg_id, wk->str_tmp);
  WORDSET_ExpandStr( wk->wset, wk->str_expand, wk->str_tmp );
}


/*
 *  @brief  Union�A�N�^�[�X�V
 */
static void draw_UnionObjUpdate( BEACON_DETAIL_WORK* wk, u8 char_no )
{
  u8 sex, p_ofs;
 
  //�L�����N�^�]��
  GFL_CLGRP_CGR_Replace(  wk->objResUnion.cgr,
                          wk->resCharUnion[ char_no ].p_char );
  //�p���b�g�]��
  p_ofs = 16*sc_wifi_unionobj_plt[char_no];
//  PaletteWorkSet( wk->pfd, &wk->resPlttUnion.dat[p_ofs], FADE_SUB_OBJ, (ACT_PAL_UNION+pp->id)*16, 0x20 );
  sub_PlttVramTrans( &wk->resPlttUnion.dat[p_ofs], FADE_MAIN_OBJ, PLTID_OBJ_UNION_M*16, 16 );
}


/*
 *  @brief  �r�[�R���E�B���h�E�����`��
 */
static void draw_BeaconWindowIni( BEACON_DETAIL_WORK* wk )
{
  int i,j;
  BEACON_WIN* bp;
  
  //�v���t�B�[�����o��
  for(i = 0;i < BEACON_PROF_MAX;i++){
    GFL_MSG_GetString( wk->msg, msg_prof01+i, wk->str_tmp );
  
    for(j = 0;j < BEACON_WIN_MAX;j++){
      bp = &wk->beacon_win[j];
  
      GFL_BMP_Clear( bp->prof[i].bmp, FCOL_BEACON_BASE(i%2) );

      PRINT_UTIL_PrintColor( &bp->prof[i].putil, wk->print_que,
          0, 0, wk->str_tmp, wk->font, FCOL_BEACON_COL(i%2) );
      GFL_BMPWIN_MakeTransWindow( bp->prof[i].win );
    }
  }
  //�o�g�n���o��
  GFL_MSG_GetString( wk->msg, msg_home_town, wk->str_tmp );
  for(j = 0;j < BEACON_WIN_MAX;j++){
    bp = &wk->beacon_win[j];

    GFL_BMP_Clear( bp->home[0].bmp, FCOL_BEACON_BASE(0) );

    PRINT_UTIL_PrintColor( &bp->home[0].putil, wk->print_que,
          0, 0, wk->str_tmp, wk->font, FCOL_BEACON_COL(0) );
    GFL_BMPWIN_MakeTransWindow( bp->home[0].win );
  }

  //���R�[�h���o��
  GFL_MSG_GetString( wk->msg, msg_record, wk->str_tmp);
  
  for(i = 0;i < BEACON_WIN_MAX;i++){
    bp = &wk->beacon_win[i];
    
    GFL_BMP_Clear( bp->record.bmp, FCOL_BEACON_BASE(0) );
    PRINT_UTIL_PrintColor( &bp->record.putil, wk->print_que,
          0, 0, wk->str_tmp, wk->font, FCOL_BEACON_COL(0) );
    GFL_BMPWIN_MakeTransWindow( bp->record.win );
  }
}

/*
 *  @brief  �r�[�R���E�B���h�E�`�� �T�u
 */
static void draw_BeaconData( BEACON_DETAIL_WORK* wk, BMP_WIN* win, STRBUF* str, u8 px, u8 fx, u8 sx, u8 sy, u8 col_idx)
{
  GFL_BMP_Fill( win->bmp, px*8, 0, sx*8, sy*8, FCOL_BEACON_BASE(col_idx) );
  PRINT_UTIL_PrintColor( &win->putil, wk->print_que,
      fx, 0, str, wk->font, FCOL_BEACON_COL(col_idx) );
  GFL_BMPWIN_MakeTransWindow( win->win );
}

/*
 *  @brief  �r�[�R���E�B���h�E �\������
 */
static void draw_BeaconWindowVisibleSet( BEACON_DETAIL_WORK* wk, u8 idx, BOOL visible_f )
{
  BEACON_WIN* bp = &wk->beacon_win[idx];

  GFL_CLACT_WK_SetDrawEnable( bp->cTrainer, visible_f );
  if(bp->rank > 0){
    GFL_CLACT_WK_SetDrawEnable( bp->cRank, visible_f );
  }
  PMS_DRAW_VisibleSet( wk->pms_draw, idx, visible_f );
	GFL_BG_SetVisible( bp->frame, visible_f );
}

/*
 *  @brief  �r�[�R���E�B���h�E�|�W�V�����Z�b�g
 */
static void draw_BeaconWindowScroll( BEACON_WIN* bp, s16 py )
{
  GFL_BG_SetScroll( bp->frame, GFL_BG_SCROLL_Y_SET, -py );
 
  act_SetPosition( bp->cTrainer, ACT_TRAINER_PX, ACT_TRAINER_PY+py, ACT_SF_SUB );
  act_SetPosition( bp->cRank, ACT_RANK_PX, ACT_RANK_PY+py, ACT_SF_SUB );
}

/*
 *  @brief  �r�[�R���E�B���h�E�`��
 */
static void draw_BeaconWindow( BEACON_DETAIL_WORK* wk, GAMEBEACON_INFO* info, u16 time, u8 idx )
{
  BEACON_WIN* bp = &wk->beacon_win[idx];

  //�p�l���x�[�X�J���[
  {
    u8  col_idx;
    u16 pal[2];
    GAMEBEACON_Get_FavoriteColor((GXRgb*)&pal[0], info);
    SoftFade(&pal[0], &pal[1], 1, 3, 0x0000); //��F���쐬
 
    sub_PlttVramTrans( pal, FADE_SUB_BG, 16*idx+0x01, 2);

    col_idx = sub_WinFrameColorGet( info );
    sub_PlttVramTrans( &wk->resPlttPanel.dat[3*col_idx], FADE_SUB_BG, 16*idx+0x0B, 1);
  }

  //�v���C���[��
  GAMEBEACON_Get_PlayerNameToBuf( info, wk->str_tmp );
  draw_BeaconData( wk, &bp->prof[0], wk->str_tmp,
      BMP_PROF01_DAT_PX, BMP_PROF01_DAT_PX*8, BMP_PROF01_DAT_SX, BMP_PROF01_DAT_SY, 0 );
  //�d��
  {
    u8 job = GAMEBEACON_Get_Job(info);
    if( job == GAMEBEACON_DETAILS_JOB_NULL ){
      GFL_STR_ClearBuffer( wk->str_expand );
    }else{
      WORDSET_RegisterJobName( wk->wset, 0, job);
    }
    print_GetMsgToBuf( wk, msg_prof_job );
    draw_BeaconData( wk, &bp->prof[1], wk->str_expand,
      BMP_PROF01_DAT_PX, BMP_PROF01_DAT_PX*8, BMP_PROF01_DAT_SX, BMP_PROF01_DAT_SY, 1 );
  }
  //�
  {
    u8 hobby = GAMEBEACON_Get_Hobby( info );
    if( hobby == GAMEBEACON_DETAILS_HOBBY_NULL ){
      GFL_STR_ClearBuffer( wk->str_expand );
    }else{
      WORDSET_RegisterJobName( wk->wset, 0, hobby );
    }
    print_GetMsgToBuf( wk, msg_prof_hobby );
    draw_BeaconData( wk, &bp->prof[2], wk->str_expand,
        BMP_PROF01_DAT_PX, BMP_PROF01_DAT_PX*8, BMP_PROF01_DAT_SX, BMP_PROF01_DAT_SY, 0 );
  }

  //�v���C����
  {
    u16 hour,min;
    GAMEBEACON_Get_PlayTime( info, &hour, &min );

    WORDSET_RegisterNumber( wk->wset, 2,
        hour, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( wk->wset, 3,
        min, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
  
    print_GetMsgToBuf( wk, msg_prof_ptime );
    draw_BeaconData( wk, &bp->prof[3], wk->str_expand,
        BMP_PROF02_DAT_PX, 0, BMP_PROF02_DAT_SX, BMP_PROF02_DAT_SY, 1);
  }
  //�o�g�n
  GFL_MSG_GetString( wk->msg, msg_home_town_word , wk->str_tmp);
  {
    u16 nation = GAMEBEACON_Get_Nation(info);
    WORDSET_RegisterCountryName( wk->wset, 0, nation);
    WORDSET_RegisterLocalPlaceName( wk->wset, 1, nation, GAMEBEACON_Get_Area(info));
    draw_BeaconData( wk, &bp->home[1], wk->str_tmp,
        0, 0, BMP_HOME02_SX, BMP_HOME02_SY, 0 );
  }
  //���R�[�h
  WORDSET_RegisterNumber( wk->wset, 0,
      GAMEBEACON_Get_ThanksRecvCount(info), 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
  WORDSET_RegisterNumber( wk->wset, 1,
      GAMEBEACON_Get_SuretigaiCount(info), 5, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
  
  print_GetMsgToBuf( wk, msg_record_num );
  draw_BeaconData( wk, &bp->record, wk->str_expand,
      BMP_RECORD_DAT_PX, 0, BMP_RECORD_DAT_SX, BMP_RECORD_DAT_SY, 0);

  //�ȈՉ�b
  {
    PMS_DATA pms;
   
    PMS_DRAW_Clear( wk->pms_draw, idx, FALSE );
    PMSDAT_SetDebugRandom( &pms );
    PMSDAT_SetDeco( &pms, 0, PMS_DECOID_HERO + idx );
    PMS_DRAW_Print( wk->pms_draw, bp->pms, &pms , idx );
  }
  //���j�I���L�����N�^�i���o�[���擾
  bp->union_char = GAMEBEACON_Get_TrainerView( info );

  //�g���[�i�[OBJ���\�[�X�ē]��
  TR2DGRA_OBJ_RES_Replace( wk->h_trgra, UnionView_GetTrType( bp->union_char),
      wk->objResTrainer[idx].cgr, wk->objResTrainer[idx].pltt, wk->tmpHeapID );

  //�����N
  {
    int rank = GAMEBEACON_Get_ResearchTeamRank(info);

    rank = 5;
    if( rank == 0){
      GFL_CLACT_WK_SetDrawEnable( bp->cRank, FALSE );
    }else{
      act_AnmStart( bp->cRank, ACTANM_RANK01+(rank-1) );
      GFL_CLACT_WK_SetDrawEnable( bp->cRank, TRUE );
    }
  }

}

/*
 *  @brief  ����ʃJ�����g�r�[�R�����v�����g
 */
static void draw_UpdateUnderView( BEACON_DETAIL_WORK* wk )
{
  u16 zone;
  GAMEBEACON_ACTION action;
  const BEACON_DETAIL* pd;

  GAMEBEACON_InfoTblRing_GetBeacon( wk->infoLog, wk->tmpInfo, &wk->tmpTime, wk->list[wk->list_top]);
  action = GAMEBEACON_Get_Action_ActionNo( wk->tmpInfo );
  pd = sub_GetBeaconDetailParam( action );

  draw_UnionObjUpdate( wk, GAMEBEACON_Get_TrainerView( wk->tmpInfo ) );
  zone = GAMEBEACON_Get_ZoneID( wk->tmpInfo );

	//�v���C���[�̏ꏊ����A�f�[�^�C���f�b�N�X���擾
  {
	  u16 dataIndex = TOWNMAP_DATA_SearchRootZoneID( wk->tmap, GAMEBEACON_Get_TownmapRootZoneID( wk->tmpInfo ) );

	  //�^�E���}�b�v��̍��W�擾
	  wk->icon_x = TOWNMAP_DATA_GetParam( wk->tmap, dataIndex, TOWNMAP_DATA_PARAM_POS_X );
	  wk->icon_y = TOWNMAP_DATA_GetParam( wk->tmap, dataIndex, TOWNMAP_DATA_PARAM_POS_Y );

    if( wk->icon_y < 16 ){
      wk->icon_y = 16;
    }else if( wk->icon_y > 152 ){
      wk->icon_y = 152;
    }
    if( wk->icon_x < 16){
      wk->icon_x = 16;
    }else if( wk->icon_x > 240 ){
      wk->icon_x = 240;
    }
    act_SetPosition( wk->pAct[ACT_ICON_TR], wk->icon_x, wk->icon_y, ACT_SF_MAIN );

    if( pd->icon != 0 ){
      act_SetPosition( wk->pAct[ACT_ICON_EV], wk->icon_x-24, wk->icon_y, ACT_SF_MAIN );
      act_AnmStart( wk->pAct[ACT_ICON_EV], ACTANM_ICON_TR + (pd->icon-1) );
    }
    GFL_CLACT_WK_SetDrawEnable( wk->pAct[ACT_ICON_EV], (pd->icon != 0) );
  }
  //�|�b�v�A�b�v���b�Z�[�W�`��
  GFL_BMP_Clear( wk->win_popup.bmp, FCOL_POPUP_BASE );
  
  //������������
  {
    u8 hour = (wk->tmpTime>>8);
    WORDSET_RegisterNumber( wk->wset, 0,
      hour%12, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( wk->wset, 1,
      (wk->tmpTime&0xFF), 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
  
    print_GetMsgToBuf( wk, msg_receive_time01+(hour/12) );
    PRINTSYS_PrintColor( wk->win_popup.bmp, 0, 0, wk->str_expand, wk->font, FCOL_POPUP );
  }
  //�r�[�R���ڍ׏��
  sub_DetailWordset( wk->tmpInfo, wk->wset );
  print_GetMsgToBuf( wk, pd->msg_id );
//  PRINT_UTIL_PrintColor( &wk->win_popup.putil, wk->print_que, 0, 0, wk->str_expand, wk->font, FCOL_POPUP );
  PRINTSYS_PrintColor( wk->win_popup.bmp, 0, 16, wk->str_expand, wk->font, FCOL_POPUP );

  GFL_BMPWIN_MakeTransWindow( wk->win_popup.win );
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

static void taskAdd_MsgUpdown( BEACON_DETAIL_WORK* wk, u8 dir, int* task_ct );
static void tcb_MsgUpdown( GFL_TCBL *tcb , void* work);

/*
 *  @brief  ���b�Z�[�W�E�B���h�E �A�b�v�_�E���^�X�N�o�^
 */
static void taskAdd_MsgUpdown( BEACON_DETAIL_WORK* wk, u8 dir, int* task_ct )
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

  sub_PlaySE( BDETAIL_SE_POPUP );

  twk->task_ct = task_ct;
  (*task_ct)++;
}

static void tcb_MsgUpdown( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_MSG_UPDOWN* twk = (TASKWK_MSG_UPDOWN*)tcb_wk;
 
  if( twk->ct++ < twk->frame ){
    twk->y += twk->diff;
    GFL_BG_SetScroll( BG_FRAME_POPUP_M, GFL_BG_SCROLL_Y_SET, twk->y );
    IWASAWA_Printf("Popup %d\n",twk->y);
    return;
  }

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
 
  BEACON_DETAIL_WORK* bdw;
  int* task_ct;
}TASKWK_WIN_POPUP;

static void taskAdd_WinPopup( BEACON_DETAIL_WORK* wk, int* task_ct );
static void tcb_WinPopup( GFL_TCBL *tcb , void* work);

static void effReq_PopupMsg( BEACON_DETAIL_WORK* wk )
{
  taskAdd_WinPopup( wk, &wk->eff_task_ct);
}

/*
 *  @brief  ���b�Z�[�W�E�B���h�E �|�b�v�A�b�v�^�X�N�o�^
 */
static void taskAdd_WinPopup( BEACON_DETAIL_WORK* wk, int* task_ct )
{
  GFL_TCBL* tcb;
  TASKWK_WIN_POPUP* twk;

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_WinPopup, sizeof(TASKWK_WIN_POPUP), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_WIN_POPUP ));

  twk->bdw = wk;
  twk->wait = POPUP_WAIT;

  TOUCHBAR_SetActiveAll( wk->touchbar, FALSE );

  twk->task_ct = task_ct;
  (*task_ct)++;
}

static void tcb_WinPopup( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_WIN_POPUP* twk = (TASKWK_WIN_POPUP*)tcb_wk;

  switch( twk->seq ){
  case 0:
    taskAdd_MsgUpdown( twk->bdw, SCROLL_UP, &twk->child_task );
    twk->seq++;
    return;
  case 1:
    if( twk->child_task ){
      return;
    }
    twk->seq++;
    return;
  case 2:
    if( twk->wait-- > 0 ){
      return;
    }
    taskAdd_MsgUpdown( twk->bdw, SCROLL_DOWN, &twk->child_task );
    twk->seq++;
    return;
  case 3:
    if( twk->child_task ){
      return;
    }
  }
  TOUCHBAR_SetActiveAll( twk->bdw->touchbar, TRUE );
  --(*twk->task_ct);
  GFL_TCBL_Delete(tcb);
}

/////////////////////////////////////////////////////////////////////
/*
 *  @brief  �r�[�R���E�B���h�E �X�N���[�� 
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_BWIN_SCROLL{
  u8  win_idx;
  u8  wait;
  u8  frame;
  s16 py;
  s16 dy;

  int child_task;
 
  BEACON_WIN* bp;
  BEACON_DETAIL_WORK* bdw;
  int* task_ct;
}TASKWK_BWIN_SCROLL;

static void taskAdd_BWinScroll( BEACON_DETAIL_WORK* wk, u8 win_idx, u8 spos, u8 dir, int* task_ct );
static void tcb_BWinScroll( GFL_TCBL *tcb , void* work);

static void effReq_BWinScroll( BEACON_DETAIL_WORK* wk, u8 msg_id )
{
  print_GetMsgToBuf( wk, msg_id );
//  taskAdd_BWinScroll( wk, wk->str_expand, &wk->eff_task_ct);
}

/*
 *  @brief  �r�[�R���E�B���h�E �X�N���[���^�X�N�o�^
 */
static void taskAdd_BWinScroll( BEACON_DETAIL_WORK* wk, u8 win_idx, u8 spos, u8 dir, int* task_ct )
{
  GFL_TCBL* tcb;
  TASKWK_BWIN_SCROLL* twk;

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_BWinScroll, sizeof(TASKWK_BWIN_SCROLL), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_BWIN_SCROLL ));

  twk->bp = &wk->beacon_win[win_idx];
  twk->win_idx = win_idx;
  twk->py = PAGE_SCROLL_PY+(spos*PAGE_SCROLL_H);

  if( dir == SCROLL_UP ){
    twk->dy -= PAGE_SCROLL_DY;
  }else{
    twk->dy += PAGE_SCROLL_DY;
  }
  twk->wait = PAGE_SCROLL_TIME;

  draw_BeaconWindowScroll( twk->bp, twk->py );

  if( spos != SCROLL_POS_DEF ){
    draw_BeaconWindowVisibleSet( wk, twk->win_idx, TRUE );
  }
  twk->bdw = wk;

  twk->task_ct = task_ct;
  (*task_ct)++;
}

static void tcb_BWinScroll( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_BWIN_SCROLL* twk = (TASKWK_BWIN_SCROLL*)tcb_wk;

  if( twk->wait-- > 0 ){
    twk->py += twk->dy;
    draw_BeaconWindowScroll( twk->bp, twk->py );
    return;
  }

  if( twk->py != 0 ){
    draw_BeaconWindowVisibleSet( twk->bdw, twk->win_idx, FALSE );
  }
  --(*twk->task_ct);
  GFL_TCBL_Delete(tcb);
}

/////////////////////////////////////////////////////////////////////
/*
 *  @brief  �r�[�R���E�B���h�E �X�N���[�� 
 */
/////////////////////////////////////////////////////////////////////
typedef struct _TASKWK_PAGE_SCROLL{
  u8  seq;
  u8  wait;
  u8  frame;
  u8  next;
  u8  dir;

  int child_task;
 
  BEACON_WIN* bp;
  BEACON_DETAIL_WORK* bdw;
  int* task_ct;
}TASKWK_PAGE_SCROLL;

static void taskAdd_PageScroll( BEACON_DETAIL_WORK* wk, u8 dir, int* task_ct );
static void tcb_PageScroll( GFL_TCBL *tcb , void* work);

static void effReq_PageScroll( BEACON_DETAIL_WORK* wk, u8 dir )
{
  taskAdd_PageScroll( wk, dir, &wk->eff_task_ct );
}

/*
 *  @brief  �r�[�R���E�B���h�E �X�N���[���^�X�N�o�^
 */
static void taskAdd_PageScroll( BEACON_DETAIL_WORK* wk, u8 dir, int* task_ct )
{
  GFL_TCBL* tcb;
  TASKWK_PAGE_SCROLL* twk;

  tcb = GFL_TCBL_Create( wk->pTcbSys, tcb_PageScroll, sizeof(TASKWK_PAGE_SCROLL), 0 );

  twk = GFL_TCBL_GetWork(tcb);
  MI_CpuClear8( twk, sizeof( TASKWK_PAGE_SCROLL ));

  twk->dir = dir;
  if( dir == SCROLL_UP ){
    twk->next = wk->list_top+1;
  }else{
    twk->next = wk->list_top-1;
  }
  //���ɕ\�������E�B���h�E��`��
  GAMEBEACON_InfoTblRing_GetBeacon( wk->infoLog, wk->tmpInfo, &wk->tmpTime, wk->list[twk->next]);
  draw_BeaconWindow( wk, wk->tmpInfo, wk->tmpTime, wk->flip_sw^1 );

  TOUCHBAR_SetActiveAll( wk->touchbar, FALSE );

  twk->bdw = wk;
  twk->task_ct = task_ct;
  (*task_ct)++;
}

static void tcb_PageScroll( GFL_TCBL *tcb , void* tcb_wk)
{
  TASKWK_PAGE_SCROLL* twk = (TASKWK_PAGE_SCROLL*)tcb_wk;
  static const u8 pos_tbl[] = { SCROLL_POS_DOWN, SCROLL_POS_UP };

  switch( twk->seq ){
  case 0:
    taskAdd_BWinScroll( twk->bdw, twk->bdw->flip_sw, SCROLL_POS_DEF, twk->dir, &twk->child_task );
    taskAdd_BWinScroll( twk->bdw, twk->bdw->flip_sw^1, pos_tbl[twk->dir], twk->dir, &twk->child_task );
    twk->seq++;
    return;
  case 1:
    if( twk->child_task ){
      return;
    }
    break;
  }
  twk->bdw->flip_sw ^= 1;
  twk->bdw->list_top = twk->next;
  draw_UpdateUnderView( twk->bdw );
  sub_UpDownButtonActiveControl( twk->bdw );
  TOUCHBAR_SetActiveAll( twk->bdw->touchbar, TRUE );

  --(*twk->task_ct);
  GFL_TCBL_Delete(tcb);
}

