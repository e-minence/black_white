//============================================================================================
/**
 * @file	  itemmenu_disp.c
 * @brief	  �A�C�e�����j���[�̕`�����
 * @author	k.ohno
 * @date	  2009.07.19
 */
//============================================================================================


#include <gflib.h>
#include "net/network_define.h"
#include "arc_def.h"

#include "item/item.h"

#include "app/app_menu_common.h"
#include "app_menu_common.naix"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"
#include "system/bmp_winframe.h"

#include "gamesystem/msgspeed.h"

#include "message.naix"
#include "bag.naix"
#include "item_icon.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_bag.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "field/fieldmap.h"
#include "font/font.naix" //NARC_font_large_nftr
#include "sound/pm_sndsys.h"
#include "system/wipe.h"
#include "waza_tool/waza_tool.h"
#include "waza_tool/wazadata.h"
#include "itemmenu.h"
#include "itemmenu_local.h"
#include "app/itemuse.h"
#include "savedata/mystatus.h"
#include "itemmenu_local.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "system/main.h"			//GFL_HEAPID_APP�Q��



#define ITEMREPORT_FRAME (GFL_BG_FRAME2_S)
#define _UP_ITEMNAME_INITX (5)
#define _UP_ITEMNAME_INITY (6)
#define _UP_ITEMNAME_SIZEX (14)
#define _UP_ITEMNAME_SIZEY (3)


#define _UP_ITEMNUM_INITX (23)
#define _UP_ITEMNUM_INITY (6)
#define _UP_ITEMNUM_SIZEX (6)
#define _UP_ITEMNUM_SIZEY (3)

#define _UP_ITEMREPORT_INITX (2)
#define _UP_ITEMREPORT_INITY (12)
#define _UP_ITEMREPORT_SIZEX (26)
#define _UP_ITEMREPORT_SIZEY (9)

#define _ITEMICON_SCR_X (14)
#define _ITEMICON_SCR_Y (8)

#define ITEM_LIST_NUM (8)



#define _BAR_CELL_CURSOR_EXIT (200-8)    //EXIT x�{�^��
#define _BAR_CELL_CURSOR_RETURN (232-8) //RETURN Enter�{�^��


typedef enum{
  _CLACT_PLT,
  _CLACT_CHR,
  _CLACT_ANM,
} _BAG_CLACT_TYPE;


static void _itemiconAnim(FIELD_ITEMMENU_WORK* pWork,int itemid);
static void ITEMDISP_InitTaskBar( FIELD_ITEMMENU_WORK* pWork );


