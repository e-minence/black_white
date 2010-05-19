//============================================================================================
/**
 * @file		b_bag_main.c
 * @brief		�퓬�p�o�b�O���
 * @author	Hiroyuki Nakamura
 * @date		05.02.10
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "sound/pm_sndsys.h"
#include "font/font.naix"

#include "../btlv/btlv_effect.h"
#include "msg/msg_b_bag.h"

#include "b_app_tool.h"

#include "b_bag.h"
#include "b_bag_main.h"
#include "b_bag_bmp.h"
#include "b_bag_item.h"
#include "b_bag_obj.h"
#include "b_bag_anm.h"
#include "b_bag_ui.h"
#include "b_bag_bmp_def.h"
#include "b_bag_gra.naix"


//============================================================================================
//  �萔��`
//============================================================================================
// ���C���V�[�P���X
enum {
  SEQ_BBAG_INIT = 0,      // ������
  SEQ_BBAG_SHOOTER_INIT,  // �������i�V���[�^�[�p�j
  SEQ_BBAG_POCKET,        // �|�P�b�g�I��
  SEQ_BBAG_ITEM,          // �A�C�e���I��
  SEQ_BBAG_USE,           // �A�C�e���g�p

  SEQ_BBAG_PAGE1_CHG,   // �|�P�b�g�I����
  SEQ_BBAG_PAGE2_CHG,   // �A�C�e���I����
  SEQ_BBAG_PAGE3_CHG,   // �A�C�e���g�p��

  SEQ_BBAG_PAGECHG_WAIT,  // �y�[�W�؂�ւ��҂�

  SEQ_BBAG_ITEMSEL_NEXT,  // ���̃A�C�e���y�[�W��
  SEQ_BBAG_ITEMSEL_WAIT,  // �A�C�e���y�[�W�؂�ւ��҂�

  SEQ_BBAG_ERR,     // �G���[���b�Z�[�W�I���҂�
  SEQ_BBAG_MSG_WAIT,    // ���b�Z�[�W�\��
  SEQ_BBAG_TRG_WAIT,    // �g���K�[�E�F�C�g

  SEQ_BBAG_BUTTON_WAIT, // �{�^���A�j���I���҂�

  SEQ_BBAG_GETDEMO,   // �ߊl�f���p

  SEQ_BBAG_ENDSET,    // �I���t�F�[�h�Z�b�g
  SEQ_BBAG_ENDWAIT,   // �I���t�F�[�h�҂�
  SEQ_BBAG_END        // �I��
};

#define TMP_MSG_BUF_SIZ   ( 512 )   // �e���|�������b�Z�[�W�T�C�Y

#define BATTLE_BAGLIST_FADE_SPEED (0)


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static void BattleBag_Main( GFL_TCB* tcb, void * work );

static int BBAG_SeqInit( BBAG_WORK * wk );
static int BBAG_SeqShooterInit( BBAG_WORK * wk );
static int BBAG_SeqPokeSelect( BBAG_WORK * wk );
static int BBAG_SeqItemSelect( BBAG_WORK * wk );
static int BBAG_SeqUseSelect( BBAG_WORK * wk );
static int BBAG_SeqPage1Chg( BBAG_WORK * wk );
static int BBAG_SeqPage2Chg( BBAG_WORK * wk );
static int BBAG_SeqPage3Chg( BBAG_WORK * wk );
static int BBAG_SeqPageChgWait( BBAG_WORK * wk );
static int BBAG_SeqItemSelNext( BBAG_WORK * wk );
static int BBAG_SeqItemSelWait( BBAG_WORK * wk );
static int BBAG_SeqError( BBAG_WORK * wk );
static int BBAG_SeqMsgWait( BBAG_WORK * wk );
static int BBAG_SeqTrgWait( BBAG_WORK * wk );
static int BBAG_SeqButtonWait( BBAG_WORK * wk );
static int BBAG_SeqGetDemoMain( BBAG_WORK * wk );
static int BBAG_SeqEndSet( BBAG_WORK * wk );
static int BBAG_SeqEndWait( BBAG_WORK * wk );
static BOOL BBAG_SeqEnd( GFL_TCB* tcb, BBAG_WORK * wk );

static void BBAG_BgInit( BBAG_WORK * wk );
static void BBAG_BgExit(void);
static void BBAG_BgGraphicSet( BBAG_WORK * wk );
static void BBAG_MsgManSet( BBAG_WORK * wk );
static void BBAG_MsgManExit( BBAG_WORK * wk );
static int BBAG_ItemUse( BBAG_WORK * wk );

static void BBAG_PageChgBgScroll( BBAG_WORK * wk, u8 page );
static void BBAG_PageChange( BBAG_WORK * wk, u8 next_page );

static BOOL CheckTimeOut( BBAG_WORK * wk );

static void PlaySE( BBAG_WORK * wk, int no );


//============================================================================================
//  �f�[�^
//============================================================================================

// ���C���V�[�P���X
static const pBBagFunc MainSeqFunc[] = {
  BBAG_SeqInit,
  BBAG_SeqShooterInit,
  BBAG_SeqPokeSelect,
  BBAG_SeqItemSelect,
  BBAG_SeqUseSelect,

  BBAG_SeqPage1Chg,
  BBAG_SeqPage2Chg,
  BBAG_SeqPage3Chg,

  BBAG_SeqPageChgWait,

  BBAG_SeqItemSelNext,
  BBAG_SeqItemSelWait,

  BBAG_SeqError,
  BBAG_SeqMsgWait,
  BBAG_SeqTrgWait,

  BBAG_SeqButtonWait,

  BBAG_SeqGetDemoMain,

  BBAG_SeqEndSet,
  BBAG_SeqEndWait,
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		�퓬�p�o�b�O�^�X�N�ǉ�
 *
 * @param		dat   �o�b�O�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_TaskAdd( BBAG_DATA * dat )
{
  BBAG_WORK * wk;

  // �V���[�^�[���[�h�e�X�g
//  dat->mode = BBAG_MODE_SHOOTER;
	// �|�P�����h���[���L���b�`���[�h�e�X�g
//	dat->mode = BBAG_MODE_PDC;

  wk = GFL_HEAP_AllocClearMemory( dat->heap, sizeof(BBAG_WORK) );

  GFL_TCB_AddTask( BTLV_EFFECT_GetTCBSYS(), BattleBag_Main, wk, 100 );

  wk->dat = dat;
  wk->pfd = BTLV_EFFECT_GetPfd();

	// �V���[�^�[
  if( wk->dat->mode == BBAG_MODE_SHOOTER ){
    wk->page = BBAG_PAGE_MAIN;
    wk->seq  = SEQ_BBAG_SHOOTER_INIT;
		wk->poke_id = 0;
	// �|�P�����h���[���L���b�`
	}else if( wk->dat->mode == BBAG_MODE_PDC ){
    wk->page = BBAG_PAGE_MAIN;
    wk->seq  = SEQ_BBAG_INIT;
		wk->poke_id = BBAG_POKE_BALL;
	// ���̑�
  }else{
    wk->page = BBAG_PAGE_POCKET;
    wk->seq  = SEQ_BBAG_INIT;
		wk->poke_id = 0;
  }

  {
    u32 i;

    for( i=0; i<BATTLE_BAG_POKE_MAX; i++ ){
      MYITEM_BattleBagCursorGet( wk->dat->bagcursor, i, &wk->dat->item_pos[i], &wk->dat->item_scr[i] );
    }
    wk->used_item = MYITEM_BattleBagLastItemGet( wk->dat->bagcursor );
    wk->used_poke = MYITEM_BattleBagLastPageGet( wk->dat->bagcursor );
  }

  BattleBag_UsedItemChack( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���^�X�N
 *
 * @param		tcb
 * @param		work
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BattleBag_Main( GFL_TCB * tcb, void * work )
{
  BBAG_WORK * wk = (BBAG_WORK *)work;

	if( wk->dat->comm_err_flg == TRUE ){
		wk->seq = SEQ_BBAG_END;
	}

  if( wk->seq != SEQ_BBAG_END ){
    wk->seq = MainSeqFunc[wk->seq]( wk );
  }

  if( wk->seq == SEQ_BBAG_END ){
    if( BBAG_SeqEnd( tcb, wk ) == TRUE ){
      return;
    }
  }

  GFL_TCBL_Main( wk->tcbl );

  BBAGANM_ButtonAnmMain( wk );
  BGWINFRM_MoveMain( wk->bgwfrm );

  BBAGBMP_PrintMain( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�������V�[�P���X�i�ʏ�j
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqInit( BBAG_WORK * wk )
{
  G2S_BlendNone();

  wk->tcbl = GFL_TCBL_Init( wk->dat->heap, wk->dat->heap, 1, 4 );

  wk->cpwk = BAPPTOOL_CreateCursor( wk->dat->heap );

  BBAG_BgInit( wk );
  BBAG_BgGraphicSet( wk );
  BBAG_MsgManSet( wk );

  if( wk->dat->mode == BBAG_MODE_GETDEMO ){
		BattleBag_GetDemoPocketInit( wk );
	}else{
	  BattleBag_PocketInit( wk );
	}

  BattleBag_BmpInit( wk );
  BattleBag_BmpWrite( wk, wk->page );
  BBAGBMP_SetStrScrn( wk );

  BBAGANM_ButtonInit( wk );
  BBAGANM_PageButtonPut( wk, wk->page );

  BattleBag_ObjInit( wk );
  BattleBag_PageObjSet( wk, wk->page );

  if( *wk->dat->cursor_flg == 1 ){
    wk->cursor_flg = TRUE;
  }else{
    wk->cursor_flg = FALSE;
  }
  BAPPTOOL_VanishCursor( wk->cpwk, wk->cursor_flg );

	if( wk->page == BBAG_PAGE_POCKET ){
		BBAGUI_Init( wk, wk->page, wk->used_poke );
	}else{
		BBAGUI_Init( wk, wk->page, 0 );
	}

  GFL_NET_ReloadIcon();
  PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_OBJ, 14*16, 0x20 );

  PaletteFadeReq(
    wk->pfd, PF_BIT_SUB_ALL, 0xffff, BATTLE_BAGLIST_FADE_SPEED, 16, 0, 0, BTLV_EFFECT_GetTCBSYS() );

  if( wk->dat->mode == BBAG_MODE_GETDEMO ){
    return SEQ_BBAG_GETDEMO;
  }else if( wk->dat->mode == BBAG_MODE_PDC ){
		BBAG_PageChgBgScroll( wk, wk->page );
	  return SEQ_BBAG_ITEM;
	}
  return SEQ_BBAG_POCKET;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�������V�[�P���X�i�V���[�^�[�p�j
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqShooterInit( BBAG_WORK * wk )
{
  G2S_BlendNone();

  wk->tcbl = GFL_TCBL_Init( wk->dat->heap, wk->dat->heap, 1, 4 );

  wk->cpwk = BAPPTOOL_CreateCursor( wk->dat->heap );

  BBAG_BgInit( wk );
  BBAG_BgGraphicSet( wk );
  BBAG_MsgManSet( wk );

  BattleBag_ShooterPocketInit( wk );

  BBAG_PageChgBgScroll( wk, wk->page );

  BattleBag_BmpInit( wk );
  BattleBag_BmpWrite( wk, wk->page );
  BBAGBMP_SetStrScrn( wk );

  BBAGANM_ButtonInit( wk );
  BBAGANM_PageButtonPut( wk, wk->page );

  BattleBag_ObjInit( wk );
  BattleBag_PageObjSet( wk, wk->page );

  BBAGUI_Init( wk, wk->page, 0 );

  GFL_NET_ReloadIcon();
  PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_OBJ, 14*16, 0x20 );

  PaletteFadeReq(
    wk->pfd, PF_BIT_SUB_ALL, 0xffff, BATTLE_BAGLIST_FADE_SPEED, 16, 0, 0, BTLV_EFFECT_GetTCBSYS() );

  return SEQ_BBAG_ITEM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�P�̃R���g���[���V�[�P���X
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqPokeSelect( BBAG_WORK * wk )
{
  if( PaletteFadeCheck( wk->pfd ) == 0 ){
    u32 ret;
		
		if( CheckTimeOut( wk ) == TRUE ){
      return SEQ_BBAG_ENDSET;
		}

		ret = CURSORMOVE_MainCont( wk->cmwk );

    switch( ret ){
    case BBAG_UI_P1_HP_POCKET:        // HP�񕜃|�P�b�g
    case BBAG_UI_P1_STATUS_POCKET:    // ��ԉ񕜃|�P�b�g
    case BBAG_UI_P1_BALL_POCKET:      // �{�[���|�P�b�g
    case BBAG_UI_P1_BATTLE_POCKET:    // �퓬�p�|�P�b�g
      PlaySE( wk, SEQ_SE_DECIDE2 );
      wk->poke_id = (u8)ret;
      wk->ret_seq = SEQ_BBAG_PAGE2_CHG;
      BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_POCKET01+ret-BBAG_POKE_HPRCV );
      return SEQ_BBAG_BUTTON_WAIT;

    case BBAG_UI_P1_LAST_ITEM:    // �Ō�Ɏg�p��������
      if( wk->used_item != ITEM_DUMMY_DATA ){
        PlaySE( wk, SEQ_SE_DECIDE2 );
        wk->poke_id = wk->used_poke;
        wk->ret_seq = SEQ_BBAG_PAGE3_CHG;
        BattleBag_CorsorReset( wk );
        BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_USE );
        return SEQ_BBAG_BUTTON_WAIT;
      }
      break;

    case BBAG_UI_P1_RETURN:   // �߂�
			PlaySE( wk, SEQ_SE_CANCEL2 );
      wk->dat->ret_item = ITEM_DUMMY_DATA;
      wk->dat->ret_page = BBAG_POKE_MAX;
      BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_RETURN );
      return SEQ_BBAG_ENDSET;

    case CURSORMOVE_CANCEL:   // �L�����Z��
			PlaySE( wk, SEQ_SE_CANCEL2 );
      wk->dat->ret_item = ITEM_DUMMY_DATA;
      wk->dat->ret_page = BBAG_POKE_MAX;
      BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_RETURN );
      return SEQ_BBAG_ENDSET;

    case CURSORMOVE_CURSOR_MOVE:    // �ړ�
      PlaySE( wk, SEQ_SE_SELECT1 );
      break;

    case CURSORMOVE_NO_MOVE_UP:     // �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
    case CURSORMOVE_NO_MOVE_DOWN:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
    case CURSORMOVE_NO_MOVE_LEFT:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
    case CURSORMOVE_NO_MOVE_RIGHT:  // �\���L�[�E�������ꂽ���A�ړ��Ȃ�
    case CURSORMOVE_CURSOR_ON:      // �J�[�\���\��
    case CURSORMOVE_NONE:           // ����Ȃ�
      break;
    }
  }

  return SEQ_BBAG_POCKET;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�Q�̃R���g���[���V�[�P���X
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqItemSelect( BBAG_WORK * wk )
{
  u32 ret;
	
	if( CheckTimeOut( wk ) == TRUE ){
		return SEQ_BBAG_ENDSET;
	}

	ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BBAG_UI_P2_ITEM1:    // �A�C�e���P
  case BBAG_UI_P2_ITEM2:    // �A�C�e���Q
  case BBAG_UI_P2_ITEM3:    // �A�C�e���R
  case BBAG_UI_P2_ITEM4:    // �A�C�e���S
  case BBAG_UI_P2_ITEM5:    // �A�C�e���T
  case BBAG_UI_P2_ITEM6:    // �A�C�e���U
    if( BattleBag_PosItemCheck( wk, ret ) != 0 ){
      PlaySE( wk, SEQ_SE_DECIDE2 );
      wk->dat->item_pos[wk->poke_id] = (u8)ret;
      wk->ret_seq = SEQ_BBAG_PAGE3_CHG;
      BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_ITEM01+ret );
      return SEQ_BBAG_BUTTON_WAIT;
    }
    break;

  case BBAG_UI_P2_RETURN:   // �߂�
		PlaySE( wk, SEQ_SE_CANCEL2 );
    BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_RETURN );
    if( wk->dat->mode == BBAG_MODE_SHOOTER || wk->dat->mode == BBAG_MODE_PDC ){
      wk->dat->ret_item = ITEM_DUMMY_DATA;
      wk->dat->ret_page = BBAG_POKE_MAX;
      return SEQ_BBAG_ENDSET;
    }else{
      wk->ret_seq = SEQ_BBAG_PAGE1_CHG;
      return SEQ_BBAG_BUTTON_WAIT;
    }
    break;

  case CURSORMOVE_CANCEL:   // �L�����Z��
		PlaySE( wk, SEQ_SE_CANCEL2 );
    BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_RETURN );
    if( wk->dat->mode == BBAG_MODE_SHOOTER || wk->dat->mode == BBAG_MODE_PDC ){
      wk->dat->ret_item = ITEM_DUMMY_DATA;
      wk->dat->ret_page = BBAG_POKE_MAX;
      return SEQ_BBAG_ENDSET;
    }else{
      wk->ret_seq = SEQ_BBAG_PAGE1_CHG;
      return SEQ_BBAG_BUTTON_WAIT;
    }
    break;

  case BBAG_UI_P2_LEFT:     // �O��
    if( wk->scr_max[wk->poke_id] != 0 ){
      PlaySE( wk, SEQ_SE_DECIDE2 );
      wk->dat->item_pos[wk->poke_id] = 0;
      wk->ret_seq = SEQ_BBAG_ITEMSEL_NEXT;
      wk->page_mv = -1;
      BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_LEFT );
      return SEQ_BBAG_BUTTON_WAIT;
    }
    break;

  case BBAG_UI_P2_RIGHT:    // ����
    if( wk->scr_max[wk->poke_id] != 0 ){
      PlaySE( wk, SEQ_SE_DECIDE2 );
      wk->dat->item_pos[wk->poke_id] = 0;
      wk->ret_seq = SEQ_BBAG_ITEMSEL_NEXT;
      wk->page_mv = 1;
      BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_RIGHT );
      return SEQ_BBAG_BUTTON_WAIT;
    }
    break;

  case CURSORMOVE_CURSOR_MOVE:    // �ړ�
    PlaySE( wk, SEQ_SE_SELECT1 );
    break;

  case CURSORMOVE_NO_MOVE_UP:     // �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_DOWN:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_CURSOR_ON:      // �J�[�\���\��
  case CURSORMOVE_NONE:           // ����Ȃ�
    break;

  case CURSORMOVE_NO_MOVE_LEFT:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
    {
      u8  pos = CURSORMOVE_PosGet( wk->cmwk );
      if( pos == BBAG_UI_P2_ITEM1 || pos == BBAG_UI_P2_ITEM3 || BBAG_UI_P2_ITEM5 ){
        if( wk->scr_max[wk->poke_id] != 0 ){
        PlaySE( wk, SEQ_SE_DECIDE2 );
          wk->ret_seq = SEQ_BBAG_ITEMSEL_NEXT;
          wk->page_mv = -1;
          BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_LEFT );
          return SEQ_BBAG_BUTTON_WAIT;
        }
      }
    }
    break;

  case CURSORMOVE_NO_MOVE_RIGHT:  // �\���L�[�E�������ꂽ���A�ړ��Ȃ�
    {
      u8  pos = CURSORMOVE_PosGet( wk->cmwk );
      if( pos == BBAG_UI_P2_ITEM2 || pos == BBAG_UI_P2_ITEM4 || BBAG_UI_P2_ITEM6 ){
        if( wk->scr_max[wk->poke_id] != 0 ){
        PlaySE( wk, SEQ_SE_DECIDE2 );
          wk->ret_seq = SEQ_BBAG_ITEMSEL_NEXT;
          wk->page_mv = 1;
          BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_RIGHT );
          return SEQ_BBAG_BUTTON_WAIT;
        }
      }
    }
    break;
  }

  return SEQ_BBAG_ITEM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���̃A�C�e���y�[�W��
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqItemSelNext( BBAG_WORK * wk )
{
  s8  scr = wk->dat->item_scr[wk->poke_id];

  scr += wk->page_mv;
  if( scr > wk->scr_max[wk->poke_id] ){
    wk->dat->item_scr[wk->poke_id] = 0;
  }else if( scr < 0 ){
    wk->dat->item_scr[wk->poke_id] = wk->scr_max[wk->poke_id];
  }else{
    wk->dat->item_scr[wk->poke_id] = scr;
  }
  BattleBag_Page2_StrItemPut( wk );
  BattleBag_Page2_StrPageNumPut( wk );
  return SEQ_BBAG_ITEMSEL_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���̃A�C�e���y�[�W�ցi�L���[�҂��j
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqItemSelWait( BBAG_WORK * wk )
{
  if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
    BBAGBMP_SetStrScrn( wk );
    BattleBag_PageObjSet( wk, wk->page );
    BBAGANM_PageButtonPut( wk, wk->page );
    return SEQ_BBAG_ITEM;
  }
  return SEQ_BBAG_ITEMSEL_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�R�̃R���g���[���V�[�P���X
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqUseSelect( BBAG_WORK * wk )
{
  u32 ret;

	if( CheckTimeOut( wk ) == TRUE ){
		return SEQ_BBAG_ENDSET;
	}

	ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BBAG_UI_P3_USE:    // �g��
    PlaySE( wk, SEQ_SE_DECIDE2 );
    wk->dat->ret_item = BattleBag_PosItemCheck( wk, wk->dat->item_pos[wk->poke_id] );
    wk->dat->ret_page = wk->poke_id;
    if( wk->dat->mode == BBAG_MODE_SHOOTER ){
      wk->dat->ret_cost = (u8)BBAGITEM_GetCost( wk->dat->ret_item );
    }else{
      wk->dat->ret_cost = 0;
    }
    BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_USE );
    return BBAG_ItemUse( wk );

  case BBAG_UI_P3_RETURN:   // �߂�
		PlaySE( wk, SEQ_SE_CANCEL2 );
    wk->ret_seq = SEQ_BBAG_PAGE2_CHG;
    BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_RETURN );
    return SEQ_BBAG_BUTTON_WAIT;

  case CURSORMOVE_CANCEL:   // �L�����Z��
		PlaySE( wk, SEQ_SE_CANCEL2 );
    wk->ret_seq = SEQ_BBAG_PAGE2_CHG;
    BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_RETURN );
    return SEQ_BBAG_BUTTON_WAIT;

  case CURSORMOVE_CURSOR_MOVE:    // �ړ�
    PlaySE( wk, SEQ_SE_SELECT1 );
    break;

  case CURSORMOVE_NO_MOVE_UP:     // �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_DOWN:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_LEFT:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_RIGHT:  // �\���L�[�E�������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_CURSOR_ON:      // �J�[�\���\��
  case CURSORMOVE_NONE:           // ����Ȃ�
      break;
  }

  return SEQ_BBAG_USE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�A�C�e���g�p
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_ItemUse( BBAG_WORK * wk )
{
  BBAG_DATA * dat = wk->dat;

  // �V���[�^�[��
  if( dat->mode == BBAG_MODE_SHOOTER ){
    // �G�l���M�[������Ȃ�
    if( dat->energy < BBAGITEM_GetCost( dat->ret_item ) ){
      GFL_MSG_GetString( wk->mman, mes_b_bag_m16, wk->msg_buf );
      BattleBag_TalkMsgSet( wk );
      wk->ret_seq = SEQ_BBAG_ERR;
      return SEQ_BBAG_MSG_WAIT;
    }
  }

	// �{�[���g�p�`�F�b�N
	if( wk->poke_id == BBAG_POKE_BALL ){
		// �莝���E�{�b�N�X�ɋ󂫂��Ȃ�
		if( dat->ball_use == BBAG_BALLUSE_POKEMAX ){
      GFL_MSG_GetString( wk->mman, mes_b_bag_m12, wk->msg_buf );
			BattleBag_TalkMsgSet( wk );
			wk->ret_seq = SEQ_BBAG_ERR;
			return SEQ_BBAG_MSG_WAIT;
		}
		// �쐶�_�u���E�G���Q�C�̂��ߎg�p�s��
		if( dat->ball_use == BBAG_BALLUSE_DOUBLE ){
      GFL_MSG_GetString( wk->mman, mes_b_bag_m11, wk->msg_buf );
			BattleBag_TalkMsgSet( wk );
			wk->ret_seq = SEQ_BBAG_ERR;
			return SEQ_BBAG_MSG_WAIT;
		}
	}

  return SEQ_BBAG_ENDSET;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�؂�ւ��F�|�P�b�g�I���y�[�W��
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqPage1Chg( BBAG_WORK * wk )
{
  BattleBag_BmpWrite( wk, BBAG_PAGE_POCKET );
  wk->page = BBAG_PAGE_POCKET;
  wk->ret_seq = SEQ_BBAG_POCKET;
  return SEQ_BBAG_PAGECHG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�؂�ւ��F�A�C�e���I���y�[�W��
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqPage2Chg( BBAG_WORK * wk )
{
  BattleBag_BmpWrite( wk, BBAG_PAGE_MAIN );
  wk->page = BBAG_PAGE_MAIN;
  wk->ret_seq = SEQ_BBAG_ITEM;
  return SEQ_BBAG_PAGECHG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�؂�ւ��F�A�C�e���g�p�y�[�W��
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqPage3Chg( BBAG_WORK * wk )
{
  BattleBag_BmpWrite( wk, BBAG_PAGE_ITEM );
  wk->page = BBAG_PAGE_ITEM;
  wk->ret_seq = SEQ_BBAG_USE;
  return SEQ_BBAG_PAGECHG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�؂�ւ��F�A�C�e���g�p�y�[�W�ցi�L���[�҂��j
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqPageChgWait( BBAG_WORK * wk )
{
  if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
    BBAG_PageChange( wk, wk->page );
    return wk->ret_seq;
  }
  return SEQ_BBAG_PAGECHG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		����ւ��G���[���b�Z�[�W�I���҂��V�[�P���X
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqError( BBAG_WORK * wk )
{
  BmpWinFrame_Clear( wk->talk_win, WINDOW_TRANS_ON );
  return SEQ_BBAG_USE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�\���V�[�P���X
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqMsgWait( BBAG_WORK * wk )
{
  if( PRINTSYS_PrintStreamGetState(wk->stream) == PRINTSTREAM_STATE_DONE ){
    PRINTSYS_PrintStreamDelete( wk->stream );
		wk->stream = NULL;
    return SEQ_BBAG_TRG_WAIT;
  }
  return SEQ_BBAG_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�g���K�[�E�F�C�g�V�[�P���X
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqTrgWait( BBAG_WORK * wk )
{
	if( CheckTimeOut( wk ) == TRUE ){
		return SEQ_BBAG_ENDSET;
	}

  if( ( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ) || GFL_UI_TP_GetTrg() == TRUE ){
    return wk->ret_seq;
  }
  return SEQ_BBAG_TRG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�^���A�j���I���҂��V�[�P���X
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqButtonWait( BBAG_WORK * wk )
{
  if( wk->btn_flg == 0 ){
    return wk->ret_seq;
  }
  return SEQ_BBAG_BUTTON_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�I���t�F�[�h�Z�b�g�V�[�P���X
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqEndSet( BBAG_WORK * wk )
{
  PaletteFadeReq(
    wk->pfd, PF_BIT_SUB_ALL, 0xffff, BATTLE_BAGLIST_FADE_SPEED, 0, 16, 0, BTLV_EFFECT_GetTCBSYS() );

  return SEQ_BBAG_ENDWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�I���t�F�[�h�҂�
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqEndWait( BBAG_WORK * wk )
{
  if( PaletteFadeCheck( wk->pfd ) == 0 ){
    return SEQ_BBAG_END;
  }
  return SEQ_BBAG_ENDWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�I���V�[�P���X
 *
 * @param		wk    ���[�N
 *
 * @retval  "TRUE = �I��"
 * @retval  "FALSE = �t�F�[�h��"
 */
