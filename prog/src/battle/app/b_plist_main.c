//============================================================================================
/**
 * @file    b_plist_main.c
 * @brief   �퓬�p�|�P�������X�g���
 * @author  Hiroyuki Nakamura
 * @date    05.02.01
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "item/item.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "sound/pm_sndsys.h"
#include "font/font.naix"
#include "waza_tool/wazadata.h"
#include "poke_tool/gauge_tool.h"
#include "app/app_menu_common.h"

#include "../btlv/btlv_effect.h"

#include "../../field/field_skill_check.h"

#include "msg/msg_b_plist.h"

#include "b_app_tool.h"

#include "b_plist.h"
#include "b_plist_main.h"
#include "b_plist_obj.h"
#include "b_plist_anm.h"
#include "b_plist_ui.h"
#include "b_plist_bmp.h"
#include "b_plist_bmp_def.h"
#include "b_plist_gra.naix"


//============================================================================================
//  �萔��`
//============================================================================================
// ���C���V�[�P���X
enum {
  SEQ_BPL_INIT = 0,   // ������
  SEQ_BPL_SELECT,     // �|�P�����I����ʃR���g���[��
  SEQ_BPL_IREKAE,     // ����ւ��y�[�W�R���g���[��
  SEQ_BPL_ST_MAIN,    // �X�e�[�^�X���C����ʃR���g���[��
  SEQ_BPL_ST_WAZASEL,   // �X�e�[�^�X�Z�I����ʃR���g���[��
  SEQ_BPL_ST_SKILL,   // �X�e�[�^�X�Z��ʃR���g���[��

  SEQ_BPL_POKESEL_DEAD,   // �m������ւ��I���R���g���[��
  SEQ_BPL_POKECHG_DEAD,   // �m������ւ��A�j��
  SEQ_BPL_DEAD_ERR_RET,

  SEQ_BPL_PAGE1_CHG,         // �|�P�����I����
  SEQ_BPL_PAGECHG_IREKAE,   // ����ւ��y�[�W��
  SEQ_BPL_PAGECHG_STMAIN,   // �X�e�[�^�X�ڍׂ�
  SEQ_BPL_PAGECHG_WAZASEL,  // �X�e�[�^�X�Z�I����
  SEQ_BPL_PAGECHG_WAZAINFO, // �X�e�[�^�X�Z�ڍ׉�ʂ�
  SEQ_BPL_PAGECHG_WAZASET_S,  // �Z�Y��I����ʂ�
  SEQ_BPL_PAGECHG_WAZASET_I,  // �Z�Y�ꌈ���
  SEQ_BPL_PAGECHG_PPRCV,    // �Z�񕜂�
  SEQ_BPL_PAGECHG_DEAD,    // �m������ւ��I����

  SEQ_BPL_POKECHG,    // �|�P�����؂�ւ�
  SEQ_BPL_CHG_ERR_SET,  // ����ւ��G���[�Z�b�g
  SEQ_BPL_IREKAE_ERR,   // ����ւ��G���[�I���҂�

  SEQ_BPL_MSG_WAIT,   // ���b�Z�[�W�\��
  SEQ_BPL_TRG_WAIT,   // ���b�Z�[�W�\����̃L�[�҂�

  SEQ_BPL_WAZADEL_SEL,    // �Z�Y��I��
  SEQ_BPL_WAZADEL_MAIN,   // �Z�Y�ꌈ��
  SEQ_BPL_WAZADEL_ERROR,  // �Z�Y��G���[

  SEQ_BPL_WAZARCV_SEL,  // �Z�񕜑I��

  SEQ_BPL_BUTTON_WAIT,  // �{�^���A�j���I���҂�

	SEQ_BPL_WAZAINFOMODE_MAIN,

//  SEQ_BPL_STRCV,      // �X�e�[�^�X��
//  SEQ_BPL_PPALLRCV,   // PP�S��

  SEQ_BPL_ENDSET,     // �I���t�F�[�h�Z�b�g
  SEQ_BPL_ENDWAIT,    // �I���t�F�[�h�҂�
  SEQ_BPL_END,        // �I��
};

#define TMP_MSG_BUF_SIZ   ( 512 )   // �e���|�������b�Z�[�W�T�C�Y

#define PLATE_BG_SX   ( 16 )    // �v���[�gBG�T�C�YX
#define PLATE_BG_SY   ( 6 )   // �v���[�gBG�T�C�YY
#define PLATE_BG_P_PX ( 0 )   // �|�P����������ꍇ�̃v���[�gBG�Q��X���W
#define PLATE_BG_P_PY ( 0 )   // �|�P����������ꍇ�̃v���[�gBG�Q��Y���W
#define PLATE_BG_N_PX ( 16 )    // �|�P���������Ȃ��ꍇ�̃v���[�gBG�Q��X���W
#define PLATE_BG_N_PY ( 13 )    // �|�P���������Ȃ��ꍇ�̃v���[�gBG�Q��Y���W

#define PLATE_POKE1_PX  ( 0 )   // �|�P�����P�̃v���[�gBGX���W
#define PLATE_POKE1_PY  ( 0 )   // �|�P�����P�̃v���[�gBGY���W
#define PLATE_POKE2_PX  ( 16 )    // �|�P�����Q�̃v���[�gBGX���W
#define PLATE_POKE2_PY  ( 1 )   // �|�P�����Q�̃v���[�gBGY���W
#define PLATE_POKE3_PX  ( 0 )   // �|�P�����R�̃v���[�gBGX���W
#define PLATE_POKE3_PY  ( 6 )   // �|�P�����R�̃v���[�gBGY���W
#define PLATE_POKE4_PX  ( 16 )    // �|�P�����S�̃v���[�gBGX���W
#define PLATE_POKE4_PY  ( 7 )   // �|�P�����S�̃v���[�gBGY���W
#define PLATE_POKE5_PX  ( 0 )   // �|�P�����T�̃v���[�gBGX���W
#define PLATE_POKE5_PY  ( 12 )    // �|�P�����T�̃v���[�gBGY���W
#define PLATE_POKE6_PX  ( 16 )    // �|�P�����U�̃v���[�gBGX���W
#define PLATE_POKE6_PY  ( 13 )    // �|�P�����U�̃v���[�gBGY���W

#define P2_EXPGAGE_PX ( 32+14 ) // �y�[�W�Q�̌o���l�Q�[�W��������X���W
#define P2_EXPGAGE_PY ( 5 )   // �y�[�W�Q�̌o���l�Q�[�W��������Y���W
#define P3_EXPGAGE_PX ( 10 )    // �y�[�W�R�̌o���l�Q�[�W��������X���W
#define P3_EXPGAGE_PY ( 8 )   // �y�[�W�R�̌o���l�Q�[�W��������Y���W

#define EXP_CGX     ( 0x16 )  // �o���l�Q�[�W�L�����ԍ�
#define EXP_DOT_CHR_MAX ( 8 )   // �o���l�Q�[�W�̃L������
#define EXP_DOT_MAX   ( 64 )    // �o���l�Q�[�W�̍ő�h�b�g��

//#define BATTLE_BAGLIST_FADE_SPEED (-8)
#define BATTLE_BAGLIST_FADE_SPEED (0)


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static void BattlePokeList_Main( GFL_TCB* tcb, void * work );

static int BPL_SeqInit( BPLIST_WORK * wk );
static int BPL_SeqPokeSelect( BPLIST_WORK * wk );
static int BPL_SeqPokeIrekae( BPLIST_WORK * wk );
static int BPL_SeqStatusMain( BPLIST_WORK * wk );
static int BPL_SeqChgErrSet( BPLIST_WORK * wk );
static int BPL_SeqIrekaeErr( BPLIST_WORK * wk );
static int BPL_SeqMsgWait( BPLIST_WORK * wk );
static int BPL_SeqTrgWait( BPLIST_WORK * wk );
static int BPL_SeqPokeChange( BPLIST_WORK * wk );
static int BPL_SeqStInfoWaza( BPLIST_WORK * wk );
static int BPL_SeqWazaDelSelect( BPLIST_WORK * wk );
static int BPL_SeqWazaDelMain( BPLIST_WORK * wk );
static int BPL_SeqWazaDelError( BPLIST_WORK * wk );
static int BPL_SeqWazaRcvSelect( BPLIST_WORK * wk );
static int BPL_SeqButtonWait( BPLIST_WORK * wk );
static int BPL_SeqPage1Chg( BPLIST_WORK * wk );
static int BPL_SeqPageChgIrekae( BPLIST_WORK * wk );
static int BPL_SeqPageChgStWazaSel( BPLIST_WORK * wk );
static int BPL_SeqPageChgStatus( BPLIST_WORK * wk );
static int BPL_SeqPageChgWazaInfo( BPLIST_WORK * wk );
static int BPL_SeqPageChgWazaSetSel( BPLIST_WORK * wk );
static int BPL_SeqPageChgWazaSetEnter( BPLIST_WORK * wk );
static int BPL_SeqPageChgPPRcv( BPLIST_WORK * wk );
//static int BPL_SeqStRcv( BPLIST_WORK * wk );
//static int BPL_SeqPPAllRcv( BPLIST_WORK * wk );
static int BPL_SeqEndSet( BPLIST_WORK * wk );
static int BPL_SeqEndWait( BPLIST_WORK * wk );
static BOOL BPL_SeqEnd( GFL_TCB* tcb, BPLIST_WORK * wk );
static int BPL_SeqWazaSelect( BPLIST_WORK * wk );
static int BPL_SeqWazaInfoModeMain( BPLIST_WORK * wk );

static int BPL_PokeItemUse( BPLIST_WORK * wk );

static void BPL_BgInit( BPLIST_WORK * dat );
static void BPL_BgExit(void);
static void BPL_BgGraphicSet( BPLIST_WORK * wk );
static void BPL_MsgManSet( BPLIST_WORK * wk );
static void BPL_MsgManExit( BPLIST_WORK * wk );
static void BPL_PokeDataMake( BPLIST_WORK * wk );

static u8 BPL_PokemonSelect( BPLIST_WORK * wk );
//static int BPL_TPCheck( BPLIST_WORK * wk, const GFL_UI_TP_HITTBL * tbl );
static BOOL BPL_PageChange( BPLIST_WORK * wk, u8 next_page );
static void BPL_PageChgBgScreenChg( BPLIST_WORK * wk, u8 page );
static u8 BPL_IrekaeCheck( BPLIST_WORK * wk );
static u8 BPL_NextPokeGet( BPLIST_WORK * wk, s32 pos, s32 mv );
static void BPL_PokePlateSet( BPLIST_WORK * wk );
static void BPL_ExpGagePut( BPLIST_WORK * wk, u8 page );
//static void BPL_P2_ExpGagePut( BPLIST_WORK * wk, u16 cgx, u16 px, u16 py );
//static void BPL_P3_ExpGagePut( BPLIST_WORK * wk, u16 cgx, u16 px, u16 py );
//static void BPL_ContestWazaHeartPut( BPLIST_WORK * wk, u8 page );
static FIELD_SKILL_CHECK_RET BPL_HidenCheck( BPLIST_WORK * wk );
//static void BPL_HidenOff_Battle( BPLIST_WORK * wk );
//static void BPL_HidenOff_Contest( BPLIST_WORK * wk );
static u8 BPL_TamagoCheck( BPLIST_WORK * wk );

//static void BattleBag_SubItem( BATTLE_WORK * bw, u16 item, u16 page, u32 heap );

static BOOL FightPokeCheck( BPLIST_WORK * wk, u32 pos );
static BOOL ChangePokeCheck( BPLIST_WORK * wk, u32 pos );

static void CursorMoveSet( BPLIST_WORK * wk, u8 page );

static void InitListRow( BPLIST_WORK * wk );
static void ChangeListRow( BPLIST_WORK * wk, u32 pos1, u32 pos2 );
static BOOL CheckListRow( BPLIST_WORK * wk, u32 pos );
//static BOOL CheckRetNum( BPLIST_WORK * wk );
//static void SetRetNum( BPLIST_WORK * wk );
static void SetLog( BPLIST_WORK * wk, u8 pos1, u8 pos2 );
static int BPL_SeqPageChgDead( BPLIST_WORK * wk );
static int BPL_SeqPokeSelDead( BPLIST_WORK * wk );
static int BPL_SeqPokeChgDead( BPLIST_WORK * wk );
static int BPL_SeqDeadErrRet( BPLIST_WORK * wk );
static BOOL CheckDeadPoke( BPLIST_WORK * wk );
static void SetDeadChangeData( BPLIST_WORK * wk );
static BOOL CheckNextDeadSel( BPLIST_WORK * wk );
static void SetSelPosCancel( BPLIST_WORK * wk );
static BOOL CheckTimeOut( BPLIST_WORK * wk );
static void PlaySE( BPLIST_WORK * wk, int no );


//============================================================================================
//  �O���[�o���ϐ�
//============================================================================================

// ���C���V�[�P���X
static const pBPlistFunc MainSeqFunc[] = {
  BPL_SeqInit,
  BPL_SeqPokeSelect,
  BPL_SeqPokeIrekae,
  BPL_SeqStatusMain,
  BPL_SeqWazaSelect,
  BPL_SeqStInfoWaza,

  BPL_SeqPokeSelDead,
  BPL_SeqPokeChgDead,
  BPL_SeqDeadErrRet,

  BPL_SeqPage1Chg,
  BPL_SeqPageChgIrekae,
  BPL_SeqPageChgStatus,
  BPL_SeqPageChgStWazaSel,
  BPL_SeqPageChgWazaInfo,
  BPL_SeqPageChgWazaSetSel,
  BPL_SeqPageChgWazaSetEnter,
  BPL_SeqPageChgPPRcv,
  BPL_SeqPageChgDead,

  BPL_SeqPokeChange,
  BPL_SeqChgErrSet,
  BPL_SeqIrekaeErr,

  BPL_SeqMsgWait,
  BPL_SeqTrgWait,

  BPL_SeqWazaDelSelect,
  BPL_SeqWazaDelMain,
  BPL_SeqWazaDelError,

  BPL_SeqWazaRcvSelect,

  BPL_SeqButtonWait,

	BPL_SeqWazaInfoModeMain,

//  BPL_SeqStRcv,
//  BPL_SeqPPAllRcv,

  BPL_SeqEndSet,
  BPL_SeqEndWait,
};

// �v���[�g�\���ʒu
static const u8 PlatePos[][2] =
{
  { PLATE_POKE1_PX, PLATE_POKE1_PY },
  { PLATE_POKE2_PX, PLATE_POKE2_PY },
  { PLATE_POKE3_PX, PLATE_POKE3_PY },
  { PLATE_POKE4_PX, PLATE_POKE4_PY },
  { PLATE_POKE5_PX, PLATE_POKE5_PY },
  { PLATE_POKE6_PX, PLATE_POKE6_PY }
};



//--------------------------------------------------------------------------------------------
/**
 * �퓬�p�|�P�������X�g�^�X�N�ǉ�
 *
 * @param dat   ���X�g�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_TaskAdd( BPLIST_DATA * dat )
{
  BPLIST_WORK * wk;

  if( dat->sel_poke > 5 ){ dat->sel_poke = 0; }

	// �Z�������[�h�ȊO�̂Ƃ�
	if( dat->mode != BPL_MODE_WAZAINFO ){
		dat->sel_wp = 0;
	}

  wk = GFL_HEAP_AllocClearMemory( dat->heap, sizeof(BPLIST_WORK) );

  GFL_TCB_AddTask( dat->tcb_sys, BattlePokeList_Main, wk, 100 );

  wk->dat = dat;
  wk->pfd = dat->pfd;
  wk->seq = SEQ_BPL_INIT;
  wk->init_poke = dat->sel_poke;

//  wk->multi_pos = BattleWorkClientTypeGet( dat->bw, dat->client_no );
//  wk->multi_pos = 0;
// CLIENT_TYPE_A : �O�q
// CLIENT_TYPE_C : ��q

/*** �e�X�g ***/
//  wk->dat->mode = BPL_MODE_NORMAL;      // �ʏ�̃|�P�����I��
//  wk->dat->mode = BPL_MODE_ITEMUSE;     // �A�C�e���g�p
//  wk->dat->item = 38;


//  wk->dat->mode = BPL_MODE_WAZASET;     // �Z�Y��
//  wk->dat->sel_poke = 0;
//  wk->dat->chg_waza = 20;
//  wk->page = BPLIST_PAGE_PP_RCV;    // PP�񕜋Z�I���y�[�W


//  wk->dat->mode = BPL_MODE_CHG_DEAD;   // �m������ւ���
//  wk->dat->rule = BTL_RULE_SINGLE;
//  wk->dat->rule = BTL_RULE_DOUBLE;
//  wk->dat->rule = BTL_RULE_TRIPLE;

/*
  // �}���`
  wk->dat->multi_pp = wk->dat->pp;
  wk->dat->multiMode = TRUE;
  wk->dat->multiPos = 0;
*/

/**************/
}

