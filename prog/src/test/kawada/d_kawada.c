//============================================================================================
/**
 * @file	  d_kawada.c
 * @brief		��c�f�o�b�O����
 * @author  Koji Kawada
 * @date	  2010.03.03
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "system/wipe.h"
#include "system/bmp_menulist.h"
#include "gamesystem/game_data.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "item/item.h"
#include "font/font.naix"

#include "app/box2.h"
#include "app/zukan.h"
#include "app/wifi_note.h"
#include "demo/command_demo.h"
#include "../../battle/app/vs_multi_list.h"

#include "arc_def.h"  //ARCID_MESSAGE
#include "message.naix"


#include "msg/msg_d_kawada.h"

// �}�ӓo�^
#include "app/zukan_toroku.h"

// �g���C�A���n�E�X����
#include "pm_version.h"
#include "app/th_award.h"
#include "savedata/trialhouse_save.h"
#include "../../savedata/trialhouse_save_local.h"

// �n���}�ӏ܏�
#include "savedata/mystatus.h"
#include "app/chihou_zukan_award.h"

// �S���}�ӏ܏�
#include "app/zenkoku_zukan_award.h"

// �ʐM�ΐ��̘^��I�����
#include "net_app/btl_rec_sel.h"

// ����ȈՉ�b
// ����ȈՉ�b
// ��^���ȈՉ�b
#include "app/pms_input.h"

// �O�C�I��
#include "demo/psel.h"

// �n���S�H���}
#include "app/subway_map.h"

// �^�}�S�z���f��
#include "field/zonedata.h"
#include "demo/egg_demo.h"

// �i���f��
#include "poke_tool/shinka_check.h"
#include "demo/shinka_demo.h"

// �}�ӏڍ�
#include "../../app/zukan/detail/zukan_detail.h"
#include "../../../../resource/fldmapdata/flagwork/flag_define.h"

// �Q�[�����}�j���A��
#include "app/manual.h"

// �e�X�g
#include "d_test.h"


// �I�[�o�[���C
FS_EXTERN_OVERLAY(zukan_toroku);
FS_EXTERN_OVERLAY(th_award);
FS_EXTERN_OVERLAY(chihou_zukan_award);
FS_EXTERN_OVERLAY(zenkoku_zukan_award);
FS_EXTERN_OVERLAY(btl_rec_sel);
FS_EXTERN_OVERLAY(pmsinput);
FS_EXTERN_OVERLAY(psel);
FS_EXTERN_OVERLAY(subway_map);
FS_EXTERN_OVERLAY(egg_demo);
FS_EXTERN_OVERLAY(shinka_demo);
FS_EXTERN_OVERLAY(zukan_detail);
FS_EXTERN_OVERLAY(manual);


//============================================================================================
//	�萔��`
//============================================================================================
#define	TOP_MENU_SIZ	( 15 )

#define POKE_LIST_NUM  (11)


typedef struct {
	u32	main_seq;
	u32	next_seq;
	HEAPID	heapID;

	GFL_FONT * font;					// �ʏ�t�H���g
	GFL_MSGDATA * mman;				// ���b�Z�[�W�f�[�^�}�l�[�W��
	PRINT_QUE * que;					// �v�����g�L���[
	GFL_BMPWIN * win;					// BMPWIN
	PRINT_UTIL	util;

	BMP_MENULIST_DATA * ld;
	BMPMENULIST_WORK * lw;

	GAMEDATA * gamedata;

  GFL_PROCSYS*  local_procsys;

  u32 yoin_count;     // �]�C
  u32 yoin_next_seq;  // �]�C���I�������̃V�[�P���X

  // �}�ӓo�^
  ZUKAN_TOROKU_PARAM* zukan_toroku_param;
  POKEMON_PARAM*      pp;

  // �g���C�A���n�E�X����
  THSV_WORK*          thsv;
  TH_AWARD_PARAM*     th_award_param;

  // �n���}�ӏ܏�
  MYSTATUS*                    mystatus;
  CHIHOU_ZUKAN_AWARD_PARAM*    chihou_zukan_award_param;
  // �S���}�ӏ܏�
  ZENKOKU_ZUKAN_AWARD_PARAM*   zenkoku_zukan_award_param;

  // �ʐM�ΐ��̘^��I�����
  BTL_REC_SEL_PARAM*     btl_rec_sel_param;

  // ����ȈՉ�b
  // ����ȈՉ�b
  // ��^���ȈՉ�b
  PMSI_PARAM*    pmsi_param;

  // �O�C�I��
  u16                evwk;
  PSEL_PARAM*        psel_param;

  // �n���S�H���}
  SUBWAY_MAP_PARAM*  subway_map_param;

  // �^�}�S�z���f��
  EGG_DEMO_PARAM*    egg_demo_param;

  // �i���f��
  POKEPARTY*         party;
  SHINKA_DEMO_PARAM* shinka_demo_param;

  // �}�ӏڍ�
  ZUKAN_DETAIL_PARAM* zukan_detail_param;
  u16                 poke_list[POKE_LIST_NUM];

  // �e�X�g
  D_KAWADA_TEST_PARAM*  d_test_param;

  // �}�j���A��
  MANUAL_PARAM*       manual_param;

}KAWADA_MAIN_WORK;

enum {
	MAIN_SEQ_INIT = 0,
	MAIN_SEQ_MAIN,
	MAIN_SEQ_FADE_MAIN,
  MAIN_SEQ_YOIN,

  // ��������
	MAIN_SEQ_ZUKAN_TOROKU_CALL,  // top_menu00
	MAIN_SEQ_TH_AWARD_CALL,
	MAIN_SEQ_CHIHOU_ZUKAN_AWARD_CALL,
	MAIN_SEQ_ZENKOKU_ZUKAN_AWARD_CALL,
	MAIN_SEQ_BTL_REC_SEL_CALL,
	MAIN_SEQ_PMS_INPUT_DOUBLE_CALL,
	MAIN_SEQ_PMS_INPUT_SINGLE_CALL,
	MAIN_SEQ_PSEL_CALL,
	MAIN_SEQ_SUBWAY_MAP_CALL,
	MAIN_SEQ_PMS_INPUT_SENTENCE_CALL,
	MAIN_SEQ_EGG_DEMO_CALL,
	MAIN_SEQ_SHINKA_DEMO_CALL,
	MAIN_SEQ_ZUKAN_DETAIL_CALL,
	MAIN_SEQ_D_TEST_CALL,
	MAIN_SEQ_MANUAL_CALL,
  // �����܂�

	MAIN_SEQ_ZUKAN_TOROKU_CALL_RETURN,
	MAIN_SEQ_TH_AWARD_CALL_RETURN,
	MAIN_SEQ_CHIHOU_ZUKAN_AWARD_CALL_RETURN,
	MAIN_SEQ_ZENKOKU_ZUKAN_AWARD_CALL_RETURN,
	MAIN_SEQ_BTL_REC_SEL_CALL_RETURN,
	MAIN_SEQ_PMS_INPUT_DOUBLE_CALL_RETURN,
	MAIN_SEQ_PMS_INPUT_SINGLE_CALL_RETURN,
	MAIN_SEQ_PSEL_CALL_RETURN,
	MAIN_SEQ_SUBWAY_MAP_CALL_RETURN,
	MAIN_SEQ_PMS_INPUT_SENTENCE_CALL_RETURN,
	MAIN_SEQ_EGG_DEMO_CALL_RETURN,
	MAIN_SEQ_SHINKA_DEMO_CALL_RETURN,
	MAIN_SEQ_ZUKAN_DETAIL_CALL_RETURN,
	MAIN_SEQ_D_TEST_CALL_RETURN,
	MAIN_SEQ_MANUAL_CALL_RETURN,
	
  MAIN_SEQ_END,
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT MainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT MainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT MainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static void FadeInSet( KAWADA_MAIN_WORK * wk, u32 next );
static void FadeOutSet( KAWADA_MAIN_WORK * wk, u32 next );

static void YoinSet( KAWADA_MAIN_WORK * wk, u32 count, u32 next );

static void BgInit( KAWADA_MAIN_WORK * wk );
static void BgExit(void);

static void TopMenuInit( KAWADA_MAIN_WORK * wk );
static void TopMenuExit( KAWADA_MAIN_WORK * wk );


// �}�ӓo�^
static void ZukanTorokuInit( KAWADA_MAIN_WORK* wk );
static void ZukanTorokuExit( KAWADA_MAIN_WORK* wk );

// �g���C�A���n�E�X����
static void ThAwardInit( KAWADA_MAIN_WORK* wk );
static void ThAwardExit( KAWADA_MAIN_WORK* wk );

// �n���}�ӏ܏�
static void ChihouZukanAwardInit( KAWADA_MAIN_WORK* wk );
static void ChihouZukanAwardExit( KAWADA_MAIN_WORK* wk );

// �S���}�ӏ܏�
static void ZenkokuZukanAwardInit( KAWADA_MAIN_WORK* wk );
static void ZenkokuZukanAwardExit( KAWADA_MAIN_WORK* wk );

// �ʐM�ΐ��̘^��I�����
static void BtlRecSelInit( KAWADA_MAIN_WORK* wk );
static void BtlRecSelExit( KAWADA_MAIN_WORK* wk );

// ����ȈՉ�b
static void PmsInputDoubleInit( KAWADA_MAIN_WORK* wk );
static void PmsInputDoubleExit( KAWADA_MAIN_WORK* wk );

// ����ȈՉ�b
static void PmsInputSingleInit( KAWADA_MAIN_WORK* wk );
static void PmsInputSingleExit( KAWADA_MAIN_WORK* wk );

// �O�C�I��
static void PselInit( KAWADA_MAIN_WORK* wk );
static void PselExit( KAWADA_MAIN_WORK* wk );

// �n���S�H���}
static void SubwayMapInit( KAWADA_MAIN_WORK* wk );
static void SubwayMapExit( KAWADA_MAIN_WORK* wk );

// ��^���ȈՉ�b
static void PmsInputSentenceInit( KAWADA_MAIN_WORK* wk );
static void PmsInputSentenceExit( KAWADA_MAIN_WORK* wk );

// �^�}�S�z���f��
static void EggDemoInit( KAWADA_MAIN_WORK* wk );
static void EggDemoExit( KAWADA_MAIN_WORK* wk );

// �i���f��
static void ShinkaDemoInit( KAWADA_MAIN_WORK* wk );
static void ShinkaDemoExit( KAWADA_MAIN_WORK* wk );

// �}�ӏڍ�
static void ZukanDetailInit( KAWADA_MAIN_WORK* wk );
static void ZukanDetailExit( KAWADA_MAIN_WORK* wk );

// �e�X�g
static void D_TestInit( KAWADA_MAIN_WORK* wk );
static void D_TestExit( KAWADA_MAIN_WORK* wk );

// �Q�[�����}�j���A��
static void ManualInit( KAWADA_MAIN_WORK* wk );
static void ManualExit( KAWADA_MAIN_WORK* wk );


//============================================================================================
//	�O���[�o��
//============================================================================================
const GFL_PROC_DATA DebugKawadaMainProcData = {
  MainProcInit,
  MainProcMain,
  MainProcEnd,
};

static const BMPMENULIST_HEADER TopMenuListH = {
	NULL, NULL, NULL, NULL,
	TOP_MENU_SIZ,		// ���X�g���ڐ�
	6,		// �\���ő區�ڐ�
	0,		// ���x���\���w���W
	12,		// ���ڕ\���w���W
	0,		// �J�[�\���\���w���W
	0,		// �\���x���W
	1,		// �\�������F
	15,		// �\���w�i�F
	2,		// �\�������e�F
	0,		// �����Ԋu�w
	16,		// �����Ԋu�x
	BMPMENULIST_LRKEY_SKIP,		// �y�[�W�X�L�b�v�^�C�v
	0,		// �����w��(�{����u8�����ǂ���Ȃɍ��Ȃ��Ǝv���̂�)
	0,		// �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)

	NULL,
	
	12,			// �����T�C�YX(�h�b�g
	16,			// �����T�C�YY(�h�b�g
	NULL,		// �\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
	NULL,		// �\���Ɏg�p����v�����g���[�e�B���e�B
	NULL,		// �\���Ɏg�p����v�����g�L���[
	NULL,		// �\���Ɏg�p����t�H���g�n���h��
};


static GFL_PROC_RESULT MainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	KAWADA_MAIN_WORK * wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_KAWADA_DEBUG, 0x20000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(KAWADA_MAIN_WORK), HEAPID_KAWADA_DEBUG );

	wk->heapID    = HEAPID_KAWADA_DEBUG;
	wk->gamedata	= GAMEDATA_Create( wk->heapID );

  wk->local_procsys = GFL_PROC_LOCAL_boot( wk->heapID );

#if 0
  // �t�F�[�h�C������ŃX�^�[�g
	FadeInSet( wk, MAIN_SEQ_INIT );
	wk->main_seq  = MAIN_SEQ_FADE_MAIN;
#else
  // �t�F�[�h�C���Ȃ��ŃX�^�[�g
  wk->main_seq  = MAIN_SEQ_INIT;
#endif

  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT MainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	KAWADA_MAIN_WORK * wk = mywk;

  GFL_PROC_MAIN_STATUS  local_proc_status   =  GFL_PROC_LOCAL_Main( wk->local_procsys );
  if( local_proc_status == GFL_PROC_MAIN_VALID ) return GFL_PROC_RES_CONTINUE;

	switch( wk->main_seq ){
	case MAIN_SEQ_INIT:
		BgInit( wk );
		TopMenuInit( wk );
		wk->main_seq = MAIN_SEQ_MAIN;
		break;

	case MAIN_SEQ_MAIN:
		{
			u32 ret = BmpMenuList_Main( wk->lw );

			switch( ret ){
			case BMPMENULIST_NULL:
			case BMPMENULIST_LABEL:
				break;

			case BMPMENULIST_CANCEL:		// �L�����Z��
				TopMenuExit( wk );
				BgExit();
				wk->main_seq = MAIN_SEQ_END;
				break;

			default:
				TopMenuExit( wk );
				BgExit();
				FadeOutSet( wk, ret );
				wk->main_seq = MAIN_SEQ_FADE_MAIN;
			}
		}
		break;

	case MAIN_SEQ_FADE_MAIN:
		if( WIPE_SYS_EndCheck() == TRUE ){
			wk->main_seq = wk->next_seq;
		}
		break;
  
  case MAIN_SEQ_YOIN:
    if( wk->yoin_count == 0 )
    {
			wk->main_seq = wk->yoin_next_seq;
      if( wk->yoin_next_seq == MAIN_SEQ_FADE_MAIN )
      {
		    FadeInSet( wk, MAIN_SEQ_INIT );
      }
    }
    else
    {
      wk->yoin_count--;
    }
    break;

	case MAIN_SEQ_END:
		OS_Printf( "kawada�f�o�b�O�����I�����܂���\n" );
	  return GFL_PROC_RES_FINISH;




  // �}�ӓo�^
	case MAIN_SEQ_ZUKAN_TOROKU_CALL:
    ZukanTorokuInit(wk);
		wk->main_seq = MAIN_SEQ_ZUKAN_TOROKU_CALL_RETURN;
		break;
  case MAIN_SEQ_ZUKAN_TOROKU_CALL_RETURN:
    ZukanTorokuExit(wk);
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // �g���C�A���n�E�X����
  case MAIN_SEQ_TH_AWARD_CALL:
    ThAwardInit(wk); 
		wk->main_seq = MAIN_SEQ_TH_AWARD_CALL_RETURN;
    break;
  case MAIN_SEQ_TH_AWARD_CALL_RETURN:
    ThAwardExit(wk); 
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // �n���}�ӏ܏�
  case MAIN_SEQ_CHIHOU_ZUKAN_AWARD_CALL:
    ChihouZukanAwardInit(wk);
		wk->main_seq = MAIN_SEQ_CHIHOU_ZUKAN_AWARD_CALL_RETURN;
    break;
  case MAIN_SEQ_CHIHOU_ZUKAN_AWARD_CALL_RETURN:
    ChihouZukanAwardExit(wk); 
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // �S���}�ӏ܏�
  case MAIN_SEQ_ZENKOKU_ZUKAN_AWARD_CALL:
    ZenkokuZukanAwardInit(wk);
		wk->main_seq = MAIN_SEQ_ZENKOKU_ZUKAN_AWARD_CALL_RETURN;
    break;
  case MAIN_SEQ_ZENKOKU_ZUKAN_AWARD_CALL_RETURN:
    ZenkokuZukanAwardExit(wk); 
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // �ʐM�ΐ��̘^��I�����
  case MAIN_SEQ_BTL_REC_SEL_CALL:
    BtlRecSelInit(wk);
		wk->main_seq = MAIN_SEQ_BTL_REC_SEL_CALL_RETURN;
    break;
  case MAIN_SEQ_BTL_REC_SEL_CALL_RETURN:
    BtlRecSelExit(wk); 
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // ����ȈՉ�b
  case MAIN_SEQ_PMS_INPUT_DOUBLE_CALL:
    PmsInputDoubleInit(wk);
		wk->main_seq = MAIN_SEQ_PMS_INPUT_DOUBLE_CALL_RETURN;
    break;
  case MAIN_SEQ_PMS_INPUT_DOUBLE_CALL_RETURN:
    PmsInputDoubleExit(wk); 
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // ����ȈՉ�b
  case MAIN_SEQ_PMS_INPUT_SINGLE_CALL:
    PmsInputSingleInit(wk);
		wk->main_seq = MAIN_SEQ_PMS_INPUT_SINGLE_CALL_RETURN;
    break;
  case MAIN_SEQ_PMS_INPUT_SINGLE_CALL_RETURN:
    PmsInputSingleExit(wk); 
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // �O�C�I��
  case MAIN_SEQ_PSEL_CALL:
    PselInit(wk);
		wk->main_seq = MAIN_SEQ_PSEL_CALL_RETURN;
    break;
  case MAIN_SEQ_PSEL_CALL_RETURN:
    PselExit(wk); 
    YoinSet( wk, 120, MAIN_SEQ_FADE_MAIN );
		wk->main_seq = MAIN_SEQ_YOIN;
    break;
  
    
  // �n���S�H���}
  case MAIN_SEQ_SUBWAY_MAP_CALL:
    SubwayMapInit(wk);
		wk->main_seq = MAIN_SEQ_SUBWAY_MAP_CALL_RETURN;
    break;
  case MAIN_SEQ_SUBWAY_MAP_CALL_RETURN:
    SubwayMapExit(wk); 
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // ��^���ȈՉ�b
  case MAIN_SEQ_PMS_INPUT_SENTENCE_CALL:
    PmsInputSentenceInit(wk);
		wk->main_seq = MAIN_SEQ_PMS_INPUT_SENTENCE_CALL_RETURN;
    break;
  case MAIN_SEQ_PMS_INPUT_SENTENCE_CALL_RETURN:
    PmsInputSentenceExit(wk); 
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // �^�}�S�z���f��
  case MAIN_SEQ_EGG_DEMO_CALL:
    EggDemoInit(wk);
		wk->main_seq = MAIN_SEQ_EGG_DEMO_CALL_RETURN;
    break;
  case MAIN_SEQ_EGG_DEMO_CALL_RETURN:
    EggDemoExit(wk);
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // �i���f��
  case MAIN_SEQ_SHINKA_DEMO_CALL:
    ShinkaDemoInit(wk);
		wk->main_seq = MAIN_SEQ_SHINKA_DEMO_CALL_RETURN;
    break;
  case MAIN_SEQ_SHINKA_DEMO_CALL_RETURN:
    ShinkaDemoExit(wk);
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // �}�ӏڍ�
  case MAIN_SEQ_ZUKAN_DETAIL_CALL:
    ZukanDetailInit(wk);
		wk->main_seq = MAIN_SEQ_ZUKAN_DETAIL_CALL_RETURN;
    break;
  case MAIN_SEQ_ZUKAN_DETAIL_CALL_RETURN:
    ZukanDetailExit(wk);
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // �e�X�g
  case MAIN_SEQ_D_TEST_CALL:
    D_TestInit(wk);
		wk->main_seq = MAIN_SEQ_D_TEST_CALL_RETURN;
    break;
  case MAIN_SEQ_D_TEST_CALL_RETURN:
    D_TestExit(wk);
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


  // �Q�[�����}�j���A��
  case MAIN_SEQ_MANUAL_CALL:
    ManualInit(wk);
		wk->main_seq = MAIN_SEQ_MANUAL_CALL_RETURN;
    break;
  case MAIN_SEQ_MANUAL_CALL_RETURN:
    ManualExit(wk);
		FadeInSet( wk, MAIN_SEQ_INIT );
		wk->main_seq = MAIN_SEQ_FADE_MAIN;
    break;


	}

  return GFL_PROC_RES_CONTINUE;
}

static GFL_PROC_RESULT MainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	KAWADA_MAIN_WORK * wk = mywk;
  
  GFL_PROC_LOCAL_Exit( wk->local_procsys ); 

	GAMEDATA_Delete( wk->gamedata );

	GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_KAWADA_DEBUG );

  return GFL_PROC_RES_FINISH;
}

static void BgInit( KAWADA_MAIN_WORK * wk )
{
	GFL_BG_Init( wk->heapID );
	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}
	{	// ���C����ʁF����
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0, wk->heapID );
	}
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );

	// �t�H���g�p���b�g
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0, 0x20, wk->heapID );
}

static void BgExit(void)
{
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
	GFL_BG_Exit();
}

// �t�F�[�h�C��
static void FadeInSet( KAWADA_MAIN_WORK * wk, u32 next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapID );

	wk->next_seq = next;
}

// �t�F�[�h�A�E�g
static void FadeOutSet( KAWADA_MAIN_WORK * wk, u32 next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapID );

	wk->next_seq = next;
}

// �]�C
static void YoinSet( KAWADA_MAIN_WORK * wk, u32 count, u32 next )
{
  wk->yoin_count = count;
  wk->yoin_next_seq = next;
}

static void TopMenuInit( KAWADA_MAIN_WORK * wk )
{
	BMPMENULIST_HEADER	lh;
	u32	i;

	GFL_BMPWIN_Init( wk->heapID );

	wk->mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_kawada_dat, wk->heapID );
  wk->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );
	wk->que  = PRINTSYS_QUE_Create( wk->heapID );
	wk->win  = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 1, 1, 16, 12, 0, GFL_BMP_CHRAREA_GET_B );

	wk->ld = BmpMenuWork_ListCreate( TOP_MENU_SIZ, wk->heapID );
	for( i=0; i<TOP_MENU_SIZ; i++ ){
		STRBUF * str = GFL_MSG_CreateString( wk->mman, top_menu00+i );
		BmpMenuWork_ListAddString( &wk->ld[i], str, MAIN_SEQ_ZUKAN_TOROKU_CALL+i, wk->heapID );
		GFL_STR_DeleteBuffer( str );
	}

	lh = TopMenuListH;
	lh.list = wk->ld;
	lh.win  = wk->win;
	lh.msgdata = wk->mman;			//�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
	lh.print_util = &wk->util;	//�\���Ɏg�p����v�����g���[�e�B���e�B
	lh.print_que  = wk->que;		//�\���Ɏg�p����v�����g�L���[
	lh.font_handle = wk->font;	//�\���Ɏg�p����t�H���g�n���h��

	wk->lw = BmpMenuList_Set( &lh, 0, 0, wk->heapID );
	BmpMenuList_SetCursorBmp( wk->lw, wk->heapID );

	GFL_BMPWIN_TransVramCharacter( wk->win );
	GFL_BMPWIN_MakeScreen( wk->win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(wk->win) );
}

static void TopMenuExit( KAWADA_MAIN_WORK * wk )
{
	BmpMenuList_Exit( wk->lw, NULL, NULL );
	BmpMenuWork_ListDelete( wk->ld );

	GFL_BMPWIN_Delete( wk->win );
	PRINTSYS_QUE_Delete( wk->que );
	GFL_FONT_Delete( wk->font );
	GFL_MSG_Delete( wk->mman );

	GFL_BMPWIN_Exit();
}

// �}�ӓo�^
static void ZukanTorokuInit( KAWADA_MAIN_WORK* wk )
{
  ZONEDATA_Open( wk->heapID );

    //GFL_OVERLAY_Load(FS_OVERLAY_ID(zukan_toroku));
    wk->pp = PP_Create( 1, 1, 0, wk->heapID );
/*
    wk->zukan_toroku_param = ZUKAN_TOROKU_AllocParam(
        wk->heapID,
        ZUKAN_TOROKU_LAUNCH_TOROKU,
        wk->pp,
        FALSE,
        NULL,
        NULL,
        0 );
    wk->zukan_toroku_param->gamedata = wk->gamedata;
*/

    wk->zukan_toroku_param = GFL_HEAP_AllocClearMemory( wk->heapID, sizeof(ZUKAN_TOROKU_PARAM) );
    wk->zukan_toroku_param->launch           = ZUKAN_TOROKU_LAUNCH_TOROKU;
    wk->zukan_toroku_param->pp               = wk->pp;
    wk->zukan_toroku_param->b_zenkoku_flag   = FALSE;
    wk->zukan_toroku_param->box_strbuf       = NULL;
    wk->zukan_toroku_param->box_manager      = NULL;
    wk->zukan_toroku_param->box_tray         = 0;
    wk->zukan_toroku_param->gamedata         = wk->gamedata;
    
    //GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &ZUKAN_TOROKU_ProcData, wk->zukan_toroku_param );
    GFL_PROC_LOCAL_CallProc( wk->local_procsys, FS_OVERLAY_ID(zukan_toroku), &ZUKAN_TOROKU_ProcData, wk->zukan_toroku_param );
}
static void ZukanTorokuExit( KAWADA_MAIN_WORK* wk )
{
    GFL_HEAP_FreeMemory( wk->pp );
    //ZUKAN_TOROKU_FreeParam( wk->zukan_toroku_param );
    
    GFL_HEAP_FreeMemory( wk->zukan_toroku_param );
    
    //GFL_OVERLAY_Unload(FS_OVERLAY_ID(zukan_toroku));

  ZONEDATA_Close();
}

