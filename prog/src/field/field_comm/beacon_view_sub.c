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

#include "beacon_status.naix"
#include "wifi_unionobj.naix"
#include "beacon_view_local.h"

//////////////////////////////////////////////////////////////////

static BOOL list_IsView( BEACON_VIEW_PTR wk, u8 idx );

//////////////////////////////////////////////////////////////////
/*
 *  @brief  �X�^�b�N���烍�O���ꌏ���o��
 */
BOOL BeaconView_CheckStack( BEACON_VIEW_PTR wk )
{
  BOOL new_f = 0;
  int ofs,idx;
  PANEL_WORK* pp;

  //�X�^�b�N�ɐς܂ꂽ�����`�F�b�N
  if( !GAMEBEACON_Stack_GetInfo( wk->infoStack, wk->tmpInfo, &wk->tmpTime ) ){
    return FALSE;
  }
  ofs = GAMEBEACON_InfoTblRing_SetBeacon( wk->infoLog,
          wk->tmpInfo, &wk->tmpTime, &new_f );
  idx = GAMEBEACON_InfoTblRing_Ofs2Idx( ofs );

  //�o�^���X�V
  wk->ctrl.max += new_f;
  if( wk->ctrl.max <= PANEL_LINE_MAX){
    wk->ctrl.view_max = wk->ctrl.max;
  }
  //�|�b�v�A�b�v���N�G�X�g

  //�V�K�łȂ����̓X�N���[������
  if( !new_f ){
    pp = panel_GetPanelFromDataIndex( wk, idx );
    if(pp != NULL){ //�^�[�Q�b�g�p�l��������
      panel_Draw( wk, pp, info );
    }
    return TRUE;
  }

  //�󂫃p�l������
  pp = panel_GetPanelFromDataIndex( PANEL_DATA_BLANK );
  if( pp == NULL){
    return TRUE;  //����̂��߂̃`�F�b�N(�����������Ȃ�NULL�͂Ȃ�)
  }
  
  if( wk->ctrl.view_top > 0){ //�`�惊�X�g���g�b�v�łȂ����̓X�N���[���̂�
    ofs = wk->ctrl.view_top-1;
    GAMEBEACON_InfoTblRing_GetBeacon( wk->infoLog, &wk->tmpInfo, &wk->tmpTime, ofs );
    idx = GAMEBEACON_InfoTblRing_Ofs2Idx( ofs );
  }
  panel_Entry( pp, ofs, idx );  //�p�l���V�K�o�^
  panel_Draw( wk, pp, info );   //�p�l���`��
  
  //�X�N���[���p�^�[��
  if( wk->ctrl.view_top == 0){  //�X���C�h�C��
  
  }else{
  
  }

  //�X�N���[�����N�G�X�g
  return TRUE;
}

/*
 *  @�p���b�g�]��
 */
static sub_PlttVramTrans( u16* p_data, u16 pos, u16 num )
{
  u32 adrs = pos*2;
  u32 siz = num*2;

  DC_FlushRange( p_data, siz );
  GXS_LoadOBJPltt( p_data, adrs, siz );
}

/*
 *  @brief  �p�l�� �\�����On/Off
 */
static panel_VisibleSet( PANEL_WORK* pp, BOOL visible_f )
{
  BmpOam_ActorSetDrawEnable( pp->msgOam.oam, visible_f );
  GFL_CLACT_WK_SetDrawEnable( pp->cIcon, visible_f );
  GFL_CLACT_WK_SetDrawEnable( pp->cUnion, visible_f );
  GFL_CLACT_WK_SetDrawEnable( pp->cPanel, visible_f );
}

/*
 *  @brief  �p�l���N���A�[
 */
static void panel_Clear( PANEL_WORK* pp )
{
  panel_VisibleSet( pp, FALSE );

  GFL_BMP_Clear( pp->msgOam.bmp, 0 );
	BmpOam_ActorBmpTrans( pp->msgOam.oam );
  GFL_STR_ClearBuffer( pp->name );
  GFL_STR_ClearBuffer( pp->str );

  pp->data_idx = PANEL_DATA_BLANK;
}