//--------------------------------------------------------------------------------------------
/**
 * ���C���^�X�N
 *
 * @param tcb
 * @param work
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BattlePokeList_Main( GFL_TCB* tcb, void * work )
{
  BPLIST_WORK * wk = (BPLIST_WORK *)work;

  if( wk->seq != SEQ_BPL_END ){
    wk->seq = MainSeqFunc[wk->seq]( wk );
  }

  if( wk->seq == SEQ_BPL_END ){
    if( BPL_SeqEnd( tcb, wk ) == TRUE ){
      return;
    }
  }

  GFL_TCBL_Main( wk->tcbl );
  BPL_PokeIconAnime( wk );

//  GFL_CLACT_SYS_Main( wk->crp );
  BattlePokeList_ButtonAnmMain( wk );
  BPLISTBMP_PrintMain( wk );

#if 0
  switch( wk->seq ){
  case SEQ_BPL_INIT:    // ������
    wk->seq = BPL_SeqInit( wk );
    break;

  case SEQ_BPL_SELECT:  // �|�P�����I����ʃR���g���[��
    wk->seq = BPL_SeqPokeSelect( wk );
    break;

  case SEQ_BPL_IREKAE:
    wk->seq = BPL_SeqPokeIrekae( wk );
    break;

  case SEQ_BPL_ST_MAIN: // �X�e�[�^�X���C����ʃR���g���[��
    wk->seq = BPL_SeqStatusMain( wk );
    break;

  case SEQ_BPL_ST_WAZASEL:  // �X�e�[�^�X�Z�I����ʃR���g���[��
    wk->seq = BPL_SeqWazaSelect( wk );
    break;

  case SEQ_BPL_ST_SKILL:    // �Z��ʃR���g���[��
    wk->seq = BPL_SeqStInfoWaza( wk );
    break;

  case SEQ_BPL_PAGE1_CHG:
    wk->seq = BPL_SeqPage1Chg( wk );
    break;

  case SEQ_BPL_PAGECHG_IREKAE:  // ����ւ��y�[�W��
    wk->seq = BPL_SeqPageChgIrekae( wk );
    break;

  case SEQ_BPL_PAGECHG_STMAIN:  // �X�e�[�^�X�ڍׂ�
    wk->seq = BPL_SeqPageChgStatus( wk );
    break;

  case SEQ_BPL_PAGECHG_WAZASEL: // �Z�I����
    wk->seq = BPL_SeqPageChgStWazaSel( wk );
    break;

  case SEQ_BPL_PAGECHG_WAZAINFO:
    wk->seq = BPL_SeqPageChgWazaInfo( wk );
    break;

  case SEQ_BPL_PAGECHG_WAZASET_S: // �Z�Y��I����ʂ�
    wk->seq = BPL_SeqPageChgWazaSetSel( wk );
    break;

  case SEQ_BPL_PAGECHG_WAZASET_I:
    wk->seq = BPL_SeqPageChgWazaSetEnter( wk );
    break;

  case SEQ_BPL_PAGECHG_PPRCV: // �Z�񕜂�
    wk->seq = BPL_SeqPageChgPPRcv( wk );
    break;

  case SEQ_BPL_POKECHG:   // �|�P�����؂�ւ�
    wk->seq = BPL_SeqPokeChange( wk );
    break;

  case SEQ_BPL_CHG_ERR_SET: // ����ւ��G���[�Z�b�g
    wk->seq = BPL_SeqChgErrSet( wk );
    break;

  case SEQ_BPL_IREKAE_ERR:  // ����ւ��G���[�I���҂�
    wk->seq = BPL_SeqIrekaeErr( wk );
    break;

  case SEQ_BPL_MSG_WAIT:  // ���b�Z�[�W�E�F�C�g
    wk->seq = BPL_SeqMsgWait( wk );
    break;

  case SEQ_BPL_TRG_WAIT:  // ���b�Z�[�W�\����̃L�[�҂�
    wk->seq = BPL_SeqTrgWait( wk );
    break;

  case SEQ_BPL_WAZADEL_SEL: // �Z�Y��I��
    wk->seq = BPL_SeqWazaDelSelect( wk );
    break;

  case SEQ_BPL_WAZADEL_MAIN:  // �Z�Y�ꌈ��
    wk->seq = BPL_SeqWazaDelMain( wk );
    break;

  case SEQ_BPL_WAZARCV_SEL: // �Z�񕜑I��
    wk->seq = BPL_SeqWazaRcvSelect( wk );
    break;

  case SEQ_BPL_BUTTON_WAIT: // �{�^���A�j���I���҂�
    wk->seq = BPL_SeqButtonWait( wk );
    break;

  case SEQ_BPL_STRCV:     // �X�e�[�^�X��
    wk->seq = BPL_SeqStRcv( wk );
    break;

  case SEQ_BPL_PPALLRCV:    // PP�S��
    wk->seq = BPL_SeqPPAllRcv( wk );
    break;

  case SEQ_BPL_ENDSET:  // �I���t�F�[�h�Z�b�g
    wk->seq = BPL_SeqEndSet( wk );
    break;

  case SEQ_BPL_END:   // �I��
    if( BPL_SeqEnd( tcb, wk ) == TRUE ){
      return;
    }
  }

  BPL_PokeIconAnime( wk );

  GFL_CLACT_SYS_Main( wk->crp );
/*��[GS_CONVERT_TAG]*/
  BattlePokeList_ButtonAnmMain( wk );

#endif

}

//--------------------------------------------------------------------------------------------
/**
 * �������V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqInit( BPLIST_WORK * wk )
{
  u8  ret;

  G2S_BlendNone();

  wk->tcbl = GFL_TCBL_Init( wk->dat->heap, wk->dat->heap, 1, 4 );

//	wk->dat->mode = BPL_MODE_WAZAINFO;
//	wk->dat->sel_wp = 3;

	// �Z�Y��
  if( wk->dat->mode == BPL_MODE_WAZASET ){
    wk->page = BPLIST_PAGE_WAZASET_BS;
    ret = SEQ_BPL_WAZADEL_SEL;
	// �Z����
	}else if( wk->dat->mode == BPL_MODE_WAZAINFO ){
    wk->page = BPLIST_PAGE_SKILL;
    ret = SEQ_BPL_WAZAINFOMODE_MAIN;
	// ���̑�
  }else{
    wk->page = BPLIST_PAGE_SELECT;
    ret = SEQ_BPL_SELECT;
  }

//  wk->cmv_wk = BAPP_CursorMoveWorkAlloc( wk->dat->heap );

  wk->cpwk = BAPPTOOL_CreateCursor( wk->dat->heap );

  InitListRow( wk );

  BPL_PokeDataMake( wk );

//  BPL_VramInit();
  BPL_BgInit( wk );
  BPL_BgGraphicSet( wk );
  BPL_MsgManSet( wk );

  BPL_PageChgBgScreenChg( wk, wk->page );
//  BPL_PokePlateSet( wk );
  BattlePokeList_ButtonPageScreenInit( wk, wk->page );
  BattlePokeList_ButtonPalSet( wk, wk->page );

  BattlePokeList_ObjInit( wk );
  BattlePokeList_PageObjSet( wk, wk->page );

  BattlePokeList_BmpInit( wk );
  BattlePokeList_BmpWrite( wk, wk->page );
  BPLISTBMP_SetStrScrn( wk );

//  wk->dat->cursor_flg = 1;
/*
  if( wk->dat->cursor_flg != 0 ){
    BAPP_CursorMvWkSetFlag( wk->cmv_wk, 1 );
  }
*/
  if( *wk->dat->cursor_flg == 1 ){
    wk->cursor_flg = TRUE;
  }else{
    wk->cursor_flg = FALSE;
  }
  BAPPTOOL_VanishCursor( wk->cpwk, wk->cursor_flg );

  // �}���`�̂Ƃ��̏����ʒu�␳
  if( wk->page == BPLIST_PAGE_SELECT ){
    if( BattlePokeList_MultiPosCheck( wk, BPLISTMAIN_GetListRow(wk,0) ) == TRUE ){
      wk->dat->sel_poke = 1;
    }
    BPLISTUI_Init( wk, wk->page, wk->dat->sel_poke );
  }else{
		// �Z�I�����[�h�̂Ƃ�
		if( wk->dat->mode == BPL_MODE_WAZAINFO ){
	    BPLISTUI_Init( wk, wk->page, wk->dat->sel_wp );
		}else{
	    BPLISTUI_Init( wk, wk->page, 0 );
		}
  }

//  BattlePokeList_CursorMoveSet( wk, wk->page );

  BPL_ExpGagePut( wk, wk->page );

