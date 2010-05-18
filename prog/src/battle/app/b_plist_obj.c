//============================================================================================
/**
 * @file		b_plist_obj.c
 * @brief		�퓬�p�|�P�������X�g���OBJ����
 * @author	Hiroyuki Nakamura
 * @date		05.02.07
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "poke_tool/gauge_tool.h"
#include "pokeicon/pokeicon.h"
#include "item/item.h"
#include "app/app_menu_common.h"
#include "waza_tool/wazadata.h"

#include "battle/battgra_wb.naix"
#include "app_menu_common.naix"

#include "b_plist.h"
#include "b_plist_main.h"
#include "b_plist_obj.h"
#include "b_plist_bmp.h"
#include "b_plist_gra.naix"


//============================================================================================
//  �萔��`
//============================================================================================

// �p���b�g�ʒu���T�C�Y
#define PALOFS_POKEICON     ( 0 )
#define PALSIZ_POKEICON     ( 3 )
#define PALOFS_STATUSICON   ( PALOFS_POKEICON + 0x20 * PALSIZ_POKEICON )
#define PALSIZ_STATUSICON   ( 1 )
#define PALOFS_ITEMICON     ( PALOFS_STATUSICON + 0x20 * PALSIZ_STATUSICON )
#define PALSIZ_ITEMICON     ( 1 )
#define PALOFS_TYPEICON     ( PALOFS_ITEMICON + 0x20 * PALSIZ_ITEMICON )
#define PALSIZ_TYPEICON     ( 3 )
#define PALOFS_CURSOR       ( PALOFS_TYPEICON + 0x20 * PALSIZ_TYPEICON )
#define PALSIZ_CURSOR       ( 1 )
#define PALOFS_HPGAUGE      ( PALOFS_CURSOR + 0x20 * PALSIZ_CURSOR )
#define PALSIZ_HPGAUGE      ( 1 )


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static void BPL_ClactResManInit( BPLIST_WORK * wk );
static void BPL_ClactPokeLoad( BPLIST_WORK * wk );
static void BPL_ClactStatusLoad( BPLIST_WORK * wk );
static void BPL_ClactTypeLoad( BPLIST_WORK * wk );
static void BPL_ClactItemLoad( BPLIST_WORK * wk );
static void BPL_ClactAddAll( BPLIST_WORK * wk );

static void BPL_PokeIconPaletteChg( BPLIST_WORK * wk );

static void BPL_Page1ObjSet( BPLIST_WORK * wk );
static void BPL_ChgPageObjSet( BPLIST_WORK * wk );
static void BPL_StMainPageObjSet( BPLIST_WORK * wk );
static void BPL_StWazaSelPageObjSet( BPLIST_WORK * wk );
static void BPL_Page4ObjSet( BPLIST_WORK * wk );
static void BPL_Page5ObjSet( BPLIST_WORK * wk );
static void BPL_Page6ObjSet( BPLIST_WORK * wk );
static void BPL_Page7ObjSet( BPLIST_WORK * wk );

static void BPL_ClactCursorAdd( BPLIST_WORK * wk );
static void BPL_CursorDel( BPLIST_WORK * wk );

static void CursorResLoad( BPLIST_WORK * wk );
static void HPGaugeResLoad( BPLIST_WORK * wk );


//============================================================================================
//  �O���[�o��
//============================================================================================
// ���\�[�X�e�[�u��
static const u32 ObjParamEz[][5] =
{ // �L�����A�p���b�g�A�Z���A�Z���A�j���ABG�v���C�I���e�B�AOBJ�v���C�I���e�B
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���P
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���Q
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���R
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���S
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���T
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���U
  { BPLIST_CHRRES_ITEM, BPLIST_PALRES_ITEM, BPLIST_CELRES_ITEM, 1, 0 },     // �A�C�e���A�C�R���V

  { BPLIST_CHRRES_POKE1, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���P
  { BPLIST_CHRRES_POKE2, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���Q
  { BPLIST_CHRRES_POKE3, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���R
  { BPLIST_CHRRES_POKE4, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���S
  { BPLIST_CHRRES_POKE5, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���T
  { BPLIST_CHRRES_POKE6, BPLIST_PALRES_POKE, BPLIST_CELRES_POKE, 1, 1 },      // �|�P�����A�C�R���U

  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���P
  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���Q
  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���R
  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���S
  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���T
  { BPLIST_CHRRES_STATUS, BPLIST_PALRES_STATUS, BPLIST_CELRES_STATUS, 1, 1 }, // ��Ԉُ�A�C�R���U

  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�P
  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�Q
  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�R
  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�S
  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�T
  { BPLIST_CHRRES_HPGAUGE, BPLIST_PALRES_HPGAUGE, BPLIST_CELRES_HPGAUGE, 2, 0 },  // �g�o�Q�[�W�g�U

  { BPLIST_CHRRES_POKETYPE1, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �|�P�����^�C�v�A�C�R���P
  { BPLIST_CHRRES_POKETYPE2, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �|�P�����^�C�v�A�C�R���Q
  { BPLIST_CHRRES_POKETYPE3, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �|�P�����^�C�v�A�C�R���R
  { BPLIST_CHRRES_POKETYPE4, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �|�P�����^�C�v�A�C�R���S

  { BPLIST_CHRRES_WAZATYPE1, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���P
  { BPLIST_CHRRES_WAZATYPE2, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���Q
  { BPLIST_CHRRES_WAZATYPE3, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���R
  { BPLIST_CHRRES_WAZATYPE4, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���S
  { BPLIST_CHRRES_WAZATYPE5, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���T
  { BPLIST_CHRRES_WAZATYPE6, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���U
  { BPLIST_CHRRES_WAZATYPE7, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���V
  { BPLIST_CHRRES_WAZATYPE8, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���W
  { BPLIST_CHRRES_WAZATYPE9, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���X
  { BPLIST_CHRRES_WAZATYPE10, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },    // �Z�^�C�v�A�C�R���P�O

  { BPLIST_CHRRES_BUNRUI, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },			 // ���ރA�C�R��
  { BPLIST_CHRRES_BUNRUI2, BPLIST_PALRES_TYPE, BPLIST_CELRES_TYPE, 1, 0 },     // ���ރA�C�R���Q
};

// �y�[�W�P�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P1_PokePos[] =
{
  { BPL_COMM_POKEICON_LPX, BPL_COMM_POKEICON1_PY }, { BPL_COMM_POKEICON_RPX, BPL_COMM_POKEICON2_PY },
  { BPL_COMM_POKEICON_LPX, BPL_COMM_POKEICON3_PY }, { BPL_COMM_POKEICON_RPX, BPL_COMM_POKEICON4_PY },
  { BPL_COMM_POKEICON_LPX, BPL_COMM_POKEICON5_PY }, { BPL_COMM_POKEICON_RPX, BPL_COMM_POKEICON6_PY },
};

// �y�[�W�P�̏�Ԉُ�A�C�R���̍��W
static const GFL_CLACTPOS P1_StatusPos[] =
{
  { BPL_COMM_POKEICON_LPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON1_PY+BPL_COMM_STATUSICON_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON2_PY+BPL_COMM_STATUSICON_Y },
	{ BPL_COMM_POKEICON_LPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON3_PY+BPL_COMM_STATUSICON_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON4_PY+BPL_COMM_STATUSICON_Y },
  { BPL_COMM_POKEICON_LPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON5_PY+BPL_COMM_STATUSICON_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_STATUSICON_X, BPL_COMM_POKEICON6_PY+BPL_COMM_STATUSICON_Y },
};

// �y�[�W�P�̂g�o�Q�[�W�g�̍��W
static const GFL_CLACTPOS P1_HPGaugePos[] =
{
  { BPL_COMM_POKEICON_LPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON1_PY+BPL_COMM_HPGAUGE_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON2_PY+BPL_COMM_HPGAUGE_Y},
	{ BPL_COMM_POKEICON_LPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON3_PY+BPL_COMM_HPGAUGE_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON4_PY+BPL_COMM_HPGAUGE_Y },
  { BPL_COMM_POKEICON_LPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON5_PY+BPL_COMM_HPGAUGE_Y },
	{ BPL_COMM_POKEICON_RPX+BPL_COMM_HPGAUGE_X, BPL_COMM_POKEICON6_PY+BPL_COMM_HPGAUGE_Y },
};

static const GFL_CLACTPOS P_CHG_PokePos = { 128, 72 };  // ����ւ��y�[�W�̃|�P�����A�C�R���̍��W

static const GFL_CLACTPOS P2_PokePos = { 24, 12 };    // �y�[�W�Q�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P2_StatusPos = { 192+6, 17+3 }; // �y�[�W�Q�̏�Ԉُ�A�C�R���̍��W
// �y�[�W�Q�̃|�P�����̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P2_PokeTypePos[] =
{
  { 128+2, 16 },
  { 160+4, 16 }
};
// �y�[�W�Q�̃A�C�e�������̃A�C�e���A�C�R���̍��W
static const GFL_CLACTPOS P2_ItemIconPos = { 20, 132 };
// �y�[�W�Q�̂g�o�Q�[�W�g�̍��W
static const GFL_CLACTPOS P2_HPGaugePos = { 223, 52 };

static const GFL_CLACTPOS P3_PokePos = { 24, 12 };    // �y�[�W�R�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P3_StatusPos = { 192+6, 17+3 }; // �y�[�W�R�̏�Ԉُ�A�C�R���̍��W
// �y�[�W�R�̃|�P�����̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P3_PokeTypePos[] =
{
  { 128+2, 16 },
  { 160+4, 16 }
};
// �y�[�W�R�̋Z�̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P3_WazaTypePos[] =
{
  {  24,  80 },
  { 152,  80 },
  {  24, 128 },
  { 152, 128 }
};

static const GFL_CLACTPOS P4_PokePos = { 24, 12 };    // �y�[�W�S�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P4_StatusPos = { 192+6, 17+3 }; // �y�[�W�S�̏�Ԉُ�A�C�R���̍��W
// �y�[�W�S�̃|�P�����̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P4_PokeTypePos[] =
{
  { 128+2, 16 },
  { 160+4, 16 }
};
static const GFL_CLACTPOS P4_WazaTypePos = { 136, 40 }; // �y�[�W�S�̋Z�^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P4_WazaKindPos = { 24, 80 };  // �y�[�W�S�̋Z���ރA�C�R���̍��W

static const GFL_CLACTPOS P5_PokePos = { 24, 12 };    // �y�[�W�T�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P5_StatusPos = { 192+6, 17+3 }; // �y�[�W�T�̏�Ԉُ�A�C�R���̍��W
// �y�[�W�T�̃|�P�����̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P5_PokeTypePos[] =
{
  { 128+2, 16 },
  { 160+4, 16 }
};
// �y�[�W�T�̋Z�̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P5_WazaTypePos[] =
{
  {  24,  80 },
  { 152,  80 },
  {  24, 128 },
  { 152, 128 },
  {  88, 176 }
};

static const GFL_CLACTPOS P6_PokePos = { 24, 12 };    // �y�[�W�U�̃|�P�����A�C�R���̍��W
static const GFL_CLACTPOS P6_StatusPos = { 192+6, 17+3 }; // �y�[�W�T�̏�Ԉُ�A�C�R���̍��W
// �y�[�W�U�̃|�P�����̃^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P6_PokeTypePos[] =
{
  { 128+2, 16 },
  { 160+4, 16 }
};
static const GFL_CLACTPOS P6_WazaTypePos = { 136, 40 }; // �y�[�W�U�̋Z�^�C�v�A�C�R���̍��W
static const GFL_CLACTPOS P6_WazaKindPos = { 24, 80 };  // �y�[�W�U�̋Z���ރA�C�R���̍��W



//--------------------------------------------------------------------------------------------
/**
 * @brief		�퓬�p�|�P���X�gOBJ������
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ObjInit( BPLIST_WORK * wk )
{
  BPL_ClactResManInit( wk );
  BPL_ClactPokeLoad( wk );
  BPL_ClactStatusLoad( wk );
  BPL_ClactItemLoad( wk );
  BPL_ClactTypeLoad( wk );
  CursorResLoad( wk );
  HPGaugeResLoad( wk );
  BPL_ClactAddAll( wk );
  BPL_ClactCursorAdd( wk );

  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���\�[�X�}�l�[�W���[������
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactResManInit( BPLIST_WORK * wk )
{
  u32 i;

  for( i=0; i<BPLIST_CHRRES_MAX; i++ ){
    wk->chrRes[i] = 0xffffffff;
  }
  for( i=0; i<BPLIST_PALRES_MAX; i++ ){
    wk->palRes[i] = 0xffffffff;
  }
  for( i=0; i<BPLIST_CELRES_MAX; i++ ){
    wk->celRes[i] = 0xffffffff;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����A�C�R���̃O���t�B�b�N���[�h
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactPokeLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;
  u32 * res;
  u16 i;
	u16	pos;

  ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, GFL_HEAP_LOWID(wk->dat->heap) );

  // �L����
  res = &wk->chrRes[BPLIST_CHRRES_POKE1];
  for( i=0; i<TEMOTI_POKEMAX; i++ ){
		pos = i;
    if( wk->poke[pos].mons != 0 ){
      res[i] = GFL_CLGRP_CGR_Register(
                ah,
                POKEICON_GetCgxArcIndex(PP_GetPPPPointerConst(wk->poke[pos].pp)),
                FALSE,
                CLSYS_DRAW_SUB,
                wk->dat->heap );
    }else{
      res[i] = GFL_CLGRP_CGR_Register(
                ah,
                POKEICON_GetCgxArcIndexByMonsNumber(0,0,0,0),
                FALSE,
                CLSYS_DRAW_SUB,
                wk->dat->heap );
    }
  }

  // �p���b�g
  wk->palRes[BPLIST_PALRES_POKE] = GFL_CLGRP_PLTT_RegisterComp(
                                    ah, POKEICON_GetPalArcIndex(),
                                    CLSYS_DRAW_SUB, PALOFS_POKEICON, wk->dat->heap );

  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_POKE] = GFL_CLGRP_CELLANIM_Register(
                                    ah,
                                    POKEICON_GetCellSubArcIndex(),
                                    POKEICON_GetAnmSubArcIndex(),
                                    wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		��Ԉُ�A�C�R���̃O���t�B�b�N���[�h
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactStatusLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;

  ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, GFL_HEAP_LOWID(wk->dat->heap) );

  // �L����
  wk->chrRes[BPLIST_CHRRES_STATUS] = GFL_CLGRP_CGR_Register(
                                      ah, NARC_app_menu_common_p_st_ijou_NCGR,
                                      FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  // �p���b�g
  wk->palRes[BPLIST_PALRES_STATUS] = GFL_CLGRP_PLTT_Register(
                                      ah, NARC_app_menu_common_p_st_ijou_NCLR,
                                      CLSYS_DRAW_SUB, PALOFS_STATUSICON, wk->dat->heap );
  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_STATUS] = GFL_CLGRP_CELLANIM_Register(
                                      ah,
                                      NARC_app_menu_common_p_st_ijou_32k_NCER,
                                      NARC_app_menu_common_p_st_ijou_32k_NANR,
                                      wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�C�v/���ރA�C�R���̃O���t�B�b�N���[�h
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactTypeLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;
  u32 i;

  ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, GFL_HEAP_LOWID(wk->dat->heap) );

  // �p���b�g
  wk->palRes[BPLIST_PALRES_TYPE] = GFL_CLGRP_PLTT_Register(
                                      ah, NARC_app_menu_common_p_st_type_NCLR,
                                      CLSYS_DRAW_SUB, PALOFS_TYPEICON, wk->dat->heap );
  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_TYPE] = GFL_CLGRP_CELLANIM_Register(
                                      ah,
                                      NARC_app_menu_common_p_st_type_32k_NCER,
                                      NARC_app_menu_common_p_st_type_32k_NANR,
                                      wk->dat->heap );
  // �^�C�v�A�C�R���L����
  for( i=BPLIST_CHRRES_POKETYPE1; i<=BPLIST_CHRRES_WAZATYPE10; i++ ){
    wk->chrRes[i] = GFL_CLGRP_CGR_Register(
                      ah, APP_COMMON_GetPokeTypeCharArcIdx(0),
                      FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  }

  // ���ރA�C�R���L����
  wk->chrRes[BPLIST_CHRRES_BUNRUI] = GFL_CLGRP_CGR_Register(
                                      ah, APP_COMMON_GetWazaKindCharArcIdx(0),
                                      FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  wk->chrRes[BPLIST_CHRRES_BUNRUI2] = GFL_CLGRP_CGR_Register(
	                                      ah, APP_COMMON_GetWazaKindCharArcIdx(0),
		                                    FALSE, CLSYS_DRAW_SUB, wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���A�C�R���̃O���t�B�b�N���[�h
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactItemLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;

  ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, GFL_HEAP_LOWID(wk->dat->heap) );

  // �L����
  wk->chrRes[BPLIST_CHRRES_ITEM] = GFL_CLGRP_CGR_Register(
                                      ah, NARC_app_menu_common_item_icon_NCGR,
                                      FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  // �p���b�g
  wk->palRes[BPLIST_PALRES_ITEM] = GFL_CLGRP_PLTT_Register(
                                      ah, NARC_app_menu_common_item_icon_NCLR,
                                      CLSYS_DRAW_SUB, PALOFS_ITEMICON, wk->dat->heap );
  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_ITEM] = GFL_CLGRP_CELLANIM_Register(
                                      ah,
                                      NARC_app_menu_common_item_icon_32k_NCER,
                                      NARC_app_menu_common_item_icon_32k_NANR,
                                      wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\�����\�[�X���[�h
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorResLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;

  ah = GFL_ARC_OpenDataHandle( ARCID_BATTGRA, GFL_HEAP_LOWID(wk->dat->heap) );

  // �L����
  wk->chrRes[BPLIST_CHRRES_CURSOR] = GFL_CLGRP_CGR_Register(
                                      ah, NARC_battgra_wb_battle_w_cursor_NCGR,
                                      FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  // �p���b�g
  wk->palRes[BPLIST_PALRES_CURSOR] = GFL_CLGRP_PLTT_RegisterEx(
                                      ah, NARC_battgra_wb_battle_w_obj_NCLR,
                                      CLSYS_DRAW_SUB, PALOFS_CURSOR, 4, PALSIZ_CURSOR, wk->dat->heap );

  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_CURSOR] = GFL_CLGRP_CELLANIM_Register(
                                      ah,
                                      NARC_battgra_wb_battle_w_cursor_NCER,
                                      NARC_battgra_wb_battle_w_cursor_NANR,
                                      wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g�o�Q�[�W���\�[�X���[�h
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void HPGaugeResLoad( BPLIST_WORK * wk )
{
  ARCHANDLE * ah;

  ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), GFL_HEAP_LOWID(wk->dat->heap) );

  // �L����
  wk->chrRes[BPLIST_CHRRES_HPGAUGE] = GFL_CLGRP_CGR_Register(
                                        ah,
                                        APP_COMMON_GetHPBarBaseCharArcIdx(APP_COMMON_MAPPING_32K),
                                        FALSE, CLSYS_DRAW_SUB, wk->dat->heap );
  // �p���b�g
  wk->palRes[BPLIST_PALRES_HPGAUGE] = GFL_CLGRP_PLTT_Register(
                                        ah, APP_COMMON_GetHPBarBasePltArcIdx(),
                                        CLSYS_DRAW_SUB, PALOFS_HPGAUGE, wk->dat->heap );

  // �Z���E�A�j��
  wk->celRes[BPLIST_CELRES_HPGAUGE] = GFL_CLGRP_CELLANIM_Register(
                                        ah,
                                        APP_COMMON_GetHPBarBaseCellArcIdx(APP_COMMON_MAPPING_32K),
                                        APP_COMMON_GetHPBarBaseAnimeArcIdx(APP_COMMON_MAPPING_32K),
                                        wk->dat->heap );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�ǉ��i�ʁj
 *
 * @param		wk    ���[�N
 * @param		res		���\�[�X�C���f�b�N�X�e�[�u��
 *
 * @return  �Z���A�N�^�[�f�[�^
 */