// �g���C�A���n�E�X����
static void ThAwardInit( KAWADA_MAIN_WORK* wk )
{
  SAVE_CONTROL_WORK*  sv  = GAMEDATA_GetSaveControlWork( wk->gamedata );
  CONFIG*             cf  = SaveData_GetConfig( sv );

      u8 sex = PM_MALE;
      u8 type = 0;  // 0=���[�J���V���O��; 1=���[�J���_�u��; 2=�_�E�����[�h�V���O��; 3=�_�E�����[�h�_�u��;
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ) sex = PM_FEMALE;
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ) type = 1;
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ) type = 2;
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y ) type = 3;

  CONFIG_SetMojiMode( cf, MOJIMODE_HIRAGANA );
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT ) CONFIG_SetMojiMode( cf, MOJIMODE_KANJI );

      GFL_OVERLAY_Load(FS_OVERLAY_ID(th_award));
      wk->thsv = GFL_HEAP_AllocMemory( wk->heapID, sizeof(THSV_WORK) );
      {
        u8 i;
#if 0 
        u16 name[17] = L"��������������������������������";
        name[16] = 0xffff;  // gflib/src/string/strbuf.c  // EOMCode
#else
        u16 name[17] = L"����������������";
        name[8] = 0xffff;  // gflib/src/string/strbuf.c  // EOMCode
#endif

        wk->thsv->CommonData[0].Valid              = 1;
        wk->thsv->CommonData[0].IsDouble           = 0;
        wk->thsv->CommonData[0].Point              = 6000;
        wk->thsv->CommonData[0].MonsData[0].MonsNo = 1;
        wk->thsv->CommonData[0].MonsData[0].FormNo = 0;
        wk->thsv->CommonData[0].MonsData[0].Sex    = 0;
        wk->thsv->CommonData[0].MonsData[1].MonsNo = 2;
        wk->thsv->CommonData[0].MonsData[1].FormNo = 0;
        wk->thsv->CommonData[0].MonsData[1].Sex    = 0;
        wk->thsv->CommonData[0].MonsData[2].MonsNo = 3;
        wk->thsv->CommonData[0].MonsData[2].FormNo = 0;
        wk->thsv->CommonData[0].MonsData[2].Sex    = 0;
        wk->thsv->CommonData[0].MonsData[3].MonsNo = 6;
        wk->thsv->CommonData[0].MonsData[3].FormNo = 0;
        wk->thsv->CommonData[0].MonsData[3].Sex    = 0;

        wk->thsv->CommonData[1].Valid              = 1;
        wk->thsv->CommonData[1].IsDouble           = 0;
        wk->thsv->CommonData[1].Point              = 0xFFFF;
        wk->thsv->CommonData[1].MonsData[0].MonsNo = 1;
        wk->thsv->CommonData[1].MonsData[0].FormNo = 0;
        wk->thsv->CommonData[1].MonsData[0].Sex    = 0;
        wk->thsv->CommonData[1].MonsData[1].MonsNo = 2;
        wk->thsv->CommonData[1].MonsData[1].FormNo = 0;
        wk->thsv->CommonData[1].MonsData[1].Sex    = 0;
        wk->thsv->CommonData[1].MonsData[2].MonsNo = 3;
        wk->thsv->CommonData[1].MonsData[2].FormNo = 0;
        wk->thsv->CommonData[1].MonsData[2].Sex    = 0;
        wk->thsv->CommonData[1].MonsData[3].MonsNo = 6;
        wk->thsv->CommonData[1].MonsData[3].FormNo = 0;
        wk->thsv->CommonData[1].MonsData[3].Sex    = 0;

        for( i=0; i<16; i++ ) wk->thsv->DownloadBits[i] = 0;
        GFL_STD_MemCopy( name, wk->thsv->Name, 34 );

        switch(type)
        {
        case 0:
          {
          }
          break;
        case 1:
          {
            wk->thsv->CommonData[0].IsDouble           = 1;
          }
          break;
        case 2:
          {
          }
          break;
        case 3:
          {
            wk->thsv->CommonData[1].IsDouble           = 1;
          }
          break;
        }
      }
      wk->th_award_param = TH_AWARD_AllocParam( wk->heapID, sex, wk->thsv, (type==2||type==3)?TRUE:FALSE );
      GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &TH_AWARD_ProcData, wk->th_award_param );
}
static void ThAwardExit( KAWADA_MAIN_WORK* wk )
{
      TH_AWARD_FreeParam( wk->th_award_param );
      GFL_HEAP_FreeMemory( wk->thsv );
      GFL_OVERLAY_Unload(FS_OVERLAY_ID(th_award));
}

