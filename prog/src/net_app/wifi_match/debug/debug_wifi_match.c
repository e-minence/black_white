//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_wifi_match.c
 *	@brief  �T�[�o�[�A�N�Z�X�f�o�b�O
 *	@author	Toru=Nagihashi
 *	@data		2009.12.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//-------------------------------------
///	�C���N���[�h
//=====================================
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/main.h"
#include "system/gfl_use.h"
#include "debug/debug_str_conv.h"
#include "gamesystem/game_data.h"

//�A�[�J�C�u
#include "font/font.naix"
#include "arc_def.h"
#include "message.naix"

//�����̃��W���[��
#include "../wifibattlematch_util.h"
#include "../wifibattlematch_graphic.h"
#include "../wifibattlematch_net.h"
#include "../atlas_syachi2ds_v1.h"

//�O�����J
#include "net_app/debug_wifi_match.h"

//�f�o�b�O
//#include <wchar.h>          //wcslen

//-------------------------------------
///	�I�[�o�[���C
//=====================================
FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(dpw_common);
FS_EXTERN_OVERLAY(wifibattlematch_view);

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define DEBUG_BMPWIN_MAX  (21)
#define DEBUG_DATA_MAX  (21)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	BG�t���[��
//=====================================
enum
{
	//���C�����
	BG_FRAME_M_FONT =	GFL_BG_FRAME0_M, 

	//�T�u���
	BG_FRAME_S_NUM =	GFL_BG_FRAME0_S, 
	BG_FRAME_S_FONT	=	GFL_BG_FRAME1_S,
} ;

//-------------------------------------
///	�p���b�g
//=====================================
enum
{
	//���C�����BG
	PLT_BG_M	  =	0,
	PLT_FONT_M	= 15,

	//�T�u���BG
	PLT_BG_S    = 0,
	PLT_FONT_S	= 15,

	//���C�����OBJ
	
	//�T�u���OBJ
} ;

//-------------------------------------
///	���͑�
//=====================================
typedef struct 
{
  u8  x;
  u8  y;
  u8  w;
  u8  h;
}INPUTWIN_PARAM;

//-------------------------------------
///	��������
//=====================================
typedef struct 
{
  GFL_BMPWIN  *p_bmpwin;
  GFL_FONT    *p_font;
  s32         min;
  s32         max;
  s32         now;
  u8          keta;
  u8          active;
  u8          frm;
  u8          plt;
  STRBUF      *p_strbuf;
} NUMINPUT_WORK;

//-------------------------------------
///	ATLAS�f�o�b�O
//=====================================
typedef struct 
{
  u32         seq;
  u32         pre_seq;
  HEAPID      heapID;
  GFL_BMPWIN  *p_bmpwin[DEBUG_BMPWIN_MAX];
  u32         recv_data[DEBUG_DATA_MAX];
  u32         send_data[2][DEBUG_DATA_MAX];
  BOOL        is_auth;

  WIFIBATTLEMATCH_NET_WORK  *p_net;
  WIFIBATTLEMATCH_GDB_RND_SCORE_DATA  score;
  WIFIBATTLEMATCH_SC_DEBUG_DATA       report;
  WBM_TEXT_WORK             *p_text;
  GFL_FONT                  *p_font;
  GFL_FONT                  *p_small_font;
  PRINT_QUE                 *p_que;
  NUMINPUT_WORK             numinput;
  u32                       select;
  STRBUF                    *p_strbuf;
  BOOL                      is_you;
  SAVE_CONTROL_WORK         *p_save;
} DEBUG_ATLAS_WORK;

//-------------------------------------
///	SAKE�f�o�b�O
//=====================================
typedef struct 
{
  u32         seq;
  HEAPID      heapID;
  GFL_BMPWIN  *p_bmpwin[DEBUG_BMPWIN_MAX];
  u32         data[DEBUG_DATA_MAX];
  WIFIBATTLEMATCH_NET_WORK  *p_net;
  WIFIBATTLEMATCH_GDB_RND_SCORE_DATA  score;
  WBM_TEXT_WORK             *p_text;
  GFL_FONT                  *p_font;
  GFL_FONT                  *p_small_font;
  PRINT_QUE                 *p_que;
  NUMINPUT_WORK             numinput;
  u32                       select;
  STRBUF                    *p_strbuf;
  SAVE_CONTROL_WORK         *p_save;
  s64                       logindate;
} DEBUG_SAKE_WORK;

//-------------------------------------
///	���[�N
//=====================================
typedef struct 
{
  //�O���t�B�b�N�ݒ�
	WIFIBATTLEMATCH_GRAPHIC_WORK	*p_graphic;

  //SAKE
  DEBUG_SAKE_WORK               sake;

  //ATLAS
  DEBUG_ATLAS_WORK              atlas;

} WIFIBATTLEMATCH_DEBUG_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_DEBUG_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_DEBUG_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_DEBUG_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	�E�B���h�E�f�[�^
//=====================================
static void INPUTWIN_GetRange( u32 key, INPUTWIN_PARAM *p_wk );

//-------------------------------------
///	SAKE
//=====================================
static void SAKE_Init( DEBUG_SAKE_WORK *p_wk, GAMEDATA *p_gamedata, HEAPID heapID );
static void SAKE_Exit( DEBUG_SAKE_WORK *p_wk );
static BOOL SAKE_Main( DEBUG_SAKE_WORK *p_wk );
static void Sake_CreateDisplay( DEBUG_SAKE_WORK *p_wk, HEAPID heapID );
static void Sake_DeleteDisplay( DEBUG_SAKE_WORK *p_wk );
static void Sake_UpdateDisplay( DEBUG_SAKE_WORK *p_wk );

//-------------------------------------
///	ATLAS
//=====================================
static void ATLAS_Init( DEBUG_ATLAS_WORK *p_wk, GAMEDATA *p_gamedata, HEAPID heapID );
static void ATLAS_Exit( DEBUG_ATLAS_WORK *p_wk );
static BOOL ATLAS_Main( DEBUG_ATLAS_WORK *p_wk );
static void Atlas_CreateRecvDisplay( DEBUG_ATLAS_WORK *p_wk, HEAPID heapID );
static void Atlas_DeleteRecvDisplay( DEBUG_ATLAS_WORK *p_wk );
static void Atlas_UpdateRecvDisplay( DEBUG_ATLAS_WORK *p_wk );
static void Atlas_CreateReportDisplay( DEBUG_ATLAS_WORK *p_wk, HEAPID heapID );
static void Atlas_DeleteReportDisplay( DEBUG_ATLAS_WORK *p_wk );
static void Atlas_UpdateReportDisplay( DEBUG_ATLAS_WORK *p_wk );

//-------------------------------------
///	NUMINPUT
//=====================================
static void NUMINPUT_Init( NUMINPUT_WORK *p_wk, u8 frm, u8 plt, GFL_FONT *p_font, HEAPID heapID );
static void NUMINPUT_Exit( NUMINPUT_WORK *p_wk );
static void NUMINPUT_Start( NUMINPUT_WORK *p_wk, u8 keta, s32 min, s32 max, s32 now, u8 x, u8 y );
static BOOL NUMINPUT_Main( NUMINPUT_WORK *p_wk );
static u16  NUMINPUT_GetNum( const NUMINPUT_WORK *cp_wk );
static u32 NumInput_AddKeta( u32 now, u32 keta, s8 dir );
static void NumInput_Print( NUMINPUT_WORK *p_wk );

//-------------------------------------
///	etc
//=====================================
static void PRINT_GetStrWithNumber( STRBUF *p_dst, const char *cp_src, ... );
static BOOL BMPWINIsCollisionRxP( const GFL_BMPWIN *cp_bmpwin, u32 x, u32 y );
static u8 GetFig( u32 now, u8 keta );


//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X�f�[�^
//=====================================
const GFL_PROC_DATA	DEBUG_WifiBattleMatch_ProcData =
{ 
  WIFIBATTLEMATCH_DEBUG_PROC_Init,
  WIFIBATTLEMATCH_DEBUG_PROC_Main,
  WIFIBATTLEMATCH_DEBUG_PROC_Exit,
};

//=============================================================================
/**
 *          �f�[�^
 */
//=============================================================================
//-------------------------------------
///	�L�[��
//=====================================
static const char *sc_key_name[] =
{ 
  "���傤�͂� %d",
  "�V���O������ %d",
  "�V���O���܂� %d",
  "�V���O�����[�g %d",
  "�_�u������ %d",
  "�_�u���܂� %d",
  "�_�u�����[�g %d",
  "�g���v������ %d",
  "�g���v���܂� %d",
  "�g���v�����[�g %d",
  "���[�e���� %d",
  "���[�e�܂� %d",
  "���[�e���[�g %d",
  "�V���[�^���� %d",
  "�V���[�^�܂� %d",
  "�V���[�^���[�g %d",
  "�ӂ����J�E���^ %d",
  "�����������|�[�g %d",
};
//-------------------------------------
///	�X�^�b�g��
//=====================================
static const char *sc_stat_name[]  =
{ 
  "�V���O������ %d",
  "�V���O���܂� %d",
  "�V���O�����[�g %d",
  "�_�u������ %d",
  "�_�u���܂� %d",
  "�_�u�����[�g %d",
  "�g���v������ %d",
  "�g���v���܂� %d",
  "�g���v�����[�g %d",
  "���[�e���� %d",
  "���[�e�܂� %d",
  "���[�e���[�g %d",
  "�V���[�^���� %d",
  "�V���[�^�܂� %d",
  "�V���[�^���[�g %d",
  "������J�E���^ %d",
  "�ӂ����J�E���^ %d",
  "���O�C�������� %d",
};

//=============================================================================
/**
 *          �v���Z�X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�O�v���Z�X  ������
 *
 *	@param	GFL_PROC *p_proc  �v���Z�X
 *	@param	*p_seq            �V�[�P���X
 *	@param	*p_param_adrs     ����
 *	@param	*p_wk_adrs        ���[�N
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_DEBUG_PROC_Init
  ( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  DEBUG_WIFIBATTLEMATCH_PARAM *p_param  = p_param_adrs;
  WIFIBATTLEMATCH_DEBUG_WORK  *p_wk;

  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));
	GFL_OVERLAY_Load( FS_OVERLAY_ID(wifibattlematch_view));

  //�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIMATCH_DEBUG, 0x30000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_DEBUG_WORK), HEAPID_WIFIMATCH_DEBUG );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_DEBUG_WORK) );	

  //�O���t�B�b�N�ݒ�
	p_wk->p_graphic	= WIFIBATTLEMATCH_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_WIFIMATCH_DEBUG );


	//�ǂ݂���
	{	
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr,
				PALTYPE_MAIN_BG, PLT_FONT_M*0x20, 0, HEAPID_WIFIMATCH_DEBUG );
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr,
				PALTYPE_SUB_BG, PLT_FONT_S*0x20, 0, HEAPID_WIFIMATCH_DEBUG );
	}

  if( p_param->mode == DEBUG_WIFIBATTLEMATCH_MODE_ATLAS )
  { 
    ATLAS_Init( &p_wk->atlas, p_param->p_gamedata, HEAPID_WIFIMATCH_DEBUG );
  }
  else if( p_param->mode == DEBUG_WIFIBATTLEMATCH_MODE_SAKE )
  { 
    SAKE_Init( &p_wk->sake, p_param->p_gamedata, HEAPID_WIFIMATCH_DEBUG );
  }


  //�f�o�b�O�Ȃ̂Ńt�F�[�h�͊ȕւ��Ă��炢�܂�
  GX_SetMasterBrightness( 0 );
	GXS_SetMasterBrightness( 0 );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�O�v���Z�X  �j��
 *
 *	@param	GFL_PROC *p_proc  �v���Z�X
 *	@param	*p_seq            �V�[�P���X
 *	@param	*p_param_adrs     ����
 *	@param	*p_wk_adrs        ���[�N
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_DEBUG_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  DEBUG_WIFIBATTLEMATCH_PARAM *p_param  = p_param_adrs;
  WIFIBATTLEMATCH_DEBUG_WORK  *p_wk = p_wk_adrs;

  //�f�o�b�O�Ȃ̂Ńt�F�[�h�͊ȕւ��Ă��炢�܂�
  GX_SetMasterBrightness( 16 );
	GXS_SetMasterBrightness( 16 );

  if( p_param->mode == DEBUG_WIFIBATTLEMATCH_MODE_ATLAS )
  { 
    ATLAS_Exit( &p_wk->atlas );
  }
  else if( p_param->mode == DEBUG_WIFIBATTLEMATCH_MODE_SAKE )
  { 
    SAKE_Exit( &p_wk->sake );
  }


  //�O���t�B�b�N�j��
	WIFIBATTLEMATCH_GRAPHIC_Exit( p_wk->p_graphic );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_WIFIMATCH_DEBUG );
	
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(wifibattlematch_view));
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(dpw_common));
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�O�v���Z�X  ���C������
 *
 *	@param	GFL_PROC *p_proc  �v���Z�X
 *	@param	*p_seq            �V�[�P���X
 *	@param	*p_param_adrs     ����
 *	@param	*p_wk_adrs        ���[�N
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_DEBUG_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  DEBUG_WIFIBATTLEMATCH_PARAM *p_param  = p_param_adrs;
  WIFIBATTLEMATCH_DEBUG_WORK  *p_wk = p_wk_adrs;
  BOOL ret;

  if( p_param->mode == DEBUG_WIFIBATTLEMATCH_MODE_ATLAS )
  { 
    ret = ATLAS_Main( &p_wk->atlas );
  }
  else if( p_param->mode == DEBUG_WIFIBATTLEMATCH_MODE_SAKE )
  { 
    ret = SAKE_Main( &p_wk->sake );
  }

  if( ret )
  { 
    return GFL_PROC_RES_FINISH;
  }

	return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN�̃T�C�Y�擾
 *
 *	@param	u32 key �L�[
 *	@param	*p_wk   �擾���[�N
 */
//-----------------------------------------------------------------------------
static void INPUTWIN_GetRange( u32 key, INPUTWIN_PARAM *p_wk )
{ 
  const u8 w  = 15;
  const u8 h  = 2;
  const u8 padding_w  = 1;
  const u8 padding_h  = 0;

  u16 x;

  x = key * (w + padding_w);

  p_wk->x =  (x) % 32;
  p_wk->y =  ((x) / 32) * (h + padding_h);

  p_wk->w = w;
  p_wk->h = h;

}

//=============================================================================
/**
 *    SAKE
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �T�P�f�o�b�O  ������
 *
 *	@param	DEBUG_SAKE_WORK *p_wk ���[�N
 *	@param	*p_user_data  ���[�U�[�f�[�^
 *	@param	heapID �q�[�v
 */
//-----------------------------------------------------------------------------
static void SAKE_Init( DEBUG_SAKE_WORK *p_wk, GAMEDATA *p_gamedata, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(DEBUG_SAKE_WORK) );
  p_wk->heapID    = heapID;
  p_wk->p_save    = GAMEDATA_GetSaveControlWork( p_gamedata );
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
	p_wk->p_small_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_small_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
  p_wk->p_strbuf  = GFL_STR_CreateBuffer( 255, heapID );
  p_wk->p_que     = PRINTSYS_QUE_Create( heapID );
  p_wk->p_text  = WBM_TEXT_Init( BG_FRAME_M_FONT, PLT_FONT_M, 0, 0, p_wk->p_que, p_wk->p_font, heapID );
  Sake_CreateDisplay( p_wk, heapID );
  p_wk->p_net = WIFIBATTLEMATCH_NET_Init( 0, p_gamedata, NULL, heapID );

  NUMINPUT_Init( &p_wk->numinput, BG_FRAME_S_NUM, PLT_FONT_S, p_wk->p_small_font, heapID );
  GFL_BG_SetVisible( BG_FRAME_S_NUM, FALSE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �T�P�f�o�b�O  �j��
 *
 *	@param	DEBUG_SAKE_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SAKE_Exit( DEBUG_SAKE_WORK *p_wk )
{ 
  NUMINPUT_Exit( &p_wk->numinput );

  GFL_STR_DeleteBuffer( p_wk->p_strbuf );
  WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
  Sake_DeleteDisplay( p_wk );
  WBM_TEXT_Main( p_wk->p_text );
	GFL_FONT_Delete( p_wk->p_font );
	GFL_FONT_Delete( p_wk->p_small_font );
  GFL_STD_MemClear( p_wk, sizeof(DEBUG_SAKE_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �T�P�f�o�b�O  ���C������
 *
 *	@param	DEBUG_SAKE_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static BOOL SAKE_Main( DEBUG_SAKE_WORK *p_wk )
{ 
  enum
  { 
    SEQ_START_INIT,
    SEQ_WAIT_INIT,

    SEQ_INIT,
    SEQ_MAIN,

    SEQ_START_NUMINPUT,
    SEQ_WAIT_NUMINPUT,
  
    SEQ_START_RECVDATA,
    SEQ_WAIT_RECVDATA,
    SEQ_START_RECVTIME,
    SEQ_WAIT_RECVTIME,

    SEQ_START_SENDDATA,
    SEQ_WAIT_SENDDATA,
    SEQ_START_SENDTIME,
    SEQ_WAIT_SENDTIME,

    SEQ_WAIT_DISCONNECT,
    SEQ_EXIT,
  };
  switch( p_wk->seq )
  { 
  case SEQ_START_INIT:
    { 
      const u16 str[] = L"���������s���Ă��܂��B";
      WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
    }
    WIFIBATTLEMATCH_NET_StartInitialize( p_wk->p_net );
    p_wk->seq = SEQ_WAIT_INIT;
    break;

  case SEQ_WAIT_INIT:
    { 
      if( WIFIBATTLEMATCH_NET_WaitInitialize( p_wk->p_net, p_wk->p_save )  )
      { 
        p_wk->seq = SEQ_INIT;
      }
    }
    break;

  case SEQ_INIT:
    { 
      const u16 str[] = L"A�Ŏ�M�AB�ő��M�@�^�b�`�Œl�ύX";
      WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
    }
    Sake_UpdateDisplay( p_wk );
    p_wk->seq = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    {
      int i;
      u32 x, y;
      if( GFL_UI_TP_GetPointTrg( &x, &y) )
      { 
        for( i = 0; i < DEBUG_BMPWIN_MAX; i++ )
        {
          if( p_wk->p_bmpwin[i] )
          { 
            if( BMPWINIsCollisionRxP( p_wk->p_bmpwin[i], x, y ) )
            {
              p_wk->select  = i;
              p_wk->seq     = SEQ_START_NUMINPUT;
              break;
            }
          }
        }
      }
    }

    //���M
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    { 
      p_wk->seq = SEQ_START_SENDDATA;
    }
    //��M
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    { 
      p_wk->seq = SEQ_START_RECVDATA;
    }
    break;

  case SEQ_START_NUMINPUT:
    GFL_BG_SetVisible( BG_FRAME_S_NUM, TRUE );
    { 
      u8 x, y;
      x = GFL_BMPWIN_GetPosX( p_wk->p_bmpwin[p_wk->select] );
      y = GFL_BMPWIN_GetPosY( p_wk->p_bmpwin[p_wk->select] );
      NUMINPUT_Start( &p_wk->numinput, 4, 0, 9999, p_wk->data[ p_wk->select ], x+10, y );
    }
    p_wk->seq = SEQ_WAIT_NUMINPUT;
    break;

  case SEQ_WAIT_NUMINPUT:
    if( NUMINPUT_Main( &p_wk->numinput ) )
    { 
      p_wk->data[ p_wk->select ] = NUMINPUT_GetNum( &p_wk->numinput );
      Sake_UpdateDisplay( p_wk );
      GFL_BG_SetVisible( BG_FRAME_S_NUM, FALSE );
      p_wk->seq = SEQ_MAIN;
    }
    break;

  case SEQ_START_RECVDATA:
    { 
      const u16 str[] = L"SAKE����f�[�^���擾��";
      WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
    }
    WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_MYRECORD, WIFIBATTLEMATCH_GDB_GET_DEBUGALL, &p_wk->score );
    p_wk->seq = SEQ_WAIT_RECVDATA;
    break;

  case SEQ_WAIT_RECVDATA:
    { 
      if( WIFIBATTLEMATCH_GDB_Process( p_wk->p_net ) )
      { 
        p_wk->data[0] = p_wk->score.single_win;
        p_wk->data[1] = p_wk->score.single_lose;
        p_wk->data[2] = p_wk->score.single_rate;
        p_wk->data[3] = p_wk->score.double_win;
        p_wk->data[4] = p_wk->score.double_lose;
        p_wk->data[5] = p_wk->score.double_rate;
        p_wk->data[6] = p_wk->score.rot_win;
        p_wk->data[7] = p_wk->score.rot_lose;
        p_wk->data[8] = p_wk->score.rot_rate;
        p_wk->data[9] = p_wk->score.triple_win;
        p_wk->data[10] = p_wk->score.triple_lose;
        p_wk->data[11] = p_wk->score.triple_rate;
        p_wk->data[12] = p_wk->score.shooter_win;
        p_wk->data[13] = p_wk->score.shooter_lose;
        p_wk->data[14] = p_wk->score.shooter_rate;
        p_wk->data[15] = p_wk->score.disconnect;
        p_wk->data[16] = p_wk->score.cheat;
        p_wk->seq = SEQ_START_RECVTIME;
      }
    }
    break;
  
  case SEQ_START_RECVTIME:
    WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_MYRECORD, WIFIBATTLEMATCH_GDB_GET_LOGIN_DATE, &p_wk->logindate );
    p_wk->seq = SEQ_WAIT_RECVTIME;
    break;

  case SEQ_WAIT_RECVTIME:
    { 
      if( WIFIBATTLEMATCH_GDB_Process( p_wk->p_net ) )
      { 
        OS_TPrintf( "���m�ȃ��O�C������ =[%d]\n", p_wk->logindate );
        //����32bit
        p_wk->data[17]  = p_wk->logindate & 0xFFFFFFFF;
        p_wk->seq = SEQ_INIT;
      }
    }
    break;

  case SEQ_START_SENDDATA:
    { 
        p_wk->score.single_win  =p_wk->data[0];
        p_wk->score.single_lose =p_wk->data[1];
        p_wk->score.single_rate =p_wk->data[2];
        p_wk->score.double_win  =p_wk->data[3];
        p_wk->score.double_lose =p_wk->data[4];
        p_wk->score.double_rate =p_wk->data[5];
        p_wk->score.rot_win     =p_wk->data[6];
        p_wk->score.rot_lose    =p_wk->data[7];
        p_wk->score.rot_rate    =p_wk->data[8];
        p_wk->score.triple_win  =p_wk->data[9];
        p_wk->score.triple_lose =p_wk->data[10];
        p_wk->score.triple_rate =p_wk->data[11];
        p_wk->score.shooter_win =p_wk->data[12];
        p_wk->score.shooter_lose  =p_wk->data[13];
        p_wk->score.shooter_rate  =p_wk->data[14];
        p_wk->score.disconnect  =p_wk->data[15];
        p_wk->score.cheat       =p_wk->data[16];
    }
    { 
      const u16 str[] = L"�f�[�^���M��";
      WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
    }
    WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_DEBUGALL, &p_wk->score );
    p_wk->seq = SEQ_WAIT_SENDDATA;
    break;

  case SEQ_WAIT_SENDDATA:
    { 
      if( WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net )  )
      { 
        p_wk->seq = SEQ_START_SENDTIME;
      }
    }
    break;
    
  case SEQ_START_SENDTIME:
    WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_LOGIN_DATE, NULL );
    p_wk->seq = SEQ_WAIT_SENDTIME;
    break;

  case SEQ_WAIT_SENDTIME:
    { 
      if( WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net )  )
      { 
        { 
          const u16 str[] = L"�f�[�^���M����";
          WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
        }
        p_wk->seq = SEQ_INIT;
      }
    }
    break;

  case SEQ_WAIT_DISCONNECT:
    if( WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, FALSE ) )
    { 
      p_wk->seq = SEQ_EXIT;
    }
    break;

  case SEQ_EXIT:
    return TRUE;
  }

  WIFIBATTLEMATCH_NET_Main( p_wk->p_net );
  WBM_TEXT_Main( p_wk->p_text );
  PRINTSYS_QUE_Main( p_wk->p_que );

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  SAKE�p��ʍ쐬
 *
 *	@param	DEBUG_SAKE_WORK *p_wk ���[�N
 *	@param	heapID                �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Sake_CreateDisplay( DEBUG_SAKE_WORK *p_wk, HEAPID heapID )
{ 
  int i;
  INPUTWIN_PARAM  param;

  for( i = 0; i < NELEMS(sc_stat_name); i++ )
  { 

    INPUTWIN_GetRange( i, &param );

    PRINT_GetStrWithNumber( p_wk->p_strbuf, sc_stat_name[i], p_wk->data[i] );

    p_wk->p_bmpwin[i]  = GFL_BMPWIN_Create( BG_FRAME_S_FONT, 
        param.x, param.y, param.w, param.h, 
        PLT_FONT_S, GFL_BMP_CHRAREA_GET_B );


    PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0, 0, p_wk->p_strbuf, p_wk->p_small_font );

    GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  SAKE�p��ʔj��
 *
 *	@param	DEBUG_SAKE_WORK *p_wk ���[�N
 *	@param	heapID                �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Sake_DeleteDisplay( DEBUG_SAKE_WORK *p_wk )
{ 
  int i;
  for( i = 0; i < NELEMS(sc_stat_name); i++ )
  {
    if( p_wk->p_bmpwin[i] )
    { 
      GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
      p_wk->p_bmpwin[i] = NULL;
    }
  }

  GFL_BG_ClearScreen( BG_FRAME_S_FONT );
}
//----------------------------------------------------------------------------
/**
 *	@brief  SAKE�p��ʃA�b�v�f�[�g
 *
 *	@param	DEBUG_ATLAS_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Sake_UpdateDisplay( DEBUG_SAKE_WORK *p_wk )
{ 
  int i;
  INPUTWIN_PARAM  param;

  for( i = 0; i < NELEMS(sc_stat_name); i++ )
  { 

    INPUTWIN_GetRange( i, &param );

    PRINT_GetStrWithNumber( p_wk->p_strbuf, sc_stat_name[i], p_wk->data[i] );

    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0 );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0, 0, p_wk->p_strbuf, p_wk->p_small_font );

    GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );
  }
}

//=============================================================================
/**
 *    ATLAS
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���X�f�o�b�O  ������
 *
 *	@param	DEBUG_ATLAS_WORK *p_wk    ���[�N
 *	@param	heapID                    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void ATLAS_Init( DEBUG_ATLAS_WORK *p_wk, GAMEDATA *p_gamedata, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(DEBUG_ATLAS_WORK) );
  p_wk->heapID    = heapID;
  p_wk->p_save    = GAMEDATA_GetSaveControlWork( p_gamedata );
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
	p_wk->p_small_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_small_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
  p_wk->p_strbuf  = GFL_STR_CreateBuffer( 255, heapID );
  p_wk->p_que     = PRINTSYS_QUE_Create( heapID );
  p_wk->p_text  = WBM_TEXT_Init( BG_FRAME_M_FONT, PLT_FONT_M, 0, 0, p_wk->p_que, p_wk->p_font, heapID );
  Atlas_CreateReportDisplay( p_wk, heapID );
  p_wk->p_net = WIFIBATTLEMATCH_NET_Init( 0, p_gamedata, NULL, heapID );

  NUMINPUT_Init( &p_wk->numinput, BG_FRAME_S_NUM, PLT_FONT_S, p_wk->p_small_font, heapID );
  GFL_BG_SetVisible( BG_FRAME_S_NUM, FALSE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�h���X�f�o�b�O  �j��
 *
 *	@param	DEBUG_ATLAS_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void ATLAS_Exit( DEBUG_ATLAS_WORK *p_wk )
{ 
  NUMINPUT_Exit( &p_wk->numinput );

  GFL_STR_DeleteBuffer( p_wk->p_strbuf );
  WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
  Atlas_DeleteRecvDisplay( p_wk );
  WBM_TEXT_Main( p_wk->p_text );
  PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_small_font );
	GFL_FONT_Delete( p_wk->p_font );
  GFL_STD_MemClear( p_wk, sizeof(DEBUG_ATLAS_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���X�f�o�b�O  ���C������
 *
 *	@param	DEBUG_ATLAS_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static BOOL ATLAS_Main( DEBUG_ATLAS_WORK *p_wk )
{ 
  enum
  { 
    SEQ_START_MATCH,
    SEQ_WAIT_MATCH,

    SEQ_START_INIT,
    SEQ_WAIT_INIT,

    SEQ_SENDMAIN,
    SEQ_RECVMAIN,

    SEQ_START_NUMINPUT,
    SEQ_WAIT_NUMINPUT,
  
    SEQ_START_RECVDATA,
    SEQ_WAIT_RECVDATA,

    SEQ_START_SENDDATA,
    SEQ_WAIT_SENDDATA,

    SEQ_WAIT_DISCONNECT,
    SEQ_EXIT,
  };

  switch( p_wk->seq )
  { 
  case SEQ_START_MATCH:
    { 
      const u16 str[] = L"�f�o�b�O����ƃ}�b�`���O���ĉ������B";
      WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
    }
    WIFIBATTLEMATCH_NET_StartMatchMakeDebug( p_wk->p_net );
    p_wk->seq = SEQ_WAIT_MATCH;
    break;

  case SEQ_WAIT_MATCH:
    if( WIFIBATTLEMATCH_NET_WaitMatchMake( p_wk->p_net ) )
    { 
      p_wk->seq = SEQ_START_INIT;
    }
    break;

  case SEQ_START_INIT:
    { 
      const u16 str[] = L"���������s���Ă��܂��B";
      WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
    }
    WIFIBATTLEMATCH_NET_StartInitialize( p_wk->p_net );
    p_wk->seq = SEQ_WAIT_INIT;
    break;

  case SEQ_WAIT_INIT:
    { 
      if( WIFIBATTLEMATCH_NET_WaitInitialize( p_wk->p_net, p_wk->p_save )  )
      { 
        { 
          const u16 str[] = L"���M�����BA�ő��M�AX�Ń��[�h�A�^�b�`�Œl";
          WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
        }
        p_wk->seq = SEQ_SENDMAIN;
      }
    }
    break;

  case SEQ_SENDMAIN:
    {
      int i;
      u32 x, y;
      if( GFL_UI_TP_GetPointTrg( &x, &y) )
      { 
        for( i = 0; i < DEBUG_BMPWIN_MAX; i++ )
        {
          if( p_wk->p_bmpwin[i] )
          { 
            if( BMPWINIsCollisionRxP( p_wk->p_bmpwin[i], x, y ) )
            {
              if( i == 17 )
              { 
                p_wk->is_auth ^= 1;
                Atlas_UpdateReportDisplay( p_wk );
              }
              else
              { 
                p_wk->select  = i;
                p_wk->pre_seq = SEQ_SENDMAIN;
                p_wk->seq     = SEQ_START_NUMINPUT;
              }
              break;
            }
          }
        }
      }
    }

    //���M
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    { 
      p_wk->seq = SEQ_START_SENDDATA;
    }

    //���؂�ւ�
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
    { 
      p_wk->is_you  ^=  1;
      { 
        if( !p_wk->is_you )
        { 
          const u16 str[] = L"���M�����BA�ő��M�AX�Ń��[�h�A�^�b�`�Œl";
          WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
        }
        else
        { 
          const u16 str[] = L"���M����BA�ő��M�AX�Ń��[�h�A�^�b�`�Œl";
          WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
        }
        Atlas_UpdateReportDisplay( p_wk );
      }
    }

    //�؂�ւ�
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    { 
      { 
        const u16 str[] = L"��M���[�h�BA�Ŏ�M�AX�Ń��[�h�ؑ�";
        WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
      }
      Atlas_DeleteReportDisplay( p_wk );
      Atlas_CreateRecvDisplay( p_wk, p_wk->heapID );
      p_wk->seq = SEQ_RECVMAIN;
    }
    break;

  case SEQ_RECVMAIN:
    //��M
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    { 
      p_wk->seq = SEQ_START_RECVDATA;
    }

    //�؂�ւ�
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    { 
      { 
        const u16 str[] = L"���M���[�h�BA�ő��M�AX�Ń��[�h�ؑցA�^�b�`�Œl�ύX";
        WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
      }
      Atlas_DeleteRecvDisplay( p_wk );
      Atlas_CreateReportDisplay( p_wk, p_wk->heapID );
      p_wk->seq = SEQ_SENDMAIN;
    }
    break;

  case SEQ_START_NUMINPUT:
    GFL_BG_SetVisible( BG_FRAME_S_NUM, TRUE );
    { 
      u32 data;
      u32 max;
      u8 x, y;
      if( p_wk->pre_seq == SEQ_RECVMAIN )
      { 
        data  = p_wk->recv_data[ p_wk->select ];
      }
      else if( p_wk->pre_seq == SEQ_SENDMAIN )
      { 
        data  = p_wk->send_data[p_wk->is_you][ p_wk->select ];
      }
      x = GFL_BMPWIN_GetPosX( p_wk->p_bmpwin[p_wk->select] );
      y = GFL_BMPWIN_GetPosY( p_wk->p_bmpwin[p_wk->select] );
      NUMINPUT_Start( &p_wk->numinput, 4, 0, 1, data, x+10, y );
    }
    p_wk->seq = SEQ_WAIT_NUMINPUT;
    break;

  case SEQ_WAIT_NUMINPUT:
    if( NUMINPUT_Main( &p_wk->numinput ) )
    { 
      if( p_wk->pre_seq == SEQ_RECVMAIN )
      { 
        p_wk->recv_data[ p_wk->select ] = NUMINPUT_GetNum( &p_wk->numinput );
        Atlas_UpdateRecvDisplay( p_wk );
      }
      else if( p_wk->pre_seq == SEQ_SENDMAIN )
      { 
        p_wk->send_data[p_wk->is_you][ p_wk->select ] = NUMINPUT_GetNum( &p_wk->numinput );
        Atlas_UpdateReportDisplay( p_wk );
      }
      GFL_BG_SetVisible( BG_FRAME_S_NUM, FALSE );
      p_wk->seq = p_wk->pre_seq;
    }
    break;

  case SEQ_START_RECVDATA:
    { 
      const u16 str[] = L"SAKE����f�[�^���擾��";
      WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
    }
    WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_MYRECORD, WIFIBATTLEMATCH_GDB_GET_DEBUGALL, &p_wk->score );
    p_wk->seq = SEQ_WAIT_RECVDATA;
    break;

  case SEQ_WAIT_RECVDATA:
    { 
      if( WIFIBATTLEMATCH_GDB_Process( p_wk->p_net ) )
      { 
        { 
          const u16 str[] = L"�f�[�^�擾����";
          WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
        }

        p_wk->recv_data[0] = p_wk->score.single_win;
        p_wk->recv_data[1] = p_wk->score.single_lose;
        p_wk->recv_data[2] = p_wk->score.single_rate;
        p_wk->recv_data[3] = p_wk->score.double_win;
        p_wk->recv_data[4] = p_wk->score.double_lose;
        p_wk->recv_data[5] = p_wk->score.double_rate;
        p_wk->recv_data[6] = p_wk->score.rot_win;
        p_wk->recv_data[7] = p_wk->score.rot_lose;
        p_wk->recv_data[8] = p_wk->score.rot_rate;
        p_wk->recv_data[9] = p_wk->score.triple_win;
        p_wk->recv_data[10] = p_wk->score.triple_lose;
        p_wk->recv_data[11] = p_wk->score.triple_rate;
        p_wk->recv_data[12] = p_wk->score.shooter_win;
        p_wk->recv_data[13] = p_wk->score.shooter_lose;
        p_wk->recv_data[14] = p_wk->score.shooter_rate;
        p_wk->recv_data[15] = p_wk->score.disconnect;
        p_wk->recv_data[16] = p_wk->score.cheat;

        Atlas_UpdateRecvDisplay( p_wk );

        p_wk->seq = SEQ_RECVMAIN;
      }
    }
    break;

  case SEQ_START_SENDDATA:
    { 
      p_wk->report.my_result      = p_wk->send_data[0][0];
      p_wk->report.my_single_win  = p_wk->send_data[0][1];
      p_wk->report.my_single_lose = p_wk->send_data[0][2];
      p_wk->report.my_single_rate = p_wk->send_data[0][3];
      p_wk->report.my_double_win  = p_wk->send_data[0][4];
      p_wk->report.my_double_lose = p_wk->send_data[0][5];
      p_wk->report.my_double_rate = p_wk->send_data[0][6];
      p_wk->report.my_triple_win  = p_wk->send_data[0][7];
      p_wk->report.my_triple_lose = p_wk->send_data[0][8];
      p_wk->report.my_triple_rate = p_wk->send_data[0][9];
      p_wk->report.my_rot_win     = p_wk->send_data[0][10];
      p_wk->report.my_rot_lose    = p_wk->send_data[0][11];
      p_wk->report.my_rot_rate    = p_wk->send_data[0][12];
      p_wk->report.my_shooter_win  = p_wk->send_data[0][13];
      p_wk->report.my_shooter_lose = p_wk->send_data[0][14];
      p_wk->report.my_shooter_rate = p_wk->send_data[0][15];
      p_wk->report.my_cheat       = p_wk->send_data[0][16];
      p_wk->report.you_result      = p_wk->send_data[1][0];
      p_wk->report.you_single_win  = p_wk->send_data[1][1];
      p_wk->report.you_single_lose = p_wk->send_data[1][2];
      p_wk->report.you_single_rate = p_wk->send_data[1][3];
      p_wk->report.you_double_win  = p_wk->send_data[1][4];
      p_wk->report.you_double_lose = p_wk->send_data[1][5];
      p_wk->report.you_double_rate = p_wk->send_data[1][6];
      p_wk->report.you_triple_win  = p_wk->send_data[1][7];
      p_wk->report.you_triple_lose = p_wk->send_data[1][8];
      p_wk->report.you_triple_rate = p_wk->send_data[1][9];
      p_wk->report.you_rot_win     = p_wk->send_data[1][10];
      p_wk->report.you_rot_lose    = p_wk->send_data[1][11];
      p_wk->report.you_rot_rate    = p_wk->send_data[1][12];
      p_wk->report.you_shooter_win  = p_wk->send_data[1][13];
      p_wk->report.you_shooter_lose = p_wk->send_data[1][14];
      p_wk->report.you_shooter_rate = p_wk->send_data[1][15];
      p_wk->report.you_cheat       = p_wk->send_data[1][16];
    }
    { 
      const u16 str[] = L"���|�[�g���M��";
      WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
    }
    WIFIBATTLEMATCH_SC_StartDebug( p_wk->p_net, &p_wk->report, p_wk->is_auth );
    p_wk->seq = SEQ_WAIT_SENDDATA;
    break;

  case SEQ_WAIT_SENDDATA:
    { 
      if( WIFIBATTLEMATCH_SC_Process( p_wk->p_net )  )
      { 
        { 
          const u16 str[] = L"���|�[�g���M����";
          WBM_TEXT_PrintDebug( p_wk->p_text, str, NELEMS(str), p_wk->p_font );
        }
        p_wk->seq = SEQ_SENDMAIN;
      }
    }
    break;

  case SEQ_WAIT_DISCONNECT:
    if( WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, FALSE ) )
    { 
      p_wk->seq = SEQ_EXIT;
    }
    break;

  case SEQ_EXIT:
    return TRUE;
  }

  WIFIBATTLEMATCH_NET_Main( p_wk->p_net );
  WBM_TEXT_Main( p_wk->p_text );
  PRINTSYS_QUE_Main( p_wk->p_que );

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���X�p��ʕ\���쐬
 *
 *	@param	DEBUG_ATLAS_WORK *p_wk  ���[�N
 *	@param	heapID                  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Atlas_CreateRecvDisplay( DEBUG_ATLAS_WORK *p_wk, HEAPID heapID )
{ 
  int i;
  INPUTWIN_PARAM  param;

  for( i = 0; i < NELEMS(sc_stat_name); i++ )
  { 

    INPUTWIN_GetRange( i, &param );

    PRINT_GetStrWithNumber( p_wk->p_strbuf, sc_stat_name[i], p_wk->recv_data[i] );

    p_wk->p_bmpwin[i]  = GFL_BMPWIN_Create( BG_FRAME_S_FONT, 
        param.x, param.y, param.w, param.h, 
        PLT_FONT_S, GFL_BMP_CHRAREA_GET_B );


    PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0, 0, p_wk->p_strbuf, p_wk->p_font );

    GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���X�p��ʕ\���j��
 *
 *	@param	DEBUG_ATLAS_WORK *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void Atlas_DeleteRecvDisplay( DEBUG_ATLAS_WORK *p_wk )
{ 
  int i;
  for( i = 0; i < NELEMS(sc_stat_name); i++ )
  {
    if( p_wk->p_bmpwin[i] )
    { 
      GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
      p_wk->p_bmpwin[i] = NULL;
    }
  }

  GFL_BG_ClearScreen( BG_FRAME_S_FONT );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���X�p��ʃA�b�v�f�[�g
 *
 *	@param	DEBUG_ATLAS_WORK *p_wk  ���[�N
 *	@param  
 *
 */
//-----------------------------------------------------------------------------
static void Atlas_UpdateRecvDisplay( DEBUG_ATLAS_WORK *p_wk )
{ 
  int i;

  for( i = 0; i < NELEMS(sc_stat_name); i++ )
  { 
    
    PRINT_GetStrWithNumber( p_wk->p_strbuf, sc_stat_name[i], p_wk->recv_data[i] );

    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0 );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0, 0, p_wk->p_strbuf, p_wk->p_font );

    GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���X�p���|�[�g��ʍ쐬
 *
 *	@param	DEBUG_ATLAS_WORK *p_wk  ���[�N
 *	@param	heapID                  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Atlas_CreateReportDisplay( DEBUG_ATLAS_WORK *p_wk, HEAPID heapID )
{ 
  int i;
  INPUTWIN_PARAM  param;

  for( i = 0; i < NELEMS(sc_key_name)-1; i++ )
  { 
    INPUTWIN_GetRange( i, &param );

    PRINT_GetStrWithNumber( p_wk->p_strbuf, sc_key_name[i], p_wk->send_data[p_wk->is_you][i] );

    p_wk->p_bmpwin[i]  = GFL_BMPWIN_Create( BG_FRAME_S_FONT, 
        param.x, param.y, param.w, param.h, 
        PLT_FONT_S, GFL_BMP_CHRAREA_GET_B );


    PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0, 0, p_wk->p_strbuf, p_wk->p_small_font );

    GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );
  }
  
  INPUTWIN_GetRange( i, &param );
  PRINT_GetStrWithNumber( p_wk->p_strbuf, sc_key_name[i], p_wk->is_auth );
  p_wk->p_bmpwin[i]  = GFL_BMPWIN_Create( BG_FRAME_S_FONT, 
        param.x, param.y, param.w, param.h, 
        PLT_FONT_S, GFL_BMP_CHRAREA_GET_B );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0, 0, p_wk->p_strbuf, p_wk->p_small_font );
  GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );

}
//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���X�p���|�[�g��ʍ쐬
 *
 *	@param	DEBUG_ATLAS_WORK *p_wk  ���[�N
 *	@param	heapID                  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Atlas_DeleteReportDisplay( DEBUG_ATLAS_WORK *p_wk )
{ 
  int i;
  for( i = 0; i < NELEMS(sc_key_name); i++ )
  {
    if( p_wk->p_bmpwin[i] )
    { 
      GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
      p_wk->p_bmpwin[i] = NULL;
    }
  }
  GFL_BG_ClearScreen( BG_FRAME_S_FONT );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �A�g���X�p���|�[�g��ʍ쐬
 *
 *	@param	DEBUG_ATLAS_WORK *p_wk  ���[�N
 *	@param	heapID                  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Atlas_UpdateReportDisplay( DEBUG_ATLAS_WORK *p_wk )
{ 
  int i;

  for( i = 0; i < NELEMS(sc_key_name) - 1; i++ )
  { 
    PRINT_GetStrWithNumber( p_wk->p_strbuf, sc_key_name[i], p_wk->send_data[p_wk->is_you][i] );

    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0 );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0, 0, p_wk->p_strbuf, p_wk->p_small_font );

    GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin[i] );
  }

  PRINT_GetStrWithNumber( p_wk->p_strbuf, sc_key_name[i], p_wk->is_auth );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0, 0, p_wk->p_strbuf, p_wk->p_small_font );
  GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );

}

//=============================================================================
/**
 *  NUMINPUT
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���l����  ������
 *
 *	@param	NUMINPUT_WORK *p_wk ���[�N  
 *	@param	frm                 �쐬�t���[��
 *	@param	x                   X
 *	@param	y                   Y
 *	@param	w                   ��
 *	@param	h                   ����
 *	@param	plt                 �p���b�g
 *	@param	*p_font             �t�H���g
 *	@param	heapID              �q�[�vID
 */
//-----------------------------------------------------------------------------
static void NUMINPUT_Init( NUMINPUT_WORK *p_wk, u8 frm, u8 plt, GFL_FONT *p_font, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(NUMINPUT_WORK) );
  p_wk->p_font  = p_font;
  p_wk->frm     = frm;
  p_wk->plt     = plt;
  p_wk->p_strbuf  = GFL_STR_CreateBuffer( 255, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���l����  �j��
 *
 *	@param	NUMINPUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void NUMINPUT_Exit( NUMINPUT_WORK *p_wk )
{ 
  GFL_STR_DeleteBuffer( p_wk->p_strbuf );
  GFL_STD_MemClear( p_wk, sizeof(NUMINPUT_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���l����  �J�n
 *
 *	@param	NUMINPUT_WORK *p_wk ���[�N
 *	@param	keta                ��
 *	@param	min                 �ŏ�
 *	@param	max                 �ő�
 *	@param  now                 �����l
 */
//-----------------------------------------------------------------------------
static void NUMINPUT_Start( NUMINPUT_WORK *p_wk, u8 keta, s32 min, s32 max, s32 now, u8 x, u8 y )
{ 
  if( p_wk->p_bmpwin == NULL )
  { 

    p_wk->keta  = keta;
    p_wk->min   = min;
    p_wk->max   = max;
    p_wk->now   = now;

    p_wk->p_bmpwin  = GFL_BMPWIN_Create( p_wk->frm, x, y, 6, 2, p_wk->plt, GFL_BMP_CHRAREA_GET_B );

    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 15 );
    NumInput_Print( p_wk );
    GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���l����  �I��
 *
 *	@param	NUMINPUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static BOOL NUMINPUT_Main( NUMINPUT_WORK *p_wk )
{ 
  BOOL  is_update = FALSE;

  if( p_wk->p_bmpwin == NULL)
  { 
    return TRUE;
  }

  //���E�ŕύX���鐔����؂�ւ���
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
  { 
    p_wk->active++;
    p_wk->active  %= p_wk->keta;

    is_update = TRUE;
  }
  else if( GFL_UI_KEY_GetTrg( ) & PAD_KEY_RIGHT )
  { 
    if( p_wk->active == 0 )
    { 
      p_wk->active  = p_wk->keta  - 1 ;
    }
    else
    { 
      p_wk->active--;
    }
    is_update = TRUE;
  }

  //�㉺�Ő�����؂�ւ���
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
  { 
    p_wk->now = NumInput_AddKeta( p_wk->now, p_wk->active, 1 );
    p_wk->now = MATH_CLAMP( p_wk->now, p_wk->min, p_wk->max );
    is_update = TRUE;
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
  { 
    p_wk->now = NumInput_AddKeta( p_wk->now, p_wk->active, -1 );
    p_wk->now = MATH_CLAMP( p_wk->now, p_wk->min, p_wk->max );
    is_update = TRUE;
  }

  //����
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  { 
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );
    GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
    GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin );
    GFL_BMPWIN_Delete( p_wk->p_bmpwin );
    p_wk->p_bmpwin  = NULL;

    return TRUE;
  }

  //�`��
  if( is_update )
  { 
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 15 );
    NumInput_Print( p_wk );
    GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���͂������l�擾
 *
 *	@param	const NUMINPUT_WORK *cp_wk ���[�N
 *
 *	@return ���l
 */
//-----------------------------------------------------------------------------
static u16  NUMINPUT_GetNum( const NUMINPUT_WORK *cp_wk )
{ 
  return cp_wk->now;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���l����  �\��
 *
 *	@param	NUMINPUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void NumInput_Print( NUMINPUT_WORK *p_wk )
{ 

  int i;
  PRINTSYS_LSB color;
  u8 fig;

  //�`��
  for( i = 0; i < p_wk->keta; i++ )
  { 
    fig =  GetFig( p_wk->now, p_wk->keta - i - 1 );

    PRINT_GetStrWithNumber( p_wk->p_strbuf, "%d", fig );

    if( p_wk->active  == p_wk->keta - i - 1 )
    { 
      color = PRINTSYS_LSB_Make( 3,4,15 );
    }
    else
    { 
      color = PRINTSYS_LSB_Make( 1,2,15 );
    }
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), i * 12, 0, p_wk->p_strbuf, p_wk->p_font, color );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �C�ӂ̌��ɂP�����Z����
 *
 *	@param	u32 now ���݂̒l
 *	@param	keta    ��
 *	@param	dir     +�Ȃ�Ή��Z -�Ȃ�Ό��Z
 *
 *	@return �C����̐��l
 */
//-----------------------------------------------------------------------------
static u32 NumInput_AddKeta( u32 now, u32 keta, s8 dir )
{
  u32 add   = 1;
  while( keta-- )
  { 
    add *= 10;
  }
    
  now +=  dir*add;
  return now;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����ɐ��l�������
 *
 *	@param	STRBUF *p_dst   �󂯎��
 *	@param	u16 *cp_src     ������
 */
//-----------------------------------------------------------------------------
static void PRINT_GetStrWithNumber( STRBUF *p_dst, const char *cp_src, ... )
{ 
  char str[128];
  STRCODE str_code[128];
  u16 strlen;
  va_list vlist;

  //���l������
  va_start(vlist, cp_src);
  STD_TVSNPrintf( str, 128, cp_src, vlist );
  va_end(vlist);

  //STRBUF�p�ɕϊ�
  DEB_STR_CONV_SjisToStrcode( str, str_code, 128 );

  strlen  = GFL_STD_StrLen(str)*2;
  str_code[strlen] = GFL_STR_GetEOMCode();

  //STRBUF�ɓ]��
  GFL_STR_SetStringCode( p_dst, str_code);
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPWIN�����蔻��
 *
 *	@param	const GFL_BMPWIN *cp_bmpwin
 *	@param	x �_X
 *	@param	y �_Y
 *
 *	@return TRUE�ŏՓ�  FALSE�ŏՓ˂��Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL BMPWINIsCollisionRxP( const GFL_BMPWIN *cp_bmpwin, u32 x, u32 y )
{
	s16	left, right, top, bottom;

	left	  = ( GFL_BMPWIN_GetPosX( cp_bmpwin ) )*8;
	right	  = ( GFL_BMPWIN_GetPosX( cp_bmpwin ) + GFL_BMPWIN_GetSizeX( cp_bmpwin ) )*8;
	top		  = ( GFL_BMPWIN_GetPosY( cp_bmpwin ) )*8;
	bottom  = ( GFL_BMPWIN_GetPosY( cp_bmpwin ) + GFL_BMPWIN_GetSizeY( cp_bmpwin ) )*8;

	return ((s32)( x - left) <= (s32)(right - left))
		&	((s32)( y - top) <= (s32)(bottom - top));
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���Ԗڂ̐��l���擾
 *
 *	@param	u32 now ����
 *	@param	keta    ��
 *
 *	@return keta�Ԗڂ̐��l
 */
//-----------------------------------------------------------------------------
static u8 GetFig( u32 now, u8 keta )
{ 
  static const u32 sc_keta_tbl[]  =
  { 
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
  };

  GF_ASSERT( keta < NELEMS(sc_keta_tbl) );
  return (now % sc_keta_tbl[ keta + 1 ]) / sc_keta_tbl[ keta ] ;
}

