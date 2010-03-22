//============================================================================================
/**
 * @file    pdwacc_disp.c
 * @bfief   PDWACC�摜�\���֘A
 * @author  k.ohno
 * @date    2009.11.14
 */
//============================================================================================

#include <gflib.h>
#include <dwc.h>
#include "calctool.h"

#include "arc_def.h"
#include "net_app/pdwacc.h"

#include "infowin/infowin.h"
#include "system/main.h"
#include "system/wipe.h"
#include "gamesystem/msgspeed.h" // MSGSPEED_GetWait

#include "message.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"
#include "pokeicon/pokeicon.h"
#include "system/laster.h"


#include "system/bmp_menuwork.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "ui/touchbar.h"

#include "../../field/event_ircbattle.h"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK

#include "pdwacc_local.h"
#include "msg/msg_gtsnego.h"
//#include "pdwacc.naix"
//#include "gsync.naix"  //@todo �f�ނ��ؗp
#include "wifi_login.naix"
#include "net_app/connect_anm.h"


#include "box_gra.naix"

#include "pdwacc_poke.cdat"
//#include "pdwacc_obj_NANR_LBLDEFS.h"


#define _OBJPLT_POKEICON_OFFSET (0)  //�|�P�����p���b�g
#define _OBJPLT_PDWACC_OFFSET (3)     //�Q�[���V���N�f��
#define _OBJPLT_HAND_OFFSET (6)    //��

FS_EXTERN_OVERLAY(ui_common);

typedef enum
{
  PLT_PDWACC,
  PLT_HANDOBJ,
  PLT_POKEICON,
  PLT_RESOURCE_MAX,
  CHAR_PDWACC = PLT_RESOURCE_MAX,
  CHAR_HANDOBJ,
  CHAR_SELECT_POKE,
  CHAR_RESOURCE_MAX,
  ANM_PDWACC = CHAR_RESOURCE_MAX,
  ANM_HANDOBJ,
  ANM_POKEICON,
  ANM_RESOURCE_MAX,
  CEL_RESOURCE_MAX,
} _CELL_RESOURCE_TYPE;


typedef enum
{
  CELL_CUR_BED,
  CELL_CUR_BED_ANI,
  CELL_CUR_KEMURI_R,
  CELL_CUR_KEMURI_L,
  CELL_CUR_RUG_ANI1 , 
  CELL_CUR_RUG_ANI2,
  CELL_CUR_RUG_ANI3,
  CELL_CUR_RUG_ANI4,
  CELL_CUR_ZZZ_ANI ,
  CELL_CUR_YUME_1  ,
  CELL_CUR_POKE_PLAYER,
  CELL_CUR_HAND,
  CELL_CUR_HUTON,
  
    _CELL_DISP_NUM,
} _CELL_WK_ENUM;

typedef enum  //���݂��̃Z���̕\������
{
  _ZZZ_CELL_PRI,
  _SMOKE_CELL_PRI,
  _FUTON_CELL_PRI,
  _POKEMON_CELL_PRI,
    _BED_CELL_PRI,

} _CELL_PRI_ENUM;




//#define _CELL_DISP_NUM (12)

struct _PDWACC_DISP_WORK {
	u32 subchar;
  u32 mainchar;
  CONNECT_BG_PALANM cbp;
  TOUCHBAR_WORK* pTouchWork;
  GFL_CLUNIT	*cellUnit;
  GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��
  GFL_TCB *p_hblank; //HBlank�n���h��

  u32 cellRes[CEL_RESOURCE_MAX];
  GFL_CLWK* curIcon[_CELL_DISP_NUM];
  HEAPID heapID;

//  s16 scroll_index;
  s16 scroll[192];

  fx32 blendCount;
  int blendStart;
};


static GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

  GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g

  GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K,

};


typedef struct {
  int x;
  int y;
  int pri;
} _PDWACCOBJPOS_STRUCT;