// �n���}�ӏ܏�
static void ChihouZukanAwardInit( KAWADA_MAIN_WORK* wk )
{
  SAVE_CONTROL_WORK*  sv  = GAMEDATA_GetSaveControlWork( wk->gamedata );
  CONFIG*             cf  = SaveData_GetConfig( sv );
  BOOL b_fix = TRUE;

  u8 i;
  u16 name[6] = L"�S���W�m�R";
  name[5] = 0xffff;  // gflib/src/string/strbuf.c  // EOMCode

  GFL_OVERLAY_Load(FS_OVERLAY_ID(chihou_zukan_award));

  CONFIG_SetMojiMode( cf, MOJIMODE_HIRAGANA );
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ) CONFIG_SetMojiMode( cf, MOJIMODE_KANJI );
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ) b_fix = FALSE;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X )
  {
    u16 min_name[2] = L"1";
    min_name[1] = 0xffff;  // gflib/src/string/strbuf.c  // EOMCode
    for( i=0; i<2; i++ )
    {
      name[i] = min_name[i];
    }
  }

  wk->mystatus = GAMEDATA_GetMyStatus( wk->gamedata );
  
  for( i=0; i<6; i++ )
  {
    wk->mystatus->name[i] = name[i];
  }
  wk->mystatus->sex = PM_MALE;

  wk->chihou_zukan_award_param = CHIHOU_ZUKAN_AWARD_AllocParam( wk->heapID, wk->gamedata, b_fix );
  
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &CHIHOU_ZUKAN_AWARD_ProcData, wk->chihou_zukan_award_param );
}
static void ChihouZukanAwardExit( KAWADA_MAIN_WORK* wk )
{
  CHIHOU_ZUKAN_AWARD_FreeParam( wk->chihou_zukan_award_param );
  GFL_OVERLAY_Unload(FS_OVERLAY_ID(chihou_zukan_award));
}