//------------------------------------------------------------------------------
/**
 * @brief   �G�̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------

void _createSubBg(void)
{
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

  // �w�i��
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000,
      0x8000,
      GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME0_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );
    GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_OFF );

  }
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000,
      0x8000,
      GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME1_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );
    GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_OFF );

  }
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000,
      0x8000,
      GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME2_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
    GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_OFF );

  }
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x18000,
      0x8000,
      GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_FillCharacter( GFL_BG_FRAME3_M, 0x00, 1, 0 );
    GFL_BG_ClearFrame( GFL_BG_FRAME3_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME3_M );
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_OFF );

  }


  {
    int frame = GFL_BG_FRAME0_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_OFF );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME1_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_OFF );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME2_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_OFF );
    GFL_BG_LoadScreenReq( frame );
  }
}

#define FIELD_CLSYS_RESOUCE_MAX		(100)


//--------------------------------------------------------------
///	�Z���A�N�^�[�@�������f�[�^
//--------------------------------------------------------------
static const GFL_CLSYS_INIT fldmapdata_CLSYS_Init =
{
  0, 0,
  0, 512,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL, //�ʐM�A�C�R������
  0,
  FIELD_CLSYS_RESOUCE_MAX,
  FIELD_CLSYS_RESOUCE_MAX,
  FIELD_CLSYS_RESOUCE_MAX,
  FIELD_CLSYS_RESOUCE_MAX,
  16, 16,
};

static GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_32K,
};




void ITEMDISP_SetVisible(void)
{
  GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );
}


//------------------------------------------------------------------------------
/**
 * @brief   �G�̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_graphicInit(FIELD_ITEMMENU_WORK* pWork)
{
  G2_BlendNone();

  GFL_BG_Init(pWork->heapID);
  GFL_BMPWIN_Init(pWork->heapID);
  GFL_FONTSYS_Init();
  GFL_DISP_SetBank( &_defVBTbl );
  GFL_CLACT_SYS_Create(	&fldmapdata_CLSYS_Init, &_defVBTbl, pWork->heapID );

  {
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }
  GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);
  _createSubBg();


  GFL_FONTSYS_SetDefaultColor();
  pWork->SysMsgQue = PRINTSYS_QUE_Create( pWork->heapID );


  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME3_M,
                                                _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);


  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_BAG, pWork->heapID );

    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_bag_bag_basebg_u_NCLR,
                                      PALTYPE_SUB_BG, 0, 4*0x20,  pWork->heapID);
    pWork->subbg = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_bag_bag_basebg_u_NCGR,
                                                                GFL_BG_FRAME0_S, 0, 0, pWork->heapID);
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs( p_handle, NARC_bag_bag_basebg_u_NSCR,
                                            GFL_BG_FRAME0_S, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg), 0, 0, pWork->heapID);


    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_bag_bag_basebg_d_NCLR,
                                      PALTYPE_MAIN_BG, 0, 4*0x20,  pWork->heapID);
    pWork->mainbg = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_bag_bag_basebg_d_NCGR,
                                                                 GFL_BG_FRAME0_M, 0, 0, pWork->heapID);
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs( p_handle, NARC_bag_bag_basebg_d_NSCR,
                                            GFL_BG_FRAME0_M, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainbg), 0, 0, pWork->heapID);

    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_bag_bag_win01_u_NCLR,
                                      PALTYPE_SUB_BG, 4*0x20, 4*0x20,  pWork->heapID);
    pWork->subbg2 = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_bag_bag_win01_u_NCGR,
                                                                 GFL_BG_FRAME1_S, 0, 0, pWork->heapID);
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs( p_handle, NARC_bag_bag_win01_u_NSCR,
                                            GFL_BG_FRAME1_S, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg2), 0, 0, pWork->heapID);

    //    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_bag_p_list_NCLR,
    //                                    PALTYPE_MAIN_BG, _SUBLIST_NORMAL_PAL*0x20, 2*0x20,  pWork->heapID);


    //����ʃA�C�R��
    pWork->cellRes[_PLT_BAGPOCKET] = GFL_CLGRP_PLTT_RegisterEx(
      p_handle , NARC_bag_bag_parts_d_NCLR , CLSYS_DRAW_MAIN , _PAL_BAG_PARTS_CELL*32 , 0 , 2 , pWork->heapID  );

    pWork->cellRes[_NCG_BAGPOCKET] = GFL_CLGRP_CGR_Register(
      p_handle , NARC_bag_bag_parts_d_NCGR , FALSE , CLSYS_DRAW_MAIN , pWork->heapID  );

    pWork->cellRes[_ANM_BAGPOCKET] = GFL_CLGRP_CELLANIM_Register(
      p_handle, NARC_bag_bag_parts_d_NCER, NARC_bag_bag_parts_d_NANR , pWork->heapID);

    //�����̃t���[��
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_bag_bag_win01_d_NCLR,
                                      PALTYPE_MAIN_BG, 1*0x20, 2*0x20,  pWork->heapID);
    pWork->numFrameBg =
      GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
        p_handle, NARC_bag_bag_win05_d_NCGR, GFL_BG_FRAME3_M, 0, 0, pWork->heapID);


    GFL_ARC_CloseDataHandle(p_handle);
  }

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);

  pWork->cellUnit = GFL_CLACT_UNIT_Create( _CELLUNIT_NUM , 0 , pWork->heapID );

  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);
  GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);

  {
    ARCHANDLE *archandle = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON , pWork->heapID );
    //����ʃo�[
    GFL_ARCHDL_UTIL_TransVramPalette( archandle , NARC_app_menu_common_menu_bar_NCLR ,
                                      PALTYPE_MAIN_BG , 8*32 , 32 , pWork->heapID );
    pWork->barbg = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( archandle , NARC_app_menu_common_menu_bar_NCGR ,
                                                                GFL_BG_FRAME1_M , 0 , 0, pWork->heapID );
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs( archandle , NARC_app_menu_common_menu_bar_NSCR ,
                                            GFL_BG_FRAME1_M , 0 ,
                                            0x8000+GFL_ARCUTIL_TRANSINFO_GetPos(pWork->barbg),0, 0, pWork->heapID );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );

    //����ʃA�C�R��
    pWork->cellRes[_PLT_COMMON] = GFL_CLGRP_PLTT_RegisterEx(
      archandle , NARC_app_menu_common_bar_button_NCLR , CLSYS_DRAW_MAIN , 7*32 , 0 , APP_COMMON_BARICON_PLT_NUM , pWork->heapID  );

    pWork->cellRes[_NCG_COMMON] = GFL_CLGRP_CGR_Register(
      archandle , NARC_app_menu_common_bar_button_128k_NCGR , FALSE , CLSYS_DRAW_MAIN , pWork->heapID  );

    pWork->cellRes[_ANM_COMMON] = GFL_CLGRP_CELLANIM_Register(
      archandle, NARC_app_menu_common_bar_button_128k_NCER, NARC_app_menu_common_bar_button_128k_NANR , pWork->heapID);



    GFL_ARC_CloseDataHandle(archandle);
  }
  //����ʃ{�^��
  ITEMDISP_InitTaskBar(pWork);


  {
    u8 i;
    ARCHANDLE *archandleCommon = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , pWork->heapID );

    //�^�C�v�A�C�R��
    pWork->commonCell[SCR_PLT_SUB_POKE_TYPE] = GFL_CLGRP_PLTT_RegisterEx( archandleCommon ,
                                                                          APP_COMMON_GetPokeTypePltArcIdx() , CLSYS_DRAW_SUB ,
                                                                          _OBJPLT_SUB_POKE_TYPE*32 , 0 , APP_COMMON_POKETYPE_PLT_NUM , pWork->heapID  );
    pWork->commonCell[SCR_ANM_SUB_POKE_TYPE] = GFL_CLGRP_CELLANIM_Register( archandleCommon ,
                                                                            APP_COMMON_GetPokeTypeCellArcIdx(APP_COMMON_MAPPING_32K) ,
                                                                            APP_COMMON_GetPokeTypeAnimeArcIdx(APP_COMMON_MAPPING_32K), pWork->heapID  );
    //�Z�^�C�v
    pWork->commonCell[SCR_NCG_SKILL_TYPE_HENKA] = GFL_CLGRP_CGR_Register( archandleCommon ,
                                                                          NARC_app_menu_common_p_st_bunrui_henka_NCGR , FALSE , CLSYS_DRAW_SUB , pWork->heapID  );
    pWork->commonCell[SCR_NCG_SKILL_TYPE_BUTURI] = GFL_CLGRP_CGR_Register( archandleCommon ,
                                                                           NARC_app_menu_common_p_st_bunrui_buturi_NCGR , FALSE , CLSYS_DRAW_SUB , pWork->heapID  );
    pWork->commonCell[SCR_NCG_SKILL_TYPE_TOKUSHU] = GFL_CLGRP_CGR_Register( archandleCommon ,
                                                                            NARC_app_menu_common_p_st_bunrui_tokusyu_NCGR , FALSE , CLSYS_DRAW_SUB , pWork->heapID  );
    //����
    for( i=0;i<POKETYPE_MAX;i++ )
    {
      pWork->commonCellTypeNcg[i] = GFL_CLGRP_CGR_Register( archandleCommon ,
                                                            APP_COMMON_GetPokeTypeCharArcIdx(i) , FALSE , CLSYS_DRAW_SUB , pWork->heapID );
    }

    GFL_ARC_CloseDataHandle(archandleCommon);
  }
  //�Z�̃A�C�R��
  {
    GFL_CLWK_DATA cellInitData1,cellInitData2;
    cellInitData1.pos_x = 10*8;
    cellInitData1.pos_y = 20*8+20;
    cellInitData1.softpri = 1;
    cellInitData1.bgpri = 1;
    cellInitData1.anmseq = 0;

    pWork->clwkWazaKind = GFL_CLACT_WK_Create(
      pWork->cellUnit ,
      pWork->commonCell[SCR_NCG_SKILL_TYPE_HENKA], pWork->commonCell[SCR_PLT_SUB_POKE_TYPE],  pWork->commonCell[SCR_ANM_SUB_POKE_TYPE],
      &cellInitData1 ,CLSYS_DEFREND_SUB , pWork->heapID );

    cellInitData2.pos_x = 10*8;
    cellInitData2.pos_y = 20*8+2;
    cellInitData2.softpri = 1;
    cellInitData2.bgpri = 1;
    cellInitData2.anmseq = 0;

    pWork->clwkWazaType = GFL_CLACT_WK_Create(
      pWork->cellUnit ,
      pWork->commonCell[SCR_NCG_SKILL_TYPE_HENKA], pWork->commonCell[SCR_PLT_SUB_POKE_TYPE],  pWork->commonCell[SCR_ANM_SUB_POKE_TYPE],
      &cellInitData2 ,CLSYS_DEFREND_SUB , pWork->heapID );

    GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind , FALSE );
    GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType , FALSE );
  }



}

void ITEMDISP_graphicDelete(FIELD_ITEMMENU_WORK* pWork)
{

  GFL_BG_FreeCharacterArea(GFL_BG_FRAME0_S,
                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->subbg));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,
                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg2),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->subbg2));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME3_M,
                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->numFrameBg),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->numFrameBg));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME0_M,
                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainbg),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->mainbg));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_M,
                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->barbg),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->barbg));

}

//------------------------------------------------------------------------------
/**
 * @brief   ���ʂ̃��b�Z�[�W���ĕ`��
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_upMessageRewrite(FIELD_ITEMMENU_WORK* pWork)
{
  int wazano;
  // ITEM_ST * item = MYITEM_PosItemGet( pWork->pMyItem, pWork->pocketno, ITEMMENU_GetItemIndex(pWork) );
  ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );

  if((item==NULL) || (item->id==ITEM_DUMMY_DATA)){
    return;
  }
  wazano = ITEM_GetWazaNo( item->id );


  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->winItemName), 0 );
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->winItemNum), 0 );
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->winItemReport), 0 );

  GFL_FONTSYS_SetDefaultColor();

  if(wazano == 0){
    GFL_MSG_GetString(  pWork->MsgManager, MSG_ITEM_STR001, pWork->pStrBuf );
    WORDSET_RegisterItemName(pWork->WordSet, 0, item->id);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemName), 16, 0, pWork->pExpStrBuf, pWork->fontHandle);
  }
  else{
    GFL_MSG_GetString(  pWork->MsgManager, msg_bag_086, pWork->pStrBuf );
    WORDSET_RegisterNumber(pWork->WordSet, 0, ITEM_GetWazaMashineNo(item->id)+1,
                           2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
    WORDSET_RegisterWazaName(pWork->WordSet, 1, wazano);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemName), 0, 0, pWork->pExpStrBuf, pWork->fontHandle);
  }

  GFL_MSG_GetString(  pWork->MsgManager, MSG_ITEM_STR002, pWork->pStrBuf );

  WORDSET_RegisterNumber(pWork->WordSet, 0, item->no,
                         3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemNum), 0, 0, pWork->pExpStrBuf, pWork->fontHandle);


  GFL_MSG_GetString(  pWork->MsgManagerItemInfo, item->id, pWork->pStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemReport), 0, 0, pWork->pStrBuf, pWork->fontHandle);

  GFL_BMPWIN_TransVramCharacter(pWork->winItemName);
  GFL_BMPWIN_TransVramCharacter(pWork->winItemNum);
  GFL_BMPWIN_TransVramCharacter(pWork->winItemReport);
  GFL_BG_LoadScreenV_Req(ITEMREPORT_FRAME);

  _itemiconAnim(pWork, item->id);

}


//------------------------------------------------------------------------------
/**
 * @brief   ���ʂ̃��b�Z�[�W�̊J��
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_upMessageDelete(FIELD_ITEMMENU_WORK* pWork)
{
  int i;

  GFL_BMPWIN_ClearScreen(pWork->winItemName);
  GFL_BMPWIN_ClearScreen(pWork->winItemNum);
  GFL_BMPWIN_ClearScreen(pWork->winItemReport);
  GFL_BG_LoadScreenV_Req(ITEMREPORT_FRAME);
  GFL_BMPWIN_Delete(pWork->winItemName);
  GFL_BMPWIN_Delete(pWork->winItemReport);
  GFL_BMPWIN_Delete(pWork->winItemNum);
  GFL_BMPWIN_Delete(pWork->winWaza);
  GFL_BMPWIN_Delete(pWork->winNumFrame);

  GFL_CLACT_WK_Remove( pWork->scrollCur );
  GFL_CLACT_WK_Remove( pWork->cellicon );
  GFL_CLGRP_CGR_Release( pWork->objRes[_CLACT_CHR] );
  GFL_CLGRP_PLTT_Release( pWork->objRes[_CLACT_PLT] );
  GFL_CLGRP_CELLANIM_Release(pWork->objRes[_CLACT_ANM]);

  for( i=0 ; i < ITEM_LIST_NUM ; i++ ){
    GFL_BMP_Delete(pWork->listBmp[i]);
  }

  GFL_CLACT_UNIT_Delete(pWork->cellUnit);

  GFL_CLACT_SYS_Delete();

}

//------------------------------------------------------------------------------
/**
 * @brief   ���ʂ̃��b�Z�[�W�̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_upMessageCreate(FIELD_ITEMMENU_WORK* pWork)
{
  pWork->winWaza= GFL_BMPWIN_Create(
    ITEMREPORT_FRAME,
    0, 19,
    32, 5,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );


  pWork->winNumFrame =
    GFL_BMPWIN_Create(
      GFL_BG_FRAME3_M,	_WINNUM_INITX, _WINNUM_INITY, _WINNUM_SIZEX, _WINNUM_SIZEY,	_WINNUM_PAL,
      GFL_BMP_CHRAREA_GET_B );

  pWork->winItemName = GFL_BMPWIN_Create(
    ITEMREPORT_FRAME,
    _UP_ITEMNAME_INITX, _UP_ITEMNAME_INITY,
    _UP_ITEMNAME_SIZEX, _UP_ITEMNAME_SIZEY,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

  pWork->winItemNum = GFL_BMPWIN_Create(
    ITEMREPORT_FRAME,
    _UP_ITEMNUM_INITX, _UP_ITEMNUM_INITY,
    _UP_ITEMNUM_SIZEX, _UP_ITEMNUM_SIZEY,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

  pWork->winItemReport = GFL_BMPWIN_Create(
    ITEMREPORT_FRAME,
    _UP_ITEMREPORT_INITX, _UP_ITEMREPORT_INITY,
    _UP_ITEMREPORT_SIZEX, _UP_ITEMREPORT_SIZEY,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );


  GFL_BMPWIN_MakeScreen( pWork->winItemName );
  GFL_BMPWIN_MakeScreen( pWork->winItemNum );
  GFL_BMPWIN_MakeScreen( pWork->winItemReport );
  GFL_BMPWIN_MakeScreen( pWork->winWaza );

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_ITEMICON, pWork->heapID );

    pWork->objRes[_CLACT_ANM] = GFL_CLGRP_CELLANIM_Register( p_handle ,
                                                             NARC_item_icon_itemicon_NCER ,
                                                             NARC_item_icon_itemicon_NANR ,
                                                             pWork->heapID );
    GFL_ARC_CloseDataHandle( p_handle );
  }

  {
    ITEMDISP_upMessageRewrite(pWork);
    ITEMDISP_WazaInfoWindowChange(pWork);
  }

}

static void _itemiconAnim(FIELD_ITEMMENU_WORK* pWork,int itemid)
{
  if(pWork->cellicon!=NULL){
    GFL_CLACT_WK_Remove( pWork->cellicon );
    GFL_CLGRP_CGR_Release( pWork->objRes[_CLACT_CHR] );
    GFL_CLGRP_PLTT_Release( pWork->objRes[_CLACT_PLT] );
  }


  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_ITEMICON, pWork->heapID );

    pWork->objRes[_CLACT_PLT] = GFL_CLGRP_PLTT_RegisterEx( p_handle ,
                                                           ITEM_GetIndex(itemid,ITEM_GET_ICON_PAL) ,
                                                           CLSYS_DRAW_SUB , 0, 0, 1 , pWork->heapID );
    pWork->objRes[_CLACT_CHR] = GFL_CLGRP_CGR_Register( p_handle ,
                                                        ITEM_GetIndex(itemid,ITEM_GET_ICON_CGX) ,
                                                        FALSE , CLSYS_DRAW_SUB , pWork->heapID );


    GFL_ARC_CloseDataHandle( p_handle );
  }

  {
    GFL_CLWK_DATA cellInitData;


    cellInitData.pos_x = _ITEMICON_SCR_X * 8+16;
    cellInitData.pos_y = _ITEMICON_SCR_Y * 8+16;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    pWork->cellicon = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                           pWork->objRes[_CLACT_CHR],
                                           pWork->objRes[_CLACT_PLT],
                                           pWork->objRes[_CLACT_ANM],
                                           &cellInitData ,
                                           CLSYS_DEFREND_SUB ,
                                           pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->cellicon, TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellicon, TRUE );
  }
}


void _dispMain(FIELD_ITEMMENU_WORK* pWork)
{
  GFL_CLACT_SYS_Main(); // CLSYS���C��
  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }
}





void ITEMDISP_CellResourceCreate( FIELD_ITEMMENU_WORK* pWork )
{
  int i;
  ARCHANDLE *archandle = GFL_ARC_OpenDataHandle( ARCID_BAG , pWork->heapID );

  //����ʃ��j���[�̃`�F�b�N�}�[�N
  pWork->cellRes[_PTL_CHECK] = GFL_CLGRP_PLTT_RegisterEx(
    archandle , NARC_bag_bag_win04_d_NCLR , CLSYS_DRAW_MAIN , _PAL_MENU_CHECKBOX_CELL*32 , 0 , 1 , pWork->heapID  );
  pWork->cellRes[_NCG_CHECK] = GFL_CLGRP_CGR_Register(
    archandle , NARC_bag_bag_win04_d_NCGR , FALSE , CLSYS_DRAW_MAIN , pWork->heapID  );
  pWork->cellRes[_ANM_CHECK] = GFL_CLGRP_CELLANIM_Register(
    archandle, NARC_bag_bag_win04_d_NCER, NARC_bag_bag_win04_d_NANR , pWork->heapID);

  //����ʃ��j���[���X�g

  pWork->cellRes[_PLT_CUR] = GFL_CLGRP_PLTT_RegisterEx( archandle ,
                                                        NARC_bag_bag_win03_d_NCLR , CLSYS_DRAW_MAIN ,
                                                        0 , 0 , 2 , pWork->heapID  );

  pWork->cellRes[_NCG_CUR] = GFL_CLGRP_CGR_Register(
    archandle , NARC_bag_bag_win03_d_NCGR ,
    FALSE , CLSYS_DRAW_MAIN , pWork->heapID  );

  pWork->cellRes[_ANM_CUR] = GFL_CLGRP_CELLANIM_Register(
    archandle , NARC_bag_bag_win03_d_NCER,NARC_bag_bag_win03_d_NANR ,
    pWork->heapID  );


  for( i=0 ; i < ITEM_LIST_NUM ; i++ )
  {
    pWork->listRes[i] = GFL_CLGRP_CGR_Register(
      archandle , NARC_bag_bag_win01_d_NCGR ,
      FALSE , CLSYS_DRAW_MAIN , pWork->heapID  );
    pWork->listBmp[i] = GFL_BMP_Create( 12, 2, GFL_BMP_16_COLOR, pWork->heapID );

  }

  pWork->cellRes[_ANM_LIST] = GFL_CLGRP_CELLANIM_Register(
    archandle , NARC_bag_bag_win01_d_NCER, NARC_bag_bag_win01_d_NANR ,
    pWork->heapID  );




  GFL_ARC_CloseDataHandle(archandle);
}


void ITEMDISP_CellCreate( FIELD_ITEMMENU_WORK* pWork )
{

  //�X�N���[���J�[�\��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 31*8;
    cellInitData.pos_y = _SCROLL_TOP_Y;
    cellInitData.softpri = 1;
    cellInitData.bgpri = 2;
    cellInitData.anmseq = 0;

    pWork->scrollCur = GFL_CLACT_WK_Create(
      pWork->cellUnit ,
      pWork->cellRes[_NCG_CUR], pWork->cellRes[_PLT_CUR],  pWork->cellRes[_ANM_CUR],
      &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );

    GFL_CLACT_WK_SetDrawEnable( pWork->scrollCur , TRUE );
  }


  //�I���J�[�\��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 17*8-4+8;
    cellInitData.pos_y = 24+24;
    cellInitData.softpri = 1;
    cellInitData.bgpri = 2;
    cellInitData.anmseq = 1;

    pWork->clwkCur = GFL_CLACT_WK_Create(
      pWork->cellUnit ,
      pWork->cellRes[_NCG_CUR], pWork->cellRes[_PLT_CUR],  pWork->cellRes[_ANM_CUR],
      &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );

    GFL_CLACT_WK_SetDrawEnable( pWork->clwkCur , TRUE );
  }

  //�Z���̍쐬
  {
    int i;
    for( i=0 ; i < ITEM_LIST_NUM ; i++ )
    {
      GFL_CLWK_DATA cellInitData;
      cellInitData.pos_x = 17 * 8 - 4 + 8;
      cellInitData.pos_y = 3 * 8 * i ;
      cellInitData.softpri = 10;
      cellInitData.bgpri = 3;
      cellInitData.anmseq = 0;

      pWork->listCell[i] = GFL_CLACT_WK_Create(
        pWork->cellUnit ,
        pWork->listRes[ i ],pWork->cellRes[_PLT_CUR],  pWork->cellRes[_ANM_LIST],
        &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );

      cellInitData.pos_x -= 3 * 8;
      cellInitData.anmseq = 1;
      pWork->listMarkCell[i] = GFL_CLACT_WK_Create(
        pWork->cellUnit ,
        pWork->cellRes[_NCG_CHECK],pWork->cellRes[_PTL_CHECK],  pWork->cellRes[_ANM_CHECK],
        &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );


      GFL_CLACT_WK_SetDrawEnable( pWork->listCell[i] , FALSE );
      GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i] , FALSE );

    }
  }
}


void ITEMDISP_CellMessagePrint( FIELD_ITEMMENU_WORK* pWork )
{
  int i;

  for(i = 0; i< ITEM_LIST_NUM ; i++){
    ITEM_ST * item;

    pWork->nListEnable[i]=FALSE;

    if(pWork->oamlistpos+i < 0){
      continue;
    }
    //    item = MYITEM_PosItemGet( pWork->pMyItem, pWork->pocketno,  pWork->oamlistpos+i  );
    item = ITEMMENU_GetItem( pWork , pWork->oamlistpos+i);
    if((item==NULL) || (item->id==ITEM_DUMMY_DATA)){
      continue;
    }
    {
      void * itemdata;
      itemdata = ITEM_GetItemArcData( item->id, ITEM_GET_DATA, pWork->heapID );

      GFL_BMP_Clear(pWork->listBmp[i],3);
      GFL_FONTSYS_SetColor( 0xf, 0xe, 3 );
      GFL_MSG_GetString(  pWork->MsgManager, MSG_ITEM_STR001, pWork->pStrBuf );
      WORDSET_RegisterItemName(pWork->WordSet, 0, item->id);
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      PRINTSYS_Print( pWork->listBmp[i], 0, 0, pWork->pExpStrBuf, pWork->fontHandle);

      if( ITEM_GetBufParam( itemdata, ITEM_PRM_CNV ) == 0 ){
        pWork->nListEnable[ i ] = 3;
      }
      else if(ITEMMENU_GetPosCnvButtonItem(pWork,item->id)==-1){
        pWork->nListEnable[ i ] = 2;
      }
      else{
        pWork->nListEnable[ i ] = 1;
      }
      GFL_HEAP_FreeMemory( itemdata );

    }
  }



}


void ITEMDISP_CellVramTrans( FIELD_ITEMMENU_WORK* pWork )
{
  int i;

  u32 dest = GFL_CLGRP_CGR_GetAddr( pWork->listRes[0], CLSYS_DRAW_MAIN);

  {
    GFL_CLACTPOS pos;

    GFL_CLACT_WK_GetPos( pWork->clwkCur , &pos, CLWK_SETSF_NONE );
    pos.y = 24 * pWork->curpos + 24;
    GFL_CLACT_WK_SetPos( pWork->clwkCur ,  &pos, CLWK_SETSF_NONE );
  }
  for(i = 0; i< ITEM_LIST_NUM ; i++){
    {
      u32 dest_adrs = GFL_CLGRP_CGR_GetAddr( pWork->listRes[i], CLSYS_DRAW_MAIN);
      u8* charbuff = GFL_BMP_GetCharacterAdrs(pWork->listBmp[i]);
      u32 size = GFL_BMP_GetBmpDataSize(pWork->listBmp[i]);
      DC_FlushRange(charbuff, size);

      dest_adrs += (8)*32;
      GX_LoadOBJ(&charbuff[8*32], dest_adrs, (32*4));
      dest_adrs += (4)*32;
      GX_LoadOBJ(&charbuff[(20*32)], dest_adrs, (32*4));

      dest_adrs += (12)*32;
      GX_LoadOBJ(&charbuff[4*32], dest_adrs, (32*4));
      dest_adrs += (4)*32;
      GX_LoadOBJ(&charbuff[(16*32)], dest_adrs, (32*4));

      dest_adrs += (12)*32;
      GX_LoadOBJ(&charbuff[0*32], dest_adrs, (32*4));
      dest_adrs += (4)*32;
      GX_LoadOBJ(&charbuff[(12*32)], dest_adrs, (32*4));

      if(pWork->nListEnable[i]){
        if(pWork->pocketno == BAG_POKE_EVENT){
          GFL_CLACT_WK_SetAnmSeq( pWork->listMarkCell[i] , pWork->nListEnable[i]-1 );
          GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i] , TRUE );
        }
        else{
          GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i] , FALSE );
        }
        GFL_CLACT_WK_SetDrawEnable( pWork->listCell[i] , TRUE );
      }
      else{
        GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i] , FALSE );
        GFL_CLACT_WK_SetDrawEnable( pWork->listCell[i] , FALSE );
      }
    }
  }
}

//      GX_LoadOBJ(&charbuff[ 0*32], dest_adrs + (11)*32, (32*5));
//     GX_LoadOBJ(&charbuff[12*32], dest_adrs + (19)*32, (32*5));
//      GX_LoadOBJ(&charbuff[ 5*32], dest_adrs + (40)*32, (32*7));
//      GX_LoadOBJ(&charbuff[17*32], dest_adrs + (48)*32, (32*7));


//------------------------------------------------------------------------------
/**
 * @brief   �X�N���[���o�[��̃X�N���[���J�[�\���\��
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_scrollCursorMove(FIELD_ITEMMENU_WORK* pWork)
{
  u32 x,y;

  if(GFL_UI_TP_GetPointCont(&x, &y) == TRUE){
    GFL_CLACTPOS pos;
    if(y < _SCROLL_TOP_Y){
      return;
    }
    if(y > _SCROLL_BOTTOM_Y){
      return;
    }
    GFL_CLACT_WK_GetPos( pWork->scrollCur , &pos, CLWK_SETSF_NONE );
    pos.y = y;
    GFL_CLACT_WK_SetPos( pWork->scrollCur ,  &pos, CLWK_SETSF_NONE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �w���Ă���A�C�e���ԍ�����X�N���[���J�[�\���ʒu��ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_scrollCursorChangePos(FIELD_ITEMMENU_WORK* pWork, int num)
{
  u32 x,y;

  {
    int length = ITEMMENU_GetItemPocketNumber( pWork ) - 1;
    GFL_CLACTPOS pos;
    int ymax = _SCROLL_BOTTOM_Y - _SCROLL_TOP_Y;
    int y = ((num * ymax)/length) + _SCROLL_TOP_Y;


    if(y < _SCROLL_TOP_Y){
      y=_SCROLL_TOP_Y;
    }
    if(y > _SCROLL_BOTTOM_Y){
      y=_SCROLL_BOTTOM_Y;
    }


    GFL_CLACT_WK_GetPos( pWork->scrollCur , &pos, CLWK_SETSF_NONE );
    pos.y = y;
    GFL_CLACT_WK_SetPos( pWork->scrollCur ,  &pos, CLWK_SETSF_NONE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �^�X�N�o�[��CELL�̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void ITEMDISP_InitTaskBar( FIELD_ITEMMENU_WORK* pWork )
{



  //�o�[�̃{�^��
  {
    const u8 anmIdxArr[] = { 4, 5, 6, 0, 1 };
    const u8 posXArr[] =
    {
      8,
      8*15,
      172,
      _BAR_CELL_CURSOR_EXIT,
      _BAR_CELL_CURSOR_RETURN,
    };

    u8 i;
    GFL_CLWK_DATA cellInitData;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 1;

    for( i = 0;i < elementof(anmIdxArr) ; i++ )
    {
      cellInitData.pos_x = posXArr[i];
      cellInitData.pos_y = 21*8;
      cellInitData.anmseq = anmIdxArr[i];
      pWork->clwkBarIcon[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                   pWork->cellRes[_NCG_COMMON],
                                                   pWork->cellRes[_PLT_COMMON],
                                                   pWork->cellRes[_ANM_COMMON],
                                                   &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );

      GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[i] , TRUE );
    }

  }

}





#define    NANR_bag_parts_d_taisetsu    0 //
#define    NANR_bag_parts_d_waza    1 //
#define    NANR_bag_parts_d_kinomi    2 //
#define    NANR_bag_parts_d_kaifuku    3 //
#define    NANR_bag_parts_d_dougu    4 //



//------------------------------------------------------------------------------
/**
 * @brief   �|�P�b�g��CELL��������
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_InitPocketCell( FIELD_ITEMMENU_WORK* pWork )
{

  //�o�[�̃{�^��
  {

    u8 i;
    GFL_CLWK_DATA cellInitData;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 1;

    cellInitData.pos_x = 0;
    cellInitData.pos_y = 0;
    cellInitData.anmseq = 0;
    pWork->clwkPocketIcon = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                 pWork->cellRes[_NCG_BAGPOCKET],
                                                 pWork->cellRes[_PLT_BAGPOCKET],
                                                 pWork->cellRes[_ANM_BAGPOCKET],
                                                 &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );

    GFL_CLACT_WK_SetDrawEnable( pWork->clwkPocketIcon , FALSE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�b�g��CELL��������
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_CLACTPOS pokectCellPos[]={
#include "bag_anmpos.h"
};



void ITEMDISP_ChangePocketCell( FIELD_ITEMMENU_WORK* pWork,int pocketno )
{
  int anm[] = {
    NANR_bag_parts_d_dougu,
    NANR_bag_parts_d_kaifuku,
    NANR_bag_parts_d_waza,
    NANR_bag_parts_d_kinomi,
    NANR_bag_parts_d_taisetsu};

  GFL_CLACT_WK_SetAnmSeq(pWork->clwkPocketIcon, anm[pocketno]);
  GFL_CLACT_WK_SetPos( pWork->clwkPocketIcon ,  &pokectCellPos[pocketno], CLWK_SETSF_NONE );
  GFL_CLACT_WK_SetDrawEnable( pWork->clwkPocketIcon , TRUE );

}

//------------------------------------------------------------------------------
/**
 * @brief   ���X�g�v���[�g�̃N���A
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_ListPlateClear( FIELD_ITEMMENU_WORK* pWork )
{
  //�v���[�g�̓y��̊G
  int i;

  //  for(i = 0 ; i < elementof(pWork->menuWin) ; i++){
  //  GFL_BMPWIN_ClearScreen(pWork->menuWin[i]);
  //  }
  //���b�Z�[�W�E�C���h�̃N���A��
  GFL_BMPWIN_ClearScreen(pWork->itemInfoDispWin);
  BmpWinFrame_Clear(pWork->itemInfoDispWin,WINDOW_TRANS_ON_V);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_M);
}





//------------------------------------------------------------------------------
/**
 * @brief   ���X�g�v���[�g�̊G���g���ă��X�g���쐬
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_MenuWinDisp(  FIELD_ITEMMENU_WORK *pWork , int *menustr,int num )
{
#if 1
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  num;
  appinit.itemWork =  &pWork->appitem[0];
  appinit.bgFrame =  GFL_BG_FRAME3_M;
  appinit.palNo = _SUBLIST_NORMAL_PAL;

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 24;

  appinit.msgHandle = pWork->MsgManager;
  appinit.fontHandle =  pWork->fontHandle;
  appinit.printQue =  pWork->SysMsgQue;

  for(i=0;i<num;i++){
    pWork->appitem[i].str = GFL_STR_CreateBuffer(100, pWork->heapID);
    GFL_MSG_GetString(pWork->MsgManager, menustr[i], pWork->appitem[i].str);
    pWork->appitem[i].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  }
  pWork->pAppTask = APP_TASKMENU_OpenMenu(&appinit);
  for(i=0;i<num;i++){
    GFL_STR_DeleteBuffer(pWork->appitem[i].str);
  }
  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ , -8 );


#else
  int i;
  int winindex = elementof(pWork->menuWin) - num;

  pWork->menuNum = num;
  for(i = 0 ; i < num ; i++)
  {
    PRINTSYS_LSB col;
    GFL_BMPWIN* pwin = pWork->menuWin[winindex + i];

    GFL_MSG_GetString(pWork->MsgManager, menustr[i], pWork->pStrBuf);
    GFL_STD_MemCopy32( pWork->ncgData->pRawData ,
                       GFL_BMP_GetCharacterAdrs(GFL_BMPWIN_GetBmp( pwin )) ,
                       0x20*13*3 );
    GFL_FONTSYS_SetColor( 0xe, 0xf, 0 );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 8, 4, pWork->pStrBuf, pWork->fontHandle);
    GFL_BMPWIN_TransVramCharacter(pwin);
    GFL_BMPWIN_MakeScreen(pwin);
  }
  ITEMDISP_ListPlateSelectChange(pWork, 0);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_M);
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�������̉�ʕύX
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_ItemInfoWindowChange(FIELD_ITEMMENU_WORK *pWork,int pocketno )
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_BAG, pWork->heapID );
  if(pocketno!=BAG_POKE_WAZA){
    GFL_ARCHDL_UTIL_TransVramScreenCharOfsVBlank(
      p_handle, NARC_bag_bag_win01_u_NSCR, GFL_BG_FRAME1_S, 0,
      GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg2), 0, 0, pWork->heapID);
  }
  else{
    GFL_ARCHDL_UTIL_TransVramScreenCharOfsVBlank(
      p_handle, NARC_bag_bag_win02_u_NSCR, GFL_BG_FRAME1_S, 0,
      GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg2), 0, 0, pWork->heapID);
  }
  GFL_ARC_CloseDataHandle(p_handle);
}

//------------------------------------------------------------------------------
/**
 * @brief   �Z��ނ̃A�C�R���̕\��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _wazaKindDisp( FIELD_ITEMMENU_WORK *pWork ,int wazaNo)
{
  {
    NNSG2dImageProxy imageProxy;
    const int type = WAZADATA_GetDamageType( wazaNo );
    GFL_CLGRP_CGR_GetProxy( pWork->commonCell[SCR_NCG_SKILL_TYPE_HENKA+type] , &imageProxy );
    GFL_CLACT_WK_SetImgProxy( pWork->clwkWazaKind , &imageProxy );
    GFL_CLACT_WK_SetPlttOffs( pWork->clwkWazaKind ,
                              APP_COMMON_GetWazaKindPltOffset(type) ,
                              CLWK_PLTTOFFS_MODE_PLTT_TOP );
    GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind, TRUE );
  }
}
//------------------------------------------------------------------------------
/**
 * @brief   �Z�^�C�v�̃A�C�R���\��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _wazaTypeDisp( FIELD_ITEMMENU_WORK *pWork ,int wazaNo)
{
  {
    NNSG2dImageProxy imageProxy;
    int type1 = WAZADATA_GetType(wazaNo);
    GFL_CLGRP_CGR_GetProxy( pWork->commonCellTypeNcg[type1] , &imageProxy );
    GFL_CLACT_WK_SetImgProxy( pWork->clwkWazaType , &imageProxy );
    GFL_CLACT_WK_SetPlttOffs( pWork->clwkWazaType ,
                              APP_COMMON_GetPokeTypePltOffset(type1) ,
                              CLWK_PLTTOFFS_MODE_PLTT_TOP );
    GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType , TRUE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �Z�}�V���̐����ǉ�
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_WazaInfoWindowChange( FIELD_ITEMMENU_WORK *pWork )
{
  GFL_BMPWIN* pwin = pWork->winWaza;
  ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );
  int wazano = ITEM_GetWazaNo( item->id );
  int ppnum = WT_PPMaxGet(wazano, 0);
  int pow = WT_WazaDataParaGet( wazano, ID_WTD_damage );
  int hit = WT_WazaDataParaGet( wazano, ID_WTD_hitprobability );

  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);
  if(wazano==0){
    GFL_BMPWIN_ClearScreen(pwin);
    GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind, FALSE );
    GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType, FALSE );

    return;
  }
  else{
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 0 );
    GFL_BMPWIN_MakeScreen(pwin);
  }

  _wazaKindDisp(pWork,wazano);
  _wazaTypeDisp(pWork,wazano);

  //�^�C�v
  GFL_MSG_GetString(  pWork->MsgManager, mes_bag_107, pWork->pStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 8, 4, pWork->pStrBuf, pWork->fontHandle);

  //�Ԃ�邢
  GFL_MSG_GetString(  pWork->MsgManager, mes_bag_098, pWork->pStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 8, 24, pWork->pStrBuf, pWork->fontHandle);

  //�З�
  GFL_MSG_GetString(  pWork->MsgManager, mes_bag_096, pWork->pStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 8*14, 4, pWork->pStrBuf, pWork->fontHandle);

  if(pow==0){
    GFL_MSG_GetString(  pWork->MsgManager, msg_bag_025, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 4, pWork->pStrBuf, pWork->fontHandle);
  }
  else{
    //����傭�̌���
    GFL_MSG_GetString(  pWork->MsgManager, mes_bag_100, pWork->pStrBuf );
    WORDSET_RegisterNumber(pWork->WordSet, 0, pow, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 4, pWork->pExpStrBuf, pWork->fontHandle);
  }
  //����
  GFL_MSG_GetString(  pWork->MsgManager, mes_bag_097, pWork->pStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 8*14, 24, pWork->pStrBuf, pWork->fontHandle);

  if(hit==0){
    GFL_MSG_GetString(  pWork->MsgManager, msg_bag_025, pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 24, pWork->pStrBuf, pWork->fontHandle);
  }
  else{
    //�߂����イ�̌���
    GFL_MSG_GetString(  pWork->MsgManager, mes_bag_100, pWork->pStrBuf );
    WORDSET_RegisterNumber(pWork->WordSet, 0, hit, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 24, pWork->pExpStrBuf, pWork->fontHandle);
  }
  //PP
  GFL_MSG_GetString(  pWork->MsgManager, mes_bag_095, pWork->pStrBuf );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 27*8, 4, pWork->pStrBuf, pWork->fontHandle);

  //PP�̌���
  GFL_MSG_GetString(  pWork->MsgManager, mes_bag_099, pWork->pStrBuf );
  WORDSET_RegisterNumber(pWork->WordSet, 0, ppnum, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 29*8, 4, pWork->pExpStrBuf, pWork->fontHandle);


  GFL_BMPWIN_TransVramCharacter(pwin);

}


//------------------------------------------------------------------------------
/**
 * @brief   �����E�C���h�E�\��
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_ItemInfoWindowDisp( FIELD_ITEMMENU_WORK *pWork )
{
  GFL_BMPWIN* pwin;

  if(pWork->itemInfoDispWin==NULL){
    pWork->itemInfoDispWin = GFL_BMPWIN_Create(
      GFL_BG_FRAME3_M ,
      1 , 1, 30 ,4 ,
      _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );
  }
  pwin = pWork->itemInfoDispWin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 15);
  GFL_FONTSYS_SetColor(1, 2, 15);
#if 0
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 0, 0, pWork->pExpStrBuf, pWork->fontHandle);
#else

  //==============================================================================================
  /**
   * �v�����g�X�g���[���𗘗p����������`��i�ʏ�� - �R�[���o�b�N�Ȃ��j
   *
   * @param   dst     �`���Bitmap
   * @param   xpos    �`��J�n�w���W�i�h�b�g�j
   * @param   ypos    �`��J�n�x���W�i�h�b�g�j
   * @param   str     ������
   * @param   font    �t�H���g�n���h��
   * @param   wait    �P�����`�悲�Ƃ̃E�F�C�g�t���[�����i���j
   * @param   tcbsys    �g�p����GFL_TCBL�V�X�e���|�C���^
   * @param   tcbpri    �g�p����^�X�N�v���C�I���e�B
   * @param   heapID    �g�p����q�[�vID
   *
   * @retval  PRINT_STREAM* �X�g���[���n���h��
   *
   * �� wait ��-2 �ȉ��̒l��ݒ肷�邱�ƂŁA���t���[���Q�����ȏ�̕`����s���B-2�i�Q�����j, -3�i�R�����j...
   */
  //==============================================================================================
  //extern PRINT_STREAM* PRINTSYS_PrintStream(
  //    GFL_BMPWIN* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font,
  //    int wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, HEAPID heapID, u16 clearColor );


  pWork->pStream = PRINTSYS_PrintStream(pwin ,0,0, pWork->pExpStrBuf, pWork->fontHandle,
                                        MSGSPEED_GetWait(), pWork->pMsgTcblSys, 2, pWork->heapID, 15);