//--------------------------------------------------------------------------------------------
static BOOL BBAG_SeqEnd( GFL_TCB * tcb, BBAG_WORK * wk )
{
  if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
    return FALSE;
  }

	if( wk->stream != NULL ){
		PRINTSYS_PrintStreamDelete( wk->stream );
	}

  wk->dat->end_flg = 1;
  if( wk->cursor_flg == TRUE ){
    *wk->dat->cursor_flg = 1;
  }else{
    *wk->dat->cursor_flg = 0;
  }

  BBAGANM_ButtonExit( wk );

  BattleBag_ObjFree( wk );
  BattleBag_BmpFreeAll( wk );
  BBAG_MsgManExit( wk );
  BBAG_BgExit();

  GFL_TCBL_Exit( wk->tcbl );
  BBAGUI_Exit( wk );
  BAPPTOOL_FreeCursor( wk->cpwk );

  GFL_TCB_DeleteTask( tcb );
  GFL_HEAP_FreeMemory( wk );

  return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�ߊl�f�����C��
 *
 * @param		wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BBAG_SeqGetDemoMain( BBAG_WORK * wk )
{
  switch( wk->get_seq ){
	case 0:
		if( BTLV_FINGER_CURSOR_CheckExecute( wk->getdemoCursor ) == TRUE ){
      PlaySE( wk, SEQ_SE_DECIDE2 );
      wk->poke_id = BBAG_POKE_BALL;
      wk->ret_seq = SEQ_BBAG_GETDEMO;
      BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_POCKET03 );
      wk->get_seq++;
      return SEQ_BBAG_BUTTON_WAIT;
		}
		break;

  case 1:
		if( BTLV_FINGER_CURSOR_CheckExecute( wk->getdemoCursor ) == FALSE ){
	    BBAG_SeqPage2Chg( wk );
	    wk->get_seq++;
			wk->ret_seq = SEQ_BBAG_GETDEMO;
		  return SEQ_BBAG_PAGECHG_WAIT;
		}
		break;

	case 2:
		if( BTLV_FINGER_CURSOR_CheckExecute( wk->getdemoCursor ) == TRUE ){
      PlaySE( wk, SEQ_SE_DECIDE2 );
      wk->dat->item_pos[wk->poke_id] = 0;
      wk->ret_seq = SEQ_BBAG_GETDEMO;
      BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_ITEM01 );
      wk->get_seq++;
      return SEQ_BBAG_BUTTON_WAIT;
		}
		break;

  case 3:
		if( BTLV_FINGER_CURSOR_CheckExecute( wk->getdemoCursor ) == FALSE ){
	    BBAG_SeqPage3Chg( wk );
	    wk->get_seq++;
			wk->ret_seq = SEQ_BBAG_GETDEMO;
		  return SEQ_BBAG_PAGECHG_WAIT;
		}
		break;

	case 4:
		if( BTLV_FINGER_CURSOR_CheckExecute( wk->getdemoCursor ) == TRUE ){
      PlaySE( wk, SEQ_SE_DECIDE2 );
      wk->dat->ret_item = BattleBag_PosItemCheck( wk, wk->dat->item_pos[wk->poke_id] );
      wk->dat->ret_page = wk->poke_id;
      wk->dat->ret_cost = 0;
      BBAGANM_ButtonAnmInit( wk, BBAG_BGWF_USE );
	    wk->get_seq++;
		}
		break;

	case 5:
		if( BTLV_FINGER_CURSOR_CheckExecute( wk->getdemoCursor ) == FALSE ){
      return BBAG_ItemUse( wk );
		}
  }

  return SEQ_BBAG_GETDEMO;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		BG������
 *
 * @param		wk		���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_BgInit( BBAG_WORK * wk )
{
  { // BG SYSTEM
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
    };
    GFL_BG_SetBGModeDisp( &BGsys_data, GFL_BG_SUB_DISP );
  }

  { // BG (CHAR)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xc800, GX_BG_CHARBASE_0x00000,
      0x8000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }

  { // BG (CHAR)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x2000, 0,
      GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x00000,
      0x8000,
      GX_BG_EXTPLTT_23, 2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }

  { // PARAM FONT (BMP)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000,
      0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME1_S );
  }

  { // WINDOW (BMP)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000,
      0x4000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME0_S );
  }
  GFL_BG_SetClearCharacter( GFL_BG_FRAME1_S, 32, 0, wk->dat->heap );
  GFL_BG_SetClearCharacter( GFL_BG_FRAME0_S, 32, 0, wk->dat->heap );
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_S );

  GFL_DISP_GXS_SetVisibleControl(
    GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 |
    GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3,
    VISIBLE_ON );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BG���
 *
 * @param		none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_BgExit(void)
{
  GFL_DISP_GXS_SetVisibleControl(
    GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 |
    GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ,
    VISIBLE_OFF );

  GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME2_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME3_S );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param		wk		���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_BgGraphicSet( BBAG_WORK * wk )
{
  ARCHANDLE * hdl;

  hdl = GFL_ARC_OpenDataHandle( ARCID_B_BAG_GRA, GFL_HEAP_LOWID(wk->dat->heap) );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    hdl, NARC_b_bag_gra_b_bag_bg_lz_NCGR,
    GFL_BG_FRAME2_S, 0, 0, TRUE, wk->dat->heap );

  GFL_ARCHDL_UTIL_TransVramScreen(
    hdl, NARC_b_bag_gra_b_bag_button_lz_NSCR,
    GFL_BG_FRAME2_S, 0, 0, TRUE, wk->dat->heap );

  GFL_ARCHDL_UTIL_TransVramScreen(
    hdl, NARC_b_bag_gra_b_bag_base_lz_NSCR,
    GFL_BG_FRAME3_S, 0, 0, TRUE, wk->dat->heap );

  PaletteWorkSet_ArcHandle(
      wk->pfd, hdl, NARC_b_bag_gra_b_bag_bg_NCLR,
      wk->dat->heap, FADE_SUB_BG, 0x20*7, 0 );

  GFL_ARC_CloseDataHandle( hdl );

  // ���b�Z�[�W�E�B���h�E
  BmpWinFrame_GraphicSet(
    GFL_BG_FRAME0_S, 1, BBAG_PAL_TALK_WIN, MENU_TYPE_SYSTEM, wk->dat->heap );
  PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_BG, BBAG_PAL_TALK_WIN*16, 0x20 );

  // �t�H���g�p���b�g
  PaletteWorkSet_Arc(
      wk->pfd, ARCID_FONT, NARC_font_default_nclr,
      wk->dat->heap, FADE_SUB_BG, 0x20, BBAG_PAL_TALK_FONT*16 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�֘A�Z�b�g
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_MsgManSet( BBAG_WORK * wk )
{
  wk->mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_b_bag_dat, wk->dat->heap );
  wk->wset = WORDSET_Create( wk->dat->heap );
  wk->que  = PRINTSYS_QUE_Create( wk->dat->heap );

  wk->msg_buf = GFL_STR_CreateBuffer( TMP_MSG_BUF_SIZ, wk->dat->heap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���b�Z�[�W�֘A�폜
 *
 * @param		wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_MsgManExit( BBAG_WORK * wk )
{
  GFL_MSG_Delete( wk->mman );
  WORDSET_Delete( wk->wset );
  PRINTSYS_QUE_Delete( wk->que );

  GFL_STR_DeleteBuffer( wk->msg_buf );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		BG�X�N���[��
 *
 * @param		wk    ���[�N
 * @param		page  ���̃y�[�W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_PageChgBgScroll( BBAG_WORK * wk, u8 page )
{
  switch( page ){
  case BBAG_PAGE_POCKET:    // �|�P�b�g�I���y�[�W
    GFL_BG_SetScrollReq( GFL_BG_FRAME2_S, GFL_BG_SCROLL_X_SET, 0 );
    GFL_BG_SetScrollReq( GFL_BG_FRAME2_S, GFL_BG_SCROLL_Y_SET, 0 );
    break;
  case BBAG_PAGE_MAIN:    // �A�C�e���I���y�[�W
    GFL_BG_SetScrollReq( GFL_BG_FRAME2_S, GFL_BG_SCROLL_X_SET, 256 );
    GFL_BG_SetScrollReq( GFL_BG_FRAME2_S, GFL_BG_SCROLL_Y_SET, 0 );
    break;
  case BBAG_PAGE_ITEM:    // �A�C�e���g�p�y�[�W
    GFL_BG_SetScrollReq( GFL_BG_FRAME2_S, GFL_BG_SCROLL_X_SET, 0 );
    GFL_BG_SetScrollReq( GFL_BG_FRAME2_S, GFL_BG_SCROLL_Y_SET, 256 );
    break;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�؂�ւ�
 *
 * @param		wk					���[�N
 * @param		next_page		���̃y�[�W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_PageChange( BBAG_WORK * wk, u8 next_page )
{
  BBAG_PageChgBgScroll( wk, next_page );

  GFL_BG_ClearScreenCodeVReq( GFL_BG_FRAME0_S, 0 );
  GFL_BG_ClearScreenCodeVReq( GFL_BG_FRAME1_S, 0 );

  BBAGBMP_SetStrScrn( wk );

  BBAGANM_PageButtonPut( wk, next_page );

  BBAGUI_ChangePage( wk, next_page, 0 );

  BattleBag_PageObjSet( wk, next_page );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�����I���`�F�b�N
 *
 * @param		wk		���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static BOOL CheckTimeOut( BBAG_WORK * wk )
{
	if( wk->dat->time_out_flg == TRUE ){
		wk->dat->ret_item = ITEM_DUMMY_DATA;
		wk->dat->ret_cost = 0;
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�r�d�Đ�
 *
 * @param		wk		���[�N
 * @param		no		�r�d�ԍ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PlaySE( BBAG_WORK * wk, int no )
{
	if( wk->dat->commFlag == FALSE ){
		PMSND_PlaySE( no );
	}
}