// �S���}�ӏ܏�
static void ZenkokuZukanAwardInit( KAWADA_MAIN_WORK* wk )
{
  SAVE_CONTROL_WORK*  sv  = GAMEDATA_GetSaveControlWork( wk->gamedata );
  CONFIG*             cf  = SaveData_GetConfig( sv );
  BOOL b_fix = TRUE;

  u8 i;
  u16 name[6] = L"�S���W�m�R";
  name[5] = 0xffff;  // gflib/src/string/strbuf.c  // EOMCode

  GFL_OVERLAY_Load(FS_OVERLAY_ID(zenkoku_zukan_award));

  CONFIG_SetMojiMode( cf, MOJIMODE_HIRAGANA );
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ) CONFIG_SetMojiMode( cf, MOJIMODE_KANJI );
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ) b_fix = FALSE;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X )
  {
    u16 min_name[2] = L"1";
    min_name[1] = 0xffff;  // gflib/src/string/strbuf.c  // EOMCode
    for( i=0; i<2; i++ )
    {
      name[i] = min_name[i];
    }
  }

  wk->mystatus = GAMEDATA_GetMyStatus( wk->gamedata );
  
  for( i=0; i<6; i++ )
  {
    wk->mystatus->name[i] = name[i];
  }
  wk->mystatus->sex = PM_FEMALE;

  wk->zenkoku_zukan_award_param = ZENKOKU_ZUKAN_AWARD_AllocParam( wk->heapID, wk->gamedata, b_fix );
  
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &ZENKOKU_ZUKAN_AWARD_ProcData, wk->zenkoku_zukan_award_param );
}
static void ZenkokuZukanAwardExit( KAWADA_MAIN_WORK* wk )
{
  ZENKOKU_ZUKAN_AWARD_FreeParam( wk->zenkoku_zukan_award_param );
  GFL_OVERLAY_Unload(FS_OVERLAY_ID(zenkoku_zukan_award));
}