static _PDWACCOBJPOS_STRUCT pdwaccObjPosTable[]={
  {124,160,_BED_CELL_PRI},   //NANR_pdwacc_obj_bed    0 // 
  {124,160,_BED_CELL_PRI},   //NANR_pdwacc_obj_bed_ani    1 // 
  {132,155,_SMOKE_CELL_PRI},   //NANR_pdwacc_obj_kemuri_r    2 //
  {122,155,_SMOKE_CELL_PRI},   //NANR_pdwacc_obj_kemuri_l    3 //
  {124,125,_FUTON_CELL_PRI},   //NANR_pdwacc_obj_rug_ani1    4 //
  {124,125,_FUTON_CELL_PRI},   //NANR_pdwacc_obj_rug_ani2    5 //
  {124,125,_FUTON_CELL_PRI},   //NANR_pdwacc_obj_rug_ani3    5 //
  {124,125,_FUTON_CELL_PRI},   //NANR_pdwacc_obj_rug_ani4    5 //
  {100,125,_ZZZ_CELL_PRI},   //NANR_pdwacc_obj_zzz_ani    6 // 
  {128,160,_SMOKE_CELL_PRI},   //NANR_pdwacc_obj_yume_1    7 // 
};






static GFL_BG_SYS_HEADER BGsys_data = {
//    GX_DISPMODE_GRAPHICS,GX_BGMODE_3,GX_BGMODE_0,GX_BG0_AS_2D,
  GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
};

static void dispInit(PDWACC_DISP_WORK* pWork);
static void settingBgControl(PDWACC_DISP_WORK* pWork);
static void _TOUCHBAR_Init(PDWACC_DISP_WORK* pWork);
static void	_VBlank( GFL_TCB *tcb, void *work );
static void _SetHand(PDWACC_DISP_WORK* pWork,int x,int y);
static void _SetBed(PDWACC_DISP_WORK* pWork,int no);

static void _HandRelease(PDWACC_DISP_WORK* pWork);
static void _CreatePokeIconResource(PDWACC_DISP_WORK* pWork);
static void _blendSmoke(PDWACC_DISP_WORK* pWork);



PDWACC_DISP_WORK* PDWACC_DISP_Init(HEAPID id)
{
  PDWACC_DISP_WORK* pWork = GFL_HEAP_AllocClearMemory(id, sizeof(PDWACC_DISP_WORK));
  pWork->heapID = id;

  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));

  GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);

  GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
  GXS_DispOn();
  GX_DispOn();

  GFL_BG_Init(pWork->heapID);
  GFL_CLACT_SYS_Create(	&GFL_CLSYSINIT_DEF_CONSCREEN, &_defVBTbl, pWork->heapID );
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 40 , 0 , pWork->heapID );

  GFL_DISP_SetBank( &_defVBTbl );
  GFL_BG_SetBGMode( &BGsys_data );
  settingBgControl(pWork);
  dispInit(pWork);

  pWork->g3dVintr = GFUser_VIntr_CreateTCB( _VBlank, (void*)pWork, 0 );


  
  //_TOUCHBAR_Init(pWork);

  GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
  
  return pWork;
}

void PDWACC_DISP_Main(PDWACC_DISP_WORK* pWork)
{
  GFL_CLACT_SYS_Main();
  ConnectBGPalAnm_Main(&pWork->cbp);

}