//  GFL_NET_ReloadIcon();
//  PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_OBJ, 14*16, FADE_PAL_ONE_SIZE );
//  {
/*
    u16 * defWk = PaletteWorkDefaultWorkGet( wk->pfd, FADE_SUB_OBJ );
    u16 * trsWk = PaletteWorkTransWorkGet( wk->pfd, FADE_SUB_OBJ );
//    u32 i;
    defWk[14*16+1] = 0x1f;
    defWk[14*16+2] = 0x3e;
    defWk[14*16+3] = 0x7c;
    defWk[14*16+15] = 0x7fff;
    trsWk[14*16+1] = 0x1f;
    trsWk[14*16+2] = 0x3e;
    trsWk[14*16+3] = 0x7c;
    trsWk[14*16+15] = 0x7fff;
*/
/*
    OS_Printf( "������ pal ������\n" );
    for( i=0; i<16; i++ ){
      OS_Printf( "0x%x, ", defWk[14*16+i] );
    }
    OS_Printf( "\n������ pal ������\n" );
    defWk = PaletteWorkTransWorkGet( wk->pfd, FADE_SUB_OBJ );
    for( i=0; i<16; i++ ){
      OS_Printf( "0x%x, ", defWk[14*16+i] );
    }
    OS_Printf( "\n������ pal ������\n" );
*/
//  }


  PaletteFadeReq(
    wk->pfd, PF_BIT_SUB_ALL, 0xffff, BATTLE_BAGLIST_FADE_SPEED, 16, 0, 0, wk->dat->tcb_sys );


  return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����I���y�[�W�̃R���g���[���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPokeSelect( BPLIST_WORK * wk )
{
  if( PaletteFadeCheck( wk->pfd ) != 0 ){ return SEQ_BPL_SELECT; }

  if( CheckTimeOut( wk ) == TRUE ){
    return SEQ_BPL_ENDSET;
  }

  if( BPL_PokemonSelect( wk ) == TRUE ){
    if( wk->dat->sel_poke == BPL_SEL_EXIT ){
      // �m�����ꂩ����
      if( wk->dat->mode == BPL_MODE_CHG_DEAD ){
        u8  pos1, pos2;
        if( BPLISTMAIN_GetNewLog( wk, &pos1, &pos2, TRUE ) == TRUE ){
          if( pos1 < pos2 ){
            GF_ASSERT_MSG( pos1 < BPL_SELNUM_MAX, "pos1 = %d\n", pos1 );
            wk->dat->sel_pos[pos1] = BPL_SELPOS_NONE;
          }else{
            GF_ASSERT_MSG( pos2 < BPL_SELNUM_MAX, "pos2 = %d\n", pos2 );
            wk->dat->sel_pos[pos2] = BPL_SELPOS_NONE;
          }
          wk->chg_pos1 = pos1;
          wk->chg_pos2 = pos2;
          wk->btn_seq = 0;
//          PlaySE( wk, wk->cancelSE );
          PlaySE( wk, SEQ_SE_CANCEL2 );
          return SEQ_BPL_POKECHG_DEAD;
        }
      // �L�����Z�������ȊO
      }else if( wk->dat->mode != BPL_MODE_NO_CANCEL ){
//        PlaySE( wk, wk->cancelSE );
        PlaySE( wk, SEQ_SE_CANCEL2 );
        BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
        SetSelPosCancel( wk );
        return SEQ_BPL_ENDSET;
      }
/*
      if( wk->dat->mode != BPL_MODE_CHG_DEAD ){
        PlaySE( wk, SEQ_SE_DECIDE2 );
        BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
        SetSelPosCancel( wk );
        return SEQ_BPL_ENDSET;
      }else{
        u8  pos1, pos2;
        if( BPLISTMAIN_GetNewLog( wk, &pos1, &pos2, TRUE ) == TRUE ){
          if( pos1 < pos2 ){
            GF_ASSERT_MSG( pos1 < BPL_SELNUM_MAX, "pos1 = %d\n", pos1 );
            wk->dat->sel_pos[pos1] = BPL_SELPOS_NONE;
          }else{
            GF_ASSERT_MSG( pos2 < BPL_SELNUM_MAX, "pos2 = %d\n", pos2 );
            wk->dat->sel_pos[pos2] = BPL_SELPOS_NONE;
          }
          wk->chg_pos1 = pos1;
          wk->chg_pos2 = pos2;
          wk->btn_seq = 0;
          return SEQ_BPL_POKECHG_DEAD;
        }
      }
*/
    }else{
      PlaySE( wk, SEQ_SE_DECIDE2 );
      BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_POKE1+wk->dat->sel_poke );
      if( wk->dat->mode == BPL_MODE_ITEMUSE ){
        return BPL_PokeItemUse( wk );
      }else{
        wk->ret_seq = SEQ_BPL_PAGECHG_IREKAE;
      }
      return SEQ_BPL_BUTTON_WAIT;
    }
  }

  return SEQ_BPL_SELECT;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_PokeItemUse( BPLIST_WORK * wk )
{
  BPLIST_DATA * dat = wk->dat;

  // �^�}�S�ɂ͎g���Ȃ�
  if( wk->poke[ BPLISTMAIN_GetListRow(wk,dat->sel_poke) ].egg != 0 ){
    GFL_MSG_GetString( wk->mman, mes_b_plist_m06, wk->msg_buf );
    BattlePokeList_TalkMsgSet( wk );
    wk->dat->sel_poke = BPL_SEL_EXIT;
    wk->ret_seq = SEQ_BPL_ENDSET;
    SetSelPosCancel( wk );
    return SEQ_BPL_MSG_WAIT;
  }

  // �����������Ŏg�p�s��
  if( dat->skill_item_use[ BPLISTMAIN_GetListRow(wk,dat->sel_poke) ] != 0 ){
    BattlePokeList_ItemUseSkillErrMsgSet( wk );
    BattlePokeList_TalkMsgSet( wk );
    wk->dat->sel_poke = BPL_SEL_EXIT;
    wk->ret_seq = SEQ_BPL_ENDSET;
    SetSelPosCancel( wk );
    return SEQ_BPL_MSG_WAIT;
  }

  // �P�̋Z��PP��
  if( ITEM_GetParam( dat->item, ITEM_PRM_PP_RCV, dat->heap ) != 0 &&
      ITEM_GetParam( dat->item, ITEM_PRM_ALL_PP_RCV, dat->heap ) == 0 ){
    wk->ret_seq = SEQ_BPL_PAGECHG_PPRCV;
    return SEQ_BPL_BUTTON_WAIT;
  }

  dat->sel_pos[0] = BPLISTMAIN_GetListRow( wk, dat->sel_poke );

  return SEQ_BPL_ENDSET;

#if 0
  BPLIST_DATA * dat = wk->dat;

  // �����������Ŏg�p�s��
  if( ( dat->sel_poke == 0 && dat->skill_item_use[0] != 0 ) ||
    ( dat->sel_poke == 1 && dat->skill_item_use[1] != 0 ) ){
    BattlePokeList_ItemUseSkillErrMsgSet( wk );
    BattlePokeList_TalkMsgSet( wk );
    wk->dat->sel_poke = BPL_SEL_EXIT;
    wk->ret_seq = SEQ_BPL_ENDSET;
    return SEQ_BPL_MSG_WAIT;
  }

  // �P�̋Z��PP��
  if( ITEM_GetParam( dat->item, ITEM_PRM_PP_RCV, dat->heap ) != 0 &&
      ITEM_GetParam( dat->item, ITEM_PRM_ALL_PP_RCV, dat->heap ) == 0 &&
      wk->poke[ BPLISTMAIN_GetListRow(wk,dat->sel_poke) ].egg == 0 ){

    wk->ret_seq = SEQ_BPL_PAGECHG_PPRCV;
    return SEQ_BPL_BUTTON_WAIT;
  }

  if( BattleWorkStatusRecover(dat->bw,dat->client_no,dat->list_row[dat->sel_poke],0,dat->item) == TRUE ){
    // �S�ZPP��
    if( ITEM_GetParam( dat->item, ITEM_PRM_ALL_PP_RCV, dat->heap ) != 0 ){
      wk->ret_seq = SEQ_BPL_PAGECHG_PPRCV;
    }else{
      // �퓬�ɏo�Ă���|�P�����ŁA�m���񕜈ȊO
      if( BattlePokeList_PokeSetCheck( wk, dat->sel_poke ) == 1 &&
        ITEM_GetParam( dat->item, ITEM_PRM_DEATH_RCV, dat->heap ) == 0 ){
//        BattleBag_SubItem( dat->bw, dat->item, dat->bag_page, dat->heap );
        wk->poke[ BPLISTMAIN_GetListRow(wk,dat->sel_poke) ].pp =
          BattleWorkPokemonParamGet( dat->bw, dat->client_no, dat->list_row[dat->sel_poke] );
        dat->hp_rcv = PP_Get( wk->poke[ BPLISTMAIN_GetListRow(wk,dat->sel_poke) ].pp, ID_PARA_hp, NULL );
        dat->hp_rcv -= wk->poke[ BPLISTMAIN_GetListRow(wk,dat->sel_poke) ].hp;
        wk->ret_seq = SEQ_BPL_ENDSET;
      }else{
        wk->ret_seq = SEQ_BPL_STRCV;
      }
    }
/*
    if( ITEM_GetParam( dat->item, ITEM_PRM_ALL_PP_RCV, dat->heap ) == 0 ){
      if( BattlePokeList_PokeSetCheck( wk, dat->sel_poke ) == 1 ){
        BattleBag_SubItem( dat->bw, dat->item, dat->bag_page, dat->heap );
        wk->poke[dat->sel_poke].pp =
          BattleWorkPokemonParamGet( dat->bw, dat->client_no, dat->list_row[dat->sel_poke] );
        dat->hp_rcv = PP_Get( wk->poke[dat->sel_poke].pp, ID_PARA_hp, NULL );
        dat->hp_rcv -= wk->poke[dat->sel_poke].hp;
        wk->ret_seq = SEQ_BPL_ENDSET;
      }else{
        wk->ret_seq = SEQ_BPL_STRCV;
      }
    }else{
      wk->ret_seq = SEQ_BPL_PAGECHG_PPRCV;
    }
*/
    wk->rcv_seq = 0;
    return SEQ_BPL_BUTTON_WAIT;
  }else{
    GFL_MSG_GetString( wk->mman, mes_b_plist_m06, wk->msg_buf );
    BattlePokeList_TalkMsgSet( wk );
    wk->dat->sel_poke = BPL_SEL_EXIT;
    wk->ret_seq = SEQ_BPL_ENDSET;
    return SEQ_BPL_MSG_WAIT;
  }
  return SEQ_BPL_MSG_WAIT;
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * ����ւ��y�[�W�̃R���g���[���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPokeIrekae( BPLIST_WORK * wk )
{
  u32 ret;

  if( CheckTimeOut( wk ) == TRUE ){
    return SEQ_BPL_ENDSET;
  }

/*
  ret = BPL_IrekaeSelect( wk );

  switch( ret ){
  case 0:   // ����ւ���
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_POKE_CHG );
    if( BPL_IrekaeCheck( wk ) == TRUE ){
      // �ʏ�̓���ւ�
      if( wk->dat->mode != BPL_MODE_CHG_DEAD ){
        wk->dat->sel_pos[0] = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
        return SEQ_BPL_ENDSET;
      }
      // �m�����̓���ւ�
      if( CheckDeadPoke( wk ) == FALSE ){
        // ���߂�Ȃ�A�����Ń��[�N���Z�b�g���ďI��
        SetDeadChangeData( wk );
        return SEQ_BPL_ENDSET;
      }else{
        wk->ret_seq = SEQ_BPL_PAGECHG_DEAD;
        return SEQ_BPL_BUTTON_WAIT;
      }
    }
    wk->ret_seq = SEQ_BPL_CHG_ERR_SET;
    return SEQ_BPL_BUTTON_WAIT;

  case 1:   // ����������
    if( BPL_TamagoCheck( wk ) == TRUE ){ break; }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_STATUS1 );
    wk->ret_seq = SEQ_BPL_PAGECHG_STMAIN;
    return SEQ_BPL_BUTTON_WAIT;

  case 2:   // �Z������
    if( BPL_TamagoCheck( wk ) == TRUE ){ break; }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZASEL1 );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASEL;
    return SEQ_BPL_BUTTON_WAIT;

  case 3:   // �߂�
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGE1_CHG;
    return SEQ_BPL_BUTTON_WAIT;
  }
*/
  ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_CHG_ENTER:       // ���ꂩ����
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_POKE_CHG );
    if( BPL_IrekaeCheck( wk ) == TRUE ){
      // �ʏ�̓���ւ�
      if( wk->dat->mode != BPL_MODE_CHG_DEAD ){
        wk->dat->sel_pos[wk->dat->sel_pos_index] = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
				OS_Printf( "wk->dat->sel_pos[%d] = %d\n", wk->dat->sel_pos_index, wk->dat->sel_pos[wk->dat->sel_pos_index] );
        return SEQ_BPL_ENDSET;
      }
      // �m�����̓���ւ�
      if( CheckDeadPoke( wk ) == FALSE ){
        // ���߂�Ȃ�A�����Ń��[�N���Z�b�g���ďI��
        SetDeadChangeData( wk );
        return SEQ_BPL_ENDSET;
      }else{
        wk->ret_seq = SEQ_BPL_PAGECHG_DEAD;
        return SEQ_BPL_BUTTON_WAIT;
      }
    }
    wk->ret_seq = SEQ_BPL_CHG_ERR_SET;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_CHG_STATUS:      // �悳���݂�
    if( BPL_TamagoCheck( wk ) == TRUE ){ break; }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_STATUS1 );
    wk->ret_seq = SEQ_BPL_PAGECHG_STMAIN;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_CHG_WAZA:        // �킴���݂�
    if( BPL_TamagoCheck( wk ) == TRUE ){ break; }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZASEL1 );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASEL;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_CHG_RETURN:      // ���ǂ�
/*
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
      PlaySE( wk, SEQ_SE_CANCEL2 );
    }else{
      PlaySE( wk, SEQ_SE_DECIDE2 );
    }
*/
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGE1_CHG;
    return SEQ_BPL_BUTTON_WAIT;

  case CURSORMOVE_CANCEL:         // �L�����Z��
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGE1_CHG;
    return SEQ_BPL_BUTTON_WAIT;

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

  return SEQ_BPL_IREKAE;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X���C���y�[�W�̃R���g���[���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqStatusMain( BPLIST_WORK * wk )
{
  u32 ret;

  if( CheckTimeOut( wk ) == TRUE ){
    return SEQ_BPL_ENDSET;
  }

/*
  ret = BPL_StatusMainSelect( wk );

  switch( ret ){
  case 0:   // �O�̃|�P������
    {
      u8 pos = BPL_NextPokeGet( wk, wk->dat->sel_poke, -1 );
      if( pos == 0xff ){ break; }
      wk->dat->sel_poke = pos;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_UP );
    wk->ret_seq = SEQ_BPL_POKECHG;
    return SEQ_BPL_BUTTON_WAIT;

  case 1:   // ���̃|�P������
    {
      u8 pos = BPL_NextPokeGet( wk, wk->dat->sel_poke, 1 );
      if( pos == 0xff ){ break; }
      wk->dat->sel_poke = pos;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_DOWN );
    wk->ret_seq = SEQ_BPL_POKECHG;
    return SEQ_BPL_BUTTON_WAIT;

  case 2:   // �Z������
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZASEL2 );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASEL;
    return SEQ_BPL_BUTTON_WAIT;

  case 3:   // �߂�
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->chg_page_cp = 1;    // ����ւ��y�[�W�̃J�[�\���ʒu���u����������v��
    wk->ret_seq = SEQ_BPL_PAGECHG_IREKAE;
    return SEQ_BPL_BUTTON_WAIT;
  }
*/
  ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_STATUS_UP:       // �O�̃|�P������
    {
      u8 pos = BPL_NextPokeGet( wk, wk->dat->sel_poke, -1 );
      if( pos == 0xff ){ break; }
      wk->dat->sel_poke = pos;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_UP );
    wk->ret_seq = SEQ_BPL_POKECHG;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_STATUS_DOWN:     // ���̃|�P������
    {
      u8 pos = BPL_NextPokeGet( wk, wk->dat->sel_poke, 1 );
      if( pos == 0xff ){ break; }
      wk->dat->sel_poke = pos;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_DOWN );
    wk->ret_seq = SEQ_BPL_POKECHG;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_STATUS_WAZA:     // �킴���݂�
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZASEL2 );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASEL;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_STATUS_RETURN:   // ���ǂ�
/*
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
      PlaySE( wk, SEQ_SE_CANCEL2 );
    }else{
      PlaySE( wk, SEQ_SE_DECIDE2 );
    }
*/
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->chg_page_cp = 1;    // ����ւ��y�[�W�̃J�[�\���ʒu���u����������v��
    wk->ret_seq = SEQ_BPL_PAGECHG_IREKAE;
    return SEQ_BPL_BUTTON_WAIT;

  case CURSORMOVE_CANCEL:         // �L�����Z��
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->chg_page_cp = 1;    // ����ւ��y�[�W�̃J�[�\���ʒu���u����������v��
    wk->ret_seq = SEQ_BPL_PAGECHG_IREKAE;
    return SEQ_BPL_BUTTON_WAIT;

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

  return SEQ_BPL_ST_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�Z�I���y�[�W�̃R���g���[���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqWazaSelect( BPLIST_WORK * wk )
{
  u32 ret;

  if( CheckTimeOut( wk ) == TRUE ){
    return SEQ_BPL_ENDSET;
  }

/*
  ret = BPL_StWazaSelect( wk );

  switch( ret ){
  case 0:   // �Z�P
  case 1:   // �Z�Q
  case 2:   // �Z�R
  case 3:   // �Z�S
    if( wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[ret].id == 0 ){
      break;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZA1+ret );
    wk->dat->sel_wp = ret;
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZAINFO;
    return SEQ_BPL_BUTTON_WAIT;

  case 4:   // �O�̃|�P������
    {
      u8 pos = BPL_NextPokeGet( wk, wk->dat->sel_poke, -1 );
      if( pos == 0xff ){ break; }
      wk->dat->sel_poke = pos;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_UP );
    wk->ret_seq = SEQ_BPL_POKECHG;
    return SEQ_BPL_BUTTON_WAIT;

  case 5:   // ���̃|�P������
    {
      u8 pos = BPL_NextPokeGet( wk, wk->dat->sel_poke, 1 );
      if( pos == 0xff ){ break; }
      wk->dat->sel_poke = pos;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_DOWN );
    wk->ret_seq = SEQ_BPL_POKECHG;
    return SEQ_BPL_BUTTON_WAIT;

  case 6:   // ����������
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_STATUS2 );
    wk->ret_seq = SEQ_BPL_PAGECHG_STMAIN;
    return SEQ_BPL_BUTTON_WAIT;

  case 7:   // �߂�
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->chg_page_cp = 2;    // ����ւ��y�[�W�̃J�[�\���ʒu���u�Z������v��
    wk->ret_seq = SEQ_BPL_PAGECHG_IREKAE;
    return SEQ_BPL_BUTTON_WAIT;
  }
*/
  ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_WAZA_SEL1:     // �Z�P
  case BPLIST_UI_WAZA_SEL2:     // �Z�Q
  case BPLIST_UI_WAZA_SEL3:     // �Z�R
  case BPLIST_UI_WAZA_SEL4:     // �Z�S
    if( wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[ret].id == 0 ){
      break;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZA1+ret );
    wk->dat->sel_wp = ret;
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZAINFO;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_WAZA_UP:       // �O�̃|�P������
    {
      u8 pos = BPL_NextPokeGet( wk, wk->dat->sel_poke, -1 );
      if( pos == 0xff ){ break; }
      wk->dat->sel_poke = pos;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_UP );
    wk->ret_seq = SEQ_BPL_POKECHG;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_WAZA_DOWN:     // ���̃|�P������
    {
      u8 pos = BPL_NextPokeGet( wk, wk->dat->sel_poke, 1 );
      if( pos == 0xff ){ break; }
      wk->dat->sel_poke = pos;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_DOWN );
    wk->ret_seq = SEQ_BPL_POKECHG;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_WAZA_STATUS:   // �悳���݂�
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_STATUS2 );
    wk->ret_seq = SEQ_BPL_PAGECHG_STMAIN;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_WAZA_RETURN:   // ���ǂ�
/*
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
      PlaySE( wk, SEQ_SE_CANCEL2 );
    }else{
      PlaySE( wk, SEQ_SE_DECIDE2 );
    }
*/
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->chg_page_cp = 2;    // ����ւ��y�[�W�̃J�[�\���ʒu���u�Z������v��
    wk->ret_seq = SEQ_BPL_PAGECHG_IREKAE;
    return SEQ_BPL_BUTTON_WAIT;

  case CURSORMOVE_CANCEL:       // �L�����Z��
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->chg_page_cp = 2;    // ����ւ��y�[�W�̃J�[�\���ʒu���u�Z������v��
    wk->ret_seq = SEQ_BPL_PAGECHG_IREKAE;
    return SEQ_BPL_BUTTON_WAIT;

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

  return SEQ_BPL_ST_WAZASEL;
}


//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�Z�ڍ׃y�[�W�̃R���g���[���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqStInfoWaza( BPLIST_WORK * wk )
{
  u32 ret;

  if( CheckTimeOut( wk ) == TRUE ){
    return SEQ_BPL_ENDSET;
  }

/*
  ret = BPL_StInfoWazaSelect( wk );

  switch( ret ){
  case 0:   // �Z�P
  case 1:   // �Z�Q
  case 2:   // �Z�R
  case 3:   // �Z�S
    if( wk->dat->sel_wp != ret &&
      wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[ret].id == 0 ){
      break;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    wk->dat->sel_wp = ret;
    return SEQ_BPL_PAGECHG_WAZAINFO;

  case 4:   // �߂�
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASEL;
    return SEQ_BPL_BUTTON_WAIT;
  }
*/
  ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_WAZAINFO_SEL1:     // �Z�P
  case BPLIST_UI_WAZAINFO_SEL2:     // �Z�Q
  case BPLIST_UI_WAZAINFO_SEL3:     // �Z�R
  case BPLIST_UI_WAZAINFO_SEL4:     // �Z�S
    if( wk->dat->sel_wp == ret ||
      wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[ret].id == 0 ){
      break;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    wk->dat->sel_wp = ret;
    return SEQ_BPL_PAGECHG_WAZAINFO;

  case BPLIST_UI_WAZAINFO_RETURN:   // ���ǂ�
/*
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
      PlaySE( wk, SEQ_SE_CANCEL2 );
    }else{
      PlaySE( wk, SEQ_SE_DECIDE2 );
    }
*/
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASEL;
    return SEQ_BPL_BUTTON_WAIT;

  case CURSORMOVE_CANCEL:         // �L�����Z��
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASEL;
    return SEQ_BPL_BUTTON_WAIT;

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

  return SEQ_BPL_ST_SKILL;
}

// �Z�������[�h���C������
static int BPL_SeqWazaInfoModeMain( BPLIST_WORK * wk )
{
  u32 ret;

  if( CheckTimeOut( wk ) == TRUE ){
    return SEQ_BPL_ENDSET;
  }

  ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_WAZAINFO_SEL1:     // �Z�P
  case BPLIST_UI_WAZAINFO_SEL2:     // �Z�Q
  case BPLIST_UI_WAZAINFO_SEL3:     // �Z�R
  case BPLIST_UI_WAZAINFO_SEL4:     // �Z�S
    if( wk->dat->sel_wp == ret ||
      wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[ret].id == 0 ){
      break;
    }
    PlaySE( wk, SEQ_SE_DECIDE2 );
    wk->dat->sel_wp = ret;
    return SEQ_BPL_PAGECHG_WAZAINFO;

  case BPLIST_UI_WAZAINFO_RETURN:   // ���ǂ�
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASEL;
    return SEQ_BPL_ENDSET;

  case CURSORMOVE_CANCEL:         // �L�����Z��
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASEL;
    return SEQ_BPL_ENDSET;

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

  return SEQ_BPL_WAZAINFOMODE_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�T�̃R���g���[���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqWazaDelSelect( BPLIST_WORK * wk )
{
/*
  int ret = BPL_TPCheck( wk, Page5_HitRect );

  if( ret == GFL_UI_TP_HIT_NONE ){
    ret = BAPP_CursorMove( wk->cmv_wk );
    if( ret == BAPP_CMV_CANCEL ){
      ret = 6;
    }
  }else{
    BattlePokeList_CursorOff( wk );
  }

  switch( ret ){
  case 0:   // �Z
  case 1:
  case 2:
  case 3:
  case 4:
    wk->dat->sel_wp = (u8)ret;
    wk->wws_page_cp = (u8)ret;
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZADEL1+ret );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASET_I;
    return SEQ_BPL_BUTTON_WAIT;

  case 5:   // �퓬<->�R���e�X�g�؂�ւ�
    wk->wws_page_cp = (u8)ret;
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_CONTEST );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASET_S;
    return SEQ_BPL_BUTTON_WAIT;

  case 6:   // �L�����Z��
    wk->dat->sel_wp = BPL_SEL_WP_CANCEL;
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_ENDSET;
    return SEQ_BPL_BUTTON_WAIT;
  }
  return SEQ_BPL_WAZADEL_SEL;
*/
  u32 ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_WAZADEL_SEL1:    // �Z�P
  case BPLIST_UI_WAZADEL_SEL2:    // �Z�Q
  case BPLIST_UI_WAZADEL_SEL3:    // �Z�R
  case BPLIST_UI_WAZADEL_SEL4:    // �Z�S
  case BPLIST_UI_WAZADEL_SEL5:    // �Z�T
    wk->dat->sel_wp = (u8)ret;
    wk->wws_page_cp = (u8)ret;
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZADEL1+ret );
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASET_I;
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_WAZADEL_RETURN:  // ���ǂ�
/*
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
      PlaySE( wk, SEQ_SE_CANCEL2 );
    }else{
      PlaySE( wk, SEQ_SE_DECIDE2 );
    }
*/
    PlaySE( wk, SEQ_SE_CANCEL2 );
    wk->dat->sel_wp = BPL_SEL_WP_CANCEL;
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_ENDSET;
    return SEQ_BPL_BUTTON_WAIT;

  case CURSORMOVE_CANCEL:         // �L�����Z��
    PlaySE( wk, SEQ_SE_CANCEL2 );
    wk->dat->sel_wp = BPL_SEL_WP_CANCEL;
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_ENDSET;
    return SEQ_BPL_BUTTON_WAIT;

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

  return SEQ_BPL_WAZADEL_SEL;
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�U�̃R���g���[���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqWazaDelMain( BPLIST_WORK * wk )
{
/*
  int ret = BPL_TPCheck( wk, Page6_HitRect );

  if( ret == GFL_UI_TP_HIT_NONE ){
    ret = BAPP_CursorMove( wk->cmv_wk );
    if( ret == BAPP_CMV_CANCEL ){
      ret = 2;
    }
  }else{
    BattlePokeList_CursorOff( wk );
  }

  switch( ret ){
  case 0:   // �킷���
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZADEL_B );
#if 0
    if( BPL_HidenCheck( wk ) == TRUE ){
      BPL_HidenMsgPut( wk );
      BPL_HidenOff_Battle( wk );
      wk->ret_seq = SEQ_BPL_WAZADEL_MAIN;
    }else{

      wk->ret_seq = SEQ_BPL_ENDSET;
    }
#else
    wk->ret_seq = SEQ_BPL_ENDSET;
#endif
    return SEQ_BPL_BUTTON_WAIT;

  case 1:   // �퓬<->�R���e�X�g�؂�ւ�
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_CONTEST );
    wk->wwm_page_cp = (u8)ret;
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASET_I;
    return SEQ_BPL_BUTTON_WAIT;

  case 2:   // �L�����Z��
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->wwm_page_cp = 0;
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASET_S;
    return SEQ_BPL_BUTTON_WAIT;
  }
  return SEQ_BPL_WAZADEL_MAIN;
*/
  u32 ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_DELINFO_ENTER:   // �킷���
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZADEL_B );
    // �Y�����
    if( BPL_HidenCheck( wk ) == FSCR_OK ){
      wk->ret_seq = SEQ_BPL_ENDSET;
    // �Y����Ȃ�
    }else{
      wk->ret_seq = SEQ_BPL_WAZADEL_ERROR;
    }
    return SEQ_BPL_BUTTON_WAIT;

  case BPLIST_UI_DELINFO_RETURN:  // ���ǂ�
/*
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
      PlaySE( wk, SEQ_SE_CANCEL2 );
    }else{
      PlaySE( wk, SEQ_SE_DECIDE2 );
    }
*/
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->wwm_page_cp = 0;
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASET_S;
    return SEQ_BPL_BUTTON_WAIT;

  case CURSORMOVE_CANCEL:         // �L�����Z��
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->wwm_page_cp = 0;
    wk->ret_seq = SEQ_BPL_PAGECHG_WAZASET_S;
    return SEQ_BPL_BUTTON_WAIT;

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

  return SEQ_BPL_WAZADEL_MAIN;
}

static int BPL_SeqWazaDelError( BPLIST_WORK * wk )
{
  // �Z�}�V���������Ă��Ȃ�
  if( BPL_HidenCheck( wk ) == FSCR_NO_MACHINE ){
    BPL_HidenMsgPut( wk, 0 );
  // �g�p��
  }else{
    BPL_HidenMsgPut( wk, 1 );
  }
  wk->talk_win_clear = 1;
  wk->ret_seq = SEQ_BPL_WAZADEL_MAIN;
  return SEQ_BPL_MSG_WAIT;
}


//--------------------------------------------------------------------------------------------
/**
 * �Z�񕜑I���y�[�W�̃R���g���[���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqWazaRcvSelect( BPLIST_WORK * wk )
{
/*
  BPLIST_DATA * dat;
  int ret;

  dat = wk->dat;
  ret = BPL_TPCheck( wk, PPRcvPage_HitRect );
  if( ret == GFL_UI_TP_HIT_NONE ){
    ret = BAPP_CursorMove( wk->cmv_wk );
    if( ret == BAPP_CMV_CANCEL ){
      ret = 4;
    }
  }else{
    BattlePokeList_CursorOff( wk );
  }

  switch( ret ){
  case 0:   // �Z
  case 1:
  case 2:
  case 3:
#if 0
    if( wk->poke[dat->sel_poke].waza[ret].id == 0 ){ break; }
    wk->dat->sel_wp = (u8)ret;
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZARCV1+ret );
//    if( BattleWorkStatusRecover(dat->bw,dat->client_no,dat->list_row[dat->sel_poke],ret,dat->item) == TRUE ){
    if( 0 ){
      wk->rcv_seq = 0;
      wk->ret_seq = SEQ_BPL_STRCV;
      return SEQ_BPL_BUTTON_WAIT;
    }else{
      GFL_MSG_GetString( wk->mman, mes_b_plist_m06, wk->msg_buf );
      BattlePokeList_TalkMsgSet( wk );
      wk->dat->sel_poke = BPL_SEL_EXIT;
      wk->ret_seq = SEQ_BPL_ENDSET;
      return SEQ_BPL_MSG_WAIT;
    }
    break;
#endif
    if( wk->poke[dat->sel_poke].waza[ret].id == 0 ){ break; }
    wk->dat->sel_wp = (u8)ret;
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZARCV1+ret );
    return SEQ_BPL_ENDSET;

  case 4:   // �L�����Z��
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGE1_CHG;
    return SEQ_BPL_BUTTON_WAIT;
  }
  return SEQ_BPL_WAZARCV_SEL;
*/
  BPLIST_DATA * dat;
  u32 ret;

  dat = wk->dat;
  ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_PPRCV_WAZA1:     // �Z�P
  case BPLIST_UI_PPRCV_WAZA2:     // �Z�Q
  case BPLIST_UI_PPRCV_WAZA3:     // �Z�R
  case BPLIST_UI_PPRCV_WAZA4:     // �Z�S
/*
    if( wk->poke[dat->sel_poke].waza[ret].id == 0 ){ break; }
    wk->dat->sel_wp = (u8)ret;
    PlaySE( wk, SEQ_SE_DECIDE2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZARCV1+ret );
//    if( BattleWorkStatusRecover(dat->bw,dat->client_no,dat->list_row[dat->sel_poke],ret,dat->item) == TRUE ){
    if( 0 ){
      wk->rcv_seq = 0;
      wk->ret_seq = SEQ_BPL_STRCV;
      return SEQ_BPL_BUTTON_WAIT;
    }else{
      GFL_MSG_GetString( wk->mman, mes_b_plist_m06, wk->msg_buf );
      BattlePokeList_TalkMsgSet( wk );
      wk->dat->sel_poke = BPL_SEL_EXIT;
      wk->ret_seq = SEQ_BPL_ENDSET;
      return SEQ_BPL_MSG_WAIT;
    }
    break;
*/
    {
      u8  pos = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
      if( wk->poke[pos].waza[ret].id == 0 ){ break; }
      wk->dat->sel_wp = (u8)ret;
      wk->dat->sel_pos[0] = pos;
      PlaySE( wk, SEQ_SE_DECIDE2 );
      BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_WAZARCV1+ret );
      return SEQ_BPL_ENDSET;
    }

  case BPLIST_UI_PPRCV_RETURN:    // ���ǂ�
/*
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
      PlaySE( wk, SEQ_SE_CANCEL2 );
    }else{
      PlaySE( wk, SEQ_SE_DECIDE2 );
    }
*/
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGE1_CHG;
    return SEQ_BPL_BUTTON_WAIT;

  case CURSORMOVE_CANCEL:         // �L�����Z��
    PlaySE( wk, SEQ_SE_CANCEL2 );
    BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
    wk->ret_seq = SEQ_BPL_PAGE1_CHG;
    return SEQ_BPL_BUTTON_WAIT;

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

  return SEQ_BPL_WAZARCV_SEL;
}


//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ��F�|�P�����I���y�[�W��
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPage1Chg( BPLIST_WORK * wk )
{
  if( BPL_PageChange( wk, BPLIST_PAGE_SELECT ) == FALSE ){
    return SEQ_BPL_PAGE1_CHG;
  }
  return SEQ_BPL_SELECT;
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ��F�|�P��������ւ��y�[�W��
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPageChgIrekae( BPLIST_WORK * wk )
{
  if( BPL_PageChange( wk, BPLIST_PAGE_POKE_CHG ) == FALSE ){
    return SEQ_BPL_PAGECHG_IREKAE;
  }
  return SEQ_BPL_IREKAE;
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ��F�X�e�[�^�X���C���y�[�W��
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPageChgStatus( BPLIST_WORK * wk )
{
  if( BPL_PageChange( wk, BPLIST_PAGE_MAIN ) == FALSE ){
    return SEQ_BPL_PAGECHG_STMAIN;
  }
  return SEQ_BPL_ST_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ��F�X�e�[�^�X�Z�I���y�[�W��
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPageChgStWazaSel( BPLIST_WORK * wk )
{
  if( BPL_PageChange( wk, BPLIST_PAGE_WAZA_SEL ) == FALSE ){
    return SEQ_BPL_PAGECHG_WAZASEL;
  }
  return SEQ_BPL_ST_WAZASEL;
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ��F�Z�ڍ׃y�[�W��
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPageChgWazaInfo( BPLIST_WORK * wk )
{
  if( BPL_PageChange( wk, BPLIST_PAGE_SKILL ) == FALSE ){
    return SEQ_BPL_PAGECHG_WAZAINFO;
  }
	// �Z�������[�h�̏ꍇ
	if( wk->dat->mode == BPL_MODE_WAZAINFO ){
	  return SEQ_BPL_WAZAINFOMODE_MAIN;
	}
  return SEQ_BPL_ST_SKILL;
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ��F�Z�Y��y�[�W��
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPageChgWazaSetSel( BPLIST_WORK * wk )
{
  if( BPL_PageChange( wk, BPLIST_PAGE_WAZASET_BS ) == FALSE ){
    return SEQ_BPL_PAGECHG_WAZASET_S;
  }
  return SEQ_BPL_WAZADEL_SEL;
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ��F�Z�Y��ڍ׃y�[�W��
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPageChgWazaSetEnter( BPLIST_WORK * wk )
{
/*
  if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
    return SEQ_BPL_PAGECHG_WAZASET_I;
  }
  BattlePokelist_WazaTypeSet( wk );
  BPL_PageChange( wk, BPLIST_PAGE_WAZASET_BI );
*/
  if( BPL_PageChange( wk, BPLIST_PAGE_WAZASET_BI ) == FALSE ){
    return SEQ_BPL_PAGECHG_WAZASET_I;
  }
//  BattlePokelist_WazaTypeSet( wk );
  return SEQ_BPL_WAZADEL_MAIN;
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ��F�Z�񕜃y�[�W��
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPageChgPPRcv( BPLIST_WORK * wk )
{
  if( BPL_PageChange( wk, BPLIST_PAGE_PP_RCV ) == FALSE ){
    return SEQ_BPL_PAGECHG_PPRCV;
  }
/*
  if( ITEM_GetParam( wk->dat->item, ITEM_PRM_ALL_PP_RCV, wk->dat->heap ) != 0 ){
    return SEQ_BPL_PPALLRCV;
  }
*/
  return SEQ_BPL_WAZARCV_SEL;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������肩���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqPokeChange( BPLIST_WORK * wk )
{
  BattlePokeList_PageObjSet( wk, wk->page );
  BattlePokeList_BmpWrite( wk, wk->page );
  BattlePokeList_ButtonPageScreenInit( wk, wk->page );
  BPL_ExpGagePut( wk, wk->page );
  if( wk->page == BPLIST_PAGE_MAIN ){
    return SEQ_BPL_ST_MAIN;
  }
  return SEQ_BPL_ST_WAZASEL;
}

//--------------------------------------------------------------------------------------------
/**
 * ����ւ��G���[�Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqChgErrSet( BPLIST_WORK * wk )
{
  BattlePokeList_TalkMsgSet( wk );
  wk->ret_seq = SEQ_BPL_IREKAE_ERR;
  return SEQ_BPL_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ����ւ��G���[���b�Z�[�W�I���҂��V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqIrekaeErr( BPLIST_WORK * wk )
{
  BmpWinFrame_Clear( wk->win[WIN_TALK].win, WINDOW_TRANS_ON );
  return SEQ_BPL_IREKAE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�\���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqMsgWait( BPLIST_WORK * wk )
{
  if( PRINTSYS_PrintStreamGetState(wk->stream) == PRINTSTREAM_STATE_DONE ){
    PRINTSYS_PrintStreamDelete( wk->stream );
    return SEQ_BPL_TRG_WAIT;
  }
  return SEQ_BPL_MSG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�\����̃L�[�҂��V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqTrgWait( BPLIST_WORK * wk )
{
  if( CheckTimeOut( wk ) == TRUE ){
    return SEQ_BPL_ENDSET;
  }

  if( ( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ) || GFL_UI_TP_GetTrg() == TRUE ){
    if( wk->talk_win_clear == 1 ){
      BmpWinFrame_Clear( wk->win[WIN_TALK].win, WINDOW_TRANS_ON );
      wk->talk_win_clear = 0;
    }
    return wk->ret_seq;
  }
  return SEQ_BPL_TRG_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �{�^���A�j���I���҂��V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqButtonWait( BPLIST_WORK * wk )
{
  if( wk->btn_flg == 0 ){
    return wk->ret_seq;
  }
  return SEQ_BPL_BUTTON_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �񕜃V�[�P���X
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
/*
static int BPL_SeqStRcv( BPLIST_WORK * wk )
{
  BPLIST_DATA * dat;
  u32 pos;

  dat = wk->dat;
  pos = BPLISTMAIN_GetListRow( wk, dat->sel_poke );

  switch( wk->rcv_seq ){
  case 0:
    wk->poke[pos].pp =
//      BattleWorkPokemonParamGet( dat->bw, dat->client_no, dat->list_row[dat->sel_poke] );
        NULL;
//    BattlePokeList_ItemUseMsgSet( wk );
    if( wk->page == BPLIST_PAGE_PP_RCV ){
  //    wk->rcv_pp[0] = (u16)PP_Get(wk->poke[dat->sel_poke].pp,ID_PARA_pp1+dat->sel_wp,NULL);
      wk->rcv_pp[0] = PP_Get( wk->poke[pos].pp,ID_PARA_pp1+dat->sel_wp,NULL);
      wk->rcv_seq = 2;
    }else{
      wk->poke[pos].st = APP_COMMON_GetStatusIconAnime( wk->poke[pos].pp );
      if( wk->poke[pos].st == APP_COMMON_ST_ICON_NONE ){
//        GFL_CLACT_UNIT_SetDrawEnableCap( wk->cap[BPL_CA_STATUS1+dat->sel_poke], 0 );
        BattlePokeList_P1_LvPut( wk, dat->sel_poke );
      }
      wk->rcv_hp = PP_Get( wk->poke[pos].pp, ID_PARA_hp, NULL );
      wk->rcv_seq = 4;
    }
//    PlaySE( wk, SEQ_SE_DP_KAIFUKU );
    break;

  case 1:   // HP��
    if( wk->poke[pos].hp != wk->rcv_hp ){
      wk->poke[pos].hp++;
      BattlePokeList_P1_HPPut( wk, dat->sel_poke );
      break;
    }
    wk->rcv_seq = 3;
    break;

  case 2:   // PP��
    if( wk->poke[pos].waza[dat->sel_wp].pp != wk->rcv_pp[0] ){
      wk->poke[pos].waza[dat->sel_wp].pp++;
      BattlePokeList_PPRcv( wk, WIN_P7_SKILL1+dat->sel_wp, dat->sel_wp );
      break;
    }
    wk->rcv_seq = 3;
    break;

  case 3:
//    BattleBag_SubItem( dat->bw, dat->item, dat->bag_page, dat->heap );
    BattlePokeList_TalkMsgSet( wk );
    wk->ret_seq = SEQ_BPL_ENDSET;
    wk->dat->sel_pos[0] = pos;
    return SEQ_BPL_MSG_WAIT;

  case 4:   // �m���񕜂̂��߂ɂP�x�����Ă�
    if( wk->poke[pos].hp != wk->rcv_hp ){
      wk->poke[pos].hp++;
      BattlePokeList_P1_HPPut( wk, dat->sel_poke );
      BPL_HPRcvButtonPut( wk );
    }
    wk->rcv_seq = 1;
    break;

  }
  return SEQ_BPL_STRCV;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * PP�S�񕜃V�[�P���X
 *
 * @param wk    �퓬�|�P���X�g�̃��[�N
 *
 * @return  ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
/*
static int BPL_SeqPPAllRcv( BPLIST_WORK * wk )
{
  BPLIST_DATA * dat;
  u32 pos;
  u32 i, j;

  dat = wk->dat;
  pos = BPLISTMAIN_GetListRow( wk, dat->sel_poke );

  switch( wk->rcv_seq ){
  case 0:
    wk->poke[pos].pp =
//      BattleWorkPokemonParamGet( dat->bw, dat->client_no, dat->list_row[dat->sel_poke] );
      NULL;
    for( i=0; i<4; i++ ){
      if( wk->poke[pos].waza[i].id == 0 ){ continue; }
      wk->rcv_pp[i] = (u16)PP_Get(wk->poke[pos].pp,ID_PARA_pp1+i,NULL);
    }
//    BattlePokeList_ItemUseMsgSet( wk );
//    PlaySE( wk, SEQ_SE_DP_KAIFUKU );
    wk->rcv_seq = 1;
    break;

  case 1:   // PP��
    j = 0;
    for( i=0; i<4; i++ ){
      if( wk->poke[pos].waza[i].id == 0 ){
        j++;
        continue;
      }
      if( wk->poke[pos].waza[i].pp != wk->rcv_pp[i] ){
        wk->poke[pos].waza[i].pp++;
        BattlePokeList_PPRcv( wk, WIN_P7_SKILL1+i, i );
      }else{
        j++;
      }
    }
    if( j == 4 ){ wk->rcv_seq = 2; }
    break;

  case 2:
//    BattleBag_SubItem( dat->bw, dat->item, dat->bag_page, dat->heap );
    BattlePokeList_TalkMsgSet( wk );
    wk->ret_seq = SEQ_BPL_ENDSET;
    return SEQ_BPL_MSG_WAIT;
  }

  return SEQ_BPL_PPALLRCV;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �I���t�F�[�h�Z�b�g�V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @return  �ڍs����V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int BPL_SeqEndSet( BPLIST_WORK * wk )
{
  // �}���`���[�h���̑I���ʒu�␳
  if( BattlePokeList_MultiCheck( wk ) == TRUE ){
/*
    u32 i;
    for( i=0; i<BPL_SELNUM_MAX; i++ ){
      if( wk->dat->sel_pos[i] != BPL_SELPOS_NONE ){
        wk->dat->sel_pos[i] /= 2;
      }
			OS_Printf( "ENDSET : wk->dat->sel_pos[%d] = %d\n", i, wk->dat->sel_pos[i] );
    }
*/
    wk->dat->sel_poke /= 2;
  }
/*
  OS_Printf( "\n����������������������������������������������������������������������������\n" );
  OS_Printf( "sel_poke = %d\n", wk->dat->sel_poke );
  {
    u32 i;
    for( i=0; i<BPL_SELNUM_MAX; i++ ){
      OS_Printf( "sel_pos = %d\n", wk->dat->sel_pos[i] );
    }
  }
  OS_Printf( "����������������������������������������������������������������������������\n" );
*/

  PaletteFadeReq(
    wk->pfd, PF_BIT_SUB_ALL, 0xffff, BATTLE_BAGLIST_FADE_SPEED, 0, 16, 0, wk->dat->tcb_sys );
  return SEQ_BPL_ENDWAIT;
}

static int BPL_SeqEndWait( BPLIST_WORK * wk )
{
  if( PaletteFadeCheck( wk->pfd ) == 0 ){
    return SEQ_BPL_END;
  }
  return SEQ_BPL_ENDWAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * �I���V�[�P���X
 *
 * @param wk    ���[�N
 *
 * @retval  "TRUE = �I��"
 * @retval  "FALSE = �t�F�[�h��"
 */
//--------------------------------------------------------------------------------------------
static BOOL BPL_SeqEnd( GFL_TCB * tcb, BPLIST_WORK * wk )
{
  if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
    return FALSE;
  }

  wk->dat->end_flg = 1;
  if( wk->cursor_flg == TRUE ){
    *wk->dat->cursor_flg = 1;
  }else{
    *wk->dat->cursor_flg = 0;
  }
//  wk->dat->cursor_flg = BAPP_CursorMvWkGetFlag( wk->cmv_wk );

  BPL_MsgManExit( wk );
  BattlePokeList_ObjFree( wk );
  BattlePokeList_BmpFreeAll( wk );
  BPL_BgExit();

  GFL_TCBL_Exit( wk->tcbl );

//  BAPP_CursorMoveWorkFree( wk->cmv_wk );
  BPLISTUI_Exit( wk );

  BAPPTOOL_FreeCursor( wk->cpwk );

//  FontProc_UnloadFont( FONT_TOUCH );

  GFL_TCB_DeleteTask( tcb );
  GFL_HEAP_FreeMemory( wk );

//  GFL_HEAP_DeleteHeap( HEAPID_BATTLE_APP_TEST );

//	OS_Printf( "waza pos = %d\n", wk->dat->sel_wp );

  return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * VRAM������
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_VramInit(void)
{
  GFL_DISP_VRAM tbl = {
    GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_64_E,       // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_0_B,        // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_01_FG     // �e�N�X�`���p���b�g�X���b�g
  };
  GFL_DISP_SetBank( &tbl );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * BG������
 *
 * @param dat   ���X�g�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_BgInit( BPLIST_WORK * wk )
{
  { // BG SYSTEM
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
    };
    GFL_BG_SetBGModeDisp( &BGsys_data, GFL_BG_SUB_DISP );
  }

  { // BG (CHAR)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }

  { // BG2 (CHAR)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000, 0x8000,
      GX_BG_EXTPLTT_23, 2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }

  { // PARAM FONT (BMP)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME1_S );
  }

  { // WINDOW (BMP)
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x18000, 0x8000,
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
    GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );
}

//--------------------------------------------------------------------------------------------
/**
 * BG���
 *
 * @param ini   BGL�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_BgExit(void)
{
  GFL_DISP_GXS_SetVisibleControl(
    GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 |
    GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3 |
    GX_PLANEMASK_OBJ, VISIBLE_OFF );

  GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME2_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME3_S );
}

/*
#define PCOL_N_BLUE   ( PRINTSYS_LSB_MAKE( 7, 8, 0 ) )    // �t�H���g�J���[�F��
#define PCOL_N_RED    ( PRINTSYS_LSB_MAKE( 3, 4, 0 ) )    // �t�H���g�J���[�F��
#define PCOL_B_BLUE   ( PRINTSYS_LSB_MAKE( 10, 11, 0 ) )    // �t�H���g�J���[�F�{�^���p��
#define PCOL_B_RED    ( PRINTSYS_LSB_MAKE( 12, 13, 0 ) )    // �t�H���g�J���[�F�{�^���p��
*/
#define SFNT_COL_BLUE ( 7 )
#define SFNT_COL_RED  ( 3 )
#define BFNT_COL_BLUE ( 10 )
#define BFNT_COL_RED  ( 12 )

//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param wk    �|�P�������X�g��ʂ̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_BgGraphicSet( BPLIST_WORK * wk )
{
  ARCHANDLE* hdl;

  hdl  = GFL_ARC_OpenDataHandle( ARCID_B_PLIST_GRA,  GFL_HEAP_LOWID(wk->dat->heap) );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
    hdl, NARC_b_plist_gra_b_plist_lz_NCGR, GFL_BG_FRAME3_S, 0, 0, TRUE, wk->dat->heap );

  {
    NNSG2dScreenData * dat;
    void * buf;
/*
    buf = GFL_ARC_LoadDataAllocByHandle(
            hdl, NARC_b_plist_gra_b_plist_anm_NSCR, wk->dat->heap );
    NNS_G2dGetUnpackedScreenData( buf, &dat );
*/
    buf = GFL_ARCHDL_UTIL_LoadScreen(
            hdl, NARC_b_plist_gra_b_plist_anm_lz_NSCR, TRUE, &dat, wk->dat->heap );
    BattlePokeList_ButtonScreenMake( wk, (u16 *)dat->rawData );
    GFL_HEAP_FreeMemory( buf );

/*
    buf = GFL_ARC_LoadDataAllocByHandle(
            hdl, NARC_b_plist_gra_b_plist_anm2_NSCR, wk->dat->heap );
    NNS_G2dGetUnpackedScreenData( buf, &dat );
*/
    buf = GFL_ARCHDL_UTIL_LoadScreen(
            hdl, NARC_b_plist_gra_b_plist_anm2_lz_NSCR, TRUE, &dat, wk->dat->heap );
    BattlePokeList_ButtonScreenMake2( wk, (u16 *)dat->rawData );
    GFL_HEAP_FreeMemory( buf );
  }

  PaletteWorkSet_Arc(
    wk->pfd, ARCID_B_PLIST_GRA, NARC_b_plist_gra_b_plist_NCLR,
    wk->dat->heap, FADE_SUB_BG, 0x20*15, 0 );

  GFL_ARC_CloseDataHandle( hdl );

  {
    u16 * pltt = PaletteWorkDefaultWorkGet( wk->pfd, FADE_SUB_BG );
//    GFL_STD_MemCopy( &pltt[BPL_PAL_B_GREEN*16], wk->wb_pal, 0x20*2 );
    GFL_STD_MemCopy( &pltt[BPL_PAL_B_GREEN*16], wk->wb_pal, 0x20 );
  }

  // ���b�Z�[�W�E�B���h�E
  BmpWinFrame_GraphicSet(
    GFL_BG_FRAME0_S, TALK_WIN_CGX_POS, BPL_PAL_TALK_WIN, MENU_TYPE_SYSTEM, wk->dat->heap );
  PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_BG, BPL_PAL_TALK_WIN*16, 0x20 );

  // �t�H���g�p���b�g
  PaletteWorkSet_Arc(
    wk->pfd, ARCID_FONT, NARC_font_default_nclr,
    wk->dat->heap, FADE_SUB_BG, 0x20, BPL_PAL_SYS_FONT*16 );

/*
  // �V�X�e���t�H���g�p���b�g�Z�b�g
  PaletteWorkSet_Arc(
    wk->pfd, ARC_FONT, NARC_font_system_ncrl,
    wk->dat->heap, FADE_SUB_BG, 0x20, BPL_PAL_SYS_FONT*16 );

  // ��b�t�H���g�p���b�g�Z�b�g
  PaletteWorkSet_Arc(
    wk->pfd, ARC_FONT, NARC_font_talk_ncrl,
    wk->dat->heap, FADE_SUB_BG, 0x20, BPL_PAL_TALK_FONT*16 );

  // ��b�E�B���h�E�Z�b�g
  {
    WINTYPE wt = BattleWorkConfigWinTypeGet( wk->dat->bw );
    GFL_ARC_UTIL_TransVramBgCharacter(
      ARC_WINFRAME, TalkWinCgxArcGet(wt),
      GFL_BG_FRAME0_S, TALK_WIN_CGX_POS, 0, 0, wk->dat->heap );
    PaletteWorkSet_Arc(
      wk->pfd, ARC_WINFRAME, TalkWinPalArcGet(wt),
      wk->dat->heap, FADE_SUB_BG, 0x20, BPL_PAL_TALK_WIN*16 );
  }

  { // �{�^���t�H���g�p�p���b�g�̐ƐԂ��V�X�e���t�H���g�p�̃p���b�g�ɃR�s�[
    u16 * pal = PaletteWorkDefaultWorkGet( wk->pfd, FADE_SUB_BG );
    u16 * buf = GFL_HEAP_AllocMemory( wk->dat->heap, 0x20 );

    GFL_STD_MemCopy( &pal[BPL_PAL_SYS_FONT*16], buf, 0x20 );
    GFL_STD_MemCopy( &pal[BPL_PAL_HPGAGE*16+BFNT_COL_BLUE], &buf[SFNT_COL_BLUE], 4 );
    GFL_STD_MemCopy( &pal[BPL_PAL_HPGAGE*16+BFNT_COL_RED], &buf[SFNT_COL_RED], 4 );
    PaletteWorkSet( wk->pfd, buf, FADE_SUB_BG, BPL_PAL_SYS_FONT*16, 0x20 );
    GFL_HEAP_FreeMemory( buf );
  }
*/
}


//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�֘A�Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_MsgManSet( BPLIST_WORK * wk )
{
  wk->mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_b_plist_dat, wk->dat->heap );
//  wk->nfnt = NUMFONT_Create( 15, 14, FBMP_COL_NULL, wk->dat->heap );
  wk->nfnt = GFL_FONT_Create( ARCID_FONT, NARC_font_num_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, wk->dat->heap );
  wk->wset = WORDSET_Create( wk->dat->heap );
//  wk->que  = PRINTSYS_QUE_Create( wk->dat->heap );
  wk->que  = PRINTSYS_QUE_CreateEx( 2048, wk->dat->heap );
  wk->msg_buf = GFL_STR_CreateBuffer( TMP_MSG_BUF_SIZ, wk->dat->heap );

//  PRINTSYS_QUE_ForceCommMode( wk->que, TRUE );      // �e�X�g
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�֘A�폜
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_MsgManExit( BPLIST_WORK * wk )
{
  GFL_MSG_Delete( wk->mman );
//  NUMFONT_Delete( wk->nfnt );
  GFL_FONT_Delete( wk->nfnt );
  WORDSET_Delete( wk->wset );
  PRINTSYS_QUE_Delete( wk->que );
  GFL_STR_DeleteBuffer( wk->msg_buf );
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����f�[�^�쐬
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void PokeDataMakeCore( BPLIST_WORK * wk, POKEMON_PARAM * pp, BPL_POKEDATA * dat )
{
  u32 j;

  dat->pp = pp;
  if( pp == NULL ){
    dat->mons = 0;
    return;
  }

  dat->mons = PP_Get( pp, ID_PARA_monsno, NULL );
  if( dat->mons == 0 ){
    return;
  }

  dat->pow = PP_Get( dat->pp, ID_PARA_pow, NULL );
  dat->def = PP_Get( dat->pp, ID_PARA_def, NULL );
  dat->agi = PP_Get( dat->pp, ID_PARA_agi, NULL );
  dat->spp = PP_Get( dat->pp, ID_PARA_spepow, NULL );
  dat->spd = PP_Get( dat->pp, ID_PARA_spedef, NULL );
  dat->hp  = PP_Get( dat->pp, ID_PARA_hp, NULL );
  dat->mhp = PP_Get( dat->pp, ID_PARA_hpmax, NULL );

  dat->type1 = (u8)PP_Get( dat->pp, ID_PARA_type1, NULL );
  dat->type2 = (u8)PP_Get( dat->pp, ID_PARA_type2, NULL );

  dat->lv  = (u8)PP_Get( dat->pp, ID_PARA_level, NULL );
  if( PP_Get( dat->pp, ID_PARA_nidoran_nickname, NULL ) == TRUE ){
    dat->sex_put = 0;
  }else{
    dat->sex_put = 1;
  }
  dat->sex = PP_GetSex( dat->pp );
  dat->st  = APP_COMMON_GetStatusIconAnime( dat->pp );
  dat->egg = (u8)PP_Get( dat->pp, ID_PARA_tamago_flag, NULL );

  dat->spa  = (u16)PP_Get( dat->pp, ID_PARA_speabino, NULL );
  dat->item = (u16)PP_Get( dat->pp, ID_PARA_item, NULL );

  dat->style     = (u8)PP_Get( dat->pp, ID_PARA_style, NULL );
  dat->beautiful = (u8)PP_Get( dat->pp, ID_PARA_beautiful, NULL );
  dat->cute      = (u8)PP_Get( dat->pp, ID_PARA_cute, NULL );
  dat->clever    = (u8)PP_Get( dat->pp, ID_PARA_clever, NULL );
  dat->strong    = (u8)PP_Get( dat->pp, ID_PARA_strong, NULL );

  dat->cb = (u16)PP_Get( dat->pp, ID_PARA_cb_id, NULL );
  dat->form = (u8)PP_Get( dat->pp, ID_PARA_form_no, NULL );

  dat->now_exp     = PP_Get( dat->pp, ID_PARA_exp, NULL );
  dat->now_lv_exp  = PP_GetMinExp( dat->pp );
  if( dat->lv == 100 ){
    dat->next_lv_exp = dat->now_lv_exp;
  }else{
    dat->next_lv_exp = POKETOOL_GetMinExp( dat->mons, dat->form, dat->lv+1 );
  }

  for( j=0; j<4; j++ ){
    BPL_POKEWAZA * waza = &dat->waza[j];

    waza->id   = PP_Get( dat->pp, ID_PARA_waza1+j, NULL );
    if( waza->id == 0 ){ continue; }
    waza->pp   = PP_Get( dat->pp, ID_PARA_pp1+j, NULL );
    waza->mpp  = PP_Get( dat->pp, ID_PARA_pp_count1+j, NULL );
    waza->mpp  = WAZADATA_GetMaxPP( waza->id, waza->mpp );
    waza->type = WAZADATA_GetParam( waza->id, WAZAPARAM_TYPE );
    waza->kind = WAZADATA_GetParam( waza->id, WAZAPARAM_DAMAGE_TYPE );
    waza->hit  = WAZADATA_GetParam( waza->id, WAZAPARAM_HITPER );
    waza->pow  = WAZADATA_GetParam( waza->id, WAZAPARAM_POWER );
  }
}

static void BPL_PokeDataMake( BPLIST_WORK * wk )
{
  u32 i;

  if( wk->dat->multiMode == FALSE ){
    for( i=0; i<TEMOTI_POKEMAX; i++ ){
      if( i < PokeParty_GetPokeCount(wk->dat->pp) ){
        PokeDataMakeCore( wk, PokeParty_GetMemberPointer(wk->dat->pp,i), &wk->poke[i] );
      }else{
        PokeDataMakeCore( wk, NULL, &wk->poke[i] );
      }
    }
  }else{
    for( i=0; i<TEMOTI_POKEMAX/2; i++ ){
      if( i < PokeParty_GetPokeCount(wk->dat->pp) ){
        PokeDataMakeCore( wk, PokeParty_GetMemberPointer(wk->dat->pp,i), &wk->poke[i] );
      }else{
        PokeDataMakeCore( wk, NULL, &wk->poke[i] );
      }
      if( i < PokeParty_GetPokeCount(wk->dat->multi_pp) ){
        PokeDataMakeCore( wk, PokeParty_GetMemberPointer(wk->dat->multi_pp,i), &wk->poke[i+3] );
      }else{
        PokeDataMakeCore( wk, NULL, &wk->poke[i+3] );
      }
    }
  }


//  wk->poke[0].hp = 0;
//  wk->poke[1].hp = 0;
//  wk->poke[2].hp = 0;
//  wk->poke[3].hp = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����I��
 *
 * @param wk    ���[�N
 *
 * @retval  "TRUE = �I�����ꂽ"
 * @retval  "FALSE = ���I��"
 */
//--------------------------------------------------------------------------------------------
static u8 BPL_PokemonSelect( BPLIST_WORK * wk )
{
  u32 ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_LIST_POKE1:
  case BPLIST_UI_LIST_POKE2:
  case BPLIST_UI_LIST_POKE3:
  case BPLIST_UI_LIST_POKE4:
  case BPLIST_UI_LIST_POKE5:
  case BPLIST_UI_LIST_POKE6:
    if( BattlePokeList_PokeSetCheck( wk, ret ) != 0 ){
      wk->dat->sel_poke = (u8)ret;
      return TRUE;
    }
    break;

  case BPLIST_UI_LIST_RETURN:
/*
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
      wk->cancelSE = SEQ_SE_CANCEL2;
    }else{
      wk->cancelSE = SEQ_SE_DECIDE2;
    }
*/
    wk->dat->sel_poke = BPL_SEL_EXIT;
    return TRUE;

  case CURSORMOVE_CANCEL:         // �L�����Z��
//    wk->cancelSE = SEQ_SE_CANCEL2;
    wk->dat->sel_poke = BPL_SEL_EXIT;
    return TRUE;

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

  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * ����ւ��y�[�W�I��
 *
 * @param wk    ���[�N
 *
 * @retval  "0xff != �I�����ꂽ"
 * @retval  "0xff = ���I��"
 */
//--------------------------------------------------------------------------------------------
/*
static u8 BPL_IrekaeSelect( BPLIST_WORK * wk )
{
  u32 ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_CHG_ENTER:       // ���ꂩ����
  case BPLIST_UI_CHG_STATUS:      // �悳���݂�
  case BPLIST_UI_CHG_WAZA:        // �킴���݂�
  case BPLIST_UI_CHG_RETURN:      // ���ǂ�
    break;

  case CURSORMOVE_CANCEL:         // �L�����Z��
    ret = BPLIST_UI_CHG_RETURN;
    break;

  case CURSORMOVE_CURSOR_MOVE:    // �ړ�
    PlaySE( wk, SEQ_SE_SELECT1 );
    break;

  case CURSORMOVE_NO_MOVE_UP:     // �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_DOWN:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_LEFT:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_RIGHT:  // �\���L�[�E�������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_CURSOR_ON:      // �J�[�\���\��
  case CURSORMOVE_NONE:           // ����Ȃ�
    ret = 0xff;
    break;
  }

  return (u8)ret;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�I��
 *
 * @param wk    ���[�N
 *
 * @retval  "0xff != �I�����ꂽ"
 * @retval  "0xff = ���I��"
 */
//--------------------------------------------------------------------------------------------
/*
static u8 BPL_StatusMainSelect( BPLIST_WORK * wk )
{
  u32 ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_STATUS_UP:       // �O�̃|�P������
  case BPLIST_UI_STATUS_DOWN:     // ���̃|�P������
  case BPLIST_UI_STATUS_WAZA:     // �킴���݂�
  case BPLIST_UI_STATUS_RETURN:   // ���ǂ�
    break;

  case CURSORMOVE_CANCEL:         // �L�����Z��
    ret = BPLIST_UI_STATUS_RETURN;
    break;

  case CURSORMOVE_CURSOR_MOVE:    // �ړ�
    PlaySE( wk, SEQ_SE_SELECT1 );
    break;

  case CURSORMOVE_NO_MOVE_UP:     // �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_DOWN:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_LEFT:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_RIGHT:  // �\���L�[�E�������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_CURSOR_ON:      // �J�[�\���\��
  case CURSORMOVE_NONE:           // ����Ȃ�
    ret = 0xff;
    break;
  }

  return (u8)ret;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�Z�I��
 *
 * @param wk    ���[�N
 *
 * @retval  "0xff != �I�����ꂽ"
 * @retval  "0xff = ���I��"
 */
//--------------------------------------------------------------------------------------------
/*
static u8 BPL_StWazaSelect( BPLIST_WORK * wk )
{
  u32 ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_WAZA_SEL1:     // �Z�P
  case BPLIST_UI_WAZA_SEL2:     // �Z�Q
  case BPLIST_UI_WAZA_SEL3:     // �Z�R
  case BPLIST_UI_WAZA_SEL4:     // �Z�S
  case BPLIST_UI_WAZA_UP:       // �O�̃|�P������
  case BPLIST_UI_WAZA_DOWN:     // ���̃|�P������
  case BPLIST_UI_WAZA_STATUS:   // �悳���݂�
  case BPLIST_UI_WAZA_RETURN:   // ���ǂ�
    break;

  case CURSORMOVE_CANCEL:       // �L�����Z��
    ret = BPLIST_UI_WAZA_RETURN;
    break;

  case CURSORMOVE_CURSOR_MOVE:    // �ړ�
    PlaySE( wk, SEQ_SE_SELECT1 );
    break;

  case CURSORMOVE_NO_MOVE_UP:     // �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_DOWN:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_LEFT:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_RIGHT:  // �\���L�[�E�������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_CURSOR_ON:      // �J�[�\���\��
  case CURSORMOVE_NONE:           // ����Ȃ�
    ret = 0xff;
    break;
  }

  return (u8)ret;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�Z�ڍבI��
 *
 * @param wk    ���[�N
 *
 * @retval  "0xff != �I�����ꂽ"
 * @retval  "0xff = ���I��"
 */
//--------------------------------------------------------------------------------------------
/*
static u8 BPL_StInfoWazaSelect( BPLIST_WORK * wk )
{
  u32 ret = CURSORMOVE_MainCont( wk->cmwk );

  switch( ret ){
  case BPLIST_UI_WAZAINFO_SEL1:     // �Z�P
  case BPLIST_UI_WAZAINFO_SEL2:     // �Z�Q
  case BPLIST_UI_WAZAINFO_SEL3:     // �Z�R
  case BPLIST_UI_WAZAINFO_SEL4:     // �Z�S
  case BPLIST_UI_WAZAINFO_RETURN:   // ���ǂ�
    break;

  case CURSORMOVE_CANCEL:         // �L�����Z��
    ret = BPLIST_UI_WAZAINFO_RETURN;
    break;

  case CURSORMOVE_CURSOR_MOVE:    // �ړ�
    PlaySE( wk, SEQ_SE_SELECT1 );
    break;

  case CURSORMOVE_NO_MOVE_UP:     // �\���L�[�オ�����ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_DOWN:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_LEFT:   // �\���L�[���������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_NO_MOVE_RIGHT:  // �\���L�[�E�������ꂽ���A�ړ��Ȃ�
  case CURSORMOVE_CURSOR_ON:      // �J�[�\���\��
  case CURSORMOVE_NONE:           // ����Ȃ�
    ret = 0xff;
    break;
  }

  return (u8)ret;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �^�b�`�p�l���̃{�^����������
 *
 * @param wk    ���[�N
 * @param tbl   TP�f�[�^�e�[�u��
 *
 * @retval  "GFL_UI_TP_HIT_NONE = ������Ă��Ȃ�"
 * @retval  "GFL_UI_TP_HIT_NONE != �������{�^���ԍ�"
 */
//--------------------------------------------------------------------------------------------
/*
static int BPL_TPCheck( BPLIST_WORK * wk, const GFL_UI_TP_HITTBL * tbl )
{
  int ret = GFL_UI_TP_HitTrg( tbl );
#if 0
  if( ret != GFL_UI_TP_HIT_NONE ){
    u16 pat = 0xfffe;
    if( GFL_BG_CheckDot( wk->bgl, GFL_BG_FRAME3_S, sys.tp_x, sys.tp_y, &pat ) == FALSE ){
      return GFL_UI_TP_HIT_NONE;
    }
  }
#endif
  return ret;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �w��ʒu�Ƀ|�P���������݂��邩
 *
 * @param wk    ���[�N
 * @param pos   �ʒu
 *
 * @retval  "0 = ���Ȃ�"
 * @retval  "1 = �퓬��"
 * @retval  "2 = �T��"
 */
//--------------------------------------------------------------------------------------------
u8 BattlePokeList_PokeSetCheck( BPLIST_WORK * wk, s32 pos )
{
  if( wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ].mons == 0 ){
    return 0;
  }
/*
  if( pos == 0 ||
    ( (BattlePokeList_DoubleCheck(wk)||BattlePokeList_MultiCheck(wk)) && pos == 1 ) ){
    return 1;
  }
*/
  if( FightPokeCheck( wk, pos ) == TRUE ){
    return 1;
  }
  return 2;
}

//--------------------------------------------------------------------------------------------
/**
 * ���̃|�P�������擾
 *
 * @param wk    ���[�N
 * @param pos   ���݈ʒu
 * @param mv    �ړ�����
 *
 * @retval  "0xff = ���̃|�P�����Ȃ�"
 * @retval  "0xff != ���̃|�P�����̈ʒu"
 */
//--------------------------------------------------------------------------------------------
static u8 BPL_NextPokeGet( BPLIST_WORK * wk, s32 pos, s32 mv )
{
  s32 now = pos;

  if( BattlePokeList_MultiCheck( wk ) == TRUE ){
    u8 check_tbl[] = { 0, 2, 4, 1, 3, 5 };

    for( pos=0; pos<6; pos++ ){
      if( now == check_tbl[pos] ){ break; }
    }

    while( 1 ){
      pos += mv;

      if( pos < 0 ){
        pos = 5;
      }else if( pos >= 6 ){
        pos = 0;
      }
      if( now == check_tbl[pos] ){ break; }
      if( BattlePokeList_PokeSetCheck( wk, check_tbl[pos] ) != 0 ){
        if( wk->poke[ BPLISTMAIN_GetListRow(wk,check_tbl[pos]) ].egg == 0 ){
          return check_tbl[pos];
        }
      }
    }
  }else{
    while( 1 ){
      pos += mv;
      if( pos < 0 ){
        pos = 5;
      }else if( pos >= 6 ){
        pos = 0;
      }
      if( now == pos ){ break; }
      if( BattlePokeList_PokeSetCheck( wk, pos ) != 0 ){
        if( wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ].egg == 0 ){
          return (u8)pos;
        }
      }
    }
  }

  return 0xff;
}

//--------------------------------------------------------------------------------------------
/**
 * �v���[�g��������
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_PokePlateSet( BPLIST_WORK * wk )
{
  s16 i, j;

  if( wk->dat->mode == BPL_MODE_WAZASET ){ return; }

  for( i=0; i<TEMOTI_POKEMAX; i++ ){
    j = BattlePokeList_PokeSetCheck( wk, i );
    if( j == 1 ){
      GFL_BG_WriteScreen(
        wk->bgl, GFL_BG_FRAME2_S, wk->btn_plate1[0],
        PlatePos[i][0], PlatePos[i][1], PLATE_BG_SX, PLATE_BG_SY );
    }else if( j == 2 ){
      GFL_BG_WriteScreen(
        wk->bgl, GFL_BG_FRAME2_S, wk->btn_plate2[0],
        PlatePos[i][0], PlatePos[i][1], PLATE_BG_SX, PLATE_BG_SY );
    }
  }
  GFL_BG_LoadScreenV_Req( wk->bgl, GFL_BG_FRAME2_S );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �o���l�Q�[�W��������
 *
 * @param wk    ���[�N
 * @param page  �y�[�W�ԍ�
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ExpGagePut( BPLIST_WORK * wk, u8 page )
{
  BPL_POKEDATA * pd;
  u32 max;
  u32 now;
  u16 cgx;
  u16 px, py;
  u8  dot;
  u8  i;

  if( page != BPLIST_PAGE_MAIN ){ return; }

  pd = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ];

/* ���r�T�t�@�̈�ĉ���Lv100�ȏ�̌o���l�ɂȂ邽�߁A�}�C�i�X�\���ɂȂ��Ă��܂��s��Ώ� */
  if( pd->lv < 100 ){
    max = pd->next_lv_exp - pd->now_lv_exp;
    now = pd->now_exp - pd->now_lv_exp;
  }else{
    max = 0;
    now = 0;
  }

  dot = GAUGETOOL_GetNumDotto( now, max, EXP_DOT_MAX );

  for( i=0; i<EXP_DOT_CHR_MAX; i++ ){
    if( dot >= 8 ){
      cgx = EXP_CGX + 8;
    }else{
      cgx = EXP_CGX + dot;
    }

//    BPL_P3_ExpGagePut( wk, cgx, P3_EXPGAGE_PX+i, P3_EXPGAGE_PY );
		GFL_BG_FillScreen(
			GFL_BG_FRAME3_S, cgx, P3_EXPGAGE_PX+i, P3_EXPGAGE_PY, 1, 1, GFL_BG_SCRWRT_PALNL );

    if( dot < 8 ){
      dot = 0;
    }else{
      dot -= 8;
    }
  }

  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_S );
}

/*
static void BPL_P2_ExpGagePut( BPLIST_WORK * wk, u16 cgx, u16 px, u16 py )
{
  GFL_BG_FillScreen( wk->bgl, GFL_BG_FRAME3_S, cgx, px, py, 1, 1, GFL_BG_SCRWRT_PALNL );
  GFL_BG_FillScreen( wk->bgl, GFL_BG_FRAME3_S, cgx+32, px, py+1, 1, 1, GFL_BG_SCRWRT_PALNL );
}
*/
/*
static void BPL_P3_ExpGagePut( BPLIST_WORK * wk, u16 cgx, u16 px, u16 py )
{
  GFL_BG_FillScreen( GFL_BG_FRAME3_S, cgx, px, py, 1, 1, GFL_BG_SCRWRT_PALNL );
}
*/

#define HEART1_CHR_NUM  ( 0x140 )
#define HEART2_CHR_NUM  ( 0x125 )
#define HEART_PX    ( 2 )
#define HEART_PY    ( 14 )
#define HEART_MAX   ( 6 )

//--------------------------------------------------------------------------------------------
/**
 * �R���e�X�g�Z�̃n�[�g��������
 *
 * @param wk    ���[�N
 * @param chr   �g�p�L����
 * @param num   ���ڂ̃n�[�g��
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_ContestWazaHeartPutMain( BPLIST_WORK * wk, u16 chr, u8 num )
{
  GFL_BG_FillScreen(
    GFL_BG_FRAME3_S, chr, HEART_PX+num*2, HEART_PY, 1, 1, GFL_BG_SCRWRT_PALNL );
  GFL_BG_FillScreen(
    GFL_BG_FRAME3_S, chr+1, HEART_PX+num*2+1, HEART_PY, 1, 1, GFL_BG_SCRWRT_PALNL );
  GFL_BG_FillScreen(
    GFL_BG_FRAME3_S, chr+32, HEART_PX+num*2, HEART_PY+1, 1, 1, GFL_BG_SCRWRT_PALNL );
  GFL_BG_FillScreen(
    GFL_BG_FRAME3_S, chr+33, HEART_PX+num*2+1, HEART_PY+1, 1, 1, GFL_BG_SCRWRT_PALNL );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �R���e�X�g�Z�̃n�[�g������
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_ContestWazaHeartInit( BPLIST_WORK * wk )
{
  u16 i;

  for( i=0; i<HEART_MAX; i++ ){
    BPL_ContestWazaHeartPutMain( wk, HEART2_CHR_NUM, i );
  }
}
*/



//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�؂�ւ�
 *
 * @param wk      ���[�N
 * @param next_page ���̃y�[�W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static BOOL BPL_PageChange( BPLIST_WORK * wk, u8 next_page )
{
/*
  if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
    return FALSE;
  }

  BPL_PageChgBgScreenChg( wk, next_page );

  GFL_BG_ClearScreenCodeVReq( GFL_BG_FRAME0_S, 0 );
  GFL_BG_ClearScreenCodeVReq( GFL_BG_FRAME1_S, 0 );

  BattlePokeList_PageObjSet( wk, next_page );
  BattlePokeList_BmpFree( wk );
  BattlePokeList_BmpAdd( wk, next_page );
  BattlePokeList_BmpWrite( wk, next_page );
  BPL_ExpGagePut( wk, next_page );

//  BPL_ContestWazaHeartPut( wk, next_page );

//  BattlePokeList_CursorMoveSet( wk, next_page );
//  BPLISTUI_ChangePage( wk, next_page, 0 );
  CursorMoveSet( wk, next_page );

  BattlePokeList_ButtonPageScreenInit( wk, next_page );
  BattlePokeList_ButtonPalSet( wk, next_page );

  wk->page = next_page;

  return TRUE;
*/
  switch( wk->page_chg_seq ){
  case 0:
    if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
      BattlePokeList_BmpFree( wk );
      BattlePokeList_BmpAdd( wk, next_page );
      BattlePokeList_BmpWrite( wk, next_page );
      wk->page_chg_seq++;
    }
    break;

  case 1:
    if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
      BPL_PageChgBgScreenChg( wk, next_page );
      GFL_BG_ClearScreenCodeVReq( GFL_BG_FRAME0_S, 0 );
      GFL_BG_ClearScreenCodeVReq( GFL_BG_FRAME1_S, 0 );
      BPLISTBMP_SetStrScrn( wk );
      BPL_ExpGagePut( wk, next_page );
      BattlePokeList_PageObjSet( wk, next_page );
      CursorMoveSet( wk, next_page );
      BattlePokeList_ButtonPageScreenInit( wk, next_page );
      BattlePokeList_ButtonPalSet( wk, next_page );
      wk->page = next_page;
      wk->page_chg_seq = 0;
      return TRUE;
    }
    break;
  }

  return FALSE;
}

static const u32 ScreenArc[][2] =
{ // �{�^��BG�X�N���[��, �w�iBG�X�N���[��
  { NARC_b_plist_gra_poke_sel_t_lz_NSCR, NARC_b_plist_gra_poke_sel_u_lz_NSCR },       // �|�P�����I���y�[�W
  { NARC_b_plist_gra_poke_chg_t_lz_NSCR, NARC_b_plist_gra_poke_chg_u_lz_NSCR },       // �|�P��������ւ��y�[�W
  { NARC_b_plist_gra_st_main_t_lz_NSCR, NARC_b_plist_gra_st_main_u_lz_NSCR },       // �X�e�[�^�X���C���y�[�W
  { NARC_b_plist_gra_st_waza_sel_t_lz_NSCR, NARC_b_plist_gra_st_waza_sel_u_lz_NSCR },   // �X�e�[�^�X�ڍ׃y�[�W
  { NARC_b_plist_gra_st_waza_main_t_lz_NSCR, NARC_b_plist_gra_st_waza_main_u_lz_NSCR },   // �X�e�[�^�X�Z�y�[�W
  { NARC_b_plist_gra_item_waza_t_lz_NSCR, NARC_b_plist_gra_item_waza_u_lz_NSCR },     // PP�񕜋Z�I���y�[�W
  { NARC_b_plist_gra_waza_delb_sel_t_lz_NSCR, NARC_b_plist_gra_waza_delb_sel_u_lz_NSCR }, // �X�e�[�^�X�Z�Y��P�y�[�W�i�퓬�Z�I���j
  { NARC_b_plist_gra_waza_delb_main_t_lz_NSCR, NARC_b_plist_gra_waza_delb_main_u_lz_NSCR }, // �X�e�[�^�X�Z�Y��Q�y�[�W�i�퓬�Z�ڍׁj
//  { NARC_b_plist_gra_waza_delc_sel_t_NSCR, NARC_b_plist_gra_waza_delc_sel_u_NSCR }, // �X�e�[�^�X�Z�Y��R�y�[�W�i�R���e�X�g�Z�ڍׁj
//  { NARC_b_plist_gra_waza_delc_main_t_NSCR, NARC_b_plist_gra_waza_delc_main_u_NSCR }, // �X�e�[�^�X�Z�Y��S�y�[�W�i�R���e�X�g�Z�I���j
  { NARC_b_plist_gra_poke_sel_t_lz_NSCR, NARC_b_plist_gra_poke_sel_u_lz_NSCR },       // �m������ւ��I���y�[�W
};

//--------------------------------------------------------------------------------------------
/**
 * BG�X�N���[����������
 *
 * @param wk    ���[�N
 * @param page  �y�[�WID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PageChgBgScreenChg( BPLIST_WORK * wk, u8 page )
{
  NNSG2dScreenData * dat;
  void * buf;
  u32 i;

  for( i=0; i<2; i++ ){
/*
    buf = GFL_ARC_LoadDataAlloc( ARCID_B_PLIST_GRA, ScreenArc[page][i], wk->dat->heap );
    NNS_G2dGetUnpackedScreenData( buf, &dat );
*/
    buf = GFL_ARC_UTIL_LoadScreen(
      ARCID_B_PLIST_GRA, ScreenArc[page][i], TRUE, &dat, wk->dat->heap );
    GFL_BG_WriteScreen( GFL_BG_FRAME2_S+i, (u16 *)dat->rawData, 0, 0, 32, 24 );
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S+i );
    GFL_HEAP_FreeMemory( buf );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * ����ւ��`�F�b�N
 *
 * @param wk    ���[�N
 *
 * @retval  "TRUE = ����ւ���"
 * @retval  "FALSE = ����ւ��s��"
 */
//--------------------------------------------------------------------------------------------
static u8 BPL_IrekaeCheck( BPLIST_WORK * wk )
{
  BPL_POKEDATA * dat;
  STRBUF * str;
  u8  pos;

  pos = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  dat = &wk->poke[pos];

  // ���l
  if( BattlePokeList_MultiPosCheck( wk, pos ) == TRUE ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m05 );
/*
    {
      int pcn;

      pcn = BattleWorkPartnerClientNoGet( wk->dat->bw, wk->dat->client_no );
//      WORDSET_RegisterPlayerName( wk->wset, 0, BattleWorkMyStatusGet(wk->dat->bw,pcn) );
      WORDSET_RegisterTrainerNameBattle(
        wk->wset, 0, BattleWorkTrainerDataGet(wk->dat->bw,pcn) );
    }
*/
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
    return FALSE;
  }

  // �m��
  if( dat->hp == 0 ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m02 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, dat->pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
    return FALSE;
  }

  // �o�Ă���
//  if( BattlePokeList_PokeSetCheck( wk, wk->dat->sel_poke ) == 1 ){
/*
  if( wk->dat->list_row[wk->dat->sel_poke] == wk->dat->fight_poke1 ||
    wk->dat->list_row[wk->dat->sel_poke] == wk->dat->fight_poke2 ){
*/
  if( FightPokeCheck( wk, wk->dat->sel_poke ) == TRUE ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m01 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, dat->pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
    return FALSE;
  }

  // �^�}�S
  if( BPL_TamagoCheck( wk ) == TRUE ){
    GFL_MSG_GetString( wk->mman, mes_b_plist_m04, wk->msg_buf );
    return FALSE;
  }

  // �I������Ă���
//  if( wk->dat->double_sel != 6 && wk->dat->list_row[wk->dat->sel_poke] == wk->dat->double_sel ){
  if( ChangePokeCheck( wk, wk->dat->sel_poke ) == TRUE ){
    dat = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ];
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m18 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, dat->pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
    return FALSE;
  }

  // �Z
  if( wk->dat->chg_waza != 0 ){
    dat = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->init_poke) ];
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m03 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, dat->pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
    return FALSE;
  }

  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �^�}�S�`�F�b�N
 *
 * @param wk    ���[�N
 *
 * @retval  "TRUE = �^�}�S"
 * @retval  "FALSE = �^�}�S�ȊO"
 */
//--------------------------------------------------------------------------------------------
static u8 BPL_TamagoCheck( BPLIST_WORK * wk )
{
  if( wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].egg != 0 ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �_�u���o�g���`�F�b�N
 *
 * @param wk    ���[�N
 *
 * @retval  "TRUE = �_�u���o�g��"
 * @retval  "FALSE = �_�u���o�g���ȊO"
 */
//--------------------------------------------------------------------------------------------
/*
u8 BattlePokeList_DoubleCheck( BPLIST_WORK * wk )
{
  u32 type = BattleWorkFightTypeGet(wk->dat->bw);

  if( type != FIGHT_TYPE_2vs2_YASEI && type != FIGHT_TYPE_AI_MULTI &&
    (type & (FIGHT_TYPE_2vs2|FIGHT_TYPE_TAG)) ){
    return TRUE;
  }
  return FALSE;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �}���`�o�g���`�F�b�N
 *
 * @param wk    ���[�N
 *
 * @retval  "TRUE = �}���`�o�g��"
 * @retval  "FALSE = �}���`�o�g���ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL BattlePokeList_MultiCheck( BPLIST_WORK * wk )
{
  return wk->dat->multiMode;
/*
  u32 type = BattleWorkFightTypeGet(wk->dat->bw);

  if( type != FIGHT_TYPE_2vs2_YASEI && type != FIGHT_TYPE_AI_MULTI && (type&FIGHT_TYPE_MULTI) ){
    return TRUE;
  }
  return FALSE;
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �}���`�o�g���Ńp�[�g�i�[�̃|�P�������`�F�b�N
 *
 * @param wk    ���[�N
 * @param pos   �ʒu
 *
 * @retval  "TRUE = �͂�"
 * @retval  "FALSE = ������"
 */
//--------------------------------------------------------------------------------------------
u8 BattlePokeList_MultiPosCheck( BPLIST_WORK * wk, u8 pos )
{
  if( BattlePokeList_MultiCheck( wk ) == TRUE ){
/*
    if( wk->dat->multiPos == 0 ){
      if( pos >= 3 ){ return TRUE; }
    }else{
      if( pos < 3 ){ return TRUE; }
    }
*/
    if( pos >= 3 ){ return TRUE; }
  }
  return FALSE;

/*
  if( BattlePokeList_MultiCheck( wk ) == TRUE ){
    if( wk->dat->multiPos == CLIENT_TYPE_A ){
      if( ( pos & 1 ) != 0 ){ return TRUE; }
    }else{
      if( ( pos & 1 ) == 0 ){ return TRUE; }
    }
  }
  return FALSE;
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�Y��`�F�b�N
 *
 * @param   wk    ���[�N
 *
 * @return  FIELD_SKILL_CHECK_RET
 */
//--------------------------------------------------------------------------------------------
static FIELD_SKILL_CHECK_RET BPL_HidenCheck( BPLIST_WORK * wk )
{
  u16 waza;

  // �V�����o����Z�̓`�F�b�N���Ȃ��Ă����n�Y
  if( wk->dat->sel_wp == 4 ){
    return FSCR_OK;
  }

  waza = wk->poke[wk->dat->sel_poke].waza[wk->dat->sel_wp].id;
  return FIELD_SKILL_CHECK_CheckForgetSkill( wk->dat->gamedata, waza, wk->dat->heap );

/*
  if( wk->dat->sel_wp == 4 ){
    waza = wk->dat->chg_waza;
  }else{
    waza = wk->poke[wk->dat->sel_poke].waza[wk->dat->sel_wp].id;
  }
  return FIELD_SKILL_CHECK_CheckForgetSkill( wk->dat->gamedata, waza, wk->dat->heap );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�p�����[�^���\���Ɂi�퓬�j
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_HidenOff_Battle( BPLIST_WORK * wk )
{
  // ���ރA�C�R��
  GFL_CLACT_UNIT_SetDrawEnableCap( wk->cap[BPL_CA_BUNRUI], 0 );

  // ����
  GF_BGL_BmpWinOffVReq( &wk->add_win[WIN_P6_BRNAME] );

  // ����
  GF_BGL_BmpWinOffVReq( &wk->add_win[WIN_P6_HITNUM] );

  // �З�
  GF_BGL_BmpWinOffVReq( &wk->add_win[WIN_P6_POWNUM] );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �Z�p�����[�^���\���Ɂi�R���e�X�g�j
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_HidenOff_Contest( BPLIST_WORK * wk )
{
  // �n�[�g
  BPL_ContestWazaHeartInit( wk );
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_S );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�������炷
 *
 * @param bw    �퓬�̃��[�N
 * @param item  �A�C�e���ԍ�
 * @param page  �|�P�b�g�ԍ�
 * @param heap  �q�[�vID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BattleBag_SubItem( BATTLE_WORK * bw, u16 item, u16 page, u32 heap )
{
  if( item != ITEM_AOIBIIDORO && item != ITEM_AKAIBIIDORO && item != ITEM_KIIROBIIDORO ){
    MyItem_SubItem( BattleWorkMyItemGet(bw), item, 1, heap );
  }
  MyItem_BattleBagLastItemSet( BattleWorkBagCursorGet(bw), item, page );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �w��ʒu�̃|�P�������퓬�ɏo�Ă��邩
 *
 * @param wk    �퓬���X�g���[�N
 * @param pos   �ʒu
 *
 * @retval  "TRUE = �o�Ă���"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL FightPokeCheck( BPLIST_WORK * wk, u32 pos )
{
  pos = BPLISTMAIN_GetListRow( wk, pos );

  // �}���`�o�g����
  if( BattlePokeList_MultiCheck( wk ) == TRUE ){
    if( pos == 0 || pos == 3 ){
      return TRUE;
    }
    return FALSE;
  }

/*
  switch( wk->dat->rule ){
  case BTL_RULE_SINGLE:   // �V���O��
    if( pos == 0 ){
      return TRUE;
    }
    break;

  case BTL_RULE_DOUBLE:   // �_�u��
    if( pos == 0 || pos == 1 ){
      return TRUE;
    }
    break;

  case BTL_RULE_TRIPLE:   // �g���v��
  case BTL_RULE_ROTATION: // ���[�e�[�V����
    if( pos == 0 || pos == 1 || pos == 2 ){
      return TRUE;
    }
  }
*/
	if( pos < wk->dat->fight_poke_max ){
		return TRUE;
	}
  return FALSE;
}

static BOOL FightPosCheck( BPLIST_WORK * wk, u32 pos )
{
/*
  switch( wk->dat->rule ){
  case BTL_RULE_SINGLE:   // �V���O��
    if( pos == 0 ){
      return TRUE;
    }
    break;

  case BTL_RULE_DOUBLE:   // �_�u��
    if( pos == 0 || pos == 1 ){
      return TRUE;
    }
    break;

  case BTL_RULE_TRIPLE:   // �g���v��
  case BTL_RULE_ROTATION: // ���[�e�[�V����
    if( pos == 0 || pos == 1 || pos == 2 ){
      return TRUE;
    }
  }
*/
	if( pos < wk->dat->fight_poke_max ){
		return TRUE;
	}
  return FALSE;
}

static BOOL ChangePokeCheck( BPLIST_WORK * wk, u32 pos )
{
  pos = BPLISTMAIN_GetListRow( wk, pos );

/*
  switch( wk->dat->rule ){
  case BTL_RULE_SINGLE:   // �V���O��
    break;

  case BTL_RULE_DOUBLE:   // �_�u��
  case BTL_RULE_TRIPLE:   // �g���v��
  case BTL_RULE_ROTATION: // ���[�e�[�V����
    if( pos == wk->dat->change_sel[0] || pos == wk->dat->change_sel[1] ){
      return TRUE;
    }
    return CheckListRow( wk, pos );
  }
*/
	if( wk->dat->fight_poke_max != 1 ){
    if( pos == wk->dat->change_sel[0] || pos == wk->dat->change_sel[1] ){
      return TRUE;
    }
    return CheckListRow( wk, pos );
	}
  return FALSE;
}



static void CursorMoveSet( BPLIST_WORK * wk, u8 page )
{
  u32 pos;

  switch( page ){
  case BPLIST_PAGE_SELECT:    // �|�P�����I���y�[�W
  case BPLIST_PAGE_DEAD:      // �m������ւ��I���y�[�W
    wk->chg_page_cp = 0;
    wk->dat->sel_wp = 0;
    pos = wk->dat->sel_poke;
    break;

  case BPLIST_PAGE_POKE_CHG:    // �|�P��������ւ��y�[�W
    wk->dat->sel_wp = 0;
    pos = wk->chg_page_cp;
    break;

  case BPLIST_PAGE_WAZA_SEL:    // �X�e�[�^�X�Z�I���y�[�W
  case BPLIST_PAGE_SKILL:       // �X�e�[�^�X�Z�ڍ׃y�[�W
    pos = wk->dat->sel_wp;
    break;

  case BPLIST_PAGE_WAZASET_BS:  // �X�e�[�^�X�Z�Y��P�y�[�W�i�퓬�Z�I���j
    pos = wk->wws_page_cp;
    break;

  case BPLIST_PAGE_WAZASET_BI:  // �X�e�[�^�X�Z�Y��Q�y�[�W�i�퓬�Z�ڍׁj
    pos = wk->wwm_page_cp;
    break;

  case BPLIST_PAGE_MAIN:      // �X�e�[�^�X���C���y�[�W
  case BPLIST_PAGE_PP_RCV:    // PP�񕜋Z�I���y�[�W
    pos = 0;
    break;
  }

  BPLISTUI_ChangePage( wk, page, pos );
}





//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
static const u8 InitListRowMulti[2][6] =
{
  { 0, 3, 1, 4, 2, 5 },
  { 3, 0, 4, 1, 5, 2 },
};
static void InitListRow( BPLIST_WORK * wk )
{
  u32 i;

  // ���̉�ʂł̕��т�������
  if( wk->dat->multiMode == TRUE ){
    for( i=0; i<TEMOTI_POKEMAX; i++ ){
      wk->listRow[i] = InitListRowMulti[wk->dat->multiPos][i];
    }
  }else{
    for( i=0; i<TEMOTI_POKEMAX; i++ ){
      wk->listRow[i] = i;
    }
  }
  //
  for( i=0; i<BPL_SELNUM_MAX; i++ ){
    wk->dat->sel_pos[i] = BPL_SELPOS_NONE;
  }
  // ���O������
  for( i=0; i<BPL_SELNUM_MAX-1; i++ ){
    wk->chg_log[i].pos1 = BPL_SELPOS_NONE;
    wk->chg_log[i].pos2 = BPL_SELPOS_NONE;
  }
}

static void ChangeListRow( BPLIST_WORK * wk, u32 pos1, u32 pos2 )
{
  u8  tmp;

  tmp = wk->listRow[pos1];
  wk->listRow[pos1] = wk->listRow[pos2];
  wk->listRow[pos2] = tmp;
}

static BOOL CheckListRow( BPLIST_WORK * wk, u32 pos )
{
  u32 i;

  for( i=0; i<BPL_SELNUM_MAX; i++ ){
    if( wk->dat->sel_pos[i] == pos ){
      return TRUE;
    }
  }
  return FALSE;
}

u8 BPLISTMAIN_GetListRow( BPLIST_WORK * wk, u32 pos )
{
  return wk->listRow[pos];
}

/*
static BOOL CheckRetNum( BPLIST_WORK * wk )
{
  u16 i, j;

  j = 0;
  for( i=0; i<BPL_SELNUM_MAX; i++ ){
    if( wk->dat->sel_pos[i] == BPL_SELPOS_SET ){
      j++;
      if( j == 2 ){
        return FALSE;
      }
    }
  }
  return TRUE;
}
*/
/*
static void SetRetNum( BPLIST_WORK * wk )
{
  u32 i;

  for( i=0; i<BPL_SELNUM_MAX; i++ ){
    if( wk->dat->sel_pos[i] == BPL_SELPOS_SET ){
      wk->dat->sel_pos[i] = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
      break;
    }
  }

  ChangeListRow( wk, i, wk->dat->sel_poke );
}
*/

static void SetRetNum( BPLIST_WORK * wk, u8 pos, u8 row )
{
  wk->dat->sel_pos[pos] = BPLISTMAIN_GetListRow( wk, row );
}

static void SetLog( BPLIST_WORK * wk, u8 pos1, u8 pos2 )
{
  u8  i;

  // ���O�쐬
  for( i=0; i<BPL_SELNUM_MAX-1; i++ ){
    if( wk->chg_log[i].pos1 == BPL_SELPOS_NONE ){
      wk->chg_log[i].pos1 = pos1;
      wk->chg_log[i].pos2 = pos2;
      break;
    }
  }
}

BOOL BPLISTMAIN_GetNewLog( BPLIST_WORK * wk, u8 * pos1, u8 * pos2, BOOL del )
{
  s32 i;

  for( i=BPL_SELNUM_MAX-1-1; i>=0; i-- ){
    if( wk->chg_log[i].pos1 != BPL_SELPOS_NONE ){
      *pos1 = wk->chg_log[i].pos1;
      *pos2 = wk->chg_log[i].pos2;
      if( del == TRUE ){
        wk->chg_log[i].pos1 = BPL_SELPOS_NONE;
        wk->chg_log[i].pos2 = BPL_SELPOS_NONE;
      }
      return TRUE;
    }
  }
  return FALSE;
}



static int BPL_SeqPageChgDead( BPLIST_WORK * wk )
{
  if( BPL_PageChange( wk, BPLIST_PAGE_DEAD ) == FALSE ){
    wk->chg_poke_sel = wk->dat->sel_poke;
    return SEQ_BPL_PAGECHG_DEAD;
  }
  return SEQ_BPL_POKESEL_DEAD;
}

static BOOL CheckDeadChange( BPLIST_WORK * wk )
{
  BPL_POKEDATA * dat;
  STRBUF * str;
  u8  pos;

  pos = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  dat = &wk->poke[pos];

  // ���l
  if( BattlePokeList_MultiPosCheck( wk, pos ) == TRUE ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m05 );
/*
    {
      int pcn;

      pcn = BattleWorkPartnerClientNoGet( wk->dat->bw, wk->dat->client_no );
//      WORDSET_RegisterPlayerName( wk->wset, 0, BattleWorkMyStatusGet(wk->dat->bw,pcn) );
      WORDSET_RegisterTrainerNameBattle(
        wk->wset, 0, BattleWorkTrainerDataGet(wk->dat->bw,pcn) );
    }
*/
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
    return FALSE;
  }

  // ����ւ��\�ʒu�ł͂Ȃ�
  if( FightPosCheck( wk, wk->dat->sel_poke ) == FALSE ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m21 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, dat->pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
    return FALSE;
  }

  // �m��
  if( dat->hp != 0 ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m01 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, dat->pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
    return FALSE;
  }

/*
  // �^�}�S
  if( BPL_TamagoCheck( wk ) == TRUE ){
    GFL_MSG_GetString( wk->mman, mes_b_plist_m04, wk->msg_buf );
    return FALSE;
  }

  // �I������Ă���
//  if( wk->dat->double_sel != 6 && wk->dat->list_row[wk->dat->sel_poke] == wk->dat->double_sel ){
  if( ChangePokeCheck( wk, wk->dat->sel_poke ) == TRUE ){
    dat = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ];
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m18 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, dat->pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
    return FALSE;
  }

  // �Z
  if( wk->dat->chg_waza != 0 ){
    dat = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->init_poke) ];
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m03 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, dat->pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
    return FALSE;
  }
*/

  return TRUE;
}

static int BPL_SeqPokeSelDead( BPLIST_WORK * wk )
{
  if( PaletteFadeCheck( wk->pfd ) != 0 ){ return SEQ_BPL_POKESEL_DEAD; }

  if( BPL_PokemonSelect( wk ) == TRUE ){
    if( wk->dat->sel_poke == BPL_SEL_EXIT ){
//      PlaySE( wk, wk->cancelSE );
      PlaySE( wk, SEQ_SE_CANCEL2 );
      BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_RET );
      wk->dat->sel_poke = wk->chg_poke_sel;
      wk->ret_seq = SEQ_BPL_PAGECHG_IREKAE;
      return SEQ_BPL_BUTTON_WAIT;
    }else if( wk->dat->sel_poke == wk->chg_poke_sel ){
      return SEQ_BPL_PAGECHG_IREKAE;
    }else{
/*
      PlaySE( wk, SEQ_SE_DECIDE2 );
      BattlePokeList_ButtonAnmInit( wk, BPL_BUTTON_POKE1+wk->dat->sel_poke );
      if( wk->dat->mode == BPL_MODE_ITEMUSE ){
        return BPL_PokeItemUse( wk );
      }else{
        wk->ret_seq = SEQ_BPL_PAGECHG_IREKAE;
      }
      return SEQ_BPL_BUTTON_WAIT;
*/
      if( CheckDeadChange( wk ) == TRUE ){
        SetLog( wk, wk->dat->sel_poke, wk->chg_poke_sel );
        SetRetNum( wk, wk->dat->sel_poke, wk->chg_poke_sel );
        wk->chg_pos1 = wk->dat->sel_poke;
        wk->chg_pos2 = wk->chg_poke_sel;
        wk->btn_seq = 0;
        return SEQ_BPL_POKECHG_DEAD;
      }else{
        BattlePokeList_TalkMsgSet( wk );
        wk->ret_seq = SEQ_BPL_DEAD_ERR_RET;
        return SEQ_BPL_MSG_WAIT;
      }
    }
  }

  return SEQ_BPL_POKESEL_DEAD;
}

static int BPL_SeqDeadErrRet( BPLIST_WORK * wk )
{
  BmpWinFrame_Clear( wk->win[WIN_TALK].win, WINDOW_TRANS_ON );
  BPLISTBMP_DeadSelInfoMesPut( wk );
  BPLISTBMP_SetCommentScrn( wk );
  return SEQ_BPL_POKESEL_DEAD;
}



static void GetDeadScrnPos( u8 * x, u8 * y, u8 num )
{
  *x = ( num & 1 ) * BPL_COMM_BSX_PLATE;
  *y = num / 2 * BPL_COMM_BSY_PLATE + ( num & 1 );
}

static void MakeDeadChgScrn( u16 * scrn, u8 frm, u8 num )
{
  u16 * buf;
  u8  x, y;
  u8  i;

  buf = GFL_BG_GetScreenBufferAdrs( frm );
  GetDeadScrnPos( &x, &y, num );

  for( i=0; i<BPL_COMM_BSY_PLATE; i++ ){
    GFL_STD_MemCopy16( &buf[(y+i)*32+x], &scrn[i*BPL_COMM_BSX_PLATE], BPL_COMM_BSX_PLATE*2 );
  }
}

static void MoveDeadChgObj( BPLIST_WORK * wk, u8 num, u8 mode )
{
  u8  pos;
  s8  mv;

  if( num & 1 ){
    if( mode == 0 ){
      mv = 8;
    }else{
      mv = -8;
    }
  }else{
    if( mode == 0 ){
      mv = -8;
    }else{
      mv = 8;
    }
  }

  BPLISTOBJ_MoveDeadChg( wk, num, mv );
}

static BOOL MoveDeadChangeMain( BPLIST_WORK * wk )
{
  switch( wk->btn_seq ){
  case 0:
    wk->chg_wfrm = BGWINFRM_Create( BGWINFRM_TRANS_VBLANK, 4, wk->dat->heap );
    BGWINFRM_Add( wk->chg_wfrm, 0, GFL_BG_FRAME1_S, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
    BGWINFRM_Add( wk->chg_wfrm, 1, GFL_BG_FRAME2_S, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
    BGWINFRM_Add( wk->chg_wfrm, 2, GFL_BG_FRAME1_S, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
    BGWINFRM_Add( wk->chg_wfrm, 3, GFL_BG_FRAME2_S, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
    {
      u16 * buf = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID(wk->dat->heap), BPL_COMM_BSX_PLATE*BPL_COMM_BSY_PLATE*2 );
      MakeDeadChgScrn( buf, GFL_BG_FRAME1_S, wk->chg_pos1 );
      BGWINFRM_FrameSet( wk->chg_wfrm, 0, buf );
      MakeDeadChgScrn( buf, GFL_BG_FRAME2_S, wk->chg_pos1 );
      BGWINFRM_FrameSet( wk->chg_wfrm, 1, buf );
      MakeDeadChgScrn( buf, GFL_BG_FRAME1_S, wk->chg_pos2 );
      BGWINFRM_FrameSet( wk->chg_wfrm, 2, buf );
      MakeDeadChgScrn( buf, GFL_BG_FRAME2_S, wk->chg_pos2 );
      BGWINFRM_FrameSet( wk->chg_wfrm, 3, buf );
      GFL_HEAP_FreeMemory( buf );
    }
    {
      u8  x, y;
      GetDeadScrnPos( &x, &y, wk->chg_pos1 );
      BGWINFRM_FramePut( wk->chg_wfrm, 0, x, y );
      BGWINFRM_FramePut( wk->chg_wfrm, 1, x, y );
      GetDeadScrnPos( &x, &y, wk->chg_pos2 );
      BGWINFRM_FramePut( wk->chg_wfrm, 2, x, y );
      BGWINFRM_FramePut( wk->chg_wfrm, 3, x, y );
    }
    BGWINFRM_MoveMain( wk->chg_wfrm );
    wk->btn_seq++;
    break;

  case 1:
    if( wk->chg_pos1 & 1 ){
      BGWINFRM_MoveInit( wk->chg_wfrm, 0, 1, 0, BPL_COMM_BSX_PLATE );
      BGWINFRM_MoveInit( wk->chg_wfrm, 1, 1, 0, BPL_COMM_BSX_PLATE );
    }else{
      BGWINFRM_MoveInit( wk->chg_wfrm, 0, -1, 0, BPL_COMM_BSX_PLATE );
      BGWINFRM_MoveInit( wk->chg_wfrm, 1, -1, 0, BPL_COMM_BSX_PLATE );
    }
    if( wk->chg_pos2 & 1 ){
      BGWINFRM_MoveInit( wk->chg_wfrm, 2, 1, 0, BPL_COMM_BSX_PLATE );
      BGWINFRM_MoveInit( wk->chg_wfrm, 3, 1, 0, BPL_COMM_BSX_PLATE );
    }else{
      BGWINFRM_MoveInit( wk->chg_wfrm, 2, -1, 0, BPL_COMM_BSX_PLATE );
      BGWINFRM_MoveInit( wk->chg_wfrm, 3, -1, 0, BPL_COMM_BSX_PLATE );
    }
    wk->btn_seq++;
    break;

  case 2:
    BGWINFRM_MoveMain( wk->chg_wfrm );
    MoveDeadChgObj( wk, wk->chg_pos1, 0 );
    MoveDeadChgObj( wk, wk->chg_pos2, 0 );
    {
      s8  x1, y1, x2, y2;
      BGWINFRM_PosGet( wk->chg_wfrm, 0, &x1, &y1 );
      BGWINFRM_PosGet( wk->chg_wfrm, 2, &x2, &y2 );
      if( BGWINFRM_MoveCheck( wk->chg_wfrm, 0 ) == FALSE ){
        ChangeListRow( wk, wk->chg_pos1, wk->chg_pos2 );
        BGWINFRM_FramePut( wk->chg_wfrm, 0, x2, y2 );
        BGWINFRM_FramePut( wk->chg_wfrm, 1, x2, y2 );
        BGWINFRM_FramePut( wk->chg_wfrm, 2, x1, y1 );
        BGWINFRM_FramePut( wk->chg_wfrm, 3, x1, y1 );
        BPLISTOBJ_ChgDeadSel( wk, wk->chg_pos1, wk->chg_pos2 );
        wk->btn_seq++;
      }
    }
    break;

  case 3:
    BGWINFRM_MoveMain( wk->chg_wfrm );
    if( wk->chg_pos2 & 1 ){
      BGWINFRM_MoveInit( wk->chg_wfrm, 0, -1, 0, BPL_COMM_BSX_PLATE );
      BGWINFRM_MoveInit( wk->chg_wfrm, 1, -1, 0, BPL_COMM_BSX_PLATE );
    }else{
      BGWINFRM_MoveInit( wk->chg_wfrm, 0, 1, 0, BPL_COMM_BSX_PLATE );
      BGWINFRM_MoveInit( wk->chg_wfrm, 1, 1, 0, BPL_COMM_BSX_PLATE );
    }
    if( wk->chg_pos1 & 1 ){
      BGWINFRM_MoveInit( wk->chg_wfrm, 2, -1, 0, BPL_COMM_BSX_PLATE );
      BGWINFRM_MoveInit( wk->chg_wfrm, 3, -1, 0, BPL_COMM_BSX_PLATE );
    }else{
      BGWINFRM_MoveInit( wk->chg_wfrm, 2, 1, 0, BPL_COMM_BSX_PLATE );
      BGWINFRM_MoveInit( wk->chg_wfrm, 3, 1, 0, BPL_COMM_BSX_PLATE );
    }
    wk->btn_seq++;
    break;

  case 4:
    BGWINFRM_MoveMain( wk->chg_wfrm );
    MoveDeadChgObj( wk, wk->chg_pos1, 1 );
    MoveDeadChgObj( wk, wk->chg_pos2, 1 );
    {
      s8  x1, y1, x2, y2;
      BGWINFRM_PosGet( wk->chg_wfrm, 0, &x1, &y1 );
      BGWINFRM_PosGet( wk->chg_wfrm, 2, &x2, &y2 );
      if( BGWINFRM_MoveCheck( wk->chg_wfrm, 0 ) == FALSE ){
        wk->btn_seq++;
      }
    }
    break;

  case 5:
    BGWINFRM_Exit( wk->chg_wfrm );
    wk->btn_seq = 0;
    return FALSE;
  }

  return TRUE;
}

static int BPL_SeqPokeChgDead( BPLIST_WORK * wk )
{
  if( MoveDeadChangeMain( wk ) == FALSE ){
    if( CheckNextDeadSel( wk ) == FALSE ){
      return SEQ_BPL_ENDSET;
    }
    BPLISTANM_RetButtonPut( wk );
    BPLISTBMP_PokeSelInfoMesPut( wk );
    BPLISTBMP_SetCommentScrn( wk );
    return SEQ_BPL_SELECT;
  }
  return SEQ_BPL_POKECHG_DEAD;
}

// �m������ւ����삪�K�v���ǂ���
static BOOL CheckDeadPoke( BPLIST_WORK * wk )
{
  u8  cnt;
  u8  max;
  u8  i;

  // �}���`�o�g����
  if( BattlePokeList_MultiCheck( wk ) == TRUE ){
    return FALSE;
  }

/*
  if( wk->dat->rule == BTL_RULE_SINGLE ){
    return FALSE;
  }else if( wk->dat->rule == BTL_RULE_DOUBLE ){
    max = 2;
  }else{
    max = 3;
  }
*/
	if( wk->dat->fight_poke_max == 1 ){
		return FALSE;
	}else{
		max = wk->dat->fight_poke_max;
	}

  // �󂫂���������
  cnt = 0;
  for( i=0; i<max; i++ ){
    u32 row = BPLISTMAIN_GetListRow( wk, i );
    if( wk->poke[row].hp == 0 ){
      cnt++;
      if( cnt == 2 ){
        return TRUE;
      }
    }
  }

/*
  // ����ւ��\�ȃ|�P�������������邩
  cnt = 0;
  for( i=max; i<TEMOTI_POKEMAX; i++ ){
    if( wk->poke[i].mons != 0 && wk->poke[i].egg != 0 ){
      cnt++;
      if( cnt == 2 ){
        return TRUE;
      }
    }
  }
*/

  return FALSE;
}

static BOOL CheckNextDeadSel( BPLIST_WORK * wk )
{
  u8  max;
  u8  i;

  // �}���`�o�g����
  if( BattlePokeList_MultiCheck( wk ) == TRUE ){
    return FALSE;
  }

/*
  if( wk->dat->rule == BTL_RULE_SINGLE ){
    return FALSE;
  }else if( wk->dat->rule == BTL_RULE_DOUBLE ){
    max = 2;
  }else{
    max = 3;
  }
*/
	if( wk->dat->fight_poke_max == 1 ){
		return FALSE;
	}else{
		max = wk->dat->fight_poke_max;
	}

  // ����ւ��\�ȃ|�P���������邩
  for( i=max; i<TEMOTI_POKEMAX; i++ ){
    u32 row = BPLISTMAIN_GetListRow( wk, i );
    if( wk->poke[row].mons != 0 && wk->poke[row].egg == 0 && wk->poke[row].hp != 0 ){
      return TRUE;
    }
  }

  return FALSE;
}

// �ォ�猟�����ĕm���̉ӏ��ɋ����I�ɃZ�b�g
static void SetDeadChangeData( BPLIST_WORK * wk )
{
  // �}���`�̏ꍇ�͗����ʒu���֌W����̂ŁA�m���ɂO�ɓ����
  if( BattlePokeList_MultiCheck( wk ) == TRUE ){
//    wk->dat->sel_pos[0] = wk->dat->sel_poke;
		wk->dat->sel_pos[0] = BPLISTMAIN_GetListRow( wk, wk->dat->sel_poke );
  }else{
    u32 i;

    for( i=0; i<BPL_SELNUM_MAX; i++ ){
      u32 row = BPLISTMAIN_GetListRow( wk, i );
      if( wk->poke[row].hp == 0 ){
        wk->dat->sel_pos[i] = wk->dat->sel_poke;
        break;
      }
    }
  }
}

// �L�����Z���ݒ�
static void SetSelPosCancel( BPLIST_WORK * wk )
{
  u32 i;

  for( i=0; i<BPL_SELNUM_MAX; i++ ){
    wk->dat->sel_pos[i] = BPL_SELPOS_NONE;
  }
}

// �����I���`�F�b�N
static BOOL CheckTimeOut( BPLIST_WORK * wk )
{
/*
  // �e�X�g
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
    SetSelPosCancel( wk );
    return TRUE;
  }
*/
  if( wk->dat->time_out_flg == TRUE ){
    SetSelPosCancel( wk );
    return TRUE;
  }
  return FALSE;
}

static void PlaySE( BPLIST_WORK * wk, int no )
{
	if( wk->dat->commFlag == FALSE ){
		PMSND_PlaySE( no );
	}
}
