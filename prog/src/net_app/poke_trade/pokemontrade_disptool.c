//=============================================================================
/**
 * @file	  pokemontrade_disptool.c
 * @bfief	  �|�P���������O���t�B�b�N����
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/12/18
 */
//=============================================================================


#include <gflib.h>
#include "arc_def.h"
#include "poke_tool/monsno_def.h"

#include "trade.naix"
#include "poke_icon.naix"

#include "net_app/pokemontrade.h"
#include "pokemontrade_local.h"
#include "msg/msg_poke_trade.h"
#include "gamesystem/msgspeed.h"  //MSGSPEED_GetWait
#include "font/font.naix"    // NARC_font_default_nclr

#include "system/bmp_winframe.h"
#include "savedata/box_savedata.h"
#include "pokeicon/pokeicon.h"
#include "app/app_menu_common.h"

#include "ircbattle.naix"
#include "trade.naix"
#include "tradedemo.naix"
#include "tradeirdemo.naix"
#include "app_menu_common.naix"






//----------------------------------------------------------------------------
/**
 *  @brief  TOUCHBAR������
 *  @param  POKEMON_TRADE_WORK
 *  @return none
 */
//-----------------------------------------------------------------------------

void POKETRADE_TOUCHBAR_Init(POKEMON_TRADE_WORK* pWork)
{
  //�A�C�R���̐ݒ�
  //�������
  TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]  =
  {
    {
      TOUCHBAR_ICON_RETURN,
      { TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
    },
    {
      TOUCHBAR_ICON_CUTSOM1,
      { TOUCHBAR_ICON_X_00+8+4, TOUCHBAR_ICON_Y },
    },
    {
      TOUCHBAR_ICON_CUTSOM2,
      { TOUCHBAR_ICON_X_06+4, TOUCHBAR_ICON_Y },
    },
    {
      TOUCHBAR_ICON_CUTSOM3,
      { TOUCHBAR_ICON_X_01, TOUCHBAR_ICON_Y },
    },
  };

  TOUCHBAR_SETUP  touchbar_setup;
  GFL_STD_MemClear( &touchbar_setup, sizeof(TOUCHBAR_SETUP) );

  touchbar_setup.p_item   = touchbar_icon_tbl;        //��̑����
  touchbar_setup.item_num = NELEMS(touchbar_icon_tbl);//�����������邩
  touchbar_setup.p_unit   = pWork->cellUnit;                    //OBJ�ǂݍ��݂̂��߂�CLUNIT
  touchbar_setup.is_notload_bg = TRUE;  //BG�͂Ȃ�
  touchbar_setup.bar_frm  = GFL_BG_FRAME0_S;            //BG�ǂݍ��݂̂��߂�BG�ʏ㉺��ʔ���ɂ��K�v
  touchbar_setup.bg_plt   = _TOUCHBAR_BG_PALPOS;      //BG��گ�
  touchbar_setup.obj_plt  = _TOUCHBAR_OBJ_PALPOS;      //OBJ��گ�
  touchbar_setup.mapping  = APP_COMMON_MAPPING_128K;  //�}�b�s���O���[�h

  touchbar_icon_tbl[1].cg_idx = pWork->cellRes[CHAR_SCROLLBAR];
  touchbar_icon_tbl[1].plt_idx = pWork->cellRes[PAL_SCROLLBAR];
  touchbar_icon_tbl[1].cell_idx = pWork->cellRes[ANM_SCROLLBAR];
  touchbar_icon_tbl[1].active_anmseq  = 6;            //�A�N�e�B�u�̂Ƃ��̃A�j��
  touchbar_icon_tbl[1].noactive_anmseq  =   5;            //�m���A�N�e�B�u�̂Ƃ��̃A�j��
  touchbar_icon_tbl[1].push_anmseq  =   4;            //�������Ƃ��̃A�j���iSTOP�ɂȂ��Ă��邱�Ɓj
  touchbar_icon_tbl[1].key  =   0;    //�L�[�ŉ������Ƃ��ɓ��삳�������Ȃ�΁A�{�^���ԍ�
  touchbar_icon_tbl[1].se =   POKETRADESE_DECIDE;                 //�������Ƃ���SE�Ȃ炵�����Ȃ�΁ASE�̔ԍ�

  touchbar_icon_tbl[2].cg_idx = pWork->cellRes[CHAR_SCROLLBAR];
  touchbar_icon_tbl[2].plt_idx = pWork->cellRes[PAL_SCROLLBAR];
  touchbar_icon_tbl[2].cell_idx = pWork->cellRes[ANM_SCROLLBAR];
  touchbar_icon_tbl[2].active_anmseq  = 9;            //�A�N�e�B�u�̂Ƃ��̃A�j��
  touchbar_icon_tbl[2].noactive_anmseq  =   8;            //�m���A�N�e�B�u�̂Ƃ��̃A�j��
  touchbar_icon_tbl[2].push_anmseq  =   7;            //�������Ƃ��̃A�j���iSTOP�ɂȂ��Ă��邱�Ɓj
  touchbar_icon_tbl[2].key  =   PAD_BUTTON_START;    //�L�[�ŉ������Ƃ��ɓ��삳�������Ȃ�΁A�{�^���ԍ�
  touchbar_icon_tbl[2].se =   POKETRADESE_DECIDE;                 //�������Ƃ���SE�Ȃ炵�����Ȃ�΁ASE�̔ԍ�

  touchbar_icon_tbl[3].cg_idx = pWork->cellRes[CHAR_SCROLLBAR];
  touchbar_icon_tbl[3].plt_idx = pWork->cellRes[PAL_SCROLLBAR];
  touchbar_icon_tbl[3].cell_idx = pWork->cellRes[ANM_SCROLLBAR];
  touchbar_icon_tbl[3].active_anmseq  = 22;            //�A�N�e�B�u�̂Ƃ��̃A�j��
  touchbar_icon_tbl[3].noactive_anmseq  =  21;            //�m���A�N�e�B�u�̂Ƃ��̃A�j��
  touchbar_icon_tbl[3].push_anmseq  =   20;            //�������Ƃ��̃A�j���iSTOP�ɂȂ��Ă��邱�Ɓj
  touchbar_icon_tbl[3].key  = (PAD_KEY_LEFT|PAD_KEY_RIGHT);    //�L�[�ŉ������Ƃ��ɓ��삳�������Ȃ�΁A�{�^���ԍ�
  touchbar_icon_tbl[3].se =   POKETRADESE_DECIDE;                 //�������Ƃ���SE�Ȃ炵�����Ȃ�΁ASE�̔ԍ�


  pWork->pTouchWork = TOUCHBAR_Init(&touchbar_setup, pWork->heapID);
  TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_CUTSOM3, FALSE);
  TOUCHBAR_SetVisible(pWork->pTouchWork, TOUCHBAR_ICON_CUTSOM2, FALSE);
  TOUCHBAR_SetSoftPriorityAll( pWork->pTouchWork, 2 );

  
}