//--------------------------------------------------------------------------------------------
static GFL_CLWK * BPL_ClactAdd( BPLIST_WORK * wk, const u32 * res )
{
  GFL_CLWK * clwk;
  GFL_CLWK_DATA dat;

  dat.pos_x = 0;
  dat.pos_y = 0;
  dat.anmseq = 0;
  dat.softpri = res[4];
  dat.bgpri = res[3];

  clwk = GFL_CLACT_WK_Create(
            wk->clunit,
            wk->chrRes[res[0]],
            wk->palRes[res[1]],
            wk->celRes[res[2]],
            &dat, CLSYS_DRAW_SUB, wk->dat->heap );

  return clwk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�ǉ��i�S�āj
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactAddAll( BPLIST_WORK * wk )
{
  u32 i;

  wk->clunit = GFL_CLACT_UNIT_Create( BPL_CA_MAX+BAPPTOOL_CURSOR_MAX, 0, GFL_HEAP_LOWID(wk->dat->heap) );

  for( i=0; i<BPL_CA_MAX; i++ ){
    wk->clwk[i] = BPL_ClactAdd( wk, ObjParamEz[i] );
  }
  GFL_NET_ReloadIcon();

  PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_OBJ, 0, 0x1e0 );
  BPL_PokeIconPaletteChg( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�퓬�p�|�P���X�gOBJ�폜
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ObjFree( BPLIST_WORK * wk )
{
  u32 i;

  for( i=0; i<BPL_CA_MAX; i++ ){
    GFL_CLACT_WK_Remove( wk->clwk[i] );
  }
  BAPPTOOL_DelCursor( wk->cpwk );

  for( i=0; i<BPLIST_CHRRES_MAX; i++ ){
    GFL_CLGRP_CGR_Release( wk->chrRes[i] );
  }
  for( i=0; i<BPLIST_PALRES_MAX; i++ ){
    GFL_CLGRP_PLTT_Release( wk->palRes[i] );
  }
  for( i=0; i<BPLIST_CELRES_MAX; i++ ){
    GFL_CLGRP_CELLANIM_Release( wk->celRes[i] );
  }

  GFL_CLACT_UNIT_Delete( wk->clunit );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[��\�����č��W�ύX
 *
 * @param		clwk		�Z���A�N�^�[�f�[�^
 * @param		pos			�\�����W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactOn( GFL_CLWK * clwk, const GFL_CLACTPOS * pos )
{
  GFL_CLACT_WK_SetDrawEnable( clwk, TRUE );
  GFL_CLACT_WK_SetPos( clwk, pos, CLSYS_DRAW_SUB );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����A�C�R���̃p���b�g�؂�ւ�
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PokeIconPaletteChg( BPLIST_WORK * wk )
{
  s16 i;

  for( i=0; i<TEMOTI_POKEMAX; i++ ){
    if( wk->poke[i].mons == 0 ){ continue; }

    GFL_CLACT_WK_SetPlttOffs(
      wk->clwk[BPL_CA_POKE1+i],
      POKEICON_GetPalNum(wk->poke[i].mons,wk->poke[i].form,wk->poke[i].sex,wk->poke[i].egg),
      CLWK_PLTTOFFS_MODE_PLTT_TOP );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�C�v�A�C�R���؂�ւ�
 *
 * @param		wk				���[�N
 * @param		clwk		  �Z���A�N�^�[�f�[�^
 * @param		chrResID	�L�������\�[�XID
 * @param		type		  �^�C�v
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_TypeIconChange( BPLIST_WORK * wk, GFL_CLWK * clwk, u32 chrResID, u32 type )
{
  NNSG2dCharacterData * dat;
  void * buf;
  ARCHANDLE * ah;

  ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, GFL_HEAP_LOWID(wk->dat->heap) );

  buf = GFL_ARCHDL_UTIL_LoadOBJCharacter(
          ah, APP_COMMON_GetPokeTypeCharArcIdx(type), FALSE, &dat, GFL_HEAP_LOWID(wk->dat->heap) );
  GFL_CLGRP_CGR_Replace( wk->chrRes[chrResID], dat );
  GFL_HEAP_FreeMemory( buf );

  GFL_ARC_CloseDataHandle( ah );

  GFL_CLACT_WK_SetPlttOffs(
    clwk, APP_COMMON_GetPokeTypePltOffset(type), CLWK_PLTTOFFS_MODE_PLTT_TOP );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ރA�C�R���؂�ւ�
 *
 * @param		wk				���[�N
 * @param		clwk			�Z���A�N�^�[�f�[�^
 * @param		chrResID	�L�������\�[�X�h�c
 * @param		kind			����
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_KindIconChange( BPLIST_WORK * wk, u32 objID, u32 chrResID, u32 kind )
{
  NNSG2dCharacterData * dat;
  void * buf;
  ARCHANDLE * ah;

	objID += wk->waza_kind_swap;
	chrResID += wk->waza_kind_swap;

  ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, GFL_HEAP_LOWID(wk->dat->heap) );

  buf = GFL_ARCHDL_UTIL_LoadOBJCharacter(
          ah, APP_COMMON_GetWazaKindCharArcIdx(kind), FALSE, &dat, GFL_HEAP_LOWID(wk->dat->heap) );
  GFL_CLGRP_CGR_Replace( wk->chrRes[chrResID], dat );
  GFL_HEAP_FreeMemory( buf );

  GFL_ARC_CloseDataHandle( ah );

  GFL_CLACT_WK_SetPlttOffs(
    wk->clwk[objID], APP_COMMON_GetWazaKindPltOffset(kind), CLWK_PLTTOFFS_MODE_PLTT_TOP );

	wk->waza_kind_swap ^= 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		��Ԉُ�A�C�R���؂�ւ�
 *
 * @param		st    �X�e�[�^�X
 * @param		clwk	�Z���A�N�^�[�f�[�^
 * @param		pos		�\�����W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StIconPut( u16 st, GFL_CLWK * clwk, const GFL_CLACTPOS * pos )
{
  if( st == APP_COMMON_ST_ICON_NONE ){ return; }

  GFL_CLACT_WK_SetAnmSeq( clwk, st );
  BPL_ClactOn( clwk, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����̃^�C�v�A�C�R���؂�ւ�
 *
 * @param		wk    ���[�N
 * @param		pd    �|�P�����f�[�^
 * @param		pos   ���W�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PokeTypeIconPut( BPLIST_WORK * wk, BPL_POKEDATA * pd, const GFL_CLACTPOS * pos )
{
	u16	objID, chrRes;

	if( wk->poke_type_swap == 0 ){
		objID  = BPL_CA_POKETYPE1;
		chrRes = BPLIST_CHRRES_POKETYPE1;
	}else{
		objID  = BPL_CA_POKETYPE3;
		chrRes = BPLIST_CHRRES_POKETYPE3;
	}
  BPL_TypeIconChange( wk, wk->clwk[objID], chrRes, pd->type1 );
  BPL_ClactOn( wk->clwk[objID], &pos[0] );
  if( pd->type1 != pd->type2 ){
    BPL_TypeIconChange( wk, wk->clwk[objID+1], chrRes+1, pd->type2 );
    BPL_ClactOn( wk->clwk[objID+1], &pos[1] );
  }

	wk->poke_type_swap ^= 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���A�C�R���؂�ւ�
 *
 * @param		item		�A�C�e���ԍ�
 * @param		clwk		�Z���A�N�^�[�f�[�^
 * @param		pos			�\�����W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ItemIconPut( u16 item, GFL_CLWK * clwk, const GFL_CLACTPOS * pos )
{
  if( item == ITEM_DUMMY_DATA ){ return; }

  if( ITEM_CheckMail( item ) == TRUE ){
    GFL_CLACT_WK_SetAnmSeq( clwk, 1 );
  }else{
    GFL_CLACT_WK_SetAnmSeq( clwk, 0 );
  }
  BPL_ClactOn( clwk, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W���Ƃ�OBJ���Z�b�g
 *
 * @param		wk    ���[�N
 * @param		page  �y�[�W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_PageObjSet( BPLIST_WORK * wk, u32 page )
{
  u32 i;

  for( i=0; i<BPL_CA_MAX; i++ ){
    GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], FALSE );
  }

  switch( page ){
  case BPLIST_PAGE_SELECT:	  // �|�P�����I���y�[�W
	case BPLIST_PAGE_DEAD:			// �m������ւ��I���y�[�W
    BPL_Page1ObjSet( wk );
    break;

  case BPLIST_PAGE_POKE_CHG:  // �|�P��������ւ��y�[�W
    BPL_ChgPageObjSet( wk );
    break;

  case BPLIST_PAGE_MAIN:    // �X�e�[�^�X���C���y�[�W
    BPL_StMainPageObjSet( wk );
    break;

  case BPLIST_PAGE_WAZA_SEL:  // �X�e�[�^�X�Z�I���y�[�W
    BPL_StWazaSelPageObjSet( wk );
    break;

  case BPLIST_PAGE_SKILL:   // �X�e�[�^�X�Z�y�[�W
    BPL_Page4ObjSet( wk );
    break;

  case BPLIST_PAGE_PP_RCV:    // PP�񕜋Z�I���y�[�W
    BPL_Page7ObjSet( wk );
    break;

  case BPLIST_PAGE_WAZASET_BS:  // �X�e�[�^�X�Z�Y��P�y�[�W�i�퓬�Z�I���j
    BPL_Page5ObjSet( wk );
    break;

  case BPLIST_PAGE_WAZASET_BI:  // �X�e�[�^�X�Z�Y��Q�y�[�W�i�퓬�Z�ڍׁj
    BPL_Page6ObjSet( wk );
    break;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�P��OBJ���Z�b�g
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page1ObjSet( BPLIST_WORK * wk )
{
	u16	pos;
  s16 i;

  for( i=0; i<TEMOTI_POKEMAX; i++ ){
		pos = BPLISTMAIN_GetListRow( wk, i );

    if( wk->poke[pos].mons == 0 ){ continue; }
    // �|�P�����A�C�R��
    BPL_ClactOn( wk->clwk[BPL_CA_POKE1+pos], &P1_PokePos[i] );
    // ��Ԉُ�A�C�R��
    BPL_StIconPut(
      wk->poke[pos].st, wk->clwk[BPL_CA_STATUS1+pos], &P1_StatusPos[i] );
    // �g�o�Q�[�W�g
    BPL_ClactOn( wk->clwk[BPL_CA_HPGAUGE1+pos], &P1_HPGaugePos[i] );
    // �A�C�e���A�C�R��
    {
      GFL_CLACTPOS  actpos = P1_PokePos[i];
      actpos.x += BPL_COMM_ITEMICON_X;
      actpos.y += BPL_COMM_ITEMICON_Y;
      BPL_ItemIconPut(
        wk->poke[pos].item, wk->clwk[BPL_CA_ITEM1+pos], &actpos );
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		����ւ��y�[�W��OBJ���Z�b�g
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ChgPageObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P_CHG_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�Q��OBJ���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StMainPageObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P2_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P2_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P2_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );

  // �A�C�e���A�C�R���i�A�C�e�����̉��́j
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM7], &P2_ItemIconPos );

  // �g�o�Q�[�W
  BPL_ClactOn( wk->clwk[BPL_CA_HPGAUGE1], &P2_HPGaugePos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�e�[�^�X�Z�I���y�[�W��OBJ���Z�b�g
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StWazaSelPageObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P3_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P3_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P3_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
  // �Z�^�C�v
  BattlePokelist_WazaTypeSet( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�S��OBJ���Z�b�g
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page4ObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;
  u32 i;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P4_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P4_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P4_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
	// �Z�^�C�v�A�C�R��
	if( wk->waza_type_swap == 0 ){
		BPL_ClactOn( wk->clwk[BPL_CA_WAZATYPE6+wk->dat->sel_wp], &P4_WazaTypePos );
	}else{
		BPL_ClactOn( wk->clwk[BPL_CA_WAZATYPE1+wk->dat->sel_wp], &P4_WazaTypePos );
	}
  // ���ރA�C�R��
	if( wk->waza_kind_swap == 0 ){
	  BPL_ClactOn( wk->clwk[BPL_CA_BUNRUI], &P4_WazaKindPos );
	}else{
	  BPL_ClactOn( wk->clwk[BPL_CA_BUNRUI2], &P4_WazaKindPos );
	}
	BPL_KindIconChange( wk, BPL_CA_BUNRUI, BPLIST_CHRRES_BUNRUI, pd->waza[wk->dat->sel_wp].kind );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�T��OBJ���Z�b�g
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page5ObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P5_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P5_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P5_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
  // �Z�^�C�v�A�C�R��
  BattlePokelist_WazaTypeSet( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�U��OBJ���Z�b�g
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page6ObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
  u32 i;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P6_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P6_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P6_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
  // �Z�^�C�v�A�C�R��
	if( wk->waza_type_swap == 0 ){
	  BPL_ClactOn( wk->clwk[BPL_CA_WAZATYPE6+wk->dat->sel_wp], &P6_WazaTypePos );
	}else{
	  BPL_ClactOn( wk->clwk[BPL_CA_WAZATYPE1+wk->dat->sel_wp], &P6_WazaTypePos );
	}
  // ���ރA�C�R��
	if( wk->waza_kind_swap == 0 ){
	  BPL_ClactOn( wk->clwk[BPL_CA_BUNRUI], &P6_WazaKindPos );
	}else{
	  BPL_ClactOn( wk->clwk[BPL_CA_BUNRUI2], &P6_WazaKindPos );
	}
  if( wk->dat->sel_wp < 4 ){
    BPL_KindIconChange( wk, BPL_CA_BUNRUI, BPLIST_CHRRES_BUNRUI, pd->waza[wk->dat->sel_wp].kind );
  }else{
    BPL_KindIconChange(
      wk, BPL_CA_BUNRUI, BPLIST_CHRRES_BUNRUI, WAZADATA_GetParam(wk->dat->chg_waza,WAZAPARAM_DAMAGE_TYPE) );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�V��OBJ���Z�b�g
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page7ObjSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  GFL_CLACTPOS  pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  pd  = &wk->poke[row];
  pos = P5_PokePos;

  // �|�P�����A�C�R��
  BPL_ClactOn( wk->clwk[BPL_CA_POKE1+row], &pos );
  // ��Ԉُ�A�C�R��
  BPL_StIconPut(
    pd->st, wk->clwk[BPL_CA_STATUS1+row], &P5_StatusPos );
  // �^�C�v
  BPL_PokeTypeIconPut( wk, pd, P5_PokeTypePos );
  // �A�C�e���A�C�R��
  pos.x += 8;
  pos.y += 8;
  BPL_ItemIconPut( pd->item, wk->clwk[BPL_CA_ITEM1+row], &pos );
  // �Z�^�C�v�A�C�R��
  BattlePokelist_WazaTypeSet( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z�A�C�R���Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokelist_WazaTypeSet( BPLIST_WORK * wk )
{
  BPL_POKEDATA * pd;
  u32 i;
	u16	objID, chrRes;

  pd = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ];

	if( wk->waza_type_swap == 0 ){
		objID  = BPL_CA_WAZATYPE1;
		chrRes = BPLIST_CHRRES_WAZATYPE1;
	}else{
		objID  = BPL_CA_WAZATYPE6;
		chrRes = BPLIST_CHRRES_WAZATYPE6;
	}

  // �Z�^�C�v
  for( i=0; i<4; i++ ){
    if( pd->waza[i].id == 0 ){ continue; }

    BPL_TypeIconChange( wk, wk->clwk[objID+i], chrRes+i, pd->waza[i].type );
    BPL_ClactOn( wk->clwk[objID+i], &P5_WazaTypePos[i] );
  }

  if( wk->dat->chg_waza != 0 && wk->dat->mode == BPL_MODE_WAZASET ){
      BPL_TypeIconChange(
        wk, wk->clwk[objID+i], chrRes+i,
        WAZADATA_GetParam( wk->dat->chg_waza, WAZAPARAM_TYPE ) );
      BPL_ClactOn( wk->clwk[objID+i], &P5_WazaTypePos[i] );
  }

	wk->waza_type_swap ^= 1;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����A�C�R���A�j���؂�ւ�
 *
 * @param		clwk	�Z���A�N�^�[�f�[�^
 * @param		anm		�A�j��
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PokeIconAnmChg( GFL_CLWK * clwk, u16 anm )
{
  if( GFL_CLACT_WK_GetAnmSeq( clwk ) == anm ){ return; }

  GFL_CLACT_WK_SetAnmFrame( clwk, 0 );
  GFL_CLACT_WK_SetAnmSeq( clwk, anm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����A�C�R���A�j���`�F�b�N
 *
 * @param		pd    �|�P�����f�[�^
 *
 * @return  �A�j���ԍ�
 */
//--------------------------------------------------------------------------------------------
static u8 BPL_PokeIconAnmCheck( BPL_POKEDATA * pd )
{
  if( pd->hp == 0 ){
    return POKEICON_ANM_DEATH;
  }

  if( pd->st != APP_COMMON_ST_ICON_NONE && pd->st != APP_COMMON_ST_ICON_HINSI ){
    return POKEICON_ANM_STCHG;
  }

  if( pd->hp == pd->mhp ){
    return POKEICON_ANM_HPMAX;
  }

  switch( GAUGETOOL_GetGaugeDottoColor( pd->hp, pd->mhp ) ){
  case GAUGETOOL_HP_DOTTO_GREEN:    // ��
    return POKEICON_ANM_HPGREEN;
  case GAUGETOOL_HP_DOTTO_YELLOW: // ��
    return POKEICON_ANM_HPYERROW;
  case GAUGETOOL_HP_DOTTO_RED:      // ��
    return POKEICON_ANM_HPRED;
  }

  return POKEICON_ANM_DEATH;

  return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����A�C�R���A�j��
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BPL_PokeIconAnime( BPLIST_WORK * wk )
{
  u16 i;
  u8	anm;
	u8	pos;

  for( i=0; i<TEMOTI_POKEMAX; i++ ){
		pos = BPLISTMAIN_GetListRow( wk, i );

    if( wk->poke[pos].mons == 0 ){ continue; }

    anm = BPL_PokeIconAnmCheck( &wk->poke[pos] );

    BPL_PokeIconAnmChg( wk->clwk[BPL_CA_POKE1+pos], anm );
    GFL_CLACT_WK_AddAnmFrame( wk->clwk[BPL_CA_POKE1+pos], FX32_ONE );
  }
}


//============================================================================================
//  �J�[�\��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�I���J�[�\���ǉ�
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ClactCursorAdd( BPLIST_WORK * wk )
{
  BAPPTOOL_AddCursor(
    wk->cpwk,
    wk->clunit,
    wk->chrRes[BPLIST_CHRRES_CURSOR],
    wk->palRes[BPLIST_PALRES_CURSOR],
    wk->celRes[BPLIST_CELRES_CURSOR] );

  BAPPTOOL_VanishCursor( wk->cpwk, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�I���J�[�\���폜
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_CursorDel( BPLIST_WORK * wk )
{
  BAPPTOOL_DelCursor( wk->cpwk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�m������ւ����̂n�a�i����
 *
 * @param		wk    ���[�N
 * @param		num		�|�P�����ʒu
 * @param		mv		�ړ��l
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BPLISTOBJ_MoveDeadChg( BPLIST_WORK * wk, u8 num, s8 mv )
{
	GFL_CLACTPOS	pos;
	u32	row;

	row = BPLISTMAIN_GetListRow( wk, num );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_ITEM1+row], &pos, CLSYS_DRAW_SUB );
	pos.x += mv;
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_ITEM1+row], &pos, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_POKE1+row], &pos, CLSYS_DRAW_SUB );
	pos.x += mv;
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_POKE1+row], &pos, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_STATUS1+row], &pos, CLSYS_DRAW_SUB );
	pos.x += mv;
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_STATUS1+row], &pos, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_HPGAUGE1+row], &pos, CLSYS_DRAW_SUB );
	pos.x += mv;
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_HPGAUGE1+row], &pos, CLSYS_DRAW_SUB );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�m������ւ����̂n�a�i����ւ�
 *
 * @param		wk    ���[�N
 * @param		num1	�|�P�����ʒu�P
 * @param		num2	�|�P�����ʒu�Q
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BPLISTOBJ_ChgDeadSel( BPLIST_WORK * wk, u8 num1, u8 num2 )
{
	GFL_CLACTPOS	pos1, pos2;
	u32	row1, row2;

	row1 = BPLISTMAIN_GetListRow( wk, num1 );
	row2 = BPLISTMAIN_GetListRow( wk, num2 );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_ITEM1+row1], &pos1, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_ITEM1+row2], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_ITEM1+row1], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_ITEM1+row2], &pos1, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_POKE1+row1], &pos1, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_POKE1+row2], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_POKE1+row1], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_POKE1+row2], &pos1, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_STATUS1+row1], &pos1, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_STATUS1+row2], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_STATUS1+row1], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_STATUS1+row2], &pos1, CLSYS_DRAW_SUB );

  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_HPGAUGE1+row1], &pos1, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_GetPos( wk->clwk[BPL_CA_HPGAUGE1+row2], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_HPGAUGE1+row1], &pos2, CLSYS_DRAW_SUB );
  GFL_CLACT_WK_SetPos( wk->clwk[BPL_CA_HPGAUGE1+row2], &pos1, CLSYS_DRAW_SUB );
}