#endif

  BmpWinFrame_Write( pwin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar), _BUTTON_WIN_PAL );

  GFL_BMPWIN_TransVramCharacter(pwin);
  GFL_BMPWIN_MakeScreen(pwin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_M);
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�������̕��͂�Exp�o�b�t�@�ɓ����
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_ItemInfoMessageMake( FIELD_ITEMMENU_WORK *pWork,int id )
{
  GFL_MSG_GetString( pWork->MsgManager, msg_bag_042, pWork->pStrBuf );
  WORDSET_RegisterItemName(pWork->WordSet, 0, id);
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
}

//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�̏I���҂�
 * @retval  none
 */
//------------------------------------------------------------------------------

BOOL ITEMDISP_MessageEndCheck(FIELD_ITEMMENU_WORK* pWork)
{
  if(pWork->pStream){
    int state = PRINTSYS_PrintStreamGetState( pWork->pStream );
    switch(state){
    case PRINTSTREAM_STATE_DONE:
      PRINTSYS_PrintStreamDelete( pWork->pStream );
      pWork->pStream = NULL;
      break;
    case PRINTSTREAM_STATE_PAUSE:
      if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
        PRINTSYS_PrintStreamReleasePause( pWork->pStream );
      }
      break;
    default:
      break;
    }
    return FALSE;  //�܂��I����ĂȂ�
  }
  return TRUE;// �I����Ă���
}