// �ʐM�ΐ��̘^��I�����
static void BtlRecSelInit( KAWADA_MAIN_WORK* wk )
{
  BOOL        b_rec       = TRUE;
  BOOL        b_sync      = TRUE;
  MYSTATUS*   mystatus    = GAMEDATA_GetMyStatus( wk->gamedata );
  MYITEM_PTR  myitem_ptr  = GAMEDATA_GetMyItem( wk->gamedata );
  if( MYITEM_CheckItem( myitem_ptr, ITEM_BATORUREKOODAA, 1, wk->heapID ) )
  {
    MYITEM_SubItem( myitem_ptr, ITEM_BATORUREKOODAA, 1, wk->heapID );
  }

  GFL_OVERLAY_Load(FS_OVERLAY_ID(btl_rec_sel));

  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ) b_rec = FALSE;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y ) b_sync = FALSE;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    mystatus->sex = 0;
  }
  else
  {
    mystatus->sex = 1;
  }
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X )
  {
    MYITEM_AddItem( myitem_ptr, ITEM_BATORUREKOODAA, 1, wk->heapID );
  }

  wk->btl_rec_sel_param = BTL_REC_SEL_AllocParam( wk->heapID, wk->gamedata, b_rec, b_sync,0,0 );
  
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &BTL_REC_SEL_ProcData, wk->btl_rec_sel_param );
}
static void BtlRecSelExit( KAWADA_MAIN_WORK* wk )
{
  BTL_REC_SEL_FreeParam( wk->btl_rec_sel_param );
  GFL_OVERLAY_Unload(FS_OVERLAY_ID(btl_rec_sel));
}

