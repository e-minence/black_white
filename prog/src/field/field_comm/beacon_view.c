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
#include "gamesystem/game_beacon.h"
#include "infowin/infowin.h"
#include "app_menu_common.naix"
#include "arc_def.h"
#include "monolith.naix"
#include "font/font.naix"
#include "field/beacon_view.h"
#include "print\printsys.h"



//==============================================================================
//  �萔��`
//==============================================================================
///�t�H���g�p���b�g�W�J�ʒu
#define FONT_PAL    (14)

///�\�����郍�O����
#define VIEW_LOG_MAX    (3)

//==============================================================================
//  �\���̒�`
//==============================================================================
///����Ⴂ�ʐM��ԎQ�Ɖ�ʊǗ����[�N
typedef struct _BEACON_VIEW{
  GAMESYS_WORK *gsys;
  GFL_BMPWIN *win[VIEW_LOG_MAX];
  PRINT_UTIL print_util[VIEW_LOG_MAX];
}BEACON_VIEW;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
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
BEACON_VIEW_PTR BEACON_VIEW_Init(GAMESYS_WORK *gsys)
{
  BEACON_VIEW_PTR view;
  ARCHANDLE *handle;
  
  view = GFL_HEAP_AllocClearMemory(HEAPID_FIELDMAP, sizeof(BEACON_VIEW));
  view->gsys = gsys;
  
  
  handle = GFL_ARC_OpenDataHandle(ARCID_MONOLITH, HEAPID_FIELDMAP);
  
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
  int i;
  
  for(i = 0; i < VIEW_LOG_MAX; i++){
    info = GAMEBEACON_Get_BeaconLog(i);
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
  ;
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
  ;
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
    handle, NARC_monolith_mono_bgd_lz_NCGR, GFL_BG_FRAME3_S, 0, 
    0, TRUE, HEAPID_FIELDMAP);
  
  //�X�N���[���]��
  GFL_ARCHDL_UTIL_TransVramScreen(
    handle, NARC_monolith_mono_bgd_base_lz_NSCR, GFL_BG_FRAME3_S, 0, 
    0x800, TRUE, HEAPID_FIELDMAP);
  GFL_BG_ClearScreen(GFL_BG_FRAME2_S);
  
  //�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette(handle, NARC_monolith_mono_bgd_NCLR, PALTYPE_SUB_BG, 0, 
    0x20 * 3, HEAPID_FIELDMAP);
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
    view->win[i] = GFL_BMPWIN_Create( GFL_BG_FRAME2_S, 2, 2 + 2*i, 28, 2, 
      FONT_PAL, GFL_BMP_CHRAREA_GET_B );
    PRINT_UTIL_Setup( &view->print_util[i], view->win[i] );
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