void PDWACC_DISP_End(PDWACC_DISP_WORK* pWork)
{
  int i;
  _HandRelease(pWork);
  ConnectBGPalAnm_End(&pWork->cbp);

  for(i=0;i<PLT_RESOURCE_MAX;i++){
    if(pWork->cellRes[i] ){
      GFL_CLGRP_PLTT_Release(pWork->cellRes[i] );
    }
  }
  for(;i<CHAR_RESOURCE_MAX;i++){
    if(pWork->cellRes[i] ){
      GFL_CLGRP_CGR_Release(pWork->cellRes[i] );
    }
  }
  for(;i<ANM_RESOURCE_MAX;i++){
    if(pWork->cellRes[i] ){
      GFL_CLGRP_CELLANIM_Release(pWork->cellRes[i] );
    }
  }
  
  if(pWork->p_hblank){
    GFL_TCB_DeleteTask( pWork->p_hblank );
  }
  GFL_TCB_DeleteTask( pWork->g3dVintr );
  GFL_CLACT_UNIT_Delete(pWork->cellUnit);
  GFL_CLACT_SYS_Delete();

  GFL_BG_FillCharacterRelease( GFL_BG_FRAME1_M, 1, 0);
//  GFL_BG_FillCharacterRelease( GFL_BG_FRAME1_S, 1, 0);
  GFL_BG_FillCharacterRelease( GFL_BG_FRAME2_S, 1, 0);
  GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
  GFL_BG_FreeBGControl(GFL_BG_FRAME0_S);
  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
	GFL_BG_Exit();
  GFL_HEAP_FreeMemory(pWork);
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

}


static void settingBgControl(PDWACC_DISP_WORK* pWork)
{
  int frameno = GFL_BG_FRAME0_M;
  {
    int frame = frameno;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = frameno+1;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = frameno+2;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x10000, 0x8000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    //GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = frameno+3;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x18000, 0x8000,GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };
    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }

  frameno=GFL_BG_FRAME0_S;
  {
    int frame = frameno;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = frameno+1;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = frameno+2;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x10000, 0x8000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = frameno+3;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x18000, 0x8000,GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };
    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
}

//--------------------------------------------------------------
/**
 * G3D VBlank����
 * @param TCB GFL_TCB
 * @param work tcb work
 * @retval nothing
 */
//--------------------------------------------------------------
static void	_VBlank( GFL_TCB *tcb, void *work )
{
  PDWACC_DISP_WORK *pWork=work;

  GFL_CLACT_SYS_VBlankFunc();	//�Z���A�N�^�[VBlank

}

static void dispInit(PDWACC_DISP_WORK* pWork)
{
#if 0
	{
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_GSYNC, pWork->heapID );
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_gsync_gsync_bg_NCLR,
                                      PALTYPE_SUB_BG, 0, 0,  pWork->heapID);
    // �T�u���BG0�L�����]��
    pWork->subchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_gsync_gsync_bg_NCGR,
                                                                  GFL_BG_FRAME0_S, 0, 0, pWork->heapID);

    // �T�u���BG0�X�N���[���]��
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_gsync_downner_bg_NSCR,
                                              GFL_BG_FRAME0_S, 0,
                                              GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                              pWork->heapID);


    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_gsync_gsync_bg_NCLR,
                                      PALTYPE_MAIN_BG, 0, 0,  pWork->heapID);
    // �T�u���BG0�L�����]��
    pWork->mainchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_gsync_gsync_bg_NCGR,
                                                                  GFL_BG_FRAME0_M, 0, 0, pWork->heapID);

    // �T�u���BG0�X�N���[���]��
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_gsync_upper_bg_NSCR,
                                              GFL_BG_FRAME0_M, 0,
                                              GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainchar), 0, 0,
                                              pWork->heapID);

    GFL_ARC_CloseDataHandle(p_handle);
	}