// ����ȈՉ�b
static void PmsInputDoubleInit( KAWADA_MAIN_WORK* wk )
{
  SAVE_CONTROL_WORK*  sv  = GAMEDATA_GetSaveControlWork( wk->gamedata );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(pmsinput) );
  wk->pmsi_param = PMSI_PARAM_Create( PMSI_MODE_DOUBLE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, sv, wk->heapID );
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &ProcData_PMSInput, wk->pmsi_param );
}
static void PmsInputDoubleExit( KAWADA_MAIN_WORK* wk )
{
  if( PMSI_PARAM_CheckModified( wk->pmsi_param ) )
  { 
    //PMS_DATA pms;
    //PMSI_PARAM_GetInputDataSentence( wk->pmsi_param, &pms );
    //OS_Printf( "%d, %d\n", pms.word[0], pms.word[1] );
    PMS_WORD word[2];
    PMSI_PARAM_GetInputDataDouble(  wk->pmsi_param, word );
    OS_Printf( "double: %d, %d\n", word[0], word[1] );
  }
  else
  {
    OS_Printf( "double: not modified\n" );
  }
  PMSI_PARAM_Delete( wk->pmsi_param );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(pmsinput) );
}

// ����ȈՉ�b
static void PmsInputSingleInit( KAWADA_MAIN_WORK* wk )
{
  SAVE_CONTROL_WORK*  sv  = GAMEDATA_GetSaveControlWork( wk->gamedata );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(pmsinput) );
  wk->pmsi_param = PMSI_PARAM_Create( PMSI_MODE_SINGLE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, sv, wk->heapID );
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &ProcData_PMSInput, wk->pmsi_param );
}
static void PmsInputSingleExit( KAWADA_MAIN_WORK* wk )
{
  if( PMSI_PARAM_CheckModified( wk->pmsi_param ) )
  { 
    PMS_WORD word = PMSI_PARAM_GetInputDataSingle( wk->pmsi_param );
    OS_Printf( "single: %d\n", word );
  }
  else
  {
    OS_Printf( "single: not modified\n" );
  }
  PMSI_PARAM_Delete( wk->pmsi_param );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(pmsinput) );
}