/*
 *  @brief  �p�l���o�^
 */
static void panel_Entry( PANEL_WORK* pp, u8 data_ofs, u8 data_idx )
{
  panel_Clear( pp );
  pp->data_ofs = data_ofs;
  pp->data_idx = data_idx;
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
  u8 no, sex, char_no;
 
  no = GAMEBEACON_Get_TrainerView(info);
  sex = GAMEBEACON_Get_Sex(info);
 
  if(no > 8){
    no = 0;
  }
  if(sex > 1){
    sex = 0;
  }
  char_no = no+(sex*8);

  //�L�����N�^�]��
  GFL_CLGRP_CGR_Replace(  wk->objResUnion.res[ OBJ_RES_CGR ].tbl[ pp->id ],
                          wk->resCharUnion[ char_no ].p_char );
  //�p���b�g�]��
  sub_PlttVramTrans( wk->resPlttUnion.dat, (ACT_PAL_UNION+pp->id)*16, 16 );
}

/*
 *  @brief  �p�l���J���[�Q�b�g 
 */
static u8 panel_FrameColorGet( GAMEBEACON_INFO* info )
{
  u16 thanks = GAMEBEACON_Get_ThanksRecvCount( info );

  if( thanks < 11 ){
    return 0;
  }else if( thanks < 21 ){
    return 1;
  }else if( thanks < 41 ){
    return 2;
  }else if( thanks < 71 ){
    return 3;
  }
  return 4;
}

/*
 *  @brief  �p�l���J���[�Z�b�g
 */
static void panel_ColorPlttSet( BEACON_VIEW_PTR wk, PANEL_WORK* pp, GAMEBEACON_INFO* info )
{
  u16 pal[2];
  u8  col;

  //�p�l���x�[�X�J���[
  GAMEBEACON_Get_FavoriteColor((GXRgb*)&pal[0], info);

  //�p�l���t���[���J���[
  col = panel_FrameColorGet( info );
  pal[1] = wk->resPlttPanel.dat[ col ];
  
  sub_PlttVramTrans( pal, (ACT_PAL_PANEL+pp->id)*16+1, 2 );
}

/*
 *  @brief  �p�l���@������`��
 */
static void panel_MsgPrint( BEACON_VIEW_PTR wk, PANEL_WORK* pp, STRBUF* str )
{
  GFL_BMP_Clear( pp->msgOam.bmp, 1 );
	PRINTSYS_PrintColor( pp->msgOam.bmp, 0, 0, str, wk->fontHandle, FCOL_FNTOAM );
	BmpOam_ActorBmpTrans( pp->msgOam.oam );
}

/*
 *  @brief  �p�l���`��
 */
static void panel_Draw( BEACON_VIEW_PTR wk, PANEL_WORK* pp, GAMEBEACON_INFO* info )
{
  //�v���C���[���擾
  GAMEBEACON_Get_PlayerNameToBuf( info, pp->name );
  //���b�Z�[�W�o�b�t�@������
  GFL_STR_ClearBuffer( pp->str );
  //�v���C���[���]��
  panel_MsgPrint( wk, pp, pp->name );

  //UnionOBJ���\�[�X�X�V
  panel_UnionObjUpdate( wk, pp, info);

  //�p�l���J���[�]��
  panel_ColorPlttSet( wk, pp, info );
}


/*
 *  @brief  �w�肵��index�̃f�[�^���`�惉�C���ɂ��邩�ǂ����H
 */
static list_panel_IsView( BEACON_VIEW_PTR wk, u8 idx )
{
  int i;

  for( i = 0;i < )
  if(idx < wk->ctrl.view_top || idx >= wk->ctrl.view_btm ){
    return FALSE;
  }
  return TRUE;
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

