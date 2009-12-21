//==============================================================================
/**
 * @file    beacon_view.c
 * @brief   ����Ⴂ�ʐM��ԎQ�Ɖ��
 * @author  matsuda
 * @date    2009.12.13(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "field/field_subscreen.h"
#include "gamesystem/game_beacon.h"
#include "infowin/infowin.h"
#include "app_menu_common.naix"
#include "arc_def.h"
#include "beacon_status.naix"
#include "font/font.naix"
#include "field/beacon_view.h"
#include "print\printsys.h"
#include "message.naix"


//==============================================================================
//  �萔��`
//==============================================================================
///�t�H���g�p���b�g�W�J�ʒu
#define FONT_PAL    (14)

///�\�����郍�O����
#define VIEW_LOG_MAX    (4)

//==============================================================================
//  �\���̒�`
//==============================================================================
///����Ⴂ�ʐM��ԎQ�Ɖ�ʊǗ����[�N
typedef struct _BEACON_VIEW{
  GAMESYS_WORK *gsys;
  FIELD_SUBSCREEN_WORK *subscreen;
  GFL_FONT *fontHandle;
  PRINT_QUE *printQue;
  WORDSET *wordset;
  GFL_MSGDATA *mm_status;
  STRBUF *strbuf_temp;
  STRBUF *strbuf_expand;
  
  GFL_BMPWIN *win[VIEW_LOG_MAX];
  PRINT_UTIL print_util[VIEW_LOG_MAX];
  
  u32 log_count;
}BEACON_VIEW;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void BEACON_VIEW_TouchUpdata(BEACON_VIEW_PTR view);
static void _BeaconView_SystemSetup(BEACON_VIEW_PTR view);
static void _BeaconView_SystemExit(BEACON_VIEW_PTR view);
static void _BeaconView_BGLoad(BEACON_VIEW_PTR view, ARCHANDLE *handle);
static void _BeaconView_BGUnload(BEACON_VIEW_PTR view);
static void _BeaconView_ActorResouceLoad(BEACON_VIEW_PTR view, ARCHANDLE *handle);
static void _BeaconView_ActorResourceUnload(BEACON_VIEW_PTR view);
static void _BeaconView_ActorCreate(BEACON_VIEW_PTR view, ARCHANDLE *handle);
static void _BeaconView_ActorDelete(BEACON_VIEW_PTR view);
static void _BeaconView_BmpWinCreate(BEACON_VIEW_PTR view);
static void _BeaconView_BmpWinDelete(BEACON_VIEW_PTR view);


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
  BEACON_VIEW_PTR view;
  ARCHANDLE *handle;
  
  view = GFL_HEAP_AllocClearMemory(HEAPID_FIELDMAP, sizeof(BEACON_VIEW));
  view->gsys = gsys;
  view->subscreen = subscreen;
  
  
  handle = GFL_ARC_OpenDataHandle(ARCID_BEACON_STATUS, HEAPID_FIELDMAP);
  
  _BeaconView_SystemSetup(view);
  _BeaconView_BGLoad(view, handle);
  _BeaconView_ActorResouceLoad(view, handle);
  _BeaconView_ActorCreate(view, handle);
  _BeaconView_BmpWinCreate(view);
  
  GFL_ARC_CloseDataHandle(handle);

  return view;
}

//==================================================================
/**
 * ����Ⴂ�Q�Ɖ�ʁF�j���ݒ�
 *
 * @param   view		
 */
//==================================================================
void BEACON_VIEW_Exit(BEACON_VIEW_PTR view)
{
  _BeaconView_BmpWinDelete(view);
  _BeaconView_ActorDelete(view);
  _BeaconView_ActorResourceUnload(view);
  _BeaconView_BGUnload(view);
  _BeaconView_SystemExit(view);

  GFL_HEAP_FreeMemory(view);
}

//==================================================================
/**
 * ����Ⴂ�Q�Ɖ�ʁF�X�V
 *
 * @param   view		
 */