// �O�C�I��
static void PselInit( KAWADA_MAIN_WORK* wk )
{
  GFL_OVERLAY_Load( FS_OVERLAY_ID(psel) );
  wk->psel_param = PSEL_AllocParam( wk->heapID, &(wk->evwk) );
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &PSEL_ProcData, wk->psel_param );
}
static void PselExit( KAWADA_MAIN_WORK* wk )
{
  OS_Printf( "DebugKawada: evwk=%d\n", wk->evwk );
  PSEL_FreeParam( wk->psel_param );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(psel) );
}

// �n���S�H���}
static void SubwayMapInit( KAWADA_MAIN_WORK* wk )
{
  GFL_OVERLAY_Load(FS_OVERLAY_ID(subway_map));

  wk->mystatus = GFL_HEAP_AllocMemory( wk->heapID, MYSTATUS_SAVE_SIZE );
  wk->mystatus->sex = PM_MALE;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ) wk->mystatus->sex = PM_FEMALE;

  wk->subway_map_param = SUBWAY_MAP_AllocParam( wk->heapID, wk->mystatus );
  
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &SUBWAY_MAP_ProcData, wk->subway_map_param );
}
static void SubwayMapExit( KAWADA_MAIN_WORK* wk )
{
  SUBWAY_MAP_FreeParam( wk->subway_map_param );
  GFL_HEAP_FreeMemory( wk->mystatus );
  GFL_OVERLAY_Unload(FS_OVERLAY_ID(subway_map));
}

// ��^���ȈՉ�b
static void PmsInputSentenceInit( KAWADA_MAIN_WORK* wk )
{
                                              // ��^��  ��������  �f�R����
#define SENTENCE_CHANGE_NOSET_PICTURE    (0)  // �ύX��  �Ȃ�      ����
#define SENTENCE_CHANGE_SET_PICTURE      (1)  // �ύX��  ����      ����
#define SENTENCE_NOCHANGE_NOSET_PICTURE  (2)  // �Œ�    �Ȃ�      ����
#define SENTENCE_NOCHANGE_SET_PICTURE    (3)  // �Œ�    ����      ����
#define SENTENCE_CHANGE_NOSET_NOPICTURE  (4)  // �Œ�    �Ȃ�      �Ȃ�

  u8 sentence_mode = SENTENCE_CHANGE_NOSET_PICTURE;

  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )  sentence_mode = SENTENCE_CHANGE_SET_PICTURE;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )  sentence_mode = SENTENCE_NOCHANGE_NOSET_PICTURE;
  if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )  sentence_mode = SENTENCE_NOCHANGE_SET_PICTURE;
  if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT ) sentence_mode = SENTENCE_CHANGE_NOSET_NOPICTURE;

  switch( sentence_mode )
  {
  case SENTENCE_CHANGE_NOSET_PICTURE:
    {
    }
    break;
  case SENTENCE_CHANGE_SET_PICTURE:
    {
    }
    break;
  case SENTENCE_NOCHANGE_NOSET_PICTURE:
    {
    }
    break;
  case SENTENCE_NOCHANGE_SET_PICTURE:
    {
    }
    break;
  case SENTENCE_CHANGE_NOSET_NOPICTURE:
    {
    }
    break;
  }
}
static void PmsInputSentenceExit( KAWADA_MAIN_WORK* wk )
{
}

// �^�}�S�z���f��
static void EggDemoInit( KAWADA_MAIN_WORK* wk )
{
  u16 monsno = MONSNO_KAMEKKUSU;

  ZONEDATA_Open( wk->heapID );

  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )  monsno = MONSNO_MANAFI;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )  monsno = MONSNO_PIZYOTTO;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X )  monsno = MONSNO_PIZYON;

  wk->pp = PP_Create( monsno, 1, 0, wk->heapID );
  PP_Put( wk->pp, ID_PARA_tamago_flag, 1 );
     
  wk->egg_demo_param = GFL_HEAP_AllocMemory( wk->heapID, sizeof(EGG_DEMO_PARAM) );
  wk->egg_demo_param->gamedata  = wk->gamedata;
  wk->egg_demo_param->pp        = wk->pp;
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, FS_OVERLAY_ID(egg_demo), &EGG_DEMO_ProcData, wk->egg_demo_param );
}
static void EggDemoExit( KAWADA_MAIN_WORK* wk )
{
  GFL_HEAP_FreeMemory( wk->egg_demo_param );
  GFL_HEAP_FreeMemory( wk->pp );

  ZONEDATA_Close();
}

// �i���f��
static void ShinkaDemoInit( KAWADA_MAIN_WORK* wk )
{
  BOOL b_field = TRUE;

  u16 monsno_before;
  u16 monsno_after;

  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    monsno_before = MONSNO_650;
    monsno_after  = MONSNO_651;
  }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    monsno_before = MONSNO_557;
    monsno_after  = MONSNO_558;
  }
  else
  {
    monsno_before = MONSNO_PIZYON;//MONSNO_KAMEERU;
    monsno_after  = MONSNO_PIZYOTTO;//MONSNO_RIZAADON;
  }

  ZONEDATA_Open( wk->heapID );
  GAMEBEACON_Setting(wk->gamedata);

  wk->pp = PP_Create( monsno_before, 98, 0, wk->heapID );
  
  wk->party = PokeParty_AllocPartyWork(wk->heapID);
  PokeParty_InitWork(wk->party);
  PokeParty_Init(wk->party, 6);
  PokeParty_Add( wk->party, wk->pp );
 
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ) b_field = FALSE;

  {
    SHINKA_DEMO_PARAM* sdp = GFL_HEAP_AllocMemory( wk->heapID, sizeof( SHINKA_DEMO_PARAM ) );
    sdp->gamedata          = wk->gamedata;
    sdp->ppt               = wk->party;
    sdp->after_mons_no     = monsno_after;
    sdp->shinka_pos        = 0;
    sdp->shinka_cond       = SHINKA_COND_LEVELUP;
    sdp->b_field           = b_field;
    sdp->b_enable_cancel   = TRUE;
    wk->shinka_demo_param  = sdp;
  }
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, wk->shinka_demo_param );
}
static void ShinkaDemoExit( KAWADA_MAIN_WORK* wk )
{
  GFL_HEAP_FreeMemory( wk->shinka_demo_param );
  GFL_HEAP_FreeMemory( wk->party );
  GFL_HEAP_FreeMemory( wk->pp );
  ZONEDATA_Close();
}