#endif
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFI_LOGIN, pWork->heapID );

  //�����
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle , NARC_wifi_login_connect_win_NCLR , 
      PALTYPE_SUB_BG , 0 , 0 , pWork->heapID );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle , NARC_wifi_login_connect_win_NCGR ,
      GFL_BG_FRAME0_S , 0 , 0, FALSE , pWork->heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( p_handle , NARC_wifi_login_connect_win2_d_NSCR , 
      GFL_BG_FRAME0_S ,  0 , 0, FALSE , pWork->heapID );

  //����
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle , NARC_wifi_login_connect_win_NCLR , 
      PALTYPE_MAIN_BG , 0 , 0 , pWork->heapID );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle , NARC_wifi_login_connect_win_NCGR ,
      GFL_BG_FRAME0_M , 0 , 0, FALSE , pWork->heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( p_handle , NARC_wifi_login_connect_win1_u_NSCR , 
      GFL_BG_FRAME0_M,  0 , 0, FALSE , pWork->heapID );
  //����
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle , NARC_wifi_login_connect_win_NCGR ,
      GFL_BG_FRAME3_M , 0 , 0, FALSE , pWork->heapID );
  GFL_ARCHDL_UTIL_TransVramScreen( p_handle , NARC_wifi_login_connect_win2_u_NSCR , 
      GFL_BG_FRAME3_M ,  0 , 0, FALSE , pWork->heapID );

  ConnectBGPalAnm_InitBg( &pWork->cbp , p_handle , NARC_wifi_login_connect_ani_NCLR , pWork->heapID , GFL_BG_FRAME0_M , GFL_BG_FRAME3_M );

  GFL_ARC_CloseDataHandle( p_handle );
  
}


void PDWACC_DISP_HandInit(PDWACC_DISP_WORK* pWork)
{
  _SetHand(pWork,_POKEMON_CELLX, _POKEMON_CELLY);
}


void PDWACC_DISP_ObjInit(PDWACC_DISP_WORK* pWork,int no)
{
  _SetBed(pWork, no);
}


void PDWACC_DISP_ObjChange(PDWACC_DISP_WORK* pWork,int no,int no2)
{
  GFL_CLACT_WK_SetAnmSeq( pWork->curIcon[no] , no2 );
}

void PDWACC_DISP_ObjEnd(PDWACC_DISP_WORK* pWork,int no)
{
  GFL_CLACT_WK_Remove( pWork->curIcon[no] );
  pWork->curIcon[no]=NULL;
}



//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR������
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------