//==================================================================
void BEACON_VIEW_Update(BEACON_VIEW_PTR view)
{
  const GAMEBEACON_INFO *info;
  u32 old_log_count;
  s32 new_log_num, copy_src, copy_dest, write_start;
  int i;
  
  BEACON_VIEW_TouchUpdata(view);

  PRINTSYS_QUE_Main(view->printQue);
  for(i = 0; i < VIEW_LOG_MAX; i++){
    PRINT_UTIL_Trans( &view->print_util[i], view->printQue );
  }
  
  //BMP�`��
  {
    old_log_count = view->log_count;
    view->log_count = GAMEBEACON_Get_LogCount();
    new_log_num = view->log_count - old_log_count;  //�X�V���ꂽ���O����
    
    if(new_log_num > 0){
      copy_dest = 0;
      for(copy_src = new_log_num; copy_src < VIEW_LOG_MAX; copy_src++){
        GFL_BMP_DATA *bmp_src, *bmp_dest;
        
        bmp_src = GFL_BMPWIN_GetBmp(view->win[copy_src]);
        bmp_dest = GFL_BMPWIN_GetBmp(view->win[copy_dest]);
        GFL_BMP_Copy( bmp_src, bmp_dest );
        GFL_BMPWIN_TransVramCharacter( view->win[copy_dest] );
        copy_dest++;
      }
      
      write_start = new_log_num - 1;
      if(write_start >= VIEW_LOG_MAX){
        write_start = VIEW_LOG_MAX - 1;
      }
      for( ; copy_dest < VIEW_LOG_MAX; copy_dest++){
        info = GAMEBEACON_Get_BeaconLog(write_start);
        if(info != NULL){
          GFL_BMP_Clear( GFL_BMPWIN_GetBmp(view->win[copy_dest]), 0x00 );
          GFL_MSG_GetString(view->mm_status, GAMEBEACON_GetMsgID(info), view->strbuf_temp);
          GAMEBEACON_Wordset(info, view->wordset, HEAPID_FIELDMAP);
          WORDSET_ExpandStr( view->wordset, view->strbuf_expand, view->strbuf_temp );
          PRINT_UTIL_PrintColor( &view->print_util[copy_dest], view->printQue, 0, 0, 
            view->strbuf_expand, view->fontHandle, PRINTSYS_LSB_Make( 15, 2, 0 ) );
        }
        write_start--;
        if(write_start < 0){
          break;
        }
      }
    }
  }
}

//==================================================================
/**
 * ����Ⴂ�Q�Ɖ�ʁF�`��
 *
 * @param   view		
 */
//==================================================================
void BEACON_VIEW_Draw(BEACON_VIEW_PTR view)
{
  ;
}

//--------------------------------------------------------------
/**
 * �^�b�`����
 *
 * @param   view		
 */
//--------------------------------------------------------------
static void BEACON_VIEW_TouchUpdata(BEACON_VIEW_PTR view)
{
  u32 tp_x, tp_y;
  
  int tp_ret = GFL_UI_TP_HitTrg(TouchRect);
  
  switch(tp_ret){
  case 0: //G�p���[
    OS_TPrintf("G�p���[ �r�[�R���Z�b�g\n");
    GAMEBEACON_Set_EncountDown();
    break;
  case 1: //���߂łƂ�
    OS_TPrintf("���߂łƂ� �r�[�R���Z�b�g\n");
    GAMEBEACON_Set_Congratulations();
    break;
  case 2: //�߂�
    FIELD_SUBSCREEN_SetAction(view->subscreen , FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_CGEAR);
    break;
  }
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * �V�X�e���֘A�̃Z�b�g�A�b�v
 *
 * @param   view		
 */
//--------------------------------------------------------------
static void _BeaconView_SystemSetup(BEACON_VIEW_PTR view)
{
	view->printQue = PRINTSYS_QUE_Create( HEAPID_FIELDMAP );
	view->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_FIELDMAP );
  view->wordset = WORDSET_Create(HEAPID_FIELDMAP);

	view->mm_status = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_beacon_status_dat, HEAPID_FIELDMAP);

  view->strbuf_temp = GFL_STR_CreateBuffer(128, HEAPID_FIELDMAP);
  view->strbuf_expand = GFL_STR_CreateBuffer(128, HEAPID_FIELDMAP);
}

//--------------------------------------------------------------
/**
 * �V�X�e���֘A�̔j��
 *
 * @param   view		
 */