//------------------------------------------------------------------------------
/**
 * @brief   �����t���[���̕\��
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_NumFrameDisp(FIELD_ITEMMENU_WORK* pWork)
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_BAG, pWork->heapID );


  {
    void* arcData = GFL_ARCHDL_UTIL_Load( p_handle, NARC_bag_bag_win05_d_NSCR, 0, pWork->heapID );
    NNSG2dScreenData *scrnData;

    if( NNS_G2dGetUnpackedScreenData( arcData, &scrnData ) )
    {
      int i,xs=scrnData->screenWidth/8,ys=scrnData->screenHeight/8;
      u16* buff=(u16*)&scrnData->rawData;

      for(i=0;i<(xs*ys);i++){
        buff[i] += GFL_ARCUTIL_TRANSINFO_GetPos(pWork->numFrameBg) | 0x1000;
      }

      GFL_BG_WriteScreen(
        GFL_BG_FRAME3_M, &scrnData->rawData,_WINNUM_SCR_INITX,_WINNUM_SCR_INITY,
        scrnData->screenWidth/8,scrnData->screenHeight/8);
      GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_M);
    }
    GFL_HEAP_FreeMemory( arcData );

  }

  GFL_ARC_CloseDataHandle(p_handle);

}

//------------------------------------------------------------------------------
/**
 * @brief   �����̕\��
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_TrashNumDisp(FIELD_ITEMMENU_WORK* pWork,int num)
{
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->winNumFrame), 3 );
  GFL_FONTSYS_SetColor( 0xf, 0xa, 3 );
  GFL_MSG_GetString(  pWork->MsgManager, MSG_ITEM_STR002, pWork->pStrBuf );
  WORDSET_RegisterNumber(pWork->WordSet, 0, num,
                         3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winNumFrame), 0, 2, pWork->pExpStrBuf, pWork->fontHandle);
  GFL_BMPWIN_TransVramCharacter(pWork->winNumFrame);
  GFL_BMPWIN_MakeScreen(pWork->winNumFrame);

}

//------------------------------------------------------------------------------
/**
 * @brief   �͂��������E�C���h�E
 * @retval  none
 */
//------------------------------------------------------------------------------


void ITEMDISP_YesNoStart(FIELD_ITEMMENU_WORK* pWork)
{
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  2;
  appinit.itemWork =  &pWork->appitem[0];
  appinit.bgFrame =  GFL_BG_FRAME3_M;
  appinit.palNo = _SUBLIST_NORMAL_PAL;

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 12;

  appinit.msgHandle = pWork->MsgManager;
  appinit.fontHandle = pWork->fontHandle;
  appinit.printQue = pWork->SysMsgQue;

  pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->MsgManager, MSG_ITEM_STR003, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->appitem[1].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->MsgManager, MSG_ITEM_STR004, pWork->appitem[1].str);
  pWork->appitem[1].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->pAppTask = APP_TASKMENU_OpenMenu(&appinit);
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);
  GFL_STR_DeleteBuffer(pWork->appitem[1].str);
  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ , -8 );
}
