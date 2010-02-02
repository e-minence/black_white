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

//�O�����J
#include "app/beacon_detail.h"



//@TODO BG�ǂݍ��� �Ƃ肠�����}�C�N�e�X�g�̃��\�[�X
#include "message.naix"
#include "msg/msg_beacon_detail.h"  // GMM
#include "beacon_status.naix"		// �^�b�`�o�[�J�X�^���{�^���p�T���v���Ƀ^�E���}�b�v���\�[�X
#include "townmap_gra.naix"		// �^�b�`�o�[�J�X�^���{�^���p�T���v���Ƀ^�E���}�b�v���\�[�X
#include "wifi_unionobj.naix"
#include "wmi.naix"

#include "beacon_detail_local.h"
#include "beacon_detail_sub.h"

static void draw_BeaconWindowIni( BEACON_DETAIL_WORK* wk );


/*
 *  @brief  �r�[�R���ڍ׉�ʏ����`��
 */
void BeaconDetail_InitialDraw( BEACON_DETAIL_WORK* wk )
{
  draw_BeaconWindowIni( wk );

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
    GFL_MSG_GetString( wk->msg, msg_prof01+i, wk->str_tmp);
  
    for(j = 0;j < BEACON_WIN_MAX;j++){
      bp = &wk->beacon_win[j];

      PRINT_UTIL_PrintColor( &bp->prof[i].putil, wk->print_que,
          0, 0, wk->str_tmp, wk->font, FCOL_WIN01 );
      GFL_BMPWIN_MakeTransWindow( bp->prof[i].win );
    }
  }
  //���R�[�h���o��
  GFL_MSG_GetString( wk->msg, msg_record, wk->str_tmp);
  
  for(i = 0;i < BEACON_WIN_MAX;i++){
    bp = &wk->beacon_win[i];

    PRINT_UTIL_PrintColor( &bp->record.putil, wk->print_que,
          0, 0, wk->str_tmp, wk->font, FCOL_WIN01 );
    GFL_BMPWIN_MakeTransWindow( bp->record.win );
  }
}

/*
 *  @brief  �r�[�R���E�B���h�E�`��
 */
static void draw_BeaconWindow( BEACON_DETAIL_WORK* wk, u8 idx )
{
  BEACON_WIN* bp = &wk->beacon_win[idx];


}