//--------------------------------------------------------------
static void _BeaconView_SystemExit(BEACON_VIEW_PTR view)
{
  GFL_STR_DeleteBuffer(view->strbuf_temp);
  GFL_STR_DeleteBuffer(view->strbuf_expand);

  GFL_MSG_Delete(view->mm_status);
  
	WORDSET_Delete(view->wordset);
  GFL_FONT_Delete(view->fontHandle);
  PRINTSYS_QUE_Delete(view->printQue);
}

//--------------------------------------------------------------
/**
 * BG���[�h
 *
 * @param   view		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _BeaconView_BGLoad(BEACON_VIEW_PTR view, ARCHANDLE *handle)
{
	static const GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
  	{//GFL_BG_FRAME2_S
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, 0x7000,GX_BG_EXTPLTT_01,
  		3, 0, 0, FALSE
  	},
  	{//GFL_BG_FRAME3_S
  		0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  		GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, 0x7000,GX_BG_EXTPLTT_01,
  		3, 0, 0, FALSE
  	},
	};
	GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &TextBgCntDat[1], GFL_BG_MODE_TEXT );

	//BG VRAM�N���A
	GFL_STD_MemClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);

	//���W�X�^OFF
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2S_BlendNone();
	
  //�L�����]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    handle, NARC_beacon_status_beacon_status_lz_NCGR, GFL_BG_FRAME3_S, 0, 
    0, TRUE, HEAPID_FIELDMAP);
  
  //�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreen(
    handle, NARC_beacon_status_beacon_status_lz_NSCR, GFL_BG_FRAME3_S, 0, 
    0x800, TRUE, HEAPID_FIELDMAP);
  GFL_BG_ClearScreen(GFL_BG_FRAME2_S);
  
  //�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette(handle, NARC_beacon_status_beacon_status_NCLR, 
    PALTYPE_SUB_BG, 0, 0x20 * 4, HEAPID_FIELDMAP);
  //�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
    0x20*FONT_PAL, 0x20, HEAPID_FIELDMAP);

	GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
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
static void _BeaconView_BGUnload(BEACON_VIEW_PTR view)
{
	GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_OFF );
	GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
	GFL_BG_FreeBGControl(GFL_BG_FRAME2_S);
	GFL_BG_FreeBGControl(GFL_BG_FRAME3_S);
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�Ŏg�p���郊�\�[�X�̃��[�h
 *
 * @param   view		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _BeaconView_ActorResouceLoad(BEACON_VIEW_PTR view, ARCHANDLE *handle)
{
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�Ŏg�p����A�N�^�[�̃A�����[�h
 *
 * @param   view		
 */
//--------------------------------------------------------------
static void _BeaconView_ActorResourceUnload(BEACON_VIEW_PTR view)
{
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�쐬
 *
 * @param   view		
 * @param   handle		
 */
//--------------------------------------------------------------
static void _BeaconView_ActorCreate(BEACON_VIEW_PTR view, ARCHANDLE *handle)
{
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�폜
 *
 * @param   view		
 */
//--------------------------------------------------------------
static void _BeaconView_ActorDelete(BEACON_VIEW_PTR view)
{
}

//--------------------------------------------------------------
/**
 * BMPWIN�쐬
 *
 * @param   view		
 */
//--------------------------------------------------------------
static void _BeaconView_BmpWinCreate(BEACON_VIEW_PTR view)
{
  int i;
  
  for(i = 0; i < VIEW_LOG_MAX; i++){
    view->win[i] = GFL_BMPWIN_Create( GFL_BG_FRAME2_S, 2, 2 + 5*i, 28, 4, 
      FONT_PAL, GFL_BMP_CHRAREA_GET_B );
    PRINT_UTIL_Setup( &view->print_util[i], view->win[i] );
    GFL_BMPWIN_MakeTransWindow(view->win[i]);
  }
}

//--------------------------------------------------------------
/**
 * BMPWIN�폜
 *
 * @param   view		
 */
//--------------------------------------------------------------
static void _BeaconView_BmpWinDelete(BEACON_VIEW_PTR view)
{
  int i;
  
  for(i = 0; i < VIEW_LOG_MAX; i++){
    GFL_BMPWIN_Delete(view->win[i]);
  }
}