static void _TOUCHBAR_Init(PDWACC_DISP_WORK* pWork)
{
  //�A�C�R���̐ݒ�
  //�������
  TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
  {
    {
      TOUCHBAR_ICON_RETURN,
      {	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
    },
  };
  TOUCHBAR_SETUP	touchbar_setup;

  GFL_STD_MemClear( &touchbar_setup, sizeof(TOUCHBAR_SETUP) );

  touchbar_setup.p_item		= touchbar_icon_tbl;				//��̑����
  touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//�����������邩
  touchbar_setup.p_unit		= pWork->cellUnit;										//OBJ�ǂݍ��݂̂��߂�CLUNIT
  touchbar_setup.is_notload_bg = FALSE;
  touchbar_setup.bar_frm	= GFL_BG_FRAME1_S;						//BG�ǂݍ��݂̂��߂�BG�ʏ㉺��ʔ���ɂ��K�v
  touchbar_setup.bg_plt		= _TOUCHBAR_PAL;			//BG��گ�
  touchbar_setup.obj_plt	= _TOUCHBAR_PAL;			//OBJ��گ�
  touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;	//�}�b�s���O���[�h
  pWork->pTouchWork = TOUCHBAR_Init(&touchbar_setup, pWork->heapID);
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBARWORK������
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------


TOUCHBAR_WORK* PDWACC_DISP_GetTouchWork(PDWACC_DISP_WORK* pWork)
{
  return pWork->pTouchWork;
}


//----------------------------------------------------------------------------
/**
 *	@brief	���ݒ�
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------


static void _SetHand(PDWACC_DISP_WORK* pWork,int x,int y)
{
  int i=0;


  
  
  if(pWork->curIcon[CELL_CUR_HAND]==NULL){
    GFL_CLWK_DATA cellInitData;

    cellInitData.pos_x = x;
    cellInitData.pos_y = y;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 1;
    pWork->curIcon[CELL_CUR_HAND] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                         pWork->cellRes[CHAR_HANDOBJ],
                                                         pWork->cellRes[PLT_HANDOBJ],
                                                         pWork->cellRes[ANM_HANDOBJ],
                                                         &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_HAND] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_HAND], TRUE );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���ݒ�
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------


static void _SetBed(PDWACC_DISP_WORK* pWork,int no)
{
  GF_ASSERT(pWork->curIcon[no]==NULL);
  {
    GFL_CLWK_DATA cellInitData;

    cellInitData.pos_x = pdwaccObjPosTable[no].x;
    cellInitData.pos_y = pdwaccObjPosTable[no].y;
    cellInitData.anmseq = no;
    cellInitData.softpri = pdwaccObjPosTable[no].pri;
    cellInitData.bgpri = 1;
    pWork->curIcon[no] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                        pWork->cellRes[CHAR_PDWACC],
                                                        pWork->cellRes[PLT_PDWACC],
                                                        pWork->cellRes[ANM_PDWACC],
                                                        &cellInitData ,CLSYS_DRAW_MAIN , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[no] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[no], TRUE );
    GFL_CLACT_WK_SetAffineParam( pWork->curIcon[no], CLSYS_AFFINETYPE_DOUBLE );

  }
}


//----------------------------------------------------------------------------
/**
 *	@brief	���J��
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------

static void _HandRelease(PDWACC_DISP_WORK* pWork)
{
  int i=0;
   
  for(i = 0 ; i < _CELL_DISP_NUM ;i++){
    if(pWork->curIcon[i]!=NULL){
      GFL_CLACT_WK_Remove(pWork->curIcon[i]);
      pWork->curIcon[i]=NULL;
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R�����\�[�X�̍쐬
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------


static void _CreatePokeIconResource(PDWACC_DISP_WORK* pWork)
{
}



//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����A�C�R��Create
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------


void PDWACC_DISP_PokemonIconCreate(PDWACC_DISP_WORK* pWork, POKEMON_PASO_PARAM* ppp, int draw)
{
#if 0

  {
    GFL_CLWK_DATA cellInitData;
    ARCHANDLE *arcHandlePoke = GFL_ARC_OpenDataHandle( ARCID_POKEICON , pWork->heapID );

    pWork->cellRes[CHAR_SELECT_POKE] =
      GFL_CLGRP_CGR_Register( arcHandlePoke , POKEICON_GetCgxArcIndex(ppp) ,
                              FALSE , CLSYS_DRAW_MAX , pWork->heapID );

    cellInitData.pos_x = _POKEMON_CELLX;
    cellInitData.pos_y = _POKEMON_CELLY;
    cellInitData.anmseq = 0;
    cellInitData.softpri = _POKEMON_CELL_PRI;
    cellInitData.bgpri = 1;
    pWork->curIcon[CELL_CUR_POKE_PLAYER] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                                pWork->cellRes[CHAR_SELECT_POKE],
                                                                pWork->cellRes[PLT_POKEICON],
                                                                pWork->cellRes[ANM_POKEICON],
                                                                &cellInitData ,draw , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_POKE_PLAYER] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[CELL_CUR_POKE_PLAYER], TRUE );
    GFL_CLACT_WK_SetPlttOffs( pWork->curIcon[CELL_CUR_POKE_PLAYER] , POKEICON_GetPalNumGetByPPP( ppp ) , CLWK_PLTTOFFS_MODE_PLTT_TOP );

    GFL_ARC_CloseDataHandle(arcHandlePoke);
  }
#endif
}


void PDWACC_DISP_PokemonIconMove(PDWACC_DISP_WORK* pWork)
{
  GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_POKE_PLAYER] , TRUE );
}

void PDWACC_DISP_PokemonIconJump(PDWACC_DISP_WORK* pWork)
{
  GFL_CLACT_WK_SetAnmSeq( pWork->curIcon[CELL_CUR_POKE_PLAYER] , 1 );
  GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[CELL_CUR_POKE_PLAYER] , TRUE );
}