// �}�ӏڍ�
static void ZukanDetailInit( KAWADA_MAIN_WORK* wk )
{
  u16 poke_list_num = POKE_LIST_NUM;
  const u8 lang_code[6] =
  {
    LANG_ENGLISH,
    LANG_FRANCE,
    LANG_GERMANY,
    LANG_ITALY,
    LANG_SPAIN, 
    LANG_KOREA, 
  };
  ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( wk->gamedata );
  u16 i;
  u16 lc;
  EVENTWORK* eventwork = GAMEDATA_GetEventWork( wk->gamedata );
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ) EVENTWORK_SetEventFlag( eventwork, SYS_FLAG_ARRIVE_T01 );

  ZONEDATA_Open( wk->heapID );
  
  ZUKANSAVE_SetGraphicVersionUpFlag( zukan_savedata );
 
  wk->poke_list[ 0] = 201;
  wk->poke_list[ 1] = 202;
  wk->poke_list[ 2] = 151;
  wk->poke_list[ 3] = 487;
  wk->poke_list[ 4] =  17;
  wk->poke_list[ 5] =  18;
  wk->poke_list[ 6] = 582;  // MONSNO_TURURI
  wk->poke_list[ 7] = 583;  // MONSNO_TURUTTO
  wk->poke_list[ 8] = 584;  // MONSNO_TURUTURUDA
  wk->poke_list[ 9] = 585;  // MONSNO_SIKIZIKA
  wk->poke_list[10] = 586;  // MONSNO_ANTORESU

  for( i=0; i<poke_list_num; i++ )
  {
    POKEMON_PARAM* pp = PP_Create( wk->poke_list[i] , 1, 0, wk->heapID );
    ZUKANSAVE_SetPokeSee( zukan_savedata, pp );  // ����  // �}�Ӄt���O���Z�b�g����
    ZUKANSAVE_SetPokeGet( zukan_savedata, pp );  // �߂܂���  // �}�Ӄt���O���Z�b�g����
    
    if( wk->poke_list[i] == 201 )
    {
      u16 j;
      for( j=1; j<=6; j++ )
      {
        PP_Put( pp, ID_PARA_form_no, j );
        ZUKANSAVE_SetPokeSee( zukan_savedata, pp );  // ����  // �}�Ӄt���O���Z�b�g����
        ZUKANSAVE_SetPokeGet( zukan_savedata, pp );  // �߂܂���  // �}�Ӄt���O���Z�b�g����
      }
    }
    if( wk->poke_list[i] == 487 )
    {
      u16 j;
      for( j=1; j<=1; j++ )
      {
        PP_Put( pp, ID_PARA_form_no, j );
        ZUKANSAVE_SetPokeSee( zukan_savedata, pp );  // ����  // �}�Ӄt���O���Z�b�g����
        ZUKANSAVE_SetPokeGet( zukan_savedata, pp );  // �߂܂���  // �}�Ӄt���O���Z�b�g����
      }
    }

    for( lc=0; lc<6; lc++ )
    {
      PP_Put( pp, ID_PARA_country_code, lang_code[lc] );
      ZUKANSAVE_SetPokeSee( zukan_savedata, pp );  // ����  // �}�Ӄt���O���Z�b�g����
      ZUKANSAVE_SetPokeGet( zukan_savedata, pp );  // �߂܂���  // �}�Ӄt���O���Z�b�g����
    }

    GFL_HEAP_FreeMemory( pp );
  }

	wk->zukan_detail_param = GFL_HEAP_AllocMemory( wk->heapID, sizeof(ZUKAN_DETAIL_PARAM) );
  wk->zukan_detail_param->gamedata = wk->gamedata;
  wk->zukan_detail_param->type     = ZUKAN_DETAIL_TYPE_INFO;
  wk->zukan_detail_param->list     = wk->poke_list;
  wk->zukan_detail_param->num      = poke_list_num;
  wk->zukan_detail_param->no       = 0;

  GFL_PROC_LOCAL_CallProc( wk->local_procsys, FS_OVERLAY_ID(zukan_detail), &ZUKAN_DETAIL_ProcData, wk->zukan_detail_param );
}
static void ZukanDetailExit( KAWADA_MAIN_WORK* wk )
{
  GFL_HEAP_FreeMemory( wk->zukan_detail_param );
  
  ZONEDATA_Close();
}

// �e�X�g
static void D_TestInit( KAWADA_MAIN_WORK* wk )
{
  wk->d_test_param = D_KAWADA_TEST_AllocParam(
                              wk->heapID,
                              0 );
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, &D_KAWADA_TEST_ProcData, wk->d_test_param );
}
static void D_TestExit( KAWADA_MAIN_WORK* wk )
{
  D_KAWADA_TEST_FreeParam( wk->d_test_param );
}

// �Q�[�����}�j���A��
static void ManualInit( KAWADA_MAIN_WORK* wk )
{
  wk->manual_param            = GFL_HEAP_AllocMemory( wk->heapID, sizeof( MANUAL_PARAM ) );
  wk->manual_param->gamedata  = wk->gamedata;
  
  GFL_PROC_LOCAL_CallProc( wk->local_procsys, FS_OVERLAY_ID(manual), &MANUAL_ProcData, wk->manual_param );
}
static void ManualExit( KAWADA_MAIN_WORK* wk )
{
  GFL_HEAP_FreeMemory( wk->manual_param );
}

