//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery.c
 *	@brief  �ӂ����Ȃ�������̃v���Z�X
 *	@author	Toru=Nagihashi
 *	@data		2009.12.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/main.h"
#include "gamesystem/game_data.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "print/wordset.h"
#include "system/bmp_winframe.h"
#include "system/ds_system.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "arc\mystery.naix"
#include "msg/msg_mystery.h"
#include "item/item.h"
#include "item_icon.naix"
#include "system/poke2dgra.h"
#include "script_message.naix"
#include "msg/script/msg_common_scr.h"

//�Z�[�u�f�[�^
#include "savedata/mystery_data.h"

//�O���v���Z�X
#include "net_app/wifi_login.h"
#include "title/title.h"

//�ӂ����Ȃ�������̓����W���[��
#include "mystery_graphic.h"
#include "mystery_util.h"
#include "mystery_album.h"
#include "mystery_net.h"
#include "mystery_snd.h"
#include "net_app/mystery_data_util.h"

//�f�o�b�O
#include "mystery_debug.h"
#include "debug/debug_str_conv.h"

//�O�����J
#include "net_app/mystery.h"

//-------------------------------------
///	�I�[�o�[���C
//=====================================
FS_EXTERN_OVERLAY(dpw_common);

//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
#endif //PM_DEBUG

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	BG�t���[��
//=====================================
enum
{
	//���C�����
	BG_FRAME_M_LIST 	=	GFL_BG_FRAME0_M, 
	BG_FRAME_M_TEXT	  =	GFL_BG_FRAME1_M, 
	BG_FRAME_M_BACK1	=	GFL_BG_FRAME2_M, 
	BG_FRAME_M_BACK2	=	GFL_BG_FRAME3_M, 

	//�T�u���
	BG_FRAME_S_TEXT	  =	GFL_BG_FRAME0_S, 
	BG_FRAME_S_BACK1	=	GFL_BG_FRAME2_S, 
	BG_FRAME_S_BACK2	=	GFL_BG_FRAME3_S, 
} ;

//-------------------------------------
///	�p���b�g
//=====================================
enum
{
	//���C�����BG
	PLT_BG_BACK_M	=	0,
  PLT_BG_RECV_M = 1,  //6�{
  PLT_BG_CARD_M = 8,
  PLT_BG_LIST_M = 13,
  PLT_BG_TEXT_M = 14,
	PLT_BG_FONT_M = 15,

	//�T�u���BG
	PLT_BG_BACK_S	= 0,
  PLT_BG_CARD_S  = 1,
	PLT_BG_FONT_S	= 15,

	//���C�����OBJ
	PLT_OBJ_CURSOR_M  = 0,
  PLT_OBJ_GIFT_M    = 14,
  PLT_OBJ_CARD_ICON_M       = 10,
  PLT_OBJ_CARD_SILHOUETTE_M = 13,

	//�T�u���OBJ
} ;

//-------------------------------------
///	OBJ_WORK
//=====================================
enum
{ 
  OBJ_RES_PLT_CURSOR  = 0, 
  OBJ_RES_PLT_MAX,

  OBJ_RES_CGX_CURSOR  = 0,
  OBJ_RES_CGX_MAX,

  OBJ_RES_CEL_CURSOR  = 0,
  OBJ_RES_CEL_MAX,
};

#define EFFECT_BRIGHT_MAX  (32)
enum
{
	OBJ_CLWKID_CURSOR,
  OBJ_CLWKID_BRIGHT_TOP,
  OBJ_CLWKID_BRIGHT_END = OBJ_CLWKID_BRIGHT_TOP + EFFECT_BRIGHT_MAX,

  OBJ_CLWKID_BTN,
	OBJ_CLWKID_MAX,
} ;

//-------------------------------------
///	�L����
//=====================================
enum
{ 
  //���C�����  BG_FRAME_M_TEXT
  BG_CGX_OFS_M_CLEAR = 0,
  BG_CGX_OFS_M_LIST  = 1,
  BG_CGX_OFS_M_TEXT  = 10,
};

//-------------------------------------
///	BG�ǂݍ���
//=====================================
typedef enum
{
  BG_LOAD_TYPE_INIT,  //�����O��
  BG_LOAD_TYPE_GUIDE_S, //����ʃK�C�hBG
  BG_LOAD_TYPE_STAGE_M, //�f���p�X�e�[�W
} BG_LOAD_TYPE;


//-------------------------------------
///	�ʐM���@
//=====================================
typedef enum
{
  MYSTERY_NET_MODE_WIRELESS,
  MYSTERY_NET_MODE_WIFI,
  MYSTERY_NET_MODE_IRC,
} MYSTERY_NET_MODE;

//-------------------------------------
///	�f��
//=====================================
#define MYSTERY_DEMO_GIFT_X             (128)
#define MYSTERY_DEMO_MOVE_GIFT_START_Y  (-96)
#define MYSTERY_DEMO_MOVE_GIFT_END_Y    (100)
#define MYSTERY_DEMO_MOVE_GIFT_DIFF_Y   (MYSTERY_DEMO_MOVE_GIFT_END_Y-MYSTERY_DEMO_MOVE_GIFT_START_Y)
#define MYSTERY_DEMO_MOVE_GIFT_SYNC     (60)
#define MYSTERY_DEMO_INIT_WAIT_SYNC     (180)
#define MYSTERY_DEMO_END_WAIT_SYNC      (60)
#define MYSTERY_DEMO_STAGE_FADE_SYNC    (30)

//-------------------------------------
///	���o
//=====================================
#define MYSTERY_EFFECT_DEFAULT_SCROLL_SPEED     (0)
#define MYSTERY_EFFECT_DEMO_SCROLL_SPEED        (-FX32_CONST(4.0))
#define MYSTERY_EFFECT_DEMO_SCROLL_CHANGE_SYNC  (60)

//���o�^�C�v
typedef enum
{ 
  MYSTERY_EFFECT_TYPE_NORMAL,
  MYSTERY_EFFECT_TYPE_DEMO,
} MYSTERY_EFFECT_TYPE;


#define MYSTERY_MENU_ALPHA_EV1  (15)
#define MYSTERY_MENU_ALPHA_EV2  (3)


#define MYSTERY_RECV_TIMEOUT    (60*60)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	��M�f�����[�N
//=====================================
typedef struct 
{
  GFL_CLWK  *p_clwk;
  BOOL      is_end;
  u32       res_plt;
  u32       res_cgx;
  u32       res_cel;
  u32       sync;
  u32       seq;
} MYSTERY_DEMO_WORK;

//-------------------------------------
///	BG���\�[�X�Ǘ�
//=====================================
typedef struct 
{
  HEAPID  heapID;
} BG_WORK;
//-------------------------------------
///	OBJ���\�[�X�Ǘ�
//=====================================
typedef struct 
{
  u32       res_plt[OBJ_RES_PLT_MAX]; //���\�[�X�p���b�g
  u32       res_cgx[OBJ_RES_CGX_MAX]; //���\�[�X�L�����N�^�[
  u32       res_cel[OBJ_RES_CEL_MAX]; //���\�[�X�Z��
  GFL_CLWK  *p_clwk[OBJ_CLWKID_MAX];  //CLWK
} OBJ_WORK;

//-------------------------------------
///	�{�^��
//=====================================
typedef struct 
{
  BMPOAM_SYS_PTR      p_bmpoam_sys;
  MYSTERY_MSGOAM_WORK *p_msgoam;
  const OBJ_WORK      *cp_obj;
} MYSTERY_BTN_WORK;

//-------------------------------------
///	�G�t�F�N�g���[�N
//=====================================
typedef struct 
{
  BOOL is_start;
  BOOL is_update;
  MYSTERY_EFFECT_TYPE type;
  u16   cnt;
  fx32  scroll_now;
  fx32  scroll_add;
  fx32  init_add;
  fx32  bright_pos[EFFECT_BRIGHT_MAX];
  fx32  bright_add[EFFECT_BRIGHT_MAX];
  const OBJ_WORK *cp_obj;
} MYSTERY_EFFECT_WORK;

//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct 
{ 
  //�J�[�h���
  MYSTERY_CARD_WORK         *p_card;

  //�A���o��
  MYSTERY_ALBUM_WORK        *p_album;

  //�f��
  MYSTERY_DEMO_WORK         demo;

  //���o
  MYSTERY_EFFECT_WORK       effect;
    
  //�I����
  MYSTERY_LIST_WORK         *p_list;

  MYSTERY_BTN_WORK          btn;

  //���j���[
  MYSTERY_MENU_WORK         *p_menu;

  //�e�L�X�g
  MYSTERY_TEXT_WORK         *p_text;

  //���C����ʗpBMPWIN�Z�b�g
  MYSTERY_MSGWINSET_WORK    *p_winset_m;

  //�T�u��ʗpBMPWIN�Z�b�g
  MYSTERY_MSGWINSET_WORK    *p_winset_s;

  //�V�[�P���X�Ǘ�
  MYSTERY_SEQ_WORK          *p_seq;

	//OBJ���\�[�X
	OBJ_WORK		            	obj;

	//BG���\�[�X
	BG_WORK				            bg;

  //�O���t�B�b�N�ݒ�
  MYSTERY_GRAPHIC_WORK      *p_graphic;

  //���ʂŎg���t�H���g
	GFL_FONT			            *p_font;

	//���ʂŎg���L���[
	PRINT_QUE				          *p_que;

	//���ʂŎg���G���[���b�Z�[�W
	GFL_MSGDATA	          		*p_err_msg;

	//���ʂŎg�����b�Z�[�W
	GFL_MSGDATA	          		*p_msg;

	//���ʂŎg���P��
	WORDSET				          	*p_word;

  //�ʐM���@
  MYSTERY_NET_MODE          mode;

  //�ʐM
  MYSTERY_NET_WORK          *p_net;

  //����
  MYSTERY_PARAM             *p_param;

  //�Q�[���f�[�^
  GAMEDATA                  *p_gamedata;

  //�Z�[�u�f�[�^
  MYSTERY_DATA              *p_sv;

  //���O�C���v���Z�X�ւ̈���
  WIFILOGIN_PARAM           *p_wifilogin_param;

  //�ėp�J�E���^
  u32                       cnt;

  //�擾�f�[�^
  DOWNLOAD_GIFT_DATA        data;
} MYSTERY_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT MYSTERY_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT MYSTERY_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT MYSTERY_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
///	BG���\�[�X
//=====================================
static void BG_Init( BG_WORK *p_wk, HEAPID heapID );
static void BG_Exit( BG_WORK *p_wk );
static void BG_Load( BG_WORK *p_wk, BG_LOAD_TYPE type );
static void BG_UnLoad( BG_WORK *p_wk, BG_LOAD_TYPE type );
//-------------------------------------
///	OBJ���\�[�X
//=====================================
static void OBJ_Init( OBJ_WORK *p_wk, GFL_CLUNIT *p_clunit, HEAPID heapID );
static void OBJ_Exit( OBJ_WORK *p_wk );
static GFL_CLWK *OBJ_GetClwk( const OBJ_WORK *cp_wk, u32 clwkID );
static void OBJ_ReLoad( OBJ_WORK *p_wk, HEAPID heapID );
//-------------------------------------
///	SEQFUNC
//=====================================
static void SEQFUNC_Start( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_FadeIn( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_FadeOut( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_StartSelect( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );  //�J�n�I��
static void SEQFUNC_Infomation( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );   //����
static void SEQFUNC_RecvGift( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );     //���蕨�󂯎��
static void SEQFUNC_Demo( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );         //�󂯎��f��
static void SEQFUNC_CardAlbum( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );    //�A���o��
static void SEQFUNC_DeleteCard( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );   //�J�[�h�P���폜
static void SEQFUNC_WifiLogin( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_DisConnectEnd( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_DisConnectReturn( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_End( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

static void SEQFUNC_RestrictUGC( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_EnableWireless( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	UTIL  Mystery_util�̃��W���[�����p�b�N��������
//=====================================
//�͂��������Ȃǂ̃��X�g
typedef enum
{ 
  UTIL_LIST_TYPE_YESNO,
}UTIL_LIST_TYPE;
static void UTIL_CreateList( MYSTERY_WORK *p_wk, UTIL_LIST_TYPE type, HEAPID heapID );
static void UTIL_DeleteList( MYSTERY_WORK *p_wk );
//���j���[
typedef enum
{ 
  UTIL_MENU_TYPE_TOP,
  UTIL_MENU_TYPE_NETMODE,
  UTIL_MENU_TYPE_GIFT,
  UTIL_MENU_TYPE_INFO,
}UTIL_MENU_TYPE;
static void UTIL_CreateMenu( MYSTERY_WORK *p_wk, UTIL_MENU_TYPE type, HEAPID heapID );
static void UTIL_DeleteMenu( MYSTERY_WORK *p_wk );
//�K�C�h�e�L�X�g
static void UTIL_CreateGuideText( MYSTERY_WORK *p_wk, HEAPID heapID );
static void UTIL_DeleteGuideText( MYSTERY_WORK *p_wk );
//-------------------------------------
///	�f��
//=====================================
static void MYSTERY_DEMO_Init( MYSTERY_DEMO_WORK *p_wk, GFL_CLUNIT *p_unit, const DOWNLOAD_GIFT_DATA *cp_data, GAMEDATA *p_gamedata, HEAPID heapID );
static void MYSTERY_DEMO_Exit( MYSTERY_DEMO_WORK *p_wk );
static void MYSTERY_DEMO_Main( MYSTERY_DEMO_WORK *p_wk, BG_WORK *p_bg );
static BOOL MYSTERY_DEMO_IsEnd( const MYSTERY_DEMO_WORK *cp_wk );
//-------------------------------------
///	���o
//=====================================
static void MYSTERY_EFFECT_Init( MYSTERY_EFFECT_WORK *p_wk, const OBJ_WORK *cp_obj, HEAPID heapID );
static void MYSTERY_EFFECT_Exit( MYSTERY_EFFECT_WORK *p_wk );
static void MYSTERY_EFFECT_Main( MYSTERY_EFFECT_WORK *p_wk );
static void MYSTERY_EFFECT_Start( MYSTERY_EFFECT_WORK *p_wk, MYSTERY_EFFECT_TYPE type );
static void MYSTERY_EFFECT_SetUpdateFlag( MYSTERY_EFFECT_WORK *p_wk, BOOL is_update );
//-------------------------------------
///	�{�^��
//=====================================
static void MYSTERY_BTN_Init( MYSTERY_BTN_WORK *p_wk, const OBJ_WORK *cp_obj, GFL_CLUNIT *p_clunit, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font, HEAPID heapID );
static void MYSTERY_BTN_Exit( MYSTERY_BTN_WORK *p_wk );
static void MYSTERY_BTN_PrintMain( MYSTERY_BTN_WORK *p_wk );

//=============================================================================
/**
 *					�O���f�[�^
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X�f�[�^
//=====================================
const GFL_PROC_DATA MysteryGiftProcData  =
{ 
  MYSTERY_PROC_Init,
  MYSTERY_PROC_Main,
  MYSTERY_PROC_Exit,
};
//=============================================================================
/**
 *          �v���Z�X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ӂ����Ȃ��������	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT MYSTERY_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  MYSTERY_WORK  *p_wk;
  MYSTERY_PARAM *p_param  = p_param_adrs;

  GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));

  //�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MYSTERYGIFT, 0x40000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(MYSTERY_WORK), HEAPID_MYSTERYGIFT );
	GFL_STD_MemClear( p_wk, sizeof(MYSTERY_WORK) );	
  p_wk->p_param = p_param;

  p_wk->p_gamedata  = GAMEDATA_Create( HEAPID_MYSTERYGIFT );

  p_wk->p_sv    = SaveData_GetMysteryData( GAMEDATA_GetSaveControlWork(p_wk->p_gamedata) );

#if 0
#ifdef DEBUG_SET_SAVEDATA
  MYSTERY_DEBUG_SetGiftData( &p_wk->data );
  { 
    int i;
    for( i = 0; i < 1; i++ )
    { 
      MYSTERYDATA_SetCardData( p_wk->p_sv, &p_wk->data.data );
    }
#if 0
    { 
      int i;
      const u8  *cp_data = (const u8*)&p_wk->data;
      NAGI_Printf( "size == %d\n",sizeof(DOWNLOAD_GIFT_DATA) );
      NAGI_Printf( "adrs == 0x%x\n",&p_wk->data );
      NAGI_Printf( "!!!!!!!!!!!!-- binary start --!!!!!!!!!!!!!!!\n\n" );
      for( i = 0; i < sizeof(DOWNLOAD_GIFT_DATA); i++ )
      { 
        NAGI_Printf( "%x", cp_data[i] );
      }
      NAGI_Printf( "\n" );
      NAGI_Printf( "!!!!!!!!!!!!-- binary end --!!!!!!!!!!!!!!!\n\n" );
    }
#endif
  }
#endif 
#endif

  //�O���t�B�b�N�ݒ�
	p_wk->p_graphic	= MYSTERY_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_MYSTERYGIFT );

	//���\�[�X�ǂݍ���
	BG_Init( &p_wk->bg, HEAPID_MYSTERYGIFT );
	{	
		GFL_CLUNIT	*p_clunit	= MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
		OBJ_Init( &p_wk->obj, p_clunit, HEAPID_MYSTERYGIFT );
	}

  //���ʃ��W���[���̏�����
  p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_MYSTERYGIFT );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_MYSTERYGIFT );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_mystery_dat, HEAPID_MYSTERYGIFT );
	p_wk->p_err_msg	= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, NARC_script_message_common_scr_dat, HEAPID_MYSTERYGIFT );
	p_wk->p_word	= WORDSET_Create( HEAPID_MYSTERYGIFT );

  //���W���[���쐬
  p_wk->p_seq = MYSTERY_SEQ_Init( p_wk, SEQFUNC_Start, HEAPID_MYSTERYGIFT );
  p_wk->p_text  = MYSTERY_TEXT_Init( BG_FRAME_M_TEXT, PLT_BG_FONT_M, p_wk->p_que, p_wk->p_font, HEAPID_MYSTERYGIFT );
  MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, BG_CGX_OFS_M_TEXT, PLT_BG_TEXT_M );

  p_wk->p_net   = MYSTERY_NET_Init( GAMEDATA_GetSaveControlWork(p_wk->p_gamedata), HEAPID_MYSTERYGIFT );

  MYSTERY_EFFECT_Init( &p_wk->effect, &p_wk->obj, HEAPID_MYSTERYGIFT );
  MYSTERY_EFFECT_Start( &p_wk->effect, MYSTERY_EFFECT_TYPE_NORMAL );
  MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, TRUE );

  PMSND_PlayBGM( SEQ_BGM_WIFI_PRESENT );

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ӂ����Ȃ��������	���C���v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT MYSTERY_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  MYSTERY_WORK  *p_wk     = p_wk_adrs;
  MYSTERY_PARAM *p_param  = p_param_adrs;

  //���W���[���j��
  MYSTERY_EFFECT_Exit( &p_wk->effect );
  MYSTERY_NET_Exit( p_wk->p_net );
  if( p_wk->p_text )
  { 
    MYSTERY_TEXT_Exit( p_wk->p_text );
  }
   MYSTERY_SEQ_Exit( p_wk->p_seq );

	//���ʃ��W���[���̔j��
	WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_err_msg );
	GFL_MSG_Delete( p_wk->p_msg );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_font );

  //���\�[�X�j��
	OBJ_Exit( &p_wk->obj );
	BG_Exit( &p_wk->bg );

  //�O���t�B�b�N�j��
	MYSTERY_GRAPHIC_Exit( p_wk->p_graphic );

  //�Q�[���f�[�^�j��
  GAMEDATA_Delete( p_wk->p_gamedata );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_MYSTERYGIFT );

  //�^�C�g���ɖ߂�
  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);

  GFL_OVERLAY_Unload( FS_OVERLAY_ID(dpw_common));

  PMSND_StopBGM();

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ӂ����Ȃ��������	���C���v���Z�X����
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT MYSTERY_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  MYSTERY_WORK  *p_wk     = p_wk_adrs;
  MYSTERY_PARAM *p_param  = p_param_adrs;

  //�ʐM����
  MYSTERY_NET_Main( p_wk->p_net );

  //�V�[�P���X
  MYSTERY_SEQ_Main( p_wk->p_seq );
  
  //�`��
	MYSTERY_GRAPHIC_2D_Draw( p_wk->p_graphic );
  
  MYSTERY_EFFECT_Main( &p_wk->effect );

  //�v�����g
	PRINTSYS_QUE_Main( p_wk->p_que );
  if( p_wk->p_text )
  { 
    MYSTERY_TEXT_Main( p_wk->p_text );
  }
  if( p_wk->p_winset_s )
  { 
    MYSTERY_MSGWINSET_PrintMain( p_wk->p_winset_s );
  }
  if( p_wk->p_winset_m )
  { 
    MYSTERY_MSGWINSET_PrintMain( p_wk->p_winset_m );
  }

	//�I��
	if( MYSTERY_SEQ_IsEnd( p_wk->p_seq ) )
	{	
		return GFL_PROC_RES_FINISH;
	}
	else
	{	
		return GFL_PROC_RES_CONTINUE;
	}
}

//=============================================================================
/**
 *		BG���\�[�X�ǂ݂���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG���\�[�X�ǂ݂���
 *
 *	@param	BG_WORK *p_wk	���[�N
 *	@param	heapID				�q�[�vID
 */
//-----------------------------------------------------------------------------
static void BG_Init( BG_WORK *p_wk, HEAPID heapID )
{	
  GFL_STD_MemClear( p_wk, sizeof(BG_WORK) );
  p_wk->heapID  = heapID;

  BG_Load( p_wk, BG_LOAD_TYPE_INIT );


  //�����͔�\���̂���
  GFL_BG_SetVisible( BG_FRAME_S_BACK1, FALSE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG���\�[�X�j��
 *
 *	@param	BG_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BG_Exit( BG_WORK *p_wk )
{	
  //GFL_BG_FillCharacterRelease( BG_FRAME_M_LIST, 1, BG_CGX_OFS_M_CLEAR );
  //GFL_BG_FillCharacterRelease( BG_FRAME_M_TEXT, 1, BG_CGX_OFS_M_CLEAR );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�ǂݍ���
 *
 *	@param	BG_WORK *p_wk ���[�N
 *	@param  type          �ǂݍ��݃^�C�v
 */
//-----------------------------------------------------------------------------
static void BG_Load( BG_WORK *p_wk, BG_LOAD_TYPE type )
{ 
  HEAPID  heapID  = GFL_HEAP_LOWID( p_wk->heapID );

  switch( type )
  { 
  case BG_LOAD_TYPE_INIT:
    {	
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
      //PLT
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fusigi_bg_00_NCLR,
          PALTYPE_MAIN_BG, PLT_BG_BACK_M*0x20, 0, heapID );
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fusigi_bg_00_NCLR,
          PALTYPE_SUB_BG, PLT_BG_BACK_S*0x20, 0x20*4, heapID );
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fusigi_card_NCLR,
          PALTYPE_SUB_BG, PLT_BG_CARD_S*0x20, 0x20, heapID );

      //CHR
      GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fusigi_bg_00_NCGR, 
          BG_FRAME_M_BACK2, 0, 0, FALSE, heapID );
      GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fusigi_bg_00_NCGR, 
          BG_FRAME_S_BACK2, 0, 0, FALSE, heapID );

      //SCR
      GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fusigi_bg_00_NSCR,
          BG_FRAME_M_BACK2, 0, 0, FALSE, heapID );
      GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fusigi_bg_00_NSCR,
          BG_FRAME_S_BACK2, 0, 0, FALSE, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
    }
    //�t�H���g
    {	
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_FONT, heapID );

      //�㉺��ʃt�H���g�p���b�g
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_font_default_nclr,
          PALTYPE_MAIN_BG, PLT_BG_FONT_M*0x20, 0x20, heapID );
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_font_default_nclr,
          PALTYPE_SUB_BG, PLT_BG_FONT_S*0x20, 0x20, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
    }

    //�L������P�ʂœǂݍ���
    { 
      static const u8 sc_blank_chr[0x20] = { 
        0
      };
      GFL_BG_LoadCharacter( BG_FRAME_M_TEXT, sc_blank_chr, 0x20, BG_CGX_OFS_M_CLEAR );
      GFL_BG_LoadCharacter( BG_FRAME_M_LIST, sc_blank_chr, 0x20, BG_CGX_OFS_M_CLEAR );
    }


    { 
      TalkWinFrame_GraphicSet( BG_FRAME_M_TEXT, BG_CGX_OFS_M_TEXT, PLT_BG_TEXT_M, MENU_TYPE_SYSTEM, heapID );
      TalkWinFrame_GraphicSet( BG_FRAME_M_LIST, BG_CGX_OFS_M_LIST, PLT_BG_LIST_M, MENU_TYPE_SYSTEM, heapID );

    }
    break;

  case BG_LOAD_TYPE_GUIDE_S:
    {	
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fusigi_bg_00_NCLR,
          PALTYPE_SUB_BG, PLT_BG_BACK_S*0x20, 0x20*6, heapID );
      GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fusigi_bg_00_NCGR, 
          BG_FRAME_S_BACK1, 0, 0, FALSE, heapID );
      GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fushigi_card03_NSCR,
          BG_FRAME_S_BACK1, 0, 0, FALSE, heapID );
      GFL_ARC_CloseDataHandle( p_handle );
    }
    break;

  case BG_LOAD_TYPE_STAGE_M:
    {	
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
      GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fushigi_back_NCGR, 
          BG_FRAME_M_BACK1, 0, 0, FALSE, heapID );
      GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fushigi_stage_NSCR,
          BG_FRAME_M_BACK1, 0, 0, FALSE, heapID );
      GFL_ARC_CloseDataHandle( p_handle );
    }
    break;
  }
}
//=============================================================================
/**
 *		OBJ���\�[�X�ǂ݂���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ���\�[�X�ǂ݂���
 *
 *	@param	OBJ_WORK *p_wk	���[�N
 *	@param	clunit					���j�b�g
 *	@param	HEAPID heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void OBJ_Init( OBJ_WORK *p_wk, GFL_CLUNIT *p_clunit, HEAPID heapID )
{	
	//���\�[�X�ǂ݂���
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );

		p_wk->res_plt[ OBJ_RES_PLT_CURSOR ]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_mystery_title_cursol_NCLR, CLSYS_DRAW_MAX, PLT_OBJ_CURSOR_M*0x20, heapID );
		p_wk->res_cel[ OBJ_RES_CEL_CURSOR ]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_mystery_title_cursol_NCER, NARC_mystery_title_cursol_NANR, heapID );
		p_wk->res_cgx[ OBJ_RES_CGX_CURSOR ]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_mystery_title_cursol_NCGR, FALSE, CLSYS_DRAW_MAX, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}

	//CLWK�o�^
	{
		int i;
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

		p_wk->p_clwk[OBJ_CLWKID_CURSOR]	=		GFL_CLACT_WK_Create( p_clunit,
				p_wk->res_cgx[OBJ_RES_CGX_CURSOR],
				p_wk->res_plt[OBJ_RES_PLT_CURSOR],
				p_wk->res_cel[OBJ_RES_CEL_CURSOR],
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID );
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[OBJ_CLWKID_CURSOR], FALSE );

    for( i = 0; i < EFFECT_BRIGHT_MAX; i++ )
    { 
      cldata.pos_x   = GFUser_GetPublicRand0( 256 );
      cldata.pos_y   = GFUser_GetPublicRand0( 256 ) - (256 + 16);
      cldata.bgpri   = BG_FRAME_M_BACK2;
      cldata.anmseq  = 3 + GFUser_GetPublicRand0( 6 );
      p_wk->p_clwk[OBJ_CLWKID_BRIGHT_TOP + i]	=		GFL_CLACT_WK_Create( p_clunit,
          p_wk->res_cgx[OBJ_RES_CGX_CURSOR],
          p_wk->res_plt[OBJ_RES_PLT_CURSOR],
          p_wk->res_cel[OBJ_RES_CEL_CURSOR],
          &cldata,
          CLSYS_DEFREND_MAIN,
          heapID );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[OBJ_CLWKID_BRIGHT_TOP + i], FALSE );
    }

    cldata.pos_x   = 128;
    cldata.pos_y   = 177;
    cldata.bgpri   = 0;
    cldata.softpri = 1;
    cldata.anmseq  = 9;
    p_wk->p_clwk[OBJ_CLWKID_BTN]	=		GFL_CLACT_WK_Create( p_clunit,
				p_wk->res_cgx[OBJ_RES_CGX_CURSOR],
				p_wk->res_plt[OBJ_RES_PLT_CURSOR],
				p_wk->res_cel[OBJ_RES_CEL_CURSOR],
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID );
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[OBJ_CLWKID_BTN], FALSE );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ���\�[�X�j��
 *
 *	@param	BG_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void OBJ_Exit( OBJ_WORK *p_wk )
{	
	//CLWK�j��
	{	
		int i;
		for( i = 0; i < OBJ_CLWKID_MAX; i++ )
		{	
			if( p_wk->p_clwk[i] )
			{	
				GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
			}
		}
	}

	//���\�[�X�j��
	{
    int i;
    for( i = 0; i < OBJ_RES_PLT_MAX; i++ )
    { 
      GFL_CLGRP_PLTT_Release( p_wk->res_plt[i] );
    }
    for( i = 0; i < OBJ_RES_CGX_MAX; i++ )
    { 
      GFL_CLGRP_CGR_Release( p_wk->res_cgx[i] );
    }
    for( i = 0; i < OBJ_RES_CEL_MAX; i++ )
    { 
      GFL_CLGRP_CELLANIM_Release( p_wk->res_cel[i] );
    }
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	CLWK�擾
 *
 *	@param	const OBJ_WORK *p_wk		���[�N
 *	@param	clwkID									CLWK�ԍ�
 *
 *	@return	CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK *OBJ_GetClwk( const OBJ_WORK *cp_wk, u32 clwkID )
{	
	return cp_wk->p_clwk[ clwkID ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ēǂݍ���
 *
 *	@param	OBJ_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void OBJ_ReLoad( OBJ_WORK *p_wk, HEAPID heapID )
{ 

  //���\�[�X�ǂ݂���
	{	
    void *p_plt_buff;
    NNSG2dPaletteData *p_plt_data;
    p_plt_buff  = GFL_ARC_UTIL_LoadPalette( ARCID_MYSTERY, NARC_mystery_title_cursol_NCLR, &p_plt_data, heapID );
    GFL_CLGRP_PLTT_Replace( p_wk->res_plt[ OBJ_RES_PLT_CURSOR ], p_plt_data, 6 );
    GFL_HEAP_FreeMemory( p_plt_buff );
	} 
}
//=============================================================================
/**
 *					SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�J�n
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Start( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  MYSTERY_WORK  *p_wk     = p_wk_adrs;  
  
  MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_001, MYSTERY_TEXT_TYPE_QUE );
  UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_TOP, HEAPID_MYSTERYGIFT ); 

	MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�C��
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{	
	enum
	{	
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_END,
	};

	switch( *p_seq )
	{	
	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
		break;

	default:
		GF_ASSERT(0);
	}
	
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
	enum
	{
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};	

	switch( *p_seq )
	{	
	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_EXIT;
		}
		break;

	case SEQ_EXIT:
		MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_End );
		break;

	default:
		GF_ASSERT(0);
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�n�I��
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_StartSelect( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{	
  enum
  { 
    SEQ_INIT,
    SEQ_TOP_SELECT,
    SEQ_YESNO_INIT,
    SEQ_YESNO_SELECT,
    SEQ_NET_INIT,
    SEQ_NET_WAIT,
    SEQ_NET_SELECT_MSG,
    SEQ_NET_SELECT_WAIT,
    SEQ_NET_YESNO_MSG,
    SEQ_NET_YESNO_INIT,
    SEQ_NET_YESNO_WAIT,
    SEQ_NET_EXIT,
    SEQ_NET_EXIT_WAIT,
    SEQ_NET_EXIT_RET,
    SEQ_NET_EXIT_RET_WAIT,

    //�J�[�h�������ς��ō폜�V�[�P���X��
    SEQ_FULL_YESNO_MSG,
    SEQ_FULL_YESNO_INIT,
    SEQ_FULL_YESNO_WAIT,

    SEQ_MSG_WAIT,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  //�G���[�`�F�b�N
  switch( MYSTERY_NET_GetErrorRepairType( p_wk->p_net ) )
  { 
  case MYSTERY_NET_ERROR_REPAIR_RETURN:      //�P�O�̑I�����܂Ŗ߂�
  case MYSTERY_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf����
    MYSTERY_NET_ClearError( p_wk->p_net );

    *p_seq  = SEQ_INIT;
    break;
  }

  //�V�[�P���X
  switch( *p_seq )
  { 
  case SEQ_INIT:
    if( p_wk->p_menu == NULL )
    {
      MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_001, MYSTERY_TEXT_TYPE_QUE );
      UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_TOP, HEAPID_MYSTERYGIFT ); 
    }
    *p_seq  = SEQ_TOP_SELECT;
    break;

  case SEQ_TOP_SELECT:
    //��������̂������Ƃ�or�J�[�h�A���o�����݂�or��߂�
    { 
      u32 ret;
      ret = MYSTERY_MENU_Main( p_wk->p_menu ); 
      if( ret != MYSTERY_MENU_SELECT_NULL )
      {
        if( ret == 0 )
        { 
          BOOL  is_full;
          BOOL  is_no_recv  = TRUE;

          //��������̂������Ƃ�
          UTIL_DeleteMenu( p_wk );

          is_full = !MYSTERYDATA_CheckCardDataSpace( p_wk->p_sv );

          //���擾�̑��蕨�f�[�^��MAX���`�F�b�N
          if( is_full )
          { 
            int i;
            for( i = 0; i < GIFT_DATA_MAX; i++ )
            { 
              if( MYSTERYDATA_IsHavePresent( p_wk->p_sv, i ) )
              { 
                is_no_recv  = FALSE;
              }
            }
          }

          if( is_full )
          { 
            if( is_no_recv )
            { 
              //���擾�̑��蕨�f�[�^��MAX
              MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_err_001, MYSTERY_TEXT_TYPE_STREAM );
              MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INIT );
              *p_seq  = SEQ_MSG_WAIT;
            }
            else
            { 
              //�J�[�h�̐���MAX
              *p_seq  = SEQ_FULL_YESNO_MSG;
            }
          }
          else
          { 
            if( !DS_SYSTEM_IsAvailableWireless() )
            { 
              //DS�̖����ݒ�ŒʐM�s�̂Ƃ�
              MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_EnableWireless );
            }
#if 0
            else if( DS_SYSTEM_IsRestrictUGC() )
            { 
              //�y�A�����^���R���g���[���ő���M���ۂ��Ă���Ƃ�
              MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_RestrictUGC );
            }
#endif
            else
            { 
              //�ʏ펞  �󂯎���
              MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_002, MYSTERY_TEXT_TYPE_STREAM );
              MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_YESNO_INIT );
              *p_seq  = SEQ_MSG_WAIT;
            }
          }
        }
        else if( ret == 1 )
        { 
          //�J�[�h�A���o�����݂�
          UTIL_DeleteMenu( p_wk );
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_CardAlbum );
        }
        else if( ret == 2)
        { 
          //���߂�������
          UTIL_DeleteMenu( p_wk );
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_Infomation );
        }
        else if( ret == 3 )
        {
          //��߂�
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
        }
      }
    }
    break;
    
  case SEQ_YESNO_INIT:
    UTIL_CreateList( p_wk, UTIL_LIST_TYPE_YESNO, HEAPID_MYSTERYGIFT );
    *p_seq  = SEQ_YESNO_SELECT;
    break;

  case SEQ_YESNO_SELECT:
    //��������J�n���܂���낵���ł���
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        UTIL_DeleteList( p_wk );
        if( ret == 0 )
        { 
          //�͂�
          *p_seq  = SEQ_NET_INIT;
        }
        else if( ret == 1 )
        { 
          //������
          *p_seq  = SEQ_INIT;
        }
      }
    }
    break;
  
  case SEQ_NET_INIT:
    MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_START_BEACON );
    *p_seq  = SEQ_NET_WAIT;
    break;

  case SEQ_NET_WAIT:
    if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_MAIN_BEACON )
    {
      *p_seq  = SEQ_NET_SELECT_MSG;
    }
    break;

  case SEQ_NET_SELECT_MSG:
    UTIL_DeleteMenu( p_wk );
    UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_NETMODE, HEAPID_MYSTERYGIFT );
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_003, MYSTERY_TEXT_TYPE_QUE );
    *p_seq  = SEQ_NET_SELECT_WAIT;
    break;

  case SEQ_NET_SELECT_WAIT:
    { 
      int i;
      u32 ret;
      GAME_COMM_STATUS_BIT  bit;

      bit = MYSTERY_NET_GetCommStatus( p_wk->p_net );
      if( bit & (GAME_COMM_STATUS_BIT_WIRELESS|GAME_COMM_STATUS_BIT_WIRELESS_TR|GAME_COMM_STATUS_BIT_WIRELESS_UN|GAME_COMM_STATUS_BIT_WIRELESS_FU) )
      { 
        MYSTERY_MENU_BlinkMain( p_wk->p_menu, 0 );
      }
      if( bit & (GAME_COMM_STATUS_BIT_WIFI|GAME_COMM_STATUS_BIT_WIFI_ZONE|GAME_COMM_STATUS_BIT_WIFI_FREE|GAME_COMM_STATUS_BIT_WIFI_LOCK) )
      { 
        MYSTERY_MENU_BlinkMain( p_wk->p_menu, 1 );
      }
      if( bit & GAME_COMM_STATUS_BIT_IRC )
      { 
        MYSTERY_MENU_BlinkMain( p_wk->p_menu, 2 );
      }

      //�I��
      ret = MYSTERY_MENU_Main( p_wk->p_menu ); 
      if( ret != MYSTERY_MENU_SELECT_NULL )
      { 
        UTIL_DeleteMenu( p_wk );

        switch( ret )
        { 
        case 0: //���C�����X
          p_wk->mode  = MYSTERY_NET_MODE_WIRELESS;
          *p_seq  = SEQ_NET_YESNO_MSG;
          break;
        case 1: //Wi-Fi
          p_wk->mode  = MYSTERY_NET_MODE_WIFI;
          *p_seq  = SEQ_NET_EXIT;
          break;
        case 2: //�ԊO��
          p_wk->mode  = MYSTERY_NET_MODE_IRC;
          *p_seq  = SEQ_NET_YESNO_MSG;
          break;
        case 3: //���ǂ�
          *p_seq  = SEQ_NET_EXIT_RET;
          break;
        }
      }
    }
    break;

  case SEQ_NET_YESNO_MSG:
    //���C�����XorWIFIor�ԊO���ł��������܂�
    switch( p_wk->mode )
    {
    case MYSTERY_NET_MODE_WIRELESS:
      MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_007, MYSTERY_TEXT_TYPE_STREAM );
      MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NET_YESNO_INIT );
      *p_seq  = SEQ_MSG_WAIT;
      break;

    case MYSTERY_NET_MODE_WIFI:
      GF_ASSERT(0); //�����ւ͂��Ȃ�
      break;

    case MYSTERY_NET_MODE_IRC:
      MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_009, MYSTERY_TEXT_TYPE_STREAM );
      MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NET_YESNO_INIT );
      *p_seq  = SEQ_MSG_WAIT;
      break;
    } 
    break;

  case SEQ_NET_YESNO_INIT:
    UTIL_CreateList( p_wk, UTIL_LIST_TYPE_YESNO, HEAPID_MYSTERYGIFT );
    *p_seq  = SEQ_NET_YESNO_WAIT;
    break;

  case SEQ_NET_YESNO_WAIT:
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        UTIL_DeleteList( p_wk );
        if( ret == 0 )
        { 
          //�͂�
          *p_seq  =  SEQ_NET_EXIT;
        }
        else if( ret == 1 )
        { 
          //������
          *p_seq  = SEQ_NET_SELECT_MSG;
        }
      }
    }
    break;

  //�ؒf���Đ��
  case SEQ_NET_EXIT:
    if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_MAIN_BEACON )
    { 
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_END_BEACON );
      *p_seq  = SEQ_NET_EXIT_WAIT;
    }
    break;

  case SEQ_NET_EXIT_WAIT:
    if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_WAIT )
    {
      //�͂�
      if( p_wk->mode == MYSTERY_NET_MODE_WIFI )
      { 
        MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_WifiLogin );
      }
      else
      { 
        MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_RecvGift );
      }
    }
    break;

  //�ؒf���Ė߂�
  case SEQ_NET_EXIT_RET:
    if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_MAIN_BEACON )
    { 
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_END_BEACON );
      *p_seq  = SEQ_NET_EXIT_RET_WAIT;
    }
    else
    { 
      *p_seq  = SEQ_INIT;
    }
    break;

  case SEQ_NET_EXIT_RET_WAIT:
    if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_WAIT )
    {
      *p_seq  = SEQ_INIT;
    }
    break;

    //-------------------------------------
    ///	�J�[�h�����ς��Ő����V�[�P���X��
    //=====================================
  case SEQ_FULL_YESNO_MSG:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_err_005, MYSTERY_TEXT_TYPE_STREAM );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_FULL_YESNO_INIT );
    *p_seq  = SEQ_MSG_WAIT;
    break;

  case SEQ_FULL_YESNO_INIT:
    UTIL_CreateList( p_wk, UTIL_LIST_TYPE_YESNO, HEAPID_MYSTERYGIFT );
    *p_seq  = SEQ_FULL_YESNO_WAIT;
    break;

  case SEQ_FULL_YESNO_WAIT:
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        UTIL_DeleteList( p_wk );
        if( ret == 0 )
        { 
          //�͂�
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_DeleteCard );
        }
        else if( ret == 1 )
        { 
          //������
          *p_seq  = SEQ_INIT;
        }
      }
    }
    break;
    
  case SEQ_MSG_WAIT:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      MYSTERY_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	����
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Infomation( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INFO_SELECT_MSG,
    SEQ_INFO_SELECT_INIT,
    SEQ_INFO_SELECT_WAIT,

    SEQ_MSG_WAIT,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INFO_SELECT_MSG:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_022, MYSTERY_TEXT_TYPE_QUE );
    UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_INFO, HEAPID_MYSTERYGIFT ); 
    *p_seq  = SEQ_INFO_SELECT_WAIT;
    break;

  case SEQ_INFO_SELECT_WAIT:
    { 
      u32 ret;
      ret = MYSTERY_MENU_Main( p_wk->p_menu ); 
      if( ret != MYSTERY_MENU_SELECT_NULL )
      {
        UTIL_DeleteMenu( p_wk );
        if( ret == 0 )
        { 
          //�ӂ����Ȃ�������̂ɂ���
          MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_023, MYSTERY_TEXT_TYPE_STREAM );
          MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INFO_SELECT_MSG );
          *p_seq  = SEQ_MSG_WAIT;
        }
        else if( ret == 1 )
        { 
          //�J�[�h�A���o���ɂ���
          MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_024, MYSTERY_TEXT_TYPE_STREAM );
          MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INFO_SELECT_MSG );
          *p_seq  = SEQ_MSG_WAIT;
        }
        else if( ret == 2 )
        { 
          //��������̂������ς��̂Ƃ�
          MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_025, MYSTERY_TEXT_TYPE_STREAM );
          MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INFO_SELECT_MSG );
          *p_seq  = SEQ_MSG_WAIT;
        }
        else if( ret == 3 )
        {
          //��߂�
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
        }
      }
    }
    break;

  case SEQ_MSG_WAIT:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      MYSTERY_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	��M
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RecvGift( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_SEARCH,
    SEQ_NO_GIFT_INIT,
    SEQ_NO_GIFT_WAIT,
    SEQ_SELECT_GIFT_MSG,
    SEQ_SELECT_GIFT_WAIT,
    SEQ_GIFT_YESNO_INIT,
    SEQ_GIFT_YESNO_WAIT,
    SEQ_CANCEL_GIFT_INIT,
    SEQ_CANCEL_GIFT_WAIT,

    SEQ_DIRTY_MSG,
    SEQ_DIRTY_END,

    SEQ_MSG_WAIT,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    switch( p_wk->mode )
    { 
    case MYSTERY_NET_MODE_WIFI:
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_WIFI_DOWNLOAD );
      break;
    case MYSTERY_NET_MODE_WIRELESS:
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_START_BEACON_DOWNLOAD );
      break;
    case MYSTERY_NET_MODE_IRC:
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_START_IRC_DOWNLOAD );
      break;
    }

    //�������Ă��܂�
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_010, MYSTERY_TEXT_TYPE_STREAM );
    p_wk->cnt = 0;
    *p_seq  = SEQ_SEARCH;
    break;

  case SEQ_SEARCH:
    { 
      if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_WAIT )
      {
        BOOL ret;
        ret = MYSTERY_NET_GetDownloadData( p_wk->p_net, &p_wk->data, sizeof(DOWNLOAD_GIFT_DATA) );
        if( ret )
        { 
          u32 dirty;

          dirty = MYSTERYDATA_ModifyDownloadData( &p_wk->data, p_wk->p_gamedata, GFL_HEAP_LOWID(HEAPID_MYSTERYGIFT) );

#ifdef PM_DEBUG
          { 
          static char title_buff[GIFT_DATA_CARD_TITLE_MAX];
          static char text_buff[GIFT_DATA_CARD_TEXT_MAX];
          DEB_STR_CONV_StrcodeToSjis( p_wk->data.data.event_name, title_buff, GIFT_DATA_CARD_TITLE_MAX );
          DEB_STR_CONV_StrcodeToSjis( p_wk->data.event_text, text_buff, GIFT_DATA_CARD_TEXT_MAX );

          NAGI_Printf( "�擾���܂���\n" );
          NAGI_Printf( "TITLE: %s\n", title_buff );
          NAGI_Printf( "TEXT : %s\n", text_buff );
          NAGI_Printf( "EV_ID: %d\n", p_wk->data.data.event_id );
          NAGI_Printf( "TYPE : %d\n",   p_wk->data.data.gift_type );
          NAGI_Printf( "ONLYONE : %d\n",   p_wk->data.data.only_one_flag );
          NAGI_Printf( "�s�� : %d\n", dirty );
          }
#endif //PM_DEBUG

          if( dirty == 0 )
          { 
            *p_seq  = SEQ_SELECT_GIFT_MSG;
          }
          else
          { 
            *p_seq  = SEQ_DIRTY_MSG;
          }
        }
        else
        { 
          NAGI_Printf( "�擾�ł��Ȃ�����\n" );
          *p_seq = SEQ_NO_GIFT_INIT;
        }
      }
    }

    //�G���[�`�F�b�N
    switch( MYSTERY_NET_GetErrorRepairType( p_wk->p_net ) )
    { 
    case MYSTERY_NET_ERROR_REPAIR_RETURN:      //�P�O�̑I�����܂Ŗ߂�
    case MYSTERY_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf����
      MYSTERY_NET_ClearError( p_wk->p_net );

      NAGI_Printf( "�擾�ł��Ȃ�����\n" );
      *p_seq = SEQ_NO_GIFT_INIT;
      break;
    }

    //B�{�^���L�����Z�����^�C���A�E�g
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL || p_wk->cnt++ > MYSTERY_RECV_TIMEOUT )
    { 
      BOOL is_cancel  = FALSE;
      switch( p_wk->mode )
      { 
      case MYSTERY_NET_MODE_WIFI:
        MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_CANCEL_WIFI_DOWNLOAD );
        is_cancel  = TRUE;
        break;
      case MYSTERY_NET_MODE_WIRELESS:
        if( MYSTERY_NET_GetState( p_wk->p_net ) == MYSTERY_NET_STATE_MAIN_BEACON_DOWNLOAD )
        { 
          MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_END_BEACON_DOWNLOAD );
          is_cancel  = TRUE;
        }
        break;
      case MYSTERY_NET_MODE_IRC:
        if( MYSTERY_NET_GetState( p_wk->p_net ) == MYSTERY_NET_STATE_MAIN_IRC_DOWNLOAD )
        { 
          MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_END_IRC_DOWNLOAD );
          is_cancel  = TRUE;
        }
        break;
      }

      if( is_cancel )
      { 
        //B�L�����Z��or�^�C���A�E�g
        *p_seq = SEQ_NO_GIFT_INIT;
        p_wk->cnt   = 0;
      }
    }
    break;

  case SEQ_NO_GIFT_INIT:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_err_003, MYSTERY_TEXT_TYPE_STREAM );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NO_GIFT_WAIT );
    *p_seq  = SEQ_MSG_WAIT;
    break;

  case SEQ_NO_GIFT_WAIT:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
    { 
      MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_DisConnectReturn );
    }
    break;

  case SEQ_SELECT_GIFT_MSG:
    UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_GIFT, HEAPID_MYSTERYGIFT );
    UTIL_CreateGuideText( p_wk, HEAPID_MYSTERYGIFT );
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_011, MYSTERY_TEXT_TYPE_QUE );
    *p_seq  = SEQ_SELECT_GIFT_WAIT;
    break;

  case SEQ_SELECT_GIFT_WAIT:
    //�����Ƃ邨������̂������ł�������
    { 
      u32 ret;
      ret = MYSTERY_MENU_Main( p_wk->p_menu ); 
      if( ret != MYSTERY_MENU_SELECT_NULL )
      { 
        if( ret == 0 )
        { 

          MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_013, MYSTERY_TEXT_TYPE_STREAM );
          MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_GIFT_YESNO_INIT );
          *p_seq  = SEQ_MSG_WAIT;
        }
      }
      else  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL )
      { 
        MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_012, MYSTERY_TEXT_TYPE_STREAM );
        MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_CANCEL_GIFT_INIT );
        *p_seq  = SEQ_MSG_WAIT;
      }
    }
    break;

  case SEQ_GIFT_YESNO_INIT:
    UTIL_CreateList( p_wk, UTIL_LIST_TYPE_YESNO, HEAPID_MYSTERYGIFT );
    *p_seq  = SEQ_GIFT_YESNO_WAIT;
    break;

  case SEQ_GIFT_YESNO_WAIT:
    //��������̂����サ�񂵂܂���
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        UTIL_DeleteList( p_wk );
        if( ret == 0 )
        { 
          //�͂�
          UTIL_DeleteMenu(p_wk);

          //���łɎ󂯎���Ă�����A�󂯎��Ȃ�
          if( p_wk->data.data.only_one_flag && MYSTERYDATA_IsEventRecvFlag( p_wk->p_sv, p_wk->data.data.event_id ) )
          { 
            NAGI_Printf( "���łɎ󂯎���Ă��܂���\n" );
            *p_seq = SEQ_SELECT_GIFT_MSG;
          }
          else
          { 
            MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_Demo );
          }
        }
        else if( ret == 1 )
        { 
          //������
          *p_seq  = SEQ_SELECT_GIFT_MSG;
        }
      }
    }
    break;

  case SEQ_CANCEL_GIFT_INIT:
    UTIL_CreateList( p_wk, UTIL_LIST_TYPE_YESNO, HEAPID_MYSTERYGIFT );
    *p_seq  = SEQ_CANCEL_GIFT_WAIT;
    break;

  case SEQ_CANCEL_GIFT_WAIT:
    //��������̂̂����Ƃ����߂܂���
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        UTIL_DeleteList( p_wk );
        if( ret == 0 )
        { 
          //�͂�
          UTIL_DeleteMenu(p_wk);
          UTIL_DeleteGuideText( p_wk );
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
        }
        else if( ret == 1 )
        { 
          //������
          *p_seq  = SEQ_SELECT_GIFT_MSG;
        }
      }
    }
    break;

    //-------------------------------------
    ///	�s���ȏꍇ
    //=====================================
  case SEQ_DIRTY_MSG:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_err_006, MYSTERY_TEXT_TYPE_STREAM );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    *p_seq  = SEQ_MSG_WAIT;
    break;

  case SEQ_DIRTY_END:
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_DisConnectReturn );
    break;

  case SEQ_MSG_WAIT:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      MYSTERY_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f��
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Demo( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_INIT_WAIT,
    SEQ_DEMO_MAIN,
    SEQ_SAVE_INIT,
    SEQ_SAVE_MAIN,
    SEQ_RECV,
    SEQ_MSG_WAIT,
    SEQ_CARD_INIT,
    SEQ_START_CARD_EFFECT,
    SEQ_WAIT_CARD_EFFECT,
    SEQ_WAIT_SYNC,
    SEQ_INIT_BTN,
    SEQ_CARD_WAIT,
    SEQ_END,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    MYSTERY_EFFECT_Start( &p_wk->effect, MYSTERY_EFFECT_TYPE_DEMO );

    //��������̂���M���ł�
    { 
      GFL_CLUNIT	*p_unit	= MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
      MYSTERY_DEMO_Init( &p_wk->demo, p_unit, &p_wk->data, p_wk->p_gamedata, HEAPID_MYSTERYGIFT );
      MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_014, MYSTERY_TEXT_TYPE_QUE );
      *p_seq  = SEQ_INIT_WAIT;
    }
    break;

  case SEQ_INIT_WAIT:
    if( MYSTERY_TEXT_IsEndPrint(p_wk->p_text) )
    {
      *p_seq  = SEQ_DEMO_MAIN;
    }
    break;

  case SEQ_DEMO_MAIN:
    MYSTERY_DEMO_Main( &p_wk->demo, &p_wk->bg );
    if( MYSTERY_DEMO_IsEnd(&p_wk->demo) )
    {
      *p_seq  = SEQ_SAVE_INIT;
    }
    break;

  case SEQ_SAVE_INIT:
    GAMEDATA_SaveAsyncStart(p_wk->p_gamedata);
    *p_seq  = SEQ_SAVE_MAIN;
    break;

  case SEQ_SAVE_MAIN:
    { 
      SAVE_RESULT result  = GAMEDATA_SaveAsyncMain(p_wk->p_gamedata);
      if( result == SAVE_RESULT_OK )
      { 
        *p_seq  = SEQ_RECV;
      }
    }
    break;

  case SEQ_RECV:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_015, MYSTERY_TEXT_TYPE_STREAM );
    *p_seq  = SEQ_MSG_WAIT;
    break;

  case SEQ_MSG_WAIT:
    if( MYSTERY_TEXT_IsEndPrint(p_wk->p_text) )
    { 
      MYSTERY_EFFECT_Start( &p_wk->effect, MYSTERY_EFFECT_TYPE_NORMAL );
      *p_seq  = SEQ_CARD_INIT;
    }
    break;

  case SEQ_CARD_INIT:
    { 
      MYSTERY_DEMO_Exit( &p_wk->demo );
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
      UTIL_DeleteMenu( p_wk );

      GFL_BG_SetVisible( BG_FRAME_M_TEXT, FALSE );
      { 
        MYSTERY_CARD_SETUP setup;
        GFL_STD_MemClear( &setup, sizeof(MYSTERY_CARD_SETUP) );

        setup.p_data   = &p_wk->data.data;
        setup.back_frm = BG_FRAME_M_BACK1;
        setup.font_frm = BG_FRAME_M_LIST;
        setup.back_plt_num  = PLT_BG_CARD_M;
        setup.font_plt_num  = PLT_BG_FONT_M;
        setup.icon_obj_plt_num  = PLT_OBJ_CARD_ICON_M; 
        setup.silhouette_obj_plt_num  = PLT_OBJ_CARD_SILHOUETTE_M; 
        setup.p_clunit  = MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
        setup.p_sv      = p_wk->p_sv;
        setup.p_msg     = p_wk->p_msg; 
        setup.p_font    = p_wk->p_font; 
        setup.p_que     = p_wk->p_que; 
        setup.p_word    = p_wk->p_word;
        p_wk->p_card  = MYSTERY_CARD_Init( &setup, p_wk->p_gamedata, HEAPID_MYSTERYGIFT );
      }
    }
    *p_seq  = SEQ_START_CARD_EFFECT;
    break;

  case SEQ_START_CARD_EFFECT:
    MYSTERY_CARD_Main( p_wk->p_card );
    MYSTERY_CARD_StartEffect( p_wk->p_card );
    *p_seq = SEQ_WAIT_CARD_EFFECT;
    break;

  case SEQ_WAIT_CARD_EFFECT:
    MYSTERY_CARD_Main( p_wk->p_card );
    if( MYSTERY_CARD_IsEndEffect(p_wk->p_card) )
    { 
      *p_seq  = SEQ_WAIT_SYNC;
    }
    break;
    
  case SEQ_WAIT_SYNC:
    if( p_wk->cnt++ > 30 )
    { 
      p_wk->cnt  = 0;
      *p_seq  = SEQ_INIT_BTN;
    }
    break;

  case SEQ_INIT_BTN:
    {	
      GFL_CLUNIT	*p_clunit	= MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
      MYSTERY_BTN_Init( &p_wk->btn, &p_wk->obj, p_clunit, p_wk->p_que, p_wk->p_msg, syachi_mystery_album_009, p_wk->p_font, HEAPID_MYSTERYGIFT );
      MYSTERY_BTN_PrintMain( &p_wk->btn );
      *p_seq  = SEQ_CARD_WAIT;
    }
    break;

  case SEQ_CARD_WAIT:
    MYSTERY_BTN_PrintMain( &p_wk->btn );
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
    { 
      MYSTERY_BTN_Exit( &p_wk->btn );
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_DisConnectEnd );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�h�A���o��
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_CardAlbum( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_FADEOUT_INIT,
    SEQ_INIT,
    SEQ_START_FADEIN_INIT,
    SEQ_MAIN,
    SEQ_START_FADEOUT_EXIT,
    SEQ_EXIT,
    SEQ_START_FADEIN_EXIT,
    SEQ_NEXT_STARTSELECT,

    SEQ_WAIT_FADE,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_FADEOUT_INIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INIT );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_INIT:
    MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, FALSE );
    { 
      UTIL_DeleteMenu( p_wk );
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }
    {
      MYSTERY_ALBUM_SETUP setup;
      GFL_STD_MemClear( &setup, sizeof(MYSTERY_LIST_SETUP) );
      setup.mode  = MYSTERY_ALBUM_MODE_VIEW;
      setup.p_clunit  = MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
      setup.p_sv      = p_wk->p_sv;
      setup.p_font    = p_wk->p_font;
      setup.p_que     = p_wk->p_que;
      setup.p_word    = p_wk->p_word;
      setup.p_msg     = p_wk->p_msg;
      setup.p_gamedata  = p_wk->p_gamedata;
      p_wk->p_album = MYSTERY_ALBUM_Init( &setup, HEAPID_MYSTERYGIFT );
      *p_seq  = SEQ_START_FADEIN_INIT;
    }
    break;
  case SEQ_START_FADEIN_INIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_MAIN );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_MAIN:
    MYSTERY_ALBUM_Main( p_wk->p_album );
    if( MYSTERY_ALBUM_IsEnd( p_wk->p_album ) )
    { 
      *p_seq  = SEQ_START_FADEOUT_EXIT;
    }
    break;
  case SEQ_START_FADEOUT_EXIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_EXIT );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_EXIT:
    MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, TRUE );
    MYSTERY_ALBUM_Exit( p_wk->p_album );
    p_wk->p_album = NULL;
    BG_Load( &p_wk->bg, BG_LOAD_TYPE_INIT );
    OBJ_ReLoad( &p_wk->obj, HEAPID_MYSTERYGIFT );
    p_wk->p_text  = MYSTERY_TEXT_Init( BG_FRAME_M_TEXT, PLT_BG_FONT_M, p_wk->p_que, p_wk->p_font, HEAPID_MYSTERYGIFT );
    MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, BG_CGX_OFS_M_TEXT, PLT_BG_TEXT_M );
    *p_seq  = SEQ_START_FADEIN_EXIT;
    break;
  case SEQ_START_FADEIN_EXIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NEXT_STARTSELECT );
    *p_seq  = SEQ_WAIT_FADE;
    break;

  case SEQ_NEXT_STARTSELECT:
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    break;

  case SEQ_WAIT_FADE:
		if( !GFL_FADE_CheckFade() )
		{	
      MYSTERY_SEQ_NextReservSeq( p_seqwk );
		}
    break;
  }

  if( p_wk->p_album )
  { 
    MYSTERY_ALBUM_PrintMain( p_wk->p_album );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�h�A���o���@�J�[�h�폜
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DeleteCard( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_FADEOUT_INIT,
    SEQ_INIT,
    SEQ_START_FADEIN_INIT,
    SEQ_MAIN,
    SEQ_START_FADEOUT_EXIT,
    SEQ_EXIT,
    SEQ_START_FADEIN_EXIT,
    SEQ_NEXT_STARTSELECT,

    SEQ_WAIT_FADE,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_FADEOUT_INIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INIT );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_INIT:
    MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, FALSE );
    { 
      UTIL_DeleteMenu( p_wk );
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }
    {
      MYSTERY_ALBUM_SETUP setup;
      GFL_STD_MemClear( &setup, sizeof(MYSTERY_LIST_SETUP) );
      setup.mode  = MYSTERY_ALBUM_MODE_DELETE;
      setup.p_clunit  = MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
      setup.p_sv      = p_wk->p_sv;
      setup.p_font    = p_wk->p_font;
      setup.p_que     = p_wk->p_que;
      setup.p_word    = p_wk->p_word;
      setup.p_msg     = p_wk->p_msg;
      setup.p_gamedata  = p_wk->p_gamedata;
      p_wk->p_album = MYSTERY_ALBUM_Init( &setup, HEAPID_MYSTERYGIFT );
      *p_seq  = SEQ_START_FADEIN_INIT;
    }
    break;
  case SEQ_START_FADEIN_INIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_MAIN );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_MAIN:
    MYSTERY_ALBUM_Main( p_wk->p_album );
    if( MYSTERY_ALBUM_IsEnd( p_wk->p_album ) )
    { 
      *p_seq  = SEQ_START_FADEOUT_EXIT;
    }
    break;
  case SEQ_START_FADEOUT_EXIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_EXIT );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_EXIT:
    MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, TRUE );
    MYSTERY_ALBUM_Exit( p_wk->p_album );
    p_wk->p_album = NULL;
    BG_Load( &p_wk->bg, BG_LOAD_TYPE_INIT );
    OBJ_ReLoad( &p_wk->obj, HEAPID_MYSTERYGIFT );
    p_wk->p_text  = MYSTERY_TEXT_Init( BG_FRAME_M_TEXT, PLT_BG_FONT_M, p_wk->p_que, p_wk->p_font, HEAPID_MYSTERYGIFT );
    MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, BG_CGX_OFS_M_TEXT, PLT_BG_TEXT_M );
    *p_seq  = SEQ_START_FADEIN_EXIT;
    break;
  case SEQ_START_FADEIN_EXIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NEXT_STARTSELECT );
    *p_seq  = SEQ_WAIT_FADE;
    break;

  case SEQ_NEXT_STARTSELECT:
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    break;

  case SEQ_WAIT_FADE:
		if( !GFL_FADE_CheckFade() )
		{	
      MYSTERY_SEQ_NextReservSeq( p_seqwk );
		}
    break;
  }

  if( p_wk->p_album )
  { 
    MYSTERY_ALBUM_PrintMain( p_wk->p_album );
  }
}

//----------------------------------------------------------------------------
/** 
 *	@brief  WiFiLogin��ʂ̐ڑ�
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_WifiLogin( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
    SEQ_DELETE,
    SEQ_PROC_CALL,
    SEQ_PROC_WAIT,
    SEQ_CREATE,
    SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
    SEQ_END,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  //�G���[�`�F�b�N
  switch( MYSTERY_NET_GetErrorRepairType( p_wk->p_net ) )
  { 
  case MYSTERY_NET_ERROR_REPAIR_RETURN:      //�P�O�̑I�����܂Ŗ߂�
  case MYSTERY_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf����
    MYSTERY_NET_ClearError( p_wk->p_net );

    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    break;
  }

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
      MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, FALSE );
			*p_seq	= SEQ_DELETE;
		}
		break;

  case SEQ_DELETE:
    if( p_wk->p_text )
    { 
      MYSTERY_TEXT_Exit( p_wk->p_text );
    }

    //���\�[�X�j��
    OBJ_Exit( &p_wk->obj );
    BG_Exit( &p_wk->bg );

    //�O���t�B�b�N�j��
    MYSTERY_GRAPHIC_Exit( p_wk->p_graphic );

    *p_seq  = SEQ_PROC_CALL;
    break;

  case SEQ_PROC_CALL:
    p_wk->p_wifilogin_param = GFL_HEAP_AllocMemory( HEAPID_MYSTERYGIFT, sizeof(WIFILOGIN_PARAM) );
    GFL_STD_MemClear( p_wk->p_wifilogin_param, sizeof(WIFILOGIN_PARAM) );
    p_wk->p_wifilogin_param->gamedata     = p_wk->p_gamedata;
    p_wk->p_wifilogin_param->bg           = WIFILOGIN_BG_NORMAL;
    p_wk->p_wifilogin_param->display      = WIFILOGIN_DISPLAY_UP;
    p_wk->p_wifilogin_param->pSvl         = NULL;

    GFL_PROC_SysCallProc( FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, p_wk->p_wifilogin_param );
    *p_seq  = SEQ_CREATE;
    break;

  case SEQ_CREATE:
    //�O���t�B�b�N�ݒ�
    p_wk->p_graphic	= MYSTERY_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_MYSTERYGIFT );

    //���\�[�X�ǂݍ���
    BG_Init( &p_wk->bg, HEAPID_MYSTERYGIFT );
    {	
      GFL_CLUNIT	*p_clunit	= MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
      OBJ_Init( &p_wk->obj, p_clunit, HEAPID_MYSTERYGIFT );
    }

    p_wk->p_text  = MYSTERY_TEXT_Init( BG_FRAME_M_TEXT, PLT_BG_FONT_M, p_wk->p_que, p_wk->p_font, HEAPID_MYSTERYGIFT );
    MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, BG_CGX_OFS_M_TEXT, PLT_BG_TEXT_M );

    MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, TRUE );
    *p_seq  = SEQ_PROC_WAIT;
    break;

  case SEQ_PROC_WAIT:
    *p_seq  = SEQ_FADEOUT_START;
    break;

  case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_END;
		}
		break;

  case SEQ_END:
    if( p_wk->p_wifilogin_param->result == WIFILOGIN_RESULT_LOGIN )
    { 
      //����
      MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_RecvGift );
    }
    else
    { 
      //�L�����Z��
      MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    }
    GFL_HEAP_FreeMemory( p_wk->p_wifilogin_param );
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ؒf������PROC�I��
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DisConnectEnd( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_WAIT,
    SEQ_END,
  };
  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  //�G���[�`�F�b�N
  switch( MYSTERY_NET_GetErrorRepairType( p_wk->p_net ) )
  { 
  case MYSTERY_NET_ERROR_REPAIR_RETURN:      //�P�O�̑I�����܂Ŗ߂�
  case MYSTERY_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf����
    MYSTERY_NET_ClearError( p_wk->p_net );

    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
    break;
  }

  switch( *p_seq )
  { 
  case SEQ_INIT:
    switch( p_wk->mode )
    { 
    case MYSTERY_NET_MODE_WIRELESS:
      break;

    case MYSTERY_NET_MODE_WIFI:
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_LOGOUT_WIFI );
      break;

    case MYSTERY_NET_MODE_IRC:
      break;
    }
    *p_seq  = SEQ_WAIT;
    break;

  case SEQ_WAIT:
    if( MYSTERY_NET_GetState( p_wk->p_net ) == MYSTERY_NET_STATE_WAIT )
    { 
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief	�ؒf���������j���[��
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DisConnectReturn( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_WAIT,
    SEQ_END,
  };
  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    switch( p_wk->mode )
    { 
    case MYSTERY_NET_MODE_WIRELESS:
      break;

    case MYSTERY_NET_MODE_WIFI:
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_LOGOUT_WIFI );
      break;

    case MYSTERY_NET_MODE_IRC:
      break;
    }
    *p_seq  = SEQ_WAIT;
    break;

  case SEQ_WAIT:
    if( MYSTERY_NET_GetState( p_wk->p_net ) == MYSTERY_NET_STATE_WAIT )
    { 
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief	�I������
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  UTIL_DeleteList( p_wk );
  UTIL_DeleteMenu( p_wk );
  UTIL_DeleteGuideText( p_wk );

  if( p_wk->p_card )
  { 
    MYSTERY_CARD_Exit( p_wk->p_card );
  }

  //�I��
  MYSTERY_SEQ_End( p_seqwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�y�A�����^���R���g���[���Ń��[�U�[�쐬�R���e���c�̑���M�����ۂ��Ă����ꍇ
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RestrictUGC( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG,
    SEQ_WAIT_MSG,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_MSG:
    //@TODO
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_err_msg, msg_common_wireless_off_keywait, MYSTERY_TEXT_TYPE_STREAM );

    *p_seq  = SEQ_WAIT_MSG;
    break;
  
  case SEQ_WAIT_MSG:
    if( MYSTERY_TEXT_IsEndPrint(p_wk->p_text) )
    {
      MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	DS�̖����ʐM�ݒ�ŒʐM���ׂɂ��Ă����ꍇ
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_EnableWireless( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG,
    SEQ_WAIT_MSG,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_MSG:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_err_msg, msg_common_wireless_off_keywait, MYSTERY_TEXT_TYPE_STREAM );

    *p_seq  = SEQ_WAIT_MSG;
    break;
  
  case SEQ_WAIT_MSG:
    if( MYSTERY_TEXT_IsEndPrint(p_wk->p_text) )
    {
      MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    }
    break;
  }
}

//=============================================================================
/**
 *      UTIL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�쐬
 *
 *	@param	MYSTERY_WORK *p_wk    ���[�N
 *	@param	heapID                �q�[�vID
 */
//-----------------------------------------------------------------------------
static void UTIL_CreateList( MYSTERY_WORK *p_wk, UTIL_LIST_TYPE type, HEAPID heapID )
{ 
  if( p_wk->p_list == NULL )
  { 
    MYSTERY_LIST_SETUP setup;
    GFL_STD_MemClear( &setup, sizeof(MYSTERY_LIST_SETUP) );
    setup.p_msg   = p_wk->p_msg;
    setup.p_font  = p_wk->p_font;
    setup.p_que   = p_wk->p_que;
    setup.strID[0]= syachi_mystery_01_002_yes;
    setup.strID[1]= syachi_mystery_01_002_no;
    setup.list_max= 2;
    setup.frm     = BG_FRAME_M_LIST;
    setup.font_plt= PLT_BG_FONT_M;
    setup.frm_plt = PLT_BG_TEXT_M;
    setup.frm_chr = BG_CGX_OFS_M_LIST;
    p_wk->p_list  = MYSTERY_LIST_Init( &setup, heapID ); 
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�j��
 *
 *	@param	MYSTERY_WORK *p_wk    ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_DeleteList( MYSTERY_WORK *p_wk )
{ 
  if( p_wk->p_list )
  { 
    MYSTERY_LIST_Exit( p_wk->p_list );
    p_wk->p_list  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���j���[�쐬
 *
 *	@param	MYSTERY_WORK *p_wk    ���[�N
 *	@param	heapID                �q�[�vID
 */
//-----------------------------------------------------------------------------
static void UTIL_CreateMenu( MYSTERY_WORK *p_wk, UTIL_MENU_TYPE type, HEAPID heapID )
{ 
 //���j���[�쐬
  if( p_wk->p_menu == NULL )
  { 
    MYSTERY_MENU_SETUP setup;
    GFL_STD_MemClear( &setup, sizeof(MYSTERY_MENU_SETUP) );
    setup.p_font  = p_wk->p_font;
    setup.p_que   = p_wk->p_que;
    setup.p_cursor= OBJ_GetClwk( &p_wk->obj, OBJ_CLWKID_CURSOR );
    setup.font_frm     = BG_FRAME_M_TEXT;
    setup.font_plt= PLT_BG_FONT_M;
    setup.bg_frm = BG_FRAME_M_BACK1;
    setup.bg_ofs = PLT_BG_RECV_M;

    switch( type )
    { 
    case UTIL_MENU_TYPE_TOP:
      setup.p_msg   = p_wk->p_msg;
      setup.strID[0]= syachi_mystery_menu_001;
      setup.strID[1]= syachi_mystery_menu_002;
      setup.strID[2]= syachi_mystery_menu_015;
      setup.strID[3]= syachi_mystery_menu_003;
      setup.list_max= 4;
      break;

    case UTIL_MENU_TYPE_NETMODE:
      setup.p_msg   = p_wk->p_msg;
      setup.strID[0]= syachi_mystery_menu_004;
      setup.strID[1]= syachi_mystery_menu_005;
      setup.strID[2]= syachi_mystery_menu_006;
      setup.strID[3]= syachi_mystery_menu_007;
      setup.list_max= 4;
      break;
      
    case UTIL_MENU_TYPE_GIFT:
      setup.p_msg   = NULL;
      setup.p_strbuf[0] = GFL_STR_CreateBuffer( GIFT_DATA_CARD_TITLE_MAX+1, heapID );
      GFL_STR_SetStringCodeOrderLength( setup.p_strbuf[0], p_wk->data.data.event_name, GIFT_DATA_CARD_TITLE_MAX );
      setup.list_max    = 1;
      break;

    case UTIL_MENU_TYPE_INFO:
      setup.p_msg   = p_wk->p_msg;
      setup.strID[0]= syachi_mystery_menu_011;
      setup.strID[1]= syachi_mystery_menu_012;
      setup.strID[2]= syachi_mystery_menu_013;
      setup.strID[3]= syachi_mystery_menu_014;
      setup.list_max= 4;
      break;
    }

    //���j���[�쐬
    p_wk->p_menu  = MYSTERY_MENU_Init( &setup, heapID );

    //���j���[�֕�����o�b�t�@��n���Ă�����J��
    if( setup.p_msg == NULL )
    { 
      int i;
      for( i  = 0; i < MYSTERY_MENU_WINDOW_MAX; i++ )
      {
        if( setup.p_strbuf[i] )
        { 
          GFL_STR_DeleteBuffer( setup.p_strbuf[i] );
        }
      }
    }

    //BG�ݒ�
    { 
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
      GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fusigi_bg_00_NCGR, 
          BG_FRAME_M_BACK1, 0, 0, FALSE, heapID );

      if( type == UTIL_MENU_TYPE_GIFT )
      { 
        GFL_ARC_UTIL_TransVramScreen( ARCID_MYSTERY, NARC_mystery_fushigi_top02_NSCR,
            BG_FRAME_M_BACK1, 0, 0, FALSE, heapID );
      }
      else
      { 
        GFL_ARC_UTIL_TransVramScreen( ARCID_MYSTERY, NARC_mystery_fushigi_top_NSCR,
            BG_FRAME_M_BACK1, 0, 0, FALSE, heapID );
      }
      GFL_ARC_CloseDataHandle( p_handle );

      GFL_BG_ChangeScreenPalette( BG_FRAME_M_BACK1, 0, 0, 32, 24, PLT_BG_RECV_M );

      GFL_BG_LoadScreenReq( BG_FRAME_M_BACK1 );
      GFL_BG_SetVisible( BG_FRAME_M_BACK1, TRUE );
    }

    //�A���t�@�ݒ�
    { 
      G2_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG2,
          GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
          MYSTERY_MENU_ALPHA_EV1,
          MYSTERY_MENU_ALPHA_EV2
          );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���j���[�j��
 *
 *	@param	MYSTERY_WORK *p_wk    ���[�N
 *	@param	heapID                �q�[�vID
 */
//-----------------------------------------------------------------------------
static void UTIL_DeleteMenu( MYSTERY_WORK *p_wk )
{
  if(p_wk->p_menu)
  {
    G2_BlendNone();

    GFL_BG_ClearScreen( BG_FRAME_M_BACK1 );
    GFL_BG_LoadScreenReq( BG_FRAME_M_BACK1 );
    GFL_BG_SetVisible( BG_FRAME_M_BACK1, FALSE );

    MYSTERY_MENU_Exit( p_wk->p_menu );
    p_wk->p_menu  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �K�C�h�p�e�L�X�g��  �쐬
 *
 *	@param	MYSTERY_WORK *p_wk
 *	@param	heapID 
 */
//-----------------------------------------------------------------------------
static void UTIL_CreateGuideText( MYSTERY_WORK *p_wk, HEAPID heapID )
{ 
  if( p_wk->p_winset_s == NULL )
  { 
    MYSTERY_MSGWINSET_SETUP_TBL tbl[] =
    { 
      //�^�C�g��
      { 
        1,2,30,2, 0, NULL, MYSTERY_MSGWIN_POS_WH_CENTER, 0, 0, MYSTERY_MSGWIN_WHITE_COLOR,
      },
      //�{��
      {
        2,7,28,13, 0, NULL, MYSTERY_MSGWIN_POS_ABSOLUTE, 0, 0, MYSTERY_MSGWIN_WHITE_COLOR,
      },
    };
    tbl[0].p_strbuf = GFL_STR_CreateBuffer( GIFT_DATA_CARD_TITLE_MAX+1, heapID );
    GFL_STR_SetStringCodeOrderLength( tbl[0].p_strbuf, p_wk->data.data.event_name, GIFT_DATA_CARD_TITLE_MAX ); 

    tbl[1].p_strbuf = GFL_STR_CreateBuffer( GIFT_DATA_CARD_TEXT_MAX+1, heapID );
    GFL_STR_SetStringCodeOrderLength( tbl[1].p_strbuf, p_wk->data.event_text, GIFT_DATA_CARD_TEXT_MAX ); 

    p_wk->p_winset_s  = MYSTERY_MSGWINSET_Init( tbl, NELEMS(tbl), BG_FRAME_S_TEXT, PLT_BG_FONT_S, p_wk->p_que, p_wk->p_msg, p_wk->p_font, heapID );

    GFL_STR_DeleteBuffer( tbl[0].p_strbuf );
    GFL_STR_DeleteBuffer( tbl[1].p_strbuf );

    BG_Load( &p_wk->bg, BG_LOAD_TYPE_GUIDE_S ); 
    
 //   GFL_BG_ChangeScreenPalette( BG_FRAME_S_BACK1, 0, 0, 32, 23, PLT_BG_CARD_S );
    GFL_BG_LoadScreenReq( BG_FRAME_S_BACK1 );

    GFL_BG_SetVisible( BG_FRAME_S_BACK1, TRUE );

    //�A���t�@�ݒ�
    { 
      G2S_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG2,
          GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
          MYSTERY_MENU_ALPHA_EV1,
          MYSTERY_MENU_ALPHA_EV2
          );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �K�C�h�p�e�L�X�g��  �j��
 *
 *	@param	MYSTERY_WORK *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void UTIL_DeleteGuideText( MYSTERY_WORK *p_wk )
{ 
  if( p_wk->p_winset_s )
  { 
    G2S_BlendNone();

   MYSTERY_MSGWINSET_Exit( p_wk->p_winset_s );
   p_wk->p_winset_s = NULL;

   GFL_BG_LoadScreenReq( BG_FRAME_S_TEXT );
   GFL_BG_SetVisible( BG_FRAME_S_BACK1, FALSE );
  }
}
//=============================================================================
/**
 *      �f��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ��M�f��  ������
 *
 *	@param	MYSTERY_DEMO_WORK *p_wk ���[�N
 *	@param	*p_unit                 CLUNIT
 *	@param  cp_data                 �f�[�^
 *	@param	heapID                  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_DEMO_Init( MYSTERY_DEMO_WORK *p_wk, GFL_CLUNIT *p_unit, const DOWNLOAD_GIFT_DATA *cp_data, GAMEDATA *p_gamedata, HEAPID heapID )
{
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_DEMO_WORK) );

	//���\�[�X�ǂ݂���
  switch( cp_data->data.gift_type )
  { 
  case MYSTERYGIFT_TYPE_POKEMON:
    {	
      const GIFT_PRESENT_POKEMON  *cp_pokemon = &cp_data->data.data.pokemon;

      POKEMON_PARAM* p_pp = MYSTERY_CreatePokemon( &cp_data->data, GFL_HEAP_LOWID(heapID), p_gamedata );
      ARCHANDLE		*p_handle	= POKE2DGRA_OpenHandle( heapID );

      p_wk->res_plt	= POKE2DGRA_OBJ_PLTT_Register( p_handle, cp_pokemon->mons_no, cp_pokemon->form_no, cp_pokemon->sex, cp_pokemon->rare == 2, POKEGRA_DIR_FRONT, cp_pokemon->egg, CLSYS_DRAW_MAIN, PLT_OBJ_GIFT_M * 0x20, heapID );
 
      p_wk->res_cel	= POKE2DGRA_OBJ_CELLANM_Register( cp_pokemon->mons_no, cp_pokemon->form_no, cp_pokemon->sex, cp_pokemon->rare == 1, POKEGRA_DIR_FRONT, cp_pokemon->egg, APP_COMMON_MAPPING_128K, CLSYS_DRAW_MAIN, heapID );
        
      p_wk->res_cgx	= POKE2DGRA_OBJ_CGR_RegisterPPP( p_handle, PP_GetPPPPointerConst(p_pp),
          POKEGRA_DIR_FRONT, CLSYS_DRAW_MAIN, heapID );
 
      GFL_ARC_CloseDataHandle( p_handle );
      GFL_HEAP_FreeMemory( p_pp );
    }
    break;

  case MYSTERYGIFT_TYPE_ITEM:
    {	
      const GIFT_PRESENT_ITEM  *cp_item = &cp_data->data.data.item;
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_ITEMICON, heapID );

      p_wk->res_plt	= GFL_CLGRP_PLTT_Register( p_handle, 
          ITEM_GetIndex( cp_item->itemNo, ITEM_GET_ICON_PAL ), CLSYS_DRAW_MAIN, PLT_OBJ_GIFT_M*0x20, heapID );

      p_wk->res_cel	= GFL_CLGRP_CELLANIM_Register( p_handle,
          NARC_item_icon_itemicon_NCER, NARC_item_icon_itemicon_NANR, heapID );

      p_wk->res_cgx	= GFL_CLGRP_CGR_Register( p_handle,
          ITEM_GetIndex( cp_item->itemNo, ITEM_GET_ICON_CGX ), FALSE, CLSYS_DRAW_MAIN, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
    }
    break;

  case MYSTERYGIFT_TYPE_RULE:
    /* fallthrow */
  case MYSTERYGIFT_TYPE_NUTSET:
    {	
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );

      p_wk->res_plt	= GFL_CLGRP_PLTT_Register( p_handle, 
          NARC_mystery_fushigi_box_NCLR, CLSYS_DRAW_MAIN, PLT_OBJ_GIFT_M*0x20, heapID );

      p_wk->res_cel	= GFL_CLGRP_CELLANIM_Register( p_handle,
          NARC_mystery_fushigi_box_NCER, NARC_mystery_fushigi_box_NANR, heapID );

      p_wk->res_cgx	= GFL_CLGRP_CGR_Register( p_handle,
          NARC_mystery_fushigi_box_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
    }
    break;

  default:
    GF_ASSERT( 0 );
  }

	//CLWK�o�^
	{
		int i;
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x  = MYSTERY_DEMO_GIFT_X;
    cldata.pos_y  = MYSTERY_DEMO_MOVE_GIFT_START_Y;
    cldata.bgpri  = BG_FRAME_M_BACK1;

		p_wk->p_clwk	=	GFL_CLACT_WK_Create( p_unit,
				p_wk->res_cgx,
				p_wk->res_plt,
				p_wk->res_cel,
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��M�f��  �j��
 *
 *	@param	MYSTERY_DEMO_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MYSTERY_DEMO_Exit( MYSTERY_DEMO_WORK *p_wk )
{ 

  GFL_BG_SetVisible( BG_FRAME_M_BACK1, FALSE ); 

	//CLWK�j��
	{	
    GFL_CLACT_WK_Remove( p_wk->p_clwk );
	}

	//���\�[�X�j��
	{
    GFL_CLGRP_PLTT_Release( p_wk->res_plt );
    GFL_CLGRP_CGR_Release( p_wk->res_cgx );
    GFL_CLGRP_CELLANIM_Release( p_wk->res_cel );
	}

  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_DEMO_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��M�f��  ���C������
 *
 *	@param	MYSTERY_DEMO_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MYSTERY_DEMO_Main( MYSTERY_DEMO_WORK *p_wk, BG_WORK *p_bg )
{
  enum
  { 
    SEQ_INIT,
    SEQ_START_FADEIN,
    SEQ_WAIT_FADEIN,
    SEQ_INIT_WAIT,
    SEQ_MOVE,
    SEQ_END_WAIT,
    SEQ_END,
  };

  switch( p_wk->seq )
  {
  case SEQ_INIT:
    GFL_BG_SetVisible( BG_FRAME_M_BACK1, FALSE );
    BG_Load( p_bg, BG_LOAD_TYPE_STAGE_M );
    p_wk->seq = SEQ_START_FADEIN;
    break;

  case SEQ_START_FADEIN:
    G2_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG2,
          GX_BLEND_PLANEMASK_BG3,
          0,
          16
          );
    GFL_BG_SetVisible( BG_FRAME_M_BACK1, TRUE );
    p_wk->seq = SEQ_WAIT_FADEIN;
    break;

  case SEQ_WAIT_FADEIN:
    {
      s16 ev1 = 0 + 16 * p_wk->sync / MYSTERY_DEMO_STAGE_FADE_SYNC;
      s16 ev2 = 16 - 16 * p_wk->sync / MYSTERY_DEMO_STAGE_FADE_SYNC;
      G2_ChangeBlendAlpha( ev1, ev2 );
      if( p_wk->sync++ > MYSTERY_DEMO_STAGE_FADE_SYNC )
      { 
        G2_BlendNone();
        p_wk->sync  = 0;
        p_wk->seq = SEQ_INIT_WAIT;
      }
    }
    break;

  case SEQ_INIT_WAIT:
    if( p_wk->sync++ > MYSTERY_DEMO_INIT_WAIT_SYNC )
    { 
      p_wk->sync  = 0;
      p_wk->seq   = SEQ_MOVE;
    }
    break;

  case SEQ_MOVE:
    { 
      s16 pos;

      pos = MYSTERY_DEMO_MOVE_GIFT_START_Y + MYSTERY_DEMO_MOVE_GIFT_DIFF_Y * p_wk->sync / MYSTERY_DEMO_MOVE_GIFT_SYNC;
      GFL_CLACT_WK_SetTypePos( p_wk->p_clwk, pos, CLSYS_DEFREND_MAIN, CLSYS_MAT_Y );

      if( p_wk->sync++ >= MYSTERY_DEMO_MOVE_GIFT_SYNC )
      { 
        PMSND_PlaySE( MYSTERY_SE_RECV_PRESENT );

        p_wk->sync  = 0;
        p_wk->seq   = SEQ_END_WAIT;
      }
    }
    break;

  case SEQ_END_WAIT:
    if( p_wk->sync++ > MYSTERY_DEMO_END_WAIT_SYNC )
    { 
      p_wk->sync  = 0;
      p_wk->seq   = SEQ_END;
    }
    break;

  case SEQ_END:
    p_wk->is_end  = TRUE;
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ��M�f��  �I���`�F�b�N
 *
 *	@param	const MYSTERY_DEMO_WORK *cp_wk  ���[�N
 *
 *	@return TRUE�Ńf���I��  FALSE�Ńf����
 */
//-----------------------------------------------------------------------------
static BOOL MYSTERY_DEMO_IsEnd( const MYSTERY_DEMO_WORK *cp_wk )
{ 
  return cp_wk->is_end;
}

//=============================================================================
/**
 *  ���o���[�N
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���o���[�N  ������
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ���[�N
 *	@param	heapID                    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_EFFECT_Init( MYSTERY_EFFECT_WORK *p_wk, const OBJ_WORK *cp_obj, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_EFFECT_WORK) );
  p_wk->scroll_add  = MYSTERY_EFFECT_DEFAULT_SCROLL_SPEED;
  p_wk->cp_obj  = cp_obj;
  p_wk->init_add    = p_wk->scroll_add;

  { 
    int i;
    GFL_CLWK      *p_clwk;
    GFL_CLACTPOS  pos;
    for( i = 0; i < EFFECT_BRIGHT_MAX; i++ )
    { 
      p_clwk  = OBJ_GetClwk( p_wk->cp_obj, OBJ_CLWKID_BRIGHT_TOP+i );
      GFL_CLACT_WK_GetWldPos( p_clwk, &pos );
      p_wk->bright_pos[i] = pos.y << FX32_SHIFT;
      p_wk->bright_add[i] = FX32_CONST(1.6) + GFUser_GetPublicRand0( FX32_CONST(1.0) );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���o���[�N  �j��
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MYSTERY_EFFECT_Exit( MYSTERY_EFFECT_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_EFFECT_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���o���[�N  ���C������
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MYSTERY_EFFECT_Main( MYSTERY_EFFECT_WORK *p_wk )
{ 
  if( p_wk->is_update )
  { 
    if( p_wk->is_start )
    { 
      switch( p_wk->type )
      { 
      case MYSTERY_EFFECT_TYPE_NORMAL:
        if( p_wk->scroll_add != MYSTERY_EFFECT_DEFAULT_SCROLL_SPEED )
        { 
          p_wk->scroll_add  = p_wk->init_add  + (MYSTERY_EFFECT_DEFAULT_SCROLL_SPEED-p_wk->init_add) * p_wk->cnt / MYSTERY_EFFECT_DEMO_SCROLL_CHANGE_SYNC;
          if( p_wk->cnt++ > MYSTERY_EFFECT_DEMO_SCROLL_CHANGE_SYNC )
          { 
            p_wk->scroll_add  = MYSTERY_EFFECT_DEFAULT_SCROLL_SPEED;
          }
        }
        { 
          int i;
          GFL_CLWK      *p_clwk;
          GFL_CLACTPOS  pos;
          for( i = 0; i < EFFECT_BRIGHT_MAX; i++ )
          { 
            p_clwk  = OBJ_GetClwk( p_wk->cp_obj, OBJ_CLWKID_BRIGHT_TOP+i );
            if( GFL_CLACT_WK_GetDrawEnable( p_clwk ) )
            { 
              p_wk->bright_pos[i] += p_wk->bright_add[ i ];

              GFL_CLACT_WK_GetWldPos( p_clwk, &pos );
              pos.y = p_wk->bright_pos[i] >> FX32_SHIFT;
              GFL_CLACT_WK_SetWldPos( p_clwk, &pos );

              if( 0 <= pos.y && pos.y <= 192 + 16 )
              { 
                GFL_CLACT_WK_SetDrawEnable( p_clwk, TRUE );
              }
              else
              { 
                GFL_CLACT_WK_SetDrawEnable( p_clwk, FALSE );
              }
            }
          }
        }
        break;

      case MYSTERY_EFFECT_TYPE_DEMO:
        if( p_wk->scroll_add != MYSTERY_EFFECT_DEMO_SCROLL_SPEED )
        { 
          p_wk->scroll_add  = p_wk->init_add  + (MYSTERY_EFFECT_DEMO_SCROLL_SPEED-p_wk->init_add) * p_wk->cnt / MYSTERY_EFFECT_DEMO_SCROLL_CHANGE_SYNC;
          if( p_wk->cnt++ > MYSTERY_EFFECT_DEMO_SCROLL_CHANGE_SYNC )
          { 
            p_wk->scroll_add  = MYSTERY_EFFECT_DEMO_SCROLL_SPEED;
          }
        }
        { 
          int i;
          GFL_CLWK      *p_clwk;
          GFL_CLACTPOS  pos;
          for( i = 0; i < EFFECT_BRIGHT_MAX; i++ )
          { 
            p_wk->bright_pos[i] += p_wk->bright_add[ i ];
            if( p_wk->bright_pos[i]>= FX32_CONST(192 +16) )
            { 
              p_wk->bright_pos[i] = FX32_CONST( -16 );
            }

            p_clwk  = OBJ_GetClwk( p_wk->cp_obj, OBJ_CLWKID_BRIGHT_TOP+i );
            GFL_CLACT_WK_GetWldPos( p_clwk, &pos );
            pos.y = p_wk->bright_pos[i] >> FX32_SHIFT;
            GFL_CLACT_WK_SetWldPos( p_clwk, &pos );

            if( 0 <= pos.y && pos.y <= 192 + 16 )
            { 
              GFL_CLACT_WK_SetDrawEnable( p_clwk, TRUE );
            }
            else
            { 
              GFL_CLACT_WK_SetDrawEnable( p_clwk, FALSE );
            }
          }
        }
        break;
      }

      p_wk->scroll_now  += p_wk->scroll_add;
      GFL_BG_SetScrollReq( BG_FRAME_M_BACK2, GFL_BG_SCROLL_Y_SET, p_wk->scroll_now >> FX32_SHIFT );
      GFL_BG_SetScrollReq( BG_FRAME_S_BACK2, GFL_BG_SCROLL_Y_SET, p_wk->scroll_now >> FX32_SHIFT );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���o���[�N  �J�n
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ���[�N
 *	@param	type                      �J�n���
 */
//-----------------------------------------------------------------------------
static void MYSTERY_EFFECT_Start( MYSTERY_EFFECT_WORK *p_wk, MYSTERY_EFFECT_TYPE type )
{
  p_wk->type      = type;
  p_wk->is_start  = TRUE;
  p_wk->cnt       = 0;
  p_wk->init_add  = p_wk->scroll_add;

  switch( p_wk->type )
  { 
  case MYSTERY_EFFECT_TYPE_NORMAL:
    break;

  case MYSTERY_EFFECT_TYPE_DEMO:

    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���o���[�N  �A�b�v�f�[�g�ݒ�
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ���[�N
 *	@param	is_update TRUE�œ���  FALSE�Œ�~
 */
//-----------------------------------------------------------------------------
static void MYSTERY_EFFECT_SetUpdateFlag( MYSTERY_EFFECT_WORK *p_wk, BOOL is_update )
{ 
  p_wk->is_update  = is_update;
}
//=============================================================================
/**
 *    �ڂ���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �{�^��������
 *
 *	@param	MYSTERY_BTN_WORK *p_wk  ���[�N
 *	@param	heapID                  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_BTN_Init( MYSTERY_BTN_WORK *p_wk, const OBJ_WORK *cp_obj, GFL_CLUNIT *p_clunit, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_BTN_WORK) );
  p_wk->cp_obj  = cp_obj;

  p_wk->p_bmpoam_sys  = BmpOam_Init( heapID, p_clunit );

  { 
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x  = 128 - 32;
    cldata.pos_y  = 177 - 8;
    cldata.softpri  = 0;
    cldata.bgpri    = 0;

    p_wk->p_msgoam  = MYSTERY_MSGOAM_Init( &cldata, 8, 2, cp_obj->res_plt[OBJ_RES_PLT_CURSOR], 3, CLSYS_DRAW_MAIN, p_wk->p_bmpoam_sys, p_que, heapID );
    MYSTERY_MSGOAM_SetStrColor( p_wk->p_msgoam, PRINTSYS_LSB_Make( 1, 3, 0) );
    MYSTERY_MSGOAM_SetStrPos( p_wk->p_msgoam, 0, 0, MYSTERY_MSGOAM_POS_WH_CENTER );
    MYSTERY_MSGOAM_Print( p_wk->p_msgoam, p_msg, strID, p_font );
  }

  //OBJ�\��
  { 
    GFL_CLWK  *p_clwk = OBJ_GetClwk( cp_obj, OBJ_CLWKID_BTN );
    GFL_CLACT_WK_SetDrawEnable( p_clwk, TRUE );
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �{�^���j��
 *
 *	@param	MYSTERY_BTN_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MYSTERY_BTN_Exit( MYSTERY_BTN_WORK *p_wk )
{ 
  MYSTERY_MSGOAM_Clear( p_wk->p_msgoam );

  //OBJ�\��
  { 
    GFL_CLWK  *p_clwk = OBJ_GetClwk( p_wk->cp_obj, OBJ_CLWKID_BTN );
    GFL_CLACT_WK_SetDrawEnable( p_clwk, FALSE );
  }

  MYSTERY_MSGOAM_Exit( p_wk->p_msgoam );
  BmpOam_Exit( p_wk->p_bmpoam_sys );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_BTN_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �{�^��  �����`��
 *
 *	@param	MYSTERY_BTN_WORK *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void MYSTERY_BTN_PrintMain( MYSTERY_BTN_WORK *p_wk )
{
  MYSTERY_MSGOAM_PrintMain( p_wk->p_msgoam );
}
